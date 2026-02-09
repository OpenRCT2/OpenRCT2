/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetVisibilityAction.h"

#include "../Diagnostic.h"
#include "../GameState.h"
#include "../world/TileElementsView.h"
#include "../world/tile_element/TrackElement.h"
#include "GameAction.hpp"

namespace OpenRCT2::GameActions
{
    RideSetVisibilityAction::RideSetVisibilityAction(RideId rideIndex, RideSetVisibilityType visiblity)
        : _rideIndex(rideIndex)
        , _visibility(visiblity)
    {
    }

    void RideSetVisibilityAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("ride", _rideIndex);
        visitor.Visit("visiblity", _visibility);
    }

    uint16_t RideSetVisibilityAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetVisibilityAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_rideIndex) << DS_TAG(_visibility);
    }

    Result RideSetVisibilityAction::Query(GameState_t& gameState) const
    {
        if (EnumValue(_visibility) >= 2)
        {
            LOG_ERROR("Invalid visibility type %d", _visibility);
            return Result(Status::invalidParameters, STR_CANT_CHANGE_THIS, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result RideSetVisibilityAction::Execute(GameState_t& gameState) const
    {
        for (int32_t y = 1; y <= gameState.mapSize.y; y++)
        {
            for (int32_t x = 1; x <= gameState.mapSize.x; x++)
            {
                const TileCoordsXY tileCoords{ x, y };
                for (auto* trackElement : TileElementsView<TrackElement>(tileCoords))
                {
                    if (trackElement->GetRideIndex() == _rideIndex)
                    {
                        trackElement->SetInvisible(_visibility == RideSetVisibilityType::invisible);
                        MapInvalidateTileFull(tileCoords.ToCoordsXY());
                    }
                }
            }
        }

        return Result();
    }
} // namespace OpenRCT2::GameActions
