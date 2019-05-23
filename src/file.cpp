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
#include <3ds.h>
#include <stdlib.h>
#include <string.h>

#include "file.hpp"
#include "formats/vorbis.hpp"
#include "formats/opus.hpp"

int readmagic(FILE* file, const char* filename){
	char magic[4];

	fseek(file, 0, SEEK_SET);
	fread(magic, 1, 4, file);
	fseek(file, 0, SEEK_SET);

	/*Wave*/
	if (!strncmp(magic, "RIFF", 4))
	{
		return FILE_WAV;
	}
	/*Flac*/
	else if (!strncmp(magic, "fLaC", 4))
	{
		return FILE_FLAC;
	}
	/*Ogg or Opus*/
	else if (!strncmp(magic, "OggS", 4))
	{
		if (!isVorbis(filename))
			return FILE_VORBIS;
		else if (!isOpus(filename))
			return FILE_OPUS;
	}
	/*Mp3*/
	else if (!strncmp(magic, "ID3", 3))
	{
		return FILE_MP3;
	}
	/*Musical Instrument Digital Interface (MIDI)*/
	else if (!strncmp(magic, "MThd", 4))
	{
		return FILE_MIDI;
	}
	/*Human Machine Interface (midi-like format)*/
	else if (!strncmp(magic, "HMI-", 4))
	{
		return FILE_MIDI;
	}
	/*Hmi Midi P (Another midi-like format made by HMI)*/
	else if (!strncmp(magic, "HMIM", 4))
	{
		return FILE_MIDI;
	}
	/*MUS, (DOOM)*/
	else if (!strncmp(magic, "MUS", 3))
	{
		return FILE_MIDI;
	}
	/*eXtended Midi Interface (XMI)*/
	else if (!strncmp(magic, "FORM", 4))
	{
		return FILE_MIDI;
	}

	return -1; //Negetive means that the file is not officaly supported.
}

int File::GetFileType(const char* file) {
	FILE* fp = fopen(file, "r");
	int filetype = 0; //If the file does not exist it return 0.

	if (!strncmp(file, "http://", 7)) {
		return FMT_NETWORK;
	}else if(fp != NULL) {
		filetype = readmagic(fp, file);
		fclose(fp);
	}
	
	return filetype;
}
