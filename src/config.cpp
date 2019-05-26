#include <string.h>
#include <string>

#include <jansson.h>

#include "config.hpp"

#define CONFIG_STRING "settings"
#define ENTRIES_STRING "playlists"

#define NAME_STRING "name"
#define FILE_STRING "file"

#define SETTING_MIDI "WildMidiCFG"
#define SETTING_THEME "theme"
#define SETTING_LANGUAGE "lang"


void parse_entries(json_t* entries_elem, settings_t* todo_config)
{
	if (json_is_array(entries_elem)) {
		size_t entries_number = json_array_size(entries_elem);

		const char *key;
		json_t *value;

		for (uint16_t i = 0; i < entries_number; i++) {
			if (json_is_object(json_array_get(entries_elem, i)))
			{
				json_object_foreach(json_array_get(entries_elem, i), key, value) {
					playlist_t tmp_playlist;
					if(json_is_string(value)) {
						if(!strcmp(key, NAME_STRING))
						{
							tmp_playlist.name.assign(json_string_value(value));
						}
						else if(!strcmp(key, FILE_STRING))
						{
							tmp_playlist.filepath.assign(json_string_value(value));
						}
					}
					todo_config->playlist.push_back(tmp_playlist);
				}
			}
			else
			{
				//todo_config->errorState = ERROR_JSON;
			}
		}
	}
	else
	{
		//todo_config->errorState = ERROR_JSON;
	}
}

void parse_options(json_t* entries_elem, settings_t* todo_config)
{
	const char *key;
	json_t *value;
	todo_config->textLang = 0;

	void *iter = json_object_iter(entries_elem);
	while(iter)
	{
		key = json_object_iter_key(iter);
		value = json_object_iter_value(iter);
		if(!strcmp(key, SETTING_MIDI))
		{
			todo_config->wildMidiConfig.assign(json_string_value(value));
		}
		else if(!strcmp(key, SETTING_THEME))
		{
			todo_config->theme = json_integer_value(value);
		}
		else if(!strcmp(key, SETTING_LANGUAGE))
		{
			todo_config->textLang = json_integer_value(value);
		}
		iter = json_object_iter_next(entries_elem, iter);
		free(value);
	}
	free(iter);
}

int CFG_parse(const char* filepath, settings_t* settings) {
	json_error_t *pjsonError = NULL;
	json_t *pJson = json_load_file(filepath, 0, pjsonError);

	if (pJson == 0) {
		return 1;
	}

	const char *key;
	json_t *value;

	if (json_is_object(pJson)) {
		json_object_foreach(pJson, key, value) {
			if (!strcmp(key, ENTRIES_STRING))
			{
				//parse_entries(value, settings);
			}
			if (!strcmp(key, CONFIG_STRING))
			{
				parse_options(value, settings);
			}
		}
	}
	else
	{
		return 1;
	}

	return 0;
}


void CFG_clean(settings_t* parsed_config)
{
	parsed_config->wildMidiConfig.clear();
	parsed_config->playlist.clear();
}
