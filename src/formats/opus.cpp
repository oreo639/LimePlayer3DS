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

#include <opusfile.h>

#include "opus.hpp"

static OggOpusFile*		opusFile;
static const size_t		buffSize = 32 * 1024;

uint64_t fillOpusBuffer(int16_t* bufferOut);

static int opus_callback_read(void *_stream, unsigned char *_ptr, int _nbytes) {
	return ((FileTransport*) _stream)->f_read(_ptr, 1, _nbytes);
}

static int opus_callback_seek(void *_stream, opus_int64 _offset, int _whence) {
	int ret = 0;
	if ((ret = ((FileTransport*) _stream)->f_seek(_offset, _whence)) < 0)
		return -1;

	return ret;
}

static opus_int64 opus_callback_tell(void *_stream) {
	return ((FileTransport*) _stream)->f_tell();
}

static int opus_callback_close(void *_stream) {
	return 0;
}

OpusFileCallbacks transport_callbacks;


OpusDecoder::OpusDecoder(FileTransport* transport) : Decoder("Opus") {
	int err = 0;

	transport_callbacks.read = opus_callback_read;
	transport_callbacks.close = opus_callback_close;
	transport_callbacks.tell = opus_callback_tell;
	transport_callbacks.seek = opus_callback_seek;

	if ((opusFile = op_open_callbacks(transport, &transport_callbacks, NULL, 0, &err)) == NULL) {
		DEBUG("Opusfile errored with %d\n", err);
		return;
	}

	if((err = op_current_link(opusFile)) < 0)
		return;
	
	this->IsInit = true;
}

OpusDecoder::~OpusDecoder(void) {
	op_free(opusFile);
	this->IsInit = false;
}

void ProcessInfo(const OpusTags *comment, metaInfo_t* Meta) {
	const char *ret;

	if ((ret = opus_tags_query(comment, const_cast<char*>("title"), 0)))
		Meta->Title.assign(ret);

	if ((ret = opus_tags_query(comment, const_cast<char*>("artist"), 0)))
		Meta->Artist.assign(ret);

	if (Meta->Artist.empty())
		Meta->Artist.assign("(No Author-Opus)");
}

void OpusDecoder::Info(metaInfo_t* Meta) {
	const OpusTags *comment = op_tags(opusFile, -1);

	const char *old_title = Meta->Title.c_str();
	const char *new_title = opus_tags_query(comment, const_cast<char*>("title"), 0);

	if (!new_title || (old_title && !strcmp(old_title, new_title)))
        	return;

	ProcessInfo(comment, Meta);
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
