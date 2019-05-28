#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <citro3d.h>
#include <citro2d.h>

#include "player.hpp"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define MAX_LIST 7

class Gui
{
	public:
		Gui(void);
	
		~Gui(void);
	
		void Drawui(playbackInfo_t* musInfo);
	
		int InitlimeGFX(void);

		void CloselimeGFX(void);
	
		void GuiCursorMove(int move);
	
		void GuiCursorReset(void);
	
		int GuiGetCursorPos(void);

	private:
		void textinit(void);

		void startframe(void);

		void endframe(void);

		void guiprint(const char* text, float xloc, float yloc, float scaleX, float scaleY);

		void guiprintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

		void guilist(const char* text, int row);

		void drawBaseGui(void);

		void drawBrowserPlayer(playbackInfo_t* info);
	
		void drawError(void);
	
		void fblist(int rows, int startpoint);
};
#endif
