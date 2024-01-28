/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Date.h"
#include "management/Finance.h"
#include "scenario/Scenario.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Location.hpp"

#include <array>
#include <chrono>
#include <memory>
#include <unordered_map>
#include <vector>

namespace OpenRCT2
{
    class Park;

    struct GameState_t
    {
        uint32_t CurrentTicks{};
        uint64_t ParkFlags;
        uint16_t ParkRating;
        money64 ParkEntranceFee;
        std::vector<CoordsXYZD> ParkEntrances;
        uint32_t ParkSize;
        money64 ParkValue;
        money64 ParkValueHistory[FINANCE_GRAPH_SIZE];
        ClimateType Climate;
        ClimateState ClimateCurrent;
        ClimateState ClimateNext;
        uint16_t ClimateUpdateTimer;
        money64 Cash;
        money64 InitialCash;
        money64 GuestInitialCash;
        uint8_t GuestInitialHappiness;
        uint8_t GuestInitialHunger;
        uint8_t GuestInitialThirst;
        uint32_t NextGuestNumber;
        uint32_t NumGuestsHeadingForPark;
        money64 WeeklyProfitAverageDividend;
        uint16_t WeeklyProfitAverageDivisor;
        money64 WeeklyProfitHistory[FINANCE_GRAPH_SIZE];
        Objective ScenarioObjective;
        uint16_t ScenarioParkRatingWarningDays;
        money64 ScenarioCompletedCompanyValue;
        money64 ScenarioCompanyValueRecord;
        random_engine_t ScenarioRand;

        SCENARIO_CATEGORY ScenarioCategory;
        std::string ScenarioName;
        std::string ScenarioDetails;
        std::string ScenarioCompletedBy;
        std::vector<Banner> Banners;
    };

    GameState_t& GetGameState();

    /**
     * Class to update the state of the map and park.
     */
    class GameState final
    {
    private:
        std::unique_ptr<Park> _park;
        Date _date;

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

        void InitAll(const TileCoordsXY& mapSize);
        void Tick();
        void UpdateLogic();
        void SetDate(Date newDate);
        void ResetDate();

    private:
        void CreateStateSnapshot();
    };
} // namespace OpenRCT2
