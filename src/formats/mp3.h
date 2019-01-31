#include <stdint.h>
#include "player.h"

/**
 * Initialise MP3 decoder.
 *
 * \param	file	Location of MP3 file to play.
 * \return			0 on success, else failure.
 */
int initMp3(playbackInfo_t* info);

/**
 * Get sampling rate of MP3 file.
 *
 * \return	Sampling rate.
 */
uint32_t rateMp3(void);

/**
 * Get number of channels of MP3 file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelMp3(void);

/**
 * Decode part of open MP3 file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeMp3(void* buffer);

void mp3GetBuffSize(size_t *sizeOut);

void infoMp3(playbackInfo_t* infoOut);

uint32_t posMp3(void);

uint32_t lengthMp3(void);

void seekMp3(uint32_t location);

/**
 * Free MP3 decoder.
 */
void exitMp3(void);
