#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <memory>

#include "settings.hpp"
#include "error.hpp"

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
		// Stream Decoder interface
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

	void SkipPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);
}

#endif
