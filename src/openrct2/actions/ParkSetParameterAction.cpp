/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

void ParkSetParameterAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_parameter) << DS_TAG(_value);
}

GameActions::Result::Ptr ParkSetParameterAction::Query() const
{
    if (_parameter >= ParkParameter::Count)
    {
        return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    auto res = MakeResult();
    res->ErrorTitle = _ErrorTitles[EnumValue(_parameter)];
    return res;
}

GameActions::Result::Ptr ParkSetParameterAction::Execute() const
{
    switch (_parameter)
    {
        case ParkParameter::Close:
            if (gParkFlags & PARK_FLAGS_PARK_OPEN)
            {
                gParkFlags &= ~PARK_FLAGS_PARK_OPEN;
                window_invalidate_by_class(WC_PARK_INFORMATION);
            }
            break;
        case ParkParameter::Open:
            if (!(gParkFlags & PARK_FLAGS_PARK_OPEN))
            {
                gParkFlags |= PARK_FLAGS_PARK_OPEN;
                window_invalidate_by_class(WC_PARK_INFORMATION);
            }
            break;
        case ParkParameter::SamePriceInPark:
            gSamePriceThroughoutPark = _value;
            window_invalidate_by_class(WC_RIDE);
            break;
        default:
            return MakeResult(GameActions::Status::InvalidParameters, STR_NONE);
    }

    auto res = MakeResult();
    res->ErrorTitle = _ErrorTitles[EnumValue(_parameter)];
    return res;
}