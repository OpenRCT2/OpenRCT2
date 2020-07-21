/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../interface/Window.h"
#include "../ride/ShopItem.h"
#include "../world/Park.h"
#include "GameAction.h"

enum class ParkParameter : uint8_t
{
    Close,
    Open,
    SamePriceInPark,
    Count
};

DEFINE_GAME_ACTION(ParkSetParameterAction, GAME_COMMAND_SET_PARK_OPEN, GameActionResult)
{
private:
    uint8_t _parameter{ static_cast<uint8_t>(ParkParameter::Count) };
    uint64_t _value;

    constexpr static rct_string_id _ErrorTitles[] = { STR_CANT_CLOSE_PARK, STR_CANT_OPEN_PARK, STR_NONE, STR_NONE };

public:
    ParkSetParameterAction()
    {
    }
    ParkSetParameterAction(ParkParameter parameter, uint64_t value = 0)
        : _parameter(static_cast<uint8_t>(parameter))
        , _value(value)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_parameter) << DS_TAG(_value);
    }

    GameActionResult::Ptr Query() const override
    {
        if (_parameter >= static_cast<uint8_t>(ParkParameter::Count))
        {
            return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
        }

        auto res = MakeResult();
        res->ErrorTitle = _ErrorTitles[_parameter];
        return res;
    }

    GameActionResult::Ptr Execute() const override
    {
        switch (static_cast<ParkParameter>(_parameter))
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
                return MakeResult(GA_ERROR::INVALID_PARAMETERS, STR_NONE);
                break;
        }

        auto res = MakeResult();
        res->ErrorTitle = _ErrorTitles[_parameter];
        return res;
    }
};
