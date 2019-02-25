/*Copyright 2018-2019 Oreo639
*
*Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the *"Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, *distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to *the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH *THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#include <iostream>
#include <algorithm>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <3ds.h>

#include "explorer.hpp"

bool strSort(std::string a, std::string b) {return a<b;} 

int Explorer::getNumberFiles(void)
{
	DIR				*dp;
	struct dirent	*ep;
	int				ret = 0;

	if((dp = opendir(".")) == NULL)
		goto err;

	while((ep = readdir(dp)) != NULL)
		ret++;

	closedir(dp);

out:
	return ret;

err:
	ret = -1;
	goto out;
}

void Explorer::changedir(dirList_t dirList, int sel) {
	if (sel < dirList.dirnum)
		chdir(dirList.directories[sel].c_str());
	else if (sel < dirList.total) {
		chdir(dirList.files[sel-dirList.dirnum].c_str());
	}
}

int Explorer::getDir(dirList_t* dirList)
{
	DIR				*dp;
	struct dirent	*ep;
	int				fileNum = 0;
	int				dirNum = 0;
	char*			wd = getcwd(NULL, 0);

	if(wd == NULL)
		return 0;
	
	
	dirList->files.clear();
	dirList->files.clear();
	
	dirList->directories.clear();	
	dirList->directories.clear();

	dirList->currentDir.assign(wd, strlen(wd));


	//std::cout << std::endl;


	if((dp = opendir(dirList->currentDir.c_str())) == NULL) {
		closedir(dp);
		free(wd);
		return 0;
	}

	while((ep = readdir(dp)) != NULL)
	{
		if(ep->d_type == DT_DIR)
		{
			dirList->directories.push_back(ep->d_name);
			dirList->directories[dirList->directories.size()-1].append("/");
			dirNum++;
			continue;
		} else {
			dirList->files.push_back(ep->d_name);
			fileNum++;
			continue;
		}
	}

	std::sort(dirList->directories.begin(), dirList->directories.end(), strSort);
	std::sort(dirList->files.begin(), dirList->files.end(), strSort);

	dirList->filenum = fileNum;
	dirList->dirnum = dirNum;
	dirList->total = fileNum + dirNum;
	

	closedir(dp);
	free(wd);
	return fileNum + dirNum;
}
