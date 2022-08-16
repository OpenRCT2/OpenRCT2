/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Date.h"
#include "fireworks/FireworksManager.h"
#include "world/Location.hpp"

#include <array>
#include <chrono>
#include <memory>
#include <unordered_map>

namespace OpenRCT2
{
    class Park;

    // Information regarding various pieces of logic update
    enum class LogicTimePart
    {
        NetworkUpdate,
        Date,
        Scenario,
        Climate,
        MapTiles,
        MapStashProvisionalElements,
        MapPathWideFlags,
        Peep,
        MapRestoreProvisionalElements,
        Vehicle,
        Misc,
        Ride,
        Park,
        Research,
        RideRatings,
        RideMeasurments,
        News,
        MapAnimation,
        Sounds,
        GameActions,
        NetworkFlush,
        Scripts,
    };

    // ~6.5s at 40Hz
    constexpr size_t LOGIC_UPDATE_MEASUREMENTS_COUNT = 256;

    // In order not to cause allocations, collect multiple samples into single pre-allocated struct
    using LogicTimingInfo = std::unordered_map<
        LogicTimePart, std::array<std::chrono::duration<double>, LOGIC_UPDATE_MEASUREMENTS_COUNT>>;

    struct LogicTimings
    {
        LogicTimingInfo TimingInfo;
        size_t CurrentIdx{};
    };

    /**
     * Class to update the state of the map and park.
     */
    class GameState final
    {
    private:
        std::unique_ptr<Park> _park;
        Date _date;
        Fireworks::FireworksManager _fireworksMgr;
    public:
        GameState();
        GameState(const GameState&) = delete;

        Date& GetDate()
        {
            return _date;
        }
        Park& GetPark()
        {
            return *_park;
        }

        Fireworks::FireworksManager& GetFireworksManager()
        {
            return _fireworksMgr;
        }

        void InitAll(const TileCoordsXY& mapSize);
        void Tick();
        void UpdateLogic(LogicTimings* timings = nullptr);

    private:
        void CreateStateSnapshot();
    };
} // namespace OpenRCT2
