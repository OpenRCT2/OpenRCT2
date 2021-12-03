/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/
#pragma once

#include <chrono>

namespace OpenRCT2
{
    class Timer
    {
        using Clock = std::chrono::high_resolution_clock;
        using Timepoint = Clock::time_point;

        Timepoint _tp = Clock::now();

    public:
        void Restart() noexcept
        {
            _tp = Clock::now();
        }

        std::chrono::duration<float> GetElapsedSeconds() const noexcept
        {
            return Clock::now() - _tp;
        }
    };

} // namespace OpenRCT2
