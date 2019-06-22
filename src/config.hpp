#ifndef __LIME_CFG_PARSER__
#define __LIME_CFG_PARSER__

#include "settings.hpp"

#define ERROR_JSON 1
#define ERROR_FILE 2

#define UPDATE_ERROR 1
#define UPDATE_DONE 2
#define STATE_MARKED 4

namespace Cfg {
	int ParseSettings(const char* filepath, settings_t* parsed_config);

	int ParseNC(const char* filepath, std::string* url);

	void CleanSettings(settings_t* parsed_config);
}

#endif
