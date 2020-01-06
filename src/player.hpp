#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <memory>

#include "settings.hpp"
#include "debug.hpp"
#include "transport/transport.hpp"

typedef struct
{
	std::string	Title;
	std::string	Album;
	std::string	Artist;
	std::string	Year;
	std::string	Comment;
	std::string	Genre;
	bool		isSupported;
	bool		isParsed;
	//void	*Image;
} metaInfo_t;

typedef struct
{
	std::string	filepath;
	std::string	filename;
	metaInfo_t	fileMeta;
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
		Decoder(const char* name) : DecoderName(name) {}

		Decoder(void) {}

		virtual ~Decoder() {}

		bool GetIsInit(void) {return IsInit;}

		const char* GetErrInfo(void) {return ErrInfo;}

		virtual std::string GetDecoderName(void) {
			if (DecoderName)
				return DecoderName;
			else
				return "";
		}

		// Allow the decoder to specify if the medadata needs to be updated.
		virtual bool AllowUpdateInfo() {return false;}

		virtual void Info(metaInfo_t* Meta);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(void* buffer);
		
		virtual uint32_t Samplerate(void);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);

	protected:
		bool IsInit = 0;

		const char* ErrInfo;
		const char* DecoderName = "Unknown";
};

namespace PlayerInterface {
	void ThreadMainFunct(void* input);
	
	bool TogglePlayback(void);
		
	void ExitPlayback(void);

	void SkipPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);

	uint32_t GetTotalLength(void);

	uint32_t GetTotalTime(void);

	uint32_t GetCurrentPos(void);

	uint32_t GetCurrentTime(void);

	void SeekSection(uint32_t location);

	void SeekSectionPercent(uint32_t percent);

	void SeekSectionTime(int time);

	std::string GetDecoderName(void);
}

#endif
