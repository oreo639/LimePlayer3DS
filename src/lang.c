#include "lang.h"
#include "settings.h"

#define STR_JP(_str) [CFG_LANGUAGE_JP] = _str
#define STR_EN(_str) [CFG_LANGUAGE_EN] = _str
#define STR_FR(_str) [CFG_LANGUAGE_FR] = _str
#define STR_DE(_str) [CFG_LANGUAGE_DE] = _str
#define STR_IT(_str) [CFG_LANGUAGE_IT] = _str
#define STR_ES(_str) [CFG_LANGUAGE_ES] = _str
#define STR_ZH(_str) [CFG_LANGUAGE_ZH] = _str
#define STR_KO(_str) [CFG_LANGUAGE_KO] = _str
#define STR_NL(_str) [CFG_LANGUAGE_NL] = _str
#define STR_PT(_str) [CFG_LANGUAGE_PT] = _str
#define STR_RU(_str) [CFG_LANGUAGE_RU] = _str
#define STR_TW(_str) [CFG_LANGUAGE_TW] = _str

const char* const g_strings[TEXT_MAX][16] =
{
	[TEXT_LOADING_GENERIC] =
	{
		STR_EN("Loading…"),
		STR_ES("Cargando…"),
		STR_DE("Lade…"),
		STR_FR("Chargement…"),
		STR_IT("Caricamento…"),
		STR_JP("ロード中…"),
		STR_PT("Carregando…"),
		STR_NL("Laden…"),
		STR_KO("로딩중…"),
		STR_RU("загрузка…"),
		STR_ZH("加载中…"),
		STR_TW("加載中…"),
	},
	
	[TEXT_LOADING_INFO] =
	{
		STR_EN("Obtaining information…"),
		STR_ES(NULL),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
	
	[TEXT_MENU_SETTINGS] =
	{
		STR_EN("Settings"),
		STR_ES("Ajustes"),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
	
	[TEXT_MENU_MODE] =
	{
		STR_EN("Modes"),
		STR_ES(NULL),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
	
	[TEXT_MENU_INTERNET] =
	{
		STR_EN("Internet"),
		STR_ES(NULL),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
	
	[TEXT_MENU_CREDITS] =
	{
		STR_EN("Credits and Copyright"),
		STR_ES(NULL),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
	
	[TEXT_CREDITS_MAIN] =
	{
		STR_EN("Limeplayer was created by Oreo639, and was heavly based \non Ctrmus."),
		STR_ES(NULL),
		STR_DE(NULL),
		STR_FR(NULL),
		STR_IT(NULL),
		STR_JP(NULL),
		STR_PT(NULL),
		STR_NL(NULL),
		STR_KO(NULL),
		STR_RU(NULL),
		STR_ZH(NULL),
		STR_TW(NULL),
	},
};
