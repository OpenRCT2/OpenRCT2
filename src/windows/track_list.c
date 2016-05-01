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
#include "../rct1.h"
#include "../ride/ride.h"
#include "../ride/track.h"
#include "../ride/track_design.h"
#include "../sprites.h"
#include "error.h"

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_TRACK_LIST,
	WIDX_TRACK_PREVIEW,
	WIDX_ROTATE,
	WIDX_TOGGLE_SCENERY,
	WIDX_BACK,
};

static rct_widget window_track_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		599,	0,		399,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_CAPTION,			0,	1,		598,	1,		14,		STR_SELECT_DESIGN,		STR_WINDOW_TITLE_TIP					},
	{ WWT_CLOSEBOX,			0,	587,	597,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP					},
	{ WWT_SCROLL,			0,	4,		221,	33,		395,	2,						STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP		},
	{ WWT_FLATBTN,			0,	224,	595,	18,		236,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_FLATBTN,			0,	574,	597,	374,	397,	5169,					STR_ROTATE_90_TIP						},
	{ WWT_FLATBTN,			0,	574,	597,	350,	373,	5171,					STR_TOGGLE_SCENERY_TIP					},
	{ WWT_13,				0,	4,		221,	18,		29,		STR_SELECT_OTHER_RIDE,	STR_NONE								},
	{ WIDGETS_END },
};

static void window_track_list_close(rct_window *w);
static void window_track_list_mouseup(rct_window *w, int widgetIndex);
static void window_track_list_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height);
static void window_track_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y);
static void window_track_list_scrollmouseover(rct_window *w, int scrollIndex, int x, int y);
static void window_track_list_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_track_list_invalidate(rct_window *w);
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static rct_window_event_list window_track_list_events = {
	window_track_list_close,
	window_track_list_mouseup,
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
	window_track_list_scrollgetsize,
	window_track_list_scrollmousedown,
	NULL,
	window_track_list_scrollmouseover,
	NULL,
	NULL,
	NULL,
	window_track_list_tooltip,
	NULL,
	NULL,
	window_track_list_invalidate,
	window_track_list_paint,
	window_track_list_scrollpaint
};

ride_list_item _window_track_list_item;

static track_design_file_ref *_trackDesigns = NULL;
static size_t _trackDesignsCount = 0;
static uint16 _loadedTrackDesignIndex;
static rct_track_td6 *_loadedTrackDesign;
static uint8 _loadedTrackDesignPreview[4][TRACK_PREVIEW_IMAGE_SIZE];

static void track_list_load_designs(ride_list_item item);
static bool track_list_load_design_for_preview(utf8 *path);

/**
 *
 *  rct2: 0x006CF1A2
 */
void window_track_list_open(ride_list_item item)
{
	window_close_construction_windows();
	_window_track_list_item = item;
	track_list_load_designs(item);

	if (RCT2_GLOBAL(0x00F635ED, uint8) & 1) {
		window_error_open(STR_WARNING, STR_TOO_MANY_TRACK_DESIGNS_OF_THIS_TYPE);
	}

	int x, y;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		x = gScreenWidth / 2 - 300;
		y = max(28, gScreenHeight / 2 - 200);
	} else {
		x = 0;
		y = 29;
	}
	rct_window *w = window_create(
		x,
		y,
		600,
		400,
		&window_track_list_events,
		WC_TRACK_DESIGN_LIST,
		0
	);
	w->widgets = window_track_list_widgets;
	w->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_ROTATE) | (1 << WIDX_TOGGLE_SCENERY) | (1 << WIDX_BACK);
	window_init_scroll_widgets(w);
	w->track_list.var_480 = 0xFFFF;
	w->track_list.var_482 = gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 1;
	w->track_list.var_484 = 0;
	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) = 0;
	window_push_others_right(w);
	_currentTrackPieceDirection = 2;
}

/**
 *
 *  rct2: 0x006CFD76
 */
static void window_track_list_close(rct_window *w)
{
	free(_loadedTrackDesign);
	_loadedTrackDesign = NULL;

	// Dispose track list
	for (size_t i = 0; i < _trackDesignsCount; i++) {
		free(_trackDesigns[i].name);
		free(_trackDesigns[i].path);
	}
	SafeFree(_trackDesigns);
	_trackDesignsCount = 0;
}

