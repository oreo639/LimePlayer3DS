#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include "player.hpp"

class FlacDecoder : public Decoder {
	public:
		FlacDecoder(const char* filename);
		
		~FlacDecoder(void);
		
		bool IsInit(void) override;
		
		void Info(musinfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(void* buffer) override;
		
		uint32_t Samplerate(void) override;

		uint32_t Spf(void* buffer) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};

int isFlac(const char* in);
#endif
