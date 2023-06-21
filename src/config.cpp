/*   LimePlayer3DS FOSS graphcal music player for the Nintendo 3DS.
*    Copyright (C) 2018-2019  LimePlayer Team
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <string.h>
#include <string>

#include <jansson.h>

#include "config.hpp"
#include "i18n/i18n.hpp"

#define CONFIG_STRING "settings"
#define ENTRIES_STRING "playlists"
#define RADIO_STRING "station"

#define PLAYLIST_NAME "name"
#define PLAYLIST_FILE "file"

#define SETTING_MIDI "WildMidiCFG"
#define SETTING_THEME "theme"
#define SETTING_LANGUAGE "language"


void parse_entries(json_t* entries_elem, settings_t* todo_config) {
	if (json_is_array(entries_elem)) {
		size_t entries_number = json_array_size(entries_elem);

		const char *key;
		json_t *value;

		for (uint16_t i = 0; i < entries_number; i++) {
			if (json_is_object(json_array_get(entries_elem, i))) {
				playlist_t tmp_playlist;
				json_object_foreach(json_array_get(entries_elem, i), key, value) {
					if(strcmp(key, PLAYLIST_NAME)==0)
					{
						if (json_is_string(value))
							tmp_playlist.name.assign(json_string_value(value));
					}
					else if(strcmp(key, PLAYLIST_FILE)==0)
					{
						if (json_is_array(value)) {
							size_t index;
							json_t *value_arr;
							json_array_foreach(value, index, value_arr) {
								if (json_is_string(value_arr)) {
									std::string tmp_filepath(json_string_value(value_arr));
									tmp_playlist.filepath.push_back(tmp_filepath);
								}
							}
						} else if (json_is_string(value)) {
							std::string tmp_filepath(json_string_value(value));
							tmp_playlist.filepath.push_back(tmp_filepath);
						}
					}
				}
				todo_config->playlist.push_back(tmp_playlist);
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

void parse_options(json_t* entries_elem, settings_t* todo_config) {
	const char *key;
	json_t *value;
	todo_config->language = 0;

	void *iter = json_object_iter(entries_elem);
	while(iter)
	{
		key = json_object_iter_key(iter);
		value = json_object_iter_value(iter);
		if(strcmp(key, SETTING_MIDI)==0)
		{
			if(json_is_string(value)) {
				todo_config->wildMidiConfig.assign(json_string_value(value));
			}
		}
		else if(strcmp(key, SETTING_THEME)==0)
		{
			todo_config->theme = json_integer_value(value);
		}
		else if(strcmp(key, SETTING_LANGUAGE)==0)
		{
			if(json_is_string(value)) {
				todo_config->language = i18n::Code2Int(json_string_value(value));
			} else {
				todo_config->language = json_integer_value(value);
			}
		}
		iter = json_object_iter_next(entries_elem, iter);
	}
}

void parse_station(json_t* entries_elem, std::string* url) {
	const char *key;
	json_t *value;

	void *iter = json_object_iter(entries_elem);
	while(iter)
	{
		key = json_object_iter_key(iter);
		value = json_object_iter_value(iter);
		if(strcmp(key, "url")==0)
		{
			url->assign(json_string_value(value));
		}
		iter = json_object_iter_next(entries_elem, iter);
	}
}

int Cfg::ParseSettings(const char* filepath, settings_t* settings) {
	json_error_t *pjsonError = NULL;
	json_t *pJson = json_load_file(filepath, 0, pjsonError);

	if (!pJson)
		return 1;

	const char *key;
	json_t *value;

	if (json_is_object(pJson)) {
		json_object_foreach(pJson, key, value) {
			if (strcmp(key, ENTRIES_STRING)==0)
			{
				parse_entries(value, settings);
			}
			if (strcmp(key, CONFIG_STRING)==0)
			{
				parse_options(value, settings);
			}
		}
	}
	else
	{
		json_decref(pJson);
		return 1;
	}
	json_decref(pJson);

	return 0;
}

int Cfg::ParseNC(const char* filepath, std::string* url) {
	json_error_t *pjsonError = NULL;
	json_t *pJson = json_load_file(filepath, 0, pjsonError);

	if (!pJson)
		return 1;

	const char *key;
	json_t *value;

	if (json_is_object(pJson)) {
		json_object_foreach(pJson, key, value) {
			if (strcmp(key, RADIO_STRING)==0)
			{
				parse_station(value, url);
			}
		}
	}
	else
	{
		json_decref(pJson);
		return 1;
	}
	json_decref(pJson);

	return 0;
}

int Cfg::WriteJson(const char* outpath, settings_t* settings) {
	json_t *root = json_object();
	json_t *set_obj = json_object();
	json_t *pls_json_arr = json_array();

	json_object_set_new(root, "Note", json_string("You can find documentation on LimePlayer3DS's config.json at https://github.com/Oreo639/LimePlayer3DS/wiki/config.json"));

	json_object_set_new(set_obj, SETTING_THEME, json_integer(settings->theme));
	json_object_set_new(set_obj, SETTING_MIDI, json_string(settings->wildMidiConfig.c_str()));
	json_object_set_new(set_obj, SETTING_LANGUAGE, json_string(i18n::Int2Code(settings->language).c_str()));
	json_object_set_new(root, CONFIG_STRING, set_obj);

	for (uint32_t i = 0; settings->playlist.size() > i; i++) {
		json_t *pls_obj = json_object();
		json_object_set_new(pls_obj, PLAYLIST_NAME, json_string(settings->playlist[i].name.c_str()));

		if (settings->playlist[i].filepath.size() > 1) {
			json_t *pls_obj_arr = json_array();

			for (uint32_t j = 0; settings->playlist[i].filepath.size() > j; j++)
				json_array_append_new(pls_obj_arr, json_string(settings->playlist[i].filepath[j].c_str()));

			json_object_set_new(pls_obj, PLAYLIST_FILE, pls_obj_arr);

		} else if (settings->playlist[i].filepath.size() == 1)
			json_object_set_new(pls_obj, PLAYLIST_FILE, json_string(settings->playlist[i].filepath[0].c_str()));

		json_array_append_new(pls_json_arr, pls_obj);
	}
	json_object_set_new(root, ENTRIES_STRING, pls_json_arr);
	json_dump_file(root, outpath, JSON_INDENT(8));
	json_decref(root);
	return 0;
}


void Cfg::CleanSettings(settings_t* parsed_config)
{
	parsed_config->wildMidiConfig.clear();
	parsed_config->playlist.clear();
}
