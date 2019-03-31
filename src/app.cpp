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
#include <iostream>
#include <fstream>
#include <filesystem>

#include <sys/stat.h>
#include <stdio.h>

#include "app.hpp"
#include "gui.hpp"
#include "error.hpp"
#include "explorer.hpp"
#include "player.hpp"
#include "file.hpp"

Gui *gui;

App::AppState App::appState = INIT;
dirList_t App::dirList;
int App::Error = 0;
playbackInfo_t App::pInfo;

Thread thread = NULL;

/**
 * Stop the currently playing file (if there is one) and play another file.
 *
 * \param	ep_file			File to play.
 * \param	playbackInfo	Information that the playback thread requires to
 *							play file.
 */
static int changeFile(const std::string* filename, playbackInfo_t* playbackInfo)
{
	s32 prio;

	/**
	 * If music is playing, stop it. Only one playback thread should be playing
	 * at any time.
	 */
	if(thread != NULL)
	{
		/* Tell the thread to stop playback before we join it */
		PlayerInterface::ExitPlayback();

		threadJoin(thread, U64_MAX);
		threadFree(thread);
		thread = NULL;
	}

	if(filename == NULL || playbackInfo == NULL)
		return 0;

	playbackInfo->filename = *filename;

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(PlayerInterface::ThreadMainFunct, (void *)playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

App::App(void) {
	appState = INIT;
	LibInit();
	osSetSpeedupEnable(true);
	appState = LOGO;
	gui = new Gui();
	
	struct stat buf;
	if (stat("/3ds/limeplayer3ds/config.json", &buf)) {
		mkdir("/3ds/", 0777);
		mkdir("/3ds/limeplayer3ds/", 0777);
		
		std::ifstream  src("romfs:/defaultcfg/config.json", std::ios::binary);
		std::ofstream  dst("/3ds/limeplayer3ds/config.json",   std::ios::binary);
		dst << src.rdbuf();
	}
	//CFG_parse("/3ds/limeplayer3ds/config.json", &fileSettings);
	
	chdir("sdmc:/");
	chdir("MUSIC");
	Explorer::getDir(&dirList);
}

App::~App(void) {
	delete gui;
	osSetSpeedupEnable(false);
	LibExit();
}

void noDspFirmExit(void) {
	consoleInit(GFX_TOP, NULL);
	printf("\x1b[01;00H/////////////FATAL///ERROR////////////////////////");
	printf("\x1b[03;00HFatal error: Ndsp could not be Initalized.");
	printf("\x1b[05;00HThis is probably because your dspfirm is missing.");
	printf("\x1b[07;00HYou can correct this by running dsp1.");
	printf("\x1b[30;00H//////////////////////////////////////////////////");
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


void App::LibInit(void) {
	romfsInit();
	gfxInitDefault();
	if(ndspInit()) {
		noDspFirmExit();
	}
	consoleDebugInit(debugDevice_SVC);
}

void App::LibExit(void) {
	changeFile(NULL, NULL);
	ndspExit();
	romfsExit();
	gfxExit();
}

int App::MainLoop() {
	if (appState != EXITING) {
		return aptMainLoop();
	} else {
		return 0;
	}
}

void App::Update() {
	hidScanInput();
	// Respond to user input
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	static u64	mill = 0;
	int cursor;
	
	cursor = gui->GuiGetCursorPos();
	
	if (kDown & KEY_START) {
		appState = EXITING;
	}
	
	if (kDown & KEY_A && App::Error != 0) {
		App::Error = 0;
	} else if (kDown & KEY_A && appState == LOGO) {
		appState = MENU;
	} else if (kDown & KEY_A) {
		if (cursor < dirList.dirnum) {
			chdir(dirList.directories[cursor].c_str());
			gui->GuiCursorReset();
		} else {
			changeFile(&dirList.files[cursor-dirList.dirnum], &App::pInfo);
		} 
		Explorer::getDir(&dirList);
	}
	
	if(kHeld & KEY_L)
	{
		/* Pause/Play */
		if(kDown & (KEY_R | KEY_UP))
		{
			if(PlayerInterface::IsPlaying() == false)
				return;
	
			if(PlayerInterface::TogglePlayback() == true)
				puts("Paused");
			else
				puts("Playing");
			return;
		}
	
		/* Stop */
		if(kDown & KEY_B)
		{
			PlayerInterface::ExitPlayback();
			changeFile(NULL, NULL);
			/* If the playback thread is currently playing, it will now
			 * stop and tell the Watchdog thread to display "Stopped".
			 */
			return;
		}
	}
	
	if (kDown & KEY_B) {
		chdir("../");
		gui->GuiCursorReset();
		Explorer::getDir(&dirList);
	}
	
	if ((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) && cursor > 0) {
		gui->GuiCursorMove(-1);
	}
	
	if ((kDown & KEY_DOWN || ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) && cursor < dirList.total-1) {
		gui->GuiCursorMove(1);
	}
	
	if((kDown & KEY_RIGHT || ((kHeld & KEY_RIGHT) && (osGetTime() - mill > 500))) && cursor < dirList.total-1)
	{
		gui->GuiCursorMove(-5);
	}
			
	if((kDown & KEY_LEFT || ((kHeld & KEY_LEFT) && (osGetTime() - mill > 500))) && cursor > 0)
	{
		gui->GuiCursorMove(5);
	}
			
	if(kDown) {
		mill = osGetTime();
	}
}

void App::Draw() {
	gui->Drawui(&App::pInfo);
}
