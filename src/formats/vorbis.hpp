#ifndef DECODER_VORBIS_H
#define DECODER_VORBIS_H

#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include "player.hpp"

class VorbisDecoder : public Decoder {
	public:
		VorbisDecoder(const char* filename);

		~VorbisDecoder(void);

		bool AllowUpdateInfo() override {return true;}

		void UpdateInfo(metaInfo_t* Meta) override;

		void GetInfo(metaInfo_t* Meta) override;

		uint32_t Position(void) override;

		uint32_t Length(void) override;

		void Seek(uint32_t location) override;

		uint32_t Decode(void* buffer) override;

		uint32_t Samplerate(void) override;

		uint32_t Buffsize(void) override;

		int Channels(void) override;

		uint64_t fillVorbisBuffer(char* bufferOut);
	private:
		OggVorbis_File vorbisFile;
		vorbis_info    *vi;
		FILE           *f;
		const size_t   buffSize = 8 * 4096;
};

int isVorbis(const char* in);

#endif
