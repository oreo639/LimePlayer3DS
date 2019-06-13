#ifndef NODEBUGSTR
#define DEBUG(...) fprintf(stderr, __VA_ARGS__)
#else
#pragma message("Note: Debug strings are disabled.")
#define DEBUG(...)
#endif
