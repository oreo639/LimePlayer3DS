#include "BrowserMenu.hpp"
#include "config.hpp"
#include "plsparse.hpp"
#include "m3uparse.hpp"
#include "error.hpp"
#include "app.hpp"

Thread thread = NULL;

static void exitPlayback(void)
{
	PlayerInterface::ExitPlayback();

	threadJoin(thread, U64_MAX);
	threadFree(thread);
	thread = NULL;
}

static int changeFile(const std::string &filename, playbackInfo_t* playbackInfo)
{
	s32 prio;

	/**
	* If music is playing, stop it. Only one playback thread should be playing
	* at any time.
	*/
	if(thread != NULL)
	{
		exitPlayback();
	}

	if(!playbackInfo)
		return 0;

	playbackInfo->usePlaylist = 0;

	std::string extension = filename.substr(filename.find_last_of('.') + 1);
	if (!strncmp(extension.c_str(), "json", 4)) {
		std::string url;
		Cfg::ParseNC(filename.c_str(), &url);
		playbackInfo->filename = url;
	} else if (!strncmp(extension.c_str(), "pls", 3)) {
		// Note to future me.
		// Add pls support to netfmt.
		Pls::Parse(filename, &playbackInfo->playlistfile);
		playbackInfo->usePlaylist = 1;
	} else if (!strncmp(extension.c_str(), "m3u", 3)) {
		M3u::Parse(filename, &playbackInfo->playlistfile);
		playbackInfo->usePlaylist = 1;
	} else
		playbackInfo->filename = filename;

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(PlayerInterface::ThreadMainFunct, (void *)playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

BrowserMenu::BrowserMenu() {}

BrowserMenu::~BrowserMenu()
{
	exitPlayback();
}

void BrowserMenu::drawTop() const
{
	Gui::drawBaseGui();
	if (PlayerInterface::IsPlaying()) {
		Gui::drawBrowserPlayer(&App::pInfo);
	}
}

void BrowserMenu::drawBottom() const
{
	Gui::menuList(Gui::GetCursorPos(), Gui::GetSelPos(), 15, 32.5f, App::dirList.total);
	C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
	Gui::List(App::dirList.currentDir.c_str(), 0);
	Gui::fblist(App::dirList.total, 15);
}

void BrowserMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	static u64	mill = 0;
	int cursor;
	
	cursor = Gui::GetCursorPos();

	if (Error::IsQuered()) {
		if (kDown & KEY_A) {
			Error::Remove();
			kDown = 0;
		}
	}
	else {
		if(kHeld & KEY_L)
		{
			if(kDown & (KEY_R | KEY_UP))
			{
				if(PlayerInterface::IsPlaying())
					PlayerInterface::TogglePlayback();
			}

			if(kDown & KEY_B)
			{
				exitPlayback();
			}

			if(kDown & KEY_X)
			{
				PlayerInterface::SkipPlayback();
			}
			return;
		}

		if (kDown & KEY_A) {
			if (cursor < App::dirList.dirnum) {
				chdir(App::dirList.directories[cursor].c_str());
				Gui::CursorReset();
			} else {
				changeFile(App::dirList.files[cursor-App::dirList.dirnum], &App::pInfo);
			}
			Explorer::getDir(&App::dirList);
		}

		if (kDown & KEY_X) {
			// KUSC http://16643.live.streamtheworld.com/KUSCMP128.mp3
			// RadioSega http://content.radiosega.net:8006/rs-mpeg.mp3
			// RadioNintendo http://play.radionintendo.com/stream
			changeFile("http://play.radionintendo.com/stream", &App::pInfo);
		}

		if (kDown & KEY_B) {
			chdir("../");
			Gui::CursorReset();
			Explorer::getDir(&App::dirList);
		}

		if ((kDown & KEY_DOWN || ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) && cursor < App::dirList.total) {
			Gui::CursorMove(1);
		}

		if ((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) && cursor >= 0) {
			Gui::CursorMove(-1);
		}

		if((kDown & KEY_RIGHT || ((kHeld & KEY_RIGHT) && (osGetTime() - mill > 500))) && cursor < App::dirList.total)
		{
			Gui::CursorMove(5);
		}

		if((kDown & KEY_LEFT || ((kHeld & KEY_LEFT) && (osGetTime() - mill > 500))) && cursor >= 0)
		{
			Gui::CursorMove(-5);
		}

		if(kDown) {
			mill = osGetTime();
		}
	}
}
