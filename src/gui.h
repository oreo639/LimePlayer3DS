#ifndef __GUIC_LIME_2D__
#define __GUIC_LIME_2D__
#include <citro2d.h>
#include "main.h"
#include "player.h"

#define MAX_SPRITES   768
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240

#define PLAYER_INTERFACE 0
#define PLAYING_ICON 1
#define SPEAKER_MUTE 2
#define SPEAKER_QUIET 3
#define SPEAKER_SOFT 4
#define SPEAKER_MEDIUM 5
#define SPEAKER_LOUD 6
#define SPEAKER_BACKGROUND 7
#define MENU_SELECT 8
#define MENU_SETTINGS 9
#define MENU_FOLDER 10
#define MENU_GLOBE 11
#define MENU_CREDITS 12
#define CONTROL_PLAY 13
#define CONTROL_PAUSE 14
#define CONTROL_STOP 15
#define CONTROL_SKPFW 16
#define CONTROL_SKPBW 17
#define CONTROL_SKIP 18

int initlimeGFX(void);

void closelimeGFX(void);

void textinit(settings_t* settings);

void startframe(void);

void endframe(void);

void guiprint(const char* text, float xloc, float yloc, float scaleX, float scaleY);

void guiprintColor(const char* text, float xloc, float yloc, float scaleX, float scaleY, u32 color);

void guilist(const char* text, int row);

void drawBaseGui(void);

void drawList(int rows, int from, int max, int select, dirList_t dirList);

void drawMenu(int select);

void drawMenuPartTwo(int mode, int select);

void drawMenuErr(errInfo_t errInfo);

void menuList(int select, int rows);

void drawControls(const playbackInfo_t* info);

void draw_text_wrap_scaled(const char * text, float x, float y, u32 color, float max_scale, float min_scale, float max_width);
#endif
