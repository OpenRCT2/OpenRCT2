/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"

interface IStream;

enum class RCT12TrackDesignVersion : uint8_t;

namespace SawyerEncoding
{
    bool ValidateChecksum(IStream* stream);
    RCT12TrackDesignVersion ValidateTrackChecksum(IStream* stream);
} // namespace SawyerEncoding
