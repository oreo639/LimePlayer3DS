#ifndef __LP_ABOUTMENU_HPP__
#define __LP_ABOUTMENU_HPP__

#include "menu.hpp"
#include <memory>
#include <vector>

class AboutMenu : public Menu
{
	public:
		AboutMenu();
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
};

#endif
