#ifndef __FILEBROWSER__
#define __FILEBROWSER__

#include <string>
#include <vector>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

typedef std::vector<std::string> strvec;

typedef struct
{
	strvec		files;
	int		fileNum;
	strvec		directories;
	int		dirNum;
	std::string	currentDir;
	int		total;
} dirList_t;

typedef struct DirEntry {
	DirEntry(const std::string& name, bool directory) : filename(name), directory(directory) {}
	std::string filename;
	bool directory;
} DirEntry_t;

enum {
	EXPATH_NOEXIST = 1,   // Directory does not exist
	EXPATH_EMPTY = 2,     // The given path is an empty string
	EXPATH_EXIT = 200     // Attempted to access beyond root dir
};

class Explorer
{
	public:
	Explorer(const std::string& root);
	~Explorer() {}
	std::string Item(uint32_t index);
	bool IsDir(uint32_t index);
	DirEntry GetEntry(uint32_t index);
	std::string GetAbsolutePath(uint32_t index);
	int ChangeTo(uint32_t index);
	int ChangeDir(const std::string path);
	int BackDir(void);
	void GotoRoot(void) {RelativePath.clear(); LoadEntries();}
	int Size(void) {return entries.size();}
	std::string GetCurrentDir(void) {return RootDir + RelativePath;}

	private:
	int LoadEntries(void);
	std::vector<DirEntry_t> entries;
	std::string RootDir;
	std::string RelativePath;
};

#endif
