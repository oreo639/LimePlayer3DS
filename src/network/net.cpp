#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <3ds.h>

#include "net.hpp"
#include "debug.hpp"
#include "content.hpp"
#include "macrotools.h"

#define HTTP_USER_AGENT "LimePlayer3DS (Nintendo 3DS; HOS ARMv6K) version/" STRINGIFY(LIMEPLAYER_VERSION)

#define R_APP_OUT_OF_MEMORY MAKERESULT(RL_FATAL, RS_OUTOFRESOURCE, RM_APPLICATION, RD_OUT_OF_MEMORY)

#define HTTP_TIMEOUT_SEC 20
#define HTTP_TIMEOUT_NS ((u64) HTTP_TIMEOUT_SEC * 1000000000)

/*
Note::https://github.com/dreamerc/mpg123/blob/master/src/httpget.c#L73
*/

Result http_open(http_context* httpctx, const char* url, bool allowIcyCast) {
	Result ret = 0;
	u32 statuscode;
	char currUrl[1024];
	memcpy(currUrl, url, sizeof(currUrl)-1);

	httpctx->dbufSize = 0;

	DEBUG("Connecting to %s\n", url);

	do {
		ret = httpcOpenContext(&httpctx->httpc, HTTPC_METHOD_GET, currUrl, 1);
		DEBUG("return from httpcOpenContext: %" PRId32 "\n",ret);

		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(&httpctx->httpc, SSLCOPT_DisableVerify);
		DEBUG("return from httpcSetSSLOpt: %" PRId32 "\n",ret);

		// Enable Keep-Alive connections
		ret = httpcSetKeepAlive(&httpctx->httpc, HTTPC_KEEPALIVE_ENABLED);
		DEBUG("return from httpcSetKeepAlive: %" PRId32 "\n",ret);

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(&httpctx->httpc, "User-Agent", HTTP_USER_AGENT);
		DEBUG("return from httpcAddRequestHeaderField (user-agent): %" PRId32 "\n",ret);

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcAddRequestHeaderField(&httpctx->httpc, "Connection", "Keep-Alive");
		DEBUG("return from httpcAddRequestHeaderField (connection): %" PRId32 "\n",ret);

		// Tell the server that we support ICYcast/SHOUTcast metadata.
		if (allowIcyCast)
			ret = httpcAddRequestHeaderField(&httpctx->httpc, "Icy-MetaData", "1");

		ret = httpcBeginRequest(&httpctx->httpc);
		if(ret!=0){
			httpcCloseContext(&httpctx->httpc);
			return ret;
		}
		DEBUG("return from httpcBeginRequest: %" PRId32 "\n",ret);

		ret = httpcGetResponseStatusCodeTimeout(&httpctx->httpc, &statuscode, HTTP_TIMEOUT_NS);
		if(ret!=0){
			httpcCloseContext(&httpctx->httpc);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			char newurl[1024];
			ret = httpcGetResponseHeader(&httpctx->httpc, "Location", newurl, 1024);
			memcpy(currUrl, newurl, sizeof(currUrl)-1);
			DEBUG("redirecting to url: %s\n", currUrl);
			httpcCloseContext(&httpctx->httpc); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if (statuscode!=200) {
		DEBUG("URL returned status: %" PRId32 "\n", statuscode);
		httpcCloseContext(&httpctx->httpc);
		return statuscode;
	}

	char content_type[255];
	if (!httpcGetResponseHeader(&httpctx->httpc, "Content-Type", content_type, 255)) {
		DEBUG("Process recieved content-type.\n");
		httpctx->content_type = getContentType(content_type);
		DEBUG("Content-Type processed.\n");
	} else
		httpctx->content_type = CONTENT_UNKNOWN;

	httpctx->isShoutcastSupported = false;

	if (allowIcyCast && httpctx->content_type == CONTENT_MPEG3) {
		// Neither MP3 nor AAC support metadata in streams, so ICYcast/SHOUTcast
		// must be used instead.
		char icy_byteinterval[8];
		if (!httpcGetResponseHeader(&httpctx->httpc, "icy-metaint", icy_byteinterval, 8)) {
			DEBUG("icy-metaint: %s.\n", icy_byteinterval);
			httpctx->icy_byteinterval = atoi(icy_byteinterval);
			if (httpctx->icy_byteinterval)
				httpctx->isShoutcastSupported = true;
		}
	}

	httpctx->dbuf = (uint8_t*)malloc(4096);
	if (!httpctx->dbuf) {
		httpcCloseContext(&httpctx->httpc);
		DEBUG("Net: Malloc Failed.\n");
		return R_APP_OUT_OF_MEMORY;
	}
	httpctx->dbufSize = 4096;

	return ret;
}

Result http_download(http_context* httpctx) {
	return httpcDownloadData(&httpctx->httpc, httpctx->dbuf, httpctx->dbufSize, &httpctx->readsize);
}

void http_close(http_context* httpctx) {
	if (httpctx->dbuf)
		free(httpctx->dbuf);
	httpctx->dbufSize = 0;
	httpcCloseContext(&httpctx->httpc);
	return;
}
