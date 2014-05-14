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

#ifdef _WIN32
#include <windows.h>
#endif

#include <string.h>
#include "addresses.h"
#include "rct2.h"
#include "sawyercoding.h"

static int decode_chunk_rle(char *buffer, int length);
static int decode_chunk_repeat(char *buffer, int length);
static void decode_chunk_rotate(char *buffer, int length);

/**
 * 
 *  rct2: 0x00676FD2
 */
int sawyercoding_validate_checksum(HANDLE hFile)
{
	uint32 i, checksum, fileChecksum;
	DWORD dataSize, bufferSize, numBytesRead;
	uint8 buffer[1024];

	// Get data size
	if ((dataSize = SetFilePointer(hFile, 0, NULL, FILE_END)) < 8)
		return 0;
	dataSize -= 4;
	
	// Calculate checksum
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
	checksum = 0;
	do {
		bufferSize = min(dataSize, 1024);
		ReadFile(hFile, buffer, bufferSize, &numBytesRead, NULL);
		if (numBytesRead != bufferSize)
			return 0;

		for (i = 0; i < bufferSize; i++)
			checksum += buffer[i];
		dataSize -= bufferSize;
	} while (dataSize != 0);

	// Read file checksum
	ReadFile(hFile, &fileChecksum, sizeof(fileChecksum), &numBytesRead, NULL);
	if (numBytesRead != sizeof(fileChecksum))
		return 0;

	// Reset file position
	SetFilePointer(hFile, 0, NULL, FILE_BEGIN);

	// Validate
	return checksum == fileChecksum;
}

/**
 * 
 *  rct2: 0x0067685F
 * buffer (esi)
 */
int sawyercoding_read_chunk(HANDLE hFile, uint8 *buffer)
{
	DWORD numBytesRead;
	sawyercoding_chunk_header chunkHeader;

	// Read chunk header
	ReadFile(hFile, &chunkHeader, sizeof(sawyercoding_chunk_header), &numBytesRead, NULL);

	// Read chunk data
	ReadFile(hFile, buffer, chunkHeader.length, &numBytesRead, NULL);

	// Decode chunk data
	switch (chunkHeader.encoding) {
	case CHUNK_ENCODING_RLE:
		chunkHeader.length = decode_chunk_rle(buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_RLECOMPRESSED:
		chunkHeader.length = decode_chunk_rle(buffer, chunkHeader.length);
		chunkHeader.length = decode_chunk_repeat(buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_ROTATE:
		decode_chunk_rotate(buffer, chunkHeader.length);
		break;
	}

	// Set length
	RCT2_GLOBAL(0x009E3828, uint32) = chunkHeader.length;
	return chunkHeader.length;
}

/**
 * 
 *  rct2: 0x0067693A
 */
static int decode_chunk_rle(char *buffer, int length)
{
	int i, j, count;
	uint8 *src, *dst, rleCodeByte;

	// Backup buffer
	src = malloc(length);
	memcpy(src, buffer, length);
	dst = buffer;

	for (i = 0; i < length; i++) {
		rleCodeByte = src[i];
		if (rleCodeByte & 128) {
			i++;
			count = 257 - rleCodeByte;
			for (j = 0; j < count; j++)
				*dst++ = src[i];
		} else {
			for (j = 0; j <= rleCodeByte; j++)
				*dst++ = src[++i];
		}
	}
	
	// Free backup buffer
	free(src);

	// Return final size
	return dst - buffer;
}

/**
 * 
 *  rct2: 0x006769F1
 */
static int decode_chunk_repeat(char *buffer, int length)
{
	int i, j, count;
	uint8 *src, *dst, *copyOffset;

	// Backup buffer
	src = malloc(length);
	memcpy(src, buffer, length);
	dst = buffer;

	for (i = 0; i < length; i++) {
		if (src[i] == 0xFF) {
			*dst++ = src[++i];
		} else {
			count = (src[i] & 7) + 1;
			copyOffset = dst + (int)(src[i] >> 3) - 32;
			for (j = 0; j < count; j++)
				*dst++ = *copyOffset++;
		}
	}

	// Free backup buffer
	free(src);

	// Return final size
	return dst - buffer;
}

/**
 * 
 *  rct2: 0x006768F4
 */
static void decode_chunk_rotate(char *buffer, int length)
{
	int i, code = 1;
	for (i = 0; i < length; i++) {
		buffer[i] = ror8(buffer[i], code);
		code = (code + 2) % 8;
	}
}
