#ifndef DECODER_MIDI_H
#define DECODER_MIDI_H

#include <wildmidi_lib.h>

#include "player.hpp"

#define MIDISAMPLERATE 32000
#define SAMPLESPERBUF 4096
#define BYTESPERSAMPLE 4

class MidiDecoder : public Decoder {
	public:
		MidiDecoder(const char* filename, const char* midicfg);

		~MidiDecoder(void);

		void GetInfo(metaInfo_t* Meta) override;

		uint32_t Position(void) override;

		uint32_t Length(void) override;

		void Seek(uint32_t location) override;

		uint32_t Decode(void* buffer) override;

		uint32_t Samplerate(void) override;

		uint32_t Buffsize(void) override;

		int Channels(void) override;
	private:
		midi *wMidi;
		struct _WM_Info	*midiInfo;
		size_t buffSize = SAMPLESPERBUF*BYTESPERSAMPLE;
};

#endif
