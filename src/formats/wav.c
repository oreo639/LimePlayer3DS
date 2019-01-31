#define DR_WAV_IMPLEMENTATION
#define WAVBUFSIZE 16 * 1024
#include <dr_libs/dr_wav.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "wav.h"

static const size_t	buffSize	= 16 * 1024;
static drwav*		pWav		= NULL;
static uint32_t		wavprogress;

/**
 * Initialise WAV playback.
 *
 * \param	file	Location of WAV file to play.
 * \return			0 on success, else failure.
 */
int initWav(playbackInfo_t* info)
{
	pWav = drwav_open_file(info->file);
	wavprogress = 0;
	return pWav == NULL ? -1 : buffSize;
}

/**
 * Get sampling rate of Wav file.
 *
 * \return	Sampling rate.
 */
uint32_t rateWav(void)
{
	return pWav->sampleRate;
}

/**
 * Get number of channels of Wav file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelWav(void)
{
	return pWav->channels;
}

/**
 * Read part of open Wav file.
 *
 * \param buffer	Output.
 * \return			Samples read for each channel.
 */
uint64_t readWav(void* buffer)
{
	uint64_t out = drwav_read_s16(pWav, buffSize, buffer);
	wavprogress += out;
	return out;
}

void infoWav(playbackInfo_t* infoOut) {
	//Metadat can be stored in many diffrent ways for wav. I will probably handle it last.
	infoOut->fileMeta->authorCpright = strdup("(No Author-Wav)");
}

uint32_t posWav(void)
{
	return wavprogress;
}

uint32_t lengthWav(void)
{
	return pWav->totalSampleCount;
}

void seekWav(uint32_t location)
{
	if(location > pWav->totalSampleCount) {
		return;
	}
	drwav_seek_to_sample(pWav, location);
	wavprogress = location;
}

/**
 * Free Wav file.
 */
void exitWav(void)
{
	drwav_close(pWav);
}
