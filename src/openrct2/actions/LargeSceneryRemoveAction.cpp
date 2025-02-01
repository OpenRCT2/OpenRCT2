/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeSceneryRemoveAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../object/LargeSceneryEntry.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/LargeSceneryElement.h"

using namespace OpenRCT2;

LargeSceneryRemoveAction::LargeSceneryRemoveAction(const CoordsXYZD& location, uint16_t tileIndex)
    : _loc(location)
    , _tileIndex(tileIndex)
{
}

void LargeSceneryRemoveAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("tileIndex", _tileIndex);
}

uint16_t LargeSceneryRemoveAction::GetActionFlags() const
{
    return GameAction::GetActionFlags();
}

void LargeSceneryRemoveAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_tileIndex);
}

GameActions::Result LargeSceneryRemoveAction::Query() const
{
    auto res = GameActions::Result();

    const uint32_t flags = GetFlags();

    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = 0;

    TileElement* tileElement = FindLargeSceneryElement(_loc, _tileIndex);
    if (tileElement == nullptr)
    {
        LOG_ERROR("No large scenery element to remove at x = %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    // If we have a bugged scenery entry, do not touch the tile element.
    if (sceneryEntry == nullptr)
    {
        LOG_WARNING("Scenery entry at x = %d, y = %d not removed because it is an unknown object type", _loc.x, _loc.y);
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REMOVE_THIS, STR_UNKNOWN_OBJECT_TYPE);
    }

    auto rotatedOffsets = CoordsXYZ{ CoordsXY{ sceneryEntry->tiles[_tileIndex].offset }.Rotate(_loc.direction),
                                     sceneryEntry->tiles[_tileIndex].offset.z };

    auto firstTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedOffsets;

    bool calculate_cost = true;
    for (auto& tile : sceneryEntry->tiles)
    {
        auto currentTileRotatedOffset = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(_loc.direction), tile.offset.z };

        auto currentTile = CoordsXYZ{ firstTile.x, firstTile.y, firstTile.z } + currentTileRotatedOffset;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
        {
            if (GetGameState().Park.Flags & PARK_FLAGS_FORBID_TREE_REMOVAL)
            {
                if (sceneryEntry->HasFlag(LARGE_SCENERY_FLAG_IS_TREE))
                {
                    res.Error = GameActions::Status::NoClearance;
                    res.ErrorTitle = STR_CANT_REMOVE_THIS;
                    res.ErrorMessage = STR_FORBIDDEN_BY_THE_LOCAL_AUTHORITY;
                    return res;
                }
            }

            if (!MapIsLocationOwned({ currentTile.x, currentTile.y, currentTile.z }))
            {
                return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        if (!LocationValid(currentTile))
        {
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_OFF_EDGE_OF_MAP);
        }
        // Prevent duplicate costs when using the clear scenery tool that overlaps multiple large
        // scenery tile elements.
        if (flags & GAME_COMMAND_FLAG_TRACK_DESIGN)
        {
            if (tileElement->AsLargeScenery()->IsAccounted())
                calculate_cost = false;

            // Sets the flag to prevent this being counted in additional calls
            tileElement->AsLargeScenery()->SetIsAccounted(true);
        }
    }

    if (calculate_cost)
        res.Cost = sceneryEntry->removal_price;

    return res;
}

GameActions::Result LargeSceneryRemoveAction::Execute() const
{
    auto res = GameActions::Result();

    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = 0;

    TileElement* tileElement = FindLargeSceneryElement(_loc, _tileIndex);
    if (tileElement == nullptr)
    {
        LOG_ERROR("No large scenery element to remove at %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    // If we have a bugged scenery entry, do not touch the tile element.
    if (sceneryEntry == nullptr)
    {
        LOG_WARNING("Scenery entry at x = %d, y = %d not removed because it is an unknown object type", _loc.x, _loc.y);
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REMOVE_THIS, kStringIdNone);
    }

    tileElement->RemoveBannerEntry();

    auto rotatedFirstTile = CoordsXYZ{ CoordsXY{ sceneryEntry->tiles[_tileIndex].offset }.Rotate(_loc.direction),
                                       sceneryEntry->tiles[_tileIndex].offset.z };

    auto firstTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedFirstTile;

    for (auto& tile : sceneryEntry->tiles)
    {
        auto rotatedCurrentTile = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(_loc.direction), tile.offset.z };

        auto currentTile = CoordsXYZ{ firstTile.x, firstTile.y, firstTile.z } + rotatedCurrentTile;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
        {
            if (!MapIsLocationOwned({ currentTile.x, currentTile.y, currentTile.z }))
            {
                return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        auto* sceneryElement = FindLargeSceneryElement(currentTile, tile.index);
        if (sceneryElement == nullptr)
        {
            LOG_ERROR("Tile not found when trying to remove element!");
        }
        else
        {
            MapInvalidateTileFull(currentTile);
            TileElementRemove(sceneryElement);
        }
    }

    res.Cost = sceneryEntry->removal_price;

    return res;
}

TileElement* LargeSceneryRemoveAction::FindLargeSceneryElement(const CoordsXYZ& pos, int32_t sequenceIndex) const
{
    const bool isGhost = GetFlags() & GAME_COMMAND_FLAG_GHOST;
    for (auto* sceneryElement : TileElementsView<LargeSceneryElement>(pos))
    {
        // If we are removing ghost elements
        if (isGhost && sceneryElement->IsGhost() == false)
            continue;

        if (sceneryElement->GetDirection() != _loc.direction)
            continue;

        if (sceneryElement->GetSequenceIndex() != sequenceIndex)
            continue;

        if (sceneryElement->GetBaseZ() != pos.z)
            continue;

        return sceneryElement->as<TileElement>();
    }

    return nullptr;
}
