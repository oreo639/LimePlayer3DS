#ifndef __LIMEPLAYER_CURLTRANS__
#define __LIMEPLAYER_CURLTRANS__

#include "transport.hpp"
extern "C" {
#include "fcurl.h"
}

class CurlTransport : public FileTransport {
	public:
		CurlTransport() {}
		//FileTransport(const char* filename) {f_open(filename, "rb");}

		~CurlTransport() {f_close();}

		int f_open(const char *filename, const char *mode) override;

		void f_close() override;

		int64_t f_read(void* ptr, int64_t size, int64_t nmemb) override;
		int f_seek(int64_t offset, int whence) override;

		int64_t f_tell() override;
		int64_t f_size() override;
		bool f_eof() override;

		//int64_t fwrite(const void * ptr, int64_t size, int64_t nmemb);
		//int ftruncate(int64_t length);
		//int fflush();

		//String get_metadata(const char * field);
	private:
		FCURL *fcurl_handle;
};

#endif
