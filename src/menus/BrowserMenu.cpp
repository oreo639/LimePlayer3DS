#include "BrowserMenu.hpp"
#include "app.hpp"

BrowserMenu::BrowserMenu() {}

void BrowserMenu::drawTop() const
{
	Gui::drawBaseGui();
	if (PlayerInterface::IsPlaying()) {
		Gui::drawBrowserPlayer(&App::pInfo);
	}
}

void BrowserMenu::drawBottom() const
{
	Gui::menuList(Gui::GetCursorPos(), Gui::GetSelPos(), 15, 32.5f, App::dirList.total);
	C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
	Gui::List(App::dirList.currentDir.c_str(), 0);
	Gui::fblist(App::dirList.total, 15);
}

void BrowserMenu::update(touchPosition* touch)
{

}
