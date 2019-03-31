/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Context.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"
#include "GameAction.h"

DEFINE_GAME_ACTION(ParkSetDateAction, GAME_COMMAND_SET_DATE, GameActionResult)
{
private:
    int32_t _year;
    int32_t _month;
    int32_t _day;

public:
    ParkSetDateAction()
    {
    }
    ParkSetDateAction(int32_t year, int32_t month, int32_t day)
        : _year(year), _month(month), _day(day)
    {
    }

    uint16_t GetActionFlags() const override
    {
        return GameAction::GetActionFlags() | GA_FLAGS::ALLOW_WHILE_PAUSED;
    }

    void Serialise(DataSerialiser & stream) override
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_year) << DS_TAG(_month) << DS_TAG(_day);
    }

    GameActionResult::Ptr Query() const override
    {
        return MakeResult();
    }

    GameActionResult::Ptr Execute() const override
    {
        date_set(_year, _month, _day);
        return MakeResult();
    }
};
