#include "TitleScreen.hpp"

TitleScreen::TitleScreen() {}

void TitleScreen::drawTop() const
{
	Gui::PrintStatic("TEXT_WELCOME", 25, 25, 0.5f, 0.5f);
	Gui::Print("Press the <A> button to continue.", 25, 35, 0.5f, 0.5f);
}

void TitleScreen::drawBottom() const
{
}

void TitleScreen::update(touchPosition* touch)
{

}
