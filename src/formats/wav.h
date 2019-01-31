#include "player.h"

/**
 * Initialise WAV playback.
 *
 * \param	file	Location of WAV file to play.
 * \return			0 on success, else failure.
 */
int initWav(playbackInfo_t* info);

/**
 * Get sampling rate of Wav file.
 *
 * \return	Sampling rate.
 */
uint32_t rateWav(void);

/**
 * Get number of channels of Wav file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelWav(void);

/**
 * Read part of open Wav file.
 *
 * \param buffer	Output.
 * \return			Samples read for each channel.
 */
uint64_t readWav(void* buffer);

void infoWav(playbackInfo_t* infoOut);

uint32_t posWav(void);

uint32_t lengthWav(void);

void seekWav(uint32_t location);

/**
 * Free Wav file.
 */
void exitWav(void);
