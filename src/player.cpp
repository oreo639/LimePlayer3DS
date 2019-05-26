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
#include <iostream>
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

#include "formats/midi.hpp"
#include "formats/flac.hpp"
#include "formats/mp3.hpp"
#include "formats/opus.hpp"
#include "formats/vorbis.hpp"
#include "formats/wav.hpp"
#include "network/netfmt.hpp"

Player audioplayer;

static volatile bool	stop = true;
static ndspWaveBuf	waveBuf[2];

void PlayerInterface::ThreadMainFunct(void *input) {
	playbackInfo_t* info = static_cast<playbackInfo_t*>(input);
	audioplayer.Play(info);
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
	stop = true;
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
	stop = false;
	int filetype = File::GetFileType(playbackInfo->filename.c_str());
	if (!filetype) {
		App::Error = FILE_NOT_SUPPORTED;
		return;
	}
	Decoder* decoder = GetFormat(playbackInfo, filetype);
	
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
		for(int i = 0; i < 2; i++) {
			waveBuf[i].nsamples = decoder->Spf(audioBuffer + (i == 0 ? 0 : decoder->Buffsize()));
			waveBuf[i].data_vaddr = audioBuffer + (i == 0 ? 0 : decoder->Buffsize());
			ndspChnWaveBufAdd(0, &waveBuf[i]);
		}
		
		/**
		 * There may be a chance that the music has not started by the time we get
		 * to the while loop. So we ensure that music has started here.
		 */
		for(int i = 0; ndspChnIsPlaying(0) == false; i++) {
			if(i > 90000) {
				DEBUG("player.cpp: Chnn wait imeout.\n");
				stop = true;
				App::Error = DECODER_INIT_TIMEOUT;
				break;
			}
		}
		//playerget_FileInfo(info);
		bool lastbuf = false;
		while(stop == false)
		{
			svcSleepThread(100 * 1000);
	
			/* When the last buffer has finished playing, break. */
			if(lastbuf == true)
				break;

			while (ndspChnIsPaused(0) == true || lastbuf == true)
				continue;

			for(int i = 0; i < 2; i++) {
				if(waveBuf[i].status == NDSP_WBUF_DONE)
				{
					size_t read = decoder->Decode(waveBuf[i].data_pcm16);

					if(read <= 0)
					{
						lastbuf = true;
						continue;
					}
					else if(read < decoder->Buffsize())
						waveBuf[i].nsamples = read / decoder->Channels();
		
					ndspChnWaveBufAdd(0, &waveBuf[i]);
				}
				DSP_FlushDataCache(waveBuf[i].data_pcm16, decoder->Buffsize() * sizeof(int16_t));
			}
		}
		linearFree(audioBuffer);
		ndspChnWaveBufClear(0);
		ClearMetadata(&playbackInfo->fileMeta);
		DEBUG("player.cpp: Playback complete.\n");
		delete decoder;
	} else {
		App::Error = DECODER_INIT_FAIL;
		DEBUG("player.cpp: Decoder could not be initalized.\n");
	}
}

void Player::ClearMetadata(musinfo_t* fileMeta) {
	fileMeta->authorCpright.clear();
}

Decoder* Player::GetFormat(const playbackInfo_t* playbackInfo, int filetype) {
	if (filetype == FILE_WAV) {
		auto wavdec = (new WavDecoder(playbackInfo->filename.c_str()));
		if (wavdec->IsInit())
			return wavdec;
	}
	else if (filetype == FILE_FLAC) {
		auto flacdec = (new FlacDecoder(playbackInfo->filename.c_str()));
		if (flacdec->IsInit())
			return flacdec;
	}
	else if (filetype == FILE_MP3) {
		auto mp3dec = (new Mp3Decoder(playbackInfo->filename.c_str()));
		if (mp3dec->IsInit())
			return mp3dec;
	}
	else if (filetype == FILE_VORBIS) {
		auto vorbisdec = (new VorbisDecoder(playbackInfo->filename.c_str()));
		if (vorbisdec->IsInit())
			return vorbisdec;
	}
	else if (filetype == FILE_OPUS) {
		auto opusdec = (new OpusDecoder(playbackInfo->filename.c_str()));
		if (opusdec->IsInit())
			return opusdec;
	}	
	else if (filetype == FILE_MIDI) {
		auto mididec = (new MidiDecoder(playbackInfo->filename.c_str(), playbackInfo->settings.wildMidiConfig.c_str()));
		if (mididec->IsInit())
			return mididec;
	}
	else if (filetype == FMT_NETWORK) {
		auto netdec = (new NetfmtDecoder(playbackInfo->filename.c_str()));
		if (netdec->IsInit())
			return netdec;
	}
	return nullptr;
}