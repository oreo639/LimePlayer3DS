#ifndef DECODER_MP3_H
#define DECODER_MP3_H

#include "player.hpp"

class Mp3Decoder : public Decoder {
	public:
		Mp3Decoder(const char* filename);
		
		~Mp3Decoder(void);
		
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

int isFlac(const char* in);
#endif
