/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunk.h"

#include "../core/Memory.hpp"
#include "SawyerChunkReader.h"

SawyerChunk::SawyerChunk(SAWYER_ENCODING encoding, std::unique_ptr<uint8_t[]> data, size_t length)
{
    _encoding = encoding;
    _data = std::move(data);
    _length = length;
}
