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
#include "../../object/ObjectEntryManager.h"
#include "../../object/ObjectManager.h"
#include "../Map.h"

namespace OpenRCT2::World::MapGenerator
{


    static void placeScenery(
        const TileCoordsXY& loc, const std::optional<uint8_t> quadrant, const Rule::SceneryResultItem& sceneryItem)
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

    void placeScenery(const TileCoordsXY& loc, const Rule::SceneryResult& sceneryResult)
    {
        if (std::holds_alternative<Rule::SceneryResultItem>(sceneryResult))
        {
            auto& sceneryItem = std::get<Rule::SceneryResultItem>(sceneryResult);
            placeScenery(loc, std::nullopt, sceneryItem);
        }
        else if (std::holds_alternative<Rule::QuadSceneryItems>(sceneryResult))
        {
            auto& quadSceneryItems = std::get<Rule::QuadSceneryItems>(sceneryResult);
            for (int q = 0; q < 4; ++q)
            {
                auto& quadSceneryItem = quadSceneryItems[q];
                if (quadSceneryItem.has_value())
                {
                    placeScenery(loc, std::make_optional(q), quadSceneryItem.value());
                }
            }
        }
    }

    static std::optional<ObjectEntryIndex> lookupObjectEntryIdxByIdentifier(const std::string_view identifier)
    {
        auto& objectManager = GetContext()->GetObjectManager();
        ObjectEntryIndex idx = objectManager.GetLoadedObjectEntryIndex(identifier);
        return idx == kObjectEntryIndexNull ? std::nullopt : std::make_optional(idx);
    }

    void placeDebugSign(const DebugSign& debugSign)
    {
        auto banner = lookupObjectEntryIdxByIdentifier("rct2.scenery_large.ssig3");

        if (!banner.has_value())
        {
            LOG_WARNING("vertical banner not loaded");
            return;
        }

        CoordsXY position = debugSign.position.ToCoordsXY();
        auto groundHeight = TileElementHeight(position);
        auto waterHeight = TileElementWaterHeight(position);

        auto actionPlace = GameActions::LargeSceneryPlaceAction(
            CoordsXYZD{ position.x, position.y, std::max(groundHeight, waterHeight) + 2*kCoordsZStep, 0 }, banner.value(),
            debugSign.backgroundColour, debugSign.textColour, Drawing::Colour::brightPink);

        auto& gameState = getGameState();
        auto& park = gameState.park;

        auto queryPlaceResult = actionPlace.Query(gameState, park);
        if (queryPlaceResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "LargeSceneryPlaceAction query: %s - %s", queryPlaceResult.getErrorTitle().c_str(),
                queryPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto execPlaceResult = actionPlace.Execute(gameState, park);
        if (execPlaceResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "LargeSceneryPlaceAction exec: %s - %s", execPlaceResult.getErrorTitle().c_str(),
                execPlaceResult.getErrorMessage().c_str());
            return;
        }

        auto placeResultData = execPlaceResult.getData<GameActions::LargeSceneryPlaceActionResult>();

        auto actionSetSignName = GameActions::SignSetNameAction(placeResultData.bannerId, debugSign.text);

        auto querySetSignNameResult = actionSetSignName.Query(gameState, park);
        if (querySetSignNameResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "SignSetNameAction query: %s - %s", querySetSignNameResult.getErrorTitle().c_str(),
                querySetSignNameResult.getErrorMessage().c_str());
            return;
        }

        auto execSetSignNameResult = actionSetSignName.Execute(gameState, park);
        if (execSetSignNameResult.error != GameActions::Status::ok)
        {
            LOG_WARNING(
                "SignSetNameAction exec: %s - %s", execSetSignNameResult.getErrorTitle().c_str(),
                execSetSignNameResult.getErrorMessage().c_str());
        }

    }
} // namespace OpenRCT2::World::MapGenerator
