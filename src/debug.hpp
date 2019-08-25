#include "macrotools.h"

#ifndef NODEBUGSTR
#define DEBUG(...) debug_perform("[" __FILE__ "] " STRINGIFY(__LINE__) ": " __VA_ARGS__)
#else
#pragma message("Note: Debug strings are disabled.")
#define DEBUG(...)
#endif

void debug_init(bool use_file);

void debug_exit(void);

void debug_perform(const char* fmt, ...);
