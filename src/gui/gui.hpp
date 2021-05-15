#ifndef __GUICTR_LIME_2D__
#define __GUICTR_LIME_2D__

#include <string>

#include <citro3d.h>
#include <citro2d.h>

#include "player.hpp"
#include "menu.hpp"
#include "color.hpp"

// Generated at build time
#include "sprites.h"

#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#define BOT_SCREEN_WIDTH 320

namespace Gui {
	void Init(settings_t* settings);

	void Exit(void);

	void Update(playbackInfo_t* musInfo);

	void SetTarget(gfxScreen_t screen);

	void ClearScreen(gfxScreen_t screen);

	void SetMenu(std::unique_ptr<Menu> menu);

	void BackMenu(void);

	C2D_Text StaticTextGen(const std::string& str);

	void Print(const std::string& text, float xloc, float yloc, float scaleX, float scaleY);

	void PrintStatic(const std::string& ident, float xloc, float yloc, float scaleX, float scaleY);

	void DrawImage(int image_id, float x, float y);

	void DrawImageLayered(int image_id, float x, float y, float layer);

	void DrawImage(C2D_Image& image, float x, float y);

	void DrawImageLayered(C2D_Image& image, float x, float y, float layer);

	void DrawSolidRectangle(float x, float y, float w, float h, u32 color);

	void PrintColor(const std::string& text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

	void DrawBaseGui(void);

	void DrawError(void);
};
#endif
