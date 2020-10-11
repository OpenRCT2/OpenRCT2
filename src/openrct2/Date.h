/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "common.h"

namespace OpenRCT2
{
    /**
     * Represents the current day, month and year in OpenRCT2.
     */
    class Date final
    {
    private:
        uint16_t _monthTicks = 0;
        uint32_t _monthsElapsed = 0;

    public:
        Date() = default;
        Date(uint32_t monthsElapsed, uint16_t monthTicks);

        static Date FromYMD(int32_t year, int32_t month = 0, int32_t day = 0);

        void Update();

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
} // namespace OpenRCT2
