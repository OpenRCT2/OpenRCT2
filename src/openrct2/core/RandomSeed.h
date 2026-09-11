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
#include <random>

#ifdef __amigaos__
    #include <ctime>
extern "C" unsigned amiga_ticks_ms(void);
#endif

namespace OpenRCT2
{
    // A non-deterministic seed for the PRNGs that are not part of the simulation.
    // libstdc++'s std::random_device has no entropy source on AmigaOS and throws.
    inline uint32_t RandomDeviceSeed()
    {
#ifdef __amigaos__
        static uint32_t counter = 0;
        return static_cast<uint32_t>(std::time(nullptr)) ^ (amiga_ticks_ms() * 2654435761u) ^ (++counter << 24);
#else
        return std::random_device{}();
#endif
    }
} // namespace OpenRCT2
