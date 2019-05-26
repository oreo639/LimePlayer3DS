#ifndef __LIME_CFG_PARSER__
#define __LIME_CFG_PARSER__

#include "settings.hpp"

#define ERROR_JSON 1
#define ERROR_FILE 2

#define UPDATE_ERROR 1
#define UPDATE_DONE 2
#define STATE_MARKED 4

int CFG_parse(const char* filepath, settings_t* parsed_config);

void CFG_clean(settings_t* parsed_config);

#endif
