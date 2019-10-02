#ifndef __LP_SELECTFILEMENU_HPP__
#define __LP_SELECTFILEMENU_HPP__

#include <memory>
#include <vector>

#include "menu.hpp"
#include "explorer.hpp"

class SelectFileMenu : public Menu
{
	public:
		SelectFileMenu(std::string* path);
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
		void fblist(int rows, int startpoint, float size) const;
	private:
		std::string *filepath;
		std::unique_ptr<Explorer> expInst;
};

#endif
