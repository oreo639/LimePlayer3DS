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
#include <assert.h>

#include "flac.hpp"

#define BUFFER_SIZE_SAMP (FLAC__MAX_BLOCK_SIZE * FLAC__MAX_CHANNELS)
#define BUFTYPE_BITS 16

static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);
static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);

/* This is a mix between squeeze_audio from audacious and convertBuffersGeneric from scummvm */
static void convertBuffersGeneric(int32_t* src, int16_t* dst, unsigned count, unsigned res)
{
	assert(BUFTYPE_BITS == 16);
	int32_t* rp = src;
	//int8_t* wp1 = dst;
	int16_t* wp2 = dst;
	//int32_t* wp3 = dst;

	if (res < BUFTYPE_BITS) {
		const uint8_t kPower = (uint8_t)(BUFTYPE_BITS - res);

		for (unsigned i = 0; i < count; i++)
			*wp2++ = rp[i] << kPower;
	} else if (res > BUFTYPE_BITS) {
		const uint8_t kPower = (uint8_t)(res - BUFTYPE_BITS);

		for (unsigned i = 0; i < count; i++)
			*wp2++ = rp[i] >> kPower;
	} else {
		for (unsigned i = 0; i < count; i++)
			*wp2++ = rp[i];
	}
}

static inline void callbackFrameClear(struct flac_callback_info* cinfo)
{
	cinfo->buffer_used = 0;
	memset(cinfo->output_buffer, 0, BUFFER_SIZE_SAMP);
	cinfo->write_pointer = &cinfo->output_buffer[0];
}

FlacDecoder::FlacDecoder(const char* filename) : Decoder("FLAC") {
	dFlac = FLAC__stream_decoder_new();

	FLAC__StreamDecoderInitStatus ret;	
	if((ret = FLAC__stream_decoder_init_file(dFlac,
		filename,
		write_callback,
		metadata_callback,
		error_callback,
		&cFlac)) != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
			DEBUG("Could not initialize the main FLAC decoder: %s(%d)\n",
				FLAC__StreamDecoderInitStatusString[ret], ret);
			return;
	}

	FLAC__stream_decoder_process_until_end_of_metadata(dFlac);

	cFlac.output_buffer = new int32_t[BUFFER_SIZE_SAMP];

	mIsInit = true;
}

FlacDecoder::~FlacDecoder(void) {
	delete[] cFlac.output_buffer;
	FLAC__stream_decoder_finish(dFlac);
	FLAC__stream_decoder_delete(dFlac);
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
	return cFlac.total_samples;
}

void FlacDecoder::Seek(uint32_t location)
{
	if(location > cFlac.total_samples) {
		return;
	}
	FLAC__stream_decoder_seek_absolute(dFlac, location);
}

uint32_t FlacDecoder::Decode(void* buffer)
{
	if (FLAC__stream_decoder_get_state(dFlac) == FLAC__STREAM_DECODER_END_OF_STREAM)
		return 0;

	callbackFrameClear(&cFlac);

	/* Try to decode a single frame of audio */
	if (FLAC__stream_decoder_process_single(dFlac) == false)
	{
		DEBUG("Error while decoding!\n");
		return 0;
	}

	/* Copy data to audio buffer and if necessary, convert the data */
	convertBuffersGeneric(cFlac.output_buffer, static_cast<int16_t*>(buffer), cFlac.buffer_used, cFlac.bits_per_sample);
	return cFlac.buffer_used;
}

uint32_t FlacDecoder::Samplerate(void)
{
	return cFlac.sample_rate;
}

uint32_t FlacDecoder::Buffsize(void)
{
	return BUFFER_SIZE_SAMP;
}

int FlacDecoder::Channels(void)
{
	return cFlac.channels;
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

static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	flac_callback_info *info = (flac_callback_info*)client_data;

	if (info->channels != frame->header.channels ||
		info->sample_rate != frame->header.sample_rate)
	{
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;
	}

	for (unsigned sample = 0; sample < frame->header.blocksize; sample++)
	{
		for (unsigned channel = 0; channel < frame->header.channels; channel++)
		{
			*(info->write_pointer++) = buffer[channel][sample];
			info->buffer_used += 1;
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
	const char *str;
	DEBUG("FLAC: decoder error callback was called: %d\n", status);
	if ((str = FLAC__StreamDecoderErrorStatusString[status]))
		DEBUG("FLAC: status of %d: %s\n", status, str);
}

static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	flac_callback_info *info = (flac_callback_info*)client_data;

	if (metadata->type == FLAC__METADATA_TYPE_STREAMINFO)
	{
		info->total_samples = metadata->data.stream_info.total_samples;
		DEBUG("total_total_samples=%ld\n", (long)metadata->data.stream_info.total_samples);

		info->bits_per_sample = metadata->data.stream_info.bits_per_sample;
		DEBUG("bits_per_sample=%d\n", metadata->data.stream_info.bits_per_sample);

		info->channels = metadata->data.stream_info.channels;
		DEBUG("channels=%d\n", metadata->data.stream_info.channels);

		info->sample_rate = metadata->data.stream_info.sample_rate;
		DEBUG("sample_rate=%d\n", metadata->data.stream_info.sample_rate);
	}
}
