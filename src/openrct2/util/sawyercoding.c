#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
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

#include "../platform/platform.h"
#include "sawyercoding.h"
#include "../scenario/scenario.h"
#include "util.h"

static size_t decode_chunk_rle(const uint8* src_buffer, uint8* dst_buffer, size_t length);
static size_t decode_chunk_rle_with_size(const uint8* src_buffer, uint8* dst_buffer, size_t length, size_t dstSize);
static size_t decode_chunk_repeat(uint8 *buffer, size_t length);
static void decode_chunk_rotate(uint8 *buffer, size_t length);

static size_t encode_chunk_rle(const uint8 *src_buffer, uint8 *dst_buffer, size_t length);
static size_t encode_chunk_repeat(const uint8 *src_buffer, uint8 *dst_buffer, size_t length);
static void encode_chunk_rotate(uint8 *buffer, size_t length);

bool gUseRLE = true;

uint32 sawyercoding_calculate_checksum(const uint8* buffer, size_t length)
{
    size_t i;
    uint32 checksum = 0;
    for (i = 0; i < length; i++)
        checksum += buffer[i];

    return checksum;
}

size_t sawyercoding_read_chunk_buffer(uint8 *dst_buffer, const uint8 *src_buffer, sawyercoding_chunk_header chunkHeader, size_t dst_buffer_size) {
    switch (chunkHeader.encoding) {
    case CHUNK_ENCODING_NONE:
        assert(chunkHeader.length <= dst_buffer_size);
        memcpy(dst_buffer, src_buffer, chunkHeader.length);
        break;
    case CHUNK_ENCODING_RLE:
        chunkHeader.length = (uint32)decode_chunk_rle_with_size(src_buffer, dst_buffer, chunkHeader.length, dst_buffer_size);
        break;
    case CHUNK_ENCODING_RLECOMPRESSED:
        chunkHeader.length = (uint32)decode_chunk_rle_with_size(src_buffer, dst_buffer, chunkHeader.length, dst_buffer_size);
        chunkHeader.length = (uint32)decode_chunk_repeat(dst_buffer, chunkHeader.length);
        break;
    case CHUNK_ENCODING_ROTATE:
        memcpy(dst_buffer, src_buffer, chunkHeader.length);
        decode_chunk_rotate(dst_buffer, chunkHeader.length);
        break;
    }
    return chunkHeader.length;
}

/**
*
*  rct2: 0x006762E1
*
*/
size_t sawyercoding_write_chunk_buffer(uint8 *dst_file, const uint8* buffer, sawyercoding_chunk_header chunkHeader) {
    uint8 *encode_buffer, *encode_buffer2;

    if (gUseRLE == false) {
        if (chunkHeader.encoding == CHUNK_ENCODING_RLE || chunkHeader.encoding == CHUNK_ENCODING_RLECOMPRESSED) {
            chunkHeader.encoding = CHUNK_ENCODING_NONE;
        }
    }
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
        chunkHeader.length = (uint32)encode_chunk_rle(buffer, encode_buffer, chunkHeader.length);
        memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
        dst_file += sizeof(sawyercoding_chunk_header);
        memcpy(dst_file, encode_buffer, chunkHeader.length);

        free(encode_buffer);
        break;
    case CHUNK_ENCODING_RLECOMPRESSED:
        encode_buffer = malloc(chunkHeader.length * 2);
        encode_buffer2 = malloc(0x600000);
        chunkHeader.length = (uint32)encode_chunk_repeat(buffer, encode_buffer, chunkHeader.length);
        chunkHeader.length = (uint32)encode_chunk_rle(encode_buffer, encode_buffer2, chunkHeader.length);
        memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
        dst_file += sizeof(sawyercoding_chunk_header);
        memcpy(dst_file, encode_buffer2, chunkHeader.length);

        free(encode_buffer2);
        free(encode_buffer);
        break;
    case CHUNK_ENCODING_ROTATE:
        encode_buffer = malloc(chunkHeader.length);
        memcpy(encode_buffer, buffer, chunkHeader.length);
        encode_chunk_rotate(encode_buffer, chunkHeader.length);
        memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
        dst_file += sizeof(sawyercoding_chunk_header);
        memcpy(dst_file, encode_buffer, chunkHeader.length);

        free(encode_buffer);
        break;
    }

    return chunkHeader.length + sizeof(sawyercoding_chunk_header);
}

size_t sawyercoding_decode_sv4(const uint8 *src, uint8 *dst, size_t length, size_t bufferLength)
{
    // (0 to length - 4): RLE chunk
    // (length - 4 to length): checksum
    return decode_chunk_rle_with_size(src, dst, length - 4, bufferLength);
}

