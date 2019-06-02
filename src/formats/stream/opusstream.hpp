#ifndef __LIME_STREAM_OPUS_H__
#define __LIME_STREAM_OPUS_H__

#include "player.hpp"

class OpusStreamDecoder : public StreamDecoder {
	public:
		OpusStreamDecoder(uint8_t* inbuffer, uint32_t inbufsize);
		
		~OpusStreamDecoder(void);
		
		bool IsInit(void) override;
		
		void Info(musinfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer) override;
		
		uint32_t Samplerate(void) override;

		uint32_t Spf(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};

int isOpusStream(uint8_t* inbuffer, uint32_t inbufsize);

#endif
