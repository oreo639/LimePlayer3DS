/*   LimePlayer3DS FOSS graphcal music player for the 3ds.
*    Copyright (C) 2018  LimePlayer Team
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
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

#include "explorer.h"

int cmpstringp(const void *p1, const void *p2)
{
	/* The actual arguments to this function are "pointers to
	   pointers to char", but strcmp(3) arguments are "pointers
	   to char", hence the following cast plus dereference */

	return strcasecmp(* (char * const *) p1, * (char * const *) p2);
}

int getNumberFiles(void)
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

int getDir(dirList_t* dirList)
{
	DIR				*dp;
	struct dirent	*ep;
	int				fileNum = 0;
	int				dirNum = 0;
	char*			wd = getcwd(NULL, 0);

	if(wd == NULL)
		goto out;

	/* Clear strings */
	for(int i = 0; i < dirList->dirNum; i++)
		free(dirList->directories[i]);

	for(int i = 0; i < dirList->fileNum; i++)
		free(dirList->files[i]);

	free(dirList->currentDir);

	if((dirList->currentDir = strdup(wd)) == NULL)
		puts("Failure");

	if((dp = opendir(wd)) == NULL)
		goto out;

	while((ep = readdir(dp)) != NULL)
	{
		if(ep->d_type == DT_DIR)
		{
			/* Add more space for another pointer to a dirent struct */
			dirList->directories = realloc(dirList->directories, (dirNum + 1) * sizeof(char*));

			if((dirList->directories[dirNum] = strdup(ep->d_name)) == NULL)
				puts("Failure");

			dirNum++;
			continue;
		}

		/* Add more space for another pointer to a dirent struct */
		dirList->files = realloc(dirList->files, (fileNum + 1) * sizeof(char*));

		if((dirList->files[fileNum] = strdup(ep->d_name)) == NULL)
			puts("Failure");

		fileNum++;
	}

	qsort(&dirList->files[0], fileNum, sizeof(char *), cmpstringp);
	qsort(&dirList->directories[0], dirNum, sizeof(char *), cmpstringp);

	dirList->dirNum = dirNum;
	dirList->fileNum = fileNum;

	if(closedir(dp) != 0)
		goto out;

out:
	free(wd);
	return fileNum + dirNum;
}
