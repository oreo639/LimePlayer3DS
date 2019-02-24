#include <iostream>
#include <string>
#include <memory>
#include <vector>

#include <stdio.h>
#include <string.h>

#include "app.hpp"
#include "player.hpp"
#include "file.hpp"

#include "formats/midi.hpp"
#include "formats/flac.hpp"
#include "formats/mp3.hpp"
#include "formats/opus.hpp"
#include "formats/vorbis.hpp"
#include "formats/wav.hpp"

Player audioplayer;

static volatile bool	stop = true;
static int16_t*	buffer1 = NULL;
static int16_t*	buffer2 = NULL;
static ndspWaveBuf	waveBuf[2];

void PlayerInterface::ThreadMainFunct(void *input) {
	char* instr = static_cast<char*>(input);
	std::string filename;
	filename.assign(instr, strlen(instr));
	audioplayer.Play(filename);
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

void Player::Play(const std::string& filename) {
	stop = false;
	int filetype = File::GetFileType(filename.c_str());
	if (!filetype) {
		App::Error = 10;
		return;
	}
	std::unique_ptr<Decoder> decoder = GetFormat(filename, filetype);
	
	if (decoder != nullptr) {
		buffer1 = (int16_t*)linearAlloc(decoder->Buffsize() * sizeof(int16_t));
		buffer2 = (int16_t*)linearAlloc(decoder->Buffsize() * sizeof(int16_t));

		ndspChnReset(0);
		ndspChnWaveBufClear(0);
		ndspSetOutputMode(NDSP_OUTPUT_STEREO);
		ndspChnSetInterp(0, NDSP_INTERP_POLYPHASE);
		ndspChnSetRate(0, decoder->Samplerate());
		ndspChnSetFormat(0, decoder->Channels() == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

		memset(waveBuf, 0, sizeof(waveBuf));
		waveBuf[0].nsamples = decoder->Spf(&buffer1[0]);
		waveBuf[0].data_vaddr = &buffer1[0];
		ndspChnWaveBufAdd(0, &waveBuf[0]);

		waveBuf[1].nsamples = decoder->Spf(&buffer2[0]);
		waveBuf[1].data_vaddr = &buffer2[0];
		ndspChnWaveBufAdd(0, &waveBuf[1]);
		
		/**
		 * There may be a chance that the music has not started by the time we get
		 * to the while loop. So we ensure that music has started here.
		 */

		while(ndspChnIsPlaying(0) == false);
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

			if(waveBuf[0].status == NDSP_WBUF_DONE)
			{
				size_t read = decoder->Decode(&buffer1[0]);

				if(read <= 0)
				{
					lastbuf = true;
					continue;
				}
				else if(read < decoder->Buffsize())
					waveBuf[0].nsamples = read / decoder->Channels();
	
				ndspChnWaveBufAdd(0, &waveBuf[0]);
			}

			if(waveBuf[1].status == NDSP_WBUF_DONE)
			{
				size_t read = decoder->Decode(&buffer2[0]);

				if(read <= 0)
				{
					lastbuf = true;
					continue;
				}
				else if(read < decoder->Buffsize())
					waveBuf[1].nsamples = read / decoder->Channels();
	
				ndspChnWaveBufAdd(0, &waveBuf[1]);
			}
		
		
			DSP_FlushDataCache(buffer1, decoder->Buffsize() * sizeof(int16_t));
			DSP_FlushDataCache(buffer2, decoder->Buffsize() * sizeof(int16_t));
		}
		linearFree(buffer1);
		linearFree(buffer2);
		ndspChnWaveBufClear(0);
	} else {
		App::Error = 31;
	}
	App::Error = 30;
}

std::unique_ptr<Decoder> Player::GetFormat(const std::string& filename, int filetype) {
	if (filetype == FILE_WAV) {
		auto wavdec = std::unique_ptr<Decoder>(new WavDecoder(filename.c_str()));
		if (wavdec->IsInit())
			return wavdec;
	}
	else if (filetype == FILE_FLAC) {
		auto flacdec = std::unique_ptr<Decoder>(new FlacDecoder(filename.c_str()));
		if (flacdec->IsInit())
			return flacdec;
	}
	else if (filetype == FILE_MP3) {
		auto mp3dec = std::unique_ptr<Decoder>(new Mp3Decoder(filename.c_str()));
		if (mp3dec->IsInit())
			return mp3dec;
	}
	else if (filetype == FILE_VORBIS) {
		auto vorbisdec = std::unique_ptr<Decoder>(new VorbisDecoder(filename.c_str()));
		if (vorbisdec->IsInit())
			return vorbisdec;
	}
	else if (filetype == FILE_OPUS) {
		auto opusdec = std::unique_ptr<Decoder>(new OpusDecoder(filename.c_str()));
		if (opusdec->IsInit())
			return opusdec;
	}	
	else if (filetype == FILE_MIDI) {
		auto mididec = std::unique_ptr<Decoder>(new MidiDecoder(filename.c_str(), "sdmc:/3ds/limeplayer3ds/dgguspat/wildmidi.cfg"));
		if (mididec->IsInit())
			return mididec;	
	}
	return nullptr;
}
