#ifndef __LP_OVERLAY_HPP__
#define __LP_OVERLAY_HPP__

#include <3ds.h>
#include <memory>

class Menu;

class Overlay
{
	public:
		Overlay(Menu& menu);
		Overlay(Overlay& overlay);
		virtual ~Overlay() {}
		template <typename Class, typename... Params>
		void addOverlay(Params&&... args)
		{
			if (overlay)
			{
				overlay->addOverlay<Class>(std::forward<Params>(args)...);
			}
			else
			{
				overlay = std::make_shared<Class>(*this, std::forward<Params>(args)...);
			}
		}
		bool willHandleUpdate() const;
		bool willReplaceBottom() const;
		bool willReplaceTop() const;
		void doUpdate(touchPosition* touch);
		void doTopDraw() const;
		void doBottomDraw() const;
		virtual void update(touchPosition* touch) = 0;
		virtual void drawTop() const			  = 0;
		virtual void drawBottom() const		   = 0;
		virtual bool replacesTop() const { return false; }
		virtual bool replacesBottom() const { return false; }
		virtual bool handlesUpdate() const { return true; }
		void dim(void) const;

	protected:
		std::shared_ptr<Overlay>& me;
		std::shared_ptr<Overlay> overlay = nullptr;
};

#endif
