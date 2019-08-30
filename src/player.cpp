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
#include <string>
#include <memory>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <3ds.h>

#include "app.hpp"
#include "player.hpp"
#include "file.hpp"
#include "error.hpp"
#include "debug.hpp"

#include "formats/midi.hpp"
#include "formats/flac.hpp"
#include "formats/mp3.hpp"
#include "formats/opus.hpp"
#include "formats/vorbis.hpp"
#include "formats/wav.hpp"
#include "network/netfmt.hpp"

static volatile bool	skip = false;
static volatile bool	stop = true;
static ndspWaveBuf	waveBuf[2];

namespace Player
{
		void Play(playbackInfo_t* playbackInfo);

		void ClearMetadata(musinfo_t* fileMeta);

		std::unique_ptr<Decoder> GetFormat(const playbackInfo_t* playbackInfo, int filetype);
};

void PlayerInterface::ThreadMainFunct(void *input) {
	playbackInfo_t* info = static_cast<playbackInfo_t*>(input);
	stop = false;

	if (info->usePlaylist == 1) {
		for (uint32_t i = 0; i < info->playlistfile.filepath.size() && !stop; i++) {
			info->filename = info->playlistfile.filepath[i];
			Player::Play(info);
		}
	} else {
		Player::Play(info);
	}
	info->usePlaylist = 0;
	stop = true;
	threadExit(0);
	return;
}

/**
 * Pause or play current file.
 *
 * \return	True if paused.
 */
bool PlayerInterface::TogglePlayback(void) {
	bool paused = ndspChnIsPaused(0);
	ndspChnSetPaused(0, !paused);
	return !paused;
}

/**
 * Stops current playback. Playback thread should exit as a result.
 */
void PlayerInterface::ExitPlayback(void) {
	DEBUG("Exit Playback called!\n");
	stop = true;
}

/**
 * Skips to the next song if playlist. Otherwise, the music will exit.
 */
void PlayerInterface::SkipPlayback(void) {
	skip = true;
}

/**
 * Returns whether music is playing or stoped.
 * \return	True if not stoped.
 */
bool PlayerInterface::IsPlaying(void) {
	return !stop;
}

/**
 * Returns whether music is playing or paused.
 * \return	True if paused.
 */
bool PlayerInterface::IsPaused(void) {
	return ndspChnIsPaused(0);
}

