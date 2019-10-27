#include "QuickSetOverlay.hpp"
#include "SettingsMenu.hpp"
#include "AboutMenu.hpp"
#include "gui.hpp"

enum {
	SELMENU_SETTINGS = 0,
	SELMENU_RADIO,
	SELMENU_GLOBE,
	SELMENU_CREDITS
};

#define MENU_ICON_HORIZONTAL 100

static int selcursor = 0;

QuickSetOverlay::QuickSetOverlay(Menu& menu)
	: Overlay(menu)
{

}

QuickSetOverlay::QuickSetOverlay(Overlay& ovly)
		: Overlay(ovly)
{

}

void QuickSetOverlay::drawTop() const
{
	Overlay::dim();
	Gui::Print("Quick menu", 20.0f, 40.0f, 0.5f, 0.5f);
}

void QuickSetOverlay::drawBottom() const
{
	Overlay::dim();

	if (selcursor == SELMENU_SETTINGS)
		Gui::DrawSolidRectangle(20, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == SELMENU_RADIO)
		Gui::DrawSolidRectangle(80, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == SELMENU_GLOBE)
		Gui::DrawSolidRectangle(140, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == SELMENU_CREDITS)
		Gui::DrawSolidRectangle(200, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));

	Gui::DrawImage(sprites_menu_settings_idx, 20, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_radio_idx, 80, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_globe_idx, 140, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_copyright_colored_idx, 200, MENU_ICON_HORIZONTAL);
}

void QuickSetOverlay::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();

	if (kDown & KEY_B)
	{
		me = nullptr;
		return;
	}

	if (kDown & KEY_RIGHT && selcursor < 3)
		selcursor++;

	if (kDown & KEY_LEFT && selcursor > 0)
		selcursor--;

	if (kDown & KEY_A && selcursor == SELMENU_SETTINGS) {
		Gui::SetMenu(std::make_unique<SettingsMenu>());
	}
	if (kDown & KEY_A && selcursor == SELMENU_CREDITS) {
		Gui::SetMenu(std::make_unique<AboutMenu>());
	}
}
