#include "playerget.h"
#include "formats/flac.h"
#include "formats/mp3.h"
#include "formats/vorbis.h"
#include "formats/wav.h"
#include "formats/midi.h"
#include "formats/opus.h"

int16_t*		buffer1 = NULL;
int16_t*		buffer2 = NULL;
ndspWaveBuf		waveBuf[2];
int32_t Samplerate = 0;
int32_t Channels = 0;
size_t  Buffsize = 0;
int32_t	format = 0;
bool	lastbuf = false;

static void ctr_InitPlayer(void);
static int ctr_getOutput();

int playerget_InitDecoder(playbackInfo_t* info, int filetype) {
	if (filetype == FILE_WAV) {
		Buffsize = initWav(info);
		Samplerate = rateWav();
		Channels = channelWav();
	}

	else if (filetype == FILE_FLAC) {
		Buffsize = initFlac(info);
		Samplerate = rateFlac();
		Channels = channelFlac();
	}

	else if (filetype == FILE_MP3) {
		mp3GetBuffSize(&Buffsize);
		initMp3(info);
		Samplerate = rateMp3();
		Channels = channelMp3();
	}

	else if (filetype == FILE_VORBIS) {
		Buffsize = initVorbis(info);
		Samplerate = rateVorbis();
		Channels = channelVorbis();
	}
	
	else if (filetype == FILE_OPUS) {
		Buffsize = initOpus(info);
		Samplerate = rateOpus();
		Channels = channelOpus();
	}
		
	else if (filetype == FILE_MIDI) {
		Buffsize = initMidi(info);
		Samplerate = rateMidi();
		Channels = channelMidi();
	}
	format = filetype;
	#ifdef _3DS
	if (Buffsize != -1) {
		ctr_InitPlayer();
	}
	#endif
	return Buffsize == -1 ? -1 : 0;
}

void playerget_Exit(void) {
	if (format == FILE_WAV) {
		exitWav();
	}

	else if (format == FILE_FLAC) {
		exitFlac();
	}

	else if (format == FILE_MP3) {
		exitMp3();
	}

	else if (format == FILE_VORBIS) {
		exitVorbis();
	}
	
	else if (format == FILE_OPUS) {
		exitOpus();
	}
		
	else if (format == FILE_MIDI) {
		exitMidi();
	}
	linearFree(buffer1);
	linearFree(buffer2);
	lastbuf = false;
	format = 0;
}

size_t playerget_Decode(void *buffer) {
	if (format == FILE_WAV) {
		return readWav(buffer);
	}

	else if (format == FILE_FLAC) {
		return decodeFlac(buffer);
	}

	else if (format == FILE_MP3) {
		return decodeMp3(buffer);
	}

	else if (format == FILE_VORBIS) {
		return decodeVorbis(buffer);
	}
	
	else if (format == FILE_OPUS) {
		return decodeOpus(buffer);
	}
		
	else if (format == FILE_MIDI) {
		return decodeMidi(buffer);
	}
	return 0;
}

void playerget_FileInfo(playbackInfo_t* info) {
	if (format == FILE_WAV) {
		infoWav(info);
	}

	else if (format == FILE_FLAC) {
		infoFlac(info);
	}

	else if (format == FILE_MP3) {
		infoMp3(info);
	}

	else if (format == FILE_VORBIS) {
		infoVorbis(info);
	}
	
	else if (format == FILE_OPUS) {
		infoOpus(info);
	}
		
	else if (format == FILE_MIDI) {
		infoMidi(info);
	}
}

uint32_t playerget_totalLength(void) {
	if (format == FILE_WAV) {
		return lengthWav();
	}

	else if (format == FILE_FLAC) {
		return lengthFlac();
	}

	else if (format == FILE_MP3) {
		return lengthMp3();
	}

	else if (format == FILE_VORBIS) {
		return lengthVorbis();
	}
	
	else if (format == FILE_OPUS) {
		return lengthOpus();
	}
		
	else if (format == FILE_MIDI) {
		return lengthMidi();
	}
	return 0;
}

