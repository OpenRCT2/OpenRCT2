#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../addresses.h"
#include "../audio/audio.h"
#include "../editor.h"
#include "../interface/themes.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/ride.h"
#include "../ride/track.h"
#include "../ride/track_design.h"
#include "../sprites.h"
#include "error.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_TRACK_PREVIEW,
	WIDX_ROTATE,
	WIDX_TOGGLE_SCENERY,
	WIDX_INSTALL,
	WIDX_CANCEL
};

static rct_widget window_install_track_widgets[] = {
	{ WWT_FRAME,			0,	0,		401,	0,		399,	0xFFFFFFFF,				STR_NONE				},
	{ WWT_CAPTION,			0,	1,		400,	1,		14,		2309,					STR_WINDOW_TITLE_TIP	},
	{ WWT_CLOSEBOX,			0,	389,	399,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP	},
	{ WWT_FLATBTN,			0,	15,		386,	18,		236,	0xFFFFFFFF,				STR_NONE				},
	{ WWT_FLATBTN,			0,	376,	399,	374,	397,	5169,					STR_ROTATE_90_TIP		},
	{ WWT_FLATBTN,			0,	376,	399,	350,	373,	5171,					STR_TOGGLE_SCENERY_TIP	},
	{ WWT_DROPDOWN_BUTTON,	0,	303,	397,	241,	252,	3378,					STR_NONE				},
	{ WWT_DROPDOWN_BUTTON,	0,	303,	397,	256,	267,	3379,					STR_NONE				},
	{ WIDGETS_END },
};

static void window_install_track_close(rct_window *w);
static void window_install_track_mouseup(rct_window *w, int widgetIndex);
static void window_install_track_invalidate(rct_window *w);
static void window_install_track_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_install_track_text_input(rct_window *w, int widgetIndex, char *text);

static rct_window_event_list window_install_track_events = {
	window_install_track_close,
	window_install_track_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_install_track_text_input,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_install_track_invalidate,
	window_install_track_paint,
	NULL
};

ride_list_item _window_install_track_item;

char track_dest_name[MAX_PATH];
char track_path[MAX_PATH];

/**
*
*  rct2: 0x006D386D
*/
void window_install_track_open(const char* path)
{
	rct_window *w;
	int x, y;
	void *mem;

	window_close_by_class(WC_EDITOR_OBJECT_SELECTION);

	window_close_construction_windows();
	ride_list_item item = {
		.type = 0xFF,
		.entry_index = 0
	};
	_window_install_track_item = item;

	mem = malloc(1285292);
	if (mem == NULL)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, void*) = mem;
	gTrackDesignSceneryToggle = false;
	_currentTrackPieceDirection = 2;
	// reset_track_list_cache();

	x = gScreenWidth / 2 - 201;
	y = max(28, gScreenHeight / 2 - 200);

	w = window_create(x, y, 402, 400, &window_install_track_events, WC_INSTALL_TRACK, 0);
	w->widgets = window_install_track_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ROTATE) | (1 << WIDX_TOGGLE_SCENERY) | (1 << WIDX_INSTALL) | (1 << WIDX_CANCEL);
	window_init_scroll_widgets(w);
	w->track_list.var_484 = 0;
	window_push_others_right(w);

	strncpy(track_path, path, MAX_PATH);
	track_path[MAX_PATH - 1] = '\0';

	char* track_name_pointer = strrchr(track_path, platform_get_path_separator());
	track_name_pointer++;

	strncpy(track_dest_name, track_name_pointer, MAX_PATH);
	track_dest_name[MAX_PATH - 1] = '\0';

	window_invalidate(w);
}

