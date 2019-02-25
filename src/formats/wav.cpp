/*Copyright 2018-2019 Oreo639
*
*Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the *"Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, *distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to *the following conditions:
*
*The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
*
*THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF *MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR *ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH *THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define DR_WAV_IMPLEMENTATION
#include <dr_libs/dr_wav.h>

#include "wav.hpp"

static int LibInit = false;
static const size_t	buffSize	= 16 * 1024;
static drwav*		pWav		= NULL;
static uint32_t		wavprogress; //credit tangerine

WavDecoder::WavDecoder(const char* filename) {
	pWav = drwav_open_file(filename);
	wavprogress = 0;
	if (pWav == NULL)
		LibInit = false;
	else
		LibInit = true;
}

WavDecoder::~WavDecoder(void) {
	drwav_close(pWav);
	LibInit = false;
}

bool WavDecoder::IsInit(void) {
	return LibInit;
}

void WavDecoder::Info(std::string& copyright) {
	copyright.assign("(No Author-Wav)", strlen("(No Author-Wav)"));
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

uint32_t WavDecoder::Spf(void* buffer)
{
	return Decode(buffer)/Channels();
}

uint32_t WavDecoder::Buffsize(void)
{
	return buffSize;
}

int WavDecoder::Channels(void)
{
	return pWav->channels;
}
