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
#include <stdio.h>
#include <time.h>

#include "gui.hpp"
#include "app.hpp"
#include "error.hpp"

// Generated at build time
#include "sprites.h"

#define MENU_ICON_HORIZONTAL 100

C3D_RenderTarget* top;
C3D_RenderTarget* bot;
C2D_SpriteSheet spriteSheet;
C2D_Sprite* sprite;
C2D_TextBuf g_staticBuf, g_dynamicBuf;

//C2D_Text staticText[TEXT_MAX];

static u8 preVol;
static u64 trigeredTime = 0;

int cursor = 0;
int seloffs = 0;

static void menuList(int cur, int from, float startpoint, float size, int rows);
static void filelist(int cur, int from, dirList_t dirList);

Gui::Gui(void) {
	InitlimeGFX();
	textinit();
}

Gui::~Gui(void) {
	CloselimeGFX();
}

void Gui::guilist(const char* text, int row)
{
	guiprintColor(text, 8.0f, row*12, 0.4f, 0.4f, 0xFF000000);
}

void Gui::fblist(int rows, int startpoint)
{
	for (int i = 0; rows-seloffs > i && i <= MAX_LIST; i++) {
		if (seloffs+i < App::dirList.dirnum)
			guiprintColor(App::dirList.directories[seloffs+i].c_str(), 8.0f, i*32.5f+startpoint, 0.4f, 0.4f, 0xFF000000);
		else if (seloffs+i < App::dirList.total) {
			guiprintColor(App::dirList.files[seloffs+i-App::dirList.dirnum].c_str(), 8.0f, i*32.5f+startpoint, 0.4f, 0.4f, 0xFF000000);
		}
	}
}


void Gui::Drawui(playbackInfo_t* playbackInfo)
{
	startframe();
	if (App::appState == App::MENU) {
		drawBaseGui();
		if (PlayerInterface::IsPlaying()) {
			drawBrowserPlayer(playbackInfo);
		}
		C2D_SceneBegin(bot);
		menuList(cursor, seloffs, 15, 32.5f, App::dirList.total);
		C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
		guilist(App::dirList.currentDir.c_str(), 0);
		fblist(App::dirList.total, 15);
	}
	else if (App::appState == App::LOGO) {
		C2D_SceneBegin(top);
		C2D_SceneBegin(bot);
	}
	
	if (App::Error != 0) {
		drawError();
	}
	endframe();
}

int Gui::InitlimeGFX(void)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();

	// Create screens
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	
	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);
	
	if (!top && !bot && !spriteSheet)
		return 1;
		
	else 
		return 0;

}

void Gui::CloselimeGFX(void)
{
	// Delete spritesheet
	C2D_SpriteSheetFree(spriteSheet);
	
	C2D_TextBufDelete(g_dynamicBuf);
	C2D_TextBufDelete(g_staticBuf);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
}

void Gui::GuiCursorMove(int move) {
	if(move < 0 && (App::dirList.total - cursor >= MAX_LIST && seloffs != 0))
					seloffs = seloffs+move;
	else if(move > 0 && (cursor >= MAX_LIST-1 && App::dirList.total - cursor > 0 && seloffs < App::dirList.total - MAX_LIST))
					seloffs = seloffs+move;
	if (cursor + move < 0) {
		cursor = 0;
		seloffs = 0;
	} else if (cursor + move > App::dirList.total-1)
		cursor = App::dirList.total-1;
	else
		cursor = cursor + move;
}

void Gui::GuiCursorReset(void) {
	cursor = 0;
	seloffs = 0;
}

int Gui::GuiGetCursorPos(void) {
	return cursor;
}

void Gui::startframe(void)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, C2D_Color32(20, 29, 31, 255));
	C2D_TargetClear(bot, C2D_Color32(20, 29, 31, 255));
}

void Gui::endframe(void) 
{
	C3D_FrameEnd(0);
	C2D_TextBufClear(g_dynamicBuf);
}

void Gui::textinit(void)
{
	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	//textload(TEXT_LOADING_GENERIC);
	//textload(TEXT_LOADING_INFO);
	//textload(TEXT_MENU_SETTINGS);
	//textload(TEXT_MENU_MODE);
	//textload(TEXT_MENU_INTERNET);
	//textload(TEXT_MENU_CREDITS);
	//textload(TEXT_CREDITS_MAIN);
		
}

