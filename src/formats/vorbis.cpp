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

#include "vorbis.hpp"

VorbisDecoder::VorbisDecoder(const char* filename) : Decoder("Vorbis") {
	if((f = fopen(filename, "rb")) == NULL)
		return;

	if(ov_open(f, &vorbisFile, NULL, 0) < 0)
		return;

	if((vi = ov_info(&vorbisFile, -1)) == NULL)
		return;
	
	mIsInit = true;
}

VorbisDecoder::~VorbisDecoder(void) {
	ov_clear(&vorbisFile);
	fclose(f);
	mIsInit = false;
}

void ProcessInfo(vorbis_comment *comment, metaInfo_t* Meta) {
	const char *ret;

	if ((ret = vorbis_comment_query(comment, const_cast<char*>("title"), 0)))
		Meta->Title.assign(ret);

	if ((ret = vorbis_comment_query(comment, const_cast<char*>("artist"), 0)))
		Meta->Artist.assign(ret);

	if (Meta->Artist.empty())
		Meta->Artist.assign("(No Author-Vorbis)");
}

void VorbisDecoder::UpdateInfo(metaInfo_t* Meta) {
	vorbis_comment *comment = ov_comment(&vorbisFile, -1);

	const char *old_title = Meta->Title.c_str();
	const char *new_title = vorbis_comment_query(comment, const_cast<char*>("title"), 0);

	if (!new_title || (old_title && strcmp(old_title, new_title)==0))
        	return;

	ProcessInfo(comment, Meta);
}

void VorbisDecoder::GetInfo(metaInfo_t* Meta) {
	vorbis_comment *comment = ov_comment(&vorbisFile, -1);

	ProcessInfo(comment, Meta);
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

uint64_t VorbisDecoder::fillVorbisBuffer(char* bufferOut)
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
