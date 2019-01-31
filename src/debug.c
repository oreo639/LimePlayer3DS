#include <debug.h>

void DEBUG_printf(const char* dbgStr) {
	int length = strlen(dbgStr);
	svcOutputDebugString(dbgStr, length);
}
