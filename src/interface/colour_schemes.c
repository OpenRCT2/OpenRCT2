/*****************************************************************************
* Copyright (c) 2014 Ted John, Peter Hill
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

#include "../config.h"
#include "../localisation/string_ids.h"
#include "window.h"
#include "colour_schemes.h"

window_colour_scheme gColourSchemes[] = {
	{ WC_TOP_TOOLBAR, { 7, 12, 24, 1, 0, 0 }, 4, 5178, "top_toolbar" },
	{ WC_BOTTOM_TOOLBAR, { 140, 140, 0, 14, 0, 0 }, 4, 5179, "bottom_toolbar" },
	{ WC_RIDE, { 1, 26, 11, 0, 0, 0 }, 3, 5203, "ride" },
	{ WC_RIDE_CONSTRUCTION, { 24, 24, 24, 0, 0, 0 }, 3, 5199, "ride_construction" },
	{ WC_RIDE_LIST, { 1, 26, 26, 0, 0, 0 }, 3, 5204, "ride_list" },
	{ WC_SAVE_PROMPT, { 154, 0, 0, 0, 0, 0 }, 1, 5223, "save_prompt" },
	{ WC_CONSTRUCT_RIDE, { 24, 26, 26, 0, 0, 0 }, 3, 5201, "new_ride" },
	{ WC_DEMOLISH_RIDE_PROMPT, { 154, 0, 0, 0, 0, 0 }, 1, 5224, "demolish_ride_prompt" },
	{ WC_SCENERY, { 24, 12, 12, 0, 0, 0 }, 3, 5197, "scenery" },
	{ WC_OPTIONS, { 7, 7, 7, 0, 0, 0 }, 3, 5219, "options" },
	{ WC_FOOTPATH, { 24, 24, 24, 0, 0, 0 }, 3, 5198, "footpath" },
	{ WC_LAND, { 24, 24, 24, 0, 0, 0 }, 3, 5193, "land" },
	{ WC_WATER, { 24, 24, 24, 0, 0, 0 }, 3, 5194, "water" },
	{ WC_PEEP, { 1, 15, 15, 0, 0, 0 }, 3, 5205, "guest" },
	{ WC_GUEST_LIST, { 1, 15, 15, 0, 0, 0 }, 3, 5206, "guest_list" },
	{ WC_STAFF_LIST, { 1, 4, 4, 0, 0, 0 }, 3, 5208, "staff_list" },
	{ WC_FIRE_PROMPT, { 154, 0, 0, 0, 0, 0 }, 1, 5225, "staff_fire_prompt" },
	{ WC_PARK_INFORMATION, { 1, 19, 19, 0, 0, 0 }, 3, 5186, "park_information" },
	{ WC_FINANCES, { 1, 19, 19, 0, 0, 0 }, 3, 5187, "finances" },
	{ WC_TITLE_MENU, { 140, 140, 140, 0, 0, 0 }, 3, 5182, "title_menu" },
	{ WC_TITLE_EXIT, { 140, 140, 140, 0, 0, 0 }, 3, 5183, "title_exit" },
	{ WC_RECENT_NEWS, { 1, 1, 0, 0, 0, 0 }, 3, 5192, "recent_news" },
	{ WC_SCENARIO_SELECT, { 1, 26, 26, 0, 0, 0 }, 3, 5185, "scenario_select" },
	{ WC_TRACK_DESIGN_LIST, { 26, 26, 26, 0, 0, 0 }, 3, 5202, "track_design_list" },
	{ WC_TRACK_DESIGN_PLACE, { 24, 24, 24, 0, 0, 0 }, 3, 5200, "track_design_place" },
	{ WC_NEW_CAMPAIGN, { 19, 19, 19, 0, 0, 0 }, 3, 5188, "new_campaign" },
	{ WC_KEYBOARD_SHORTCUT_LIST, { 7, 7, 7, 0, 0, 0 }, 3, 5220, "keyboard_shortcuts" },
	{ WC_CHANGE_KEYBOARD_SHORTCUT, { 7, 7, 7, 0, 0, 0 }, 3, 5221, "change_keyboard_shortcut" },
	{ WC_MAP, { 12, 24, 0, 0, 0, 0 }, 2, 5190, "map" },
	{ WC_BANNER, { 24, 24, 24, 0, 0, 0 }, 3, 5209, "banner" },
	{ WC_EDITOR_OBJECT_SELECTION, { 4, 1, 1, 0, 0, 0 }, 3, 5210, "editor_object_selection" },
	{ WC_EDITOR_INVENTION_LIST, { 4, 1, 1, 0, 0, 0 }, 3, 5211, "editor_invention_list" },
	{ WC_EDITOR_SCENARIO_OPTIONS, { 4, 1, 1, 0, 0, 0 }, 3, 5212, "editor_scenario_options" },
	{ WC_EDTIOR_OBJECTIVE_OPTIONS, { 4, 1, 1, 0, 0, 0 }, 3, 5213, "editor_objection_options" },
	{ WC_MANAGE_TRACK_DESIGN, { 1, 1, 1, 0, 0, 0 }, 3, 5215, "manage_track_design" },
	{ WC_TRACK_DELETE_PROMPT, { 26, 26, 26, 0, 0, 0 }, 3, 5226, "track_delete_prompt" },
	{ WC_INSTALL_TRACK, { 26, 26, 26, 0, 0, 0 }, 3, 5216, "install_track" },
	{ WC_CLEAR_SCENERY, { 24, 24, 24, 0, 0, 0 }, 3, 5195, "clear_scenery" },
	{ WC_CHEATS, { 1, 19, 19, 0, 0, 0 }, 3, 5217, "cheats" },
	{ WC_RESEARCH, { 1, 19, 19, 0, 0, 0 }, 3, 5189, "research" },
	{ WC_VIEWPORT, { 24, 24, 24, 0, 0, 0 }, 3, 5191, "viewport" },
	{ WC_MAPGEN, { 12, 24, 24, 0, 0, 0 }, 3, 5214, "map_generation" },
	{ WC_LOADSAVE, { 7, 7, 7, 0, 0, 0 }, 3, 5222, "loadsave" },
	{ WC_LOADSAVE_OVERWRITE_PROMPT, { 154, 0, 0, 0, 0, 0 }, 1, 5227, "loadsave_overwrite_prompt" },
	{ WC_TITLE_OPTIONS, { 140, 140, 140, 0, 0, 0 }, 3, 5184, "title_options" },
	{ WC_LAND_RIGHTS, { 19, 19, 19, 0, 0, 0 }, 3, 5196, "land_rights" },
	{ WC_COLOUR_SCHEMES, { 1, 12, 12, 0, 0, 0 }, 3, 5218, "colour_schemes" },
	{ WC_STAFF, { 1, 4, 4, 0, 0, 0 }, 3, 5207, "staff" },
	{ WC_EDITOR_TRACK_BOTTOM_TOOLBAR, { 135, 135, 135, 0, 0, 0 }, 3, 5180, "editor_track_bottom_toolbar" },
	{ WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR, { 150, 150, 141, 0, 0, 0 }, 3, 5181, "editor_scenario_bottom_toolbar" },
};

marked_window_colours gColourSchemesRCT1[sizeof(gColourSchemes)] = {
	{ WC_TOP_TOOLBAR, { 1, 1, 1, 1, 0, 0 } }, // WC_TOP_TOOLBAR
	{ WC_BOTTOM_TOOLBAR, { 129, 129, 0, 18, 0, 0 } }, // WC_BOTTOM_TOOLBAR
	{ WC_RIDE, { 26, 1, 11, 0, 0, 0 } }, // WC_RIDE
	{ 0, 0, 0, 0, 0, 0 }, // WC_RIDE_CONSTRUCTION
	{ WC_RIDE_LIST, { 0, 0, 0, 0, 0, 0 } }, // WC_RIDE_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_SAVE_PROMPT
	{ 0, 0, 0, 0, 0, 0 }, // WC_CONSTRUCT_RIDE
	{ 0, 0, 0, 0, 0, 0 }, // WC_DEMOLISH_RIDE_PROMPT
	{ 0, 0, 0, 0, 0, 0 }, // WC_SCENERY
	{ 0, 0, 0, 0, 0, 0 }, // WC_OPTIONS
	{ 0, 0, 0, 0, 0, 0 }, // WC_FOOTPATH
	{ 0, 0, 0, 0, 0, 0 }, // WC_LAND
	{ 0, 0, 0, 0, 0, 0 }, // WC_WATER
	{ 0, 0, 0, 0, 0, 0 }, // WC_PEEP
	{ 0, 0, 0, 0, 0, 0 }, // WC_GUEST_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_STAFF_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_FIRE_PROMPT
	{ 0, 0, 0, 0, 0, 0 }, // WC_PARK_INFORMATION
	{ 0, 0, 0, 0, 0, 0 }, // WC_FINANCES
	{ 0, 0, 0, 0, 0, 0 }, // WC_TITLE_MENU
	{ 0, 0, 0, 0, 0, 0 }, // WC_TITLE_EXIT
	{ 0, 0, 0, 0, 0, 0 }, // WC_RECENT_NEWS
	{ 0, 0, 0, 0, 0, 0 }, // WC_SCENARIO_SELECT
	{ 0, 0, 0, 0, 0, 0 }, // WC_TRACK_DESIGN_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_TRACK_DESIGN_PLACE
	{ 0, 0, 0, 0, 0, 0 }, // WC_NEW_CAMPAIGN
	{ 0, 0, 0, 0, 0, 0 }, // WC_KEYBOARD_SHORTCUT_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_CHANGE_KEYBOARD_SHORTCUT
	{ 0, 0, 0, 0, 0, 0 }, // WC_MAP
	{ 0, 0, 0, 0, 0, 0 }, // WC_BANNER
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDITOR_OBJECT_SELECTION
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDITOR_INVENTION_LIST
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDITOR_SCENARIO_OPTIONS
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDTIOR_OBJECTIVE_OPTIONS
	{ 0, 0, 0, 0, 0, 0 }, // WC_MANAGE_TRACK_DESIGN
	{ 0, 0, 0, 0, 0, 0 }, // WC_TRACK_DELETE_PROMPT
	{ 0, 0, 0, 0, 0, 0 }, // WC_INSTALL_TRACK
	{ 0, 0, 0, 0, 0, 0 }, // WC_CLEAR_SCENERY
	{ 0, 0, 0, 0, 0, 0 }, // WC_CHEATS
	{ 0, 0, 0, 0, 0, 0 }, // WC_RESEARCH
	{ 0, 0, 0, 0, 0, 0 }, // WC_VIEWPORT
	{ 0, 0, 0, 0, 0, 0 }, // WC_MAPGEN
	{ 0, 0, 0, 0, 0, 0 }, // WC_LOADSAVE
	{ 0, 0, 0, 0, 0, 0 }, // WC_LOADSAVE_OVERWRITE_PROMPT
	{ 0, 0, 0, 0, 0, 0 }, // WC_TITLE_OPTIONS
	{ 0, 0, 0, 0, 0, 0 }, // WC_LAND_RIGHTS
	{ 0, 0, 0, 0, 0, 0 }, // WC_COLOUR_SCHEMES
	{ 0, 0, 0, 0, 0, 0 }, // WC_STAFF
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDITOR_TRACK_BOTTOM_TOOLBAR
	{ 0, 0, 0, 0, 0, 0 }, // WC_EDITOR_SCENARIO_BOTTOM_TOOLBAR
};


uint32 gNumColourSchemeWindows = sizeof(gColourSchemes) / sizeof(window_colour_scheme);

window_colour_scheme* colour_scheme_get_by_class(rct_windowclass classification)
{
	for (int i = 0; i < sizeof(gColourSchemes); i++) {
		if (gColourSchemes[i].classification == classification) {
			return &(gColourSchemes[i]);
		}
	}
	return NULL;
}


void colour_scheme_update(rct_window *window)
{
	window_colour_scheme* colour_scheme = colour_scheme_get_by_class(window->classification);

	bool transparent = false;
	for (int i = 0; i < colour_scheme->num_colours; i++) {
		window->colours[i] = colour_scheme->colours[i];
		if (colour_scheme->colours[i] & 0x80) {
			transparent = true;
		}
	}
	//if (transparent)
		window->flags |= WF_TRANSPARENT;
	//else
	//	window->flags &= ~WF_TRANSPARENT;
}

void colour_scheme_update_by_class(rct_window *window, rct_windowclass classification)
{
	window_colour_scheme* colour_scheme = colour_scheme_get_by_class(classification);

	bool transparent = false;
	for (int i = 0; i < colour_scheme->num_colours; i++) {
		window->colours[i] = colour_scheme->colours[i];
		if (colour_scheme->colours[i] & 0x80) {
			transparent = true;
		}
	}
	//if (transparent)
		window->flags |= WF_TRANSPARENT;
	//else
	//	window->flags &= ~WF_TRANSPARENT;
}
