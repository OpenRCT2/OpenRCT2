/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#ifndef _DATE_H_
#define _DATE_H_

#include "../common.h"

enum {
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

enum {
    DATE_FORMAT_DAY_MONTH_YEAR,
    DATE_FORMAT_MONTH_DAY_YEAR,
    DATE_FORMAT_YEAR_MONTH_DAY,
    DATE_FORMAT_YEAR_DAY_MONTH
};

struct openrct_timeofday {
    uint8 second;
    uint8 minute;
    uint8 hour;
};

extern const sint16 days_in_month[MONTH_COUNT];
extern const rct_string_id DateFormatStringIds[];
extern const rct_string_id DateFormatStringFormatIds[];

extern uint16 gDateMonthTicks;
extern uint16 gDateMonthsElapsed;

extern openrct_timeofday gRealTimeOfDay;

sint32 date_get_month(sint32 months);
sint32 date_get_year(sint32 months);
sint32 date_get_total_months(sint32 month, sint32 year);
void date_reset();
void date_update();
void date_set(sint32 year, sint32 month, sint32 day);
void date_update_real_time_of_day();
bool date_is_day_start(sint32 monthTicks);
bool date_is_week_start(sint32 monthTicks);
bool date_is_fortnight_start(sint32 monthTicks);
bool date_is_month_start(sint32 monthTicks);

#endif
