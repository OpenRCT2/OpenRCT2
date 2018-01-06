#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#include "core/Guard.hpp"
#include "core/Math.hpp"
#include "localisation/Date.h"
#include "Date.h"

using namespace OpenRCT2;

constexpr sint32 MONTH_TICKS_INCREMENT  = 4;
constexpr sint32 MASK_WEEK_TICKS        = 0x3FFF;
constexpr sint32 MASK_FORTNIGHT_TICKS   = 0x7FFF;
constexpr sint32 MASK_MONTH_TICKS       = 0xFFFF;

Date::Date(uint32 monthsElapsed, uint16 monthTicks)
    : _monthTicks(monthTicks),
      _monthsElapsed(monthsElapsed)
{
}

Date Date::FromYMD(sint32 year, sint32 month, sint32 day)
{
    // Year, months
    Guard::ArgumentInRange(month, 0, MONTH_COUNT - 1);
    sint32 monthsElapsed = (year * MONTH_COUNT) + month;

    // Day
    sint32 monthTicks = 0;
    if (day != 0)
    {
        auto daysInMonth = GetDaysInMonth(month);
        day = Math::Clamp(0, day, daysInMonth - 1);
        monthTicks = (day << 16) / daysInMonth;
    }

    return Date(monthsElapsed, monthTicks);
}

void Date::Update()
{
    sint32 monthTicks = _monthTicks + MONTH_TICKS_INCREMENT;
    if (monthTicks > MASK_MONTH_TICKS)
    {
        _monthTicks = 0;
        _monthsElapsed++;
    }
    else
    {
        _monthTicks = (uint16)monthTicks;
    }
}

uint16 Date::GetMonthTicks() const
{
    return _monthTicks;
}

uint32 Date::GetMonthsElapsed() const
{
    return _monthsElapsed;
}

sint32 Date::GetDay() const
{
    auto month = GetMonth();
    auto daysInMonth = GetDaysInMonth(month);
    return ((_monthTicks * daysInMonth) >> 16) & 0xFF;
}

sint32 Date::GetMonth() const
{
    return _monthsElapsed % MONTH_COUNT;
}

sint32 Date::GetYear() const
{
    return _monthsElapsed / MONTH_COUNT;
}

bool Date::IsDayStart() const
{
    if (_monthTicks < 4)
    {
        return false;
    }
    sint32 prevMonthTick = _monthTicks - 4;
    sint32 currentMonth = GetMonth();
    sint32 currentDaysInMonth = GetDaysInMonth(currentMonth);
    return ((currentDaysInMonth * _monthTicks) >> 16 != (currentDaysInMonth * prevMonthTick) >> 16);
}

bool Date::IsWeekStart() const
{
    return (_monthTicks & MASK_WEEK_TICKS) == 0;
}

bool Date::IsFortnightStart() const
{
    return (_monthTicks & MASK_FORTNIGHT_TICKS) == 0;
}

bool Date::IsMonthStart() const
{
    return (_monthTicks == 0);
}

sint32 Date::GetDaysInMonth(sint32 month)
{
    Guard::ArgumentInRange(month, 0, MONTH_COUNT - 1);

    return days_in_month[month];
}
