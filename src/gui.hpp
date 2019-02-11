#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <citro3d.h>
#include <citro2d.h>

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define MAX_LIST 7

typedef enum {
	PLAYER_INTERFACE = 0,
	PLAYING_ICON,
	SPEAKER_MUTE,
	SPEAKER_QUIET,
	SPEAKER_SOFT,
	SPEAKER_MEDIUM,
	SPEAKER_LOUD,
	SPEAKER_BACKGROUND,
	MENU_SETTINGS,
	MENU_FOLDER,
	MENU_GLOBE,
	MENU_CREDITS,
	CONTROL_PLAY,
	CONTROL_PAUSE,
	CONTROL_STOP,
	CONTROL_SKPFW,
	CONTROL_SKPBW,
	CONTROL_SKIP,
} Gui_Sprites;

class Gui
{
	public:
	Gui(void);
	
	~Gui(void);
	
	void Drawui(void);
	
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
	
	void drawError(void);
	
	void fblist(int rows, int startpoint);
};
#endif
