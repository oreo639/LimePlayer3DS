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
#include <memory>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "midi.hpp"

MidiDecoder::MidiDecoder(const char* filename, const char* midicfg) : Decoder("Midi") {
	int res = WildMidi_Init(midicfg, MIDISAMPLERATE, 0);
	wMidi = WildMidi_Open(filename);
	WildMidi_GetInfo(wMidi);
	if (!wMidi || res != 0)
		return;

	mIsInit = true;
}

MidiDecoder::~MidiDecoder(void) {
	WildMidi_Close(wMidi);
	WildMidi_Shutdown();
	mIsInit = false;
}

void MidiDecoder::Info(metaInfo_t* Meta) {
	midiInfo = WildMidi_GetInfo(wMidi);	
	if (midiInfo && midiInfo->copyright)
		Meta->Artist.assign(midiInfo->copyright);
	else
		Meta->Artist.assign("(No Author-Midi)");
}

uint32_t MidiDecoder::Position(void) {
	WildMidi_GetInfo(wMidi);
	return midiInfo->current_sample;
}

uint32_t MidiDecoder::Length(void) {
	return midiInfo->approx_total_samples;
}

void MidiDecoder::Seek(uint32_t location)
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
 * \return		Samples read for each channel.
 */
uint32_t MidiDecoder::Decode(void* buffer)
{
	return WildMidi_GetOutput(wMidi, static_cast<int8_t*>(buffer), buffSize)/sizeof(uint16_t);
}

/**
 * Get sampling rate of Midi file.
 *
 * \return	Sampling rate.
 */
uint32_t MidiDecoder::Samplerate(void)
{
	return MIDISAMPLERATE;
}

uint32_t MidiDecoder::Buffsize(void)
{
	return buffSize;
}

int MidiDecoder::Channels(void)
{
	return 2;
}
