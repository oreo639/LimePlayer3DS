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

#include "net.hpp"
#include "netfmt.hpp"
#include "error.hpp"
#include "formats/mp3stream.hpp"

Mp3StreamDecoder *streamdec;

static int LibInit = false;

NetfmtDecoder::NetfmtDecoder(const char* url) {
	if (R_FAILED(http_open(&this->httpc, url))) {
		DEBUG("Netfmt: Http_open failed.\n");
		return;
	}
	this->dbuf = (uint8_t*)malloc(0x1000);
	if(!this->dbuf){
		DEBUG("Netfmt: Malloc Failed.\n");
		return;
	}

	http_download(&this->httpc, this->dbuf, 0x1000, &this->readsize);

	streamdec = (new Mp3StreamDecoder(this->dbuf, this->readsize));

	if (streamdec->IsInit())
		LibInit = true;
}

NetfmtDecoder::~NetfmtDecoder(void) {
	http_close(&this->httpc);
	free(this->dbuf);
	LibInit = false;
}

bool NetfmtDecoder::IsInit(void) {
	return LibInit;
}

void NetfmtDecoder::Info(musinfo_t* Meta) {
	return;
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
	if (http_download(&this->httpc, this->dbuf, 0x1000, &this->readsize) != (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
		return 0;
	return streamdec->Decode(this->dbuf, this->readsize, buffer);
}

uint32_t NetfmtDecoder::Samplerate(void) {
	return streamdec->Samplerate();
}

uint32_t NetfmtDecoder::Spf(void* buffer) {
	if (http_download(&this->httpc, this->dbuf, 0x1000, &this->readsize) != (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
		return 0;
	return streamdec->Spf(this->dbuf, this->readsize, buffer);
}

uint32_t NetfmtDecoder::Buffsize(void) {
	return streamdec->Buffsize();
}

int NetfmtDecoder::Channels(void) {
	return streamdec->Channels();
}
