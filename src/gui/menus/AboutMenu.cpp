#include "AboutMenu.hpp"
#include "gui.hpp"

#include "SelectFileMenu.hpp"
#include "app.hpp"

AboutMenu::AboutMenu()
{

}

void AboutMenu::drawTop() const
{
	Gui::Print("About:", 20.0f, 20.0f, 0.5f, 0.5f);
	Gui::Print("LimePlayer3DS version " LIMEPLAYER_VERSION, 40.0f, 40.0f, 0.5f, 0.5f);
	Gui::Print("This application is licensed under GPL3.0-or-later.", 40.0f, 60.0f, 0.5f, 0.5f);
	Gui::Print("Build" BUILD "(Beta)", 40.0f, 50.0f, 0.5f);
}

void AboutMenu::drawBottom() const
{

}

void AboutMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();

	if (kDown & KEY_B)
	{
		Gui::BackMenu();
		return;
	}
}