void Player::Play(playbackInfo_t* playbackInfo) {
	skip = false;

	int filetype = File::GetFileType(playbackInfo->filename);
	if (filetype < 0) {
		Error::Add(FILE_NOT_SUPPORTED);
		return;
	}
	std::unique_ptr<Decoder> decoder = GetFormat(playbackInfo, filetype);
	
	if (decoder != nullptr) {
		decoder->Info(&playbackInfo->fileMeta);

		int16_t* audioBuffer = (int16_t*)linearAlloc((decoder->Buffsize() * sizeof(int16_t)) * 2);

		ndspChnReset(0);
		ndspChnWaveBufClear(0);
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspChnSetInterp(0, decoder->Channels() == 2 ? NDSP_INTERP_POLYPHASE : NDSP_INTERP_LINEAR);
		ndspChnSetRate(0, decoder->Samplerate());
		ndspChnSetFormat(0, decoder->Channels() == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

		memset(waveBuf, 0, sizeof(waveBuf));
		waveBuf[0].data_vaddr = audioBuffer;
		waveBuf[1].data_vaddr = audioBuffer + decoder->Buffsize();
		for (auto& buf : waveBuf) {
			buf.nsamples = decoder->Decode(buf.data_pcm16) / decoder->Channels();
			ndspChnWaveBufAdd(0, &buf);
		}
		
		/**
		 * There may be a chance that the music has not started by the time we get
		 * to the while loop. So we ensure that music has started here.
		 */
		for(int i = 0; ndspChnIsPlaying(0) == false; i++) {
			svcSleepThread(1000000); // Wait one millisecond.
			if(i > 5 * 1000) { // Wait 5 seconds
				DEBUG("Chnn wait imeout.\n");
				stop = true;
				Error::Add(DECODER_INIT_TIMEOUT);
				break;
			}
		}


		bool lastbuf = false;
		while(!stop && !skip)
		{
			svcSleepThread(100 * 1000);
	
			/* When the last buffer has finished playing, break. */
			if (lastbuf == true)
				break;

			if (ndspChnIsPaused(0) == true || lastbuf == true)
				continue;

			for (auto& buf : waveBuf) {
				if(buf.status == NDSP_WBUF_DONE)
				{
					size_t read = decoder->Decode(buf.data_pcm16);

					if(read <= 0)
					{
						lastbuf = true;
						continue;
					}
					else if(read < decoder->Buffsize())
						buf.nsamples = read / decoder->Channels();
		
					ndspChnWaveBufAdd(0, &buf);
				}
				DSP_FlushDataCache(buf.data_pcm16, decoder->Buffsize() * sizeof(int16_t));
			}
		}
		linearFree(audioBuffer);
		ndspChnWaveBufClear(0);
		ClearMetadata(&playbackInfo->fileMeta);
		DEBUG("Playback complete.\n");
		decoder = nullptr;
	} else {
		DEBUG("Decoder could not be initalized.\n");
	}
}

void Player::ClearMetadata(musinfo_t* fileMeta) {
	fileMeta->authorCpright.clear();
}

std::unique_ptr<Decoder> Player::GetFormat(const playbackInfo_t* playbackInfo, int filetype) {
	std::string errInfo;

	if (filetype == FILE_WAV) {
		DEBUG("Attempting to load the Wav decoder.\n");
		auto wavdec = std::unique_ptr<Decoder>(new WavDecoder(playbackInfo->filename.c_str()));
		if (wavdec->GetIsInit())
			return wavdec;
	}
	else if (filetype == FILE_FLAC) {
		DEBUG("Attempting to load the Flac decoder.\n");
		auto flacdec = std::unique_ptr<Decoder>(new FlacDecoder(playbackInfo->filename.c_str()));
		if (flacdec->GetIsInit())
			return flacdec;
	}
	else if (filetype == FILE_MP3 || filetype == FILE_MP3_NOID3) {
		DEBUG("Attempting to load the Mp3 decoder.\n");
		if (filetype == FILE_MP3_NOID3) {
			auto mp3dec = std::unique_ptr<Decoder>(new Mp3Decoder(playbackInfo->filename.c_str(), false));
			if (mp3dec->GetIsInit())
				return mp3dec;
		}
		else {
			auto mp3dec = std::unique_ptr<Decoder>(new Mp3Decoder(playbackInfo->filename.c_str(), true));
			if (mp3dec->GetIsInit())
				return mp3dec;
		}
	}
	else if (filetype == FILE_VORBIS) {
		DEBUG("Attempting to load the Vorbis decoder.\n");
		auto vorbisdec = std::unique_ptr<Decoder>(new VorbisDecoder(playbackInfo->filename.c_str()));
		if (vorbisdec->GetIsInit())
			return vorbisdec;
	}
	else if (filetype == FILE_OPUS) {
		DEBUG("Attempting to load the Opus decoder.\n");
		auto opusdec = std::unique_ptr<Decoder>(new OpusDecoder(playbackInfo->filename.c_str()));
		if (opusdec->GetIsInit())
			return opusdec;
	}	
	else if (filetype == FILE_MIDI) {
		DEBUG("Attempting to load the Midi decoder.\n");
		auto mididec = std::unique_ptr<Decoder>(new MidiDecoder(playbackInfo->filename.c_str(), playbackInfo->settings.wildMidiConfig.c_str()));
		if (mididec->GetIsInit())
			return mididec;
	}
	else if (filetype == FMT_NETWORK) {
		DEBUG("Attempting to load the Network decoder.\n");
		auto netdec = std::unique_ptr<Decoder>(new NetfmtDecoder(playbackInfo->filename.c_str()));
		if (netdec->GetIsInit())
			return netdec;

		errInfo = netdec->GetErrInfo();
	}

	else
		DEBUG("No decoder found.\n");

	Error::Add(DECODER_INIT_FAIL, errInfo);
	return nullptr;
}
