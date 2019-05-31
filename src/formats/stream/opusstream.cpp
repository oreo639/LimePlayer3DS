#include <stdlib.h>
#include <string.h>

#include <opusfile.h>

#include "opus.hpp"

static OggOpusFile*		opusFile;
static const OpusHead*	opusHead;
static const size_t		buffSize = 32 * 1024;

static int LibInit = false;

uint64_t fillOpusBuffer(int16_t* bufferOut);


OpusStreamDecoder::OpusStreamDecoder(uint8_t* inbuffer, uint32_t inbufsize) {
	int err = 0;

	if((opusFile = op_open_memory(static_cast<unsigned char*>(inbuffer), inbufsize, &err)) == NULL)
		return;

	if((err = op_current_link(opusFile)) < 0)
		return;

	opusHead = op_head(opusFile, err);
	
	LibInit = true;
}

OpusStreamDecoder::~OpusStreamDecoder(void) {
	op_free(opusFile);
	LibInit = false;
}

bool OpusStreamDecoder::IsInit(void) {
	return LibInit;
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

uint32_t OpusStreamDecoder::Spf(void* buffer)
{
	return Decode(buffer)/Channels();
}

uint32_t OpusStreamDecoder::Buffsize(void)
{
	return buffSize;
}

int OpusStreamDecoder::Channels(void)
{
	return 2;
}

int isOpus(uint8_t* inbuffer, uint32_t inbufsize)
{
	int err = 0;
	OggOpusFile* opusTest = op_test_file(in, &err);

	op_free(opusTest);
	return err;
}

uint64_t fillOpusBuffer(int16_t* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = buffSize;

	while(samplesToRead > 0)
	{
		int samplesJustRead = op_read_stereo(opusFile, bufferOut,
				samplesToRead > 120*48*2 ? 120*48*2 : samplesToRead);

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
