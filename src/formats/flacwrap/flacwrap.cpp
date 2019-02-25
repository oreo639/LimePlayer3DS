/*   LimePlayer3DS OSS graphcal music player for the 3ds.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 2 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    YoYou should have received a copy of the GNU General Public License along
*    with this program; if not, write to the Free Software Foundation, Inc.,
*    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

/*This is hevaly modified from scummvm's flac.c*/
#include <FLAC/stream_decoder.h>

#include <cstring>
#include <cassert>
#include <cstdlib>

#include "flacwrap.hpp"
#include "error.hpp"

#define MAX_OUTPUT_CHANNELS 2

#define MIN(x,y) ((x)<(y)?(x):(y))


static FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data);//Write callback
static void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);//Metadata callback
static void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);//Error callback

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

int FLACWRAP_init(const char* filename, flacwrap_t* flacstruct) {
	int status = FLAC__stream_decoder_init_file(flacstruct->dFlac, filename, write_callback, metadata_callback, error_callback, flacstruct);
	if(status != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
		return 1;
	}
	FLAC__stream_decoder_process_until_end_of_metadata(flacstruct->dFlac);
	return 0;
}

void FLACWRAP_free(flacwrap_t* flacstruct) {
	FLAC__stream_decoder_finish(flacstruct->dFlac);
	if(flacstruct->outBuffer) {
		flacstruct->outBuffer = NULL;
	}
	flacstruct->requestedSamples = 0;
	flacstruct->lastSampleWritten = false;
	flacstruct->sampleCache.bufFill = 0;
}


int FLACWRAP_decode(flacwrap_t* flacstruct, int16_t *buffer, int numSamples) {
	const uint numChannels = flacstruct->mFlac.channels;

	if (numChannels == 0) {
		DEBUG("FLACStream: Stream not successfully initialized, cant playback");
		return -1; // streaminfo wasnt read!
	}

	if(numSamples % numChannels != 0 || buffer == NULL || flacstruct->outBuffer != NULL || flacstruct->requestedSamples != 0) {
		return -1;
	}

	flacstruct->outBuffer = buffer;
	flacstruct->requestedSamples = numSamples;

	// If there is still data in our buffer from the last time around,
	// copy that first.
	if (flacstruct->sampleCache.bufFill > 0) {
		assert(flacstruct->sampleCache.bufReadPos >= flacstruct->sampleCache.bufData);
		assert(flacstruct->sampleCache.bufFill % numChannels == 0);

		const uint copySamples = MIN((uint)numSamples, flacstruct->sampleCache.bufFill);
		memcpy(buffer, flacstruct->sampleCache.bufReadPos, copySamples*sizeof(buffer[0]));

		flacstruct->outBuffer = buffer + copySamples;
		flacstruct->requestedSamples = numSamples - copySamples;
		flacstruct->sampleCache.bufReadPos += copySamples;
		flacstruct->sampleCache.bufFill -= copySamples;
	}

	bool decoderOk = true;

	FLAC__StreamDecoderState state = FLAC__stream_decoder_get_state(flacstruct->dFlac);

	if (state == FLAC__STREAM_DECODER_END_OF_STREAM) {
		flacstruct->lastSampleWritten = true;
	}
	else if (state == FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) {
		// Keep poking FLAC to process more samples until we completely satisfied the request
		// respectively until we run out of data.
		while (!flacstruct->lastSampleWritten && flacstruct->requestedSamples > 0) {
			assert(flacstruct->sampleCache.bufFill == 0);
			assert(flacstruct->requestedSamples % numChannels == 0);
			FLAC__stream_decoder_process_single(flacstruct->dFlac);
			state = FLAC__stream_decoder_get_state(flacstruct->dFlac);

			if (state == FLAC__STREAM_DECODER_END_OF_STREAM) {
				flacstruct->lastSampleWritten = true;
			}
			if (state != FLAC__STREAM_DECODER_SEARCH_FOR_FRAME_SYNC) {
				break;
			}
		}
	}
	else {
		decoderOk = false;
		DEBUG("FLACStream: An error occurred while decoding. DecoderState is: %s",
			FLAC__StreamDecoderStateString[FLAC__stream_decoder_get_state(flacstruct->dFlac)]);
	}

	// Compute how many samples we actually produced
	const int processedSamples = (int)(flacstruct->outBuffer - buffer);

	flacstruct->outBuffer = NULL; 
	flacstruct->requestedSamples = 0;

	return decoderOk ? processedSamples : -1;
}

