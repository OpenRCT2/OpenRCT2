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
    struct Staff;
}

struct PaintSession;

void PaintStaff(PaintSession& session, const OpenRCT2::Staff& staff, int32_t orientation);
void PaintStaffLightingEffects(const OpenRCT2::Staff& staff);
