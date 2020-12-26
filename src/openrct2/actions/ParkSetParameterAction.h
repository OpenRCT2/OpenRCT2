/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "GameAction.h"

enum class ParkParameter : uint8_t
{
    Close,
    Open,
    SamePriceInPark,
    Count
};

DEFINE_GAME_ACTION(ParkSetParameterAction, GameCommand::SetParkOpen, GameActions::Result)
{
private:
    ParkParameter _parameter{ ParkParameter::Count };
    uint64_t _value{};

    constexpr static rct_string_id _ErrorTitles[] = { STR_CANT_CLOSE_PARK, STR_CANT_OPEN_PARK, STR_NONE, STR_NONE };

public:
    ParkSetParameterAction() = default;
    ParkSetParameterAction(ParkParameter parameter, uint64_t value = 0);

    void AcceptParameters(GameActionParameterVisitor & visitor) override; 
    uint16_t GetActionFlags() const override;

    void Serialise(DataSerialiser & stream) override;
    GameActions::Result::Ptr Query() const override;
    GameActions::Result::Ptr Execute() const override;
};
