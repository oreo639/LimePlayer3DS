/*   LimePlayer3DS FOSS graphcal music player for the Nintendo 3DS.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string>
#include <vector>

#include <3ds.h>
#include <string.h>
#include <jansson.h>

#include "lang.hpp"
#include "gui.hpp"
#include "debug.hpp"

std::string getSystemLanguageString(void);
std::string getLanguageString(int lang);

TranslationStrings::TranslationStrings(int lang) {
	TranslationStrings::ParseJson(lang, "lang.json", gui);
}

int TranslationStrings::ParseJson(int lang, std::string file, textMap& strings) {
	json_error_t *pjsonError = NULL;
	json_t *pJson = json_load_file(("romfs:/i18n/"+getLanguageString(lang)+"/"+file).c_str(), 0, pjsonError);

	std::string tmp;

	if (!pJson) {
		pJson = json_load_file(("romfs:/i18n/en/"+file).c_str(), 0, pjsonError);
		DEBUG("Locale %s not implemented.\n Using en as fallback", getLanguageString(lang).c_str());
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
				strings.emplace(key, Gui::StaticTextGen(json_string_value(value)));
			}
		}
	}
	else
	{
		return 2;
	}

	return 0;
}

C2D_Text* TranslationStrings::localize(const std::string& v) {
	auto staticText = gui.find(v);
	if (staticText != gui.end()) {
		return &staticText->second;
	}
	return NULL;
}

int getSystemLanguage(void) {
	u8 language = 0;
	cfguInit();
	if (R_SUCCEEDED(CFGU_GetSystemLanguage(&language)))
		switch (language) {
			case CFG_LANGUAGE_JP:
				return LANG_JP;
			case CFG_LANGUAGE_EN:
				return LANG_EN;
			case CFG_LANGUAGE_FR:
				return LANG_FR;
			case CFG_LANGUAGE_DE:
				return LANG_DE;
			case CFG_LANGUAGE_IT:
				return LANG_IT;
			case CFG_LANGUAGE_ES:
				return LANG_ES;
			case CFG_LANGUAGE_ZH:
				return LANG_ZH;
			case CFG_LANGUAGE_KO:
				return LANG_KO;
			case CFG_LANGUAGE_NL:
				return LANG_NL;
			case CFG_LANGUAGE_PT:
				return LANG_PT;
			case CFG_LANGUAGE_RU:
				return LANG_RU;
			case CFG_LANGUAGE_TW:
				return LANG_TW;
			default:
				return LANG_EN;
		}
	return LANG_EN;
}

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
