/*   LimePlayer3DS FOSS graphcal music player for the 3ds.
*    Copyright (C) 2018  LimePlayer Team
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
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#include "kbd.h"
#include "touch.h"
#include "debug.h"
#include "error.h"
#include "file.h"
#include "main.h"
#include "player.h"
#include "config.h"
#include "explorer.h"

static Thread thread = NULL;

const int MAX_LIST[2] = {18, 15};

void noDspFirmExit() {
	consoleInit(GFX_TOP, NULL);
	printf("/////////////FATAL///ERROR////////////////////////\x1b[03;00HFatal error: Ndsp could not be Initalized.\x1b[05;00HThis is probably because your dspfirm is missing.\x1b[07;00HYou can correct this by running dsp1. \x1b[08;00HThis works on both cfw (cia) and homebrew (3dsx). \x1b[10;00HFor homebrew, you may need to compile dsp1 yourself as the 3dsx file is not available on the release page. \x1b[30;00H//////////////////////////////////////////////////");
	DEBUG_printf("Fatal error! No dspfirm.cdc found.\n");
	while (aptMainLoop()) {
		hidScanInput();
		
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;
	}
	
	gfxExit();
	romfsExit();
	exit(22);
}

void initLime(void) {
	romfsInit();
	gfxInitDefault();
	if(ndspInit()) {
		noDspFirmExit();
	}
	else {
		initlimeGFX();
	}
	DEBUG("All libraries inatilized sucessfully.\n");
}

/**
 * Stop the currently playing file (if there is one) and play another file.
 *
 * \param	ep_file			File to play.
 * \param	playbackInfo	Information that the playback thread requires to
 *							play file.
 */
