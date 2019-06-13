#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <memory>

#include "settings.hpp"
#include "debug.hpp"

typedef struct
{
	std::string	songName;
	std::string	authorCpright;
	std::string	description;
} musinfo_t;

typedef struct
{
	std::string	filename;
	musinfo_t	fileMeta;
	settings_t	settings;
	playlist_t	playlistfile;
	int		usePlaylist; // 0 do not use playlist, 1 use playlistfile, 2 use playlist from settings
	uint32_t	selectPlaylistOffset;
} playbackInfo_t;

enum Format {
	FORMAT_16,
	FORMAT_8,
	FORMAT_ADPCM
};

class Decoder {
	public:
		// Decoder interface
		bool GetIsInit(void) {return IsInit;};

		const char* GetErrInfo(void) {return ErrInfo;};

		virtual ~Decoder() {}

		virtual void Info(musinfo_t* Meta);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(void* buffer);
		
		virtual uint32_t Samplerate(void);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);

	protected:
		bool IsInit = 0;

		const char* ErrInfo = NULL;
};

class StreamDecoder {
	public:
		virtual ~StreamDecoder() {}
		// Stream Decoder interface
		bool GetIsInit(void) {return IsInit;};
		
		virtual void Info(musinfo_t* Meta);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(uint8_t* inbuffer, uint32_t inbuffsize, void* outbuffer);
		
		virtual uint32_t Samplerate(void);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);

	protected:
		bool IsInit = 0;
};

namespace PlayerInterface {
	void ThreadMainFunct(void* input);
	
	bool TogglePlayback(void);
		
	void ExitPlayback(void);

	void SkipPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);
}

#endif
