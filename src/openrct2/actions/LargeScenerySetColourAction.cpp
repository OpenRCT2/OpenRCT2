/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeScenerySetColourAction.h"

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Scenery.h"

LargeScenerySetColourAction::LargeScenerySetColourAction(
    const CoordsXYZD& loc, uint8_t tileIndex, uint8_t primaryColour, uint8_t secondaryColour, uint8_t tertiaryColour)
    : _loc(loc)
    , _tileIndex(tileIndex)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

uint16_t LargeScenerySetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void LargeScenerySetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_tileIndex) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
           << DS_TAG(_tertiaryColour);
}

GameActions::Result LargeScenerySetColourAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result LargeScenerySetColourAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result LargeScenerySetColourAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = tile_element_height(_loc);
    res.ErrorTitle = STR_CANT_REPAINT_THIS;

    auto mapSizeMax = GetMapSizeMaxXY();
    if (_loc.x < 0 || _loc.y < 0 || _loc.x > mapSizeMax.x || _loc.y > mapSizeMax.y)
    {
        log_error("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (_primaryColour >= COLOUR_COUNT)
    {
        log_error("Invalid primary colour: colour = %u", _primaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (_secondaryColour >= COLOUR_COUNT)
    {
        log_error("Invalid secondary colour: colour = %u", _secondaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (_tertiaryColour >= COLOUR_COUNT)
    {
        log_error("Invalid tertiary colour: colour = %u", _tertiaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    auto largeElement = map_get_large_scenery_segment(_loc, _tileIndex);

    if (largeElement == nullptr)
    {
        log_error(
            "Could not find large scenery at: x = %d, y = %d, z = %d, direction = %d, tileIndex = %u", _loc.x, _loc.y, _loc.z,
            _loc.direction, _tileIndex);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(largeElement->IsGhost()))
    {
        return res;
    }

    auto* sceneryEntry = largeElement->GetEntry();

    if (sceneryEntry == nullptr)
    {
        log_error("Could not find scenery object. type = %u", largeElement->GetEntryIndex());
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_NONE);
    }
    // Work out the base tile coordinates (Tile with index 0)
    auto rotatedBaseCoordsOffset = CoordsXYZ{
        CoordsXY{ sceneryEntry->tiles[_tileIndex].x_offset, sceneryEntry->tiles[_tileIndex].y_offset }.Rotate(_loc.direction),
        sceneryEntry->tiles[_tileIndex].z_offset
    };

    auto baseTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedBaseCoordsOffset;

    auto i = 0;
    for (auto tile = sceneryEntry->tiles; tile->x_offset != -1; ++tile, ++i)
    {
        // Work out the current tile coordinates
        auto rotatedTileCoords = CoordsXYZ{ CoordsXY{ tile->x_offset, tile->y_offset }.Rotate(_loc.direction), tile->z_offset };
        auto currentTile = CoordsXYZ{ baseTile.x, baseTile.y, baseTile.z } + rotatedTileCoords;

        if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
        {
            if (!map_is_location_owned(currentTile))
            {
                return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }
        }

        if (!LocationValid(currentTile))
        {
            return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto tileElement = map_get_large_scenery_segment({ currentTile.x, currentTile.y, _loc.z, _loc.direction }, i);

        if (tileElement == nullptr)
        {
            log_error(
                "Large scenery element not found at: x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_NONE);
        }
        if (isExecuting)
        {
            tileElement->SetPrimaryColour(_primaryColour);
            tileElement->SetSecondaryColour(_secondaryColour);
            tileElement->SetTertiaryColour(_tertiaryColour);

            map_invalidate_tile_full(currentTile);
        }
    }
    return res;
}
