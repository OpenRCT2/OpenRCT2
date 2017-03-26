#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#ifndef _SAWYERCODING_H_
#define _SAWYERCODING_H_

#include "../common.h"

#pragma pack(push, 1)
typedef struct sawyercoding_chunk_header {
	uint8 encoding;
	uint32 length;
} sawyercoding_chunk_header;
assert_struct_size(sawyercoding_chunk_header, 5);
#pragma pack(pop)

extern bool gUseRLE;

enum {
	CHUNK_ENCODING_NONE,
	CHUNK_ENCODING_RLE,
	CHUNK_ENCODING_RLECOMPRESSED,
	CHUNK_ENCODING_ROTATE
};

enum {
	FILE_VERSION_MASK = (3 << 0),
	FILE_VERSION_RCT1 = (0 << 0),
	FILE_VERSION_RCT1_AA = (1 << 0),
	FILE_VERSION_RCT1_LL = (2 << 0),

	FILE_TYPE_MASK = (3 << 2),
	FILE_TYPE_TD4 = (0 << 2),
	FILE_TYPE_SV4 = (1 << 2),
	FILE_TYPE_SC4 = (2 << 2)
};

uint32 sawyercoding_calculate_checksum(const uint8* buffer, size_t length);
size_t sawyercoding_read_chunk_buffer(uint8 *dst_buffer, const uint8 *src_buffer, sawyercoding_chunk_header chunkHeader, size_t dst_buffer_size);
size_t sawyercoding_write_chunk_buffer(uint8 *dst_file, const uint8 *src_buffer, sawyercoding_chunk_header chunkHeader);
size_t sawyercoding_decode_sv4(const uint8 *src, uint8 *dst, size_t length, size_t bufferLength);
size_t sawyercoding_decode_sc4(const uint8 *src, uint8 *dst, size_t length, size_t bufferLength);
size_t sawyercoding_encode_sv4(const uint8 *src, uint8 *dst, size_t length);
size_t sawyercoding_decode_td6(const uint8 *src, uint8 *dst, size_t length);
size_t sawyercoding_encode_td6(const uint8 *src, uint8 *dst, size_t length);
sint32 sawyercoding_validate_track_checksum(const uint8* src, size_t length);

sint32 sawyercoding_detect_file_type(const uint8 *src, size_t length);
sint32 sawyercoding_detect_rct1_version(sint32 gameVersion);

#endif
