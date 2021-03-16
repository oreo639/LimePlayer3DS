#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "nettransport.hpp"

int NetTransport::f_open(const char *filename, const char *mode) {
	int ret = http_open(&m_httpcontext, filename, false);
	if (ret)
		m_errorStr = "Failed to connect to url.";
	this->content_type = m_httpcontext.content_type;
	return ret;
}

void NetTransport::f_close() {
	if (m_httpcontext.is_init)
		http_close(&m_httpcontext);
}

int64_t NetTransport::f_read(void* ptr, int64_t size, int64_t nmemb) {
	if (!m_httpcontext.is_init)
		return -1;

	m_ctx_eof = httpcDownloadData(&m_httpcontext.httpc, (u8*)ptr, nmemb*size, &m_httpcontext.readsize) != (s32)HTTPC_RESULTCODE_DOWNLOADPENDING;

	return m_httpcontext.readsize;
}


int NetTransport::f_seek(int64_t offset, int whence) {
	//if (!filehandle)
		return -1;

//	return fseek(filehandle, offset, whence);
}

int64_t NetTransport::f_tell() {
//	if (!filehandle)
		return -1;

//	return ftell(filehandle);
}

int64_t NetTransport::f_size() {
//	if (!filehandle)
		return -1;

//	int64_t f_size;

//	fseeko(filehandle, 0, SEEK_END);
//	f_size = ftello(filehandle);
//	rewind(filehandle); // to back to start again

//	return f_size;
}

bool NetTransport::f_eof() {
	if (!m_httpcontext.is_init)
		return true;

	return m_ctx_eof;
}
