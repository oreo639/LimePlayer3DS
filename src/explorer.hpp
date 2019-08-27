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

namespace Explorer
{
	int getNumberFiles(void);

	void changedir(dirList_t dirList, int sel);

	int getDir(dirList_t* dirList);	
}

#endif
