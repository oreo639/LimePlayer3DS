#include "player.h"

/**
 * Initialise Flac decoder.
 *
 * \param	file	Location of flac file to play.
 * \return			0 on success, else failure.
 */
int initFlac(playbackInfo_t* info);

/**
 * Get sampling rate of Flac file.
 *
 * \return	Sampling rate.
 */
uint32_t rateFlac(void);

/**
 * Get number of channels of Flac file.
 *
 * \return	Number of channels for opened file.
 */
uint8_t channelFlac(void);

/**
 * Decode part of open Flac file.
 *
 * \param buffer	Decoded output.
 * \return			Samples read for each channel.
 */
uint64_t decodeFlac(void* buffer);



void infoFlac(playbackInfo_t* infoOut);

uint32_t posFlac(void);

uint32_t lengthFlac(void);

void seekFlac(uint32_t location);

/**
 * Free Flac decoder.
 */
void exitFlac(void);

/**
 * Checks if the input file is Flac
 *
 * \param in	Input file.
 * \return		0 if Flac file, else not or failure.
 */
int isFlac(const char* in);
