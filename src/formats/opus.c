#include <stdlib.h>
#include <string.h>

#include "opus.h"

static OggOpusFile*		opusFile;
static const OpusHead*	opusHead;
static const size_t		buffSize = 32 * 1024;

/**
 * Initialise Opus decoder.
 *
 * \param	file	Location of opus file to play.
 * \return			0 on success, else failure.
 */
int initOpus(playbackInfo_t* info)
{
	int err = 0;

	if((opusFile = op_open_file(info->file, &err)) == NULL)
		goto out;

	if((err = op_current_link(opusFile)) < 0)
		goto out;

	opusHead = op_head(opusFile, err);

out:
	return opusFile == NULL ? -1 : buffSize;
}

/**
 * Get sampling rate of Opus file.
 *
 * \return	Sampling rate. Should be 48000.
 */
uint32_t rateOpus(void)
{
	return 48000;
}

/**
 * Get number of channels of Opus file.
 *
 * \return	Number of channels for opened file, so always be 2.
 */
uint8_t channelOpus(void)
{
	/* Opus decoder always returns stereo stream */
	return 2;
}

/**
 * Decode part of open Opus file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel. 0 for end of file, negative
 *					for error.
 */
uint64_t decodeOpus(void* buffer)
{
	return fillOpusBuffer(buffer);
}

void infoOpus(playbackInfo_t* infoOut) {
	//vorbis_comment *vc = ov_comment(&vorbisFile,-1);
	infoOut->fileMeta->authorCpright = strdup("(No Author-Opus)");
}

uint32_t posOpus(void) {
	return op_pcm_tell(opusFile);
}

uint32_t lengthOpus(void) {
	return op_pcm_total(opusFile, -1);
}

void seekOpus(uint32_t location)
{
	if(location > lengthOpus()) {
		return;
	}
	op_pcm_seek(opusFile, location);
}

/**
 * Free Opus decoder.
 */
void exitOpus(void)
{
	op_free(opusFile);
}

/**
 * Decode Opus file to fill buffer.
 *
 * \param bufferOut		Pointer to buffer.
 * \return				Samples read per channel.
 */
uint64_t fillOpusBuffer(int16_t* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = buffSize;

	while(samplesToRead > 0)
	{
		int samplesJustRead = op_read_stereo(opusFile, bufferOut,
				samplesToRead > 120*48*2 ? 120*48*2 : samplesToRead);

		if(samplesJustRead < 0)
			return samplesJustRead;
		else if(samplesJustRead == 0)
		{
			/* End of file reached. */
			break;
		}

		samplesRead += samplesJustRead * 2;
		samplesToRead -= samplesJustRead * 2;
		bufferOut += samplesJustRead * 2;
	}

	return samplesRead;
}

/**
 * Checks if the input file is Opus.
 *
 * \param in	Input file.
 * \return		0 if Opus file, else not or failure.
 */
int isOpus(const char* in)
{
	int err = 0;
	OggOpusFile* opusTest = op_test_file(in, &err);

	op_free(opusTest);
	return err;
}