uint32_t playerget_currentPos(void) {
	if (format == FILE_WAV) {
		return posWav();
	}

	else if (format == FILE_FLAC) {
		return posFlac();
	}

	else if (format == FILE_MP3) {
		return posMp3();
	}

	else if (format == FILE_VORBIS) {
		return posVorbis();
	}
	
	else if (format == FILE_OPUS) {
		return posOpus();
	}
		
	else if (format == FILE_MIDI) {
		return posMidi();
	}
	return 0;
}

uint32_t playerget_rate(void) {
	if (format == FILE_WAV) {
		return rateWav();
	}

	else if (format == FILE_FLAC) {
		return rateFlac();
	}

	else if (format == FILE_MP3) {
		return rateMp3();
	}

	else if (format == FILE_VORBIS) {
		return rateVorbis();
	}
	
	else if (format == FILE_OPUS) {
		return rateOpus();
	}
		
	else if (format == FILE_MIDI) {
		return rateMidi();
	}
	return 0;
}


void playerget_seek(uint32_t location) {
	if (format == FILE_WAV) {
		seekWav(location);
	}

	else if (format == FILE_FLAC) {
		seekFlac(location);
	}

	else if (format == FILE_MP3) {
		seekMp3(location);
	}

	else if (format == FILE_VORBIS) {
		seekVorbis(location);
	}
	
	else if (format == FILE_OPUS) {
		seekOpus(location);
	}
		
	else if (format == FILE_MIDI) {
		seekMidi(location);
	}
}

/**/

int playerget_SendOutput() {
		#ifdef _3DS
		return ctr_getOutput();
		#endif
}

void ctr_InitPlayer() {
#ifdef _3DS
	buffer1 = linearAlloc(Buffsize * sizeof(int16_t));
	buffer2 = linearAlloc(Buffsize * sizeof(int16_t));

	ndspChnReset(CHANNEL);
	ndspChnWaveBufClear(CHANNEL);
	ndspSetOutputMode(NDSP_OUTPUT_STEREO);
	ndspChnSetInterp(CHANNEL, NDSP_INTERP_POLYPHASE);
	ndspChnSetRate(CHANNEL, Samplerate);
	ndspChnSetFormat(CHANNEL, Channels == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);

	memset(waveBuf, 0, sizeof(waveBuf));
	waveBuf[0].nsamples = playerget_Decode(&buffer1[0]) / Channels;
	waveBuf[0].data_vaddr = &buffer1[0];
	ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);

	waveBuf[1].nsamples = playerget_Decode(&buffer2[0]) / Channels;
	waveBuf[1].data_vaddr = &buffer2[0];
	ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);
#endif
}

int ctr_getOutput() {
#ifdef _3DS
		/* When the last buffer has finished playing, break. */
		if(lastbuf == true && waveBuf[0].status == NDSP_WBUF_DONE &&
				waveBuf[1].status == NDSP_WBUF_DONE)
			return -1;

		while (ndspChnIsPaused(CHANNEL) == true || lastbuf == true)
			return -2;

		if(waveBuf[0].status == NDSP_WBUF_DONE)
		{
			size_t read = playerget_Decode(&buffer1[0]);

			if(read <= 0)
			{
				lastbuf = true;
				return -2;
			}
			else if(read < Buffsize)
				waveBuf[0].nsamples = read / Channels;

			ndspChnWaveBufAdd(CHANNEL, &waveBuf[0]);
		}

		if(waveBuf[1].status == NDSP_WBUF_DONE)
		{
			size_t read = playerget_Decode(&buffer2[0]);

			if(read <= 0)
			{
				lastbuf = true;
				return -2;
			}
			else if(read < Buffsize)
				waveBuf[1].nsamples = read / Channels;

			ndspChnWaveBufAdd(CHANNEL, &waveBuf[1]);
		}
		
		
		DSP_FlushDataCache(buffer1,Buffsize * sizeof(int16_t));
		DSP_FlushDataCache(buffer2, Buffsize * sizeof(int16_t));
		return 0;
#endif
}
