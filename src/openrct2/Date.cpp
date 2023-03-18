/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "localisation/Date.h"

#include "Date.h"
#include "Game.h"
#include "core/Guard.hpp"

#include <algorithm>

using namespace OpenRCT2;

constexpr int32_t MONTH_TICKS_INCREMENT = 4;
constexpr int32_t MASK_WEEK_TICKS = 0x3FFF;
constexpr int32_t MASK_FORTNIGHT_TICKS = 0x7FFF;
constexpr int32_t MASK_MONTH_TICKS = 0xFFFF;

// rct2: 0x00993988
const int16_t days_in_month[MONTH_COUNT] = {
    31, 30, 31, 30, 31, 31, 30, 31,
};

Date gDate;
RealWorldTime gRealTimeOfDay;

Date::Date(uint32_t monthsElapsed, uint16_t monthTicks)
    : _monthTicks(monthTicks)
    , _monthsElapsed(monthsElapsed)
{
}

Date Date::FromYMD(int32_t year, int32_t month, int32_t day)
{
    year = std::clamp(year, 0, MAX_YEAR - 1);
    month = std::clamp(month, 0, static_cast<int>(MONTH_COUNT) - 1);
    auto daysInMonth = days_in_month[month];
    day = std::clamp(day, 0, daysInMonth - 1);

    int32_t monthsElapsed = (year * MONTH_COUNT) + month;
    // Day
    int32_t monthTicks = 0;
    if (day != 0)
    {
        monthTicks = ((day << 16) / daysInMonth) + MONTH_TICKS_INCREMENT;
    }

    return Date(monthsElapsed, monthTicks);
}

void Date::Update()
{
    int32_t monthTicks = _monthTicks + MONTH_TICKS_INCREMENT;
    if (monthTicks > MASK_MONTH_TICKS)
    {
        _monthTicks = 0;
        _monthsElapsed++;
    }
    else
    {
        _monthTicks = static_cast<uint16_t>(monthTicks);
    }
}

uint16_t Date::GetMonthTicks() const
{
    return _monthTicks;
}

uint32_t Date::GetMonthsElapsed() const
{
    return _monthsElapsed;
}

int32_t Date::GetDay() const
{
    auto month = GetMonth();
    auto daysInMonth = GetDaysInMonth(month);
    return ((_monthTicks * daysInMonth) >> 16) & 0xFF;
}

int32_t Date::GetMonth() const
{
    return _monthsElapsed % MONTH_COUNT;
}

int32_t Date::GetYear() const
{
    return _monthsElapsed / MONTH_COUNT;
}

bool Date::IsDayStart() const
{
    if (_monthTicks < 4)
    {
        return false;
    }
    int32_t prevMonthTick = _monthTicks - 4;
    int32_t currentMonth = GetMonth();
    int32_t currentDaysInMonth = GetDaysInMonth(currentMonth);
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

int32_t Date::GetDaysInMonth(int32_t month)
{
    Guard::ArgumentInRange(month, 0, MONTH_COUNT - 1);

    return days_in_month[month];
}

int32_t DateGetMonth(int32_t months)
{
    return months % MONTH_COUNT;
}

int32_t DateGetYear(int32_t months)
{
    return months / MONTH_COUNT;
}

int32_t DateGetTotalMonths(int32_t month, int32_t year)
{
    return (year - 1) * MONTH_COUNT + month;
}

/**
 *
 *  rct2: 0x006C4494
 */
void DateReset()
{
    gDate = OpenRCT2::Date();
    gCurrentRealTimeTicks = 0;
}

void DateUpdateRealTimeOfDay()
{
    time_t timestamp = time(nullptr);
    struct tm* now = localtime(&timestamp);

    gRealTimeOfDay.second = now->tm_sec;
    gRealTimeOfDay.minute = now->tm_min;
    gRealTimeOfDay.hour = now->tm_hour;
}
