#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <citro3d.h>
#include <citro2d.h>

#include "player.hpp"
#include "menu.hpp"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240
#define MAX_LIST 15

namespace Gui {
	void Init(settings_t* settings);

	void Exit(void);

	C2D_Text StaticTextGen(std::string str);

	void Update(playbackInfo_t* musInfo);

	void ClearScreen(gfxScreen_t screen);

	void SetTarget(gfxScreen_t screen);

	void SetMenu(std::unique_ptr<Menu> menu);

	void BackMenu(void);

	void Print(const char* text, float xloc, float yloc, float scaleX, float scaleY);

	void PrintStatic(const std::string &ident, float xloc, float yloc, float scaleX, float scaleY);

	void drawImage(int image_id, float x, float y);

	void drawImageLayered(int image_id, float x, float y, float layer);

	void PrintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

	void drawBaseGui(void);

	void drawBrowserPlayer(playbackInfo_t* info);

	void drawError(void);
};
#endif
