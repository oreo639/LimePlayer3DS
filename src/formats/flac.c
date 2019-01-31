#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_BUFFER_SIZE 16 * 1024
#include <dr_libs/dr_flac.h>

#include "flac.h"

static drflac*		pFlac;
static const size_t	buffSize = 16 * 1024;
uint32_t flacprogress; //Credit Tangerine.

/**
 * Initialise Flac decoder.
 *
 * \param	file	Location of flac file to play.
 * \return			0 on success, else failure.
 */
int initFlac(playbackInfo_t* info)
{
	pFlac = drflac_open_file(info->file);
	flacprogress = 0;
	return pFlac == NULL ? -1 : buffSize;
}

/**
 * Get sampling rate of Flac file.
 *
 * \return	Sampling rate.
 */
uint32_t rateFlac(void)
{
	return pFlac->sampleRate;
}

/**
 * Get number of channels of Flac file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelFlac(void)
{
	return pFlac->channels;
}

/**
 * Decode part of open Flac file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeFlac(void* buffer)
{
	uint64_t out = drflac_read_s16(pFlac, buffSize, buffer);
	flacprogress += out;
	return out;
}

void infoFlac(playbackInfo_t* infoOut) {
	infoOut->fileMeta->authorCpright = strdup("(No Author-Flac)");
}

uint32_t posFlac(void) {
	return flacprogress;
}

uint32_t lengthFlac(void) {
	return pFlac->totalSampleCount;
}

void seekFlac(uint32_t location)
{
	if(location > pFlac->totalSampleCount) {
		return;
	}
	drflac_seek_to_sample(pFlac, location);
	flacprogress = location;
}


/**
 * Free Flac decoder.
 */
void exitFlac(void)
{
	drflac_close(pFlac);
}

/**
 * Checks if the input file is Flac
 *
 * \param in	Input file.
 * \return		0 if Flac file, else not or failure.
 */
int isFlac(const char* in)
{
	int err = -1;
	drflac* pFlac = drflac_open_file(in);

	if(pFlac != NULL)
		err = 0;

	drflac_close(pFlac);
	return err;
}

//Both libflac and drflac use a Metadata callback. TODO:: Add metadata callback.
void metadata_callback(void) {
	return;
}
