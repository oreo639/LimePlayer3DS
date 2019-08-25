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

C2D_Text* i18n::Localize(int lang, const std::string& val) {
	switch (lang)
	{
		case LANG_DE:
			return de->localize(val);
		case LANG_EN:
			return en->localize(val);
		case LANG_ES:
			return es->localize(val);
		case LANG_FR:
			return fr->localize(val);
		case LANG_IT:
			return it->localize(val);
		case LANG_JP:
			return jp->localize(val);
		case LANG_KO:
			return ko->localize(val);
		case LANG_NL:
			return nl->localize(val);
		case LANG_PT:
			return pt->localize(val);
		case LANG_ZH:
			return zh->localize(val);
		case LANG_TW:
			return tw->localize(val);
		case LANG_RU:
			return ru->localize(val);
		default:
			return NULL;
	}
	return NULL;
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
	else if (str  == "ru")
		return LANG_RU;
	else if (str == "tw")
		return LANG_TW;

	return LANG_EN;
}
