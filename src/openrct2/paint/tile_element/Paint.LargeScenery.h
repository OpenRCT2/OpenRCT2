/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

struct PaintSession;

namespace OpenRCT2
{
    struct LargeSceneryElement;
}

void PaintLargeScenery(
    PaintSession& session, uint8_t direction, uint16_t height, const OpenRCT2::LargeSceneryElement& tileElement);
