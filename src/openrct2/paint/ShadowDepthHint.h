/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../drawing/IDrawingContext.h"

#include <cstdint>

struct PaintStruct;

/**
 * What a paint struct is to the drop-shadow pipeline: which per-pixel solver reconstructs its world
 * position, and the geometry that solver needs. The drawing engine carries it onto every command the
 * sprite draws. See SpriteWorldDepthMode for the kinds.
 */
OpenRCT2::Drawing::SpriteWorldDepthHint MakeSpriteWorldDepthHint(const PaintStruct& ps, uint8_t rotation);
