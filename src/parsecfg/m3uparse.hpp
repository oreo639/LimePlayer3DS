#ifndef __LIME_M3UPARSE_H__
#define __LIME_M3UPARSE_H__
#include "settings.hpp"

namespace M3u {
	int Parse(const std::string& file, playlist_t* playlist);
}
#endif
