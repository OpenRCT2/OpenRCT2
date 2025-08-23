/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SceneryPlacement.h"

#include "../../Diagnostic.h"
#include "../../GameState.h"
#include "../../actions/SmallSceneryPlaceAction.h"

namespace OpenRCT2::World::MapGenerator
{
    static void placeScenery(
        const TileCoordsXY& loc, const std::optional<uint8_t> quadrant, const Rule::SceneryResultItem& sceneryItem)
    {
        auto coords = loc.ToCoordsXY();
        auto action = SmallSceneryPlaceAction(
            CoordsXYZD{ coords.x, coords.y, 0, sceneryItem.direction }, quadrant.value_or(0), sceneryItem.index,
            sceneryItem.colours[0], sceneryItem.colours[1], sceneryItem.colours[2]);

        auto queryResult = action.Query();
        if (queryResult.Error != GameActions::Status::Ok)
        {
            LOG_VERBOSE(
                "SmallSceneryPlaceAction query: %s - %s", queryResult.GetErrorTitle().c_str(),
                queryResult.GetErrorMessage().c_str());
            return;
        }

        auto execResult = action.Execute();
        if (execResult.Error != GameActions::Status::Ok)
        {
            LOG_VERBOSE(
                "SmallSceneryPlaceAction exec: %s - %s", execResult.GetErrorTitle().c_str(),
                execResult.GetErrorMessage().c_str());
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
} // namespace OpenRCT2::World::MapGenerator
