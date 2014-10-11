/*****************************************************************************
 * Copyright (c) 2014 Ted John
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 * 
 * This file is part of OpenRCT2.
 * 
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.

 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *****************************************************************************/
 
#ifndef _SAWYERCODING_H_
#define _SAWYERCODING_H_

#include <stdio.h>
#include "../common.h"

typedef struct {
	uint8 encoding;
	uint32 length;
} sawyercoding_chunk_header;

enum {
	CHUNK_ENCODING_NONE,
	CHUNK_ENCODING_RLE,
	CHUNK_ENCODING_RLECOMPRESSED,
	CHUNK_ENCODING_ROTATE
};

int sawyercoding_validate_checksum(FILE *file);
int sawyercoding_calculate_checksum(uint8* buffer, uint32 length);
int sawyercoding_read_chunk(FILE *file, uint8 *buffer);
int sawyercoding_write_chunk_buffer(uint8 *dst_file, uint8* buffer, sawyercoding_chunk_header chunkHeader);
int sawyercoding_decode_sv4(char *src, char *dst, int length);
int sawyercoding_decode_sc4(char *src, char *dst, int length);
int sawyercoding_encode_sv4(char *src, char *dst, int length);
int sawyercoding_decode_td6(char *src, char *dst, int length);

#endif
