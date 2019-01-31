#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3.h"

static size_t*			buffSize;
static mpg123_handle	*mh = NULL;
static uint32_t			rate;
static uint8_t			channels;
//char filename[100];

/**
 * Initialise MP3 decoder.
 *
 * \param	file	Location of MP3 file to play.
 * \return			0 on success, else failure.
 */
int initMp3(playbackInfo_t* info)
{
	//strcpy(filename, file);
	int err = 0;
	int encoding = 0;

	if((err = mpg123_init()) != MPG123_OK)
		return err;

	if((mh = mpg123_new(NULL, &err)) == NULL)
	{
		printf("Error: %s\n", mpg123_plain_strerror(err));
		return err;
	}

	if(mpg123_open(mh, info->file) != MPG123_OK ||
			mpg123_getformat(mh, (long *) &rate, (int *) &channels, &encoding) != MPG123_OK)
	{
		printf("Trouble with mpg123: %s\n", mpg123_strerror(mh));
		return -1;
	}

	/*
	 * Ensure that this output format will not change (it might, when we allow
	 * it).
	 */
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);

	/*
	 * Buffer could be almost any size here, mpg123_outblock() is just some
	 * recommendation. The size should be a multiple of the PCM frame size.
	 */
	*buffSize = mpg123_outblock(mh) * 16;

	return 0;
}

void mp3GetBuffSize(size_t *sizeOut) {
	buffSize = sizeOut;
}

/**
 * Get sampling rate of MP3 file.
 *
 * \return	Sampling rate.
 */
uint32_t rateMp3(void)
{
	return rate;
}

/**
 * Get number of channels of MP3 file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelMp3(void)
{
	return channels;
}

/**
 * Decode part of open MP3 file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeMp3(void* buffer)
{
	size_t done = 0;
	mpg123_read(mh, buffer, *buffSize, &done);
	return done / (sizeof(int16_t));
}

void infoMp3(playbackInfo_t* infoOut) {
	mpg123_id3v1* v1;
	mpg123_id3v2* v2;
	mpg123_id3(mh, &v1, &v2);
	
	/*Not going to deal with this bs. For now ofc.*/
	//if (v1->artist) {
	//infoOut->fileMeta->authorCpright = strdup(v1->artist);
	//}
	if (mpg123_strlen(v2->artist, true)) {
	infoOut->fileMeta->authorCpright = strdup(v2->artist->p);
	}
	else {
	infoOut->fileMeta->authorCpright = strdup("(No Author-Mp3)");
	}
}

uint32_t posMp3(void) {
	return mpg123_tell(mh);
}

uint32_t lengthMp3(void) {
	return mpg123_length(mh);
}

void seekMp3(uint32_t location)
{
	if(location > lengthMp3()) {
		return;
	}
	mpg123_seek(mh, location, SEEK_SET);
}


/**
 * Free MP3 decoder.
 */
void exitMp3(void)
{
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}
