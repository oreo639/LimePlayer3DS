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
#define DR_FLAC_IMPLEMENTATION
#include <dr_libs/dr_flac.h>

#include "flac.hpp"

static drflac*		pFlac;
static const size_t	buffSize = 16 * 1024;
uint32_t flacprogress; //Credit Tangerine.

FlacDecoder::FlacDecoder(const char* filename) {
	pFlac = drflac_open_file(filename);
	flacprogress = 0;
	if (pFlac == NULL)
		return;

	this->IsInit = true;
}

FlacDecoder::~FlacDecoder(void) {
	drflac_close(pFlac);
	this->IsInit = false;
}

void FlacDecoder::Info(musinfo_t* Meta) {
	Meta->authorCpright.assign("(No Author-Flac)", strlen("(No Author-Flac)"));
}

uint32_t FlacDecoder::Position(void) {
	return flacprogress;
}

uint32_t FlacDecoder::Length(void) {
	return pFlac->totalSampleCount;
}

void FlacDecoder::Seek(uint32_t location)
{
	if(location > pFlac->totalSampleCount) {
		return;
	}
	drflac_seek_to_sample(pFlac, location);
	flacprogress = location;
}

uint32_t FlacDecoder::Decode(void* buffer)
{
	uint64_t out = drflac_read_s16(pFlac, buffSize, (int16_t*)buffer);
	flacprogress += out;
	return out;
}

uint32_t FlacDecoder::Samplerate(void)
{
	return pFlac->sampleRate;
}


uint32_t FlacDecoder::Buffsize(void)
{
	return buffSize;
}

int FlacDecoder::Channels(void)
{
	return pFlac->channels;
}

int isFlac(const char* in)
{
	int err = -1;
	drflac* pFlac = drflac_open_file(in);

	if(pFlac != NULL)
		err = 0;

	drflac_close(pFlac);
	return err;
}