/**
 *
 *  rct2: 0x006CFB82
 */
static void window_track_list_select(rct_window *w, int index)
{
	w->track_list.var_480 = index;

	audio_play_sound_panned(SOUND_CLICK_1, w->x + (w->width / 2), 0, 0, 0);
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		if (index == 0) {
			window_close(w);
			ride_construct_new(_window_track_list_item);
			return;
		}
		index--;
	}

	if (RCT2_GLOBAL(0x00F44153, uint8) != 0) {
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) = 1;
	}

	track_design_file_ref *tdRef = &_trackDesigns[index];

	// trackDesignItem = trackDesignList + (index * 128);
	// RCT2_GLOBAL(0x00F4403C, utf8*) = trackDesignItem;
	// window_track_list_format_name(
	// 	(char*)0x009BC313,
	// 	trackDesignItem,
	// 	gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ?
	// 	0 :
	// 	FORMAT_WHITE,
	// 	1);

	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_manage_open();
		return;
	}

	if (_loadedTrackDesignIndex != -1 && (_loadedTrackDesign->track_flags & 4)) {
		window_error_open(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE);
	}

	window_close(w);
	window_track_place_open(tdRef->path);
}

static int window_track_list_get_list_item_index_from_position(int x, int y)
{
	int maxItems = _trackDesignsCount;
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Extra item: custom design
		maxItems++;
	}

	int index = y / 10;
	if (index < 0 || index >= maxItems) {
		index = -1;
	}
	return index;
}

/**
 *
 *  rct2: 0x006CFA31
 */
static void window_track_list_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
			window_close_by_number(WC_MANAGE_TRACK_DESIGN, w->number);
			window_close_by_number(WC_TRACK_DELETE_PROMPT, w->number);
			trackmanager_load();
		}
		break;
	case WIDX_ROTATE:
		_currentTrackPieceDirection++;
		_currentTrackPieceDirection %= 4;
		window_invalidate(w);
		break;
	case WIDX_TOGGLE_SCENERY:
		RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) ^= 1;
		// reset_track_list_cache();
		window_invalidate(w);
		break;
	case WIDX_BACK:
		window_close(w);
		if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
			window_close_by_number(WC_MANAGE_TRACK_DESIGN, w->number);
			window_close_by_number(WC_TRACK_DELETE_PROMPT, w->number);
			trackmanager_load();
		} else {
			window_new_ride_open();
		}
		break;
	}
}

/**
 *
 *  rct2: 0x006CFAB0
 */
static void window_track_list_scrollgetsize(rct_window *w, int scrollIndex, int *width, int *height)
{
	size_t numItems = _trackDesignsCount;
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Extra item: custom design
		numItems++;
	}

	*height = (int)(numItems * 10);
}

/**
 *
 *  rct2: 0x006CFB39
 */
static void window_track_list_scrollmousedown(rct_window *w, int scrollIndex, int x, int y)
{
	if (!(w->track_list.var_484 & 1)) {
		int i = window_track_list_get_list_item_index_from_position(x, y);
		if (i != -1) {
			window_track_list_select(w, i);
		}
	}
}

/**
 *
 *  rct2: 0x006CFAD7
 */
static void window_track_list_scrollmouseover(rct_window *w, int scrollIndex, int x, int y)
{
	if (!(w->track_list.var_484 & 1)) {
		int i = window_track_list_get_list_item_index_from_position(x, y);
		if (i != -1 && w->track_list.var_482 != i) {
			w->track_list.var_482 = i;
			window_invalidate(w);
		}
	}
}

/**
 *
 *  rct2: 0x006CFD6C
 */
static void window_track_list_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = STR_LIST;
}

/**
 *
 *  rct2: 0x006CF2D6
 */
