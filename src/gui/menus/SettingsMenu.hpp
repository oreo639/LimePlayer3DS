#ifndef __LP_SETTINGMENU_HPP__
#define __LP_SETTINGMENU_HPP__

#include "menu.hpp"
#include <memory>
#include <vector>

class SettingsMenu : public Menu
{
	public:
		SettingsMenu();
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
};

#endif
