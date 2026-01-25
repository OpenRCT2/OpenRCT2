/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "LargeScenerySetColourAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/LargeSceneryEntry.h"
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "../world/tile_element/LargeSceneryElement.h"

namespace OpenRCT2::GameActions
{
    LargeScenerySetColourAction::LargeScenerySetColourAction(
        const CoordsXYZD& loc, uint8_t tileIndex, Drawing::Colour primaryColour, Drawing::Colour secondaryColour,
        Drawing::Colour tertiaryColour)
        : _loc(loc)
        , _tileIndex(tileIndex)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
        , _tertiaryColour(tertiaryColour)
    {
    }

    void LargeScenerySetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("tileIndex", _tileIndex);
        visitor.Visit("primaryColour", _primaryColour);
        visitor.Visit("secondaryColour", _secondaryColour);
        visitor.Visit("tertiaryColour", _tertiaryColour);
    }

    uint16_t LargeScenerySetColourAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void LargeScenerySetColourAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_tileIndex) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
               << DS_TAG(_tertiaryColour);
    }

    Result LargeScenerySetColourAction::Query(GameState_t& gameState) const
    {
        return QueryExecute(gameState, false);
    }

    Result LargeScenerySetColourAction::Execute(GameState_t& gameState) const
    {
        return QueryExecute(gameState, true);
    }

    Result LargeScenerySetColourAction::QueryExecute(GameState_t& gameState, bool isExecuting) const
    {
        auto res = Result();
        res.expenditure = ExpenditureType::landscaping;
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = TileElementHeight(_loc);
        res.errorTitle = STR_CANT_REPAINT_THIS;

        auto mapSizeMax = GetMapSizeMaxXY();
        if (_loc.x < 0 || _loc.y < 0 || _loc.x > mapSizeMax.x || _loc.y > mapSizeMax.y)
        {
            LOG_ERROR("Invalid x / y coordinates: x = %d, y = %d", _loc.x, _loc.y);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        if (!Drawing::colourIsValid(_primaryColour))
        {
            LOG_ERROR("Invalid primary colour %u", _primaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        if (!Drawing::colourIsValid(_secondaryColour))
        {
            LOG_ERROR("Invalid secondary colour %u", _secondaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        if (!Drawing::colourIsValid(_tertiaryColour))
        {
            LOG_ERROR("Invalid tertiary colour %u", _tertiaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }

        auto largeElement = MapGetLargeScenerySegment(_loc, _tileIndex);

        if (largeElement == nullptr)
        {
            LOG_ERROR(
                "Could not find large scenery at: x = %d, y = %d, z = %d, direction = %d, tileIndex = %u", _loc.x, _loc.y,
                _loc.z, _loc.direction, _tileIndex);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        if ((GetFlags().has(CommandFlag::ghost)) && !(largeElement->IsGhost()))
        {
            return res;
        }

        auto* sceneryEntry = largeElement->GetEntry();

        if (sceneryEntry == nullptr)
        {
            LOG_ERROR("Scenery element doesn't have scenery entry");
            return Result(Status::unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
        }
        // Work out the base tile coordinates (Tile with index 0)
        auto rotatedBaseCoordsOffset = CoordsXYZ{ CoordsXY{ sceneryEntry->tiles[_tileIndex].offset }.Rotate(_loc.direction),
                                                  sceneryEntry->tiles[_tileIndex].offset.z };

        auto baseTile = CoordsXYZ{ _loc.x, _loc.y, _loc.z } - rotatedBaseCoordsOffset;

        for (auto& tile : sceneryEntry->tiles)
        {
            // Work out the current tile coordinates
            auto rotatedTileCoords = CoordsXYZ{ CoordsXY{ tile.offset }.Rotate(_loc.direction), tile.offset.z };
            auto currentTile = CoordsXYZ{ baseTile.x, baseTile.y, baseTile.z } + rotatedTileCoords;

            if (gLegacyScene != LegacyScene::scenarioEditor && !gameState.cheats.sandboxMode)
            {
                if (!MapIsLocationOwned(currentTile))
                {
                    return Result(Status::notOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
                }
            }

            if (!LocationValid(currentTile))
            {
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
            }

            auto tileElement = MapGetLargeScenerySegment({ currentTile.x, currentTile.y, _loc.z, _loc.direction }, tile.index);

            if (tileElement == nullptr)
            {
                LOG_ERROR(
                    "Large scenery element not found at: x = %d, y = %d, z = %d, direction = %d", _loc.x, _loc.y, _loc.z,
                    _loc.direction);
                return Result(Status::unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
            if (isExecuting)
            {
                tileElement->SetPrimaryColour(_primaryColour);
                tileElement->SetSecondaryColour(_secondaryColour);
                tileElement->SetTertiaryColour(_tertiaryColour);

                MapInvalidateTileFull(currentTile);
            }
        }
        return res;
    }
} // namespace OpenRCT2::GameActions
