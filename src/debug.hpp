#include "macrotools.h"

#ifndef __LP_DEBUG_H__
#define __LP_DEBUG_H__

#if !NODEBUG
#define DEBUG(...) debug_perform("[" __FILE__ "] " STRINGIFY(__LINE__) ": " __VA_ARGS__)

void debug_init(bool use_file);

void debug_exit(void);

void debug_perform(const char* fmt, ...);

#else

#pragma message("Note: Debug strings are disabled.")
#define DEBUG(...)

static inline void debug_init(bool use_file) {};

static inline void debug_exit(void) {};

static inline void debug_perform(const char* fmt, ...) {};

#endif // if !NODEBUG
#endif // ifndef __LP_DEBUG_H__