size_t sawyercoding_decode_sc4(const uint8 *src, uint8 *dst, size_t length, size_t bufferLength)
{
    // Uncompress
    size_t decodedLength = decode_chunk_rle_with_size(src, dst, length - 4, bufferLength);

    // Decode
    for (size_t i = 0x60018; i <= min(decodedLength - 1, 0x1F8353); i++)
        dst[i] = dst[i] ^ 0x9C;

    for (size_t i = 0x60018; i <= min(decodedLength - 1, 0x1F8350); i += 4) {
        dst[i + 1] = ror8(dst[i + 1], 3);

        uint32 *code = (uint32*)&dst[i];
        *code = rol32(*code, 9);
    }

    return decodedLength;
}

size_t sawyercoding_encode_sv4(const uint8 *src, uint8 *dst, size_t length)
{
    size_t encodedLength;
    uint32 checksum;

    // Encode
    encodedLength = encode_chunk_rle(src, dst, length);

    // Append checksum
    checksum = sawyercoding_calculate_checksum(dst, encodedLength);
    *((uint32*)&dst[encodedLength]) = checksum;

    return encodedLength + 4;
}

size_t sawyercoding_decode_td6(const uint8 *src, uint8 *dst, size_t length)
{
    return decode_chunk_rle(src, dst, length - 4);
}

size_t sawyercoding_encode_td6(const uint8* src, uint8* dst, size_t length){
    size_t output_length = encode_chunk_rle(src, dst, length);

    uint32 checksum = 0;
    for (size_t i = 0; i < output_length; i++){
        uint8 new_byte = ((checksum & 0xFF) + dst[i]) & 0xFF;
        checksum = (checksum & 0xFFFFFF00) + new_byte;
        checksum = rol32(checksum, 3);
    }
    checksum -= 0x1D4C1;

    *((uint32*)&dst[output_length]) = checksum;
    output_length += 4;
    return output_length;
}

/* Based off of rct2: 0x006770C1 */
sint32 sawyercoding_validate_track_checksum(const uint8* src, size_t length){
    uint32 file_checksum = *((uint32*)&src[length - 4]);

    uint32 checksum = 0;
    for (size_t i = 0; i < length - 4; i++){
        uint8 new_byte = ((checksum & 0xFF) + src[i]) & 0xFF;
        checksum = (checksum & 0xFFFFFF00) + new_byte;
        checksum = rol32(checksum, 3);
    }

    if (checksum - 0x1D4C1 == file_checksum)
        return 1;   // .TD6
    else if (checksum - 0x1A67C == file_checksum)
        return 1;   // .TD4
    else if (checksum - 0x1A650 == file_checksum)
        return 1;   // .TD4
    else
        return 0;
}

#pragma region Decoding

/**
 *
 *  rct2: 0x0067693A
 */
static size_t decode_chunk_rle(const uint8* src_buffer, uint8* dst_buffer, size_t length)
{
    size_t count;
    uint8 *dst, rleCodeByte;

    dst = dst_buffer;

    for (size_t i = 0; i < length; i++) {
        rleCodeByte = src_buffer[i];
        if (rleCodeByte & 128) {
            i++;
            count = 257 - rleCodeByte;
            memset(dst, src_buffer[i], count);
            dst = (uint8*)((uintptr_t)dst + count);
        } else {
            memcpy(dst, src_buffer + i + 1, rleCodeByte + 1);
            dst = (uint8*)((uintptr_t)dst + rleCodeByte + 1);
            i += rleCodeByte + 1;
        }
    }

    // Return final size
    return dst - dst_buffer;
}

/**
 *
 *  rct2: 0x0067693A
 */
static size_t decode_chunk_rle_with_size(const uint8* src_buffer, uint8* dst_buffer, size_t length, size_t dstSize)
{
    size_t count;
    uint8 *dst, rleCodeByte;

    dst = dst_buffer;

    for (size_t i = 0; i < length; i++) {
        rleCodeByte = src_buffer[i];
        if (rleCodeByte & 128) {
            i++;
            count = 257 - rleCodeByte;
            assert(dst + count <= dst_buffer + dstSize);
            memset(dst, src_buffer[i], count);
            dst = (uint8*)((uintptr_t)dst + count);
        } else {
            assert(dst + rleCodeByte + 1 <= dst_buffer + dstSize);
            memcpy(dst, src_buffer + i + 1, rleCodeByte + 1);
            dst = (uint8*)((uintptr_t)dst + rleCodeByte + 1);
            i += rleCodeByte + 1;
        }
    }

    // Return final size
    return dst - dst_buffer;
}

/**
 *
 *  rct2: 0x006769F1
 */
