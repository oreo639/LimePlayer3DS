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
#include "i18n.hpp"

static TranslationStrings* jp = nullptr;
static TranslationStrings* en = nullptr;
static TranslationStrings* fr = nullptr;
static TranslationStrings* it = nullptr;
static TranslationStrings* de = nullptr;
static TranslationStrings* es = nullptr;
static TranslationStrings* ko = nullptr;
static TranslationStrings* zh = nullptr;
static TranslationStrings* tw = nullptr;
static TranslationStrings* nl = nullptr;
static TranslationStrings* pt = nullptr;
static TranslationStrings* ru = nullptr;

void i18n::Init(void)
{
	jp = new TranslationStrings(LANG_JP);
	en = new TranslationStrings(LANG_EN);
	fr = new TranslationStrings(LANG_FR);
	it = new TranslationStrings(LANG_IT);
	de = new TranslationStrings(LANG_DE);
	es = new TranslationStrings(LANG_ES);
	ko = new TranslationStrings(LANG_KO);
	zh = new TranslationStrings(LANG_ZH);
	tw = new TranslationStrings(LANG_TW);
	nl = new TranslationStrings(LANG_NL);
	pt = new TranslationStrings(LANG_PT);
	ru = new TranslationStrings(LANG_RU);
}

void i18n::Exit(void)
{
	delete jp;
	delete en;
	delete fr;
	delete it;
	delete de;
	delete es;
	delete ko;
	delete zh;
	delete tw;
	delete nl;
	delete pt;
	delete ru;
}

staticString i18n::LocalizeStatic(int lang, const std::string& val) {
	int tmplang = LANG_EN;
	staticString lang_string = NULL;

	if (lang == LANG_SYSTEM)
		tmplang = getSystemLanguage();
	else
		tmplang = lang;

	switch (tmplang)
	{
		case LANG_JP:
			lang_string = jp->LocalizeStatic(val);
			break;
		case LANG_EN:
			lang_string = en->LocalizeStatic(val);
			break;
		case LANG_FR:
			lang_string = fr->LocalizeStatic(val);
			break;
		case LANG_DE:
			lang_string = de->LocalizeStatic(val);
			break;
		case LANG_IT:
			lang_string = it->LocalizeStatic(val);
			break;
		case LANG_ES:
			lang_string = es->LocalizeStatic(val);
			break;
		case LANG_ZH:
			lang_string = zh->LocalizeStatic(val);
			break;
		case LANG_KO:
			lang_string = ko->LocalizeStatic(val);
			break;
		case LANG_NL:
			lang_string = nl->LocalizeStatic(val);
			break;
		case LANG_PT:
			lang_string = pt->LocalizeStatic(val);
			break;
		case LANG_RU:
			lang_string = ru->LocalizeStatic(val);
			break;
		case LANG_TW:
			lang_string = tw->LocalizeStatic(val);
			break;
	}

	if (!lang_string)
		lang_string = en->LocalizeStatic(val);

	return lang_string;
}

std::string i18n::Localize(int lang, const std::string& val) {
	int tmplang = LANG_EN;
	std::string lang_string;

	if (lang == LANG_SYSTEM)
		tmplang = getSystemLanguage();
	else
		tmplang = lang;

	switch (tmplang)
	{
		case LANG_JP:
			lang_string = jp->Localize(val);
			break;
		case LANG_EN:
			lang_string = en->Localize(val);
			break;
		case LANG_FR:
			lang_string = fr->Localize(val);
			break;
		case LANG_DE:
			lang_string = de->Localize(val);
			break;
		case LANG_IT:
			lang_string = it->Localize(val);
			break;
		case LANG_ES:
			lang_string = es->Localize(val);
			break;
		case LANG_ZH:
			lang_string = zh->Localize(val);
			break;
		case LANG_KO:
			lang_string = ko->Localize(val);
			break;
		case LANG_NL:
			lang_string = nl->Localize(val);
			break;
		case LANG_PT:
			lang_string = pt->Localize(val);
			break;
		case LANG_RU:
			lang_string = ru->Localize(val);
			break;
		case LANG_TW:
			lang_string = tw->Localize(val);
			break;
	}

	if (lang_string.empty())
		lang_string = en->Localize(val);

	return lang_string;
}

int i18n::Code2Int(const std::string& str) {
	if (str == "sy")
		return getSystemLanguage();
	else if (str == "jp")
		return LANG_JP;
	else if (str == "en")
		return LANG_EN;
	else if (str == "fr")
		return LANG_FR;
	else if (str == "de")
		return LANG_DE;
	else if (str == "it")
		return LANG_IT;
	else if (str == "es")
		return LANG_ES;
	else if (str == "zh")
		return LANG_ZH;
	else if (str == "ko")
		return LANG_KO;
	else if (str == "nl")
		return LANG_NL;
	else if (str == "pt")
		return LANG_PT;
	else if (str == "ru")
		return LANG_RU;
	else if (str == "tw")
		return LANG_TW;

	return LANG_EN;
}

std::string i18n::Int2Code(int lang) {
	switch (lang) {
		case LANG_SYSTEM:
			return "sy";
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
