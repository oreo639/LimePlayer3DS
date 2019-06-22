#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <citro3d.h>
#include <citro2d.h>

#include "player.hpp"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define MAX_LIST 7

namespace Gui {
	void Init(settings_t* settings);

	void Exit(void);

	void TextInit(settings_t* settings);

	void TextExit(void);

	void Drawui(playbackInfo_t* musInfo);

	int InitlimeGFX(void);

	void CloselimeGFX(void);

	void CursorMove(int move);

	void CursorReset(void);

	int GetCursorPos(void);

	void startframe(void);

	void endframe(void);

	void Print(const char* text, float xloc, float yloc, float scaleX, float scaleY);

	void PrintStatic(uint8_t id, float xloc, float yloc, float scaleX, float scaleY);

	void drawImage(int image_id, float x, float y);

	void drawImageLayered(int image_id, float x, float y, float layer);

	void PrintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

	void List(const char* text, int row);

	void drawBaseGui(void);

	void drawBrowserPlayer(playbackInfo_t* info);

	void drawError(void);

	void fblist(int rows, int startpoint);
};
#endif
