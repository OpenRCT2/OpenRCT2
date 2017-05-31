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

#include "../audio/audio.h"
#include "../config/Config.h"
#include "../Context.h"
#include "../drawing/IDrawingEngine.h"
#include "../editor.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../rct1.h"
#include "../ride/ride.h"
#include "../ride/track.h"
#include "../ride/track_design.h"
#include "../ride/TrackDesignRepository.h"
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

validate_global_widx(WC_TRACK_DESIGN_LIST, WIDX_ROTATE);

static rct_widget window_track_list_widgets[] = {
	{ WWT_FRAME,			0,	0,		599,	0,		399,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_CAPTION,			0,	1,		598,	1,		14,		STR_SELECT_DESIGN,		STR_WINDOW_TITLE_TIP					},
	{ WWT_CLOSEBOX,			0,	587,	597,	2,		13,		STR_CLOSE_X,			STR_CLOSE_WINDOW_TIP					},
	{ WWT_SCROLL,			0,	4,		221,	33,		395,	SCROLL_VERTICAL,						STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP		},
	{ WWT_FLATBTN,			0,	224,	595,	18,		236,	0xFFFFFFFF,				STR_NONE								},
	{ WWT_FLATBTN,			0,	574,	597,	374,	397,	SPR_ROTATE_ARROW,		STR_ROTATE_90_TIP						},
	{ WWT_FLATBTN,			0,	574,	597,	350,	373,	SPR_SCENERY,			STR_TOGGLE_SCENERY_TIP					},
	{ WWT_13,				0,	4,		221,	18,		29,		STR_SELECT_OTHER_RIDE,	STR_NONE								},
	{ WIDGETS_END },
};

static void window_track_list_close(rct_window *w);
static void window_track_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_track_list_update(rct_window *w);
static void window_track_list_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_track_list_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_track_list_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y);
static void window_track_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_track_list_invalidate(rct_window *w);
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static rct_window_event_list window_track_list_events = {
	window_track_list_close,
	window_track_list_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_list_update,
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

#define TRACK_DESIGN_INDEX_UNLOADED UINT16_MAX

ride_list_item _window_track_list_item;

static track_design_file_ref *_trackDesigns = NULL;
static size_t _trackDesignsCount = 0;
static uint16 _loadedTrackDesignIndex;
static rct_track_td6 *_loadedTrackDesign;
static uint8 *_trackDesignPreviewPixels;

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

	sint32 x, y;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		sint32 screenWidth = context_get_width();
		sint32 screenHeight = context_get_height();
		x = screenWidth / 2 - 300;
		y = max(28, screenHeight / 2 - 200);
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
	w->track_list.var_484 = 0;
	w->track_list.reload_track_designs = false;
	w->selected_list_item = 0;
	if (_trackDesignsCount != 0 && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		w->selected_list_item = 1;
	}
	gTrackDesignSceneryToggle = false;
	window_push_others_right(w);
	_currentTrackPieceDirection = 2;

	_trackDesignPreviewPixels = calloc(4, TRACK_PREVIEW_IMAGE_SIZE);

	_loadedTrackDesign = NULL;
	_loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
}

/**
 *
 *  rct2: 0x006CFD76
 */
static void window_track_list_close(rct_window *w)
{
	// Dispose track design and preview
	track_design_dispose(_loadedTrackDesign);
	_loadedTrackDesign = NULL;
	SafeFree(_trackDesignPreviewPixels);

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
static void window_track_list_select(rct_window *w, sint32 index)
{
	w->track_list.var_480 = index;

	// Displays a message if the ride can't load, fix #4080
	if (_loadedTrackDesign == NULL) {
		window_error_open(STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_TRACK_LOAD_FAILED_ERROR);
		return;
	}

	audio_play_sound(SOUND_CLICK_1, 0, w->x + (w->width / 2));
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		if (index == 0) {
			window_close(w);
			ride_construct_new(_window_track_list_item);
			return;
		}
		index--;
	}

	rct_track_td6 *td6 = _loadedTrackDesign;
	if (td6 != NULL && (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)) {
		gTrackDesignSceneryToggle = true;
	}

	track_design_file_ref *tdRef = &_trackDesigns[index];
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_manage_open(tdRef);
	} else {
		if (_loadedTrackDesignIndex != TRACK_DESIGN_INDEX_UNLOADED && (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE)) {
			window_error_open(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE);
		}

		window_track_place_open(tdRef);
	}
}

static sint32 window_track_list_get_list_item_index_from_position(sint32 x, sint32 y)
{
	sint32 maxItems = (sint32)_trackDesignsCount;
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Extra item: custom design
		maxItems++;
	}

	sint32 index = y / 10;
	if (index < 0 || index >= maxItems) {
		index = -1;
	}
	return index;
}

/**
 *
 *  rct2: 0x006CFA31
 */
