#ifndef __LIME_AUDIO_BACKEND__
#define __LIME_AUDIO_BACKEND__
#include <3ds.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "file.h"
#include "player.h"

int playerget_InitDecoder(playbackInfo_t* info, int filetype);
int playerget_SendOutput(void);
void playerget_FileInfo(playbackInfo_t* info);
uint32_t playerget_currentPos(void);
uint32_t playerget_totalLength(void);
uint32_t playerget_rate(void);
void playerget_seek(uint32_t location);
void playerget_Exit(void);
size_t playerget_Decode(void *buffer);
#endif
