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

using StringId = uint16_t;

constexpr StringId kStringIdNone = 0xFFFF;
constexpr StringId kStringIdEmpty = 0;

constexpr uint16_t kRealNameStart = 0xA000;
constexpr uint16_t kRealNameEnd = 0xDFFF;
