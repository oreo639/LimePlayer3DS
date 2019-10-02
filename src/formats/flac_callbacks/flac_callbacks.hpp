#ifndef __FLAC_CALLBACKS_H__
#define __FLAC_CALLBACKS_H__

#define BUFFER_SIZE 65536

#define BUFTYPE_BITS 16

typedef struct
{
	unsigned bits_per_sample = 0;
	unsigned sample_rate = 0;
	unsigned channels = 0;
	unsigned long total_samples = 0;

	int32_t output_buffer[BUFFER_SIZE];
	int32_t *write_pointer = nullptr;
	unsigned samples_used = 0;

	void clear()
	{
		samples_used = 0;
		memset(output_buffer, 0, sizeof(output_buffer));
		write_pointer = &output_buffer[0];
	}
} callback_info;

int FLAC_decode(FLAC__StreamDecoder* decoder, callback_info* cinfo, int16_t *buffer, int numSamples);


// Callbacks
FLAC__StreamDecoderWriteStatus write_callback(const FLAC__StreamDecoder *decoder, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data);

void error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data);

void metadata_callback(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data);

#endif