/**
*
*  rct2: 0x006CFB82
*/
static void window_install_track_select(rct_window *w, int index)
{
	utf8 *trackDesignItem, *trackDesignList = RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, utf8);
	// rct_track_design *trackDesign;

	w->track_list.var_480 = index;

	audio_play_sound_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && index == 0) {
		window_close(w);
		ride_construct_new(_window_install_track_item);
		return;
	}

	if (RCT2_GLOBAL(0x00F44153, uint8) != 0) {
		gTrackDesignSceneryToggle = true;
	}

	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
		index--;

	trackDesignItem = trackDesignList + (index * 128);
	RCT2_GLOBAL(0x00F4403C, utf8*) = trackDesignItem;

	// window_track_list_format_name(
	// 	(char*)0x009BC313,
	// 	trackDesignItem,
	// 	gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ?
	// 	0 :
	// 	FORMAT_WHITE,
	// 	1);

	char track_path[MAX_PATH] = { 0 };
	substitute_path(track_path, (char*)RCT2_ADDRESS_TRACKS_PATH, trackDesignItem);

	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_manage_open(NULL);
		return;
	}

	// if (!load_track_design(track_path)) {
	// 	w->track_list.var_480 = 0xFFFF;
	// 	window_invalidate(w);
	// 	return;
	// }

	// trackDesign = track_get_info(index, NULL);
	// if (trackDesign == NULL) return;
	// if (trackDesign->track_td6.track_flags & 4)
	// 	window_error_open(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, -1);

	window_close(w);
	window_track_place_open(NULL);
}

/**
*
*  rct2: 0x006D41DC
*/
static void window_install_track_close(rct_window *w)
{
	free(RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_CACHE, void*));
}

/**
*
*  rct2: 0x006D407A
*/
static void window_install_track_mouseup(rct_window *w, int widgetIndex)
{
	int result;

	switch (widgetIndex) {
	case WIDX_CLOSE:
	case WIDX_CANCEL:
		window_close(w);
		break;
	case WIDX_ROTATE:
		_currentTrackPieceDirection++;
		_currentTrackPieceDirection %= 4;
		window_invalidate(w);
		break;
	case WIDX_TOGGLE_SCENERY:
		gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
		// reset_track_list_cache();
		window_invalidate(w);
		break;
	case WIDX_INSTALL:
		result = install_track(track_path, track_dest_name);

		if (result == 1)
			window_close(w);
		else if(result == 0){
			window_error_open(3380, 3382);
			window_close(w);
		}
		else{
			// Copy the track name into the string buffer.
			// window_track_list_format_name(RCT2_ADDRESS(0x009BC677, char), track_dest_name, 0, 0);
			window_text_input_open(w, WIDX_INSTALL, 3383, 3384, 3165, 0, 255);
		}
		break;
	}
}


/**
*
*  rct2: 0x006D3B06
*/
static void window_install_track_invalidate(rct_window *w)
{
	colour_scheme_update(w);

	w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
	if (!gTrackDesignSceneryToggle) {
		w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
	} else {
		w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
	}

	// if (w->track_list.var_482 != 0xFFFF) {
	// 	w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
	// } else {
	// 	w->disabled_widgets |= (1 << WIDX_TRACK_PREVIEW);
	// }
}

