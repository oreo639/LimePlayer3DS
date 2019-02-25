/*Copyright 2018-2019 Oreo639
*
*Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the *"Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, *distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to *the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH *THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
	
	if(fp != NULL) {
		filetype = readmagic(fp, file);
		fclose(fp);
	}
	
	return filetype;
}
