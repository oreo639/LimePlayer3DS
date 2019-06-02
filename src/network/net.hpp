#ifndef __LIME_NETWORK_H__
#define __LIME_NETWORK_H__

#include <3ds.h>

typedef struct {
	httpcContext httpc;
	uint32_t content_type;
	bool isShoutcastSupported;
	uint32_t icy_byteinterval;
	char *stream_title;
	uint8_t *dbuf;
	uint32_t dbufSize;
	uint32_t readsize;
} http_context;

Result http_open(http_context* httpctx, const char* url, bool allowIcyCast);

Result http_download(http_context* httpctx);

Result http_close(http_context* httpctx);
#endif
