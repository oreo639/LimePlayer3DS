#ifndef __LIME_SETTINGS_H__
#define __LIME_SETTINGS_H__
#include <vector>
#include <string>

typedef struct
{
	std::string		 name;
	std::vector<std::string> files;
} playlist_t;

typedef std::vector<playlist_t> playvec;

typedef struct
{
	std::string		wildMidiConfig;
	uint32_t		theme;
	int			language;
	playvec		playlists;
} settings_t;

#endif
