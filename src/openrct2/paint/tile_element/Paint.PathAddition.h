/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageId.hpp"
#include "../../localisation/StringIdType.h"
#include "../Paint.h"

#include <cstdint>

namespace OpenRCT2
{
    struct PathElement;
}

void paintPathAddition(
    PaintSession& session, const OpenRCT2::PathElement& pathElement, uint16_t height, ImageId sceneryImageTemplate);
void PaintLampLightEffects(PaintSession& session, const OpenRCT2::PathElement& pathEl, uint16_t height);
