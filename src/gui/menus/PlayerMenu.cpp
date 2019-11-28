#include "PlayerMenu.hpp"
#include "gui.hpp"
#include "app.hpp"
#include "config.hpp"
#include "plsparse.hpp"
#include "m3uparse.hpp"
#include "error.hpp"

#include "QuickSetOverlay.hpp"

Thread thread = NULL;

std::vector<ProgressBar> progressbars;

enum {
	WINDOW_MIN = 0,
	WINDOW_BROWSER = 0,
	WINDOW_CONTROLS,
	WINDOW_MAX
};

static int window = WINDOW_BROWSER;
static int cursor = 1;
static int seloffs = 0;

static void exitPlayback(void)
{
	PlayerInterface::ExitPlayback();

	threadJoin(thread, U64_MAX);
	threadFree(thread);
	thread = NULL;
}

static int changeFile(const std::string &filepath, playbackInfo_t* playbackInfo)
{
	s32 prio;

	/**
	* If music is playing, stop it. Only one playback thread should be playing
	* at any time.
	*/
	if(thread != NULL)
		exitPlayback();

	if(!playbackInfo)
		return 0;

	playbackInfo->usePlaylist = 0;

	std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
	if (!strncmp(extension.c_str(), "json", 4)) {
		std::string url;
		Cfg::ParseNC(filepath.c_str(), &url);
		playbackInfo->filepath = url;
	} else if (!strncmp(extension.c_str(), "pls", 3)) {
		// Note to future me.
		// Add pls support to netfmt.
		Pls::Parse(filepath, &playbackInfo->playlistfile);
		playbackInfo->usePlaylist = 1;
	} else if (!strncmp(extension.c_str(), "m3u", 3)) {
		M3u::Parse(filepath, &playbackInfo->playlistfile);
		playbackInfo->usePlaylist = 1;
	} else
		playbackInfo->filepath = filepath;

	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(PlayerInterface::ThreadMainFunct, (void *)playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

void List(const char* text, int row)
{
	Gui::PrintColor(text, 8.0f, row*12, 0.4f, 0.4f, 0xFF000000);
}

void PlayerMenu::fblist(int rows, int startpoint, float size) const
{
	for (int i = 0; rows-seloffs > i && i <= MAX_LIST; i++) {
		if (seloffs+i < expInst->Size()) {
			DirEntry drent = expInst->GetEntry(i + seloffs);
			Gui::PrintColor(drent.filename.c_str(), 8.0f, i*size+startpoint, 0.4f, 0.4f, 0xFF000000);
		}
	}
}

void menuList(int cur, int from, float startpoint, float size, int rows)
{
	int i = 0;
	bool color = true;

	if (rows < 1)
		return;

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

void drawBrowserPlayer(playbackInfo_t* info)
{
	Gui::DrawImage(sprites_player_playlist_idx, 20, 15);
	if (!PlayerInterface::GetDecoderName().empty())
		Gui::Print(PlayerInterface::GetDecoderName(), 150.0f, 80.0f, 0.5f, 0.5f);

	if (!info->filename.empty())
		Gui::Print(info->filename, 150.0f, 20.0f, 0.5f, 0.5f);
	else
		Gui::PrintStatic("TEXT_LOADING_GENERIC", 150.0f, 20.0f, 0.5f, 0.5f);

	if (!info->fileMeta.Artist.empty())
		Gui::Print(info->fileMeta.Artist, 150.0f, 40.0f, 0.5f, 0.5f);
	else
		Gui::PrintStatic("TEXT_LOADING_GENERIC", 150.0f, 40.0f, 0.5f, 0.5f);
}

PlayerMenu::PlayerMenu() {
	expInst = std::make_unique<Explorer>("sdmc:/");
	expInst->ChangeDir("music");
	progressbars.emplace_back(40, 160, 240, 7, C2D_Color32(0x29, 0x71, 0xEE, 0xFF));
}

PlayerMenu::~PlayerMenu()
{
	exitPlayback();
	progressbars.clear();
}

void PlayerMenu::drawTop() const
{
	if (PlayerInterface::IsPlaying())
		drawBrowserPlayer(&App::pInfo);

	Gui::DrawBaseGui();
}

void PlayerMenu::drawBottom() const
{
	if (window == WINDOW_BROWSER) {
		menuList(cursor, seloffs, 15, 15, expInst->Size());
		C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
		List(expInst->GetCurrentDir().c_str(), 0);
		PlayerMenu::fblist(expInst->Size(), 15, 15);
	} else if (window == WINDOW_CONTROLS) {
		Gui::DrawImage(sprites_player_rew_idx, 58, 203);
		Gui::DrawImage(sprites_player_stop_idx, 100, 203);

		if (PlayerInterface::IsPaused()) {
			Gui::DrawImage(sprites_player_play_idx, 145, 203);
		} else {
			Gui::DrawImage(sprites_player_pause_idx, 145, 203);
		}

		Gui::DrawImage(sprites_player_skip_idx, 187, 203);
		Gui::DrawImage(sprites_player_ffw_idx, 229, 203);

		progressbars[0].Draw();
		//C2D_DrawRectSolid(40, 160, 0.5f,  240, 7, C2D_Color32(0xC9, 0xCA, 0xDD, 0xFF));
		//C2D_DrawRectSolid(41, 161, 0.5f, (int)(238 * (PlayerInterface::GetCurrentPos()/(float)PlayerInterface::GetTotalLength())), 5, C2D_Color32(0x29, 0x71, 0xEE, 0xFF));

		Gui::Print("Position = " + std::to_string(PlayerInterface::GetTotalLength()) + "/" + std::to_string(PlayerInterface::GetCurrentPos()), 10.0f, 20.0f, 0.5f, 0.5f);
		if (!PlayerInterface::GetTotalLength())
			Gui::Print("Warn: Seeking not avalible for this file.", 10.0f, 40.0f, 0.5f, 0.5f);
	}else
		Gui::Print("window = " + std::to_string(window), 150.0f, 20.0f, 0.5f, 0.5f);
}

void PlayerMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	u32 kUp = hidKeysUp();

	progressbars[0].UpdateProgress((float)PlayerInterface::GetCurrentPos()/PlayerInterface::GetTotalLength());

	if (kDown & KEY_SELECT) {
		addOverlay<QuickSetOverlay>();
	}

	if (kHeld & KEY_X) {
		if (kDown & (KEY_R | KEY_UP)) {
			if(PlayerInterface::IsPlaying())
				PlayerInterface::TogglePlayback();
		}

		if (kDown & KEY_RIGHT) {
			if(PlayerInterface::IsPlaying())
				PlayerInterface::SeekSectionTime(PlayerInterface::GetCurrentTime()+15);
		}

		if (kDown & KEY_LEFT) {
			if(PlayerInterface::IsPlaying())
				PlayerInterface::SeekSectionTime(PlayerInterface::GetCurrentTime()-15);
		}

		if(kDown & KEY_A) {
			exitPlayback();
		}

		if(kDown & KEY_Y) {
			PlayerInterface::SkipPlayback();
		}

		return;
	}

	if (kDown & KEY_R && window < WINDOW_MAX-1)
		window++;

	if (kDown & KEY_L && window > WINDOW_MIN)
		window--;

	if (window == WINDOW_BROWSER)
		PlayerMenu::BrowserControls(touch);

	if (window == WINDOW_CONTROLS) {
		if (kDown & KEY_TOUCH) {
			int seekto = progressbars[0].SeekByClick(touch->px, touch->py);

			if (seekto > -1)
				PlayerInterface::SeekSectionPercent(seekto);
		}
	}
}

void PlayerMenu::BrowserControls(touchPosition* touch) {
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	static u64	mill = 0;

	if (kDown & KEY_A) {
		if (expInst->IsDir(cursor)) {
			expInst->ChangeTo(cursor);
			cursor = 1;
			seloffs = 0;
		} else {
			changeFile(expInst->GetAbsolutePath(cursor), &App::pInfo);
		}
	}

	/*if (kDown & KEY_X) {
		// KUSC http://16643.live.streamtheworld.com/KUSCMP128.mp3
		// RadioSega http://content.radiosega.net:8006/rs-mpeg.mp3
		// RadioNintendo http://play.radionintendo.com/stream
		changeFile("http://play.radionintendo.com/stream", &App::pInfo);
	}*/

	if (kDown & KEY_B) {
		cursor = 1;
		seloffs = 0;
		expInst->BackDir();
	}

	if((kDown & KEY_UP || ((kHeld & KEY_UP) && (osGetTime() - mill > 500))) && cursor > 0)
	{
		cursor--;

		/* 26 is the maximum number of entries that can be printed */
		if(expInst->Size() - cursor > MAX_LIST && seloffs != 0)
			seloffs--;
	}

	if((kDown & KEY_DOWN || ((kHeld & KEY_DOWN) && (osGetTime() - mill > 500))) && cursor < expInst->Size()-1)
	{
		cursor++;

		if(cursor >= MAX_LIST && expInst->Size() - cursor >= 0 && seloffs < expInst->Size() - MAX_LIST)
			seloffs++;
	}

	if((kDown & KEY_LEFT || ((kHeld & KEY_LEFT) && (osGetTime() - mill > 500))) && cursor > 0)
	{
		int skip = MAX_LIST / 2;

		if(cursor < skip)
			skip = cursor;

		cursor -= skip;

		if(expInst->Size() - cursor > MAX_LIST && seloffs != 0)
		{
			seloffs -= skip;
			if(seloffs < 0)
				seloffs = 0;
		}
	}

	if((kDown & KEY_RIGHT || ((kHeld & KEY_RIGHT) && (osGetTime() - mill > 500))) && cursor < expInst->Size()-1)
	{
		int skip = expInst->Size()-1 - cursor;

		if(skip > MAX_LIST / 2)
			skip = MAX_LIST / 2;

		cursor += skip;

		if(cursor >= MAX_LIST && expInst->Size() - cursor >= 0 && seloffs < expInst->Size() - MAX_LIST)
		{
			seloffs += skip;
			if(seloffs > expInst->Size() - MAX_LIST)
				seloffs = expInst->Size() - MAX_LIST;
		}
	}

	if(kDown) {
		mill = osGetTime();
	}
}
