/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Date.h"

#ifdef _WIN32
    #include <windows.h>
#endif

#include "../Context.h"
#include "../Game.h"
#include "../config/Config.h"
#include "../core/EnumUtils.hpp"
#include "../core/File.h"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../localisation/Currency.h"
#include "../localisation/Formatting.h"
#include "Platform.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <time.h>

#ifdef _WIN32
static constexpr std::array _prohibitedCharacters = { '<', '>', '*', '\\', ':', '|', '?', '"', '/' };
#else
#endif

namespace OpenRCT2::Platform
{
    static double _ticks = 0;

    uint32_t GetTicks()
    {
        return round(_ticks);
    }

    void AdvanceTicks()
    {
        // ms to advance ticks by. 16.67 => 1000/16.67 = 60fps
        _ticks += 16.67;
    }
} // namespace OpenRCT2::Platform
