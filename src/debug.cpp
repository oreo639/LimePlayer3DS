#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>

#include "debug.hpp"

static FILE* logFP = NULL;
static bool is_init = false;
static bool log_file = false;

void debug_init(bool use_file) {
	if (!is_init) {
		log_file = use_file;
		if (log_file)
			logFP = fopen("/3ds/limeplayer3ds/recent.log", "w+");
		is_init = true;
	}
}

void debug_exit(void) {
	if (is_init) {
		if (log_file) fclose(logFP);
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
	}
}
