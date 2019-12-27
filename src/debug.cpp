#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#if !NODEBUG
#include "debug.hpp"
#include "file.hpp"

static FILE* logFP = NULL;
static bool is_init = false;
static bool log_file = false;

void debug_init(bool use_file) {
	if (!is_init && use_file) {
		File::Copy("/3ds/limeplayer3ds/2.log", "/3ds/limeplayer3ds/3.log");
		File::Copy("/3ds/limeplayer3ds/1.log", "/3ds/limeplayer3ds/2.log");
		File::Copy("/3ds/limeplayer3ds/0.log", "/3ds/limeplayer3ds/1.log");
		File::Copy("/3ds/limeplayer3ds/recent.log", "/3ds/limeplayer3ds/0.log");

		logFP = fopen("/3ds/limeplayer3ds/recent.log", "w+");
		if (logFP) {
			is_init = true;
			log_file = use_file;
			debug_perform("Debug output for LimePlayer3DS version %s\nTHIS FILE IS AUTOMATICALY GENERATED PLEASE DO NOT MODIFY!\n", LIMEPLAYER_VERSION);
		}
	}
}

void debug_exit(void) {
	if (is_init) {
		if (log_file)
			fclose(logFP);
		is_init = false;
	}
}


void debug_perform(const char* fmt, ...) {
	if (is_init) {
		va_list args;

		va_start(args, fmt);
		if (log_file)
			vfprintf(logFP, fmt, args);
		vfprintf(stderr, fmt, args);
		va_end(args);
		// Make sure data is written to file NOW.
		fflush(logFP);
	}
}
#endif // ifndef NODEBUG
