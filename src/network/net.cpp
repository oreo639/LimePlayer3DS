#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include <3ds.h>

#include "error.hpp"
#include "net.hpp"

#define HTTP_USER_AGENT "LimePlayer3DS (Nintendo 3DS; HOS ARMv6K) version/VERSION_MAJOR.VERSION_MINOR.VERSION_BUILD"

Result http_open(httpcContext *httpc, const char* url) {
	Result ret = 0;
	u32 statuscode;
	char currUrl[1024];
	char *newurl = NULL;
	memcpy(currUrl, url, sizeof(currUrl)-1);

	DEBUG("Connecting to %s\n", url);

	do {
		ret = httpcOpenContext(httpc, HTTPC_METHOD_GET, currUrl, 1);
		DEBUG("return from httpcOpenContext: %" PRId32 "\n",ret);

		// This disables SSL cert verification, so https:// will be usable
		ret = httpcSetSSLOpt(httpc, SSLCOPT_DisableVerify);
		DEBUG("return from httpcSetSSLOpt: %" PRId32 "\n",ret);

		// Enable Keep-Alive connections
		ret = httpcSetKeepAlive(httpc, HTTPC_KEEPALIVE_ENABLED);
		DEBUG("return from httpcSetKeepAlive: %" PRId32 "\n",ret);

		// Set a User-Agent header so websites can identify your application
		ret = httpcAddRequestHeaderField(httpc, "User-Agent", "httpc-example/1.0.0");
		DEBUG("return from httpcAddRequestHeaderField: %" PRId32 "\n",ret);

		// Tell the server we can support Keep-Alive connections.
		// This will delay connection teardown momentarily (typically 5s)
		// in case there is another request made to the same server.
		ret = httpcAddRequestHeaderField(httpc, "Connection", "Keep-Alive");
		DEBUG("return from httpcAddRequestHeaderField: %" PRId32 "\n",ret);

		ret = httpcBeginRequest(httpc);
		if(ret!=0){
			httpcCloseContext(httpc);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		ret = httpcGetResponseStatusCode(httpc, &statuscode);
		if(ret!=0){
			httpcCloseContext(httpc);
			if(newurl!=NULL) free(newurl);
			return ret;
		}

		if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308)) {
			if(newurl==NULL) newurl = (char*)malloc(0x1000); // One 4K page for new URL
			if (newurl==NULL){
				httpcCloseContext(httpc);
				return ret;
			}
			ret = httpcGetResponseHeader(httpc, "Location", newurl, 0x1000);
			memcpy(currUrl, newurl, sizeof(currUrl)-1);
			DEBUG("redirecting to url: %s\n", currUrl);
			httpcCloseContext(httpc); // Close this context before we try the next
		}
	} while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

	if(statuscode!=200){
		DEBUG("URL returned status: %" PRId32 "\n", statuscode);
		httpcCloseContext(httpc);
		if(newurl) free(newurl);
		return ret;
	}

	if(newurl) free(newurl);

	return ret;
}

Result http_download(httpcContext *httpc, u8 *buf, u32 size, u32 *readsize) {
	return httpcDownloadData(httpc, buf, size, readsize);
}

Result http_close(httpcContext *httpc) {
	return httpcCloseContext(httpc);
}
