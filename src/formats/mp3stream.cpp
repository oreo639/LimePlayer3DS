/*   LimePlayer3DS FOSS graphcal music player for the Nintendo 3DS.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <mpg123.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mp3stream.hpp"

static size_t				buffSize;
static mpg123_handle*			mh;
static uint32_t			rate;
static uint8_t			channels;

static int LibInit = false;

Mp3StreamDecoder::Mp3StreamDecoder(uint8_t* inbuffer, uint32_t inbufsize) {
	int err = 0;
	int encoding = 0;

	if((err = mpg123_init()) != MPG123_OK)
		return;

	if((mh = mpg123_new(NULL, &err)) == NULL)
	{
		printf("Error: %s\n", mpg123_plain_strerror(err));
		return;
	}

	if (mpg123_open_feed(mh) != MPG123_OK ||
		mpg123_feed(mh, (const unsigned char*) inbuffer, inbufsize) != MPG123_OK ||
		mpg123_getformat(mh, (long *) &rate, (int *) &channels, &encoding) != MPG123_OK)
	{
		printf("Trouble with mpg123: %s\n", mpg123_strerror(mh));
		return;
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
	buffSize = mpg123_outblock(mh) * 16;

	LibInit = true;
}

Mp3StreamDecoder::~Mp3StreamDecoder(void) {
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
	LibInit = false;
}

bool Mp3StreamDecoder::IsInit(void) {
	return LibInit;
}

void Mp3StreamDecoder::Info(musinfo_t* Meta) {
	mpg123_id3v1* v1;
	mpg123_id3v2* v2;
	mpg123_id3(mh, &v1, &v2);

	/*Not going to deal with this bs. For now ofc.*/
	//if (v1->artist) {
	//infoOut->fileMeta->authorCpright = strdup(v1->artist);
	//}
	if (mpg123_strlen(v2->artist, true)) {
		Meta->authorCpright.assign(v2->artist->p, strlen(v2->artist->p));
	}
	else {
		Meta->authorCpright.assign("(No Author-Mp3)", strlen("(No Author-Mp3)"));
	}
}

uint32_t Mp3StreamDecoder::Position(void) {
	return mpg123_tell(mh);
}

uint32_t Mp3StreamDecoder::Length(void) {
	return mpg123_length(mh);
}

void Mp3StreamDecoder::Seek(uint32_t location)
{
	return; // Cannot seek in a stream.
}

uint32_t Mp3StreamDecoder::Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer)
{
	size_t done = 0;
	mpg123_feed(mh, reinterpret_cast<unsigned char*>(inbuffer), inbuffsize);
	mpg123_read(mh, reinterpret_cast<unsigned char*>(outbuffer), buffSize, &done);
	return done / (sizeof(int16_t));
}

uint32_t Mp3StreamDecoder::Samplerate(void)
{
	return rate;
}

uint32_t Mp3StreamDecoder::Spf(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer)
{
	return Decode(inbuffer, inbuffsize, outbuffer)/Channels();
}

uint32_t Mp3StreamDecoder::Buffsize(void)
{
	return buffSize;
}

int Mp3StreamDecoder::Channels(void)
{
	return channels;
}
