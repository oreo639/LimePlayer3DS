/***************************************************************************
 * fcurl
 *
 * Copyright (C) 2016, Daniel Stenberg, <daniel@haxx.se>
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>
#include <curl/curl.h>

#include "handle.h"

#define MIN(x, y) ((x)<(y)?(x):(y))

/* called on data arrival */
static size_t write_callback(char *buffer, size_t size, size_t nitems,
                             void *userp)
{
  struct fcurl_handle *h = (struct fcurl_handle *)userp;
  size_t n_remaining;
  size_t n_remainuser;
  size_t osize;

  size *= nitems;

  osize = size; /* keep the original input size for returning */

  n_remaining = h->b.size - h->b.used; /* remaining space in private buffer */
  n_remainuser = h->user.size - h->user.used; /* remaining space in user buffer */

  if(n_remainuser) {
    size_t n_copytouser = MIN(size, n_remainuser);

    /* space left in the user buffer, copy as much as possible to there */
    memcpy(h->user.p, buffer, n_copytouser);

    buffer += n_copytouser;
    size -= n_copytouser;

    h->user.used += n_copytouser;
  }

  if(!size)
    /* all data is taken care of */
    return osize;

  if(size > n_remaining) {
    char *newp;
    size_t n_missing = (size - n_remaining);

    /* we will have reaons to go back and reconsider buffer growth algorithms
       later */
    size_t newsize = h->b.size + n_missing + BUFFERGROWTHMARGIN;

    newp = realloc(h->b.p, newsize);
    if(!newp) {
      /* use fprintf() during intial debugging */
      fprintf(stderr, "out of memory!\n");
      return 0; /* makes the transfer fail */
    }
    else {
      h->b.size = newsize;
      h->b.p = newp;
    }
  }

  /* copy data from libcurl into our handle specific buffer */
  memcpy(&h->b.p[h->b.used], buffer, size);
  h->b.used += size;

  return osize;
}

/*
 * transfer data, copy no more than 'max' bytes into the 'target' buffer
 */
static int transfer(struct fcurl_handle *h, const void *target, size_t max)
{
  CURLMcode mc;
  int numfds;

  h->user.p = (char *)target;
  h->user.used = 0;
  h->user.size = max;

  if(h->b.used) {
    /* there is data left from the previous invoke */
    size_t n_copytouser = MIN(h->b.used, max);
    memcpy((char *)target, h->b.p, n_copytouser);
    h->user.used += n_copytouser;

    /* slide the remaining buffer to the beginning */
    memmove(h->b.p, h->b.p+n_copytouser, h->b.used - n_copytouser);
    h->b.used -= n_copytouser;

    if(h->user.used == max)
      return 0;
  }
  else if(h->transfer_complete)
    return 1; /* no buffer data left and transfer complete == done */

  if(!h->transfer_complete) {
    do {
      mc = curl_multi_wait(h->mh, NULL, 0, 5000, &numfds);
      if(mc == CURLM_OK) {
        int left;
        struct CURLMsg *m;
        mc = curl_multi_perform(h->mh, &left);
        if(mc != CURLM_OK)
          return 1;

        m = curl_multi_info_read(h->mh, &left);
        if(m && (m->msg == CURLMSG_DONE)) {
          h->transfer_complete = true;
          h->transfer_rc = m->data.result;
          break;
        }
      }
    } while(!h->user.used);
  }

  return 0; /* all is well */
}

/*
 * fopen() replacement
 *
 *   FILE *fopen(const char *path, const char *mode);
 */

struct fcurl_handle *fcurl_open(const char *url, const char *operation)
{
  struct fcurl_handle *h;
  int still_running;
  CURLMcode mc;
  (void)operation; /* assume reading for now */

  h = malloc(sizeof(struct fcurl_handle));
  if(!h)
    return NULL;

  memset(h, 0, sizeof(struct fcurl_handle));

  h->curl = curl_easy_init();
  if(!h->curl)
    goto fail;

  curl_easy_setopt(h->curl, CURLOPT_URL, url);
  curl_easy_setopt(h->curl, CURLOPT_VERBOSE, 1L);
  curl_easy_setopt(h->curl, CURLOPT_WRITEDATA, h);
  curl_easy_setopt(h->curl, CURLOPT_WRITEFUNCTION, write_callback);

  h->mh = curl_multi_init();
  if(!h->mh)
    goto fail;

  mc = curl_multi_add_handle(h->mh, h->curl);
  if(mc)
    goto fail;

  return h;

  fail:
  if(h->mh)
    curl_multi_cleanup(h->mh);
  if(h->curl)
    curl_easy_cleanup(h->curl);
  if(h)
    free(h);

  return NULL;
}

int fcurl_close(struct fcurl_handle *h)
{
  int ret=0;/* default is good return */

  /* make sure the easy handle is not in the multi handle anymore */
  curl_multi_remove_handle(h->mh, h->curl);

  /* cleanup */
  curl_easy_cleanup(h->curl);

  free(h->b.p);
  free(h);

  return ret;
}

int fcurl_eof(struct fcurl_handle *h)
{
  int ret=0;

  /* add code here */

  return ret;
}

/*
 * fread() look-alike
 *
 *    size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
 *
 * "fread() does not distinguish between end-of-file and error, and callers
 * must use feof(3) and ferror(3) to determine which occurred."
 */

size_t fcurl_read(void *ptr, size_t size, size_t nmemb,
                  struct fcurl_handle *h)
{
  size_t max = nmemb * size;
  int rc;

  rc = transfer(h, ptr, max); /* transfer data */

  if(rc)
    return 0;

  return h->user.used;
}

char *fcurl_fgets(char *ptr, size_t size, struct fcurl_handle *file)
{
  size_t want = size - 1;/* always need to leave room for zero termination */

  /* add code */

  return ptr;/*success */
}

void fcurl_rewind(struct fcurl_handle *h)
{

}
