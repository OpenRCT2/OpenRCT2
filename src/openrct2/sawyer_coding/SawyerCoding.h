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
    uint32_t CalculateChecksum(const uint8_t* buffer, size_t length);
    size_t WriteChunkBuffer(uint8_t* dst_file, const uint8_t* src_buffer, ChunkHeader chunkHeader);
    size_t DecodeSV4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t DecodeSC4(const uint8_t* src, uint8_t* dst, size_t length, size_t bufferLength);
    size_t EncodeSV4(const uint8_t* src, uint8_t* dst, size_t length);
    size_t DecodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    size_t EncodeTD6(const uint8_t* src, uint8_t* dst, size_t length);
    int32_t ValidateTrackChecksum(const uint8_t* src, size_t length);
    size_t EncodeChunkRLE(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length);
    size_t EncodeChunkRepeat(const uint8_t* src_buffer, uint8_t* dst_buffer, size_t length);
    void EncodeChunkRotate(uint8_t* buffer, size_t length);
} // namespace OpenRCT2::SawyerCoding
