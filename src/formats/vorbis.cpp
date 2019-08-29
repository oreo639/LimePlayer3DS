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

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include "vorbis.hpp"

static OggVorbis_File		vorbisFile;
static vorbis_info		*vi;
static FILE			*f;
static const size_t		buffSize = 8 * 4096;

uint64_t fillVorbisBuffer(char* bufferOut);

VorbisDecoder::VorbisDecoder(const char* filename) {
	if((f = fopen(filename, "rb")) == NULL)
		return;

	if(ov_open(f, &vorbisFile, NULL, 0) < 0)
		return;

	if((vi = ov_info(&vorbisFile, -1)) == NULL)
		return;
	
	this->IsInit = true;
}

VorbisDecoder::~VorbisDecoder(void) {
	ov_clear(&vorbisFile);
	fclose(f);
	this->IsInit = false;
}

void VorbisDecoder::Info(musinfo_t* Meta) {
	const char *ret = NULL;
	vorbis_comment *comment = ov_comment(&vorbisFile, -1);

	if ((ret = vorbis_comment_query(comment, const_cast<char*>("artist"), 0)))
		Meta->authorCpright.assign(ret);

	if (Meta->authorCpright.empty())
		Meta->authorCpright.assign("(No Author-Vorbis)");
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
