#include <stdio.h>
#include <string.h>

#include <string>

#include "m3uparse.hpp"

int M3u::Parse(const std::string& file, playlist_t* playlist) {
	FILE *fp = fopen(file.c_str(), "r");

	if (!fp) {
		return 1;
	}

	char line[1000];
	while (fgets(line, 1000, fp) != NULL) {
		line[strcspn(line, "\n")] = 0;
		line[strcspn(line, "\r\n")] = 0;
		if (line[0] == '\0')
			// Empty lines can be ignored.
			std::printf("Empty.\n");
		else if (line[0] =='#')
			// No handling for comments rn.
			std::printf("Ignored.\n");
		else {
			std::string tmpfp(line);
			playlist->filepath.push_back(tmpfp);
		}
		
	}

	return 0;
}
