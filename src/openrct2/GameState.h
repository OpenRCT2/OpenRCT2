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
#include "Editor.h"
#include "Limits.h"
#include "interface/ZoomLevel.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "ride/Ride.h"
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

    enum class StaffSpeedCheat
    {
        None,
        Frozen,
        Fast,
    };

    struct CheatsState
    {
        bool AllowTrackPlaceInvalidHeights;
        bool AllowRegularPathAsQueue;
        bool AllowSpecialColourSchemes;
        bool MakeAllDestructible;
        StaffSpeedCheat SelectedStaffSpeed;
    };

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
        money64 CompanyValue;
        money64 ConstructionRightsPrice;
        money64 CurrentExpenditure;
        money64 CurrentProfit;
        uint8_t ParkRatingHistory[32];
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
        uint32_t NumGuestsInPark;
        uint32_t NumGuestsHeadingForPark;
        uint32_t NumGuestsInParkLastWeek;
        money64 WeeklyProfitAverageDividend;
        uint64_t TotalAdmissions;
        money64 TotalIncomeFromAdmissions;
        money64 TotalRideValueForMoney;
        uint16_t WeeklyProfitAverageDivisor;
        money64 WeeklyProfitHistory[FINANCE_GRAPH_SIZE];
        Objective ScenarioObjective;
        uint16_t ScenarioParkRatingWarningDays;
        money64 ScenarioCompletedCompanyValue;
        money64 ScenarioCompanyValueRecord;
        money64 BankLoan;
        uint8_t BankLoanInterestRate;
        money64 MaxBankLoan;
        random_engine_t ScenarioRand;
        TileCoordsXY MapSize;

        ::EditorStep EditorStep;

        SCENARIO_CATEGORY ScenarioCategory;
        std::string ScenarioName;
        std::string ScenarioDetails;
        std::string ScenarioCompletedBy;

        std::vector<Banner> Banners;
        // Ride storage for all the rides in the park, rides with RideId::Null are considered free.
        std::array<Ride, OpenRCT2::Limits::MaxRidesInPark> Rides{};
        std::vector<TileElement> TileElements;

        std::vector<ScenerySelection> RestrictedScenery;

        News::ItemQueues NewsItems;

        colour_t StaffHandymanColour;
        colour_t StaffMechanicColour;
        colour_t StaffSecurityColour;

        uint8_t ResearchFundingLevel;
        uint8_t ResearchPriorities;
        uint16_t ResearchProgress;
        uint8_t ResearchProgressStage;
        uint8_t ResearchExpectedMonth;
        uint8_t ResearchExpectedDay;
        std::optional<ResearchItem> ResearchLastItem;
        std::optional<ResearchItem> ResearchNextItem;

        std::vector<ResearchItem> ResearchItemsUninvented;
        std::vector<ResearchItem> ResearchItemsInvented;
        uint8_t ResearchUncompletedCategories;

        ScreenCoordsXY SavedView;
        uint8_t SavedViewRotation;
        ZoomLevel SavedViewZoom;

        /**
         * Probability out of 65535, of gaining a new guest per game tick.
         * new guests per second = 40 * (probability / 65535)
         * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive
         * awards, approximately 1 guest per second can be generated (+60 guests in one minute).
         */
        int32_t GuestGenerationProbability;
        /**
         * In a difficult guest generation scenario, no guests will be generated if over this value.
         */
        uint32_t SuggestedGuestMaximum;

        CheatsState Cheats;
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
