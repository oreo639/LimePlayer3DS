#include "player.h"

/**
 * Initialise Midi decoder.
 *
 * \param	file	Location of midi file to play.
 * \return			0 on success, else failure.
 */
int initMidi(playbackInfo_t* info);

/**
 * Get sampling rate of Midi file.
 *
 * \return	Sampling rate.
 */
uint32_t rateMidi(void);

/**
 * Get number of channels of Midi file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelMidi(void);

/**
 * Decode part of open Midi file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeMidi(void* buffer);


void infoMidi(playbackInfo_t* infoOut);

uint32_t posMidi(void);

uint32_t lengthMidi(void);

void seekMidi(uint32_t location);

/**
 * Free Midi decoder.
 */
void exitMidi(void);
