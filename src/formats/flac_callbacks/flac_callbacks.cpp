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

/* Based on https://github.com/audacious-media-player/audacious-plugins/tree/master/src/flac */

#include <string.h>
#include <FLAC/all.h>

#include "flac_callbacks.hpp"
#include "error.hpp"

/* This is a mix between squeeze_audio from audacious and convertBuffersGeneric from scummvm */
static void convertBuffersGeneric(int32_t* src, int16_t* dst, unsigned count, unsigned res)
{
	int32_t* rp = src;
	int16_t* wp2 = dst;

	if (res < BUFTYPE_BITS) {
		const uint8_t kPower = (uint8_t)(BUFTYPE_BITS - res);

		for (unsigned i = 0; i < count; i++) {
			*wp2++ = static_cast<int16_t>(rp[i]) << kPower;
		}
	} else if (res > BUFTYPE_BITS) {
		const uint8_t kPower = (uint8_t)(res - BUFTYPE_BITS);

		for (unsigned i = 0; i < count; i++) {
			*wp2++ = static_cast<int16_t>(rp[i] >> kPower);
		}
	} else {
		for (unsigned i = 0; i < count; i++) {
			*wp2++ = static_cast<int16_t>(rp[i]);
		}
	}
}

int FLAC_decode(FLAC__StreamDecoder* decoder, callback_info* cinfo, int16_t *buffer, int numSamples) {
	if (FLAC__stream_decoder_get_state(decoder) == FLAC__STREAM_DECODER_END_OF_STREAM)
		return 0;

	cinfo->clear();

	/* Try to decode a single frame of audio */
	if (FLAC__stream_decoder_process_single(decoder) == false)
	{
		DEBUG("Error while decoding!\n");
		return 0;
	}

	/* Copy data to audio buffer and if necessary, convert the data */
	convertBuffersGeneric(cinfo->output_buffer, buffer, cinfo->samples_used, cinfo->bits_per_sample);
	return cinfo->samples_used;
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	callback_info *info = (callback_info*)client_data;

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
		info->samples_used += 1;
		}
	}

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data)
{
	const char *str;
	DEBUG("FLAC: decoder error callback was called: %d\n", status);
	if ((str = FLAC__StreamDecoderErrorStatusString[status]))
		DEBUG("FLAC: status of %d: %s\n", status, str);
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data)
{
	callback_info *info = (callback_info*)client_data;

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

		info->max_blocksize = metadata->data.stream_info.sample_rate;
		DEBUG("max_blocksize=%d\n", metadata->data.stream_info.max_blocksize);

		info->min_blocksize = metadata->data.stream_info.sample_rate;
		DEBUG("min_blocksize=%d\n", metadata->data.stream_info.min_blocksize);
	}
}
