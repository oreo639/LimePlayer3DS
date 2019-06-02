#include <string>
#include <cstring>

#include <3ds.h>

#include "content.hpp"
#include "error.hpp"

static const char* mime_mpeg3[] = {
	"audio/mpeg",  "audio/x-mpeg",
	"audio/mp3",   "audio/x-mp3", 
	"audio/mpeg3", "audio/x-mpeg3",
	"audio/mpg",   "audio/x-mpg",
	"audio/x-mpegaudio", NULL
};

static const char* mime_ogg[] = {
	"audio/ogg", "audio/x-ogg",
	"application/ogg", "application/x-ogg",
	"audio/vorbis", "audio/x-vorbis",
	"application/vorbis", "application/x-vorbis",
	NULL
};

static const char* mime_m3u[] = {
	"audio/mpegurl", "audio/mpeg-url",
	"audio/x-mpegurl", NULL
};

static const char* mime_pls[] = {
	"audio/x-scpls", "audio/scpls",
	"application/pls", NULL
};

static const char** mimes[] = { mime_mpeg3, mime_ogg, mime_m3u, mime_pls, NULL };

int getContentType(const char* content_type)
{
	int i,j;
	size_t len;
	int r = 0;
	DEBUG("Content-Type: %s\n", content_type);
	std::string mimestr(content_type);
	/* Watch out for such: "audio/x-mpegurl; charset=utf-8" */
	len = mimestr.find_first_of(';');
	std::string aux = mimestr.substr(0, len);
	if(len < mimestr.size())
	{
		DEBUG("Warning: additional info in content-type ignored (%s)\n", mimestr.substr(len).c_str());
	}
	else 
		len = mimestr.size();

	for(i=0; mimes[i] != NULL; ++i)
		for(j=0; mimes[i][j] != NULL; ++j)
			if(!strncasecmp(mimes[i][j], aux.c_str(), len))
				goto debunk_result;

debunk_result:
	if(mimes[i] != NULL)
	{
		switch(i)
		{
			case M_MPEG3:
				r = CONTENT_MPEG3;
				break;
			case M_OGG:
				r = CONTENT_OGG;
				break;
			case M_M3U:
				r = LIST_M3U;
				break;
			case M_PLS:
				r = LIST_PLS;
				break;
			default:
				DEBUG("unexpected MIME debunk result -- Coding error?!\n");
		}
	}
	return r;
}
