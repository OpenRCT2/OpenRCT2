/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerCoding.h"

#include "../core/Numerics.hpp"
#include "../platform/Platform.h"
#include "../scenario/Scenario.h"
#include "Util.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

static size_t decode_chunk_rle(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length);
static size_t decode_chunk_rle_with_size(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length, size_t dstSize);

static size_t encode_chunk_rle(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length);
static size_t encode_chunk_repeat(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length);
static void encode_chunk_rotate(uint8_t* buffer, size_t length);

uint32_t sawyercoding_calculate_checksum(const uint8_t* buffer, size_t length)
{
    uint32_t checksum = 0;
    for (size_t i = 0; i < length; i++)
        checksum += buffer[i];

    return checksum;
}

/**
 *
 *  rct2: 0x006762E1
 *
 */
size_t sawyercoding_write_chunk_buffer(uint8_t* dst_file, const uint8_t* buffer, sawyercoding_chunk_header chunkHeader)
{
    uint8_t *encode_buffer, *encode_buffer2;

    switch (chunkHeader.encoding)
    {
        case CHUNK_ENCODING_NONE:
            std::memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
            dst_file += sizeof(sawyercoding_chunk_header);
            std::memcpy(dst_file, buffer, chunkHeader.length);
            // fwrite(&chunkHeader, sizeof(sawyercoding_chunk_header), 1, file);
            // fwrite(buffer, 1, chunkHeader.length, file);
            break;
        case CHUNK_ENCODING_RLE:
            encode_buffer = static_cast<uint8_t*>(malloc(0x600000));
            chunkHeader.length = static_cast<uint32_t>(encode_chunk_rle(buffer, encode_buffer, chunkHeader.length));
            std::memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
            dst_file += sizeof(sawyercoding_chunk_header);
            std::memcpy(dst_file, encode_buffer, chunkHeader.length);

            free(encode_buffer);
            break;
        case CHUNK_ENCODING_RLECOMPRESSED:
            encode_buffer = static_cast<uint8_t*>(malloc(chunkHeader.length * 2));
            encode_buffer2 = static_cast<uint8_t*>(malloc(0x600000));
            chunkHeader.length = static_cast<uint32_t>(encode_chunk_repeat(buffer, encode_buffer, chunkHeader.length));
            chunkHeader.length = static_cast<uint32_t>(encode_chunk_rle(encode_buffer, encode_buffer2, chunkHeader.length));
            std::memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
            dst_file += sizeof(sawyercoding_chunk_header);
            std::memcpy(dst_file, encode_buffer2, chunkHeader.length);

            free(encode_buffer2);
            free(encode_buffer);
            break;
        case CHUNK_ENCODING_ROTATE:
            encode_buffer = static_cast<uint8_t*>(malloc(chunkHeader.length));
            std::memcpy(encode_buffer, buffer, chunkHeader.length);
            encode_chunk_rotate(encode_buffer, chunkHeader.length);
            std::memcpy(dst_file, &chunkHeader, sizeof(sawyercoding_chunk_header));
            dst_file += sizeof(sawyercoding_chunk_header);
            std::memcpy(dst_file, encode_buffer, chunkHeader.length);

            free(encode_buffer);
            break;
    }

    return chunkHeader.length + sizeof(sawyercoding_chunk_header);
}

size_t sawyercoding_decode_sv4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength)
{
    // (0 to length - 4): RLE chunk
    // (length - 4 to length): checksum
    return decode_chunk_rle_with_size(src, dst, length - 4, bufferLength);
}

size_t sawyercoding_decode_sc4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength)
{
    // Uncompress
    size_t decodedLength = decode_chunk_rle_with_size(src, dst, length - 4, bufferLength);

    // Decode
    for (size_t i = 0x60018; i <= std::min(decodedLength - 1, static_cast<size_t>(0x1F8353)); i++)
        dst[i] = dst[i] ^ 0x9C;

    for (size_t i = 0x60018; i <= std::min(decodedLength - 1, static_cast<size_t>(0x1F8350)); i += 4)
    {
        dst[i + 1] = Numerics::ror8(dst[i + 1], 3);

        uint32_t* code = reinterpret_cast<uint32_t*>(&dst[i]);
        *code = Numerics::rol32(*code, 9);
    }

    return decodedLength;
}

