#ifndef __LIMEPLAYER_NETTRANS__
#define __LIMEPLAYER_NETTRANS__

#include "transport.hpp"
#include "net.hpp"

class NetTransport : public FileTransport {
	public:
		NetTransport() {}
		//FileTransport(const char* filename) {f_open(filename, "rb");}

		~NetTransport() {f_close();}

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
		http_context httpcontext;
		bool ctx_eof;
};

#endif
