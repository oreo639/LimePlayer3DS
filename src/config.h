#ifndef __LIME_CFG_PARSER__
#define __LIME_CFG_PARSER__

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <3ds.h>

#include "settings.h"

#define ERROR_JSON 1
#define ERROR_FILE 2

#define UPDATE_ERROR 1
#define UPDATE_DONE 2
#define STATE_MARKED 4

/*struct entry_t {
	const char * name;
	const char * file;
};

struct settings_t {
	int theme;
	char wm_cfg[200];
};

struct config_t {
	u8 errorState;
	int entries_number;
	struct settings_t options;
	struct entry_t entries[100];
};*/

int CFG_parse(const char* filepath, settings_t* parsed_config);

void CFG_clean(settings_t* parsed_config);

#endif
