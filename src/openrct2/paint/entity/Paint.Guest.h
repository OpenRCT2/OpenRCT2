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

namespace OpenRCT2
{
    struct Guest;
    enum class PeepAnimationType : uint8_t;
} // namespace OpenRCT2

struct BoundBoxXYZ;
struct PaintSession;
using Direction = uint8_t;

void PaintGuest(PaintSession& session, const OpenRCT2::Guest& guest, int32_t orientation);
void PaintGuestAccesories(
    PaintSession& session, const OpenRCT2::Guest& guest, uint8_t imageOffset, OpenRCT2::PeepAnimationType actionAnimationGroup,
    Direction direction);
