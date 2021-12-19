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
#include "formats/netfmt.hpp"

#include "transport.hpp"
#include "nettransport.hpp"
//#include "curltransport.hpp"
#include "stdiotransport.hpp"

#define MUSIC_CHANNEL 0

static bool		skip = false;
static bool		stop = true;
static ndspWaveBuf	waveBuf[2];
std::unique_ptr<Decoder> decoder = nullptr;
LightEvent soundEvent;

namespace Player
{
		void Play(playbackInfo_t* playbackInfo);

		void ClearMetadata(metaInfo_t* fileMeta);

		std::unique_ptr<Decoder> GetFormat(const playbackInfo_t* playbackInfo, int filetype, FileTransport* transport);

		std::unique_ptr<FileTransport> GetTransport(int filetype);
};

void PlayerInterface::ThreadMainFunct(void *input) {
	playbackInfo_t* info = static_cast<playbackInfo_t*>(input);
	stop = false;

	if (info->usePlaylist == 1) {
		for (uint32_t i = 0; i < info->playlistfile.filepath.size() && !stop; i++) {
			info->filepath = info->playlistfile.filepath[i];
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
	bool paused = ndspChnIsPaused(MUSIC_CHANNEL);
	DEBUG("Playback %s.\n", !paused ? "paused" : "unpaused");
	ndspChnSetPaused(MUSIC_CHANNEL, !paused);
	return !paused;
}

/**
 * Stops current playback. Playback thread should exit as a result.
 */
void PlayerInterface::ExitPlayback(void) {
	DEBUG("Exit Playback called!\n");
	stop = true;
	LightEvent_Signal(&soundEvent);
}

/**
 * Skips to the next song if playlist. Otherwise, the music will exit.
 */
void PlayerInterface::SkipPlayback(void) {
	DEBUG("Exit Playback called!\n");
	skip = true;
	LightEvent_Signal(&soundEvent);
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
	return stop ? true : ndspChnIsPaused(0);
}

/*
 * Returns the total number of samples in a file.
 * \return	Total number of samples.
 */
uint32_t PlayerInterface::GetTotalLength(void) {
	if (!stop && decoder)
		return decoder->Length();

	return 0;
}

/*
 * Returns the estimated total number of seconds in the file.
 * \return	Estimated total seconds.
 */
uint32_t PlayerInterface::GetTotalTime(void) {
	if (!stop && decoder)
		return PlayerInterface::GetTotalLength()/decoder->Samplerate();

	return 0;
}

/*
 * Returns the current sample.
 * \return	Current sample.
 */
uint32_t PlayerInterface::GetCurrentPos(void) {
	if (!stop && decoder)
		return decoder->Position();

	return 0;
}

/*
 * Returns the estimated number of seconds elapsed.
 * \return	Current location in file in seconds.
 */
uint32_t PlayerInterface::GetCurrentTime(void) {
	if (!stop && decoder)
		return PlayerInterface::GetCurrentPos()/decoder->Samplerate();

	return 0;
}

/*
 * Seeks to a specific sample in the file,
 * although it is not meant to be called directly
 * it is avaliable to be used by other functions to provide seeking
 * using percent, seconds, etc.
 */
void PlayerInterface::SeekSection(uint32_t location) {
	if (!stop && decoder) {
		bool oldstate = ndspChnIsPaused(MUSIC_CHANNEL);
		ndspChnSetPaused(MUSIC_CHANNEL, true); //Pause playback...
		decoder->Seek(location);
		ndspChnSetPaused(MUSIC_CHANNEL, oldstate); //once the seeking is done, playback can continue.
	}
}

/*
 * Seeks to a percent in the file.
 */
void PlayerInterface::SeekSectionPercent(uint32_t percent) {
	PlayerInterface::SeekSection((PlayerInterface::GetTotalLength() * (percent / 100.0f)));
}

/*
 * Estimates the elapsed samples for a given time in seconds.
 */
void PlayerInterface::SeekSectionTime(int time) {
	if (time < 0)
		time = 0;
	PlayerInterface::SeekSection(time * decoder->Samplerate());
}

/**
 * Returns Decoder name.
 * \return	String of name.
 */
std::string PlayerInterface::GetDecoderName(void) {
	if (!stop && decoder)
		return decoder->GetDecoderName();
	else
		return "";
}

static inline void audioCallback(void *const nul_) {
	(void)nul_;  // Unused

	if(stop || skip) { // Quit flag
		return;
	}
    
	LightEvent_Signal(&soundEvent);
}

void Player::Play(playbackInfo_t* playbackInfo) {
	std::unique_ptr<FileTransport> transport = nullptr;
	skip = false;
	LightEvent_Init(&soundEvent, RESET_ONESHOT);

	int filetype = File::GetFileType(playbackInfo->filepath);

	if (filetype < 0) {
		Error::Add(FILE_NOT_SUPPORTED);
		return;
	} else if (filetype != FMT_NETWORK)
		playbackInfo->filename = playbackInfo->filepath.substr(playbackInfo->filepath.find_last_of('/') + 1);
	else if (filetype == FMT_NETWORK)
		playbackInfo->filename = playbackInfo->filepath;

	transport = GetTransport(filetype);

	if (transport != nullptr)
		if (!transport->f_open(playbackInfo->filepath.c_str(), "rb"))
			decoder = GetFormat(playbackInfo, filetype, transport.get());
		else {
			if (transport->GetError().empty())
				Error::Add(DECODER_INIT_FAIL, "Failed to open transport.");
			else
				Error::Add(DECODER_INIT_FAIL, transport->GetError());
			DEBUG("Failed to open transport.\n");
		}
	else {
		Error::Add(DECODER_INIT_FAIL, "Failed initalize transport.");
		DEBUG("Failed to initalize transport.\n");
	}

	if (decoder != nullptr) {
		bool lastbuf = false;
		decoder->GetInfo(&playbackInfo->fileMeta);

		int16_t* audioBuffer = (int16_t*)linearAlloc((decoder->Buffsize() * sizeof(int16_t)) * 2);

		ndspChnReset(MUSIC_CHANNEL);
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspChnSetInterp(MUSIC_CHANNEL, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(MUSIC_CHANNEL, decoder->Samplerate());
		ndspChnSetFormat(MUSIC_CHANNEL, decoder->Channels() == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);
		ndspSetCallback(audioCallback, NULL);

		memset(waveBuf, 0, sizeof(waveBuf));
		waveBuf[0].data_vaddr = audioBuffer;
		waveBuf[1].data_vaddr = audioBuffer + decoder->Buffsize();

		while(!stop && !skip)
		{
			for (auto& buf : waveBuf) {
				if(buf.status == NDSP_WBUF_DONE || buf.status == NDSP_WBUF_FREE)
				{
					size_t read = decoder->Decode(buf.data_pcm16);

					if(read <= 0)
					{
						lastbuf = true;
						continue;
					}
					buf.nsamples = read / decoder->Channels();
		
					ndspChnWaveBufAdd(MUSIC_CHANNEL, &buf);
				}
				DSP_FlushDataCache(buf.data_pcm16, decoder->Buffsize() * sizeof(int16_t));
			}

			/* When the last buffer has finished playing, break. */
			if (lastbuf == true)
				break;

			if (ndspChnIsPaused(MUSIC_CHANNEL) == true || lastbuf == true)
				continue;

			if (decoder->AllowUpdateInfo())
				decoder->UpdateInfo(&playbackInfo->fileMeta);

			LightEvent_Wait(&soundEvent);
		}

		linearFree(audioBuffer);
		ndspChnReset(MUSIC_CHANNEL);
		ClearMetadata(&playbackInfo->fileMeta);
		DEBUG("Playback complete.\n");
		decoder = nullptr;
		transport = nullptr;
	} else {
		DEBUG("Decoder could not be initalized.\n");
	}
}

void Player::ClearMetadata(metaInfo_t* fileMeta) {
	fileMeta->isParsed = false;
	fileMeta->Title.clear();
	fileMeta->Album.clear();
	fileMeta->Artist.clear();
	fileMeta->Year.clear();
	fileMeta->Comment.clear();
	fileMeta->Genre.clear();
}

std::unique_ptr<Decoder> Player::GetFormat(const playbackInfo_t* playbackInfo, int filetype, FileTransport* transport) {
	std::string errInfo;

	if (filetype == FILE_WAV) {
		DEBUG("Attempting to load the Wav decoder.\n");
		auto wavdec = std::make_unique<WavDecoder>(playbackInfo->filepath.c_str());
		if (wavdec->GetIsInit())
			return wavdec;
	}
	else if (filetype == FILE_FLAC) {
		DEBUG("Attempting to load the Flac decoder.\n");
		auto flacdec = std::make_unique<FlacDecoder>(playbackInfo->filepath.c_str());
		if (flacdec->GetIsInit())
			return flacdec;
	}
	else if (filetype == FILE_MP3) {
		DEBUG("Attempting to load the Mp3 decoder.\n");
		auto mp3dec = std::make_unique<Mp3Decoder>(transport);
		if (mp3dec->GetIsInit())
			return mp3dec;
	}
	else if (filetype == FILE_VORBIS) {
		DEBUG("Attempting to load the Vorbis decoder.\n");
		auto vorbisdec = std::make_unique<VorbisDecoder>(playbackInfo->filepath.c_str());
		if (vorbisdec->GetIsInit())
			return vorbisdec;
	}
	else if (filetype == FILE_OPUS) {
		DEBUG("Attempting to load the Opus decoder.\n");
		auto opusdec = std::make_unique<OpusDecoder>(transport);
		if (opusdec->GetIsInit())
			return opusdec;
	}	
	else if (filetype == FILE_MIDI) {
		DEBUG("Attempting to load the Midi decoder.\n");
		auto mididec = std::make_unique<MidiDecoder>(playbackInfo->filepath.c_str(), playbackInfo->settings.wildMidiConfig.c_str());
		if (mididec->GetIsInit())
			return mididec;
	}
	else if (filetype == FMT_NETWORK) {
		DEBUG("Attempting to load the Network decoder.\n");
		auto netdec = Netfmt::GetFormat(playbackInfo->filepath.c_str(), transport);

		if (netdec) {
			if (netdec->GetIsInit())
				return netdec;

			errInfo = netdec->GetErrInfo();
		}
	}
	else
		DEBUG("No decoder found.\n");

	Error::Add(DECODER_INIT_FAIL, errInfo);
	return nullptr;
}

std::unique_ptr<FileTransport> Player::GetTransport(int filetype) {
	if (filetype < FMT_NETWORK) {
		DEBUG("Using stdio transport.\n");
		auto stdiotrans = std::make_unique<StdioTransport>();
		return stdiotrans;
	}
	else if (filetype == FMT_NETWORK) {
		DEBUG("Using network transport.\n");
		auto nettrans = std::make_unique<NetTransport>();
		//auto nettrans = std::make_unique<CurlTransport>();
		return nettrans;
	}
	else
		DEBUG("No transport avaliable.\n");

	return nullptr;
}
