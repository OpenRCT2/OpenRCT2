/*****************************************************************************
 * Copyright (c) 2014 Ted John
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

#include "addresses.h"
#include "date.h"
#include "research.h"
#include "rct2.h"
#include "window.h"

const int _researchRate[] = { 0, 160, 250, 400 };

/**
 *
 *  rct2: 0x00684D2A
 */
static void research_calculate_expected_date()
{
	int progress = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16);
	int progressStage = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8);
	int researchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);
	int currentDay = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_TICKS, uint16);
	int currentMonth = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16);
	int expectedDay, expectedMonth, dayQuotient, dayRemainder, progressRemaining, daysRemaining;

	if (progressStage == RESEARCH_STAGE_INITIAL_RESEARCH || researchLevel == RESEARCH_FUNDING_NONE) {
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8) = 255;
	} else {
		progressRemaining = progressStage == RESEARCH_STAGE_COMPLETING_DESIGN ? 0x10000 : 0x20000;
		progressRemaining -= progress;
		daysRemaining = (progressRemaining / _researchRate[researchLevel]) * 128;

		expectedDay = currentDay + (daysRemaining & 0xFFFF);
		dayQuotient = expectedDay / 0x10000;
		dayRemainder = expectedDay % 0x10000;
		
		expectedMonth = date_get_month(currentMonth + dayQuotient + (daysRemaining >> 16));
		expectedDay = (dayRemainder * days_in_month[expectedMonth]) >> 16;

		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_DAY, uint8) = expectedDay;
		RCT2_GLOBAL(RCT2_ADDRESS_NEXT_RESEARCH_EXPECTED_MONTH, uint8) = expectedMonth;
	}
}

/**
 *
 *  rct2: 0x00684C7A
 */
void research_update()
{
	int editorScreenFlags, researchLevel, currentResearchProgress;

	editorScreenFlags = SCREEN_FLAGS_SCENARIO_EDITOR | SCREEN_FLAGS_TRACK_DESIGNER | SCREEN_FLAGS_TRACK_MANAGER;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & editorScreenFlags)
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_SCENARIO_TICKS, uint32) % 32 != 0)
		return;

	researchLevel = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_RESEARCH_LEVEL, uint8);

	currentResearchProgress = RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16);
	currentResearchProgress += _researchRate[researchLevel];
	if (currentResearchProgress <= 0xFFFF) {
		RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = currentResearchProgress;
	} else {
		switch (RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8)) {
		case RESEARCH_STAGE_INITIAL_RESEARCH:
			RCT2_CALLPROC_EBPSAFE(0x00684BE5);
			research_calculate_expected_date();
			break;
		case RESEARCH_STAGE_DESIGNING:
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_COMPLETING_DESIGN;
			research_calculate_expected_date();
			window_invalidate_by_id(WC_CONSTRUCT_RIDE, 0);
			window_invalidate_by_id(WC_RESEARCH, 0);
			break;
		case RESEARCH_STAGE_COMPLETING_DESIGN:
			RCT2_CALLPROC_X(0x006848D4, RCT2_GLOBAL(0x013580E0, uint32), 0, 0, 0, 0, 0, 0);
			RCT2_CALLPROC_EBPSAFE(0x006848D4);
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = 0;
			research_calculate_expected_date();
			RCT2_CALLPROC_EBPSAFE(0x00684BAE);
			window_invalidate_by_id(WC_CONSTRUCT_RIDE, 0);
			window_invalidate_by_id(WC_RESEARCH, 0);
			break;
		}
	}
}