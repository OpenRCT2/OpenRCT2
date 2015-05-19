/*****************************************************************************
* Copyright (c) 2014 Matthias Lanzinger
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* This file is part of OpenRCT2.
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/

#ifndef _FINANCE_H_
#define _FINANCE_H_

#include "../common.h"

typedef int rct_expenditure_type;

enum {
	RCT_EXPENDITURE_TYPE_RIDE_CONSTRUCTION,
	RCT_EXPENDITURE_TYPE_RIDE_RUNNING_COSTS,
	RCT_EXPENDITURE_TYPE_LAND_PURCHASE,
	RCT_EXPENDITURE_TYPE_LANDSCAPING,
	RCT_EXPENDITURE_TYPE_PARK_ENTRANCE_TICKETS,
	RCT_EXPENDITURE_TYPE_PARK_RIDE_TICKETS,
	RCT_EXPENDITURE_TYPE_SHOP_SHOP_SALES,
	RCT_EXPENDITURE_TYPE_SHOP_STOCK,
	RCT_EXPENDITURE_TYPE_FOODDRINK_SALES,
	RCT_EXPENDITURE_TYPE_FOODDRINK_STOCK,
	RCT_EXPENDITURE_TYPE_WAGES,
	RCT_EXPENDITURE_TYPE_MARKETING,
	RCT_EXPENDITURE_TYPE_RESEARCH,
	RCT_EXPENDITURE_TYPE_INTEREST,
	RCT_EXPENDITURE_TYPE_COUNT
};

#define EXPENDITURE_TABLE_MONTH_COUNT 16
#define EXPENDITURE_TABLE_TOTAL_COUNT (EXPENDITURE_TABLE_MONTH_COUNT * RCT_EXPENDITURE_TYPE_COUNT)

extern const money32 research_cost_table[4];

void finance_payment(money32 amount, rct_expenditure_type type);
void finance_pay_wages();
void finance_pay_research();
void finance_pay_interest();
void finance_pay_ride_upkeep();
void finance_reset_history();
void finance_init();
void finance_update_daily_profit();
void finance_shift_expenditure_table();
void sub_69E869();

void finance_set_loan(money32 loan);
void game_command_set_current_loan(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

#endif