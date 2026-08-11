/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"

#include <cstdint>

namespace OpenRCT2::SawyerCoding
{
    /**
     * The type of encoding / compression for a sawyer encoded chunk.
     */
    enum class ChunkEncoding : uint8_t
    {
        none,
        rle,
        rleCompressed,
        rotate,
    };
#pragma pack(push, 1)
    struct ChunkHeader
    {
        ChunkEncoding encoding;
        uint32_t length;
    };
    static_assert(sizeof(ChunkHeader) == 5);
#pragma pack(pop)

    /**
     * Represents a sawyer encoded chunk.
     */
    class SawyerChunk final
    {
    private:
        MemoryStream _data;
        ChunkEncoding _encoding = ChunkEncoding::none;

    public:
        const void* GetData() const
        {
            return _data.GetData();
        }
        size_t GetLength() const
        {
            return _data.GetLength();
        }
        ChunkEncoding GetEncoding() const
        {
            return _encoding;
        }

        SawyerChunk(ChunkEncoding encoding, MemoryStream&& data);
    };
} // namespace OpenRCT2::SawyerCoding
