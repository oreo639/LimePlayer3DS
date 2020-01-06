#ifndef __LIMEPLAYER_FILETRANS__
#define __LIMEPLAYER_FILETRANS__
#include <string>

class FileTransport {
	public:
		FileTransport() {}
		//FileTransport(const char* filename) {f_open(filename, "rb");}

		virtual ~FileTransport() {}

		virtual int f_open(const char *filename, const char *mode);

		virtual void f_close();

		virtual int64_t f_read(void* ptr, int64_t size, int64_t nmemb);
		virtual int f_seek(int64_t offset, int whence);

		virtual int64_t f_tell();
		virtual int64_t f_size();
		virtual bool f_eof();

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
