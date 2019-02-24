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
	// There doesn't seem to be a better solution to this.
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
