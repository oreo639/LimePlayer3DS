#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <memory>

#include "settings.hpp"
#include "error.hpp"

///https://github.com/FlagBrew/PKSM/blob/PKSM.cpp/source/gui/gui.cpp#L281-L297
typedef struct
{
	std::string	songName;
	std::string	authorCpright;
	std::string	description;
} musinfo_t;

typedef struct
{
	std::string	filename;
	settings_t	settings;
	musinfo_t	fileMeta;
} playbackInfo_t;

enum Format {
	FORMAT_16,
	FORMAT_8,
	FORMAT_ADPCM
};

class Decoder {
	public:
		virtual ~Decoder() {}
		// Decoder interface
		virtual bool IsInit(void) {return false;};
		
		virtual void Info(musinfo_t* Meta);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(void* buffer);
		
		virtual uint32_t Samplerate(void);

		virtual uint32_t Spf(void* buffer);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);
};

class StreamDecoder {
	public:
		virtual ~StreamDecoder() {}
		// Decoder interface
		virtual bool IsInit(void) {return false;};
		
		virtual void Info(musinfo_t* Meta);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer);
		
		virtual uint32_t Samplerate(void);

		virtual uint32_t Spf(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);
};


class Player
{
	public:
		Player() {};
		~Player() {};
		
		void Play(playbackInfo_t* playbackInfo);

		void ClearMetadata(musinfo_t* fileMeta);
		
	private:
		Decoder* GetFormat(const playbackInfo_t* playbackInfo, int filetype);
};

namespace PlayerInterface {
	void ThreadMainFunct(void* input);
	
	bool TogglePlayback(void);
		
	void ExitPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);
}

#endif