// Callbacks
FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *clientData) {
	flacwrap_t* flacstruct = (flacwrap_t*)clientData;
	assert(frame->header.channels == flacstruct->mFlac.channels);
	assert(frame->header.sample_rate == flacstruct->mFlac.sample_rate);
	assert(frame->header.bits_per_sample == flacstruct->mFlac.bits_per_sample);
	assert(frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER || flacstruct->mFlac.min_blocksize == flacstruct->mFlac.max_blocksize);

	// We require that either the sample cache is empty, or that no samples were requested
	assert(flacstruct->sampleCache.bufFill == 0 || flacstruct->requestedSamples == 0);

	uint numSamples = frame->header.blocksize;
	const uint numChannels = flacstruct->mFlac.channels;
	const uint8_t numBits = (uint8_t)flacstruct->mFlac.bits_per_sample;

	assert(flacstruct->requestedSamples % numChannels == 0); // must be integral multiply of channels

	const FLAC__uint64 firstSampleNumber = (frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER) ?
		frame->header.number.sample_number : (static_cast<FLAC__uint64>(frame->header.number.frame_number)) * flacstruct->mFlac.max_blocksize;

	// Check whether we are about to reach beyond the last sample we are supposed to play.
	if (flacstruct->lastSample != 0 && firstSampleNumber + numSamples >= flacstruct->lastSample) {
		numSamples = (uint)(firstSampleNumber >= flacstruct->lastSample ? 0 : flacstruct->lastSample - firstSampleNumber);
		flacstruct->lastSampleWritten = true;
	}

	// The value in flacstruct->requestedSamples counts raw samples, so if there are more than one
	// channel, we have to multiply the number of available sample "pairs" by numChannels
	numSamples *= numChannels;

	const FLAC__int32 *inChannels[MAX_OUTPUT_CHANNELS];
	for (uint i = 0; i < numChannels; ++i)
		inChannels[i] = buffer[i];

	// write the incoming samples directly into the buffer provided to us by the mixer
	if (flacstruct->requestedSamples > 0) {
		assert(flacstruct->requestedSamples % numChannels == 0);
		assert(flacstruct->outBuffer != NULL);

		// Copy & convert the available samples (limited both by how many we have available, and
		// by how many are actually needed).
		const uint copySamples = MIN(flacstruct->requestedSamples, numSamples);
		convertBuffersGeneric(flacstruct->outBuffer, inChannels, copySamples, numChannels, numBits);

		flacstruct->requestedSamples -= copySamples;
		numSamples -= copySamples;
		flacstruct->outBuffer += copySamples;
	}

	// Write all remaining samples (i.e. those which didn't fit into the mixer buffer)
	// into the sample cache.
	if (flacstruct->sampleCache.bufFill == 0)
		flacstruct->sampleCache.bufReadPos = flacstruct->sampleCache.bufData;
	const uint cacheSpace = (flacstruct->sampleCache.bufData + BUFFER_SIZE) - (flacstruct->sampleCache.bufReadPos + flacstruct->sampleCache.bufFill);
	assert(numSamples <= cacheSpace);
	convertBuffersGeneric(flacstruct->sampleCache.bufReadPos + flacstruct->sampleCache.bufFill, inChannels, numSamples, numChannels, numBits);

	flacstruct->sampleCache.bufFill += numSamples;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *clientData) {
	assert(metadata->type == FLAC__METADATA_TYPE_STREAMINFO); // others arent really interesting
	flacwrap_t* flacstruct = (flacwrap_t*)clientData;
	flacstruct->mFlac = metadata->data.stream_info;
	
}

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *clientData) {
	DEBUG("An error occured in flac streaming. DecoderState is: %s", FLAC__StreamDecoderErrorStatusString[status]); //Output to gdb.
}
