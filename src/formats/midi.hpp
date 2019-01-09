#ifndef DECODER_MIDI_H
#define DECODER_MIDI_H

#include "player.hpp"

class MidiDecoder : public Decoder {
	public:
		MidiDecoder(const char* filename, const char* midicfg);
		
		~MidiDecoder(void);
		
		bool IsInit(void) override;
		
		void Info(std::string& copyright) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(int16_t* buffer) override;
		
		uint32_t Samplerate(void) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};

#endif
