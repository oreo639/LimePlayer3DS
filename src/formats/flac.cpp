/*Copyright 2018-2019 Oreo639
*
*Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the *"Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, *distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to *the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH *THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
