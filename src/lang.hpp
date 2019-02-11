#ifndef __LIME_LANG_i18n__
#define __LIME_LANG_i18n__
#include <3ds.h>

//Custom language defines here.
#define CFG_LANGUAGE_DEFAULT 12

typedef enum {
	// InstallType text
	TEXT_LOADING_GENERIC = 0,
	TEXT_LOADING_INFO,
	TEXT_MENU_SETTINGS,
	TEXT_MENU_MODE,
	TEXT_MENU_INTERNET,
	TEXT_MENU_CREDITS,
	TEXT_CREDITS_MAIN,
	TEXT_MAX
} Text_id;

extern const char* const g_strings[TEXT_MAX][16];
#endif
