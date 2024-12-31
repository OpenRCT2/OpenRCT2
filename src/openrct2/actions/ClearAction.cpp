/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ClearAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../drawing/Drawing.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "../world/tile_element/LargeSceneryElement.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "FootpathRemoveAction.h"
#include "LargeSceneryRemoveAction.h"
#include "SmallSceneryRemoveAction.h"
#include "WallRemoveAction.h"

using namespace OpenRCT2;

ClearAction::ClearAction(MapRange range, ClearableItems itemsToClear)
    : _range(range)
    , _itemsToClear(itemsToClear)
{
}

void ClearAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_range);
    visitor.Visit("itemsToClear", _itemsToClear);
}

uint16_t ClearAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void ClearAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_range) << DS_TAG(_itemsToClear);
}

GameActions::Result ClearAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result ClearAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result ClearAction::CreateResult() const
{
    auto result = GameActions::Result();
    result.ErrorTitle = STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE;
    result.Expenditure = ExpenditureType::Landscaping;

    auto x = (_range.GetLeft() + _range.GetRight()) / 2 + 16;
    auto y = (_range.GetTop() + _range.GetBottom()) / 2 + 16;
    auto z = TileElementHeight({ x, y });
    result.Position = CoordsXYZ(x, y, z);

    return result;
}

GameActions::Result ClearAction::QueryExecute(bool executing) const
{
    auto result = CreateResult();

    auto noValidTiles = true;
    auto error = GameActions::Status::Ok;
    StringId errorMessage = STR_NONE;
    money64 totalCost = 0;

    auto validRange = ClampRangeWithinMap(_range);
    for (int32_t y = validRange.GetTop(); y <= validRange.GetBottom(); y += kCoordsXYStep)
    {
        for (int32_t x = validRange.GetLeft(); x <= validRange.GetRight(); x += kCoordsXYStep)
        {
            if (LocationValid({ x, y }) && MapCanClearAt({ x, y }))
            {
                auto cost = ClearSceneryFromTile({ x, y }, executing);
                if (cost != kMoney64Undefined)
                {
                    noValidTiles = false;
                    totalCost += cost;
                }
            }
            else
            {
                error = GameActions::Status::NotOwned;
                errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
            }
        }
    }

    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
    {
        ResetClearLargeSceneryFlag();
    }

    if (noValidTiles)
    {
        result.Error = error;
        result.ErrorMessage = errorMessage;
    }

    result.Cost = totalCost;
    return result;
}

money64 ClearAction::ClearSceneryFromTile(const CoordsXY& tilePos, bool executing) const
{
    // Pass down all flags.
    TileElement* tileElement = nullptr;
    money64 totalCost = 0;
    bool tileEdited;
    do
    {
        tileEdited = false;
        tileElement = MapGetFirstElementAt(tilePos);
        if (tileElement == nullptr)
            return totalCost;
        do
        {
            if (tileElement->IsGhost())
                continue;

            switch (tileElement->GetType())
            {
                case TileElementType::Path:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_FOOTPATH)
                    {
                        auto footpathRemoveAction = FootpathRemoveAction({ tilePos, tileElement->GetBaseZ() });
                        footpathRemoveAction.SetFlags(GetFlags());

                        auto res = executing ? GameActions::ExecuteNested(&footpathRemoveAction)
                                             : GameActions::QueryNested(&footpathRemoveAction);

                        if (res.Error == GameActions::Status::Ok)
                        {
                            totalCost += res.Cost;
                            tileEdited = executing;
                        }
                    }
                    break;
                case TileElementType::SmallScenery:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                    {
                        auto removeSceneryAction = SmallSceneryRemoveAction(
                            { tilePos, tileElement->GetBaseZ() }, tileElement->AsSmallScenery()->GetSceneryQuadrant(),
                            tileElement->AsSmallScenery()->GetEntryIndex());
                        removeSceneryAction.SetFlags(GetFlags());

                        auto res = executing ? GameActions::ExecuteNested(&removeSceneryAction)
                                             : GameActions::QueryNested(&removeSceneryAction);

                        if (res.Error == GameActions::Status::Ok)
                        {
                            totalCost += res.Cost;
                            tileEdited = executing;
                        }
                    }
                    break;
                case TileElementType::Wall:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_SMALL)
                    {
                        CoordsXYZD wallLocation = { tilePos, tileElement->GetBaseZ(), tileElement->GetDirection() };
                        auto wallRemoveAction = WallRemoveAction(wallLocation);
                        wallRemoveAction.SetFlags(GetFlags());

                        auto res = executing ? GameActions::ExecuteNested(&wallRemoveAction)
                                             : GameActions::QueryNested(&wallRemoveAction);

                        if (res.Error == GameActions::Status::Ok)
                        {
                            totalCost += res.Cost;
                            tileEdited = executing;
                        }
                    }
                    break;
                case TileElementType::LargeScenery:
                    if (_itemsToClear & CLEARABLE_ITEMS::SCENERY_LARGE)
                    {
                        auto removeSceneryAction = LargeSceneryRemoveAction(
                            { tilePos, tileElement->GetBaseZ(), tileElement->GetDirection() },
                            tileElement->AsLargeScenery()->GetSequenceIndex());
                        removeSceneryAction.SetFlags(GetFlags() | GAME_COMMAND_FLAG_TRACK_DESIGN);

                        auto res = executing ? GameActions::ExecuteNested(&removeSceneryAction)
                                             : GameActions::QueryNested(&removeSceneryAction);

                        if (res.Error == GameActions::Status::Ok)
                        {
                            totalCost += res.Cost;
                            tileEdited = executing;
                        }
                    }
                    break;
                default:
                    break;
            }
        } while (!tileEdited && !(tileElement++)->IsLastForTile());
    } while (tileEdited);

    return totalCost;
}

void ClearAction::ResetClearLargeSceneryFlag()
{
    auto& gameState = GetGameState();
    // TODO: Improve efficiency of this
    for (int32_t y = 0; y < gameState.MapSize.y; y++)
    {
        for (int32_t x = 0; x < gameState.MapSize.x; x++)
        {
            auto tileElement = MapGetFirstElementAt(TileCoordsXY{ x, y });
            do
            {
                if (tileElement == nullptr)
                    break;
                if (tileElement->GetType() == TileElementType::LargeScenery)
                {
                    tileElement->AsLargeScenery()->SetIsAccounted(false);
                }
            } while (!(tileElement++)->IsLastForTile());
        }
    }
}

bool ClearAction::MapCanClearAt(const CoordsXY& location)
{
    return (gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || GetGameState().Cheats.sandboxMode
        || MapIsLocationOwnedOrHasRights(location);
}
