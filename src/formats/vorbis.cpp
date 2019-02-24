#include <stdlib.h>
#include <string.h>

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include "vorbis.hpp"

static OggVorbis_File		vorbisFile;
static vorbis_info		*vi;
static FILE			*f;
static const size_t		buffSize = 8 * 4096;

static int LibInit = false;
uint64_t fillVorbisBuffer(char* bufferOut);

VorbisDecoder::VorbisDecoder(const char* filename) {
	if((f = fopen(filename, "rb")) == NULL)
		return;

	if(ov_open(f, &vorbisFile, NULL, 0) < 0)
		return;

	if((vi = ov_info(&vorbisFile, -1)) == NULL)
		return;
	
	LibInit = true;
}

VorbisDecoder::~VorbisDecoder(void) {
	ov_clear(&vorbisFile);
	fclose(f);
	LibInit = false;
}

bool VorbisDecoder::IsInit(void) {
	return LibInit;
}

void VorbisDecoder::Info(std::string& copyright) {
	copyright.assign("(No Author-Vorbis)", strlen("(No Author-Vorbis)"));
}

uint32_t VorbisDecoder::Position(void) {
	return ov_pcm_tell(&vorbisFile);
}

uint32_t VorbisDecoder::Length(void) {
	return ov_pcm_total(&vorbisFile, -1);
}

void VorbisDecoder::Seek(uint32_t location)
{
	if(location > ov_pcm_total(&vorbisFile, -1)) {
		return;
	}
	ov_pcm_seek(&vorbisFile, location);
}

uint32_t VorbisDecoder::Decode(void* buffer)
{
	return fillVorbisBuffer(reinterpret_cast<char*>(buffer));
}

uint32_t VorbisDecoder::Samplerate(void)
{
	return vi->rate;
}

uint32_t VorbisDecoder::Spf(void* buffer)
{
	return 4096;
}


uint32_t VorbisDecoder::Buffsize(void)
{
	return buffSize;
}

int VorbisDecoder::Channels(void)
{
	return vi->channels;
}

int isVorbis(const char *in)
{
	FILE *ft = fopen(in, "r");
	OggVorbis_File testvf;
	int err;

	if(ft == NULL)
		return -1;

	err = ov_test(ft, &testvf, NULL, 0);

	ov_clear(&testvf);
	fclose(ft);
	return err;
}

uint64_t fillVorbisBuffer(char* bufferOut)
{
	uint64_t samplesRead = 0;
	int samplesToRead = buffSize;

	while(samplesToRead > 0)
	{
		static int current_section;
		int samplesJustRead =
			ov_read(&vorbisFile, bufferOut,
					samplesToRead > 4096 ? 4096	: samplesToRead,
					&current_section);

		if(samplesJustRead < 0)
			return samplesJustRead;
		else if(samplesJustRead == 0)
		{
			/* End of file reached. */
			break;
		}

		samplesRead += samplesJustRead;
		samplesToRead -= samplesJustRead;
		bufferOut += samplesJustRead;
	}

	return samplesRead / sizeof(int16_t);
}