static void window_track_list_invalidate(rct_window *w)
{
	rct_ride_entry *entry;
	rct_string_id stringId;

	colour_scheme_update(w);

	entry = get_ride_entry(_window_track_list_item.entry_index);

	stringId = entry->name;
	if (!(entry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(entry))
		stringId = _window_track_list_item.type + 2;

	RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS, uint16) = stringId;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_list_widgets[WIDX_TITLE].image = STR_TRACK_DESIGNS;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
	} else {
		window_track_list_widgets[WIDX_TITLE].image = STR_SELECT_DESIGN;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
	}

	if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || w->track_list.var_482 != 0) {
		w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
		w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
		window_track_list_widgets[WIDX_ROTATE].type = WWT_FLATBTN;
		window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_FLATBTN;
		if (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) == 0)
			w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
		else
			w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
	} else {
		w->pressed_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
		w->disabled_widgets |= (1 << WIDX_TRACK_PREVIEW);
		window_track_list_widgets[WIDX_ROTATE].type = WWT_EMPTY;
		window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_EMPTY;
	}
}

/**
 *
 *  rct2: 0x006CF387
 */
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	int trackIndex = w->track_list.var_482;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		if (_trackDesignsCount == 0 || trackIndex == -1) {
			return;
		}
	} else if (trackIndex-- == 0) {
		return;
	}

	// Track preview
	int x, y, colour;
	rct_widget *widget = &window_track_list_widgets[WIDX_TRACK_PREVIEW];
	x = w->x + widget->left + 1;
	y = w->y + widget->top + 1;
	colour = ColourMapA[w->colours[0]].darkest;
	gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

	if (_loadedTrackDesignIndex != trackIndex) {
		uint8 *path = _trackDesigns[trackIndex].path;
		if (track_list_load_design_for_preview(path)) {
			_loadedTrackDesignIndex = trackIndex;
		} else {
			_loadedTrackDesignIndex = -1;
			return;
		}
	}

	rct_track_td6 *td6 = _loadedTrackDesign;
	if (td6 == NULL) {
		return;
	}

	uint8 *image = _loadedTrackDesignPreview[_currentTrackPieceDirection];

	rct_g1_element *substituteElement = &g1Elements[0];
	rct_g1_element tmpElement = *substituteElement;
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
	if ((td6->track_flags & 4) && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Vehicle design not available
		gfx_draw_string_centred_clipped(dpi, STR_VEHICLE_DESIGN_UNAVAILABLE, NULL, 0, x, y, 368);
		y -= 10;
	}

	if (td6->track_flags & 1) {
		RCT2_GLOBAL(0x00F44153, uint8) = 1;
		if (RCT2_GLOBAL(RCT2_ADDRESS_TRACK_DESIGN_SCENERY_TOGGLE, uint8) == 0) {
			// Scenery not available
			gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, NULL, 0, x, y, 368);
			y -= 10;
		}
	}

	// Track design name
	utf8 *trackName = _trackDesigns[trackIndex].name;
	gfx_draw_string_centred_clipped(dpi, STR_TRACK_PREVIEW_NAME_FORMAT, &trackName, 0, x, y, 368);

	// Information
	x = w->x + widget->left + 1;
	y = w->y + widget->bottom + 2;

	// Stats
	fixed32_2dp rating = td6->excitement * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, 0, x, y);
	y += 10;

	rating = td6->intensity * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, 0, x, y);
	y += 10;

	rating = td6->nausea * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, 0, x, y);
	y += 14;

	if (td6->type != RIDE_TYPE_MAZE) {
		if (td6->type == RIDE_TYPE_MINI_GOLF) {
			// Holes
			uint16 holes = td6->holes & 0x1F;
			gfx_draw_string_left(dpi, STR_HOLES, &holes, 0, x, y);
			y += 10;
		} else {
			// Maximum speed
			uint16 speed = ((td6->max_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, 0, x, y);
			y += 10;

			// Average speed
			speed = ((td6->average_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, 0, x, y);
			y += 10;
		}

		// Ride length
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = 1345;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = td6->ride_length;
		gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y, 214);
		y += 10;
	}

	if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
		// Maximum positive vertical Gs
		int gForces = td6->max_positive_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, 0, x, y);
		y += 10;

		// Maximum negative verical Gs
		gForces = td6->max_negative_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, 0, x, y);
		y += 10;

		// Maximum lateral Gs
		gForces = td6->max_lateral_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, 0, x, y);
		y += 10;

		// If .TD6
		if (td6->version_and_colour_scheme / 4 >= 2) {
			if (td6->total_air_time != 0) {
				// Total air time
				int airTime = td6->total_air_time * 25;
				gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, 0, x, y);
				y += 10;
			}
		}
	}

	if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
		// Drops
		uint16 drops = td6->drops & 0x3F;
		gfx_draw_string_left(dpi, STR_DROPS, &drops, 0, x, y);
		y += 10;

		// Drop height is multiplied by 0.75
		uint16 dropHeight = (td6->highest_drop_height + (td6->highest_drop_height / 2)) / 2;
		gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &drops, 0, x, y);
		y += 10;
	}

	if (td6->type != RIDE_TYPE_MINI_GOLF) {
		uint16 inversions = td6->inversions & 0x1F;
		if (inversions != 0) {
			// Inversions
			gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, 0, x, y);
			y += 10;
		}
	}
	y += 4;

	if (td6->space_required_x != 0xFF) {
		// Space required
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, uint16) = td6->space_required_x;
		RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, uint16) = td6->space_required_y;
		gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y);
		y += 10;
	}

	if (td6->cost != 0) {
		gfx_draw_string_left(dpi, STR_TRACK_LIST_COST_AROUND, &td6->cost, 0, x, y);
		y += 14;
	}
}

