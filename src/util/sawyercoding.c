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

#include <windows.h>
#include <string.h>
#include "../addresses.h"
#include "sawyercoding.h"

static int decode_chunk_rle(uint8* src_buffer, uint8* dst_buffer, int length);
static int decode_chunk_repeat(char *buffer, int length);
static void decode_chunk_rotate(char *buffer, int length);

int encode_chunk_rle(char *src_buffer, char *dst_buffer, int length);
void encode_chunk_rotate(char *buffer, int length);

int sawyercoding_calculate_checksum(uint8* buffer, uint32 length){
	int checksum = 0;
	do {
		int bufferSize = min(length , 1024);

		for (int i = 0; i < bufferSize; i++)
			checksum += buffer[i];
		length -= bufferSize;
	} while (length != 0);

	return checksum;
}

/**
 * 
 *  rct2: 0x00676FD2
 */
int sawyercoding_validate_checksum(FILE *file)
{
	uint32 i, checksum, fileChecksum, dataSize, bufferSize;
	uint8 buffer[1024];

	// Get data size
	fseek(file, 0, SEEK_END);
	dataSize = ftell(file);
	if (dataSize < 8)
		return 0;
	dataSize -= 4;
	
	// Calculate checksum
	fseek(file, 0, SEEK_SET);
	checksum = 0;
	do {
		bufferSize = min(dataSize, 1024);
		if (fread(buffer, bufferSize, 1, file) != 1)
			return 0;

		for (i = 0; i < bufferSize; i++)
			checksum += buffer[i];
		dataSize -= bufferSize;
	} while (dataSize != 0);

	// Read file checksum
	if (fread(&fileChecksum, sizeof(fileChecksum), 1, file) != 1)
		return 0;

	// Reset file position
	fseek(file, 0, SEEK_SET);

	// Validate
	return checksum == fileChecksum;
}


/**
 * 
 *  rct2: 0x0067685F
 * buffer (esi)
 */
