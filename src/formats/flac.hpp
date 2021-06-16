#ifndef DECODER_FLAC_H
#define DECODER_FLAC_H

#include <FLAC/stream_decoder.h>

#include "player.hpp"

typedef struct flac_callback_info
{
	unsigned bits_per_sample = 0;
	unsigned sample_rate = 0;
	unsigned channels = 0;
	unsigned long total_samples = 0;

	int32_t *output_buffer = nullptr;
	unsigned buffer_used = 0;
	int32_t *write_pointer = nullptr;
} flac_callback_info;

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
		flac_callback_info cFlac;
};

//int isFlac(const char* in);
#endif
