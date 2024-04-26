/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

#include <memory>

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
    std::unique_ptr<uint8_t[]> _data;
    size_t _length = 0;
    SAWYER_ENCODING _encoding = SAWYER_ENCODING::NONE;

public:
    const void* GetData() const
    {
        return _data.get();
    }
    size_t GetLength() const
    {
        return _length;
    }
    SAWYER_ENCODING GetEncoding() const
    {
        return _encoding;
    }

    SawyerChunk(SAWYER_ENCODING encoding, std::unique_ptr<uint8_t[]> data, size_t length);
};
