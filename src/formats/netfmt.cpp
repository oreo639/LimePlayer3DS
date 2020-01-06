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
#include "mp3.hpp"
#include "opus.hpp"
#include "nettransport.hpp"

std::unique_ptr<Decoder> Netfmt::GetFormat(const std::string& url, FileTransport* ftrans) {
	if (ftrans->content_type == CONTENT_MPEG3) {
		DEBUG("Attempting to load the Mp3 decoder.\n");
		auto mp3dec = std::make_unique<Mp3Decoder>(ftrans);
		if (mp3dec->GetIsInit())
			return mp3dec;
	}
	else if (ftrans->content_type == CONTENT_OGG) {
		DEBUG("Attempting to load the Opus decoder.\n");
		auto opsudec = std::make_unique<OpusDecoder>(ftrans);
		if (opsudec->GetIsInit())
			return opsudec;
	}
	else
		DEBUG("Unsupported content type %d\n", ftrans->content_type);

	DEBUG("Failed to load network decoders.\n");
	return nullptr;
}
