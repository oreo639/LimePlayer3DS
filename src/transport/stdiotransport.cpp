#if BIGFILE_SUPPORT
#define _FILE_OFFSET_BITS 64
#endif

#include <stdint.h>
#include <stdio.h>
#include <errno.h>

#include "stdiotransport.hpp"

int StdioTransport::f_open(const char *filename, const char *mode) {
	m_filehandle = fopen(filename, mode);

	return m_filehandle ? 0 : errno;
}

void StdioTransport::f_close() {
	if (m_filehandle)
		fclose(m_filehandle);
}

int64_t StdioTransport::f_read(void* ptr, int64_t size, int64_t nmemb) {
	if (!m_filehandle)
		return -1;

	return fread(ptr, size, nmemb, m_filehandle);
}


int StdioTransport::f_seek(int64_t offset, int whence) {
	if (!m_filehandle)
		return -1;

	return fseek(m_filehandle, offset, whence);
}

int64_t StdioTransport::f_tell() {
	if (!m_filehandle)
		return -1;

	return ftell(m_filehandle);
}

int64_t StdioTransport::f_size() {
	if (!m_filehandle)
		return -1;

	int64_t f_size;

	fseeko(m_filehandle, 0, SEEK_END);
	f_size = ftello(m_filehandle);
	rewind(m_filehandle); // to back to start again

	return f_size;
}

bool StdioTransport::f_eof() {
	if (!m_filehandle)
		return true;

	return feof(m_filehandle) == 0;
}
