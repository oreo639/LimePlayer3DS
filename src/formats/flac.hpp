#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include <FLAC/stream_decoder.h>
#include "flac_callbacks/flac_callbacks.hpp"

#include "player.hpp"

class FlacDecoder : public Decoder {
	public:
		FlacDecoder(const char* filename);

		~FlacDecoder(void);

		void GetInfo(metaInfo_t* Meta) override;

		uint32_t Position(void) override;

		uint32_t Length(void) override;

		void Seek(uint32_t location) override;

		uint32_t Decode(void* buffer) override;

		uint32_t Samplerate(void) override;

		uint32_t Buffsize(void) override;

		int Channels(void) override;
	private:
		FLAC__StreamDecoder *dFlac;
		flac_callback_info *cFlac;
};

//int isFlac(const char* in);
#endif
