#define DR_FLAC_IMPLEMENTATION
#include <dr_libs/dr_flac.h>

#include "flac.hpp"

static int LibInit = false;
static drflac*		pFlac;
static const size_t	buffSize = 16 * 1024;
uint32_t flacprogress; //Credit Tangerine.

FlacDecoder::FlacDecoder(const char* filename) {
	pFlac = drflac_open_file(filename);
	flacprogress = 0;
	if (pFlac == NULL)
		LibInit = false;
	else
		LibInit = true;
}

FlacDecoder::~FlacDecoder(void) {
	drflac_close(pFlac);
	LibInit = false;
}

bool FlacDecoder::IsInit(void) {
	return LibInit;
}

void FlacDecoder::Info(std::string& copyright) {
	copyright.assign("(No Author-Flac)", strlen("(No Author-Flac)"));
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

uint32_t FlacDecoder::Spf(void* buffer)
{
	return Decode(buffer)/Channels();
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