int sawyercoding_read_chunk(FILE *file, uint8 *buffer)
{
	sawyercoding_chunk_header chunkHeader;

	// Read chunk header
	if (fread(&chunkHeader, sizeof(sawyercoding_chunk_header), 1, file) != 1) {
		RCT2_ERROR("Unable to read chunk header!");
		return -1;
	}

	uint8* src_buffer = malloc(chunkHeader.length);

	// Read chunk data
	if (fread(src_buffer, chunkHeader.length, 1, file) != 1) {
		free(src_buffer);
		RCT2_ERROR("Unable to read chunk data!");
		return -1;
	}

	// Decode chunk data
	switch (chunkHeader.encoding) {
	case CHUNK_ENCODING_NONE:
		memcpy(buffer, src_buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_RLE:
		chunkHeader.length = decode_chunk_rle(src_buffer, buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_RLECOMPRESSED:
		chunkHeader.length = decode_chunk_rle(src_buffer, buffer, chunkHeader.length);
		chunkHeader.length = decode_chunk_repeat(buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_ROTATE:
		memcpy(buffer, src_buffer, chunkHeader.length);
		decode_chunk_rotate(buffer, chunkHeader.length);
		break;
	}
	free(src_buffer);
	// Set length
	RCT2_GLOBAL(0x009E3828, uint32) = chunkHeader.length;
	return chunkHeader.length;
}

/**
 * 
 *  rct2: 0x0067693A
 */
static int decode_chunk_rle(uint8* src_buffer, uint8* dst_buffer, int length)
{
	int i, j, count;
	uint8 *dst, rleCodeByte;

	dst = dst_buffer;

	for (i = 0; i < length; i++) {
		rleCodeByte = src_buffer[i];
		if (rleCodeByte & 128) {
			i++;
			count = 257 - rleCodeByte;
			for (j = 0; j < count; j++)
				*dst++ = src_buffer[i];
		} else {
			for (j = 0; j <= rleCodeByte; j++)
				*dst++ = src_buffer[++i];
		}
	}
	
	// Return final size
	return dst - dst_buffer;
}

/**
 * 
 *  rct2: 0x006769F1
 */
static int decode_chunk_repeat(char *buffer, int length)
{
	int i, j, count;
	unsigned char *src, *dst, *copyOffset;

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
	return (char*)dst - buffer;
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

/**
*
*  rct2: 0x006762E1
* 
*/
int sawyercoding_write_chunk_buffer(uint8 *dst_file, uint8* buffer, sawyercoding_chunk_header chunkHeader){
	uint8* encode_buffer;

	switch (chunkHeader.encoding){
	case CHUNK_ENCODING_NONE:
		memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
		dst_file += sizeof(sawyercoding_chunk_header);
		memcpy(dst_file, buffer, chunkHeader.length);
		//fwrite(&chunkHeader, sizeof(sawyercoding_chunk_header), 1, file);
		//fwrite(buffer, 1, chunkHeader.length, file);
		break;
	case CHUNK_ENCODING_RLE:
		encode_buffer = malloc(0x600000);
		chunkHeader.length = encode_chunk_rle(buffer, encode_buffer, chunkHeader.length);
		memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
		dst_file += sizeof(sawyercoding_chunk_header);
		memcpy(dst_file, encode_buffer, chunkHeader.length);

		free(encode_buffer);
		break;
	case CHUNK_ENCODING_RLECOMPRESSED:
		RCT2_ERROR("This has not been implemented");
		return -1;
		//chunkHeader.length = decode_chunk_rle(src_buffer, buffer, chunkHeader.length);
		//chunkHeader.length = decode_chunk_repeat(buffer, chunkHeader.length);
		break;
	case CHUNK_ENCODING_ROTATE:
		encode_chunk_rotate(buffer, chunkHeader.length);
		memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
		dst_file += sizeof(sawyercoding_chunk_header);
		memcpy(dst_file, buffer, chunkHeader.length);
		break;
	}

	return chunkHeader.length + sizeof(sawyercoding_chunk_header);
}

/**
* Ensure dst_buffer is bigger than src_buffer then resize afterwards
* returns length of dst_buffer
*/
int encode_chunk_rle(char *src_buffer, char *dst_buffer, int length)
{
	char* src = src_buffer;
	char* dst = dst_buffer;
	char* end_src = src + length;
	uint8 count = 0;
	char* src_norm_start = src;

	while (src < end_src - 1){

		if ((count && *src == src[1]) || count > 120){
			*dst++ = count - 1;
			for (; count != 0; --count){
				*dst++ = *src_norm_start++;
			}
		}
		if (*src == src[1]){
			for (; (count < 120) && ((src + count) < end_src); count++){
				if (*src != src[count]) break;
			}
			*dst++ = 257 - count;
			*dst++ = *src;
			src += count;
			src_norm_start = src;
			count = 0;
		}
		else{
			count++;
			src++;
		}
	}
	if (src == end_src - 1)count++;
	if (count){
		*dst++ = count - 1;
		for (; count != 0; --count){
			*dst++ = *src_norm_start++;
		}
	}
	return dst - dst_buffer;
}

/**
*
*
*/
void encode_chunk_rotate(char *buffer, int length)
{
	int i, code = 1;
	for (i = 0; i < length; i++) {
		buffer[i] = rol8(buffer[i], code);
		code = (code + 2) % 8;
	}
}

int sawyercoding_decode_sv4(char *src, char *dst, int length)
{
	// (0 to length - 4): RLE chunk
	// (length - 4 to length): checksum
	return decode_chunk_rle(src, dst, length - 4);
}

int sawyercoding_decode_sc4(char *src, char *dst, int length)
{
	int decodedLength, i;
	uint32 *code;

	// Uncompress
	decodedLength = decode_chunk_rle(src, dst, length - 4);

	// Decode
	for (i = 0x60018; i <= min(decodedLength - 1, 0x1F8353); i++)
		dst[i] = dst[i] ^ 0x9C;

	for (i = 0x60018; i <= min(decodedLength - 1, 0x1F8350); i += 4) {
		dst[i + 1] = ror8(dst[i + 1], 3);

		code = (uint32*)&dst[i];
		*code = rol32(*code, 9);
	}

	return decodedLength;
}

int sawyercoding_encode_sv4(char *src, char *dst, int length)
{
	int encodedLength, checksum;

	// Encode
	encodedLength = encode_chunk_rle(src, dst, length);

	// Append checksum
	checksum = sawyercoding_calculate_checksum(dst, encodedLength);
	*((uint32*)&dst[encodedLength]) = checksum;

	return encodedLength + 4;
}