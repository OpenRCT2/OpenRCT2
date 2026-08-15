/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideSetBreakdownAction.h"

#include "../../Diagnostic.h"
#include "../../ui/WindowManager.h"
#include "../../world/Map.h"

#include <limits>

namespace OpenRCT2::GameActions
{
    RideSetBreakdownAction::RideSetBreakdownAction(RideId rideIndex, Breakdown breakdown)
        : _rideIndex(rideIndex)
        , _breakdown(breakdown)
    {
    }

    void RideSetBreakdownAction::AcceptParameters(GameActionParameterVisitor& visitor)
    {
        visitor.Visit("ride", _rideIndex);

        auto breakdown = static_cast<int32_t>(EnumValue(_breakdown));
        visitor.Visit("breakdown", breakdown);
        _breakdown = breakdown >= 0 && breakdown <= std::numeric_limits<uint8_t>::max()
            ? static_cast<Breakdown>(breakdown)
            : static_cast<Breakdown>(kBreakdownCount);
    }

    uint16_t RideSetBreakdownAction::GetActionFlags() const
    {
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void RideSetBreakdownAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_rideIndex) << DS_TAG(_breakdown);
    }

    Result RideSetBreakdownAction::Query(GameState_t& gameState, Park::ParkData& park) const
    {
        const auto* ride = GetRide(_rideIndex);
        if (ride == nullptr)
        {
            LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
            return Result(Status::invalidParameters, STR_CANT_FORCE_BREAKDOWN, STR_ERR_RIDE_NOT_FOUND);
        }

        if (_breakdown != Breakdown::none && EnumValue(_breakdown) >= kBreakdownCount)
        {
            LOG_ERROR("Invalid breakdown %u", EnumValue(_breakdown));
            return Result(Status::invalidParameters, STR_CANT_FORCE_BREAKDOWN, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        if (_breakdown == Breakdown::none)
            return Result();

        if (ride->flags.hasAny(RideFlag::breakdownPending, RideFlag::brokenDown, RideFlag::crashed))
        {
            return Result(Status::disallowed, STR_CANT_FORCE_BREAKDOWN, STR_ERR_RIDE_ALREADY_BROKEN_DOWN);
        }

        if (ride->status == RideStatus::closed)
        {
            return Result(Status::disallowed, STR_CANT_FORCE_BREAKDOWN, STR_ERR_RIDE_IS_CLOSED);
        }

        if (!ride->getSupportedBreakdowns().has(_breakdown))
        {
            LOG_ERROR("Ride %u does not support breakdown %u", _rideIndex.ToUnderlying(), EnumValue(_breakdown));
            return Result(Status::invalidParameters, STR_CANT_FORCE_BREAKDOWN, STR_ERR_NO_BREAKDOWNS_AVAILABLE);
        }

        return Result();
    }

    Result RideSetBreakdownAction::Execute(GameState_t& gameState, Park::ParkData& park) const
    {
        auto* ride = GetRide(_rideIndex);

        if (_breakdown == Breakdown::none)
            ride->forceFixBreakdown();
        else
            RidePrepareBreakdown(*ride, _breakdown);

        auto res = Result();
        if (!ride->overallView.IsNull())
        {
            auto location = ride->overallView.ToTileCentre();
            res.position = { location, TileElementHeight(location) };
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->InvalidateByNumber(WindowClass::ride, _rideIndex.ToUnderlying());

        return res;
    }
} // namespace OpenRCT2::GameActions
