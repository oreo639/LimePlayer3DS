#include <3ds.h>
#include <stdio.h>
#include <string.h>

#define DEBUG(...) fprintf(stderr, __VA_ARGS__)

/* Prints more error information */
#define err_print(err) \
	do { fprintf(stderr, "\nError %d:%s(): %s %s\n", __LINE__, __func__, \
			err, strerror(errno)); } while (0)

#define delete(ptr) \
	free((void*) ptr); ptr = NULL

void DEBUG_printf(const char* dbgStr);
