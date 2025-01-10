/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../drawing/ImageId.hpp"
#include "../Paint.h"

#include <cstdint>

void Sub6A3F61PathAddition(
    PaintSession& session, const PathElement& pathElement, uint16_t height, ImageId sceneryImageTemplate);
void PaintLampLightEffects(PaintSession& session, const PathElement& pathEl, uint16_t height);
