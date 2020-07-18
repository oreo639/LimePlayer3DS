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

#include <string.h>

#include "flac.hpp"

FlacDecoder::FlacDecoder(const char* filename) : Decoder("FLAC") {
	dFlac = FLAC__stream_decoder_new();
	cFlac = new callback_info;

	FLAC__StreamDecoderInitStatus ret;	
	if((ret = FLAC__stream_decoder_init_file(dFlac,
		filename,
		write_callback,
		metadata_callback,
		error_callback,
		cFlac)) != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
			DEBUG("Could not initialize the main FLAC decoder: %s(%d)\n",
				FLAC__StreamDecoderInitStatusString[ret], ret);
			return;
	}

	FLAC__stream_decoder_process_until_end_of_metadata(dFlac);

	mIsInit = true;
}

FlacDecoder::~FlacDecoder(void) {
	FLAC__stream_decoder_finish(dFlac);
	FLAC__stream_decoder_delete(dFlac);
	delete cFlac;
	mIsInit = false;
}

void FlacDecoder::GetInfo(metaInfo_t* Meta) {
	Meta->Artist.assign("(No Author-Flac)", strlen("(No Author-Flac)"));
}

uint32_t FlacDecoder::Position(void) {
	FLAC__uint64 pos = 0;
	FLAC__stream_decoder_get_decode_position(dFlac, &pos);
	return pos;
}

uint32_t FlacDecoder::Length(void) {
	return cFlac->total_samples;
}

void FlacDecoder::Seek(uint32_t location)
{
	if(location > cFlac->total_samples) {
		return;
	}
	FLAC__stream_decoder_seek_absolute(dFlac, location);
}

uint32_t FlacDecoder::Decode(void* buffer)
{
	return FLAC_decode(dFlac, cFlac, static_cast<int16_t*>(buffer), BUFFER_SIZE);
}

uint32_t FlacDecoder::Samplerate(void)
{
	return cFlac->sample_rate;
}

uint32_t FlacDecoder::Buffsize(void)
{
	return BUFFER_SIZE;
}

int FlacDecoder::Channels(void)
{
	return cFlac->channels;
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
