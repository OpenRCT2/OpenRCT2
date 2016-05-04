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

extern const sint16 days_in_month[MONTH_COUNT];
extern const rct_string_id DateFormatStringIds[];
extern const rct_string_id DateFormatStringFormatIds[];

#define gDateMonthTicks			RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16)
#define gDateMonthsElapsed		RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16)

int date_get_month(int months);
int date_get_year(int months);
int date_get_total_months(int month, int year);
void date_reset();

#endif
