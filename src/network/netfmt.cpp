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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "netfmt.hpp"
#include "content.hpp"
#include "error.hpp"
#include "formats/stream/mp3stream.hpp"
#include "formats/stream/opusstream.hpp"
#include "formats/stream/vorbisstream.hpp"

StreamDecoder *streamdec = nullptr;
musinfo_t* MetaPtr = NULL;

static uint32_t bytesread = 0;

NetfmtDecoder::NetfmtDecoder(const char* url) {
	if (R_FAILED(http_open(&this->httpctx, url, false))) {
		DEBUG("Netfmt: Http_open failed.\n");
		return;
	}

	http_download(&this->httpctx);

	if (this->httpctx.content_type == CONTENT_MPEG3) {
		streamdec = (new Mp3StreamDecoder(this->httpctx.dbuf, this->httpctx.readsize));
		DEBUG("Using mpg123.\n");
	} else if (this->httpctx.content_type == CONTENT_OGG) {
		if (isOpusStream(this->httpctx.dbuf, this->httpctx.readsize) == 0) {
			//streamdec = (new OpusStreamDecoder(this->httpctx.dbuf, this->httpctx.readsize));
			DEBUG("Opus support is not implemented.\n");

		} else if (isVorbisStream(this->httpctx.dbuf, this->httpctx.readsize) == 0) {
			DEBUG("Vorbis support is not implemented.\n");
			streamdec = nullptr;

		} else {
			DEBUG("Unsupported format using ogg container.\n");
			streamdec = nullptr;
		}
	} else {
		streamdec = nullptr;
		DEBUG("Unsupported format.");
	}

	if (streamdec != nullptr)
		if (streamdec->GetIsInit())
			this->IsInit = true;
}

NetfmtDecoder::~NetfmtDecoder(void) {
	MetaPtr = NULL;
	http_close(&this->httpctx);
	delete streamdec;
	this->IsInit = false;
}

void NetfmtDecoder::Info(musinfo_t* Meta) {
	MetaPtr = Meta;
}

uint32_t NetfmtDecoder::Position(void) {
	return streamdec->Position();
}

uint32_t NetfmtDecoder::Length(void) {
	return streamdec->Length();
}

void NetfmtDecoder::Seek(uint32_t location) {
	return;
}

uint32_t NetfmtDecoder::Decode(void* buffer) {
	if (http_download(&this->httpctx) != (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
		return 0;

	if (this->httpctx.isShoutcastSupported) {
		// Do byte interval calculations here.
	}

	return streamdec->Decode(this->httpctx.dbuf, this->httpctx.readsize, buffer);
}

uint32_t NetfmtDecoder::Samplerate(void) {
	return streamdec->Samplerate();
}

uint32_t NetfmtDecoder::Buffsize(void) {
	return streamdec->Buffsize();
}

int NetfmtDecoder::Channels(void) {
	return streamdec->Channels();
}
