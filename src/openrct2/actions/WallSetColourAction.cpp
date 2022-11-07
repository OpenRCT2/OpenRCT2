/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "WallSetColourAction.h"

#include "../OpenRCT2.h"
#include "../management/Finance.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/Banner.h"
#include "../world/LargeScenery.h"
#include "../world/MapAnimation.h"
#include "../world/Scenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"

WallSetColourAction::WallSetColourAction(
    const CoordsXYZD& loc, int32_t primaryColour, int32_t secondaryColour, int32_t tertiaryColour)
    : _loc(loc)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

uint16_t WallSetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void WallSetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour) << DS_TAG(_tertiaryColour);
}

GameActions::Result WallSetColourAction::Query() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_REPAINT_THIS;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;

    res.Expenditure = ExpenditureType::Landscaping;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !MapIsLocationInPark(_loc) && !gCheatsSandboxMode)
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    auto wallElement = MapGetWallElementAt(_loc);
    if (wallElement == nullptr)
    {
        log_error(
            "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z, _loc.direction);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(wallElement->IsGhost()))
    {
        return res;
    }

    auto* wallEntry = wallElement->GetEntry();
    if (wallEntry == nullptr)
    {
        log_error("Could not find wall object");
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (_primaryColour > 31)
    {
        log_error("Primary colour invalid: colour = %d", _primaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (_secondaryColour > 31)
    {
        log_error("Secondary colour invalid: colour = %d", _secondaryColour);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
    {
        if (_tertiaryColour > 31)
        {
            log_error("Tertiary colour invalid: colour = %d", _tertiaryColour);
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
        }
    }
    return res;
}

GameActions::Result WallSetColourAction::Execute() const
{
    auto res = GameActions::Result();
    res.ErrorTitle = STR_CANT_REPAINT_THIS;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.Expenditure = ExpenditureType::Landscaping;

    auto wallElement = MapGetWallElementAt(_loc);
    if (wallElement == nullptr)
    {
        log_error(
            "Could not find wall element at: x = %d, y = %d, z = %d, direction = %u", _loc.x, _loc.y, _loc.z, _loc.direction);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(wallElement->IsGhost()))
    {
        return res;
    }

    auto* wallEntry = wallElement->GetEntry();
    if (wallEntry == nullptr)
    {
        log_error("Could not find wall object");
        return GameActions::Result(GameActions::Status::Unknown, STR_CANT_REPAINT_THIS, STR_NONE);
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