void Gui::guiprintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Generate and draw dynamic text
	C2D_Text dynText;
	C2D_TextParse(&dynText, g_dynamicBuf, text);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, color);
}

void Gui::guiprint(const char* text, float xloc, float yloc, float scaleX, float scaleY)
{
	guiprintColor(text, xloc, yloc, scaleX, scaleY, 0xFFFFFFFF);
}

/*static void guiprintStatic(Text_id id, float xloc, float yloc, float scaleX, float scaleY)
{
	C2D_DrawText(&staticText[id], C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, 0xFFFFFFFF);
}*/

static void drawImage(int image_id, float x, float y)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, 0.6f, NULL, 1.0f, 1.0f);
}
static void drawImageLayered(int image_id, float x, float y, float layer)
{
	C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, layer, NULL, 1.0f, 1.0f);
}

static void volumeIndicator(u8 volume){
	int indicator = (volume/15);
	drawImageLayered(sprites_popup_vol_bkg_idx, 120, 30, 0.7f);
	drawImageLayered(indicator + sprites_popup_vol0_idx, 120, 30, 0.7f);
}

static void menuList(int cur, int from, float startpoint, float size, int rows) {
	C2D_SceneBegin(bot);
	if (rows < 1)
		return;
	int i = 0;
	bool color = true;
	for (i = 0; rows-from > i && i < MAX_LIST; i++) {
		if (cur-from == i)
			C2D_DrawRectSolid(0, i*size+startpoint, 0.5f, SCREEN_WIDTH, size, C2D_Color32(255, 255, 2, 255));
		else if (color)
			C2D_DrawRectSolid(0, i*size+startpoint, 0.5f, SCREEN_WIDTH, size, C2D_Color32(23, 100, 64, 255));
		else if (!color)
			C2D_DrawRectSolid(0, i*size+startpoint, 0.5f, SCREEN_WIDTH, size, C2D_Color32(43, 191, 63, 255));
	color = !color;
	}
}


void Gui::drawBaseGui(void) {
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

	guiprintColor(string_hours, 358, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	if(timeinfo->tm_sec % 2 == 1)
		guiprintColor(":", 375, 0, 0.5f, 0.5f, 0xFFFFFFFF);
	guiprintColor(string_minutes, 380, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	
	char string_volume[4] = {0};
	sprintf(string_volume, "%d", (int)((curVol/63.0f)*100));
	guiprintColor(string_volume, 1, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	if(curVol != preVol) {
		trigeredTime = osGetTime();	
	} 
	if (osGetTime() - trigeredTime < 1000){
		volumeIndicator(curVol);
	}
	preVol = curVol;
}

void Gui::drawBrowserPlayer(playbackInfo_t* info) {
	drawImage(sprites_player_interface_idx, 20, 15);
	if (!info->filename.empty()) {
		guiprint(info->filename.c_str(), 150.0f, 20.0f, 0.5f, 0.5f);
	} else {
		guiprint("Loading...", 150.0f, 20.0f, 0.5f, 0.5f);
	}

	if (!info->fileMeta.authorCpright.empty()) {
		guiprint(info->fileMeta.authorCpright.c_str(), 150.0f, 40.0f, 0.5f, 0.5f);
	} else {
		guiprint("Loading...", 150.0f, 40.0f, 0.5f, 0.5f);
	}
}

void Gui::drawError(void) {
	C2D_DrawRectSolid(10, 10, 0.5f, 300, SCREEN_HEIGHT-20, C2D_Color32(0, 0, 0, 255));
	
	if (App::Error == FILE_NOT_SUPPORTED) {
		guiprint("ERR: Unrecognized filetype.", 20.0f, 20.0f, 0.5f, 0.5f);
	}
	else if (App::Error == DECODER_INIT_FAIL){
		guiprint("ERR: Failed to initalize decoder.", 20.0f, 20.0f, 0.5f, 0.5f);
	}
	else {
		char errstr[30];
		snprintf(errstr, 30, "ERR: Undefined error (%d)", App::Error);
		guiprint(errstr, 20.0f, 20.0f, 0.5f, 0.5f);
	}
}
