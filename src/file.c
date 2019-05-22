#include <3ds.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "formats/vorbis.h"
#include "formats/opus.h"

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

int FILE_getfiletype(const char* file) {
	FILE* fp = fopen(file, "r");
	int filetype = 0; //If the file does not exist it return 0.
	
	if(fp != NULL) {
		filetype = readmagic(fp, file);
		fclose(fp);
	}
	
	return filetype;
}

int FILE_exist(const char* file) {
	FILE* fp = fopen(file, "r");
	
	if(fp != NULL) {
		fclose(fp);
		return 1;
	}
	
	return 0;
}

int FILE_copy(const char* in, const char* out) {
	FILE* infp = fopen(in, "r");
	if(infp == NULL) {
		fclose(infp);
		return 1;
	}
	FILE* outfp = fopen(out, "w");
	if(outfp == NULL) {
		fclose(infp);
		fclose(outfp);
		return 1;
	}
	fseek(infp, 0, SEEK_END);
	u32 length = ftell(infp);
	char *indata = malloc(length + 1);
	rewind(infp);
	fread(indata, 1, length, infp);
	fclose(infp);
	
	fwrite (indata , 1, length, outfp);
	fclose(outfp);
	free(indata);
	return 0;
}