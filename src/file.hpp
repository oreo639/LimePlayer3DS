#include <stdio.h>
#include <string>

#ifndef LIME_FILE_H
#define LIME_FILE_H

typedef enum
{
	FILE_WAV = 1,     ///< Microsoft WAVE
	FILE_FLAC,        ///< Free Lossless Audio Codec
	FILE_VORBIS,      ///< Ogg Vorbis
	FILE_OPUS,        ///< Ogg Opus
	FILE_MP3,         ///< Mp3 with ID3
	FILE_MP3_NOID3,   ///< Mp3 without ID3
	FILE_MIDI,        ///< Midi and midilike
	FMT_NETWORK,      ///< Network http stream
} FILE_audioformat;

namespace File {
	int GetFileType(const std::string& file);
	int Copy(const char* filein, const char* fileout);
	int Exists(const char* filepath);
}
#endif
