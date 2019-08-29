#include "QuickSetOverlay.hpp"
#include "gui.hpp"

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
	Gui::Print("Quick settings", 20.0f, 40.0f, 0.5f, 0.5f);
}

void QuickSetOverlay::drawBottom() const
{
	Overlay::dim();

	if (selcursor == 0)
		Gui::DrawSolidRectangle(20, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == 1)
		Gui::DrawSolidRectangle(80, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == 2)
		Gui::DrawSolidRectangle(140, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));
	else if (selcursor == 3)
		Gui::DrawSolidRectangle(200, MENU_ICON_HORIZONTAL, 60, 60, C2D_Color32(80, 217, 238, 90));

	Gui::DrawImage(sprites_menu_settings_idx, 20, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_radio_idx, 80, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_globe_idx, 140, MENU_ICON_HORIZONTAL);
	Gui::DrawImage(sprites_menu_copyright_idx, 200, MENU_ICON_HORIZONTAL);
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
}
