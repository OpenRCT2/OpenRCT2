/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MapChangeSizeAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../drawing/Drawing.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "../world/Map.h"
#include "../world/Park.h"

namespace OpenRCT2::GameActions
{
    MapChangeSizeAction::MapChangeSizeAction(const TileCoordsXY& targetSize)
        : MapChangeSizeAction(targetSize, TileCoordsXY())
    {
    }

    MapChangeSizeAction::MapChangeSizeAction(const TileCoordsXY& targetSize, const TileCoordsXY& shift)
        : _targetSize(targetSize)
        , _shift(shift)
    {
    }

    uint16_t MapChangeSizeAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void MapChangeSizeAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_targetSize);
        stream << DS_TAG(_shift);
    }

    Result MapChangeSizeAction::Query(GameState_t& gameState) const
    {
        if (_targetSize.x > kMaximumMapSizeTechnical || _targetSize.y > kMaximumMapSizeTechnical)
        {
            return Result(Status::invalidParameters, STR_CANT_INCREASE_MAP_SIZE_ANY_FURTHER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        if (_targetSize.x < kMinimumMapSizeTechnical || _targetSize.y < kMinimumMapSizeTechnical)
        {
            return Result(Status::invalidParameters, STR_CANT_DECREASE_MAP_SIZE_ANY_FURTHER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        return Result();
    }

    Result MapChangeSizeAction::Execute(GameState_t& gameState) const
    {
        // Expand map
        while (_targetSize.x > gameState.mapSize.x)
        {
            gameState.mapSize.x++;
            MapExtendBoundarySurfaceX();
        }
        while (_targetSize.y > gameState.mapSize.y)
        {
            gameState.mapSize.y++;
            MapExtendBoundarySurfaceY();
        }

        // Shift the map (allows increasing the map at the 0,0 position
        ShiftMap(_shift);

        // Shrink map
        if (_targetSize.x < gameState.mapSize.x || _targetSize.y < gameState.mapSize.y)
        {
            gameState.mapSize = _targetSize;
            MapRemoveOutOfRangeElements();
        }

        auto* ctx = GetContext();
        auto& uiContext = ctx->GetUiContext();
        auto* windowManager = uiContext.GetWindowManager();

        auto& park = gameState.park;
        Park::UpdateSize(park);

        windowManager->BroadcastIntent(Intent(INTENT_ACTION_MAP));
        GfxInvalidateScreen();
        return Result();
    }

    void MapChangeSizeAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("targetSizeX", _targetSize.x);
        visitor.Visit("targetSizeY", _targetSize.y);
        visitor.Visit("shiftX", _shift.x);
        visitor.Visit("shiftY", _shift.y);
    }
} // namespace OpenRCT2::GameActions