static void window_track_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
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
		gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
		_loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
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
static void window_track_list_scrollgetsize(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	size_t numItems = _trackDesignsCount;
	if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Extra item: custom design
		numItems++;
	}

	*height = (sint32)(numItems * 10);
}

/**
 *
 *  rct2: 0x006CFB39
 */
static void window_track_list_scrollmousedown(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	if (!(w->track_list.var_484 & 1)) {
		sint32 i = window_track_list_get_list_item_index_from_position(x, y);
		if (i != -1) {
			window_track_list_select(w, i);
		}
	}
}

/**
 *
 *  rct2: 0x006CFAD7
 */
static void window_track_list_scrollmouseover(rct_window *w, sint32 scrollIndex, sint32 x, sint32 y)
{
	if (!(w->track_list.var_484 & 1)) {
		sint32 i = window_track_list_get_list_item_index_from_position(x, y);
		if (i != -1 && w->selected_list_item != i) {
			w->selected_list_item = i;
			window_invalidate(w);
		}
	}
}

/**
 *
 *  rct2: 0x006CFD6C
 */
static void window_track_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
	set_format_arg(0, rct_string_id, STR_LIST);
}

static void window_track_list_update(rct_window *w)
{
	if (w->track_list.reload_track_designs) {
		track_list_load_designs(_window_track_list_item);
		w->selected_list_item = 0;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006CF2D6
 */
static void window_track_list_invalidate(rct_window *w)
{
	rct_string_id stringId = STR_NONE;
	rct_ride_entry *entry = get_ride_entry(_window_track_list_item.entry_index);
	if (entry != NULL && entry != (rct_ride_entry*)-1) {
		stringId = entry->name;
		if (!(entry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME) || rideTypeShouldLoseSeparateFlag(entry))
			stringId = _window_track_list_item.type + 2;
	}

	set_format_arg(0, rct_string_id, stringId);
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		window_track_list_widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
	} else {
		window_track_list_widgets[WIDX_TITLE].text = STR_SELECT_DESIGN;
		window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
	}

	if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || w->selected_list_item != 0) {
		w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
		w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
		window_track_list_widgets[WIDX_ROTATE].type = WWT_FLATBTN;
		window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_FLATBTN;
		if (gTrackDesignSceneryToggle) {
			w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
		} else {
			w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
		}
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

	sint32 trackIndex = w->selected_list_item;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		if (_trackDesignsCount == 0 || trackIndex == -1) {
			return;
		}
	} else if (trackIndex-- == 0) {
		return;
	}

	// Track preview
	sint32 x, y, colour;
	rct_widget *widget = &window_track_list_widgets[WIDX_TRACK_PREVIEW];
	x = w->x + widget->left + 1;
	y = w->y + widget->top + 1;
	colour = ColourMapA[w->colours[0]].darkest;
	gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

	if (_loadedTrackDesignIndex != trackIndex) {
		utf8 *path = _trackDesigns[trackIndex].path;
		if (track_list_load_design_for_preview(path)) {
			_loadedTrackDesignIndex = trackIndex;
		} else {
			_loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
			return;
		}
	}

	rct_track_td6 *td6 = _loadedTrackDesign;
	if (td6 == NULL) {
		return;
	}

	rct_g1_element *substituteElement = &g1Elements[SPR_TEMP];
	rct_g1_element tmpElement = *substituteElement;
	substituteElement->offset = _trackDesignPreviewPixels + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
	substituteElement->width = 370;
	substituteElement->height = 217;
	substituteElement->x_offset = 0;
	substituteElement->y_offset = 0;
	substituteElement->flags = G1_FLAG_BMP;
	gfx_draw_sprite(dpi, 0, x, y, 0);
	*substituteElement = tmpElement;

	x = w->x + (widget->left + widget->right) / 2;
	y = w->y + widget->bottom - 12;

	// Warnings
	if ((td6->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE) && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
		// Vehicle design not available
		gfx_draw_string_centred_clipped(dpi, STR_VEHICLE_DESIGN_UNAVAILABLE, NULL, COLOUR_BLACK, x, y, 368);
		y -= 10;
	}

	if (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE) {
		if (!gTrackDesignSceneryToggle) {
			// Scenery not available
			gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, NULL, COLOUR_BLACK, x, y, 368);
			y -= 10;
		}
	}

	// Track design name
	utf8 *trackName = _trackDesigns[trackIndex].name;
	gfx_draw_string_centred_clipped(dpi, STR_TRACK_PREVIEW_NAME_FORMAT, &trackName, COLOUR_BLACK, x, y, 368);

	// Information
	x = w->x + widget->left + 1;
	y = w->y + widget->bottom + 2;

	// Stats
	fixed32_2dp rating = td6->excitement * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, COLOUR_BLACK, x, y);
	y += 10;

	rating = td6->intensity * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, COLOUR_BLACK, x, y);
	y += 10;

	rating = td6->nausea * 10;
	gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, COLOUR_BLACK, x, y);
	y += 14;

	if (td6->type != RIDE_TYPE_MAZE) {
		if (td6->type == RIDE_TYPE_MINI_GOLF) {
			// Holes
			uint16 holes = td6->holes & 0x1F;
			gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
			y += 10;
		} else {
			// Maximum speed
			uint16 speed = ((td6->max_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, COLOUR_BLACK, x, y);
			y += 10;

			// Average speed
			speed = ((td6->average_speed << 16) * 9) >> 18;
			gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, COLOUR_BLACK, x, y);
			y += 10;
		}

		// Ride length
		set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
		set_format_arg(2, uint16, td6->ride_length);
		gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 214);
		y += 10;
	}

	if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
		// Maximum positive vertical Gs
		sint32 gForces = td6->max_positive_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
		y += 10;

		// Maximum negative vertical Gs
		gForces = td6->max_negative_vertical_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
		y += 10;

		// Maximum lateral Gs
		gForces = td6->max_lateral_g * 32;
		gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, COLOUR_BLACK, x, y);
		y += 10;

		// If .TD6
		if (td6->version_and_colour_scheme / 4 >= 2) {
			if (td6->total_air_time != 0) {
				// Total air time
				sint32 airTime = td6->total_air_time * 25;
				gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, COLOUR_BLACK, x, y);
				y += 10;
			}
		}
	}

	if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
		// Drops
		uint16 drops = td6->drops & 0x3F;
		gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
		y += 10;

		// Drop height is multiplied by 0.75
		gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &drops, COLOUR_BLACK, x, y);
		y += 10;
	}

	if (td6->type != RIDE_TYPE_MINI_GOLF) {
		uint16 inversions = td6->inversions & 0x1F;
		if (inversions != 0) {
			// Inversions
			gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
			y += 10;
		}
	}
	y += 4;

	if (td6->space_required_x != 0xFF) {
		// Space required
		set_format_arg(0, uint16, td6->space_required_x);
		set_format_arg(2, uint16, td6->space_required_y);
		gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, gCommonFormatArgs, COLOUR_BLACK, x, y);
		y += 10;
	}

	if (td6->cost != 0) {
		gfx_draw_string_left(dpi, STR_TRACK_LIST_COST_AROUND, &td6->cost, COLOUR_BLACK, x, y);
	}
}

