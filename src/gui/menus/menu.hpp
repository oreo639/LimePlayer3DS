#ifndef __LP_MENU_HPP__
#define __LP_MENU_HPP__

#include <memory>

#include <3ds.h>
#include <citro3d.h>

#include "overlay.hpp"

class Menu
{
		friend class Overlay;

	public:
		Menu(void) {}
		virtual ~Menu() {}
		virtual void update(void)
		{
			// increase timer
			mTimer += 0.025f;
		}
		// Call currentOverlay->update if it exists, and update if it doesn't
		virtual void doUpdate(touchPosition* touch) final;
		virtual void update(touchPosition* touch) = 0;
		// Call draw, then currentOverlay->draw if it exists
		virtual void doTopDraw() const final;
		virtual void drawTop() const = 0;
		virtual void doBottomDraw() const final;
		virtual void drawBottom() const = 0;
		virtual float timer() const final { return mTimer; }
		void removeOverlays() { currentOverlay = nullptr; }
		template <typename Class, typename... Params>
		void addOverlay(Params&&... args)
		{
			if (currentOverlay)
			{
				currentOverlay->addOverlay<Class>(std::forward<Params>(args)...);
			}
			else
			{
				currentOverlay = std::make_shared<Class>(*this, std::forward<Params>(args)...);
			}
		}

	protected:
		// No point in restricting this to only being editable during update, especially since it's drawn afterwards. Allows setting it before the first
		// draw loop is done
		mutable std::shared_ptr<Overlay> currentOverlay = nullptr;

	private:
		float mTimer = 0;
};

#endif
