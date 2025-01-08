/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Date.h"

#include "Context.h"
#include "Game.h"
#include "GameState.h"
#include "core/Guard.hpp"
#include "localisation/Localisation.Date.h"

using namespace OpenRCT2;

constexpr int32_t kMonthTicksIncrement = 4;
constexpr int32_t kMaskWeekTicks = 0x3FFF;
constexpr int32_t kMaskFortnightTicks = 0x7FFF;
constexpr int32_t kMaskMonthTicks = 0xFFFF;

// rct2: 0x00993988
static const int16_t days_in_month[MONTH_COUNT] = {
    31, 30, 31, 30, 31, 31, 30, 31,
};

RealWorldTime gRealTimeOfDay;

Date Date::FromYMD(int32_t year, int32_t month, int32_t day)
{
    year = std::clamp(year, 0, kMaxYear - 1);
    month = std::clamp(month, 0, static_cast<int>(MONTH_COUNT) - 1);
    auto daysInMonth = days_in_month[month];
    day = std::clamp(day, 0, daysInMonth - 1);

    uint32_t paramMonthsElapsed = (year * MONTH_COUNT) + month;
    // Day
    uint16_t paramMonthTicks = 0;
    if (day != 0)
    {
        paramMonthTicks = ((day << 16) / daysInMonth) + kMonthTicksIncrement;
    }

    return Date{ paramMonthsElapsed, paramMonthTicks };
}

uint16_t Date::GetMonthTicks() const
{
    return monthTicks;
}

uint32_t Date::GetMonthsElapsed() const
{
    return monthsElapsed;
}

int32_t Date::GetDay() const
{
    auto month = GetMonth();
    auto daysInMonth = GetDaysInMonth(month);
    return ((monthTicks * daysInMonth) >> 16) & 0xFF;
}

int32_t Date::GetMonth() const
{
    return monthsElapsed % MONTH_COUNT;
}

int32_t Date::GetYear() const
{
    return monthsElapsed / MONTH_COUNT;
}

bool Date::IsDayStart() const
{
    if (monthTicks < 4)
    {
        return false;
    }
    int32_t prevMonthTick = monthTicks - 4;
    int32_t currentMonth = GetMonth();
    int32_t currentDaysInMonth = GetDaysInMonth(currentMonth);
    return ((currentDaysInMonth * monthTicks) >> 16 != (currentDaysInMonth * prevMonthTick) >> 16);
}

bool Date::IsWeekStart() const
{
    return (monthTicks & kMaskWeekTicks) == 0;
}

bool Date::IsFortnightStart() const
{
    return (monthTicks & kMaskFortnightTicks) == 0;
}

bool Date::IsMonthStart() const
{
    return (monthTicks == 0);
}

int32_t Date::GetDaysInMonth(int32_t month)
{
    Guard::ArgumentInRange(month, 0, MONTH_COUNT - 1);

    return days_in_month[month];
}

void OpenRCT2::DateUpdate(GameState_t& gameState)
{
    int32_t monthTicks = gameState.Date.monthTicks + kMonthTicksIncrement;
    if (monthTicks > kMaskMonthTicks)
    {
        gameState.Date.monthTicks = 0;
        gameState.Date.monthsElapsed++;
    }
    else
    {
        gameState.Date.monthTicks = static_cast<uint16_t>(monthTicks);
    }
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

void DateUpdateRealTimeOfDay()
{
    time_t timestamp = time(nullptr);
    struct tm* now = localtime(&timestamp);

    gRealTimeOfDay.second = now->tm_sec;
    gRealTimeOfDay.minute = now->tm_min;
    gRealTimeOfDay.hour = now->tm_hour;
}

Date& GetDate()
{
    return GetGameState().Date;
}

/**
 *
 *  rct2: 0x006C4494
 */
void ResetDate()
{
    auto& gameState = GetGameState();
    gameState.Date = {};
    gCurrentRealTimeTicks = 0;
}
