#include <set>
#include <algorithm>
#include "PlayerMenu.hpp"
#include "gui.hpp"
#include "app.hpp"
#include "config.hpp"
#include "plsparse.hpp"
#include "m3uparse.hpp"
#include "error.hpp"
#include "color.hpp"

#include "QuickSetOverlay.hpp"

#define MAX_LIST 14

Thread thread = NULL;

enum {
	WINDOW_MIN = 0,
	WINDOW_BROWSER = 0,
	WINDOW_CONTROLS,
	WINDOW_MAX
};

enum DrawControls {
	CON_REWIND = 0,
	CON_STOP,
	CON_PLAYPAUSE,
	CON_SKIP,
	CON_FASTFOWARD,
};

static int window = WINDOW_BROWSER;
static int cursor = 1;
static int seloffs = 0;

struct {
	float x;
	float y;
	float w;
	float h;
} topbut[2] = {
	{0, 0, BOT_SCREEN_WIDTH/2, 15},
	{BOT_SCREEN_WIDTH/2, 0, BOT_SCREEN_WIDTH/2, 15},
};

std::vector<ProgressBar> m_progressbars;
std::vector<Button> m_buttons;

static const std::set<std::string> SUPPORTED_EXTS = {
	"mp3","wav","flac","ogg","opus",
	"midi","xmi","mus","hmi","hmp","hmq"
};

static bool is_extension_supported(std::string ext) {
	std::string lower_ext = ext;
	std::transform(lower_ext.begin(),lower_ext.end(),lower_ext.begin(),[](unsigned char c){ return std::tolower(c); });
	return SUPPORTED_EXTS.find(lower_ext) != SUPPORTED_EXTS.end();
}

static void exitPlayback(void)
{
	PlayerInterface::ExitPlayback();

	threadJoin(thread, U64_MAX);
	threadFree(thread);
	thread = NULL;
}

int PlayerMenu::changeFile(const std::string &filepath, playbackInfo_t* playbackInfo)
{
	s32 prio;

	std::string extension = filepath.substr(filepath.find_last_of('.') + 1);
	
	/**
	* If music is playing, stop it. Only one playback thread should be playing
	* at any time.
	*/
	if(thread != NULL)
		exitPlayback();

	if(!playbackInfo)
		return 0;
	
	playbackInfo->playlist.files.clear();
	playbackInfo->playlist.files.shrink_to_fit();

	if (!strncmp(extension.c_str(), "json", 4)) {
		std::string url;
		Cfg::ParseNC(filepath.c_str(), &url);
		playbackInfo->playlist.files.push_back(url);
	} else if (!strncmp(extension.c_str(), "pls", 3)) {
		// Note to future me.
		// Add pls support to netfmt.
		Pls::Parse(filepath, &playbackInfo->playlist);
	} else if (!strncmp(extension.c_str(), "m3u", 3)) {
		M3u::Parse(filepath, &playbackInfo->playlist);
	} else {
		int idx = 0;
		for (int i=0; i < expInst->Size(); i++) { 
			// rule out unsupporteds
			auto e = expInst->GetEntry(i);
			std::string ext = e.filename.substr(e.filename.find_last_of(".") + 1);
			if (e.directory || e.filename == ".." || !is_extension_supported(ext)) {
				DEBUG("%s wasn't recognised as supported, continuing\n", e.filename);
				continue;
			}

			std::string name = expInst->GetAbsolutePath(i);
			if (name == filepath) { playbackInfo->playindex = idx; }

			DEBUG("%s",name);
			playbackInfo->playlist.files.push_back(name);
			idx++;
		}
		if (idx+1 == expInst->Size()) {
			DEBUG("%s wasn't recognised as supported, aborting\n", e.filename);
			return 0;
		}
	}
	svcGetThreadPriority(&prio, CUR_THREAD_HANDLE);
	thread = threadCreate(PlayerInterface::ThreadMainFunct, (void *)playbackInfo, 32 * 1024, prio - 1, -2, false);

	return 0;
}

void PrintDir(const char* text, int startpoint)
{
	Gui::PrintColor(text, 8.0f, startpoint, 0.4f, 0.4f, 0xFF000000);
}

/// Draws file browser
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

	if (!info->fileMeta.Title.empty())
		Gui::Print(info->fileMeta.Title, 150.0f, 20.0f, 0.5f, 0.5f);
	else if (!info->filename.empty())
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
	m_progressbars.emplace_back(40, 160, 240, 10, COLOR_LIMEGREEN);
	m_buttons.emplace_back(58, 203, sprites_player_rew_idx);
	m_buttons.emplace_back(100, 203, sprites_player_stop_idx);
	m_buttons.emplace_back(145, 203, sprites_player_play_idx);
	m_buttons.emplace_back(187, 203, sprites_player_skip_idx);
	m_buttons.emplace_back(229, 203, sprites_player_ffw_idx);
}

