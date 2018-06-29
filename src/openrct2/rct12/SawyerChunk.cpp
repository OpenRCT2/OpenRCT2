/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../core/Memory.hpp"
#include "SawyerChunk.h"

SawyerChunk::SawyerChunk(SAWYER_ENCODING encoding, void * data, size_t length)
{
    _encoding = encoding;
    _data = data;
    _length = length;
}

SawyerChunk::~SawyerChunk()
{
    Memory::Free(_data);
}
