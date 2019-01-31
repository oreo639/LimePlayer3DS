#include <stdio.h>

#ifndef LIME_FILE_H
#define LIME_FILE_H

typedef enum
{
	FILE_WAV = 1,		///< Microsoft WAVE
	FILE_FLAC = 2,		///< Free Lossless Audio Codec
	FILE_VORBIS = 3,	///< Ogg Vorbis
	FILE_OPUS = 4,  	///< Ogg Opus
	FILE_MP3 = 5,  		///< Mp3
	FILE_MIDI = 6,  	///< Midi and midilike
} FILE_audioformat;

int FILE_getfiletype(const char* file);
int FILE_copy(const char* in, const char* out);
int FILE_exist(const char* file);
#endif
