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
#include <list>

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

Explorer::Explorer(const std::string& root) : rootDir(root) {
	this->LoadEntries();
}

int Explorer::LoadEntries(void) {
	DIR		*dp;
	struct dirent	*ep;

	if(!(dp = opendir(GetCurrentDir().c_str())) || errno == ENOENT) {
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

PathType Explorer::NormalizePath(const PathType& path)
{
	PathType ret;
	for(auto& part : path)
	{
		if(part == "..")
		{
			uint32_t find = ret.string().find_last_of("/");
			if (find >= ret.string().size()-1)
				ret.clear();
			else
				ret = ret.string().substr(0, find+1);
		} else if (part == ".")
			continue; 
		else
			ret /= part.string();
	}
	if (ret.string().size()-1 != ret.string().find_last_of("/"))
		ret += "/";
	return ret;
}

int Explorer::CheckDir(const PathType& path) {
	DIR		*dp;
	if(!(dp = opendir((path).c_str())) || errno == ENOENT) {
		closedir(dp);
		return EXPATH_NOEXIST;
	}

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

int Explorer::ChangeDir(const PathType path) {
	int ret = 0;
	if (path.empty())
		return EXPATH_EMPTY;

	if (this->CheckDir(GetCurrentDir() / path))
		return EXPATH_NOEXIST;

	relativePath /= path;
	relativePath = this->NormalizePath(relativePath);

	ret = this->LoadEntries();

	if (relativePath.empty())
		if (!ret)
			ret = EXPATH_EXIT;

	return ret;
}

int Explorer::BackDir(void) {
	return this->ChangeDir("..");
}
