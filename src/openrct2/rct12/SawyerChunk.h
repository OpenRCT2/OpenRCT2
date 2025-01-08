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
    enum class SAWYER_ENCODING : uint8_t
    {
        NONE,
        RLE,
        RLECOMPRESSED,
        ROTATE,
    };

    /**
     * Represents a sawyer encoded chunk.
     */
    class SawyerChunk final
    {
    private:
        OpenRCT2::MemoryStream _data;
        SAWYER_ENCODING _encoding = SAWYER_ENCODING::NONE;

    public:
        const void* GetData() const
        {
            return _data.GetData();
        }
        size_t GetLength() const
        {
            return _data.GetLength();
        }
        SAWYER_ENCODING GetEncoding() const
        {
            return _encoding;
        }

        SawyerChunk(SAWYER_ENCODING encoding, OpenRCT2::MemoryStream&& data);
    };
} // namespace OpenRCT2
