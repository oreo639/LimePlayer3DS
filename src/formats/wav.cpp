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
#define DR_WAV_IMPLEMENTATION

#include "wav.hpp"

WavDecoder::WavDecoder(const char* filename) : Decoder("Wave") {
	pWav = drwav_open_file(filename);
	wavprogress = 0;
	if (pWav == NULL)
		return;
	
	mIsInit = true;
}

WavDecoder::~WavDecoder(void) {
	drwav_close(pWav);
	mIsInit = false;
}

void WavDecoder::GetInfo(metaInfo_t* Meta) {
	Meta->Artist.assign("(No Author-Wav)");
}

uint32_t WavDecoder::Position(void) {
	return wavprogress;
}

uint32_t WavDecoder::Length(void) {
	return pWav->totalSampleCount;
}

void WavDecoder::Seek(uint32_t location)
{
	if(location > pWav->totalSampleCount) {
		return;
	}
	drwav_seek_to_sample(pWav, location);
	wavprogress = location;
}

uint32_t WavDecoder::Decode(void* buffer)
{
	uint64_t out = drwav_read_s16(pWav, buffSize, reinterpret_cast<drwav_int16*>(buffer));
	wavprogress += out;
	return out;
}

uint32_t WavDecoder::Samplerate(void)
{
	return pWav->sampleRate;
}

uint32_t WavDecoder::Buffsize(void)
{
	return buffSize;
}

int WavDecoder::Channels(void)
{
	return pWav->channels;
}
