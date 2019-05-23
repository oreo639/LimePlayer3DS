#ifndef __LIME_NETWORK_H__
#define __LIME_NETWORK_H__

#include <3ds.h>

Result http_open(httpcContext *httpc, const char* url);

Result http_download(httpcContext *httpc, u8 *buf, u32 size, u32 *readsize);

Result http_close(httpcContext *httpc);
#endif
