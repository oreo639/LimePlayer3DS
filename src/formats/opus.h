#include <opusfile.h>
#include "player.h"

int initOpus(playbackInfo_t* info);

uint32_t rateOpus(void);

uint8_t channelOpus(void);

uint64_t decodeOpus(void* buffer);

void infoOpus(playbackInfo_t* infoOut);

uint32_t posOpus(void);

uint32_t lengthOpus(void);

void seekOpus(uint32_t location);

void exitOpus(void);

int playOpus(const char* in);

uint64_t fillOpusBuffer(int16_t* bufferOut);

int isOpus(const char* in);