PlayerMenu::~PlayerMenu()
{
	exitPlayback();
	m_progressbars.clear();
	m_buttons.clear();
	removeOverlays();
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
		menuList(cursor, seloffs, 30, 15, expInst->Size());
		Gui::DrawSolidRectangle(0, 15, BOT_SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
		PrintDir(expInst->GetCurrentDir().c_str(), 15);
		PlayerMenu::fblist(expInst->Size(), 30, 15);

		Gui::DrawSolidRectangle(topbut[0].x, topbut[0].y, topbut[0].w, topbut[0].h, C2D_Color32(119, 131, 147, 255)); // For top buttons
		Gui::PrintColor("Browser", 10, 0, 0.5f, 0.5f, 0xFF000000);
		Gui::PrintColor("Controls", BOT_SCREEN_WIDTH/2 + 10, 0, 0.5f, 0.5f, 0xFFFFFFFF);
	} else if (window == WINDOW_CONTROLS) {
		m_buttons[CON_REWIND].Draw();
		m_buttons[CON_STOP].Draw();
		m_buttons[CON_PLAYPAUSE].Draw();
		m_buttons[CON_SKIP].Draw();
		m_buttons[CON_FASTFOWARD].Draw();

		m_progressbars[0].Draw();

		Gui::Print("Position = " + std::to_string(PlayerInterface::GetTotalLength()) + "/" + std::to_string(PlayerInterface::GetCurrentPos()), 10.0f, 20.0f, 0.5f, 0.5f);
		if (!PlayerInterface::GetTotalLength())
			Gui::Print("Warn: Seeking not avalible for this file.", 10.0f, 40.0f, 0.5f, 0.5f);

		Gui::DrawSolidRectangle(topbut[1].x, topbut[1].y, topbut[1].w, topbut[1].h, C2D_Color32(119, 131, 147, 255)); // For top buttons
		Gui::PrintColor("Browser", 10, 0, 0.5f, 0.5f, 0xFFFFFFFF);
		Gui::PrintColor("Controls", BOT_SCREEN_WIDTH/2 + 10, 0, 0.5f, 0.5f, 0xFF000000);
	} else
		Gui::Print("window = " + std::to_string(window), 150.0f, 20.0f, 0.5f, 0.5f);
}

void PlayerMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	u32 kUp = hidKeysUp();

	if (PlayerInterface::IsPlaying())
		m_progressbars[0].UpdateProgress(PlayerInterface::GetCurrentPos()*100/PlayerInterface::GetTotalLength());
	else
		m_progressbars[0].UpdateProgress(-1);

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
		if (PlayerInterface::IsPaused()) {
			m_buttons[CON_PLAYPAUSE].SetIcon(sprites_player_play_idx);
		} else {
			m_buttons[CON_PLAYPAUSE].SetIcon(sprites_player_pause_idx);
		}

		if (kDown & KEY_TOUCH) {
			if (PlayerInterface::IsPlaying()) {
				if (m_buttons[CON_REWIND].Update(touch->px, touch->py))
					PlayerInterface::SeekSectionTime(PlayerInterface::GetCurrentTime()-15);
				else if (m_buttons[CON_STOP].Update(touch->px, touch->py))
					exitPlayback();
				else if (m_buttons[CON_PLAYPAUSE].Update(touch->px, touch->py))
					PlayerInterface::TogglePlayback();
				else if (m_buttons[CON_SKIP].Update(touch->px, touch->py))
					PlayerInterface::SkipPlayback();
				else if (m_buttons[CON_FASTFOWARD].Update(touch->px, touch->py))
					PlayerInterface::SeekSectionTime(PlayerInterface::GetCurrentTime()+15);
			}

			int seekto = m_progressbars[0].SeekByClick(touch->px, touch->py);

			if (seekto > -1)
				PlayerInterface::SeekSectionPercent(seekto);
		}
	}

	if (kDown & KEY_TOUCH) {
		int clickX = touch->px;
		int clickY = touch->py;

		if ((clickX > topbut[0].x) && (clickY > topbut[0].y) && (clickX < topbut[0].x + topbut[0].w) && (clickY < topbut[0].y + topbut[0].h))
			window = WINDOW_BROWSER;
		else if ((clickX > topbut[1].x) && (clickY > topbut[1].y) && (clickX < topbut[1].x + topbut[1].w) && (clickY < topbut[1].y + topbut[1].h))
			window = WINDOW_CONTROLS;
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
