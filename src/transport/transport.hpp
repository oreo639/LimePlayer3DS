#ifndef __LIMEPLAYER_FILETRANS__
#define __LIMEPLAYER_FILETRANS__
#include <string>

class FileTransport {
	public:
		FileTransport() {}

		virtual ~FileTransport() {}

		virtual int f_open(const char *filename, const char *mode) = 0;

		virtual void f_close() = 0;

		virtual int64_t f_read(void* ptr, int64_t size, int64_t nmemb) = 0;
		virtual int f_seek(int64_t offset, int whence) = 0;

		virtual int64_t f_tell() = 0;
		virtual int64_t f_size() = 0;
		virtual bool f_eof() = 0;

		std::string GetError() {
			if (errorStr)
				return errorStr;
			else
				return "";
		}

		int content_type;

		//int64_t fwrite(const void * ptr, int64_t size, int64_t nmemb);
		//int ftruncate(int64_t length);
		//int fflush();

		//String get_metadata(const char * field);
		protected:
			const char* errorStr = "";
};

#endif
