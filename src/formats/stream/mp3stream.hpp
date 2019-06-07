#ifndef __LIME_STREAM_MP3_H___
#define __LIME_STREAM_MP3_H___

#include "player.hpp"

class Mp3StreamDecoder : public StreamDecoder {
	public:
		Mp3StreamDecoder(uint8_t* inbuffer, uint32_t inbufsize);
		
		~Mp3StreamDecoder(void);
		
		void Info(musinfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer) override;
		
		uint32_t Samplerate(void) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};

#endif
