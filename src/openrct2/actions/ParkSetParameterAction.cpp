/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetParameterAction.h"

#include "../interface/Window.h"
#include "../ride/ShopItem.h"
#include "../util/Util.h"
#include "../world/Park.h"

ParkSetParameterAction::ParkSetParameterAction(ParkParameter parameter, uint64_t value)
    : _parameter(parameter)
    , _value(value)
{
}

void ParkSetParameterAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("parameter", _parameter);
    visitor.Visit("value", _value);
}

uint16_t ParkSetParameterAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetParameterAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_parameter) << DS_TAG(_value);
}

GameActions::Result ParkSetParameterAction::Query() const
{
    if (_parameter >= ParkParameter::Count)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto res = GameActions::Result();
    res.ErrorTitle = _ErrorTitles[EnumValue(_parameter)];
    return res;
}

GameActions::Result ParkSetParameterAction::Execute() const
{
    switch (_parameter)
    {
        case ParkParameter::Close:
            if (gParkFlags & PARK_FLAGS_PARK_OPEN)
            {
                gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
                window_invalidate_by_class(WindowClass::ParkInformation);
            }
            break;
        case ParkParameter::Open:
            if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
            {
                gParkFlags |= PARK_FLAGS_PARK_OPEN;
                window_invalidate_by_class(WindowClass::ParkInformation);
            }
            break;
        case ParkParameter::SamePriceInPark:
            gSamePriceThroughoutPark = _value;
            window_invalidate_by_class(WindowClass::Ride);
            break;
        default:
            return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    auto res = GameActions::Result();
    res.ErrorTitle = _ErrorTitles[EnumValue(_parameter)];
    return res;
}
