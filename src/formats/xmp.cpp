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
#include <memory>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "xmp.hpp"

XmpDecoder::XmpDecoder(const char* filename) : Decoder("Xmp") {
	ctx = xmp_create_context();

	if (xmp_load_module(ctx, filename) < 0)
		return;

	if (xmp_start_player(ctx, XMPSAMPLERATE, 0) < 0)
		return;

	xmp_get_module_info(ctx, &mi);
	decoderName = mi.mod->type;
	mIsInit = true;
}

XmpDecoder::~XmpDecoder(void) {
	xmp_end_player(ctx);
	xmp_release_module(ctx);
	xmp_free_context(ctx);
	mIsInit = false;
}

void XmpDecoder::GetInfo(metaInfo_t* Meta) {
	Meta->Title.assign(mi.mod->name);
	Meta->Artist.assign("(No Author-Mod)");
}

uint32_t XmpDecoder::Position(void) {
	struct xmp_frame_info fi;
	xmp_get_frame_info(ctx, &fi);
	return fi.pos;
}

uint32_t XmpDecoder::Length(void) {
	return mi.mod->len;
}

void XmpDecoder::Seek(uint32_t location)
{
	if(location > (uint32_t)mi.mod->len) {
		return;
	}
	xmp_set_position(ctx, location);
}

/**
 * Decode part of open module file.
 *
 * \param buffer	Decoded output.
 * \return		Samples read for each channel.
 */
uint32_t XmpDecoder::Decode(void* buffer)
{
	xmp_play_buffer(ctx, buffer, buffSize, 0);
	return buffSize/sizeof(int16_t);
}

/**
 * Get sampling rate of module file.
 *
 * \return	Sampling rate.
 */
uint32_t XmpDecoder::Samplerate(void)
{
	return XMPSAMPLERATE;
}

uint32_t XmpDecoder::Buffsize(void)
{
	return buffSize;
}

int XmpDecoder::Channels(void)
{
	return 2;
}
