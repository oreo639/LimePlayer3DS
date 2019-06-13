#ifndef __LIME_ERROR_H__
#define __LIME_ERROR_H__

/* Errors that can't be explained with errno */
#define NDSP_INIT_FAIL			1000
#define DECODER_INIT_FAIL		1001
#define FILE_NOT_SUPPORTED		1002
#define DECODER_INIT_TIMEOUT		1003
#define UNSUPPORTED_CHANNELS		1004

#include <vector>
#include <string>

typedef struct {
	int err_type = 0;
	int err_code;
	std::string extra_info;
} LimeError_t;

namespace Error {
	void Add(int err);

	void Add(int err, std::string& extraInfo);

	void Remove(void);

	LimeError_t Get(void);

	bool IsQuered(void);

	void Clear(void);
}

#endif //ifndef __ERROR_H__
