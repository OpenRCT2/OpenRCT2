/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"
#include "Date.h"
#include "StringIds.h"

const StringId DateDayNames[] = {
    STR_DATE_DAY_1,  STR_DATE_DAY_2,  STR_DATE_DAY_3,  STR_DATE_DAY_4,  STR_DATE_DAY_5,  STR_DATE_DAY_6,  STR_DATE_DAY_7,
    STR_DATE_DAY_8,  STR_DATE_DAY_9,  STR_DATE_DAY_10, STR_DATE_DAY_11, STR_DATE_DAY_12, STR_DATE_DAY_13, STR_DATE_DAY_14,
    STR_DATE_DAY_15, STR_DATE_DAY_16, STR_DATE_DAY_17, STR_DATE_DAY_18, STR_DATE_DAY_19, STR_DATE_DAY_20, STR_DATE_DAY_21,
    STR_DATE_DAY_22, STR_DATE_DAY_23, STR_DATE_DAY_24, STR_DATE_DAY_25, STR_DATE_DAY_26, STR_DATE_DAY_27, STR_DATE_DAY_28,
    STR_DATE_DAY_29, STR_DATE_DAY_30, STR_DATE_DAY_31,
};

const StringId DateGameMonthNames[MONTH_COUNT] = {
    STR_MONTH_MARCH, STR_MONTH_APRIL,  STR_MONTH_MAY,       STR_MONTH_JUNE,
    STR_MONTH_JULY,  STR_MONTH_AUGUST, STR_MONTH_SEPTEMBER, STR_MONTH_OCTOBER,
};

const StringId DateGameShortMonthNames[MONTH_COUNT] = {
    STR_MONTH_SHORT_MAR, STR_MONTH_SHORT_APR, STR_MONTH_SHORT_MAY, STR_MONTH_SHORT_JUN,
    STR_MONTH_SHORT_JUL, STR_MONTH_SHORT_AUG, STR_MONTH_SHORT_SEP, STR_MONTH_SHORT_OCT,
};

const StringId DateFormatStringIDs[] = {
    STR_DATE_FORMAT_DAY_MONTH_YEAR,
    STR_DATE_FORMAT_MONTH_DAY_YEAR,
    STR_DATE_FORMAT_YEAR_MONTH_DAY,
    STR_DATE_FORMAT_YEAR_DAY_MONTH,
};

const StringId DateFormatStringFormatIds[] = {
    STR_DATE_FORMAT_DMY,
    STR_DATE_FORMAT_MDY,
    STR_DATE_FORMAT_YMD,
    STR_DATE_FORMAT_YDM,
};
