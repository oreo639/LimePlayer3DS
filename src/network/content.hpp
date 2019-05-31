#ifndef __LIME_NETCONTENT_H__
#define __LIME_NETCONTENT_H__

enum {
	M_MPEG3 = 0,
	M_OGG,
	M_VORBIS,
	M_M3U,
	M_PLS
};

enum {
	CONTENT_UNKNOWN = 0,
	CONTENT_MPEG3,
	CONTENT_OGG,
	CONTENT_VORBIS,
	CONTENT_OPUS,
	LIST_M3U,
	LIST_PLS
};

int debunk_mime(const char* content_type);

#endif
