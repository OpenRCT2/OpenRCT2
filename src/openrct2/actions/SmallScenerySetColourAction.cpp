/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "SmallScenerySetColourAction.h"

#include "../Cheats.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/MemoryStream.h"
#include "../interface/Window.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ride/Ride.h"
#include "../ride/TrackDesign.h"
#include "../world/MapAnimation.h"
#include "../world/Park.h"
#include "../world/tile_element/SmallSceneryElement.h"

using namespace OpenRCT2;

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

void SmallScenerySetColourAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit(_loc);
    visitor.Visit("quadrant", _quadrant);
    visitor.Visit("sceneryType", _sceneryType);
    visitor.Visit("primaryColour", _primaryColour);
    visitor.Visit("secondaryColour", _secondaryColour);
    visitor.Visit("tertiaryColour", _tertiaryColour);
}

uint16_t SmallScenerySetColourAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void SmallScenerySetColourAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);

    stream << DS_TAG(_loc) << DS_TAG(_quadrant) << DS_TAG(_sceneryType) << DS_TAG(_primaryColour) << DS_TAG(_secondaryColour)
           << DS_TAG(_tertiaryColour);
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
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_REPAINT_THIS, STR_OFF_EDGE_OF_MAP);
    }

    if (!(gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) && !GetGameState().Cheats.sandboxMode)
    {
        if (!MapIsLocationOwned(_loc))
        {
            return GameActions::Result(GameActions::Status::NotOwned, STR_CANT_REPAINT_THIS, STR_LAND_NOT_OWNED_BY_PARK);
        }
    }

    auto sceneryElement = MapGetSmallSceneryElementAt(_loc, _sceneryType, _quadrant);

    if (sceneryElement == nullptr)
    {
        LOG_ERROR("Small scenery not found at: x = %d, y = %d, z = %d", _loc.x, _loc.y, _loc.z);
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

        MapInvalidateTileFull(_loc);
    }

    return res;
}
