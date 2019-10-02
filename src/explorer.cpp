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
#include <errno.h>

#include "explorer.hpp"

bool entSort(DirEntry a, DirEntry b) {
	bool srt;

	if (a.directory == b.directory)
		srt = a.filename < b.filename;
	else
		srt = a.directory > b.directory;

	return srt;
}

Explorer::Explorer(const std::string& root) : RootDir(root) {
	this->LoadEntries();
}

int Explorer::LoadEntries(void) {
	DIR		*dp;
	struct dirent	*ep;

	if(!(dp = opendir((RootDir + RelativePath).c_str())) || errno == ENOENT) {
		closedir(dp);
		return EXPATH_NOEXIST;
	}

	this->entries.clear();

	this->entries.emplace_back("..", true);

	while((ep = readdir(dp)) != NULL) {
		std::string filename(ep->d_name);
		bool isDirectory;

		if (filename == "." || filename == "..")
			continue;

		if ((isDirectory = (ep->d_type == DT_DIR)))
			filename.append("/");

		this->entries.emplace_back(filename, isDirectory);
	}

	std::sort(this->entries.begin(), this->entries.end(), entSort);

	closedir(dp);
	return 0;
}

std::string Explorer::Item(uint32_t index) {
	if (index < this->entries.size())
		return this->entries[index].filename;
	return "";
}

bool Explorer::IsDir(uint32_t index) {
	if (index < this->entries.size())
		return this->entries[index].directory;
	return false;
}

DirEntry Explorer::GetEntry(uint32_t index) {
	return this->entries[index];
}

std::string Explorer::GetAbsolutePath(uint32_t index) {
	if (index < this->entries.size()) {
		std::string absolutePath(this->GetCurrentDir());

		if (absolutePath.find_last_of('/') != absolutePath.size()-1)
			absolutePath.append("/");

		absolutePath.append(this->entries[index].filename);
		return absolutePath;
	}

	return "";
}

int Explorer::ChangeTo(uint32_t index) {
	ChangeDir(this->entries[index].filename);
	return this->LoadEntries();
}

int Explorer::ChangeDir(const std::string path) {
	if (path.empty())
		return EXPATH_EMPTY;

	if (path == "..")
		return BackDir();

	if (path == ".")
		return 0;

	std::string curdir(GetCurrentDir());
	if (curdir.find_last_of('/') != curdir.size()-1)
		RelativePath += "/";

	if (path.find_last_of('/') != path.size()-1)
		RelativePath += (path + "/");
	else
		RelativePath += path;
	return this->LoadEntries();
}

int Explorer::BackDir(void) {
	if (RelativePath.empty())
		return EXPATH_EXIT;

	uint8_t delpos = RelativePath.find_last_of('/');
	if ((delpos == 0) && (RelativePath.size()))
		return EXPATH_EXIT;
	else if (delpos >= RelativePath.size())
		RelativePath.clear();
	else if (delpos == RelativePath.size() - 1) {
		delpos = RelativePath.substr(0, delpos).find_last_of('/');
		if (delpos >= RelativePath.size())
			RelativePath.clear();
		else
			RelativePath = RelativePath.substr(0, delpos+1);

		if (GetCurrentDir().empty())
			RelativePath = "/";
	} else
		RelativePath = RelativePath.substr(0, delpos);

	return this->LoadEntries();
	return 0;
}
