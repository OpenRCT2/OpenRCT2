/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include "core/Random.hpp"
#include "entity/EntityRegistry.h"
#include "interface/ZoomLevel.h"
#include "management/Finance.h"
#include "management/NewsItem.h"
#include "ride/Ride.h"
#include "ride/RideRatings.h"
#include "scenario/ScenarioOptions.h"
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
    using PeepSpawn = CoordsXYZD;

    struct GameState_t
    {
        Park::ParkData park{};
        Scenario::Options scenarioOptions;
        std::string pluginStorage;
        uint32_t currentTicks{};
        Date date;

        WeatherState weatherCurrent;
        WeatherState weatherNext;
        uint16_t weatherUpdateTimer;

        uint32_t nextGuestNumber;

        uint16_t scenarioParkRatingWarningDays;
        money64 scenarioCompletedCompanyValue;
        money64 scenarioCompanyValueRecord;
        random_engine_t scenarioRand;

        TileCoordsXY mapSize;

        EditorStep editorStep;

        std::string scenarioCompletedBy;
        std::string scenarioFileName;

        std::vector<Banner> banners;

        EntityRegistry entities;

        // Ride storage for all the rides in the park, rides with RideId::Null are considered free.
        std::array<Ride, Limits::kMaxRidesInPark> rides{};
        size_t ridesEndOfUsedRange{};
        RideRating::UpdateStates rideRatingUpdateStates;
        std::vector<TileElement> tileElements;

        std::vector<ScenerySelection> restrictedScenery;

        std::vector<PeepSpawn> peepSpawns;

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

        CheatsState cheats;
    };

    GameState_t& getGameState();
    void swapGameState(std::unique_ptr<GameState_t>& otherState);

    void gameStateInitAll(GameState_t& gameState, const TileCoordsXY& mapSize);
    void gameStateTick();
    void gameStateUpdateLogic();

} // namespace OpenRCT2
