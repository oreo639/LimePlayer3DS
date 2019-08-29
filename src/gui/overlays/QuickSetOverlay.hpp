#ifndef __LP_QUICKSETOVERLAY_HPP__
#define __LP_QUICKSETOVERLAY_HPP__

#include "overlay.hpp"
#include <memory>
#include <vector>

class QuickSetOverlay : public Overlay
{
	public:
		QuickSetOverlay(Menu& menu);
		QuickSetOverlay(Overlay& ovly);
		void drawTop() const override;
		void drawBottom() const override;
		void update(touchPosition* touch) override;
};

#endif
