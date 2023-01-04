/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ChangeMapSizeAction.h"

#include "../Context.h"
#include "../drawing/IDrawingEngine.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Park.h"

ChangeMapSizeAction::ChangeMapSizeAction(const TileCoordsXY& targetSize)
    : _targetSize(targetSize)
{
}

uint16_t ChangeMapSizeAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ChangeMapSizeAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_targetSize);
}

GameActions::Result ChangeMapSizeAction::Query() const
{
    if (_targetSize.x > MAXIMUM_MAP_SIZE_TECHNICAL || _targetSize.y > MAXIMUM_MAP_SIZE_TECHNICAL)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_INCREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
    }
    if (_targetSize.x < MINIMUM_MAP_SIZE_TECHNICAL || _targetSize.y < MINIMUM_MAP_SIZE_TECHNICAL)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_DECREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result ChangeMapSizeAction::Execute() const
{
    // Expand map
    while (_targetSize.x > gMapSize.x)
    {
        gMapSize.x++;
        MapExtendBoundarySurfaceX();
    }
    while (_targetSize.y > gMapSize.y)
    {
        gMapSize.y++;
        MapExtendBoundarySurfaceY();
    }

    // Shrink map
    if (_targetSize.x < gMapSize.x || _targetSize.y < gMapSize.y)
    {
        gMapSize = _targetSize;
        MapRemoveOutOfRangeElements();
    }

    auto* ctx = OpenRCT2::GetContext();
    auto uiContext = ctx->GetUiContext();
    auto* windowManager = uiContext->GetWindowManager();
    ParkCalculateSize();

    windowManager->BroadcastIntent(Intent(INTENT_ACTION_MAP));
    gfx_invalidate_screen();
    return GameActions::Result();
}

void ChangeMapSizeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("targetSizeX", _targetSize.x);
    visitor.Visit("targetSizeY", _targetSize.y);
}
