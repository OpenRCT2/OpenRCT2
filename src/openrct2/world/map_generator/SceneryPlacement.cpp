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
#include "../Map.h"
#include "MapHelpers.h"

namespace OpenRCT2::World::MapGenerator
{
    static CoordsXYZ xyzFrom(const TileCoordsXY& loc)
    {
        const auto coords = loc.ToCoordsXY();
        const auto groundHeight = TileElementHeight(coords);
        const auto waterHeight = TileElementWaterHeight(coords);
        return CoordsXYZ{ coords.x, coords.y, std::max(groundHeight, waterHeight) };
    }

    static void placeSceneryWall(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem)
    {
        auto action = GameActions::WallPlaceAction(
            sceneryItem.index, xyzFrom(loc), sceneryItem.direction,
            sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryPlaceResult = action.Query(gameState, park);
        if (queryPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "WallPlaceAction query: %s - %s", queryPlaceResult.getErrorTitle().c_str(),
                queryPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto execPlaceResult = action.Execute(gameState, park);
        if (execPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "WallPlaceAction exec: %s - %s", execPlaceResult.getErrorTitle().c_str(),
                execPlaceResult.getErrorMessage().c_str());
            return;
        }
    }

    static void placeSceneryLarge(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem)
    {
        auto action = GameActions::LargeSceneryPlaceAction(
           CoordsXYZD{ xyzFrom(loc), sceneryItem.direction }, sceneryItem.index,
            sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryPlaceResult = action.Query(gameState, park);
        if (queryPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "LargeSceneryPlaceAction query: %s - %s", queryPlaceResult.getErrorTitle().c_str(),
                queryPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto execPlaceResult = action.Execute(gameState, park);
        if (execPlaceResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "LargeSceneryPlaceAction exec: %s - %s", execPlaceResult.getErrorTitle().c_str(),
                execPlaceResult.getErrorMessage().c_str());
            return;
        }
    }

    static void placeScenerySmall(const TileCoordsXY& loc, const Rule::SceneryResultItem& sceneryItem, const std::optional<uint8_t> quadrant)
    {
        auto coords = loc.ToCoordsXY();
        auto action = GameActions::SmallSceneryPlaceAction(
            CoordsXYZD{ coords.x, coords.y, 0, sceneryItem.direction }, quadrant.value_or(0), sceneryItem.index,
            sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryResult = action.Query(gameState, park);
        if (queryResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "SmallSceneryPlaceAction query: %s - %s", queryResult.getErrorTitle().c_str(),
                queryResult.getErrorMessage().c_str());
            return;
        }

        auto execResult = action.Execute(gameState, park);
        if (execResult.error != GameActions::Status::ok)
        {
            LOG_VERBOSE(
                "SmallSceneryPlaceAction exec: %s - %s", execResult.getErrorTitle().c_str(),
                execResult.getErrorMessage().c_str());
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
