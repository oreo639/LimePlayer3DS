#ifndef FCURL_H
#define FCURL_H
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
struct fcurl_handle;

typedef struct fcurl_handle FCURL;

/* Running sed s/f/fcurl_/ on these functions should be enough. Maintain libc
   behavior as far as possible (by default) */

FCURL *fcurl_open(const char *path, const char *mode);
size_t fcurl_read(void *ptr, size_t size, size_t nmemb, FCURL *stream);
size_t fcurl_write(const void *ptr, size_t size, size_t nmemb,
                   FCURL *stream);
int fcurl_flush(FCURL *stream);
int fcurl_close(FCURL *stream);
int fcurl_getc(FCURL *stream);
char *fcurl_gets(char *s, int size, FCURL *stream);
int fcurl_ungetc(int c, FCURL *stream);

#endif