static int changeFile(const char* ep_file, playbackInfo_t* playbackInfo)
{
	s32 prio;

	if(ep_file != NULL && !FILE_getfiletype(ep_file))
	{
		*playbackInfo->errInfo->error = errno;
		svcSignalEvent(*playbackInfo->errInfo->failEvent);
		return -1;
	}

	/**
	 * If music is playing, stop it. Only one playback thread should be playing
	 * at any time.
	 */
	if(thread != NULL)
	{
		/* Tell the thread to stop playback before we join it */
		stopPlayback();

		threadJoin(thread, U64_MAX);
		threadFree(thread);
		thread = NULL;
	}

	if(ep_file == NULL || playbackInfo == NULL)
		return 0;

	playbackInfo->file = strdup(ep_file);
	printf("Playing: %s\n", playbackInfo->file);

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(playFile, playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

void closelime(playbackInfo_t *playbackInfo) {
	if (thread) {
		stopPlayback();
		changeFile(NULL, playbackInfo);
	}
}

//---------------------------------------------------------------------------------
int main(int argc, char* argv[]) {
//---------------------------------------------------------------------------------
	DEBUG_printf("Welcome <User>, please wait while things are prepared for you.\n");
	bool				menuIsActive = false;
	bool				curPlay = false;
	int				fileMax = MAX_LIST[curPlay];
	int				fileNum = 0;
	int				from = 0;
	int				error = 0;
	Handle				playbackFailEvent;
	errInfo_t			errInfo;
	musinfo_t			fileInfo;
	settings_t			fileSettings;
	playbackInfo_t			playbackInfo;
	dirList_t			dirList = {NULL, 0, NULL, 0, NULL};
	
	int menuSelect = 1;
	int menuMode = 0;
			
	// Init libs
	initLime();
	
	//Get settings
	if (!FILE_exist("/3ds/limeplayer3ds/config.json")) {
		mkdir("/3ds/limeplayer3ds", 0777);
		FILE_copy("romfs:/defaultcfg/config.json", "/3ds/limeplayer3ds/config.json");
	}
	CFG_parse("/3ds/limeplayer3ds/config.json", &fileSettings);
	
	//Initalize stuff.
	textinit(&fileSettings);
	aptSetSleepAllowed(false);
	osSetSpeedupEnable(true);

	
	//Initalize the error handling.
	errInfo.error = &error;
	errInfo.failEvent = &playbackFailEvent;
	errInfo.errstr = NULL;
	
	//Initalize file metadata display.
	fileInfo.songName	= NULL;
	fileInfo.authorCpright 	= NULL;
	fileInfo.description	= NULL;

	//Initalize player data.
	playbackInfo.file = NULL;
	playbackInfo.fileMeta = &fileInfo;
	playbackInfo.errInfo = &errInfo;
	playbackInfo.settings = &fileSettings;

	chdir("sdmc:/");
	chdir("MUSIC");
	getDir(&dirList);
	fileMax = getDir(&dirList);
	
	// Main loop
	while (aptMainLoop())
	{
		hidScanInput();
		// Respond to user input
		u32 kDown = hidKeysDown();
		u32 kHeld = hidKeysHeld();
		static u64	mill = 0;
		int t2k = touch2Key();
		
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu
			
		if(kDown)
			mill = osGetTime();
		if (!menuIsActive) {
			if(kHeld & KEY_L || t2k)
			{
				/* Pause/Play */
				if(kDown & (KEY_R | KEY_UP) || t2k == TOUCH_BUTTON_PLAY)
				{
					if(isPlaying() == false)
						continue;
	
					if(togglePlayback() == true)
						puts("Paused");
					else
						puts("Playing");
	
					continue;
				}
	
				/* Stop */
				if(kDown & KEY_B || t2k == TOUCH_BUTTON_STOP)
				{
					stopPlayback();
					changeFile(NULL, &playbackInfo);
					/* If the playback thread is currently playing, it will now
					 * stop and tell the Watchdog thread to display "Stopped".
					 */
					continue;
				}
			}
			
			//KEY_R will be used for seeking.
			if(kHeld & KEY_R || t2k)
			{
				if (kDown & KEY_RIGHT || t2k == TOUCH_BUTTON_SKPFW) {
					seekSectionTime(getCurrentTime()+15);
					kDown = false;
				}
				if (kDown & KEY_LEFT || t2k == TOUCH_BUTTON_SKPBW) {
					seekSectionTime(getCurrentTime()-15);
					kDown = false;
				}
			}
			
			if (error != 0 && thread != NULL) {
				stopPlayback();
				changeFile(NULL, &playbackInfo);
				//Stops playback if the file is done playing.
			}
			
			if ((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) && fileNum > 0) {
				fileNum--;
				/* 26 is the maximum number of entries that can be printed */
				if(getNumberFiles() - fileNum > MAX_LIST[curPlay]-1 && from != 0)
					from--;
			}
			
			if ((kDown & KEY_DOWN || ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) && fileNum < fileMax) {
				fileNum++;
				if(fileNum >= MAX_LIST[curPlay] && getNumberFiles() - fileNum >= 0 && from < fileMax - MAX_LIST[curPlay])
					from++;
			}
			
			if((kDown & KEY_RIGHT || ((kHeld & KEY_RIGHT) && (osGetTime() - mill > 500))) && fileNum < fileMax)
			{
				int skip = fileMax - fileNum;
	
				if(skip > MAX_LIST[curPlay] / 2)
					skip = MAX_LIST[curPlay] / 2;
	
				fileNum += skip;
	
				if(fileNum >= MAX_LIST[curPlay] && fileMax - fileNum >= 0 &&
						from < fileMax - MAX_LIST[curPlay])
				{
					from += skip;
					if(from > fileMax - MAX_LIST[curPlay])
						from = fileMax - MAX_LIST[curPlay];
				}
			}
			
			if((kDown & KEY_LEFT || ((kHeld & KEY_LEFT) && (osGetTime() - mill > 500))) && fileNum > 0)
			{
				int skip = MAX_LIST[curPlay] / 2;
	
				if(fileNum < skip)
					skip = fileNum;
	
				fileNum -= skip;
	
				/* 26 is the maximum number of entries that can be printed */
				/* TODO: Not using MAX_LIST here? */
				if(fileMax - fileNum > 26 && from != 0)
				{
					from -= skip;
					if(from < 0)
						from = 0;
				}
			}
				
			if((kDown & KEY_B) || ((kDown & KEY_A) && (from == 0 && fileNum == 0)))
			{
				chdir("..");
				getDir(&dirList);
				fileMax = getNumberFiles();
				fileNum = 0;
				from = 0;
			}
			
				
			if (kDown & KEY_A && fileNum > 0) {
				
				if(dirList.dirNum >= fileNum)
				{
					chdir(dirList.directories[fileNum - 1]);
					getDir(&dirList);
					fileMax = getNumberFiles();
					fileNum = 0;
					from = 0;
				}
				
				if(dirList.dirNum < fileNum)
				{
				changeFile(dirList.files[fileNum - dirList.dirNum - 1], &playbackInfo);
					if (((fileNum)-from) > 15) {
					fileNum-=((fileNum)-from) - 15;
					}
				}
			}
			
			if (error > 0) {
				menuIsActive = 1;
				menuMode = 5;
			}
		}
		
		if (menuIsActive) {
				
			if(kDown & KEY_RIGHT && menuSelect < 4 && menuMode == 0) {
				menuSelect++;
			}
			if(kDown & KEY_LEFT && menuSelect > 1 && menuMode == 0) {
				menuSelect--;
			}
			
			if (kDown & KEY_A && menuSelect == 1 && menuMode == 0) {
				menuMode = 1;
				kDown = false;
			}
			else if (kDown & KEY_A && menuSelect == 2 && menuMode == 0) {
				menuMode = 2;
				kDown = false;
			}
			else if (kDown & KEY_A && menuSelect == 3 && menuMode == 0) {
				menuMode = 3;
				kDown = false;
			}
			else if (kDown & KEY_A && menuSelect == 4 && menuMode == 0) { 
				menuMode = 4;
				kDown = false;
			}
				
			if(kDown & KEY_UP && menuMode > 0) {
				menuSelect--;
			}
			else if(kDown & KEY_DOWN && menuMode > 0) {
				menuSelect++;
			}

			if (menuMode == 1) {
				if (kDown & KEY_A && menuSelect == 2) { //Deprecated way of changing file. It is kept for unrelated reasons (something something citra and older amd graphics cards).
					getTextFromKbd("Enter path to midi config.", playbackInfo.settings->wildMidiConfig, sizeof(playbackInfo.settings->wildMidiConfig));
				}
			}
			
			if (menuMode == 2);
			
			if (menuMode == 3);
			
			if (menuMode == 4);
			
			if (menuMode == 5) {
				if (kDown & KEY_B && menuMode >= 5) {
					menuMode = 0;
					menuSelect = 1;
					menuIsActive = 0;
					error = 0;
				}
			}
			
		}
		
		if (kDown & KEY_SELECT) {
			menuMode = 0;
			menuSelect = 1;
			menuIsActive = !menuIsActive;
		}
		
		// Render the scene
		startframe();
		drawBaseGui();
		if (!menuIsActive) {
			drawList(fileMax+2, from, 20, fileNum, dirList);
			if (thread != NULL) {
				drawControls(&playbackInfo);
				curPlay = true;
			}
			else
				curPlay = false;
			}
		
		if (menuIsActive) {
			if (menuMode == 0)
				drawMenu(menuSelect);
			else
				drawMenuPartTwo(menuMode, menuSelect);
			
			if (kDown & KEY_B && menuMode == 0) {
				menuMode = 0;
				menuSelect = 1;
				menuIsActive = false;
			}
			else if (kDown & KEY_B && menuMode > 0 && menuMode < 5) {
				menuMode = 0;
				menuSelect = 1;
			}
			
			if (menuMode == 1)
				//drawSettingsMenu(menuSelect);
			
			if (menuMode == 2);
			
			if (menuMode == 3);
			
			if (menuMode == 4);
			
			if (menuMode == 5) {
				drawMenuErr(errInfo);
			}
		}
		endframe();
	}
	
	DEBUG_printf("Shuting down.\n");
	CFG_clean(&fileSettings);
	closelime(&playbackInfo); //Make sure all threads are killed before exiting.
	closelimeGFX();
	ndspExit();
	romfsExit();
	return 0;
}

