/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RideFreezeRatingAction.h"

#include "../Diagnostic.h"
#include "../ui/WindowManager.h"

using namespace OpenRCT2;

RideFreezeRatingAction::RideFreezeRatingAction(RideId rideIndex, RideRatingType type, ride_rating value)
    : _rideIndex(rideIndex)
    , _type(type)
    , _value(value)
{
}

void RideFreezeRatingAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("ride", _rideIndex);
    visitor.Visit("type", _type);
    visitor.Visit("value", _value);
}

void RideFreezeRatingAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_rideIndex) << DS_TAG(_type) << DS_TAG(_value);
}

GameActions::Result RideFreezeRatingAction::Query() const
{
    auto ride = GetRide(_rideIndex);
    if (ride == nullptr)
    {
        LOG_ERROR("Ride not found for rideIndex %u", _rideIndex.ToUnderlying());
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_RIDE_NOT_FOUND);
    }

    if (_value <= 0)
    {
        LOG_ERROR("Rating value must be positive: %u", _rideIndex.ToUnderlying());
        return GameActions::Result(
            GameActions::Status::InvalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
    }

    return GameActions::Result();
}

GameActions::Result RideFreezeRatingAction::Execute() const
{
    auto ride = GetRide(_rideIndex);

    switch (_type)
    {
        case RideRatingType::Excitement:
            ride->ratings.excitement = _value;
            break;
        case RideRatingType::Intensity:
            ride->ratings.intensity = _value;
            break;
        case RideRatingType::Nausea:
            ride->ratings.nausea = _value;
            break;
    }

    ride->lifecycle_flags |= RIDE_LIFECYCLE_FIXED_RATINGS;

    auto* windowMgr = Ui::GetWindowManager();
    windowMgr->InvalidateByNumber(WindowClass::Ride, _rideIndex.ToUnderlying());

    auto res = GameActions::Result();
    return res;
}
