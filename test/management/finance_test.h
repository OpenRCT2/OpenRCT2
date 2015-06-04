/*****************************************************************************
* Copyright (c) 2015 Marco Costa
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

#ifndef _FINANCE_TEST_H_
#define _FINANCE_TEST_H_

#include "../tests.h"

void test_finance_setup(CuTest* tc);
void test_finance_loan_increase(CuTest* tc);
void test_finance_loan_pay_back(CuTest* tc);

#endif