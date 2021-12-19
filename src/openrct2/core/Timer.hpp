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
    /// <summary>
    /// Restartable timer utility that starts at construction.
    /// </summary>
    class Timer
    {
        using Clock = std::chrono::high_resolution_clock;
        using Timepoint = Clock::time_point;

        Timepoint _tp = Clock::now();

    public:
        /// <summary>
        /// Restarts the timer.
        /// </summary>
        /// <returns></returns>
        void Restart() noexcept
        {
            _tp = Clock::now();
        }

        /// <summary>
        /// Returns the amount of time in seconds since the last start.
        /// </summary>
        [[nodiscard]] std::chrono::duration<float> GetElapsedTime() const noexcept
        {
            return Clock::now() - _tp;
        }

        /// <summary>
        /// Returns the amount of time in seconds since the last start and restarts.
        /// </summary>
        [[nodiscard]] std::chrono::duration<float> GetElapsedTimeAndRestart() noexcept
        {
            const auto res = Clock::now() - _tp;
            Restart();
            return res;
        }
    };

} // namespace OpenRCT2
