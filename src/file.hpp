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

namespace File {
	int GetFileType(const char* file);
}
#endif
