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

#ifndef _TESTS_H_
#define _TESTS_H_

#include <stdio.h>
#include <CuTest.h>
#include "../src/common.h"

int cmdline_for_test(const char **argv, int argc);
int run_all_tests();

// Test utilities

#include "../src/scenario.h"

static void test_load_scenario(CuTest* tc, const char* file_name) {
	const rct_scenario_basic* scenario = get_scenario_by_filename(file_name);
	if (scenario == NULL) {
		CuFail(tc, "Could not load scenario");
	}
	scenario_load_and_play(scenario);
}

#endif