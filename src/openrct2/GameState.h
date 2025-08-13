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
#include "entity/EntityRegistry.h"
#include "interface/ZoomLevel.h"
#include "management/Finance.h"
#include "management/Marketing.h"
#include "management/NewsItem.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "scenario/Scenario.h"
#include "world/Banner.h"
#include "world/Climate.h"
#include "world/Location.hpp"
#include "world/ParkData.h"
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

        WeatherState weatherCurrent;
        WeatherState weatherNext;
        uint16_t weatherUpdateTimer;

        money64 initialCash;
        money64 guestInitialCash;
        uint8_t guestInitialHappiness;
        uint8_t guestInitialHunger;
        uint8_t guestInitialThirst;
        uint8_t guestChangeModifier;
        uint32_t nextGuestNumber;

        money64 totalRideValueForMoney;

        Objective scenarioObjective;
        uint16_t scenarioParkRatingWarningDays;
        money64 scenarioCompletedCompanyValue;
        money64 scenarioCompanyValueRecord;

        uint8_t bankLoanInterestRate;
        random_engine_t scenarioRand;
        TileCoordsXY mapSize;
        money64 landPrice;
        money64 constructionRightsPrice;

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

        CheatsState cheats;
    };

    GameState_t& getGameState();
    void swapGameState(std::unique_ptr<GameState_t>& otherState);

    void gameStateInitAll(GameState_t& gameState, const TileCoordsXY& mapSize);
    void gameStateTick();
    void gameStateUpdateLogic();

} // namespace OpenRCT2
