#ifndef __ERROR_H__
#define __ERROR_H__

#if defined _3DS
#include <3ds.h>
#endif //if defined _3DS

/* Errors that can't be explained with errno */
#define NDSP_INIT_FAIL			1000
#define DECODER_INIT_FAIL		1001
#define FILE_NOT_SUPPORTED		1002
#define UNSUPPORTED_CHANNELS	1003


#endif //ifndef __ERROR_H__