/**
*
*  rct2: 0x006D3B1F
*/
static void window_install_track_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	rct_track_design *trackDesign = NULL;
	uint8 *image, *trackDesignList = RCT2_ADDRESS(RCT2_ADDRESS_TRACK_LIST, uint8);
	uint16 holes, speed, drops, dropHeight, inversions;
	fixed32_2dp rating;
	int x, y, colour, gForces, airTime;
	rct_g1_element tmpElement, *substituteElement;

	window_draw_widgets(w, dpi);

	// if (w->track_list.var_482 == 0xFFFF)
	// 	return;

	// Track preview
	widget = &window_install_track_widgets[WIDX_TRACK_PREVIEW];
	x = w->x + widget->left + 1;
	y = w->y + widget->top + 1;
	colour = ColourMapA[w->colours[0]].darkest;
	gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

	//call 6d3993 (load track)
	trackDesign = temp_track_get_info(track_path, &image);
	if (trackDesign == NULL)
		return;

	rct_track_td6* track_td6 = &trackDesign->track_td6;

	substituteElement = &g1Elements[0];
	tmpElement = *substituteElement;
	substituteElement->offset = image;
	substituteElement->width = 370;
	substituteElement->height = 217;
	substituteElement->x_offset = 0;
	substituteElement->y_offset = 0;
	substituteElement->flags = G1_FLAG_BMP;
	gfx_draw_sprite(dpi, 0, x, y, 0);
	*substituteElement = tmpElement;

	x = w->x + (widget->left + widget->right) / 2;
	y = w->y + widget->bottom - 12;

	RCT2_GLOBAL(0x00F44153, uint8) = 0;

	// Warnings
	if (track_td6->track_flags & 1) {
		RCT2_GLOBAL(0x00F44153, uint8) = 1;
		if (!gTrackDesignSceneryToggle) {
			// Scenery not available
			gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, NULL, 0, x, y, 368);
			y -= 10;
		}
	}

	// Track design name
	// window_track_list_format_name((char*)0x009BC677, (char*)0x009E3504, FORMAT_WINDOW_COLOUR_1, 1);
	gfx_draw_string_centred_clipped(dpi, 3165, NULL, 0, x, y, 368);

	// Information
	x = w->x + widget->left + 1;
	y = w->y + widget->bottom + 2;
	// 0x006D3CF1 -- 0x006d3d71 missing

	// Stats
	rating = track_td6->excitement * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, 0, x, y);
	y += 10;

	rating = track_td6->intensity * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, 0, x, y);
	y += 10;

	rating = track_td6->nausea * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, 0, x, y);
	y += 14;

	if (track_td6->type != RIDE_TYPE_MAZE) {
		if (track_td6->type == RIDE_TYPE_MINI_GOLF) {
			// Holes
			holes = track_td6->holes & 0x1F;
			gfx_draw_string_left(dpi, STR_HOLES, &holes, 0, x, y);
			y += 10;
		}
		else {
			// Maximum speed
			speed = ((track_td6->max_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, 0, x, y);
			y += 10;

			// Average speed
			speed = ((track_td6->average_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, 0, x, y);
			y += 10;
		}

		// Ride length
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = 1345;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = track_td6->ride_length;
		gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y, 214);
		y += 10;
	}

	if (ride_type_has_flag(track_td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
		// Maximum positive vertical Gs
		gForces = track_td6->max_positive_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, 0, x, y);
		y += 10;

		// Maximum negative verical Gs
		gForces = track_td6->max_negative_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, 0, x, y);
		y += 10;

		// Maximum lateral Gs
		gForces = track_td6->max_lateral_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, 0, x, y);
		y += 10;

		// If .TD6
		if (track_td6->version_and_colour_scheme / 4 >= 2) {
			if (track_td6->total_air_time != 0) {
				// Total air time
				airTime = track_td6->total_air_time * 25;
				gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, 0, x, y);
				y += 10;
			}
		}
	}

	if (ride_type_has_flag(track_td6->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
		// Drops
		drops = track_td6->drops & 0x3F;
		gfx_draw_string_left(dpi, STR_DROPS, &drops, 0, x, y);
		y += 10;

		// Drop height is multiplied by 0.75
		dropHeight = (track_td6->highest_drop_height + (track_td6->highest_drop_height / 2)) / 2;
		gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &drops, 0, x, y);
		y += 10;
	}

	if (track_td6->type != RIDE_TYPE_MINI_GOLF) {
		inversions = track_td6->inversions & 0x1F;
		if (inversions != 0) {
			// Inversions
			gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, 0, x, y);
			y += 10;
		}
	}
	y += 4;

	if (track_td6->space_required_x != 0xFF) {
		// Space required
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = track_td6->space_required_x;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = track_td6->space_required_y;
		gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y);
		y += 10;
	}

	if (track_td6->cost != 0) {
		gfx_draw_string_left(dpi, STR_TRACK_LIST_COST_AROUND, &track_td6->cost, 0, x, y);
		y += 14;
	}
}

/**
*
*  rct2: 0x006D40A7
*/
static void window_install_track_text_input(rct_window *w, int widgetIndex, char *text)
{
	if (text == NULL) {
		window_close(w);
		return;
	}

	if (widgetIndex == WIDX_INSTALL) {
		char* extension_pointer = track_dest_name;
		while (*extension_pointer++ != '.');
		--extension_pointer;
		strcat(text, extension_pointer);
		strcpy(track_dest_name, text);
		window_event_mouse_up_call(w, WIDX_INSTALL);
	}
}
