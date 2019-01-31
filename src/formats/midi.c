#include <wildmidi_lib.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "midi.h"
#include "error.h"

#define MIDISAMPLERATE 22050
#define SAMPLESPERBUF (MIDISAMPLERATE / 30)
#define BYTESPERSAMPLE 4

static midi*		wMidi;
static const size_t	buffSize = SAMPLESPERBUF*BYTESPERSAMPLE;
static struct _WM_Info *midiInfo;
uint32_t midiprogress;



/**
 * Initialise Midi decoder.
 *
 * \param	file	Location of midi file to play.
 * \return			0 on success, else failure.
 */
int initMidi(playbackInfo_t* info)
{
	int res = WildMidi_Init(info->settings->wildMidiConfig, MIDISAMPLERATE, 0);
	wMidi = WildMidi_Open(info->file);

	return res != 0 ? -1 : buffSize;
}

void infoMidi(playbackInfo_t* infoOut) {
	midiInfo = WildMidi_GetInfo(wMidi);	
	if(midiInfo->copyright)
		infoOut->fileMeta->authorCpright = strdup(midiInfo->copyright);
	else
		infoOut->fileMeta->authorCpright = strdup("(No Author-Midi)");
}

/**
 * Get sampling rate of Midi file.
 *
 * \return	Sampling rate.
 */
uint32_t rateMidi(void)
{
	return MIDISAMPLERATE;
}

/**
 * Get number of channels of Midi file.
 *
 * \return	Number of channels for opened Midi file (always 2 because wildmidi outputs stereo).
 */
uint8_t channelMidi(void)
{
	return 2;
}

uint32_t posMidi(void) {
	WildMidi_GetInfo(wMidi);
	return midiInfo->current_sample;
}

uint32_t lengthMidi(void) {
	return midiInfo->approx_total_samples;
}

void seekMidi(uint32_t location)
{
	if(location > midiInfo->approx_total_samples) {
		return;
	}
	WildMidi_FastSeek(wMidi, &location);
}

/**
 * Decode part of open Midi file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeMidi(void* buffer)
{
	return (WildMidi_GetOutput(wMidi, buffer, buffSize))/2;
}

/**
 * Free Midi handle and decoder.
 */
void exitMidi(void)
{
	WildMidi_Close(wMidi);
	WildMidi_Shutdown();
}
