#ifndef __LIME_LANG_i18n__
#define __LIME_LANG_i18n__

// Internal language values
typedef enum
{
	LANG_SYSTEM = 0,  ///< System Language
	LANG_JP,          ///< Japanese
	LANG_EN,          ///< English
	LANG_FR,          ///< French
	LANG_DE,          ///< German
	LANG_IT,          ///< Italian
	LANG_ES,          ///< Spanish
	LANG_ZH,          ///< Simplified Chinese
	LANG_KO,          ///< Korean
	LANG_NL,          ///< Dutch
	LANG_PT,          ///< Portugese
	LANG_RU,          ///< Russian
	LANG_TW,          ///< Traditional Chinese
} Language;

int getTranslation(Language lang, std::string file, std::vector<std::string>* array);

std::string getLanguageString(Language lang);

#endif