/**
 *
 *  rct2: 0x006CF8CD
 */
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	rct_string_id stringId, stringId2;
	int i, x, y, colour;

	colour = ColourMapA[w->colours[0]].mid_light;
	colour = (colour << 24) | (colour << 16) | (colour << 8) | colour;
	gfx_clear(dpi, colour);

	i = 0;
	x = 0;
	y = 0;

	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		if (_trackDesignsCount == 0) {
			// No track designs
			gfx_draw_string_left(dpi, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE, NULL, 0, x, y - 1);
			return;
		}
	} else {
		// Build custom track item
		if (i == w->track_list.var_482) {
			// Highlight
			gfx_fill_rect(dpi, x, y, w->width, y + 9, 0x2000000 | 49);
			stringId = 1193;
		} else {
			stringId = 1191;
		}

		stringId2 = STR_BUILD_CUSTOM_DESIGN;
		gfx_draw_string_left(dpi, stringId, &stringId2, 0, x, y - 1);
		y += 10;
		i++;
	}

	for (size_t i = 0; i < _trackDesignsCount; i++) {
		if (y + 10 >= dpi->y && y < dpi->y + dpi->height) {
			uint16 listIndex = (uint16)(i + 1);
			if (listIndex == w->track_list.var_482) {
				// Highlight
				gfx_fill_rect(dpi, x, y, w->width, y + 9, 0x2000000 | 49);
				stringId = 1193;
			} else {
				stringId = 1191;
			}

			// Draw track name
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 0, rct_string_id) = STR_TRACK_LIST_NAME_FORMAT;
			RCT2_GLOBAL(RCT2_ADDRESS_COMMON_FORMAT_ARGS + 2, utf8*) = _trackDesigns[i].name;
			gfx_draw_string_left(dpi, stringId, (void*)RCT2_ADDRESS_COMMON_FORMAT_ARGS, 0, x, y - 1);
		}
		y += 10;
	}
}

static void track_list_load_designs(ride_list_item item)
{
	char entry[9];
	const char *entryPtr = NULL;
	if (item.type < 0x80) {
		rct_ride_entry *rideEntry = get_ride_entry(item.entry_index);
		if ((rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) && !rideTypeShouldLoseSeparateFlag(rideEntry)) {
			get_ride_entry_name(entry, item.entry_index);
			entryPtr = entry;
		}
	}
	_trackDesignsCount = track_design_index_get_for_ride(&_trackDesigns, item.type, entryPtr);
}

static bool track_list_load_design_for_preview(utf8 *path)
{
	// Dispose currently loaded track
	track_design_dispose(_loadedTrackDesign);
	_loadedTrackDesign = NULL;

	_loadedTrackDesign = calloc(sizeof(rct_track_td6), 1);
	if (track_design_open(_loadedTrackDesign, path)) {
		// Load in a new preview image, calculate cost variable, calculate var_06
		draw_track_preview(_loadedTrackDesign, (uint8**)_loadedTrackDesignPreview);

		_loadedTrackDesign->cost = gTrackDesignCost;
		_loadedTrackDesign->track_flags = RCT2_GLOBAL(0x00F44151, uint8) & 7;
		return true;
	} else {
		track_design_dispose(_loadedTrackDesign);
		_loadedTrackDesign = NULL;
	}
	return false;
}
