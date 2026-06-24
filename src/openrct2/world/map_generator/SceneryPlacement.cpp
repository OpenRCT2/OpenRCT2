/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SceneryPlacement.h"

#include "../../Context.h"
#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../actions/scenery/LargeSceneryPlaceAction.h"
#include "../../actions/scenery/SignSetNameAction.h"
#include "../../actions/scenery/SmallSceneryPlaceAction.h"
#include "../../actions/scenery/WallPlaceAction.h"
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectManager.h"
#include "../../object/LargeSceneryEntry.h"
#include "../../object/SmallSceneryEntry.h"
#include "../../object/WallSceneryEntry.h"
#include "../Map.h"
#include "MapHelpers.h"

namespace OpenRCT2::World::MapGenerator
{
    static CoordsXYZ xyzFrom(const TileCoordsXY& loc, const int32_t stackElevation, const bool checkWaterHeight)
    {
        const auto coords = loc.ToCoordsXY();

        if (stackElevation > 0)
        {
            return CoordsXYZ{ coords.x, coords.y, stackElevation };
        }

        if (checkWaterHeight)
        {
            const auto waterHeight = TileElementWaterHeight(coords);

            if (waterHeight >= TileElementHeight(coords))
            {
                return CoordsXYZ{ coords.x, coords.y, waterHeight };
            }
        }

        return CoordsXYZ{ coords.x, coords.y, 0 };
    }

