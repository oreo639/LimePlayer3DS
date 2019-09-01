#ifndef DECODER_VORBIS_H
#define DECODER_VORBIS_H

#include "player.hpp"

class VorbisDecoder : public Decoder {
	public:
		VorbisDecoder(const char* filename);
		
		~VorbisDecoder(void);
		
		void Info(metaInfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(void* buffer) override;
		
		uint32_t Samplerate(void) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};
int isVorbis(const char* in);

#endif
