#include "SelectFileMenu.hpp"
#include "gui.hpp"

#define MAX_LIST 15

static int cursor = 1;
static int seloffs = 0;

SelectFileMenu::SelectFileMenu(std::string* path) : filepath(path)
{
	expInst = std::make_unique<Explorer>("");
	expInst->SetExtFilter("cfg");
	switch (expInst->ChangeDir(filepath->substr(0, filepath->find_last_of('/')+1))) {
		case EXPATH_NOEXIST:
		case EXPATH_EMPTY:
			expInst->GotoRoot();
			expInst->ChangeDir("/");
	}
}

static void displayDirName(const char* text, int row)
{
	Gui::PrintColor(text, 8.0f, row*12, 0.4f, 0.4f, 0xFF000000);
}

void SelectFileMenu::fblist(int rows, int startpoint, float size) const
{
	for (int i = 0; rows-seloffs > i && i <= MAX_LIST; i++) {
		if (seloffs+i < expInst->Size()) {
			DirEntry drent = expInst->GetEntry(i + seloffs);
			Gui::PrintColor(drent.filename.c_str(), 8.0f, i*size+startpoint, 0.4f, 0.4f, 0xFF000000);
		}
	}
}

static void menuList(int cur, int from, float startpoint, float size, int rows)
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

void SelectFileMenu::drawTop() const
{
	Gui::Print("Select a config file.", 20.0f, 40.0f, 0.5f, 0.5f);
	Gui::Print("Press <X> to exit.", 20.0f, 60.0f, 0.5f, 0.5f);
	Gui::Print("Filter: *.cfg.", 20.0f, 80.0f, 0.5f, 0.5f);
}

void SelectFileMenu::drawBottom() const
{
	menuList(cursor, seloffs, 15, 15, expInst->Size());
	C2D_DrawRectSolid(0, 0, 0.5f, SCREEN_WIDTH, 15, C2D_Color32(119, 131, 147, 255));
	displayDirName(expInst->GetCurrentDir().c_str(), 0);
	fblist(expInst->Size(), 15, 15);
}

void SelectFileMenu::update(touchPosition* touch)
{
	u32 kDown = hidKeysDown();
	u32 kHeld = hidKeysHeld();
	static u64	mill = 0;

	if (kDown & KEY_X)
		Gui::BackMenu();

	if (kDown & KEY_A) {
		DirEntry_t dent = expInst->GetEntry(cursor);
		if (dent.directory) {
			expInst->ChangeTo(cursor);
			cursor = 1;
			seloffs = 0;
		} else {
			filepath->assign(expInst->GetCurrentDir() + dent.filename);
			Gui::BackMenu();
		}
	}

	if (kDown & KEY_B) {
		cursor = 1;
		seloffs = 0;
		if (expInst->BackDir() == EXPATH_EXIT)
			Gui::BackMenu();
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
