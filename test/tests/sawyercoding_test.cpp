extern "C" {
	#include "util/sawyercoding.h"
}

#include <gtest/gtest.h>

// TODO: Seed with some proper randomness
char randomdata[] = { 0x01, 0x02, 0x03, 0x04 };

#define BUFFER_SIZE 0x600000

static void test_encode_decode(uint8_t encoding_type) {
	sawyercoding_chunk_header chdr_in, chdr_out;
	chdr_in.encoding = encoding_type;
	chdr_in.length = sizeof(randomdata);
	uint8_t * encodedDataBuffer = new uint8_t[BUFFER_SIZE];
	size_t encodedDataSize = sawyercoding_write_chunk_buffer(encodedDataBuffer, (const uint8 *)randomdata, chdr_in);
	ASSERT_GT(encodedDataSize, sizeof(sawyercoding_chunk_header));
	memcpy(&chdr_out, encodedDataBuffer, sizeof(sawyercoding_chunk_header));
	ASSERT_EQ(chdr_out.encoding, encoding_type);
	uint8_t * decodeBuffer = new uint8_t[BUFFER_SIZE];
	size_t decodedDataSize = sawyercoding_read_chunk_buffer(decodeBuffer, encodedDataBuffer + sizeof(sawyercoding_chunk_header), chdr_out, BUFFER_SIZE);
	ASSERT_EQ(decodedDataSize, sizeof(randomdata));
	int result = memcmp(decodeBuffer, randomdata, sizeof(randomdata));
	ASSERT_EQ(result, 0);
	delete [] decodeBuffer;
	delete [] encodedDataBuffer;
}

TEST(sawyercoding, write_read_chunk_none) {
	test_encode_decode(CHUNK_ENCODING_NONE);
}

TEST(sawyercoding, write_read_chunk_rle) {
	test_encode_decode(CHUNK_ENCODING_RLE);
}

TEST(sawyercoding, write_read_chunk_rle_compressed) {
	test_encode_decode(CHUNK_ENCODING_RLECOMPRESSED);
}

TEST(sawyercoding, write_read_chunk_rotate) {
	test_encode_decode(CHUNK_ENCODING_ROTATE);
}
