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
#include <stdlib.h>
#include <string.h>

#include <opusfile.h>

#include "opusstream.hpp"

#include "error.hpp"

static bool isSeekable;
static OpusDecoder*		opusDecoder;
static const OpusHead*		opusHead;
static const size_t		buffSize = 32 * 1024;

uint64_t fillOpusBuffer(uint8_t* inbuffer, uint32_t inbuffsize, int16_t* bufferOut);


OpusStreamDecoder::OpusStreamDecoder(uint8_t* inbuffer, uint32_t inbufsize) {
	int err = 0;

	if (!(opusDecoder = opus_decoder_create(48000, 2, &err))) {
		DEBUG("opus decoder initalization failed with error %d.\n", err);
		return;
	}

	DEBUG("OpusDec initalized.\n");
	
	this->IsInit = true;
}

OpusStreamDecoder::~OpusStreamDecoder(void) {
	opus_decoder_destroy(opusDecoder);
	this->IsInit = false;
}

void OpusStreamDecoder::Info(musinfo_t* Meta) {
	return;
}

uint32_t OpusStreamDecoder::Position(void) {
	return 0;
}

uint32_t OpusStreamDecoder::Length(void) {
	return 0;
}

void OpusStreamDecoder::Seek(uint32_t location)
{
	return;
}

uint32_t OpusStreamDecoder::Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer)
{
	return fillOpusBuffer(inbuffer, inbuffsize, static_cast<int16_t*>(outbuffer));
}

uint32_t OpusStreamDecoder::Samplerate(void)
{
	return 48000;
}

uint32_t OpusStreamDecoder::Buffsize(void)
{
	return buffSize;
}

int OpusStreamDecoder::Channels(void)
{
	return 2;
}

int isOpusStream(uint8_t* inbuffer, uint32_t inbufsize)
{
	int err = 0;
	err = op_test(NULL, inbuffer, inbufsize);
	return err;
}

uint64_t fillOpusBuffer(uint8_t* inbuffer, uint32_t inbuffsize, int16_t* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = buffSize;

	while(samplesToRead > 0)
	{
		int samplesJustRead = opus_decode(opusDecoder, inbuffer, inbuffsize, bufferOut,
						samplesToRead > 120*48*2 ? 120*48*2 : samplesToRead, 0);

		DEBUG("Output of samplesJustRead: %d", samplesJustRead);

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
