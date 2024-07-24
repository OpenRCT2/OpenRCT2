/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../support/MetalSupports.h"

constexpr int8_t kDefaultGeneralSupportHeight = 32;

namespace OpenRCT2::SupportedSequences
{
    constexpr MetalSupportPlace kDiagStraightFlat[] = { MetalSupportPlace::None, MetalSupportPlace::None,
                                                        MetalSupportPlace::None, MetalSupportPlace::LeftCorner };
}
