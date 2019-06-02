#ifndef __LIME_STREAM_VORBIS_H__
#define __LIME_STREAM_VORBIS_H__

#include "player.hpp"

/*class VorbisStreamDecoder : public StreamDecoder {
	public:
		VorbisStreamDecoder(uint8_t* inbuffer, uint32_t inbufsize);
		
		~VorbisStreamDecoder(void);
		
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
};*/

int isVorbisStream(uint8_t* inbuffer, uint32_t inbufsize);

#endif