size_t sawyercoding_encode_sv4(const uint8_t* src, uint8_t* dst, size_t length)
{
    // Encode
    size_t encodedLength = encode_chunk_rle(src, dst, length);

    // Append checksum
    uint32_t checksum = sawyercoding_calculate_checksum(dst, encodedLength);
    *(reinterpret_cast<uint32_t*>(&dst[encodedLength])) = checksum;

    return encodedLength + 4;
}

size_t sawyercoding_decode_td6(const uint8_t* src, uint8_t* dst, size_t length)
{
    return decode_chunk_rle(src, dst, length - 4);
}

size_t sawyercoding_encode_td6(const uint8_t* src, uint8_t* dst, size_t length)
{
    size_t output_length = encode_chunk_rle(src, dst, length);

    uint32_t checksum = 0;
    for (size_t i = 0; i < output_length; i++)
    {
        uint8_t new_byte = ((checksum & 0xFF) + dst[i]) & 0xFF;
        checksum = (checksum & 0xFFFFFF00) + new_byte;
        checksum = Numerics::rol32(checksum, 3);
    }
    checksum -= 0x1D4C1;

    *(reinterpret_cast<uint32_t*>(&dst[output_length])) = checksum;
    output_length += 4;
    return output_length;
}

/* Based off of rct2: 0x006770C1 */
int32_t sawyercoding_validate_track_checksum(const uint8_t* src, size_t length)
{
    if (length < 4)
        return 0;

    uint32_t file_checksum = *(reinterpret_cast<const uint32_t*>(&src[length - 4]));

    uint32_t checksum = 0;
    for (size_t i = 0; i < length - 4; i++)
    {
        uint8_t new_byte = ((checksum & 0xFF) + src[i]) & 0xFF;
        checksum = (checksum & 0xFFFFFF00) + new_byte;
        checksum = Numerics::rol32(checksum, 3);
    }

    if (checksum - 0x1D4C1 == file_checksum)
        return 1; // .TD6
    else if (checksum - 0x1A67C == file_checksum)
        return 1; // .TD4
    else if (checksum - 0x1A650 == file_checksum)
        return 1; // .TD4
    else
        return 0;
}

#pragma region Decoding

/**
 *
 *  rct2: 0x0067693A
 */
static size_t decode_chunk_rle(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length)
{
    size_t count;
    uint8_t *dst, rleCodeByte;

    dst = dst_buffer;

    for (size_t i = 0; i < length; i++)
    {
        rleCodeByte = src_buffer[i];
        if (rleCodeByte & 128)
        {
            i++;
            count = 257 - rleCodeByte;
            std::fill_n(dst, count, src_buffer[i]);
            dst = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(dst) + count);
        }
        else
        {
            std::memcpy(dst, src_buffer + i + 1, rleCodeByte + 1);
            dst = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(dst) + rleCodeByte + 1);
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
static size_t decode_chunk_rle_with_size(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length, size_t dstSize)
{
    size_t count;
    uint8_t *dst, rleCodeByte;

    dst = dst_buffer;

    assert(length > 0);
    assert(dstSize > 0);
    for (size_t i = 0; i < length; i++)
    {
        rleCodeByte = src_buffer[i];
        if (rleCodeByte & 128)
        {
            i++;
            count = 257 - rleCodeByte;
            assert(dst + count <= dst_buffer + dstSize);
            assert(i < length);
            std::fill_n(dst, count, src_buffer[i]);
            dst = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(dst) + count);
        }
        else
        {
            assert(dst + rleCodeByte + 1 <= dst_buffer + dstSize);
            assert(i + 1 < length);
            std::memcpy(dst, src_buffer + i + 1, rleCodeByte + 1);
            dst = reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(dst) + rleCodeByte + 1);
            i += rleCodeByte + 1;
        }
    }

    // Return final size
    return dst - dst_buffer;
}

#pragma endregion

#pragma region Encoding

/**
 * Ensure dst_buffer is bigger than src_buffer then resize afterwards
 * returns length of dst_buffer
 */
