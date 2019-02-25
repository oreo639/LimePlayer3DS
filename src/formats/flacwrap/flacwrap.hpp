enum {
	// Maximal buffer size. According to the FLAC format specification, the  block size is
	// a 16 bit value (in fact it seems the maximal block size is 32768, but we play it safe).
	BUFFER_SIZE = 65536
};

enum { BUFTYPE_BITS = 16 };

typedef struct {
	FLAC__StreamDecoder *dFlac;
	FLAC__StreamMetadata_StreamInfo mFlac;
	struct {
		int16_t bufData[BUFFER_SIZE];
		int16_t *bufReadPos;
		uint bufFill;
	} sampleCache;
	FLAC__uint64 lastSample;
	int16_t *outBuffer;
	uint requestedSamples;
	bool lastSampleWritten;
} flacwrap_t;

int FLACWRAP_init(const char* filename, flacwrap_t* flacstruct);
void FLACWRAP_free(flacwrap_t* flacstruct);
int FLACWRAP_decode(flacwrap_t* flacstruct, int16_t *buffer, const int numSamples);
