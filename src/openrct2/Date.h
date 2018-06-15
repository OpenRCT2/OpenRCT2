/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
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
        uint16 _monthTicks      = 0;
        uint32 _monthsElapsed   = 0;

    public:
        Date() = default;
        Date(uint32 monthsElapsed, uint16 monthTicks);

        static Date FromYMD(sint32 year, sint32 month = 0, sint32 day = 0);

        void Update();

        uint16 GetMonthTicks() const;
        uint32 GetMonthsElapsed() const;
        sint32 GetDay() const;
        sint32 GetMonth() const;
        sint32 GetYear() const;

        bool IsDayStart() const;
        bool IsWeekStart() const;
        bool IsFortnightStart() const;
        bool IsMonthStart() const;

        static sint32 GetDaysInMonth(sint32 month);
    };
}
