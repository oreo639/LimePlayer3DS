#include <stdbool.h>

#include "settings.h"
#include "error.h"

#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

/* Channel to play music on */
#define CHANNEL	0x08

typedef struct
{
	char*				songName;
	char*				authorCpright;
	char*				description;
} musinfo_t;

typedef struct
{
	char*				file;
	settings_t*			settings;
	errInfo_t*			errInfo;
	musinfo_t*			fileMeta;
} playbackInfo_t;

/**
 * Pause or play current file.
 *
 * \return	True if paused.
 */
bool togglePlayback(void);

/**
 * Stops current playback. Playback thread should exit as a result.
 */
void stopPlayback(void);

/**
 * Returns whether music is playing or stoped.
 */
bool isPlaying(void);

/**
 * Returns whether music is playing or paused.
 */
bool isPaused(void);

uint32_t getTotalLength(void);

uint32_t getTotalTime(void);

uint32_t getCurrentPos(void);

uint32_t getCurrentTime(void);

void seekSection(int location);

void seekSectionPercent(int percent);

void seekSectionTime(int time);

/**
 * Should only be called from a new thread only, and have only one playback
 * thread at time. This function has not been written for more than one
 * playback thread in mind.
 *
 * \param	infoIn	Playback information.
 */
void playFile(void* infoIn);

#endif
