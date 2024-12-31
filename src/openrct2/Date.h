/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>

constexpr int32_t kMaxYear = 8192;
constexpr int32_t kTicksPerMonth = 0x10000;

enum
{
    MONTH_MARCH,
    MONTH_APRIL,
    MONTH_MAY,
    MONTH_JUNE,
    MONTH_JULY,
    MONTH_AUGUST,
    MONTH_SEPTEMBER,
    MONTH_OCTOBER,

    MONTH_COUNT
};

enum
{
    DATE_FORMAT_DAY_MONTH_YEAR,
    DATE_FORMAT_MONTH_DAY_YEAR,
    DATE_FORMAT_YEAR_MONTH_DAY,
    DATE_FORMAT_YEAR_DAY_MONTH
};

namespace OpenRCT2
{
    struct GameState_t;

    /**
     * Represents the current day, month and year in OpenRCT2.
     */
    struct Date final
    {
        uint32_t monthsElapsed = 0;
        uint16_t monthTicks = 0;

        static Date FromYMD(int32_t year, int32_t month = 0, int32_t day = 0);

        uint16_t GetMonthTicks() const;
        uint32_t GetMonthsElapsed() const;
        int32_t GetDay() const;
        int32_t GetMonth() const;
        int32_t GetYear() const;

        bool IsDayStart() const;
        bool IsWeekStart() const;
        bool IsFortnightStart() const;
        bool IsMonthStart() const;

        static int32_t GetDaysInMonth(int32_t month);
    };

    void DateUpdate(GameState_t& gameState);
} // namespace OpenRCT2

struct RealWorldDate
{
    uint8_t day;
    uint8_t month;
    int16_t year;
    uint8_t day_of_week;
};

struct RealWorldTime
{
    uint8_t second;
    uint8_t minute;
    uint8_t hour;
};

OpenRCT2::Date& GetDate();
void ResetDate();

extern RealWorldTime gRealTimeOfDay;

int32_t DateGetMonth(int32_t months);
int32_t DateGetYear(int32_t months);
int32_t DateGetTotalMonths(int32_t month, int32_t year);
void DateUpdateRealTimeOfDay();
