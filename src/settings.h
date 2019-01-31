#ifndef __LIME_SETTINGS_H__
#define __LIME_SETTINGS_H__

typedef struct
{
	char*			name;
	char*			filepath;
} playlist_t;

typedef struct
{
	char*			wildMidiConfig;
	uint32_t		theme;
	int 			textLang;
	playlist_t*		playlist;
	int			playlistLen;
} settings_t;

#endif
