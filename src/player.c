/*   LimePlayer3DS FOSS graphcal music player for the 3ds.
*    Copyright (C) 2018  LimePlayer Team
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
#include <3ds.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "playerget.h"
#include "file.h"
#include "player.h"


//#define MIDISAMPLERATE 22050
//#define SAMPLESPERBUF (MIDISAMPLERATE / 30)

static volatile bool stop = true;

/**
 * Pause or play current file.
 *
 * \return	True if paused.
 */
bool togglePlayback(void) {
	bool paused = ndspChnIsPaused(CHANNEL);
	ndspChnSetPaused(CHANNEL, !paused);
	return !paused;
}

/**
 * Stops current playback. Playback thread should exit as a result.
 */
void stopPlayback(void) {
	stop = true;
}

/**
 * Returns whether music is playing or stoped.
 * \return	True if not stoped.
 */
bool isPlaying(void) {
	return !stop;
}

/**
 * Returns whether music is playing or paused.
 * \return	True if paused.
 */
bool isPaused(void) {
	return ndspChnIsPaused(CHANNEL);
}

/*
 * Returns the total number of samples in a file.
 * \return	Total number of samples.
 */
uint32_t getTotalLength(void) {
	if (!stop) {
		return playerget_totalLength();
	} else {
		return 0;
	}
}

/*
 * Returns the estimated total number of seconds in the file.
 * \return	Estimated total seconds.
 */
uint32_t getTotalTime(void) {
	if (!stop) {
		return getTotalLength()/playerget_rate();
	} else {
		return 0;
	}
}

/*
 * Returns the current sample.
 * \return	Current sample.
 */
uint32_t getCurrentPos(void) {
	if (!stop) {
		return playerget_currentPos();
	} else {
		return 0;
	}
}

/*
 * Returns the estimated number of seconds elapsed.
 * \return	Current location in file in seconds.
 */
uint32_t getCurrentTime(void) {
	if (!stop) {
		return getCurrentPos()/playerget_rate();
	} else {
		return 0;
	}
}

/*
 * Seeks to a specific sample in the file,
 * although it is not meant to be called directly
 * it is avliable to be used by other functions to provide seeking
 * using percent, seconds, etc.
 */
void seekSection(int location) {
	if (location < 0)
		location = 0;
	if (!stop) {
		togglePlayback(); //Pause playback...
		playerget_seek(location);
		togglePlayback(); //once the seeking is done, playback can continue.
	} else {
		return;
	}
}

/*
 * Seeks to a percent in the file.
 */
void seekSectionPercent(int percent) {
	if (percent < 0)
		percent = 0;
	if (!stop) {
		seekSection((getTotalLength() * (percent / 100.0)));
	} else {
		return;
	}
}

/*
 * Estimates the elapsed samples for a given time in seconds.
 */
void seekSectionTime(int time) {
	if (time < 0)
		time = 0;
	if (!stop) {
		seekSection((time*playerget_rate()));
	} else {
		return;
	}
}

/**
 * Should only be called from a new thread only, and have only one playback
 * thread at time. This function has not been written for more than one
 * playback thread in mind.
 *
 * \param	infoIn	Playback information.
 */
void playFile(void* infoIn) {
	playbackInfo_t* 	info = infoIn;
	const char*		file = info->file;
	int filetype;

	*info->errInfo->error = 0; //Tell watchdog that song is playing
	/* Reset previous stop command */
	stop = false;
	
	filetype = FILE_getfiletype(file);
	if (filetype == -1) {
		*info->errInfo->error = FILE_NOT_SUPPORTED;
		goto out;
	}
	
	if (playerget_InitDecoder(info, filetype) == -1) {
		*info->errInfo->error = DECODER_INIT_FAIL;
		goto out;
	}

	/**
	 * There may be a chance that the music has not started by the time we get
	 * to the while loop. So we ensure that music has started here.
	 */

	while(ndspChnIsPlaying(CHANNEL) == false);
	playerget_FileInfo(info);
	int signal;
	while(stop == false)
	{
		svcSleepThread(100 * 1000);

		signal = playerget_SendOutput();
		if (signal == -1) {
			break;
		}
	}

	playerget_Exit();
	
	ndspChnWaveBufClear(CHANNEL);

	free(info->file);
	info->file = NULL;
	free(info->fileMeta->authorCpright);
	info->fileMeta->authorCpright = NULL;
out:
	/* Signal Main thread that we've stopped playing */
	if(*info->errInfo->error == 0) *info->errInfo->error = -1;
	svcSignalEvent(*info->errInfo->failEvent);
	stop = true;

	threadExit(0);
	return;
}
