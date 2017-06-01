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

#pragma once

#include "../common.h"

/**
 * The type of encoding / compression for a sawyer encoded chunk.
 */
enum class SAWYER_ENCODING : uint8
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
    void *          _data       = nullptr;
    size_t          _length     = 0;
    SAWYER_ENCODING _encoding   = SAWYER_ENCODING::NONE;

public:
    const void *    GetData() const { return _data; }
    size_t          GetLength() const { return _length; }
    SAWYER_ENCODING GetEncoding() const { return _encoding; }

    SawyerChunk(SAWYER_ENCODING encoding, void * data, size_t length);
    ~SawyerChunk();
};
