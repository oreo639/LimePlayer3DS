#include <3ds.h>

#include <string.h>

#include "debug.hpp"

void DEBUG_printf(const char* dbgStr, ...) {
	int length = strlen(dbgStr);
	svcOutputDebugString(dbgStr, length);
}