static size_t decode_chunk_repeat(uint8 *buffer, size_t length)
{
    size_t i, count;
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
            copyOffset = dst + (sint32)(src[i] >> 3) - 32;
            memcpy(dst, copyOffset, count);
            dst = (uint8*)((uintptr_t)dst + count);
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
static void decode_chunk_rotate(uint8 *buffer, size_t length)
{
    size_t i;
    uint8 code = 1;
    for (i = 0; i < length; i++) {
        buffer[i] = ror8(buffer[i], code);
        code = (code + 2) % 8;
    }
}

#pragma endregion

#pragma region Encoding

/**
 * Ensure dst_buffer is bigger than src_buffer then resize afterwards
 * returns length of dst_buffer
 */
static size_t encode_chunk_rle(const uint8 *src_buffer, uint8 *dst_buffer, size_t length)
{
    const uint8* src = src_buffer;
    uint8* dst = dst_buffer;
    const uint8* end_src = src + length;
    uint8 count = 0;
    const uint8* src_norm_start = src;

    while (src < end_src - 1){

        if ((count && *src == src[1]) || count > 125){
            *dst++ = count - 1;
            memcpy(dst, src_norm_start, count);
            dst += count;
            src_norm_start += count;
            count = 0;
        }
        if (*src == src[1]){
            for (; (count < 125) && ((src + count) < end_src); count++){
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
        memcpy(dst, src_norm_start, count);
        dst += count;
        src_norm_start += count;
        count = 0;
    }
    return dst - dst_buffer;
}

static size_t encode_chunk_repeat(const uint8 *src_buffer, uint8 *dst_buffer, size_t length)
{
    if (length == 0)
        return 0;

    size_t outLength = 0;

    // Need to emit at least one byte, otherwise there is nothing to repeat
    *dst_buffer++ = 255;
    *dst_buffer++ = src_buffer[0];
    outLength += 2;

    // Iterate through remainder of the source buffer
    for (size_t i = 1; i < length; ) {
        size_t searchIndex = (i < 32) ? 0 : (i - 32);
        size_t searchEnd = i - 1;

        size_t bestRepeatIndex = 0;
        size_t bestRepeatCount = 0;
        for (size_t repeatIndex = searchIndex; repeatIndex <= searchEnd; repeatIndex++) {
            size_t repeatCount = 0;
            size_t maxRepeatCount = min(min(7, searchEnd - repeatIndex), length - i - 1);
            // maxRepeatCount should not exceed length
            assert(repeatIndex + maxRepeatCount < length);
            assert(i + maxRepeatCount < length);
            for (size_t j = 0; j <= maxRepeatCount; j++) {
                if (src_buffer[repeatIndex + j] == src_buffer[i + j]) {
                    repeatCount++;
                } else {
                    break;
                }
            }
            if (repeatCount > bestRepeatCount) {
                bestRepeatIndex = repeatIndex;
                bestRepeatCount = repeatCount;

                // Maximum repeat count is 8
                if (repeatCount == 8)
                    break;
            }
        }

        if (bestRepeatCount == 0) {
            *dst_buffer++ = 255;
            *dst_buffer++ = src_buffer[i];
            outLength += 2;
            i++;
        } else {
            *dst_buffer++ = (uint8)((bestRepeatCount - 1) | ((32 - (i - bestRepeatIndex)) << 3));
            outLength++;
            i += bestRepeatCount;
        }
    }

    return outLength;
}

static void encode_chunk_rotate(uint8 *buffer, size_t length)
{
    size_t i;
    uint8 code = 1;
    for (i = 0; i < length; i++) {
        buffer[i] = rol8(buffer[i], code);
        code = (code + 2) % 8;
    }
}

#pragma endregion

sint32 sawyercoding_detect_file_type(const uint8 *src, size_t length)
{
    size_t i;

    // Currently can't detect TD4, as the checksum is the same as SC4 (need alternative method)

    uint32 checksum = *((uint32*)&src[length - 4]);
    uint32 actualChecksum = 0;
    for (i = 0; i < length - 4; i++) {
        actualChecksum = (actualChecksum & 0xFFFFFF00) | (((actualChecksum & 0xFF) + (uint8)src[i]) & 0xFF);
        actualChecksum = rol32(actualChecksum, 3);
    }

    return sawyercoding_detect_rct1_version(checksum - actualChecksum);
}

sint32 sawyercoding_detect_rct1_version(sint32 gameVersion)
{
    sint32 fileType = (gameVersion) > 0 ? FILE_TYPE_SV4 : FILE_TYPE_SC4;
    gameVersion = abs(gameVersion);

    if (gameVersion >= 108000 && gameVersion < 110000)
        return (FILE_VERSION_RCT1 | fileType);
    else if (gameVersion >= 110000 && gameVersion < 120000)
        return (FILE_VERSION_RCT1_AA | fileType);
    else if (gameVersion >= 120000 && gameVersion < 130000)
        return (FILE_VERSION_RCT1_LL | fileType);
    // RCTOA Acres sets this, and possibly some other user-created scenarios as well
    else if (gameVersion == 0)
        return (FILE_VERSION_RCT1_LL | fileType);

    return -1;
}
