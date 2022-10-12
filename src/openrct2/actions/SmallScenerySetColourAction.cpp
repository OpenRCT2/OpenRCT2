/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallScenerySetColourAction.h"

#include "../Cheats.h"
#include "../OpenRCT2.h"
#include "../common.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/Localisation.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"

SmallScenerySetColourAction::SmallScenerySetColourAction(
    const CoordsXYZ& loc, uint8_t quadrant, ObjectEntryIndex sceneryType, uint8_t primaryColour, uint8_t secondaryColour,
    uint8_t tertiaryColour)
    : _loc(loc)
    , _quadrant(quadrant)
    , _sceneryType(sceneryType)
    , _primaryColour(primaryColour)
    , _secondaryColour(secondaryColour)
    , _tertiaryColour(tertiaryColour)
{
}

uint16_t SmallScenerySetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void SmallScenerySetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour);
}

GameActions::Result SmallScenerySetColourAction::Query() const
{
    return QueryExecute(false);
}

GameActions::Result SmallScenerySetColourAction::Execute() const
{
    return QueryExecute(true);
}

GameActions::Result SmallScenerySetColourAction::QueryExecute(bool isExecuting) const
{
    auto res = GameActions::Result();
    res.Expenditure = ExpenditureType::Landscaping;
    res.Position.x = _loc.x + 16;
    res.Position.y = _loc.y + 16;
    res.Position.z = _loc.z;
    res.ErrorTitle = STR_CANT_REPAINT_THIS;

    if (!LocationValid(_loc))
    {
        return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !gCheatsSandboxMode)
    {
        if (!MapIsLocationOwned(_loc))
        {
            return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }

    auto sceneryElement = MapGetSmallSceneryElementAt(_loc, _sceneryType, _quadrant);

    if (sceneryElement == nullptr)
    {
        log_error("Small scenery not found at: x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_NONE);
    }

    if ((GetFlags() & GAME_COMMAND_FLAG_GHOST) && !(sceneryElement->IsGhost()))
    {
        return res;
    }

    if (isExecuting)
    {
        sceneryElement->SetPrimaryColour(_primaryColour);
        sceneryElement->SetSecondaryColour(_secondaryColour);
        sceneryElement->SetTertiaryColour(_tertiaryColour);

        map_invalidate_tile_full(_loc);
    }

    return res;
}
