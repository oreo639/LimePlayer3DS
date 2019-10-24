#ifndef __LP_BROWSERMENU_HPP__
#define __LP_BROWSERMENU_HPP__
#include <cmath>
#include <memory>

#include "menu.hpp"
#include "explorer.hpp"

class PlayerMenu : public Menu
{
	public:
		PlayerMenu();
		~PlayerMenu();
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
		void fblist(int rows, int startpoint, float size) const;
	private:
		void BrowserControls(touchPosition* touch);
		std::unique_ptr<Explorer> expInst;
};

#endif
