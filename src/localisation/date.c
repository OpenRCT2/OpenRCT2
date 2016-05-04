#include "../addresses.h"
#include "../game.h"
#include "date.h"
#include "string_ids.h"

// rct2: 0x00993988
const sint16 days_in_month[MONTH_COUNT] = { 31, 30, 31, 30, 31, 31, 30, 31 };

const rct_string_id DateFormatStringIds[] = {
	STR_DATE_FORMAT_DAY_MONTH_YEAR,
	STR_DATE_FORMAT_MONTH_DAY_YEAR,
	STR_DATE_FORMAT_YEAR_MONTH_DAY,
	STR_DATE_FORMAT_YEAR_DAY_MONTH
};

const rct_string_id DateFormatStringFormatIds[] = {
	STR_DATE_FORMAT_DMY,
	STR_DATE_FORMAT_MDY,
	STR_DATE_FORMAT_YMD,
	STR_DATE_FORMAT_YDM
};

int date_get_month(int months)
{
	return months % MONTH_COUNT;
}

int date_get_year(int months)
{
	return months / MONTH_COUNT;
}

int date_get_total_months(int month, int year)
{
	return (year - 1) * MONTH_COUNT + month;
}

/**
 *
 *  rct2: 0x006C4494
 */
void date_reset()
{
	gDateMonthsElapsed = 0;
	gDateMonthTicks = 0;
	gCurrentTicks = 0;
}
