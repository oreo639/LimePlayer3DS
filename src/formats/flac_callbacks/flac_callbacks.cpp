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

/* Based on https://github.com/audacious-media-player/audacious-plugins/tree/master/src/flac*/

#include <string.h>
#include <FLAC/all.h>

#include "flac_callbacks.hpp"
#include "error.hpp"

#define MAX_OUTPUT_CHANNELS 2

#ifndef MIN
#define MIN(x,y) ((x)<(y)?(x):(y))
#endif

void convertBuffersGeneric(int16_t* bufDestination, const FLAC__int32 *inChannels[], uint numSamples, const uint numChannels, const uint8_t numBits) {
	if(numSamples % numChannels == 0) {	
		if (numBits < BUFTYPE_BITS) {
			const uint8_t kPower = (uint8_t)(BUFTYPE_BITS - numBits);
	
			for (; numSamples > 0; numSamples -= numChannels) {
				for (uint i = 0; i < numChannels; ++i)
					*bufDestination++ = static_cast<int16_t>(*(inChannels[i]++)) << kPower;
			}
		} else if (numBits > BUFTYPE_BITS) {
			const uint8_t kPower = (uint8_t)(numBits - BUFTYPE_BITS);
	
			for (; numSamples > 0; numSamples -= numChannels) {
				for (uint i = 0; i < numChannels; ++i)
					*bufDestination++ = static_cast<int16_t>(*(inChannels[i]++) >> kPower);
			}
		} else {
			for (; numSamples > 0; numSamples -= numChannels) {
				for (uint i = 0; i < numChannels; ++i)
					*bufDestination++ = static_cast<int16_t>(*(inChannels[i]++));
			}
		}
	}
}

int FLAC_decode(FLAC__StreamDecoder* decoder, callback_info* cinfo, int16_t *buffer, int numSamples) {
	if (cinfo->channels == 0) {
		DEBUG("FLACStream: Stream not successfully initialized, cant playback");
		return 1; // streaminfo wasnt read!
	}

	if(numSamples % cinfo->channels != 0 || !buffer || cinfo->outBuffer || cinfo->requested_samples) {
		return 1;
	}

	cinfo->outBuffer = buffer;
	cinfo->requested_samples = numSamples;

	// If there is still data in our buffer from the last time around,
	// copy that first.
	if (cinfo->sampleCache.bufFill > 0) {
		const uint copySamples = MIN((uint)numSamples, cinfo->sampleCache.bufFill);
		memcpy(buffer, cinfo->sampleCache.bufReadPos, copySamples*sizeof(buffer[0]));

		cinfo->outBuffer = buffer + copySamples;
		cinfo->requested_samples = numSamples - copySamples;
		cinfo->sampleCache.bufReadPos += copySamples;
		cinfo->sampleCache.bufFill -= copySamples;
	}

	bool decoderOk = true;

	FLAC__StreamDecoderState state = FLAC__stream_decoder_get_state(decoder);

	if (state == FLAC__STREAM_DECODER_END_OF_STREAM) {
		cinfo->endOfStream = true;
	}
	else if (state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) {
		// Keep poking FLAC to process more samples until we completely satisfied the request
		// respectively until we run out of data.
		while (!cinfo->endOfStream && cinfo->requested_samples > 0) {
			FLAC__stream_decoder_process_single(decoder);
			state = FLAC__stream_decoder_get_state(decoder);

			if (state == FLAC__STREAM_DECODER_END_OF_STREAM) {
				cinfo->endOfStream = true;
			}
			if (state != FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) {
				break;
			}
		}
	}
	else {
		decoderOk = false;
		DEBUG("FLACStream: An error occurred while decoding. DecoderState is: %s",
			FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(decoder)]);
	}

	// Compute how many samples we actually produced
	const int processedSamples = (int)(cinfo->outBuffer - buffer);

	cinfo->clear();

	return decoderOk ? processedSamples : 1;
}

FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data)
{
	callback_info *info = (callback_info*)client_data;

	if (info->channels != frame->header.channels ||
		info->sample_rate != frame->header.sample_rate ||
		info->bits_per_sample != frame->header.bits_per_sample)
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	if ((frame->header.number_type != FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER && info->min_blocksize != info->max_blocksize))
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	if (info->sampleCache.bufFill != 0 && info->requested_samples != 0)
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	uint numSamples = frame->header.blocksize;
	const uint numChannels = info->channels;
	const uint8_t numBits = (uint8_t)info->bits_per_sample;

	const FLAC__uint64 firstSampleNumber = (frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER) ?
		frame->header.number.sample_number : (static_cast<FLAC__uint64>(frame->header.number.frame_number)) * info->max_blocksize;

	// Check whether we are about to reach beyond the last sample we are supposed to play.
	if (info->lastSample != 0 && firstSampleNumber + numSamples >= info->lastSample) {
		numSamples = (uint)(firstSampleNumber >= info->lastSample ? 0 : info->lastSample - firstSampleNumber);
		info->endOfStream = true;
	}

	// The value in info->requested_samples counts raw samples, so if there are more than one
	// channel, we have to multiply the number of available sample "pairs" by numChannels
	numSamples *= numChannels;

	const FLAC__int32 *inChannels[MAX_OUTPUT_CHANNELS];
	for (uint i = 0; i < numChannels; ++i)
		inChannels[i] = buffer[i];

	// write the incoming samples directly into the buffer provided to us by the mixer
	if (info->requested_samples > 0) {
		// Copy & convert the available samples (limited both by how many we have available, and
		// by how many are actually needed).
		const uint copySamples = MIN(info->requested_samples, numSamples);
		convertBuffersGeneric(info->outBuffer, inChannels, copySamples, numChannels, numBits);

		info->requested_samples -= copySamples;
		numSamples -= copySamples;
		info->outBuffer += copySamples;
	}

	// Write all remaining samples (i.e. those which didn't fit into the mixer buffer)
	// into the sample cache.
	if (info->sampleCache.bufFill == 0)
		info->sampleCache.bufReadPos = info->sampleCache.bufData;
	const uint cacheSpace = (info->sampleCache.bufData + BUFFER_SIZE) - (info->sampleCache.bufReadPos + info->sampleCache.bufFill);
	if (numSamples > cacheSpace)
		return FLAC__STREAM_DECODER_WRITE_STATUS_ABORT;

	convertBuffersGeneric(info->sampleCache.bufReadPos + info->sampleCache.bufFill, inChannels, numSamples, numChannels, numBits);

	info->sampleCache.bufFill += numSamples;

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
