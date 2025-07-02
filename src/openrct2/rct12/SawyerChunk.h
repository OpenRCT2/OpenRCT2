/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/MemoryStream.h"

#include <cstdint>

namespace OpenRCT2
{
    /**
     * The type of encoding / compression for a sawyer encoded chunk.
     */
    enum class SawyerEncoding : uint8_t
    {
        none,
        rle,
        rleCompressed,
        rotate,
    };

    namespace SawyerCoding
    {
#pragma pack(push, 1)
        struct ChunkHeader
        {
            SawyerEncoding encoding;
            uint32_t length;
        };
        static_assert(sizeof(ChunkHeader) == 5);
#pragma pack(pop)
    } // namespace SawyerCoding

    /**
     * Represents a sawyer encoded chunk.
     */
    class SawyerChunk final
    {
    private:
        OpenRCT2::MemoryStream _data;
        SawyerEncoding _encoding = SawyerEncoding::none;

    public:
        const void* GetData() const
        {
            return _data.GetData();
        }
        size_t GetLength() const
        {
            return _data.GetLength();
        }
        SawyerEncoding GetEncoding() const
        {
            return _encoding;
        }

        SawyerChunk(SawyerEncoding encoding, OpenRCT2::MemoryStream&& data);
    };
} // namespace OpenRCT2
