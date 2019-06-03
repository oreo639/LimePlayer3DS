#include <string>

#include "INIReader/cpp/INIReader.h"

#include "plsparse.hpp"
#include "settings.hpp"

int Pls::Parse(const std::string& file, playlist_t* playlist) {
	// Pls is a variant of an INI file, so im just going to use
	// a premade ini parser
	int entries = 0;
	INIReader plsFile(file);

	playlist->filepath.clear();

	if (plsFile.ParseError() != 0) {
		return 1;
	}

	if (plsFile.HasSection("playlist")) {
		if (!(entries = plsFile.GetInteger("playlist", "NumberOfEntries", 0)))
			return 3;

		for (int i = 1; i <= entries; i++) {
			std::string tmpfp;
			tmpfp = plsFile.Get("playlist", "File"+std::to_string(i), "");
			if (tmpfp.size()) {
				playlist->filepath.push_back(tmpfp);
			}
		}
		return 0;
	}

	return 2;
}
