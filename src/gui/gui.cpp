/*   LimePlayer3DS FOSS graphcal music player for the Nintendo 3DS.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <vector>
#include <stack>

#include <stdio.h>
#include <time.h>

#include <citro2d.h>

#include "gui.hpp"
#include "app.hpp"
#include "error.hpp"
#include "debug.hpp"
#include "i18n.hpp"

// Generated at build time
#include "sprites.h"

std::stack<std::unique_ptr<Menu>> menus;

C3D_RenderTarget* top;
C3D_RenderTarget* bot;
C2D_SpriteSheet spriteSheet;
C2D_Sprite* sprite;
C2D_TextBuf g_staticBuf, g_dynamicBuf;

static u8 preVol;
static u64 trigeredTime = 0;

settings_t *settings_ptr;

void Gui::Init(settings_t* settings) {
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screens
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	
	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet)
		svcBreak(USERBREAK_PANIC);

	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	i18n::Init();

	settings_ptr = settings;
}

void Gui::Exit(void) {
	DEBUG("Gui is exiting.\n");
	while (!menus.empty())
		Gui::BackMenu();

	i18n::Exit();
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);

	// Delete spritesheet
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void Gui::Update(playbackInfo_t* playbackInfo)
{
	hidScanInput();
	u32 kDown = hidKeysDown();

	if (kDown & KEY_START)
		App::exit = true;

	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

	Gui::ClearScreen(GFX_TOP);
	Gui::ClearScreen(GFX_BOTTOM);

	Gui::SetTarget(GFX_TOP);
	menus.top()->doTopDraw();

	Gui::SetTarget(GFX_BOTTOM);
	menus.top()->doBottomDraw();

	if (Error::IsQuered()) {
		Gui::DrawError();
	}

	C3D_FrameEnd(0);
	C2D_TextBufClear(g_dynamicBuf);

	if (Error::IsQuered()) {
		if (kDown)
			Error::Remove();
		return;
	}

	touchPosition touch;
	hidTouchRead(&touch);
	menus.top()->doUpdate(&touch);
}

void Gui::SetTarget(gfxScreen_t screen)
{
	if (screen == GFX_BOTTOM)
		C2D_SceneBegin(bot);
	else
		C2D_SceneBegin(top);
}

void Gui::ClearScreen(gfxScreen_t screen)
{
	if (screen == GFX_BOTTOM)
		C2D_TargetClear(bot, C2D_Color32(20, 29, 31, 255));
	else
		C2D_TargetClear(top, C2D_Color32(20, 29, 31, 255));
}

void Gui::SetMenu(std::unique_ptr<Menu> menu)
{
    menus.push(std::move(menu));
}

void Gui::BackMenu(void)
{
    menus.pop();
}

C2D_Text Gui::StaticTextGen(const std::string& str) {
	C2D_Text tmpStaticText;
	C2D_TextParse(&tmpStaticText, g_staticBuf, str.c_str());
	C2D_TextOptimize(&tmpStaticText);
	return tmpStaticText;
}

void Gui::PrintColor(const std::string& text, float xloc, float yloc, float scaleX, float scaleY, u32 color)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Generate and draw dynamic text
	C2D_Text dynText;
	C2D_TextParse(&dynText, g_dynamicBuf, text.c_str());
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, color);
}

void Gui::Print(const std::string& text, float xloc, float yloc, float scaleX, float scaleY)
{
	Gui::PrintColor(text, xloc, yloc, scaleX, scaleY, 0xFFFFFFFF);
}

void Gui::PrintStatic(const std::string &ident, float xloc, float yloc, float scaleX, float scaleY)
{
	C2D_Text* tempSt = i18n::LocalizeStatic(settings_ptr->language, ident);
	if (tempSt)
		C2D_DrawText(tempSt, C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, 0xFFFFFFFF);
	else
		Gui::Print("GuiPrintStatic: Invalid string.", xloc, yloc, scaleX, scaleY);
}

void Gui::DrawImage(int image_id, float x, float y)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, 0.5f, NULL, 1.0f, 1.0f);
}

void Gui::DrawImageLayered(int image_id, float x, float y, float layer)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, layer, NULL, 1.0f, 1.0f);
}

void Gui::DrawImage(C2D_Image& image, float x, float y)
{
	C2D_DrawImageAt(image, x, y, 0.5f, NULL, 1.0f, 1.0f);
}

void Gui::DrawImageLayered(C2D_Image& image, float x, float y, float layer)
{
	C2D_DrawImageAt(image, x, y, layer, NULL, 1.0f, 1.0f);
}

void Gui::DrawSolidRectangle(float x, float y, float w, float h, u32 color)
{
	C2D_DrawRectSolid(x, y, 0.5f, w, h, color);
}

static void volumeIndicator(u8 volume) {
	int indicator = (volume/15);
	if (indicator < 6) {
		Gui::DrawImageLayered(sprites_popup_vol_bkg_idx, 120, 30, 0.6f);
		Gui::DrawImageLayered(indicator + sprites_popup_vol0_idx, 120, 30, 0.6f);
	}
}

void Gui::DrawBaseGui(void) {
	u8 curVol;
	bool twfohourtime = false;
	time_t rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	HIDUSER_GetSoundVolume(&curVol);
	
	C2D_SceneBegin(top);
	C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 20, C2D_Color32(119, 131, 147, 255));
	
	char string_hours[12] = {0};
	if (twfohourtime || timeinfo->tm_hour < 12) {
		sprintf(string_hours, "%.2i", timeinfo->tm_hour);
	} else {
		sprintf(string_hours, "%.2i", timeinfo->tm_hour-12);
	}

	char string_minutes[3] = {0};
	sprintf(string_minutes, "%.2i", timeinfo->tm_min);

	Gui::PrintColor(string_hours, 358, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	if(timeinfo->tm_sec % 2 == 1)
		Gui::PrintColor(":", 375, 0, 0.5f, 0.5f, 0xFFFFFFFF);
	Gui::PrintColor(string_minutes, 380, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	
	char string_volume[4] = {0};
	sprintf(string_volume, "%d", (int)((curVol/63.0f)*100));
	Gui::PrintColor(string_volume, 1, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	if(curVol != preVol) {
		trigeredTime = osGetTime();	
	} 
	if (osGetTime() - trigeredTime < 1000){
		volumeIndicator(curVol);
	}
	preVol = curVol;
}

void Gui::DrawError(void) {
	LimeError_t error = Error::Get();
	int errorcode = error.err_code;
	char codestr[30];

	C2D_DrawRectSolid(10, 10, 0.5f, 300, SCREEN_HEIGHT-20, C2D_Color32(0, 0, 0, 255));

	snprintf(codestr, 30, "Error code: %d", errorcode);
	Gui::Print(codestr, 20.0f, 20.0f, 0.5f, 0.5f);
	
	if (errorcode == FILE_NOT_SUPPORTED) {
		Gui::Print("ERR: Unrecognized filetype.", 20.0f, 40.0f, 0.5f, 0.5f);
	}
	else if (errorcode == DECODER_INIT_FAIL){
		Gui::Print("ERR: Failed to initalize decoder.", 20.0f, 40.0f, 0.5f, 0.5f);
	}
	else if (errorcode == ERRORH_GENERIC);
	else {
		Gui::Print("ERR: Undefined error.", 20.0f, 40.0f, 0.5f, 0.5f);
	}

	if (error.extra_info.size())
		Gui::Print(error.extra_info.c_str(), 20.0f, 60.0f, 0.5f, 0.5f);
}
