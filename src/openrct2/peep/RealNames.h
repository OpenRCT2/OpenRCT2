/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

constexpr uint16_t kRealNameStart = 0xA000;
constexpr uint16_t kRealNameEnd = 0xDFFF;

// Real name data
extern const char real_name_initials[16];
extern const char* real_names[1024];
