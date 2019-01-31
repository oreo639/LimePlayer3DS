#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include "player.h"

int initVorbis(playbackInfo_t* info);

uint32_t rateVorbis(void);

uint8_t channelVorbis(void);

uint64_t decodeVorbis(void* buffer);

void infoVorbis(playbackInfo_t* infoOut);

uint32_t posVorbis(void);

uint32_t lengthVorbis(void);

void seekVorbis(uint32_t location);

void exitVorbis(void);

int playVorbis(const char* in);

uint64_t fillVorbisBuffer(char* bufferOut);

int isVorbis(const char* in);
