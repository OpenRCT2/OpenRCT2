/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeSceneryRemoveAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../world/Park.h"
#include "../world/SmallScenery.h"
#include "../world/TileElementsView.h"

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

    int32_t z = tile_element_height(_loc);
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = 0;

    TileElement* tileElement = FindLargeSceneryElement(_loc, _tileIndex);
    if (tileElement == nullptr)
    {
        log_warning("Invalid game command for scenery removal, x = %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    // If we have a bugged scenery entry, do not touch the tile element.
    if (sceneryEntry == nullptr)
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REMOVE_THIS, STR_NONE);

    auto rotatedOffsets = CoordsXYZ{
        CoordsXY{ sceneryEntry->tiles[_tileIndex].x_offset, sceneryEntry->tiles[_tileIndex].y_offset }.Rotate(_loc.direction),
        sceneryEntry->tiles[_tileIndex].z_offset
    };

    auto firstTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedOffsets;

    bool calculate_cost = true;
    for (int32_t i = 0; sceneryEntry->tiles[i].x_offset != -1; i++)
    {
        auto currentTileRotatedOffset = CoordsXYZ{
            CoordsXY{ sceneryEntry->tiles[i].x_offset, sceneryEntry->tiles[i].y_offset }.Rotate(_loc.direction),
            sceneryEntry->tiles[i].z_offset
        };

        auto currentTile = CoordsXYZ{ firstTile.x, firstTile.y, firstTile.z } + currentTileRotatedOffset;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_owned({ currentTile.x, currentTile.y, currentTile.z }))
            {
                return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        if (!LocationValid(currentTile))
        {
            return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
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
        res.Cost = sceneryEntry->removal_price * 10;

    return res;
}

GameActions::Result LargeSceneryRemoveAction::Execute() const
{
    auto res = GameActions::Result();

    int32_t z = tile_element_height(_loc);
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = z;
    res.Expenditure = ExpenditureType::Landscaping;
    res.Cost = 0;

    TileElement* tileElement = FindLargeSceneryElement(_loc, _tileIndex);
    if (tileElement == nullptr)
    {
        log_warning("Invalid game command for scenery removal, x = %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_CANT_REMOVE_THIS, STR_INVALID_SELECTION_OF_OBJECTS);
    }

    auto* sceneryEntry = tileElement->AsLargeScenery()->GetEntry();
    // If we have a bugged scenery entry, do not touch the tile element.
    if (sceneryEntry == nullptr)
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REMOVE_THIS, STR_NONE);

    tileElement->RemoveBannerEntry();

    auto rotatedFirstTile = CoordsXYZ{
        CoordsXY{ sceneryEntry->tiles[_tileIndex].x_offset, sceneryEntry->tiles[_tileIndex].y_offset }.Rotate(_loc.direction),
        sceneryEntry->tiles[_tileIndex].z_offset
    };

    auto firstTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedFirstTile;

    for (int32_t i = 0; sceneryEntry->tiles[i].x_offset != -1; i++)
    {
        auto rotatedCurrentTile = CoordsXYZ{
            CoordsXY{ sceneryEntry->tiles[i].x_offset, sceneryEntry->tiles[i].y_offset }.Rotate(_loc.direction),
            sceneryEntry->tiles[i].z_offset
        };

        auto currentTile = CoordsXYZ{ firstTile.x, firstTile.y, firstTile.z } + rotatedCurrentTile;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_owned({ currentTile.x, currentTile.y, currentTile.z }))
            {
                return GameActions::Result(GameActions::Status::NoClearance, STR_CANT_REMOVE_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        auto* sceneryElement = FindLargeSceneryElement(currentTile, i);
        if (sceneryElement != nullptr)
        {
            map_invalidate_tile_full(currentTile);
            tile_element_remove(sceneryElement);
        }
        else
        {
            log_error("Tile not found when trying to remove element!");
        }
    }

    res.Cost = sceneryEntry->removal_price * 10;

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
