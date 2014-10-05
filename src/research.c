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
#include "research.h"
#include "rct2.h"
#include "window.h"

const int _researchRate[] = { 0, 160, 250, 400 };

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

	if (RCT2_GLOBAL(0x00F663AC, uint32) & 0x1F)
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
			RCT2_CALLPROC_EBPSAFE(0x00684D2A);
			break;
		case RESEARCH_STAGE_DESIGNING:
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = RESEARCH_STAGE_COMPLETING_DESIGN;
			RCT2_CALLPROC_EBPSAFE(0x00684D2A);
			window_invalidate_by_id(WC_CONSTRUCT_RIDE, 0);
			window_invalidate_by_id(WC_RESEARCH, 0);
			break;
		case RESEARCH_STAGE_COMPLETING_DESIGN:
			RCT2_CALLPROC_X(0x006848D4, RCT2_GLOBAL(0x013580E0, uint32), 0, 0, 0, 0, 0, 0);
			RCT2_CALLPROC_EBPSAFE(0x006848D4);
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS, uint16) = 0;
			RCT2_GLOBAL(RCT2_ADDRESS_RESEARH_PROGRESS_STAGE, uint8) = 0;
			RCT2_CALLPROC_EBPSAFE(0x00684D2A);
			RCT2_CALLPROC_EBPSAFE(0x00684BAE);
			window_invalidate_by_id(WC_CONSTRUCT_RIDE, 0);
			window_invalidate_by_id(WC_RESEARCH, 0);
			break;
		}
	}
}