static size_t encode_chunk_rle(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length)
{
    const uint8_t* src = src_buffer;
    uint8_t* dst = dst_buffer;
    const uint8_t* end_src = src + length;
    uint8_t count = 0;
    const uint8_t* src_norm_start = src;

    while (src < end_src - 1)
    {
        if ((count && *src == src[1]) || count > 125)
        {
            *dst++ = count - 1;
            std::memcpy(dst, src_norm_start, count);
            dst += count;
            src_norm_start += count;
            count = 0;
        }
        if (*src == src[1])
        {
            for (; (count < 125) && ((src + count) < end_src); count++)
            {
                if (*src != src[count])
                    break;
            }
            *dst++ = 257 - count;
            *dst++ = *src;
            src += count;
            src_norm_start = src;
            count = 0;
        }
        else
        {
            count++;
            src++;
        }
    }
    if (src == end_src - 1)
        count++;
    if (count)
    {
        *dst++ = count - 1;
        std::memcpy(dst, src_norm_start, count);
        dst += count;
    }
    return dst - dst_buffer;
}

static size_t encode_chunk_repeat(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length)
{
    if (length == 0)
        return 0;

    size_t outLength = 0;

    // Need to emit at least one byte, otherwise there is nothing to repeat
    *dst_buffer++ = 255;
    *dst_buffer++ = src_buffer[0];
    outLength += 2;

    // Iterate through remainder of the source buffer
    for (size_t i = 1; i < length;)
    {
        size_t searchIndex = (i < 32) ? 0 : (i - 32);
        size_t searchEnd = i - 1;

        size_t bestRepeatIndex = 0;
        size_t bestRepeatCount = 0;
        for (size_t repeatIndex = searchIndex; repeatIndex <= searchEnd; repeatIndex++)
        {
            size_t repeatCount = 0;
            size_t maxRepeatCount = std::min(std::min(static_cast<size_t>(7), searchEnd - repeatIndex), length - i - 1);
            // maxRepeatCount should not exceed length
            assert(repeatIndex + maxRepeatCount < length);
            assert(i + maxRepeatCount < length);
            for (size_t j = 0; j <= maxRepeatCount; j++)
            {
                if (src_buffer[repeatIndex + j] == src_buffer[i + j])
                {
                    repeatCount++;
                }
                else
                {
                    break;
                }
            }
            if (repeatCount > bestRepeatCount)
            {
                bestRepeatIndex = repeatIndex;
                bestRepeatCount = repeatCount;

                // Maximum repeat count is 8
                if (repeatCount == 8)
                    break;
            }
        }

        if (bestRepeatCount == 0)
        {
            *dst_buffer++ = 255;
            *dst_buffer++ = src_buffer[i];
            outLength += 2;
            i++;
        }
        else
        {
            *dst_buffer++ = static_cast<uint8_t>((bestRepeatCount - 1) | ((32 - (i - bestRepeatIndex)) << 3));
            outLength++;
            i += bestRepeatCount;
        }
    }

    return outLength;
}

static void encode_chunk_rotate(uint8_t* buffer, size_t length)
{
    size_t i;
    uint8_t code = 1;
    for (i = 0; i < length; i++)
    {
        buffer[i] = Numerics::rol8(buffer[i], code);
        code = (code + 2) % 8;
    }
}

#pragma endregion

int32_t sawyercoding_detect_file_type(const uint8_t* src, size_t length)
{
    if (length < 4)
    {
        throw std::length_error("Stream is (nearly) empty!");
    }

    size_t i;

    // Currently can't detect TD4, as the checksum is the same as SC4 (need alternative method)

    uint32_t checksum = *(reinterpret_cast<const uint32_t*>(&src[length - 4]));
    uint32_t actualChecksum = 0;
    for (i = 0; i < length - 4; i++)
    {
        actualChecksum = (actualChecksum & 0xFFFFFF00) | (((actualChecksum & 0xFF) + static_cast<uint8_t>(src[i])) & 0xFF);
        actualChecksum = Numerics::rol32(actualChecksum, 3);
    }

    return sawyercoding_detect_rct1_version(checksum - actualChecksum);
}

int32_t sawyercoding_detect_rct1_version(int32_t gameVersion)
{
    int32_t fileType = (gameVersion) > 0 ? FILE_TYPE_SV4 : FILE_TYPE_SC4;
    gameVersion = abs(gameVersion);

    if (gameVersion >= 108000 && gameVersion < 110000)
        return (FILE_VERSION_RCT1 | fileType);
    if (gameVersion >= 110000 && gameVersion < 120000)
        return (FILE_VERSION_RCT1_AA | fileType);
    if (gameVersion >= 120000 && gameVersion < 130000)
        return (FILE_VERSION_RCT1_LL | fileType);
    // RCTOA Acres sets this, and possibly some other user-created scenarios as well
    if (gameVersion == 0)
        return (FILE_VERSION_RCT1_LL | fileType);

    return -1;
}
