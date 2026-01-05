/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "SawyerChunk.h"

#include <cstddef>
#include <cstdint>

namespace OpenRCT2::SawyerCoding
{
    // TODO: make enum class in SawyerCoding namespace
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

    uint32_t CalculateChecksum(const uint8_t* buffer, size_t length);
    size_t WriteChunkBuffer(uint8_t* dst_file, const uint8_t* src_buffer, ChunkHeader chunkHeader);
    size_t DecodeSV4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t DecodeSC4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t EncodeSV4(const uint8_t* src, uint8_t* dst, size_t length);
    size_t DecodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    size_t EncodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    int32_t ValidateTrackChecksum(const uint8_t* src, size_t length);

    int32_t DetectFileType(const uint8_t* src, size_t length);
    int32_t DetectRCT1Version(int32_t gameVersion);
} // namespace OpenRCT2::SawyerCoding
