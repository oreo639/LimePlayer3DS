#ifndef DECODER_OPUS_H
#define DECODER_OPUS_H

#include "player.hpp"

class OpusDecoder : public Decoder {
	public:
		OpusDecoder(FileTransport* transport);
		
		~OpusDecoder(void);
		
		void Info(metaInfo_t* Meta) override;
		
		uint32_t Position(void) override;
		
		uint32_t Length(void) override;
		
		void Seek(uint32_t location) override;
		
		uint32_t Decode(void* buffer) override;
		
		uint32_t Samplerate(void) override;
		
		uint32_t Buffsize(void) override;
		
		int Channels(void) override;
};
int isOpus(const char* in);

#endif
