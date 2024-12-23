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
			return decoderName;
		}

		// Allow the decoder to specify if the medadata needs to be updated.
		virtual bool AllowUpdateInfo() {return false;}

		virtual void UpdateInfo(metaInfo_t* Meta) {};

		// Allow the decoder to implement custom next/previous behaviour.
		virtual void Next() {
			uint32_t time = (Position() / Samplerate());
			Seek((time + 15) * Samplerate());
		}

		virtual void Previous() {
			uint32_t time = (Position() / Samplerate());
			Seek((time < 15 ? 0 : time - 15) * Samplerate());
		}

		virtual void GetInfo(metaInfo_t* Meta) = 0;
		
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

		std::string decoderName = "Unknown";
};

namespace PlayerInterface {
	void ThreadMainFunct(void* input);
	
	bool TogglePlayback(void);
		
	void ExitPlayback(void);

	void SkipPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);

	uint32_t GetTotalLength(void);

	uint32_t GetCurrentPos(void);

	void NextSection(void);

	void PreviousSection(void);

	void SeekSection(uint32_t location);

	void SeekSectionPercent(uint32_t percent);

	std::string GetDecoderName(void);
}

#endif
