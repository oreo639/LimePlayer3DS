#ifndef DECODER_WAV_H
#define DECODER_WAV_H

#include <dr_libs/dr_wav.h>

#include "player.hpp"

class WavDecoder : public Decoder {
	public:
		WavDecoder(const char* filename);
		
		~WavDecoder(void);
		
		void Info(metaInfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(void* buffer) override;
		
		uint32_t Samplerate(void) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
	private:
		const size_t buffSize = 16 * 1024;
		drwav*       pWav = NULL;
		uint32_t     wavprogress;
};

#endif
