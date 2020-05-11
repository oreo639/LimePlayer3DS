#ifndef __LIME_LANG_i18n__
#define __LIME_LANG_i18n__

#include <string>
#include <unordered_map>

#include <citro2d.h>

typedef std::unordered_map<std::string, std::pair<std::string, C2D_Text>> textMap;
typedef C2D_Text* staticString;

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

class TranslationStrings {
	public:
		TranslationStrings(int lang);
		int ParseJson(int lang, std::string file, textMap& strings);
		staticString LocalizeStatic(const std::string& key);
		std::string Localize(const std::string& key);
	private:
		textMap gui;
};

int getSystemLanguage(void);
#endif
