#include <string>
#include <vector>

#include <3ds.h>
#include <jansson.h>

#include "lang.hpp"
#include "error.hpp"

std::string getSystemLanguageString(void) {
	u8 language = 0;
	cfguInit();
	if (R_SUCCEEDED(CFGU_GetSystemLanguage(&language)))
		switch (language) {
			case CFG_LANGUAGE_JP:
				return "jp";
			case CFG_LANGUAGE_EN:
				return "en";
			case CFG_LANGUAGE_FR:
				return "fr";
			case CFG_LANGUAGE_DE:
				return "de";
			case CFG_LANGUAGE_IT:
				return "it";
			case CFG_LANGUAGE_ES:
				return "es";
			case CFG_LANGUAGE_ZH:
				return "zh";
			case CFG_LANGUAGE_KO:
				return "ko";
			case CFG_LANGUAGE_NL:
				return "nl";
			case CFG_LANGUAGE_PT:
				return "pt";
			case CFG_LANGUAGE_RU:
				return "ru";
			case CFG_LANGUAGE_TW:
				return "tw";
			default:
				return "en";
		}
	return "en";	
}

std::string getLanguageString(int lang) {
	switch (lang) {
		case LANG_SYSTEM:
			return getSystemLanguageString();
		case LANG_JP:
			return "jp";
		case LANG_EN:
			return "en";
		case LANG_FR:
			return "fr";
		case LANG_DE:
			return "de";
		case LANG_IT:
			return "it";
		case LANG_ES:
			return "es";
		case LANG_ZH:
			return "zh";
		case LANG_KO:
			return "ko";
		case LANG_NL:
			return "nl";
		case LANG_PT:
			return "pt";
		case LANG_RU:
			return "ru";
		case LANG_TW:
			return "tw";
		default:
			return "en";
	}
	return "en";
}

int getTranslation(int lang, std::string file, std::vector<std::string>* array) {
	json_error_t *pjsonError = NULL;
	json_t *pJson = json_load_file(("romfs:/i18n/"+getLanguageString(lang)+"/"+file).c_str(), 0, pjsonError);

	std::string tmp;

	if (!pJson) {
		pJson = json_load_file(("romfs:/i18n/en/"+file).c_str(), 0, pjsonError);
		if (!pJson) {
			DEBUG("Json string loading failed, please check that file exists and follows the json spec.");
			return 1;
		}
	}

	const char *key;
	json_t *value;

	if (json_is_object(pJson)) {
		json_object_foreach(pJson, key, value) {
			if(json_is_string(value)) {
				tmp.assign(json_string_value(value));
				array->push_back(tmp);
			}
		}
	}
	else
	{
		return 2;
	}

	return 0;
}
