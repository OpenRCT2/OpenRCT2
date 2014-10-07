/*****************************************************************************
* Copyright (c) 2014 Dániel Tar
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

#ifndef _WINDOW_SCENERY_H_
#define _WINDOW_SCENERY_H_

#define WINDOW_SCENERY_WIDTH 0x27A
#define WINDOW_SCENERY_HEIGHT 0x8E
#define SCENERY_BUTTON_WIDTH 66
#define SCENERY_BUTTON_HEIGHT 80
#define SCENERY_ENTRIES_BY_TAB 128

#define window_scenery_active_tab_index RCT2_GLOBAL(0x00F64EDC, uint8)
#define window_scenery_selected_scenery_by_tab RCT2_ADDRESS(0x00F64EDD, sint16)
#define window_scenery_is_build_cluster_tool_on RCT2_GLOBAL(0x00F64F1A, uint8)
#define window_scenery_is_repaint_scenery_tool_on RCT2_GLOBAL(0x00F64F19, uint8)
#define window_scenery_rotation RCT2_GLOBAL(0x00F64F05, uint8)
#define window_scenery_primary_colour RCT2_GLOBAL(0x00F64F06, uint8)
#define window_scenery_secondary_colour RCT2_GLOBAL(0x00F64F07, uint8)
#define window_scenery_tertiary_colour RCT2_GLOBAL(0x00F64F08, uint8)

#endif