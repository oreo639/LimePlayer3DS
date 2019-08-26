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
#include <sys/stat.h>
#include <stdio.h>

#include "app.hpp"
#include "gui.hpp"
#include "error.hpp"
#include "explorer.hpp"
#include "player.hpp"
#include "file.hpp"
#include "config.hpp"
#include "macrotools.h"
#include "TitleScreen.hpp"
#include "BrowserMenu.hpp"
#include "parsecfg/plsparse.hpp"
#include "parsecfg/m3uparse.hpp"

bool App::exit = false;
dirList_t App::dirList;
playbackInfo_t App::pInfo;

App::App(void) {
	LibInit();
	osSetSpeedupEnable(true);
	aptSetSleepAllowed(false);

	if (!File::Exists("/3ds/limeplayer3ds/config.json")) {
		mkdir("/3ds/", 0777);
		mkdir("/3ds/limeplayer3ds/", 0777);

		File::Copy("romfs:/defaultcfg/config.json", "/3ds/limeplayer3ds/config.json");
	}
	Cfg::ParseSettings("/3ds/limeplayer3ds/config.json", &App::pInfo.settings);

	debug_init(true);
	debug_perform("Debug output for LimePlayer3DS version %s\nTHIS FILE IS AUTOMATICALY GENERATED PLEASE DO NOT MODIFY!\n", STRINGIFY(LIMEPLAYER_VERSION));

	Gui::Init(&App::pInfo.settings);
	Gui::SetMenu(std::make_unique<TitleScreen>());

	chdir("sdmc:/");
	chdir("music");
	Explorer::getDir(&dirList);
}

App::~App(void) {
	Gui::Exit();
	DEBUG("App is exiting!\n");
	Cfg::CleanSettings(&App::pInfo.settings);
	osSetSpeedupEnable(false);
	debug_exit();
	LibExit();
	DEBUG("Goodbye.\n");
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
	httpcExit();
	ndspExit();
	romfsExit();
	gfxExit();
}

int App::MainLoop() {
	if (App::exit != true) {
		return aptMainLoop();
	} else {
		aptMainLoop();
		return 0;
	}
}

void App::Update() {
	Gui::Update(&App::pInfo);
}
