#ifndef DECODER_XMP_H
#define DECODER_XMP_H

#include <xmp.h>

#include "player.hpp"

#define XMPSAMPLERATE 32000
#define SAMPLESPERBUF 4096
#define BYTESPERSAMPLE 4

class XmpDecoder : public Decoder {
	public:
		XmpDecoder(const char* filename);

		~XmpDecoder(void);

		void GetInfo(metaInfo_t* Meta) override;

		uint32_t Position(void) override;

		uint32_t Length(void) override;

		void Seek(uint32_t location) override;

		void Next(void) override;

		void Previous(void) override;

		uint32_t Decode(void* buffer) override;

		uint32_t Samplerate(void) override;

		uint32_t Buffsize(void) override;

		int Channels(void) override;
	private:
		xmp_context ctx;
		struct xmp_module_info mi;
		size_t buffSize = SAMPLESPERBUF*BYTESPERSAMPLE;
};

#endif
