#include <time.h>

#include "gui.h"
#include "error.h"
#include "lang.h"

#define MENU_ICON_HORIZONTAL 100

C3D_RenderTarget* top;
C3D_RenderTarget* bot;
C2D_SpriteSheet spriteSheet;
C2D_Sprite* sprite;
C2D_TextBuf g_staticBuf, g_dynamicBuf;

C2D_Text staticText[TEXT_MAX];

static u8 preVol;
static u64 trigeredTime = 0;
static int s_textLang = CFG_LANGUAGE_ES;

static int listDir(int from, int max, int select, dirList_t dirList)
{
	int				fileNum = 0;
	int				listed = 0;
	char				filename[100];
	
	printf("\033[0;0H");
	snprintf(filename, 100, "Dir: %s\n", dirList.currentDir);
	guilist(filename, 0);

	if(from == 0)
	{
		snprintf(filename, 100, "%c../\n", select == 0 ? '>' : ' ');
		guilist(filename, 1);
		//printf("\33[2K%c../\n", select == 0 ? '>' : ' ');
		listed++;
		max--;
	}

	while(dirList.fileNum + dirList.dirNum > fileNum)
	{
		fileNum++;

		if(fileNum <= from)
			continue;

		listed++;

		if((dirList.dirNum >= fileNum) && !((fileNum+1)-from > max))
		{
			snprintf(filename, 100, "%c%s/\n", select == fileNum ? '>' : ' ', dirList.directories[fileNum - 1]);
			guilist(filename, (fileNum+1)-from);

		}

		/* fileNum must be referring to a file instead of a directory. */
		if((dirList.dirNum < fileNum) && !((fileNum+1)-from > max))
		{
			snprintf(filename, 100, "%c%s\n", select == fileNum ? '>' : ' ', dirList.files[fileNum - dirList.dirNum - 1]);
			guilist(filename, (fileNum+1)-from);

		}

		if(fileNum == max + from)
			break;
	}

	return listed;
}

int initlimeGFX(void)
{
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	//consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);
	bot = C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT);
	
	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);
	//spriteFromPng("romfs:/icons8-music-128.png", &sprite);
	
	if (!top && !bot && !spriteSheet)
		return 1;
		
	else 
		return 0;

}

void closelimeGFX(void)
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

void startframe(void)
{
	C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
	C2D_TargetClear(top, C2D_Color32(20, 29, 31, 255));
	C2D_TargetClear(bot, C2D_Color32(20, 29, 31, 255));
}

void endframe(void) 
{
	C3D_FrameEnd(0);
	C2D_TextBufClear(g_dynamicBuf);
}

const char* textGetString(Text_id id)
{
	const char* str = g_strings[id][s_textLang];
	if (!str) str = g_strings[id][CFG_LANGUAGE_EN];
	return str;
}

void textload(Text_id id) {
	C2D_TextParse(&staticText[id], g_staticBuf, textGetString(id));
	C2D_TextOptimize(&staticText[id]);
}

void textinit(settings_t* settings)
{
	s_textLang = settings->textLang;
	//Only use system language if specified.
	if (s_textLang == CFG_LANGUAGE_DEFAULT) {
		Result res = cfguInit();
		if (R_SUCCEEDED(res))
		{
			u8 lang;
			res = CFGU_GetSystemLanguage(&lang);
			if (R_SUCCEEDED(res))
				s_textLang = lang;
			cfguExit();
		}
	}

	// Create two text buffers: one for static text, and another one for
	// dynamic text - the latter will be cleared at each frame.
	g_staticBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	g_dynamicBuf = C2D_TextBufNew(4096); // support up to 4096 glyphs in the buffer
	textload(TEXT_LOADING_GENERIC);
	textload(TEXT_LOADING_INFO);
	textload(TEXT_MENU_SETTINGS);
	textload(TEXT_MENU_MODE);
	textload(TEXT_MENU_INTERNET);
	textload(TEXT_MENU_CREDITS);
	textload(TEXT_CREDITS_MAIN);
		
}

void guiprintStatic(Text_id id, float xloc, float yloc, float scaleX, float scaleY)
{
	C2D_DrawText(&staticText[id], C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, 0xFFFFFFFF);
}

void guiprintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);

	// Generate and draw dynamic text
	C2D_Text dynText;
	C2D_TextParse(&dynText, g_dynamicBuf, text);
	C2D_TextOptimize(&dynText);
	C2D_DrawText(&dynText, C2D_WithColor, xloc, yloc, 0.5f, scaleX, scaleY, color);
}

void guiprint(const char* text, float xloc, float yloc, float scaleX, float scaleY)
{
	guiprintColor(text, xloc, yloc, scaleX, scaleY, 0xFFFFFFFF);
}


void guilist(const char* text, int row)
{
	guiprintColor(text, 8.0f, row*12, 0.4f, 0.4f, 0xFF000000);
}

static void drawSprite(int image_id, float x, float y)
{
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, 0.6f, NULL, 1.0f, 1.0f);
}
static void drawImageLayered(int image_id, float x, float y, float layer)
{
    C2D_DrawImageAt(C2D_SpriteSheetGetImage(spriteSheet, image_id), x, y, layer, NULL, 1.0f, 1.0f);
}

void volumeIndicator(u8 volume){
	int indicator = (volume/15)+2;
	drawImageLayered(7, 120, 30, 0.7f);
	drawImageLayered(indicator, 120, 30, 0.7f);
}

void drawBaseGui(void) {
	u8 curVol;
	bool twfohourtime = false;
	time_t rawtime = time(NULL);
	struct tm *timeinfo = localtime(&rawtime);
	HIDUSER_GetSoundVolume(&curVol);
	
	C2D_SceneBegin(top);
	C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
	
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
	sprintf(string_volume, "%d", curVol);
	guiprintColor(string_volume, 1, 1, 0.5f, 0.5f, 0xFFFFFFFF);
	//volumeIndicator(curVol);
	if(curVol != preVol) {
		trigeredTime = osGetTime();	
	} 
	if (osGetTime() < trigeredTime + 1000){
		volumeIndicator(curVol);
	}
	preVol = curVol;
}

void drawList(int rows, int from, int max, int select, dirList_t dirList) {
	C2D_SceneBegin(bot);
	if (rows < 1)
		return;
	int i = 0;
	bool color = true;
	while (rows > i && i < max) {
		if (color)
			C2D_DrawRectSolid(0, i*12, 0.5f, SCREEN_WIDTH, 12, C2D_Color32(23, 100, 64, 255));
		else if (!color)
			C2D_DrawRectSolid(0, i*12, 0.5f, SCREEN_WIDTH, 12, C2D_Color32(43, 191, 63, 255));
	color = !color;
	i++;
	}
	listDir(from, max, select, dirList);
}

void drawControls(const playbackInfo_t* info) {
	C2D_SceneBegin(bot);
	drawSprite(PLAYER_INTERFACE, 20, 203);
	
	drawSprite(CONTROL_SKPBW, 58, 203);
	drawSprite(CONTROL_STOP, 100, 203);
	if (isPaused()) {
		drawSprite(CONTROL_PLAY, 145, 203);
	} else {
		drawSprite(CONTROL_PAUSE, 145, 203);
	}
	drawSprite(CONTROL_SKIP, 187, 203);
	drawSprite(CONTROL_SKPFW, 229, 203);
	
	
	C2D_SceneBegin(top);
	drawSprite(PLAYING_ICON, 20, 15);
	if (info->file != NULL) {
		guiprint(info->file, 150.0f, 20.0f, 0.5f, 0.5f);
	} else {
		guiprintStatic(TEXT_LOADING_GENERIC, 150.0f, 20.0f, 0.5f, 0.5f);
	}
	
	if (info->fileMeta->authorCpright != NULL) {
		guiprint(info->fileMeta->authorCpright, 150.0f, 40.0f, 0.5f, 0.5f);
		//draw_text_wrap_scaled(info->fileMeta->authorCpright, 150.0f, 40.0f, 0xFFFFFFFF, 0.5f, 0.5f, 60.0f); 
			//Kind of a quick solution for textwrap. TODO::Improve this.
	} else {
		guiprintStatic(TEXT_LOADING_INFO, 150.0f, 40.0f, 0.5f, 0.5f);
	}
	char posstr[50];
	snprintf(posstr, 50, "Currrent pos: %ld/%ld", getCurrentTime(), getTotalTime());
	guiprint(posstr, 100.0f, 170.0f, 0.5f, 0.5f);
}

