/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../world/Scenery.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(LargeScenerySetColourAction, GAME_COMMAND_SET_LARGE_SCENERY_COLOUR, GameActionResult)
{
private:
    CoordsXYZD _loc;
    uint8_t _tileIndex;
    uint8_t _primaryColour;
    uint8_t _secondaryColour;

public:
    LargeScenerySetColourAction() = default;

    LargeScenerySetColourAction(const CoordsXYZD& loc, uint8_t tileIndex, uint8_t primaryColour, uint8_t secondaryColour)
        : _loc(loc)
        , _tileIndex(tileIndex)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_tileIndex) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour);
    }

    GameActionResult::Ptr Query() const override
    {
        return QueryExecute(false);
    }

    GameActionResult::Ptr Execute() const override
    {
        return QueryExecute(true);
    }

private:
    GameActionResult::Ptr QueryExecute(bool isExecuting) const
    {
        auto res = MakeResult();
        res->Expenditure = ExpenditureType::Landscaping;
        res->Position.x = _loc.x + 16;
        res->Position.y = _loc.y + 16;
        res->Position.z = tile_element_height(_loc);
        res->ErrorTitle = STR_CANT_REPAINT_THIS;

        if (_loc.x < 0 || _loc.y < 0 || _loc.x > gMapSizeMaxXY || _loc.y > gMapSizeMaxXY)
        {
            log_error("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if (_primaryColour > 31)
        {
            log_error("Invalid primary colour: colour = %u", _primaryColour);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if (_secondaryColour > 31)
        {
            log_error("Invalid primary colour: colour = %u", _secondaryColour);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        auto largeElement = map_get_large_scenery_segment(_loc, _tileIndex);

        if (largeElement == nullptr)
        {
            log_error(
                "Could not find large scenery at: x = %d, y = %d, z = %d, direction = %d, tileIndex = %u", _loc.x, _loc.y,
                _loc.z, _loc.direction, _tileIndex);
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_CANT_REPAINT_THIS);
        }

        if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(largeElement->IsGhost()))
        {
            return res;
        }

        rct_scenery_entry* sceneryEntry = largeElement->GetEntry();

        if (sceneryEntry == nullptr)
        {
            log_error("Could not find scenery object. type = %u", largeElement->GetEntryIndex());
            return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_REPAINT_THIS);
        }
        // Work out the base tile coordinates (Tile with index 0)
        auto rotatedBaseCoordsOffset = CoordsXYZ{ CoordsXY{ sceneryEntry->large_scenery.tiles[_tileIndex].x_offset,
                                                            sceneryEntry->large_scenery.tiles[_tileIndex].y_offset }
                                                      .Rotate(_loc.direction),
                                                  sceneryEntry->large_scenery.tiles[_tileIndex].z_offset };

        auto baseTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedBaseCoordsOffset;

        auto i = 0;
        for (auto tile = sceneryEntry->large_scenery.tiles; tile->x_offset != -1; ++tile, ++i)
        {
            // Work out the current tile coordinates
            auto rotatedTileCoords = CoordsXYZ{ CoordsXY{ tile->x_offset, tile->y_offset }.Rotate(_loc.direction),
                                                tile->z_offset };
            auto currentTile = CoordsXYZ{ baseTile.x, baseTile.y, baseTile.z } + rotatedTileCoords;

            if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
            {
                if (!map_is_location_owned(currentTile))
                {
                    return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
                }
            }

            if (!LocationValid(currentTile))
            {
                return MakeResult(GA_ERROR::NOT_OWNED, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
            }

            auto tileElement = map_get_large_scenery_segment({ currentTile.x, currentTile.y, _loc.z, _loc.direction }, i);

            if (tileElement == nullptr)
            {
                log_error(
                    "Large scenery element not found at: x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z,
                    _loc.direction);
                return MakeResult(GA_ERROR::UNKNOWN, STR_CANT_REPAINT_THIS);
            }
            if (isExecuting)
            {
                tileElement->SetPrimaryColour(_primaryColour);
                tileElement->SetSecondaryColour(_secondaryColour);

                map_invalidate_tile_full(currentTile);
            }
        }
        return res;
    }
};
