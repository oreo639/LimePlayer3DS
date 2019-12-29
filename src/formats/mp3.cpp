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

#include "mp3.hpp"

static size_t		buffSize;
static mpg123_handle*	mh;
static long		rate;
static int		channels;

static ssize_t replace_read(void * file, void * buffer, size_t length)
{
    return ((FileTransport*) file)->f_read(buffer, 1, length);
}

static off_t replace_lseek(void * file, off_t to, int whence)
{
    if (((FileTransport*) file)->f_seek(to, whence) < 0)
        return -1;

    return ((FileTransport*) file)->f_tell();
}

Mp3Decoder::Mp3Decoder(FileTransport *ftrans) : Decoder("Mp3") {
	int err = 0;
	int encoding = 0;

	if((err = mpg123_init()) != MPG123_OK)
		return;

	if((mh = mpg123_new(NULL, &err)) == NULL)
	{
		DEBUG("Error: %s\n", mpg123_plain_strerror(err));
		return;
	}

	mpg123_replace_reader_handle(mh, replace_read, replace_lseek, nullptr);

	if(mpg123_open_handle(mh, ftrans) != MPG123_OK || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
	{
		DEBUG("Trouble with mpg123: %s\n", mpg123_strerror(mh));
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

	
	this->IsInit = true;
}

Mp3Decoder::~Mp3Decoder(void) {
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
	this->IsInit = false;
}

void Mp3Decoder::Info(metaInfo_t* Meta) {
	mpg123_id3v1* v1;
	mpg123_id3v2* v2;
	if (mpg123_meta_check(mh) & MPG123_ID3) {
		mpg123_id3(mh, &v1, &v2);
	
		/*I will deal with this later.*/
		//if (v1->artist) {
		//	infoOut->fileMeta->authorCpright = strdup(v1->artist);
		//}
		if (mpg123_strlen(v2->artist, true)) {
			Meta->Artist.assign(v2->artist->p);
		}
		else {
			Meta->Artist.assign("(No Author-Mp3)");
		}
	}
	else {
		Meta->Artist.assign("(No Author-Mp3)");
	}
}

uint32_t Mp3Decoder::Position(void) {
	return mpg123_tell(mh);
}

uint32_t Mp3Decoder::Length(void) {
	return mpg123_length(mh);
}

void Mp3Decoder::Seek(uint32_t location)
{
	if(location > mpg123_length(mh)) {
		return;
	}
	mpg123_seek(mh, location, SEEK_SET);
}

uint32_t Mp3Decoder::Decode(void* buffer)
{
	size_t done = 0;
	mpg123_read(mh, reinterpret_cast<unsigned char*>(buffer), buffSize, &done);
	return done / (sizeof(int16_t));
}

uint32_t Mp3Decoder::Samplerate(void)
{
	return rate;
}

uint32_t Mp3Decoder::Buffsize(void)
{
	return buffSize;
}

int Mp3Decoder::Channels(void)
{
	return channels;
}

/*bool isMp3(const std::string& filename)
{
	mpg123_handle* tempMH;
	int err = 0;
	unsigned char tmpBuf[4096];
	mpg123_frameinfo info;
	size_t bytes_read;
	int tmpEncoding = 0;
	long tmpRate;
	int tmpChannels;

	if((err = mpg123_init()) != MPG123_OK)
		return false;

	if((tempMH = mpg123_new(NULL, &err)) == NULL)
	{
		DEBUG("Error: %s\n", mpg123_plain_strerror(err));
		return false;
	}

	if(mpg123_open(tempMH, filename.c_str()) != MPG123_OK ||
			mpg123_getformat(tempMH, &tmpRate, &tmpChannels, &tmpEncoding) != MPG123_OK)
	{
		DEBUG("Trouble with mpg123: %s\n", mpg123_strerror(tempMH));
		goto err;
	}

	while (1)
	{
		if (mpg123_getformat(tempMH, &tmpRate, &tmpChannels, &tmpEncoding) < 0)
			goto err;

		int ret = mpg123_read(tempMH, tmpBuf, sizeof(tmpBuf), &bytes_read);

		if (ret == MPG123_NEW_FORMAT)
			continue;
		if (ret < 0)
			goto err;

		if (mpg123_info(tempMH, &info) < 0)
			goto err;

		return tempMH;
	}


	mpg123_delete(tempMH);
	mpg123_exit();
	return true;
err:
	mpg123_delete(tempMH);
	mpg123_exit();
	return false;
}*/
