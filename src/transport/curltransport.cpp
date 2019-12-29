#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "debug.hpp"

#include "curltransport.hpp"

int CurlTransport::f_open(const char *filename, const char *mode) {
	fcurl_handle = fcurl_open(filename, mode);

	return fcurl_handle ? 0 : errno;
}

void CurlTransport::f_close() {
	if (fcurl_handle)
		fcurl_close(fcurl_handle);
}

int64_t CurlTransport::f_read(void* ptr, int64_t size, int64_t nmemb) {
	if (!fcurl_handle)
		return -1;

	int64_t total = 0;

	//DEBUG("fread %d x %d\n", (int) size, (int) nmemb);

	while (nmemb > 0)
	{
		int64_t part = fcurl_read(ptr, size, nmemb, fcurl_handle);
		//DEBUG("part = %d\n", (int) part);
		ptr = (char *) ptr + size * part;
		total += part;
		nmemb -= part;
	}

	//DEBUG("fread = %d\n", (int) total);

	return total;
}


int CurlTransport::f_seek(int64_t offset, int whence) {
	//if (!filehandle)
		return -1;
}

int64_t CurlTransport::f_tell() {
//	if (!filehandle)
		return -1;
}

int64_t CurlTransport::f_size() {
//	if (!filehandle)
		return -1;
}

bool CurlTransport::f_eof() {
	if (!fcurl_handle)
		return true;

	return true;
}
