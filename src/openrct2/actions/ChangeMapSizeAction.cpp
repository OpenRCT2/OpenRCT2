/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
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

ChangeMapSizeAction::ChangeMapSizeAction(const int32_t targetSize)
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

GameActions::Result::Ptr ChangeMapSizeAction::Query() const
{
    if (_targetSize >= MAXIMUM_MAP_SIZE_TECHNICAL)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_INCREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
    }
    if (_targetSize < 16)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_CANT_DECREASE_MAP_SIZE_ANY_FURTHER, STR_NONE);
    }
    return GameActions::Result();
}

GameActions::Result::Ptr ChangeMapSizeAction::Execute() const
{
    while (gMapSize != _targetSize)
    {
        if (_targetSize < gMapSize)
        {
            gMapSize--;
            map_remove_out_of_range_elements();
        }
        else
        {
            gMapSize++;
            map_extend_boundary_surface();
        }
    }

    auto* ctx = OpenRCT2::GetContext();
    auto uiContext = ctx->GetUiContext();
    auto* windowManager = uiContext->GetWindowManager();

    windowManager->BroadcastIntent(Intent(INTENT_ACTION_MAP));
    gfx_invalidate_screen();
    return GameActions::Result();
}

void ChangeMapSizeAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("targetSize", _targetSize);
}
