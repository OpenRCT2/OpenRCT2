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
