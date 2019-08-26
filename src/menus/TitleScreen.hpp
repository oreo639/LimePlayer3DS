#ifndef __LP_TITLE_SCREEN_HPP__
#define __LP_TITLE_SCREEN_HPP__
#include "gui.hpp"
#include "menu.hpp"
#include <cmath>
#include <memory>

class TitleScreen : public Menu
{
public:
    TitleScreen();
    ~TitleScreen() {}
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;
};

#endif
