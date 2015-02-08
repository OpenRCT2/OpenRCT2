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
	RCT2_EXPENDITURE_TYPE_RIDE_UPKEEP = 1,
	RCT_EXPENDITURE_TYPE_WAGES = 10,
	RCT_EXPENDITURE_TYPE_RESEARCH = 12,
	RCT_EXPENDITURE_TYPE_INTEREST = 13
};

extern const money32 research_cost_table[4];

void finance_payment(money32 amount, rct_expenditure_type type);
void finance_pay_wages();
void finance_pay_research();
void finance_pay_interest();
void finance_pay_ride_upkeep();
void finance_reset_history();
void finance_init();
void finance_update_daily_profit();
void sub_69E869();

void finance_set_loan(money32 loan);
void game_command_set_current_loan(int* eax, int* ebx, int* ecx, int* edx, int* esi, int* edi, int* ebp);

#endif