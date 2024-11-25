/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/EnumUtils.hpp"
#include "../entity/Peep.h"
#include "PeepAnimations.h"

namespace OpenRCT2
{
    const PeepAnimation& GetPeepAnimation(
        PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup = PeepAnimationType::Walking);
    const SpriteBounds& GetSpriteBounds(
        PeepAnimationGroup spriteType, PeepAnimationType actionAnimationGroup = PeepAnimationType::Walking);

    void inferMaxPeepSpriteDimensions();
} // namespace OpenRCT2
