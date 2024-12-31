/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <openrct2/localisation/StringIdType.h>

class Formatter;
struct Objective;

namespace OpenRCT2::Ui
{
    void formatObjective(Formatter& ft, Objective objective);

    extern const StringId kObjectiveNames[12];
} // namespace OpenRCT2::Ui
