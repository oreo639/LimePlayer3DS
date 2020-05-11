#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <string>
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
		Decoder(const char* name) : decoderName(name) {}

		Decoder() {}

		virtual ~Decoder() {}

		bool GetIsInit(void) {return mIsInit;}

		const char* GetErrInfo(void) {return mErrInfo;}

		std::string GetDecoderName(void) {
			if (decoderName)
				return decoderName;
			else
				return "";
		}

		// Allow the decoder to specify if the medadata needs to be updated.
		virtual bool AllowUpdateInfo() {return false;}

		virtual void Info(metaInfo_t* Meta) = 0;
		
		virtual uint32_t Position(void) = 0;
		
		virtual uint32_t Length(void) = 0;
		
		virtual void Seek(uint32_t location) = 0;
		
		virtual uint32_t Decode(void* buffer) = 0;
		
		virtual uint32_t Samplerate(void) = 0;
		
		virtual uint32_t Buffsize(void) = 0;
		
		virtual int Channels(void) = 0;

	protected:
		bool mIsInit = 0;

		const char* mErrInfo;

	private:
		const char* decoderName = "Unknown";
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
