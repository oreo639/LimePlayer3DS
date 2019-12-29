#ifndef HANDLE_H
#define HANDLE_H
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

#include <stdbool.h>

struct buffer {
  char *p;     /* the allocated memory */
  size_t size; /* size of the allocation */
  size_t used; /* number of bytes put in the buffer */
};

#define BUFFERGROWTHMARGIN 8192

struct fcurl_handle {
  CURL *curl;
  CURLM *mh;
  bool transfer_complete;
  CURLcode transfer_rc; /* assigned after completed transfer */
  struct buffer b;    /* allocated and handled internally */
  struct buffer user; /* as provided by the application */
};

#endif
