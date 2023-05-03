/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetDateAction.h"

#include "../Context.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/UiContext.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

ParkSetDateAction::ParkSetDateAction(int32_t year, int32_t month, int32_t day)
    : _year(year)
    , _month(month)
    , _day(day)
{
}

void ParkSetDateAction::AcceptParameters(GameActionParameterVisitor& visitor)
{
    visitor.Visit("year", _year);
    visitor.Visit("month", _month);
    visitor.Visit("day", _day);
}

uint16_t ParkSetDateAction::GetActionFlags() const
{
    return GameAction::GetActionFlags() | GameActions::Flags::AllowWhilePaused;
}

void ParkSetDateAction::Serialise(DataSerialiser& stream)
{
    GameAction::Serialise(stream);
    stream << DS_TAG(_year) << DS_TAG(_month) << DS_TAG(_day);
}

GameActions::Result ParkSetDateAction::Query() const
{
    if (_year < 0 || _year >= MAX_YEAR || _month < 0 || _month >= MONTH_COUNT || _day < 0 || _day >= 31)
    {
        return GameActions::Result(GameActions::Status::InvalidParameters, STR_NONE, STR_NONE);
    }

    return GameActions::Result();
}

GameActions::Result ParkSetDateAction::Execute() const
{
    OpenRCT2::GetContext()->GetGameState()->SetDate(OpenRCT2::Date::FromYMD(_year, _month, _day));
    return GameActions::Result();
}
