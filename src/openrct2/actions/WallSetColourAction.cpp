/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
        const CoordsXYZD& loc, Drawing::Colour primaryColour, Drawing::Colour secondaryColour, Drawing::Colour tertiaryColour)
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
        res.errorTitle = STR_CANT_REPAINT_THIS;
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;

        res.expenditure = ExpenditureType::landscaping;

        if (!LocationValid(_loc))
        {
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
        }

        if (gLegacyScene != LegacyScene::scenarioEditor && !MapIsLocationInPark(_loc) && !gameState.cheats.sandboxMode)
        {
            return Result(Status::notOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }

        auto wallElement = MapGetWallElementAt(_loc);
        if (wallElement == nullptr)
        {
            LOG_ERROR(
                "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_WALL_ELEMENT_NOT_FOUND);
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
            return Result(Status::unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
        }

        if (!Drawing::colourIsValid(_primaryColour))
        {
            LOG_ERROR("Primary colour invalid: colour = %d", _primaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        if (!Drawing::colourIsValid(_secondaryColour))
        {
            LOG_ERROR("Secondary colour invalid: colour = %d", _secondaryColour);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, STR_ERR_INVALID_COLOUR);
        }
        if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
        {
            if (!Drawing::colourIsValid(_tertiaryColour))
            {
                LOG_ERROR("Tertiary colour invalid: colour = %d", _tertiaryColour);
                return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
            }
        }
        return res;
    }

    Result WallSetColourAction::Execute(GameState_t& gameState) const
    {
        auto res = Result();
        res.errorTitle = STR_CANT_REPAINT_THIS;
        res.position.x = _loc.x + 16;
        res.position.y = _loc.y + 16;
        res.position.z = _loc.z;
        res.expenditure = ExpenditureType::landscaping;

        auto wallElement = MapGetWallElementAt(_loc);
        if (wallElement == nullptr)
        {
            LOG_ERROR(
                "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z,
                _loc.direction);
            return Result(Status::invalidParameters, STR_CANT_REPAINT_THIS, kStringIdNone);
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
            return Result(Status::unknown, STR_CANT_REPAINT_THIS, kStringIdNone);
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
