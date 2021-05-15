#include "SettingsMenu.hpp"
#include "gui.hpp"
#include "SelectFileMenu.hpp"
#include "app.hpp"
#include "error.hpp"
#include "config.hpp"

#define MAX_LIST 15

static bool set_wm_config();
static bool save_all_settings();

void SettingsMenu::DisplaySettingsList(int rows, float size) const
{
	for (uint32_t i = 0; rows-seloffs > i && i <= MAX_LIST; i++) {
		if (seloffs+i < settingslist.size()) {
			auto entry = settingslist[i];
			Gui::PrintColor(entry.title, 8.0f, i*size, 0.4f, 0.4f, 0xFF000000);
		}
	}
}

static void menuList(int cur, int from, float size, int rows)
{
	int i = 0;
	bool color = true;

	if (rows < 1)
		return;

	for (i = 0; rows-from > i && i < MAX_LIST; i++) {
		if (cur-from == i)
			C2D_DrawRectSolid(0, i*size, 0.5f, SCREEN_WIDTH, size, C2D_Color32(255, 255, 2, 255));
		else if (color)
			C2D_DrawRectSolid(0, i*size, 0.5f, SCREEN_WIDTH, size, C2D_Color32(23, 100, 64, 255));
		else if (!color)
			C2D_DrawRectSolid(0, i*size, 0.5f, SCREEN_WIDTH, size, C2D_Color32(43, 191, 63, 255));

		color = !color;
	}
}

SettingsMenu::SettingsMenu()
{
	this->settingslist.emplace_back("Set WildMidi config", &set_wm_config);
	this->settingslist.emplace_back("Save settings", &save_all_settings);
}

void SettingsMenu::drawTop() const
{
	Gui::Print("Settings Menu", 20.0f, 40.0f, 0.5f, 0.5f);
}

void SettingsMenu::drawBottom() const
{
	menuList(cursor, seloffs, 15, settingslist.size());
	DisplaySettingsList(settingslist.size(), 15);
}

void SettingsMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();

	if(kDown & KEY_UP && cursor > 0)
	{
		cursor--;

		/* 26 is the maximum number of entries that can be printed */
		if(settingslist.size() - cursor > MAX_LIST && seloffs != 0)
			seloffs--;
	}

	if(kDown & KEY_DOWN && cursor < settingslist.size()-1)
	{
		cursor++;

		if(cursor >= MAX_LIST && settingslist.size() - cursor >= 0 && seloffs < settingslist.size() - MAX_LIST)
			seloffs++;
	}

	if (kDown & KEY_A)
	{
		if (cursor+seloffs < settingslist.size()) {
			if (settingslist[cursor+seloffs].callback())
				Error::Add(ERRORH_GENERIC, "Failed to execute.");
		}
		return;
	}

	if (kDown & KEY_B)
	{
		Gui::BackMenu();
		return;
	}
}

static bool set_wm_config() {
	Gui::SetMenu(std::make_unique<SelectFileMenu>(&App::pInfo.settings.wildMidiConfig));
	return 0;
}

static bool save_all_settings() {
	int ret = 0;
	if (!(ret = Cfg::WriteJson("/3ds/limeplayer3ds/config.json", &App::pInfo.settings)))
		Error::Add(ERRORH_GENERIC, "Saved successfully.");
	return ret;
}
