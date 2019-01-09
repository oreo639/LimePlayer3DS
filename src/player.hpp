#ifndef __LIME_PLAYER_H__
#define __LIME_PLAYER_H__

#include <memory>

#include "settings.h"
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
	std::string	file;
	settings_t	settings;
	musinfo_t	fileMeta;
} playbackInfo_t;

enum Format {
	FORMAT_U8,
	FORMAT_U16,
};

class Decoder {
	public:
		virtual ~Decoder() {}
		// Decoder interface
		virtual bool IsInit(void);
		
		virtual void Info(std::string& copyright);
		
		virtual uint32_t Position(void);
		
		virtual uint32_t Length(void);
		
		virtual void Seek(uint32_t location);
		
		virtual uint32_t Decode(int16_t* buffer);
		
		virtual uint32_t Samplerate(void);
		
		virtual uint32_t Buffsize(void);
		
		virtual int Channels(void);
};

class Player
{
	public:
		Player() {};
		~Player() {};
		
		void Play(const std::string& filename);
		
	private:
		std::unique_ptr<Decoder> GetFormat(const std::string& filename, int filetype);
};

namespace PlayerInterface {
	void ThreadMainFunct(void* input);
	
	bool TogglePlayback(void);
		
	void ExitPlayback(void);
		
	bool IsPlaying(void);
		
	bool IsPaused(void);
}

#endif