    static void placeSceneryWall(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem)
    {
        auto* entry = ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(sceneryItem.index);
        Guard::Assert(entry != nullptr);

        int32_t elevation = 0;

        for (int s = 0; s < sceneryItem.zRepeat.value_or(1); s++)
        {
            auto action = GameActions::WallPlaceAction(
                sceneryItem.index, xyzFrom(loc, elevation, true), sceneryItem.direction,
                sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

            auto& gameState = getGameState();
            auto& park = gameState.park;

            auto queryResult = action.Query(gameState, park);
            if (queryResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "WallPlaceAction query: %s - %s", queryResult.getErrorTitle().c_str(),
                    queryResult.getErrorMessage().c_str());
                continue;
            }

            auto execResult = action.Execute(gameState, park);
            if (execResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "WallPlaceAction exec: %s - %s", execResult.getErrorTitle().c_str(),
                    execResult.getErrorMessage().c_str());
                continue;
            }

            auto execResultData = execResult.getData<GameActions::WallPlaceActionResult>();
            elevation = execResultData.BaseHeight + entry->height * kCoordsZStep;
        }
    }

    static void placeSceneryLarge(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem)
    {
        auto* entry = ObjectEntryManager::GetObjectEntry<LargeSceneryEntry>(sceneryItem.index);
        Guard::Assert(entry != nullptr);

        uint8_t height = 0;
        for (const auto & tile : entry->tiles)
        {
            height = std::max(height, static_cast<uint8_t>( tile.zClearance));
        }

        int32_t elevation = 0;
        for (int s = 0; s < sceneryItem.zRepeat.value_or(1); s++)
        {
            auto action = GameActions::LargeSceneryPlaceAction(
               CoordsXYZD{ xyzFrom(loc, elevation, true), sceneryItem.direction }, sceneryItem.index,
                sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

            auto& gameState = getGameState();
            auto& park = gameState.park;

            auto queryResult = action.Query(gameState, park);
            if (queryResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "LargeSceneryPlaceAction query: %s - %s", queryResult.getErrorTitle().c_str(),
                    queryResult.getErrorMessage().c_str());
                continue;
            }

            auto execResult = action.Execute(gameState, park);
            if (execResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "LargeSceneryPlaceAction exec: %s - %s", execResult.getErrorTitle().c_str(),
                    execResult.getErrorMessage().c_str());
                continue;
            }

            auto execResultData = execResult.getData<GameActions::LargeSceneryPlaceActionResult>();
            elevation = execResultData.firstTileHeight + height;
        }
    }

    static void placeScenerySmall(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem, const std::optional<uint8_t> quadrant)
    {
        auto* entry = ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(sceneryItem.index);
        Guard::Assert(entry != nullptr);

        int32_t elevation = 0;
        for (int s = 0; s < sceneryItem.zRepeat.value_or(1); s++)
        {
            auto action = GameActions::SmallSceneryPlaceAction(
                CoordsXYZD{ xyzFrom(loc, elevation, false), sceneryItem.direction }, quadrant.value_or(0), sceneryItem.index,
                sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

            auto& gameState = getGameState();
            auto& park = gameState.park;

            auto queryResult = action.Query(gameState, park);
            if (queryResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "SmallSceneryPlaceAction query: %s - %s", queryResult.getErrorTitle().c_str(),
                    queryResult.getErrorMessage().c_str());
                continue;
            }

            auto execResult = action.Execute(gameState, park);
            if (execResult.error != GameActions::Status::ok)
            {
                LOG_VERBOSE(
                    "SmallSceneryPlaceAction exec: %s - %s", execResult.getErrorTitle().c_str(),
                    execResult.getErrorMessage().c_str());
                continue;
            }

            auto execResultData = execResult.getData<GameActions::SmallSceneryPlaceActionResult>();
            elevation = execResultData.BaseHeight + entry->height * kCoordsZStep;
        }
    }

    static void placeScenery(
        const TileCoordsXY& loc, const std::optional<uint8_t> quadrant, const Rule::SceneryResultItem& sceneryItem)
    {
        switch (sceneryItem.type)
        {
            case Rule::Small:
                placeScenerySmall(loc, sceneryItem, quadrant);
                break;
            case Rule::Large:
                placeSceneryLarge(loc, sceneryItem);
                break;
            case Rule::Wall:
                placeSceneryWall(loc, sceneryItem);
                break;
        }
    }

    void placeScenery(const TileCoordsXY& loc, const Rule::SceneryResult& sceneryResult)
    {
        if (std::holds_alternative<Rule::SceneryResultItem>(sceneryResult.items))
        {
            auto& sceneryItem = std::get<Rule::SceneryResultItem>(sceneryResult.items);
            placeScenery(loc, std::nullopt, sceneryItem);
        }
        else if (std::holds_alternative<Rule::QuadSceneryItems>(sceneryResult.items))
        {
            auto& quadSceneryItems = std::get<Rule::QuadSceneryItems>(sceneryResult.items);
            for (int q = 0; q < 4; ++q)
            {
                auto& quadSceneryItem = quadSceneryItems[q];
                if (quadSceneryItem.has_value())
                {
                    placeScenery(loc, std::make_optional(q), quadSceneryItem.value());
                }
            }
        }
        for (int d = 0; d < 4; ++d)
        {
            auto& edgeWall = sceneryResult.walls[d];
            if (edgeWall.has_value())
            {
                placeScenery(loc, std::nullopt, edgeWall.value());
            }
        }
    }

    static std::optional<ObjectEntryIndex> lookupObjectEntryIdxByIdentifier(const std::string_view identifier)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        ObjectEntryIndex idx = objectManager.GetLoadedObjectEntryIndex(identifier);
        return idx == kObjectEntryIndexNull ? std::nullopt : std::make_optional(idx);
    }

    void placeDebugSign(const MapGenCtx& context, const DebugSign& debugSign)
    {
        auto banner = lookupObjectEntryIdxByIdentifier("rct2.scenery_large.ssig3");

        if (!banner.has_value())
        {
            LOG_VERBOSE("vertical banner not loaded");
            return;
        }

        CoordsXY position = genCoordsToWorldCoords(context, debugSign.position).ToCoordsXY();
        auto groundHeight = TileElementHeight(position);
        auto waterHeight = TileElementWaterHeight(position);


        // TODO dedupe, use placeSceneryLarge
        auto actionPlace = GameActions::LargeSceneryPlaceAction(
            CoordsXYZD{ position.x, position.y, std::max(groundHeight, waterHeight) + 2*kCoordsZStep, 0 }, banner.value(),
            debugSign.backgroundColour, debugSign.textColour, Drawing::Colour::brightPink);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryPlaceResult = actionPlace.Query(gameState, park);
        if (queryPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "LargeSceneryPlaceAction query: %s - %s", queryPlaceResult.getErrorTitle().c_str(),
                queryPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto execPlaceResult = actionPlace.Execute(gameState, park);
        if (execPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "LargeSceneryPlaceAction exec: %s - %s", execPlaceResult.getErrorTitle().c_str(),
                execPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto placeResultData = execPlaceResult.getData<GameActions::LargeSceneryPlaceActionResult>();

        auto actionSetSignName = GameActions::SignSetNameAction(placeResultData.bannerId, debugSign.text);

        auto querySetSignNameResult = actionSetSignName.Query(gameState, park);
        if (querySetSignNameResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "SignSetNameAction query: %s - %s", querySetSignNameResult.getErrorTitle().c_str(),
                querySetSignNameResult.getErrorMessage().c_str());
            return;
        }

        auto execSetSignNameResult = actionSetSignName.Execute(gameState, park);
        if (execSetSignNameResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "SignSetNameAction exec: %s - %s", execSetSignNameResult.getErrorTitle().c_str(),
                execSetSignNameResult.getErrorMessage().c_str());
        }
    }
} // namespace OpenRCT2::World::MapGenerator
