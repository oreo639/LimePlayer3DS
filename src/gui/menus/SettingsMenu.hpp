#ifndef __LP_SETTINGMENU_HPP__
#define __LP_SETTINGMENU_HPP__

#include "menu.hpp"
#include <memory>
#include <vector>
#include <functional>

typedef struct optionEntry {
	optionEntry(const std::string& title, const std::function<bool()> callback) : title(title), callback(callback) {};
	const std::string title;
	const std::function<bool()> callback;
} optionEntry_t;

class SettingsMenu : public Menu
{
	public:
		SettingsMenu();
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
		void DisplaySettingsList(int rows, float size) const;
	private:
		std::vector<optionEntry_t> settingslist;
		uint32_t cursor = 0;
		uint32_t seloffs = 0;
};

#endif
