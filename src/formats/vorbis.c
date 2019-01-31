#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "vorbis.h"

static OggVorbis_File		vorbisFile;
static vorbis_info		*vi;
static FILE			*f;
static const size_t		buffSize = 8 * 4096;



/**
 * Initialise Vorbis decoder.
 *
 * \param	file	Location of vorbis file to play.
 * \return			0 on success, else failure.
 */
int initVorbis(playbackInfo_t* info)
{
	int err = -1;

	if((f = fopen(info->file, "rb")) == NULL)
		goto out;

	if(ov_open(f, &vorbisFile, NULL, 0) < 0)
		goto out;

	if((vi = ov_info(&vorbisFile, -1)) == NULL)
		goto out;

	err = 0;

out:
	return err != 0 ? -1 : buffSize;
}

/**
 * Get sampling rate of Vorbis file.
 *
 * \return	Sampling rate.
 */
uint32_t rateVorbis(void)
{
	return vi->rate;
}

/**
 * Get number of channels of Vorbis file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelVorbis(void)
{
	return vi->channels;
}

/**
 * Decode part of open Vorbis file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel. 0 for end of file, negative
 *					for error.
 */
uint64_t decodeVorbis(void* buffer)
{
	uint64_t out = fillVorbisBuffer(buffer);
	return out;
}

void infoVorbis(playbackInfo_t* infoOut) {
	//vorbis_comment *vc = ov_comment(&vorbisFile,-1);
	infoOut->fileMeta->authorCpright = strdup("(No Author-Vorbis)");
}

uint32_t posVorbis(void) {
	return ov_pcm_tell(&vorbisFile);
}

uint32_t lengthVorbis(void) {
	return ov_pcm_total(&vorbisFile, -1);
}

void seekVorbis(uint32_t location)
{
	if(location > ov_pcm_total(&vorbisFile, -1)) {
		return;
	}
	ov_pcm_seek(&vorbisFile, location);
}

/**
 * Free Vorbis decoder.
 */
void exitVorbis(void)
{
	ov_clear(&vorbisFile);
	fclose(f);
}

/**
 * Decode Vorbis file to fill buffer.
 *
 * \param opusFile		File to decode.
 * \param bufferOut		Pointer to buffer.
 * \return				Samples read per channel.
 */
uint64_t fillVorbisBuffer(char* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = buffSize;

	while(samplesToRead > 0)
	{
		static int current_section;
		int samplesJustRead =
			ov_read(&vorbisFile, bufferOut,
					samplesToRead > 4096 ? 4096	: samplesToRead,
					&current_section);

		if(samplesJustRead < 0)
			return samplesJustRead;
		else if(samplesJustRead == 0)
		{
			/* End of file reached. */
			break;
		}

		samplesRead += samplesJustRead;
		samplesToRead -= samplesJustRead;
		bufferOut += samplesJustRead;
	}

	return samplesRead / sizeof(int16_t);
}

/**
 * Checks if the input file is Vorbis.
 *
 * \param in	Input file.
 * \return		0 if Vorbis file, else not or failure.
 */
int isVorbis(const char *in)
{
	FILE *ft = fopen(in, "r");
	OggVorbis_File testvf;
	int err;

	if(ft == NULL)
		return -1;

	err = ov_test(ft, &testvf, NULL, 0);

	ov_clear(&testvf);
	fclose(ft);
	return err;
}
