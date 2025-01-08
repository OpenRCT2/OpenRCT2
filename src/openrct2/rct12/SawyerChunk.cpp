/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SawyerChunk.h"

#include "SawyerChunkReader.h"

namespace OpenRCT2
{
    SawyerChunk::SawyerChunk(SAWYER_ENCODING encoding, MemoryStream&& data)
        : _data(std::move(data))
        , _encoding(encoding)
    {
    }
} // namespace OpenRCT2