void drawMenu(int select) {
	drawBaseGui();
	C2D_SceneBegin(bot);
	if (select == 1)
		drawSprite(MENU_SELECT, 20, MENU_ICON_HORIZONTAL);
	else if (select == 2)
		drawSprite(MENU_SELECT, 80, MENU_ICON_HORIZONTAL);
	else if (select == 3)
		drawSprite(MENU_SELECT, 140, MENU_ICON_HORIZONTAL);
	else if (select == 4)
		drawSprite(MENU_SELECT, 200, MENU_ICON_HORIZONTAL);
	drawSprite(MENU_SETTINGS, 20, MENU_ICON_HORIZONTAL);
	drawSprite(MENU_FOLDER, 80, MENU_ICON_HORIZONTAL);
	drawSprite(MENU_GLOBE, 140, MENU_ICON_HORIZONTAL);
	drawSprite(MENU_CREDITS, 200, MENU_ICON_HORIZONTAL);
	//guiprint("Menu mode.", 150.0f, 40.0f, 0.5f, 0.5f);
	C2D_SceneBegin(top);
	if (select == 1)
		guiprintStatic(TEXT_MENU_SETTINGS, 20.0f, 20.0f, 0.5f, 0.5f);
	else if (select == 2)
		guiprintStatic(TEXT_MENU_MODE, 20.0f, 20.0f, 0.5f, 0.5f);
	else if (select == 3)
		guiprintStatic(TEXT_MENU_INTERNET, 20.0f, 20.0f, 0.5f, 0.5f);
	else if (select == 4) 
		guiprintStatic(TEXT_MENU_CREDITS, 20.0f, 20.0f, 0.5f, 0.5f);
}

void drawMenuPartTwo(int mode, int select) {
	drawBaseGui();
	C2D_SceneBegin(top);
	if (mode == 1)
		menuList(select, 12);
	else if (mode == 4)
		guiprintStatic(TEXT_CREDITS_MAIN, 20.0f, 20.0f, 0.5f, 0.5f);
	C2D_SceneBegin(bot);

}

void drawMenuErr(errInfo_t errInfo) {
	drawBaseGui();
	C2D_SceneBegin(top);
	if (*errInfo.error == FILE_NOT_SUPPORTED) {
		guiprint("ERR: Unrecognized filetype.", 20.0f, 20.0f, 0.5f, 0.5f);
	}
	else if (*errInfo.error == DECODER_INIT_FAIL){
		guiprint("ERR: Failed to initalize decoder.", 20.0f, 20.0f, 0.5f, 0.5f);
	}
	else {
		char errstr[30];
		snprintf(errstr, 30, "ERR: Undefined error (%d)", *errInfo.error);
		guiprint(errstr, 20.0f, 20.0f, 0.5f, 0.5f);
	}
	C2D_SceneBegin(bot);

}

void drawSettingsMenu(int select) {
	guilist("Midi Config", 1);
}

void menuList(int select, int rows) {
	C2D_SceneBegin(bot);
	if (rows < 1)
		return;
	int i = 0;
	bool color = true;
	while (rows > i && i < 20) {
		if (select == i)
			C2D_DrawRectSolid(0, i*12, 0.5f, SCREEN_WIDTH, 12, C2D_Color32(255, 255, 2, 255));
		else if (color)
			C2D_DrawRectSolid(0, i*12, 0.5f, SCREEN_WIDTH, 12, C2D_Color32(23, 100, 64, 255));
		else if (!color)
			C2D_DrawRectSolid(0, i*12, 0.5f, SCREEN_WIDTH, 12, C2D_Color32(43, 191, 63, 255));
	color = !color;
	i++;
	}
}

/*Text wrap code. Based on the work by HM and the Anemone team.*/
static void get_text_dimensions(const char * text, float scaleX, float scaleY, float * width, float * height)
{
	// Clear the dynamic text buffer
	C2D_TextBufClear(g_dynamicBuf);
	
	C2D_Text c2d_text;
	C2D_TextParse(&c2d_text, g_dynamicBuf, text);
	C2D_TextGetDimensions(&c2d_text, scaleX, scaleY, width, height);
}

