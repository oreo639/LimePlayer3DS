#ifndef __LP_BROWSERMENU_HPP__
#define __LP_BROWSERMENU_HPP__
#include "menu.hpp"
#include <cmath>
#include <memory>

class BrowserMenu : public Menu
{
public:
    BrowserMenu();
    ~BrowserMenu();
    void drawTop() const override;
    void drawBottom() const override;
    void update(touchPosition* touch) override;
};

#endif
