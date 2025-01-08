/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../support/MetalSupports.h"
#include "../support/WoodenSupports.h"

constexpr int8_t kDefaultGeneralSupportHeight = 32;
constexpr int8_t kGeneralSupportHeightOnRidePhoto = 48;
constexpr int8_t kGeneralSupportHeightOnRidePhotoInverted = 64;

struct SupportType
{
    union
    {
        uint8_t generic{};
        WoodenSupportType wooden;
        MetalSupportType metal;
    };

    explicit constexpr SupportType()
    {
    }

    constexpr SupportType(uint8_t _generic)
        : generic(_generic)
    {
    }

    constexpr SupportType(WoodenSupportType _wooden)
        : wooden(_wooden)
    {
    }

    constexpr SupportType(MetalSupportType _metal)
        : metal(_metal)
    {
    }
};

namespace OpenRCT2::SupportedSequences
{
    constexpr MetalSupportPlace kDiagStraightFlat[] = { MetalSupportPlace::None, MetalSupportPlace::None,
                                                        MetalSupportPlace::None, MetalSupportPlace::LeftCorner };
}
