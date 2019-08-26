#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <citro3d.h>
#include <citro2d.h>

#include "player.hpp"
#include "menu.hpp"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define MAX_LIST 7

namespace Gui {
	void Init(settings_t* settings);

	void Exit(void);

	C2D_Text StaticTextGen(std::string str);

	void Update(playbackInfo_t* musInfo);

	void CursorMove(int move);

	void CursorReset(void);

	int GetCursorPos(void);

	int GetSelPos(void);

	void ClearScreen(gfxScreen_t screen);

	void SetTarget(gfxScreen_t screen);

	void SetMenu(std::unique_ptr<Menu> menu);

	void BackMenu(void);

	void Print(const char* text, float xloc, float yloc, float scaleX, float scaleY);

	void PrintStatic(const std::string &ident, float xloc, float yloc, float scaleX, float scaleY);

	void drawImage(int image_id, float x, float y);

	void drawImageLayered(int image_id, float x, float y, float layer);

	void PrintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

	void List(const char* text, int row);

	void menuList(int cur, int from, float startpoint, float size, int rows);

	void drawBaseGui(void);

	void drawBrowserPlayer(playbackInfo_t* info);

	void drawError(void);

	void fblist(int rows, int startpoint);
};
#endif
