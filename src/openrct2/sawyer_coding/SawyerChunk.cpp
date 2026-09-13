/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunk.h"

#include "SawyerChunkReader.h"

#include <utility>

namespace OpenRCT2::SawyerCoding
{
    SawyerChunk::SawyerChunk(ChunkEncoding encoding, MemoryStream&& data)
        : _data(std::move(data))
        , _encoding(encoding)
    {
    }
} // namespace OpenRCT2::SawyerCoding
