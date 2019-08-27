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
#include <algorithm>

#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "explorer.hpp"

bool strSort(std::string a, std::string b) {return a<b;} 

int Explorer::getNumberFiles(void)
{
	DIR		*dp;
	struct dirent	*ep;
	int		ret = 0;

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
	if (sel < dirList.directories.size())
		chdir(dirList.directories[sel].c_str());
	else if (sel < dirList.total) {
		chdir(dirList.files[sel-dirList.directories.size()].c_str());
	}
}

int Explorer::getDir(dirList_t* dirList)
{
	DIR		*dp;
	struct dirent	*ep;
	char*		wd = getcwd(NULL, 0);

	if(wd == NULL)
		return 0;
	
	
	dirList->files.clear();
	dirList->files.clear();
	
	dirList->directories.clear();
	dirList->directories.clear();

	dirList->currentDir.assign(wd);

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
			continue;
		} else {
			dirList->files.push_back(ep->d_name);
			continue;
		}
	}

	std::sort(dirList->directories.begin(), dirList->directories.end(), strSort);
	std::sort(dirList->files.begin(), dirList->files.end(), strSort);

	dirList->dirNum = dirList->directories.size();
	dirList->fileNum = dirList->files.size();
	dirList->total = dirList->files.size() + dirList->directories.size();

	closedir(dp);
	free(wd);
	return dirList->directories.size() + dirList->files.size();
}
