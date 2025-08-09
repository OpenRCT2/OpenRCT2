/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Cheats.h"
#include "Date.h"
#include "Editor.h"
#include "Limits.h"
#include "core/EnumUtils.hpp"
#include "entity/EntityRegistry.h"
#include "interface/ZoomLevel.h"
#include "management/Award.h"
#include "management/Finance.h"
#include "management/Marketing.h"
#include "management/NewsItem.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "scenario/Scenario.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Location.hpp"
#include "world/Park.h"
#include "world/ScenerySelection.h"

#include <array>
#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct GameState_t
    {
        Park::ParkData park{};
        std::string pluginStorage;
        uint32_t currentTicks{};
        Date date;
        money64 companyValue;
        // The total profit for the entire scenario that precedes the current financial table.
        money64 historicalProfit;
        money64 constructionRightsPrice;
        money64 currentExpenditure;
        money64 currentProfit;
        uint32_t guestsInParkHistory[kGuestsInParkHistorySize];
        WeatherState weatherCurrent;
        WeatherState weatherNext;
        uint16_t weatherUpdateTimer;
        money64 cash;
        money64 cashHistory[kFinanceHistorySize];
        money64 initialCash;
        money64 guestInitialCash;
        uint8_t guestInitialHappiness;
        uint8_t guestInitialHunger;
        uint8_t guestInitialThirst;
        uint8_t guestChangeModifier;
        uint32_t nextGuestNumber;
        uint32_t numGuestsInPark;
        uint32_t numGuestsHeadingForPark;
        uint32_t numGuestsInParkLastWeek;
        money64 weeklyProfitAverageDividend;
        uint64_t totalAdmissions;
        money64 totalIncomeFromAdmissions;
        money64 totalRideValueForMoney;
        uint16_t weeklyProfitAverageDivisor;
        money64 weeklyProfitHistory[kFinanceHistorySize];
        Objective scenarioObjective;
        uint16_t scenarioParkRatingWarningDays;
        money64 scenarioCompletedCompanyValue;
        money64 scenarioCompanyValueRecord;
        money64 bankLoan;
        uint8_t bankLoanInterestRate;
        money64 maxBankLoan;
        money64 expenditureTable[kExpenditureTableMonthCount][EnumValue(ExpenditureType::Count)];
        random_engine_t scenarioRand;
        TileCoordsXY mapSize;
        money64 landPrice;

        EditorStep editorStep;

        ScenarioCategory scenarioCategory;
        std::string scenarioName;
        std::string scenarioDetails;
        std::string scenarioCompletedBy;
        std::string scenarioFileName;

        std::vector<Banner> banners;
        Entity_t entities[kMaxEntities]{};
        // Ride storage for all the rides in the park, rides with RideId::Null are considered free.
        std::array<Ride, OpenRCT2::Limits::kMaxRidesInPark> rides{};
        size_t ridesEndOfUsedRange{};
        RideRatingUpdateStates rideRatingUpdateStates;
        std::vector<TileElement> tileElements;

        std::vector<ScenerySelection> restrictedScenery;

        std::vector<PeepSpawn> peepSpawns;
        uint8_t peepWarningThrottle[16];

        News::ItemQueues newsItems;

        uint16_t grassSceneryTileLoopPosition;
        CoordsXY widePathTileLoopPosition;

        colour_t staffHandymanColour;
        colour_t staffMechanicColour;
        colour_t staffSecurityColour;
        uint64_t samePriceThroughoutPark{};

        uint8_t researchFundingLevel;
        uint8_t researchPriorities;
        uint16_t researchProgress;
        uint8_t researchProgressStage;
        uint8_t researchExpectedMonth;
        uint8_t researchExpectedDay;
        std::optional<ResearchItem> researchLastItem;
        std::optional<ResearchItem> researchNextItem;

        std::vector<ResearchItem> researchItemsUninvented;
        std::vector<ResearchItem> researchItemsInvented;
        uint8_t researchUncompletedCategories;

        ScreenCoordsXY savedView;
        uint8_t savedViewRotation;
        ZoomLevel savedViewZoom;

        ObjectEntryIndex lastEntranceStyle;

        std::vector<Award> currentAwards;

        std::vector<MarketingCampaign> marketingCampaigns;

        /**
         * Probability out of 65535, of gaining a new guest per game tick.
         * new guests per second = 40 * (probability / 65535)
         * With a full park rating, non-overpriced entrance fee, less guests than the suggested maximum and four positive
         * awards, approximately 1 guest per second can be generated (+60 guests in one minute).
         */
        int32_t guestGenerationProbability;
        /**
         * In a difficult guest generation scenario, no guests will be generated if over this value.
         */
        uint32_t suggestedGuestMaximum;

        /** Whether to use stable paint sort (true) or legacy paint sort (false) */
        bool useStablePaintSort;

        CheatsState cheats;
    };

    GameState_t& getGameState();
    void swapGameState(std::unique_ptr<GameState_t>& otherState);

    void gameStateInitAll(GameState_t& gameState, const TileCoordsXY& mapSize);
    void gameStateTick();
    void gameStateUpdateLogic();

} // namespace OpenRCT2
