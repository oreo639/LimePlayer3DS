#if BIGFILE_SUPPORT
#define _FILE_OFFSET_BITS 64
#endif

#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include "stdiotransport.hpp"

int StdioTransport::f_open(const char *filename, const char *mode) {
	filehandle = fopen(filename, mode);

	return filehandle ? 0 : errno;
}

void StdioTransport::f_close() {
	if (filehandle)
		fclose(filehandle);
}

int64_t StdioTransport::f_read(void* ptr, int64_t size, int64_t nmemb) {
	if (!filehandle)
		return -1;

	return fread(ptr, size, nmemb, filehandle);
}


int StdioTransport::f_seek(int64_t offset, int whence) {
	if (!filehandle)
		return -1;

	return fseek(filehandle, offset, whence);
}

int64_t StdioTransport::f_tell() {
	if (!filehandle)
		return -1;

	return ftell(filehandle);
}

int64_t StdioTransport::f_size() {
	if (!filehandle)
		return -1;

	int64_t f_size;

	fseeko(filehandle, 0, SEEK_END);
	f_size = ftello(filehandle);
	rewind(filehandle); // to back to start again

	return f_size;
}

bool StdioTransport::f_eof() {
	if (!filehandle)
		return true;

	return feof(filehandle) == 0;
}
