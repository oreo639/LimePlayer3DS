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
#include <cstring>
#include <cassert>

#include <FLAC/stream_decoder.h>

#include "flac.hpp"
#include "flacwrap/flacwrap.hpp"
#include "error.hpp"

static int LibInit = false;
flacwrap_t flacstruct;
static const size_t	buffSize = 4096 * 16;

FlacDecoder::FlacDecoder(const char* filename) {
	flacstruct.dFlac = FLAC__stream_decoder_new();
	if(FLACWRAP_init(filename, &flacstruct)) {
		return;
	}
	if (flacstruct.dFlac == NULL)
		LibInit = false;
	else
		LibInit = true;
}

FlacDecoder::~FlacDecoder(void) {
	FLACWRAP_free(&flacstruct);
	FLAC__stream_decoder_delete(flacstruct.dFlac);
	LibInit = false;
}

bool FlacDecoder::IsInit(void) {
	return LibInit;
}

void FlacDecoder::Info(std::string& copyright) {
	copyright.assign("(No Author-Flac)", strlen("(No Author-Flac)"));
}

uint32_t FlacDecoder::Position(void) {
	return 0;
}

uint32_t FlacDecoder::Length(void) {
	return flacstruct.mFlac.total_samples;
}

void FlacDecoder::Seek(uint32_t location)
{
	if(location > flacstruct.mFlac.total_samples) {
		return;
	}
	FLAC__stream_decoder_seek_absolute(flacstruct.dFlac, location);
}

uint32_t FlacDecoder::Decode(void* buffer)
{
	return FLACWRAP_decode(&flacstruct, reinterpret_cast<int16_t*>(buffer), buffSize);
}

uint32_t FlacDecoder::Samplerate(void)
{
	return flacstruct.mFlac.sample_rate;
}

uint32_t FlacDecoder::Spf(void* buffer)
{
	return Decode(buffer)/Channels();
}


uint32_t FlacDecoder::Buffsize(void)
{
	return buffSize;
}

int FlacDecoder::Channels(void)
{
	return flacstruct.mFlac.channels;
}

/*int isFlac(const char* in)
{
	int err = -1;
	drflac* pFlac = drflac_open_file(in);

	if(pFlac != NULL)
		err = 0;

	drflac_close(pFlac);
	return err;
}*/
