/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

namespace OpenRCT2
{

#pragma pack(push, 1)
    struct SawyerCodingChunkHeader
    {
        uint8_t encoding;
        uint32_t length;
    };
    assert_struct_size(SawyerCodingChunkHeader, 5);
#pragma pack(pop)

    enum
    {
        CHUNK_ENCODING_NONE,
        CHUNK_ENCODING_RLE,
        CHUNK_ENCODING_RLECOMPRESSED,
        CHUNK_ENCODING_ROTATE
    };

    enum
    {
        FILE_VERSION_MASK = (3 << 0),
        FILE_VERSION_RCT1 = (0 << 0),
        FILE_VERSION_RCT1_AA = (1 << 0),
        FILE_VERSION_RCT1_LL = (2 << 0),

        FILE_TYPE_MASK = (3 << 2),
        FILE_TYPE_TD4 = (0 << 2),
        FILE_TYPE_SV4 = (1 << 2),
        FILE_TYPE_SC4 = (2 << 2)
    };

    uint32_t SawyerCodingCalculateChecksum(const uint8_t* buffer, size_t length);
    size_t SawyerCodingWriteChunkBuffer(uint8_t* dst_file, const uint8_t* src_buffer, SawyerCodingChunkHeader chunkHeader);
    size_t SawyerCodingDecodeSV4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t SawyerCodingDecodeSC4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t SawyerCodingEencodeSV4(const uint8_t* src, uint8_t* dst, size_t length);
    size_t SawyerCodingDecodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    size_t SawyerCodingEncodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    int32_t SawyerCodingValidateTrackChecksum(const uint8_t* src, size_t length);

    int32_t SawyerCodingDetectFileType(const uint8_t* src, size_t length);
    int32_t SawyerCodingDetectRCT1Version(int32_t gameVersion);
} // namespace OpenRCT2
