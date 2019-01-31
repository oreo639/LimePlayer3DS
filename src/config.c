#include <jansson.h>

#include "config.h"
#include "lang.h"

#define CONFIG_STRING "settings"
#define ENTRIES_STRING "playlists"

#define NAME_STRING "name"
#define FILE_STRING "file"

#define SETTING_MIDI "WildMidiCFG"
#define SETTING_THEME "theme"
#define SETTING_LANGUAGE "lang"


void parse_entries(json_t * entries_elem, settings_t* todo_config)
{
	if (json_is_array(entries_elem)) {
		
		size_t entries_number = json_array_size(entries_elem);

		const char *key;
		json_t *value;
		todo_config->playlistLen = entries_number;
		
		todo_config->playlist = malloc(entries_number*sizeof(playlist_t));
		
		for (u16 i = 0; i < entries_number; i++) {
			if (json_is_object(json_array_get(entries_elem, i)))
			{
				json_object_foreach(json_array_get(entries_elem, i), key, value) {
					if(json_is_string(value)) {
						if(!strcmp(key, NAME_STRING))
						{
							todo_config->playlist[i].name = strdup(json_string_value(value));
						}
						else if(!strcmp(key, FILE_STRING))
						{
							todo_config->playlist[i].filepath = strdup(json_string_value(value));
						}
					}
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

void parse_options(json_t * entries_elem, settings_t* todo_config)
{
	const char *key;
	json_t *value;
	todo_config->textLang = CFG_LANGUAGE_DEFAULT;
	
	void *iter = json_object_iter(entries_elem);
		while(iter)
		{
			key = json_object_iter_key(iter);
			value = json_object_iter_value(iter);
				if(!strcmp(key, SETTING_MIDI))
				{
					todo_config->wildMidiConfig = strdup(json_string_value(value));
				}
				else if(!strcmp(key, SETTING_THEME))
				{
					todo_config->theme = json_integer_value(value);
				}
				else if(!strcmp(key, SETTING_LANGUAGE))
				{
					todo_config->textLang = json_integer_value(value);
				}
			//printf("Key: %s, Value: %lld\n", key, json_integer_value(value) );
	
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
				parse_entries(value, settings);
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
	free(parsed_config->wildMidiConfig);
	for (int i = 0; i < parsed_config->playlistLen; i++) {
		free((char*)(parsed_config->playlist[i].name));
		free((char*)(parsed_config->playlist[i].filepath));
	}
}