/**
 *
 *  rct2: 0x006CF8CD
 */
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	uint8 paletteIndex = ColourMapA[w->colours[0]].mid_light;
	gfx_clear(dpi, paletteIndex);

	sint32 x = 0;
	sint32 y = 0;
	size_t listIndex = 0;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
		if (_trackDesignsCount == 0) {
			// No track designs
			gfx_draw_string_left(dpi, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE, NULL, COLOUR_BLACK, x, y - 1);
			return;
		}
	} else {
		// Build custom track item
		rct_string_id stringId;
		if (listIndex == (size_t)w->selected_list_item) {
			// Highlight
			gfx_filter_rect(dpi, x, y, w->width, y + 9, PALETTE_DARKEN_1);
			stringId = STR_WINDOW_COLOUR_2_STRINGID;
		} else {
			stringId = STR_BLACK_STRING;
		}

		rct_string_id stringId2 = STR_BUILD_CUSTOM_DESIGN;
		gfx_draw_string_left(dpi, stringId, &stringId2, COLOUR_BLACK, x, y - 1);
		y += 10;
		listIndex++;
	}

	for (size_t i = 0; i < _trackDesignsCount; i++, listIndex++) {
		if (y + 10 >= dpi->y && y < dpi->y + dpi->height) {
			rct_string_id stringId;
			if (listIndex == (size_t)w->selected_list_item) {
				// Highlight
				gfx_filter_rect(dpi, x, y, w->width, y + 9, PALETTE_DARKEN_1);
				stringId = STR_WINDOW_COLOUR_2_STRINGID;
			} else {
				stringId = STR_BLACK_STRING;
			}

			// Draw track name
			set_format_arg(0, rct_string_id, STR_TRACK_LIST_NAME_FORMAT);
			set_format_arg(2, utf8*, _trackDesigns[i].name);
			gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y - 1);
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
	_trackDesignsCount = track_repository_get_items_for_ride(&_trackDesigns, item.type, entryPtr);
}

static bool track_list_load_design_for_preview(utf8 *path)
{
	// Dispose currently loaded track
	track_design_dispose(_loadedTrackDesign);
	_loadedTrackDesign = NULL;

	_loadedTrackDesign = track_design_open(path);
	if (_loadedTrackDesign != NULL && drawing_engine_get_type() != DRAWING_ENGINE_OPENGL) {
		track_design_draw_preview(_loadedTrackDesign, _trackDesignPreviewPixels);
		return true;
	}
	return false;
}
