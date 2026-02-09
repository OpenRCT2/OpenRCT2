/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkSetDateAction.h"

#include "../Context.h"
#include "../Diagnostic.h"
#include "../GameState.h"
#include "../core/MemoryStream.h"
#include "../localisation/StringIds.h"
#include "../management/Finance.h"
#include "../ui/WindowManager.h"
#include "../windows/Intent.h"

namespace OpenRCT2::GameActions
{
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
        return GameAction::GetActionFlags() | Flags::AllowWhilePaused;
    }

    void ParkSetDateAction::Serialise(DataSerialiser& stream)
    {
        GameAction::Serialise(stream);
        stream << DS_TAG(_year) << DS_TAG(_month) << DS_TAG(_day);
    }

    Result ParkSetDateAction::Query(GameState_t& gameState) const
    {
        if (_year < 0 || _year >= kMaxYear)
        {
            LOG_ERROR("Invalid park date year %d", _year);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        else if (_month < 0 || _month >= MONTH_COUNT)
        {
            LOG_ERROR("Invalid park date month %d", _year);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }
        else if (_day < 0 || _day >= 31)
        {
            LOG_ERROR("Invalid park date day %d", _year);
            return Result(Status::invalidParameters, STR_ERR_INVALID_PARAMETER, STR_ERR_VALUE_OUT_OF_RANGE);
        }

        return Result();
    }

    Result ParkSetDateAction::Execute(GameState_t& gameState) const
    {
        gameState.date = Date::FromYMD(_year, _month, _day);
        return Result();
    }
} // namespace OpenRCT2::GameActions
