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
#include "config.hpp"
#include "parsecfg/plsparse.hpp"

Gui *gui;

App::AppState App::appState = INIT;
dirList_t App::dirList;
playbackInfo_t App::pInfo;

Thread thread = NULL;

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

	playbackInfo->usePlaylist = 0;

	std::string extension = filename->substr(filename->find_last_of('.') + 1);
	if (!strncmp(extension.c_str(), "json", 4)) {
		std::string url;
		CFG_parseNC(filename->c_str(), &url);
		playbackInfo->filename = url;
	} else if (!strncmp(extension.c_str(), "pls", 3)) {
		// Note to future me.
		// Add pls support to netfmt and add m3u support as well.
		Pls::Parse(*filename, &playbackInfo->playlistfile);
		playbackInfo->usePlaylist = 1;
	} else
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
	
	struct stat buf;
	if (stat("/3ds/limeplayer3ds/config.json", &buf)) {
		mkdir("/3ds/", 0777);
		mkdir("/3ds/limeplayer3ds/", 0777);
		
		std::ifstream  src("romfs:/defaultcfg/config.json", std::ios::binary);
		std::ofstream  dst("/3ds/limeplayer3ds/config.json",   std::ios::binary);
		dst << src.rdbuf();
	}
	CFG_parseSettings("/3ds/limeplayer3ds/config.json", &App::pInfo.settings);

	gui = new Gui(&App::pInfo.settings);
	
	chdir("sdmc:/");
	chdir("MUSIC");
	Explorer::getDir(&dirList);
}

App::~App(void) {
	delete gui;
	CFG_cleanSettings(&App::pInfo.settings);
	osSetSpeedupEnable(false);
	LibExit();
}

void noDspFirmExit(void) {
	consoleInit(GFX_TOP, NULL);
	std::printf("\x1b[01;00H/////////////FATAL///ERROR////////////////////////");
	std::printf("\x1b[03;00HFatal error: Ndsp could not be Initalized.");
	std::printf("\x1b[05;00HThis is probably because your dspfirm is missing.");
	std::printf("\x1b[07;00HYou can correct this by running dsp1.");
	std::printf("\x1b[09;00HPress start to exit.");
	std::printf("\x1b[30;00H//////////////////////////////////////////////////");
	while (aptMainLoop()) {
		gspWaitForVBlank();
		hidScanInput();
		
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break;

		gfxSwapBuffers();
		gfxFlushBuffers();
	}
	
	gfxExit();
	romfsExit();
	exit(22);
}


void App::LibInit(void) {
	romfsInit();
	gfxInitDefault();
	httpcInit(0);
	if(ndspInit()) {
		noDspFirmExit();
	}
	consoleDebugInit(debugDevice_SVC);
}

void App::LibExit(void) {
	changeFile(NULL, NULL);
	httpcExit();
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

	if (Error::IsQuered()) {
		if (kDown & KEY_A) {
			Error::Remove();
		}
	} else if (appState == LOGO) {
		if (kDown & KEY_A) {
			appState = MENU;
		}
	}
	else if (appState == MENU) {
		if(kHeld & KEY_L)
		{
			if(kDown & (KEY_R | KEY_UP))
			{
				if(PlayerInterface::IsPlaying() == false)
					return;

				if(PlayerInterface::TogglePlayback() == true)
					return;
			}

			if(kDown & KEY_B)
			{
				PlayerInterface::ExitPlayback();
				changeFile(NULL, NULL);
				return;
			}

			if(kDown & KEY_X)
			{
				PlayerInterface::SkipPlayback();
				return;
			}
		}

		if (kDown & KEY_A) {
			if (cursor < dirList.dirnum) {
				chdir(dirList.directories[cursor].c_str());
				gui->GuiCursorReset();
			} else {
				changeFile(&dirList.files[cursor-dirList.dirnum], &App::pInfo);
			}
			Explorer::getDir(&dirList);
		}

		if (kDown & KEY_X) {
			// KUSC http://16643.live.streamtheworld.com/KUSCMP128.mp3
			// RadioSega http://content.radiosega.net:8006/rs-mpeg.mp3
			// RadioNintendo http://play.radionintendo.com/stream
			std::string s("http://play.radionintendo.com/stream");
			changeFile(&s, &App::pInfo);
		}

		if (kDown & KEY_B) {
			chdir("../");
			gui->GuiCursorReset();
			Explorer::getDir(&dirList);
		}

		if ((kDown & KEY_DOWN || ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) && cursor < dirList.total) {
			gui->GuiCursorMove(1);
		}

		if ((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) && cursor >= 0) {
			gui->GuiCursorMove(-1);
		}

		if((kDown & KEY_RIGHT || ((kHeld & KEY_RIGHT) && (osGetTime() - mill > 500))) && cursor < dirList.total)
		{
			gui->GuiCursorMove(5);
		}

		if((kDown & KEY_LEFT || ((kHeld & KEY_LEFT) && (osGetTime() - mill > 500))) && cursor >= 0)
		{
			gui->GuiCursorMove(-5);
		}

		if(kDown) {
			mill = osGetTime();
		}
	}
}

void App::Draw() {
	gui->Drawui(&App::pInfo);
}
