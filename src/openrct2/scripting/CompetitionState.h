/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include <cstdint>
    #include <string>
    #include <unordered_map>

namespace OpenRCT2::Scripting
{
    enum class CompetitionStatus : uint8_t
    {
        Idle,
        Active,
        Finished,
    };

    enum class CompetitionMetric : uint8_t
    {
        ParkValue,
        GuestCount,
        ParkRating,
        RideCount,
        MoneyEarned,
        Custom,
    };

    struct PlayerScore
    {
        uint8_t PlayerId{};
        std::string PlayerName;
        int64_t Score{};
        int32_t Rank{};
    };

    struct CompetitionState
    {
        CompetitionStatus Status{ CompetitionStatus::Idle };
        CompetitionMetric Metric{ CompetitionMetric::ParkValue };
        uint32_t DurationTicks{};
        uint32_t StartTick{};
        uint32_t UpdateIntervalTicks{ 100 };
        std::string Name;
        std::string HostPluginId;
        std::unordered_map<uint8_t, PlayerScore> Scores;

        bool IsActive() const noexcept
        {
            return Status == CompetitionStatus::Active;
        }

        uint32_t TicksRemaining(uint32_t currentTick) const noexcept
        {
            if (currentTick <= StartTick)
                return DurationTicks;
            const auto elapsed = currentTick - StartTick;
            return elapsed >= DurationTicks ? 0u : DurationTicks - elapsed;
        }
    };
} // namespace OpenRCT2::Scripting

#endif
