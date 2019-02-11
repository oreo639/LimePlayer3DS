#include <stdlib.h>
#include <string.h>

#include <opusfile.h>

#include "opus.hpp"

static OggOpusFile*		opusFile;
static const OpusHead*	opusHead;
static const size_t		buffSize = 32 * 1024;

static int LibInit = false;

uint64_t fillOpusBuffer(int16_t* bufferOut);


OpusDecoder::OpusDecoder(const char* filename) {
	int err = 0;

	if((opusFile = op_open_file(filename, &err)) == NULL)
		return;

	if((err = op_current_link(opusFile)) < 0)
		return;

	opusHead = op_head(opusFile, err);
	
	LibInit = true;
}

OpusDecoder::~OpusDecoder(void) {
	op_free(opusFile);
	LibInit = false;
}

bool OpusDecoder::IsInit(void) {
	return LibInit;
}

void OpusDecoder::Info(std::string& copyright) {
	copyright.assign("(No Author-Opus)", strlen("(No Author-Opus)"));
}

uint32_t OpusDecoder::Position(void) {
	return op_pcm_tell(opusFile);
}

uint32_t OpusDecoder::Length(void) {
	return op_pcm_total(opusFile, -1);
}

void OpusDecoder::Seek(uint32_t location)
{
	if(location > op_pcm_total(opusFile, -1)) {
		return;
	}
	op_pcm_seek(opusFile, location);
}

uint32_t OpusDecoder::Decode(void* buffer)
{
	return fillOpusBuffer(static_cast<int16_t*>(buffer));
}

uint32_t OpusDecoder::Samplerate(void)
{
	return 48000;
}


uint32_t OpusDecoder::Buffsize(void)
{
	return buffSize;
}

int OpusDecoder::Channels(void)
{
	return 2;
}

int isOpus(const char* in)
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
