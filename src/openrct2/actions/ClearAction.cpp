/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2::GameActions
{
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

    Result ClearAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result ClearAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result ClearAction::CreateResult() const
    {
        auto result = Result();
        result.errorTitle = STR_UNABLE_TO_REMOVE_ALL_SCENERY_FROM_HERE;
        result.expenditure = ExpenditureType::landscaping;

        auto x = (_range.GetX1() + _range.GetX2()) / 2 + 16;
        auto y = (_range.GetY1() + _range.GetY2()) / 2 + 16;
        auto z = TileElementHeight({ x, y });
        result.position = CoordsXYZ(x, y, z);

        return result;
    }

    Result ClearAction::QueryExecute(GameState_t& gameState, bool executing) const
    {
        auto result = CreateResult();

        auto noValidTiles = true;
        auto error = Status::ok;
        StringId errorMessage = kStringIdNone;
        money64 totalCost = 0;

        auto validRange = ClampRangeWithinMap(_range);
        for (int32_t y = validRange.GetY1(); y <= validRange.GetY2(); y += kCoordsXYStep)
        {
            for (int32_t x = validRange.GetX1(); x <= validRange.GetX2(); x += kCoordsXYStep)
            {
                if (LocationValid({ x, y }) && MapCanClearAt(gameState, { x, y }))
                {
                    auto cost = ClearSceneryFromTile({ x, y }, executing, gameState);
                    if (cost != kMoney64Undefined)
                    {
                        noValidTiles = false;
                        totalCost += cost;
                    }
                }
                else
                {
                    error = Status::notOwned;
                    errorMessage = STR_LAND_NOT_OWNED_BY_PARK;
                }
            }
        }

        if (_itemsToClear & CLEARABLE_ITEMS::kSceneryLarge)
        {
            ResetClearLargeSceneryFlag(gameState);
        }

        if (noValidTiles)
        {
            result.error = error;
            result.errorMessage = errorMessage;
        }

        result.cost = totalCost;
        return result;
    }

    money64 ClearAction::ClearSceneryFromTile(const CoordsXY& tilePos, bool executing, GameState_t& gameState) const
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
                        if (_itemsToClear & CLEARABLE_ITEMS::kSceneryFootpath)
                        {
                            auto footpathRemoveAction = FootpathRemoveAction({ tilePos, tileElement->GetBaseZ() });
                            footpathRemoveAction.SetFlags(GetFlags());

                            auto res = executing ? ExecuteNested(&footpathRemoveAction, gameState)
                                                 : QueryNested(&footpathRemoveAction, gameState);

                            if (res.error == Status::ok)
                            {
                                totalCost += res.cost;
                                tileEdited = executing;
                            }
                            else if (res.error == Status::insufficientFunds)
                            {
                                totalCost += res.cost;
                            }
                        }
                        break;
                    case TileElementType::SmallScenery:
                        if (_itemsToClear & CLEARABLE_ITEMS::kScenerySmall)
                        {
                            auto removeSceneryAction = SmallSceneryRemoveAction(
                                { tilePos, tileElement->GetBaseZ() }, tileElement->AsSmallScenery()->GetSceneryQuadrant(),
                                tileElement->AsSmallScenery()->GetEntryIndex());
                            removeSceneryAction.SetFlags(GetFlags());

                            auto res = executing ? ExecuteNested(&removeSceneryAction, gameState)
                                                 : QueryNested(&removeSceneryAction, gameState);

                            if (res.error == Status::ok)
                            {
                                totalCost += res.cost;
                                tileEdited = executing;
                            }
                            else if (res.error == Status::insufficientFunds)
                            {
                                totalCost += res.cost;
                            }
                        }
                        break;
                    case TileElementType::Wall:
                        if (_itemsToClear & CLEARABLE_ITEMS::kScenerySmall)
                        {
                            CoordsXYZD wallLocation = { tilePos, tileElement->GetBaseZ(), tileElement->GetDirection() };
                            auto wallRemoveAction = WallRemoveAction(wallLocation);
                            wallRemoveAction.SetFlags(GetFlags());

                            auto res = executing ? ExecuteNested(&wallRemoveAction, gameState)
                                                 : QueryNested(&wallRemoveAction, gameState);

                            if (res.error == Status::ok)
                            {
                                totalCost += res.cost;
                                tileEdited = executing;
                            }
                            else if (res.error == Status::insufficientFunds)
                            {
                                totalCost += res.cost;
                            }
                        }
                        break;
                    case TileElementType::LargeScenery:
                        if (_itemsToClear & CLEARABLE_ITEMS::kSceneryLarge)
                        {
                            auto removeSceneryAction = LargeSceneryRemoveAction(
                                { tilePos, tileElement->GetBaseZ(), tileElement->GetDirection() },
                                tileElement->AsLargeScenery()->GetSequenceIndex());
                            removeSceneryAction.SetFlags(GetFlags().with(CommandFlag::trackDesign));

                            auto res = executing ? ExecuteNested(&removeSceneryAction, gameState)
                                                 : QueryNested(&removeSceneryAction, gameState);

                            if (res.error == Status::ok)
                            {
                                totalCost += res.cost;
                                tileEdited = executing;
                            }
                            else if (res.error == Status::insufficientFunds)
                            {
                                totalCost += res.cost;
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

    void ClearAction::ResetClearLargeSceneryFlag(GameState_t& gameState)
    {
        // TODO: Improve efficiency of this
        for (int32_t y = 0; y < gameState.mapSize.y; y++)
        {
            for (int32_t x = 0; x < gameState.mapSize.x; x++)
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

    bool ClearAction::MapCanClearAt(const GameState_t& gameState, const CoordsXY& location)
    {
        return gLegacyScene == LegacyScene::scenarioEditor || gameState.cheats.sandboxMode
            || MapIsLocationOwnedOrHasRights(location);
    }
} // namespace OpenRCT2::GameActions
