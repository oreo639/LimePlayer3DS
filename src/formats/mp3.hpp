#ifndef DECODER_MP3_H
#define DECODER_MP3_H

#include <mpg123.h>

#include "player.hpp"

class Mp3Decoder : public Decoder {
	public:
		Mp3Decoder(FileTransport* transport);

		~Mp3Decoder(void);

		void Info(metaInfo_t* Meta) override;

		uint32_t Position(void) override;

		uint32_t Length(void) override;

		void Seek(uint32_t location) override;

		uint32_t Decode(void* buffer) override;

		uint32_t Samplerate(void) override;

		uint32_t Buffsize(void) override;

		int Channels(void) override;
	private:
		mpg123_handle* mh;
		size_t buffSize;
		int32_t rate;
		int channels;
};

//bool isMp3(const std::string& filename);

#endif
