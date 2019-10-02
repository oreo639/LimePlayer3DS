#include "SettingsMenu.hpp"
#include "gui.hpp"

#include "SelectFileMenu.hpp"
#include "app.hpp"

SettingsMenu::SettingsMenu()
{

}

void SettingsMenu::drawTop() const
{
	Gui::Print("Settings Menu", 20.0f, 40.0f, 0.5f, 0.5f);
}

void SettingsMenu::drawBottom() const
{

}

void SettingsMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();

	if (kDown & KEY_A)
	{
		Gui::SetMenu(std::make_unique<SelectFileMenu>(&App::pInfo.settings.wildMidiConfig));
		return;
	}

	if (kDown & KEY_B)
	{
		Gui::BackMenu();
		return;
	}
}
