/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallSetColourAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../object/WallSceneryEntry.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/Banner.h"
#include "../world/Map.h"
#include "../world/Scenery.h"
#include "../world/tile_element/WallElement.h"

namespace OpenRCT2::GameActions
{
    WallSetColourAction::WallSetColourAction(
        const CoordsXYZD& loc, int32_t primaryColour, int32_t secondaryColour, int32_t tertiaryColour)
        : _loc(loc)
        , _primaryColour(primaryColour)
        , _secondaryColour(secondaryColour)
        , _tertiaryColour(tertiaryColour)
    {
    }

    void WallSetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit(_loc);
        visitor.Visit("primaryColour", _primaryColour);
        visitor.Visit("secondaryColour", _secondaryColour);
        visitor.Visit("tertiaryColour", _tertiaryColour);
    }

    uint16_t WallSetColourAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void WallSetColourAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);

        stream << DS_TAG(_loc) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour) << DS_TAG(_tertiaryColour);
    }

    Result WallSetColourAction::Query(GameState_t& gameState) const
    {
        auto res = Result();
        res.ErrorTitle = STR_CANT_REPAINT_THIS;
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;

        res.Expenditure = ExpenditureType::landscaping;

        if (!LocationValid(_loc))
        {
            return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (gLegacyScene != LegacyScene::scenarioEditor && !MapIsLocationInPark(_loc) && !getGameState().cheats.sandboxMode)
        {
            return Result(Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto wallElement = MapGetWallElementAt(_loc);
        if (wallElement == nullptr)
        {
            LOG_ERROR(
                "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_WALL_ELEMENT_NOT_FOUND);
        }

        if ((GetFlags().has(CommandFlag::ghost)) && !(wallElement->IsGhost()))
        {
            return res;
        }

        auto* wallEntry = wallElement->GetEntry();
        if (wallEntry == nullptr)
        {
            LOG_ERROR(
                "Wall element does not have wall entry at x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::Unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        if (_primaryColour >= COLOUR_COUNT)
        {
            LOG_ERROR("Primary colour invalid: colour = %d", _primaryColour);
            return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        else if (_secondaryColour >= COLOUR_COUNT)
        {
            LOG_ERROR("Secondary colour invalid: colour = %d", _secondaryColour);
            return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        else if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
        {
            if (_tertiaryColour >= COLOUR_COUNT)
            {
                LOG_ERROR("Tertiary colour invalid: colour = %d", _tertiaryColour);
                return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
        }
        return res;
    }

    Result WallSetColourAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.ErrorTitle = STR_CANT_REPAINT_THIS;
        res.Position.x = _loc.x + 16;
        res.Position.y = _loc.y + 16;
        res.Position.z = _loc.z;
        res.Expenditure = ExpenditureType::landscaping;

        auto wallElement = MapGetWallElementAt(_loc);
        if (wallElement == nullptr)
        {
            LOG_ERROR(
                "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::InvalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        if ((GetFlags().has(CommandFlag::ghost)) && !(wallElement->IsGhost()))
        {
            return res;
        }

        auto* wallEntry = wallElement->GetEntry();
        if (wallEntry == nullptr)
        {
            LOG_ERROR(
                "Wall element does not have wall entry at x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::Unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        wallElement->SetPrimaryColour(_primaryColour);
        wallElement->SetSecondaryColour(_secondaryColour);

        if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
        {
            wallElement->SetTertiaryColour(_tertiaryColour);
        }
        MapInvalidateTileZoom1({ _loc, _loc.z, _loc.z + 72 });

        return res;
    }
} // namespace OpenRCT2::GameActions
