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
#include "formats/mp3.hpp"

int File::GetFileType(const char* filename) {
	FILE* fp = fopen(filename, "r");
	char magic[12];

	if (!fp)
		return 0;

	fseek(fp, 0, SEEK_SET);
	fread(magic, 1, 12, fp);
	fclose(fp);

	if (!strncmp(filename, "http://", 7) || !strncmp(filename, "https://", 8)) {
		return FMT_NETWORK;
	}

	/*Wave*/
	if (!strncmp(magic, "RIFF", 4))
		return FILE_WAV;

	/*Flac*/
	else if (!strncmp(magic, "fLaC", 4))
		return FILE_FLAC;

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
		return FILE_MP3;

	/*Also Mp3*/
	//else if (isMp3(filename))
		//return FILE_MP3_NOID3;

	/*Musical Instrument Digital Interface (MIDI)*/
	else if (!strncmp(magic, "MThd", 4))
		return FILE_MIDI;

	/*Human Machine Interface (midi-like format)*/
	else if (!strncmp(magic, "HMI-MIDISONG", 12))
		return FILE_MIDI;

	/*Hmi Midi P (Another midi-like format made by HMI)*/
	else if (!strncmp(magic, "HMIMIDIP", 8))
		return FILE_MIDI;

	/*MUS, (DOOM)*/
	else if (!strncmp(magic, "MUS", 3))
		return FILE_MIDI;

	/*eXtended Midi Interface (XMI)*/
	else if (!strncmp(magic, "FORM", 4))
		return FILE_MIDI;

	return -1; //Negetive means that the file is not officaly supported.
}

int File::Copy(const char* filein, const char* fileout) {
	FILE* fin = fopen(filein, "r");
	FILE* fout = fopen(fileout, "w+");
	int c;

	if (!fin || !fout)
		return 1;

	while ((c = fgetc(fin)) && c != EOF) {
		fputc(c, fout);
	}

	fclose(fin);
	fclose(fout);
	return 0;
}
