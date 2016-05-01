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
#include "../cheats.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../ride/track.h"
#include "../ride/track_data.h"
#include "../ride/track_design.h"
#include "../sprites.h"
#include "../util/util.h"

#define TRACK_MINI_PREVIEW_WIDTH	168
#define TRACK_MINI_PREVIEW_HEIGHT	78
#define TRACK_MINI_PREVIEW_SIZE		(TRACK_MINI_PREVIEW_WIDTH * TRACK_MINI_PREVIEW_HEIGHT)

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_ROTATE,
	WIDX_MIRROR,
	WIDX_SELECT_DIFFERENT_DESIGN,
	WIDX_PRICE
};

static rct_widget window_track_place_widgets[] = {
	{ WWT_FRAME,			0,	0,		199,	0,		123,	0xFFFFFFFF,						STR_NONE									},
	{ WWT_CAPTION,			0,	1,		198,	1,		14,		3155,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	187,	197,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_FLATBTN,			0,	173,	196,	83,		106,	SPR_ROTATE_ARROW,				STR_ROTATE_90_TIP							},
	{ WWT_FLATBTN,			0,	173,	196,	59,		82,		5170,							STR_MIRROR_IMAGE_TIP						},
	{ WWT_DROPDOWN_BUTTON,	0,	4,		195,	109,	120,	STR_SELECT_A_DIFFERENT_DESIGN,	STR_GO_BACK_TO_DESIGN_SELECTION_WINDOW_TIP	},
	{ WWT_EMPTY,			0,	0,		0,		0,		0,		0xFFFFFFFF,						STR_NONE									},
	{ WIDGETS_END },
};

static void window_track_place_close(rct_window *w);
static void window_track_place_mouseup(rct_window *w, int widgetIndex);
static void window_track_place_update(rct_window *w);
static void window_track_place_toolupdate(rct_window* w, int widgetIndex, int x, int y);
static void window_track_place_tooldown(rct_window* w, int widgetIndex, int x, int y);
static void window_track_place_toolabort(rct_window *w, int widgetIndex);
static void window_track_place_unknown14(rct_window *w);
static void window_track_place_invalidate(rct_window *w);
static void window_track_place_paint(rct_window *w, rct_drawpixelinfo *dpi);

static rct_window_event_list window_track_place_events = {
	window_track_place_close,
	window_track_place_mouseup,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_place_update,
	NULL,
	NULL,
	window_track_place_toolupdate,
	window_track_place_tooldown,
	NULL,
	NULL,
	window_track_place_toolabort,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_place_unknown14,
	NULL,
	NULL,
	NULL,
	NULL,
	window_track_place_invalidate,
	window_track_place_paint,
	NULL
};

static uint8 *_window_track_place_mini_preview;
static sint16 _window_track_place_last_x;
static sint16 _window_track_place_last_y;

static uint8 _window_track_place_last_was_valid;
static sint16 _window_track_place_last_valid_x;
static sint16 _window_track_place_last_valid_y;
static sint16 _window_track_place_last_valid_z;
static money32 _window_track_place_last_cost;

static rct_track_td6 *_trackDesign;

static void window_track_place_clear_provisional();
static int window_track_place_get_base_z(int x, int y);
static void window_track_place_attempt_placement(rct_track_td6 *td6, int x, int y, int z, int bl, money32 *cost, uint8 *rideIndex);

static void window_track_place_clear_mini_preview();
static void window_track_place_draw_mini_preview(rct_track_td6 *td6);
static void window_track_place_draw_mini_preview_track(rct_track_td6 *td6, int pass, rct_xy16 origin, rct_xy16 *min, rct_xy16 *max);
static void window_track_place_draw_mini_preview_maze(rct_track_td6 *td6, int pass, rct_xy16 origin, rct_xy16 *min, rct_xy16 *max);
static rct_xy16 draw_mini_preview_get_pixel_position(sint16 x, sint16 y);
static bool draw_mini_preview_is_pixel_in_bounds(rct_xy16 pixel);
static uint8 *draw_mini_preview_get_pixel_ptr(rct_xy16 pixel);

/**
 *
 *  rct2: 0x006D182E
 */
static void window_track_place_clear_mini_preview()
{
	memset(_window_track_place_mini_preview, 220, TRACK_MINI_PREVIEW_SIZE);
}

#define swap(x, y) x = x ^ y; y = x ^ y; x = x ^ y;

/**
 *
 *  rct2: 0x006CFCA0
 */
void window_track_place_open(const track_design_file_ref *tdFileRef)
{
	rct_track_td6 *td6 = track_design_open(tdFileRef->path);
	if (td6 == NULL) {
		return;
	}

	window_close_construction_windows();

	_window_track_place_mini_preview = malloc(TRACK_MINI_PREVIEW_SIZE);
	window_track_place_clear_mini_preview();

	rct_window *w = window_create(
		0,
		29,
		200,
		124,
		&window_track_place_events,
		WC_TRACK_DESIGN_PLACE,
		0
	);
	w->widgets = window_track_place_widgets;
	w->enabled_widgets = 4 | 8 | 0x10 | 0x20;
	window_init_scroll_widgets(w);
	tool_set(w, 6, 12);
	gInputFlags |= INPUT_FLAG_6;
	window_push_others_right(w);
	show_gridlines();
	_window_track_place_last_cost = MONEY32_UNDEFINED;
	_window_track_place_last_x = 0xFFFF;
	_currentTrackPieceDirection = (2 - get_current_rotation()) & 3;
	window_track_place_draw_mini_preview(td6);

	char *title = (char*)language_get_string(3155);
	format_string(title, STR_TRACK_LIST_NAME_FORMAT, &td6->name);
	
	_trackDesign = td6;
}

/**
 *
 *  rct2: 0x006D0119
 */
static void window_track_place_close(rct_window *w)
{
	window_track_place_clear_provisional();
	viewport_set_visibility(0);
	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~6;
	hide_gridlines();
	SafeFree(_window_track_place_mini_preview);
	track_design_dispose(_trackDesign);
	_trackDesign = NULL;
}

/**
 *
 *  rct2: 0x006CFEAC
 */
static void window_track_place_mouseup(rct_window *w, int widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_ROTATE:
		window_track_place_clear_provisional();
		_currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
		window_invalidate(w);
		_window_track_place_last_x = 0xFFFF;
		window_track_place_draw_mini_preview(_trackDesign);
		break;
	case WIDX_MIRROR:
		track_design_mirror(_trackDesign);
		_currentTrackPieceDirection = (0 - _currentTrackPieceDirection) & 3;
		window_invalidate(w);
		_window_track_place_last_x = 0xFFFF;
		window_track_place_draw_mini_preview(_trackDesign);
		break;
	case WIDX_SELECT_DIFFERENT_DESIGN:
		window_close(w);
		window_track_list_open(_window_track_list_item);
		break;
	}
}

/**
 *
 *  rct2: 0x006CFCA0
 */
static void window_track_place_update(rct_window *w)
{
	if (!(gInputFlags & INPUT_FLAG_TOOL_ACTIVE))
		if (gCurrentToolWidget.window_classification != WC_TRACK_DESIGN_PLACE)
			window_close(w);
}

/**
 *
 *  rct2: 0x006CFF2D
 */
static void window_track_place_toolupdate(rct_window* w, int widgetIndex, int x, int y)
{
	int i;
	short mapX, mapY, mapZ;
	money32 cost;
	uint8 rideIndex;

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~7;

	// Get the tool map position
	sub_68A15E(x, y, &mapX, &mapY, NULL, NULL);
	if (mapX == (short)0x8000) {
		window_track_place_clear_provisional();
		return;
	}

	// Check if tool map position has changed since last update
	if (mapX == _window_track_place_last_x && mapY == _window_track_place_last_y) {
		sub_6D01B3(_trackDesign, PTD_OPERATION_DRAW_OUTLINES, 0, mapX, mapY, 0);
		return;
	}

	cost = MONEY32_UNDEFINED;

	// Get base Z position
	mapZ = window_track_place_get_base_z(mapX, mapY);
	if (game_is_not_paused() || gCheatsBuildInPauseMode) {
		window_track_place_clear_provisional();

		// Try increasing Z until a feasible placement is found
		for (i = 0; i < 7; i++) {
			window_track_place_attempt_placement(_trackDesign, mapX, mapY, mapZ, 105, &cost, &rideIndex);
			if (cost != MONEY32_UNDEFINED) {
				RCT2_GLOBAL(0x00F440EB, uint16) = rideIndex;
				_window_track_place_last_valid_x = mapX;
				_window_track_place_last_valid_y = mapY;
				_window_track_place_last_valid_z = mapZ;
				_window_track_place_last_was_valid = 1;
				break;
			}
			mapZ += 8;
		}
	}

	_window_track_place_last_x = mapX;
	_window_track_place_last_y = mapY;
	if (cost != _window_track_place_last_cost) {
		_window_track_place_last_cost = cost;
		widget_invalidate(w, WIDX_PRICE);
	}

	sub_6D01B3(_trackDesign, PTD_OPERATION_DRAW_OUTLINES, 0, mapX, mapY, mapZ);
}

/**
 *
 *  rct2: 0x006CFF34
 */
static void window_track_place_tooldown(rct_window* w, int widgetIndex, int x, int y)
{
	int i;
	short mapX, mapY, mapZ;
	money32 cost;
	uint8 rideIndex;

	window_track_place_clear_provisional();
	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~7;

	sub_68A15E(x, y, &mapX, &mapY, NULL, NULL);
	if (mapX == (short)0x8000)
		return;

	// Try increasing Z until a feasible placement is found
	mapZ = window_track_place_get_base_z(mapX, mapY);
	for (i = 0; i < 7; i++) {
		RCT2_GLOBAL(0x009A8C29, uint8) |= 1;
		window_track_place_attempt_placement(_trackDesign, mapX, mapY, mapZ, 1, &cost, &rideIndex);
		RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

		if (cost != MONEY32_UNDEFINED) {
			window_close_by_class(WC_ERROR);
			audio_play_sound_at_location(SOUND_PLACE_ITEM, mapX, mapY, mapZ);

			_currentRideIndex = rideIndex;
			if (RCT2_GLOBAL(0x00F4414E, uint8) & 1) {
				window_ride_main_open(rideIndex);
				window_close(w);
			} else {
				sub_6CC3FB(rideIndex);
				w = window_find_by_class(WC_RIDE_CONSTRUCTION);
				window_event_mouse_up_call(w, 29);
			}
			return;
		}

		// Check if player did not have enough funds
		if (gGameCommandErrorText == 827)
			break;

		mapZ += 8;
	}

	// Unable to build track
	audio_play_sound_at_location(SOUND_ERROR, mapX, mapY, mapZ);
}

/**
 *
 *  rct2: 0x006D015C
 */
static void window_track_place_toolabort(rct_window *w, int widgetIndex)
{
	window_track_place_clear_provisional();
}

/**
 *
 *  rct2: 0x006CFF01
 */
static void window_track_place_unknown14(rct_window *w)
{
	window_track_place_draw_mini_preview(_trackDesign);
}

static void window_track_place_invalidate(rct_window *w)
{
	colour_scheme_update(w);
}

/**
 *
 *  rct2: 0x006D017F
 */
static void window_track_place_clear_provisional()
{
	if (_window_track_place_last_was_valid) {
		sub_6D01B3(
			_trackDesign,
			PTD_OPERATION_CLEAR_OUTLINES,
			RCT2_GLOBAL(0x00F440EB, uint8),
			_window_track_place_last_valid_x,
			_window_track_place_last_valid_y,
			_window_track_place_last_valid_z
		);
		_window_track_place_last_was_valid = 0;
	}
}

/**
 *
 *  rct2: 0x006D17C6
 */
static int window_track_place_get_base_z(int x, int y)
{
	rct_map_element *mapElement;
	int z;

	mapElement = map_get_surface_element_at(x >> 5, y >> 5);
	z = mapElement->base_height * 8;

	// Increase Z above slope
	if (mapElement->properties.surface.slope & 0x0F) {
		z += 16;

		// Increase Z above double slope
		if (mapElement->properties.surface.slope & 0x10)
			z += 16;
	}

	// Increase Z above water
	if (mapElement->properties.surface.terrain & 0x1F)
		z = max(z, (mapElement->properties.surface.terrain & 0x1F) << 4);

	return z + sub_6D01B3(_trackDesign, PTD_OPERATION_GET_PLACE_Z, 0, x, y, z);
}

static void window_track_place_attempt_placement(rct_track_td6 *td6, int x, int y, int z, int bl, money32 *cost, uint8 *rideIndex)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	money32 result;

	edx = esi = ebp = 0;
	eax = x;
	ebx = bl;
	ecx = y;
	edi = z;
	
	gActiveTrackDesign = _trackDesign;
	result = game_do_command_p(GAME_COMMAND_PLACE_TRACK_DESIGN, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp);
	gActiveTrackDesign = NULL;

	if (cost != NULL) *cost = result;
	if (rideIndex != NULL) *rideIndex = edi & 0xFF;
}

/**
 *
 *  rct2: 0x006CFD9D
 */
static void window_track_place_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);

	// Draw mini tile preview
	rct_drawpixelinfo clippedDpi;
	if (clip_drawpixelinfo(&clippedDpi, dpi, w->x + 4, w->y + 18, 168, 78)) {
		rct_g1_element *substituteElement = &g1Elements[0];
		rct_g1_element tmpElement = *substituteElement;
		substituteElement->offset = _window_track_place_mini_preview;
		substituteElement->width = TRACK_MINI_PREVIEW_WIDTH;
		substituteElement->height = TRACK_MINI_PREVIEW_HEIGHT;
		substituteElement->x_offset = 0;
		substituteElement->y_offset = 0;
		substituteElement->flags = 0;
		gfx_draw_sprite(&clippedDpi, 0, 0, 0, 0);
		*substituteElement = tmpElement;
	}

	// Price
	if (_window_track_place_last_cost != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY)) {
		gfx_draw_string_centred(dpi, STR_COST_LABEL, w->x + 88, w->y + 94, 0, &_window_track_place_last_cost);
	}
}

/**
 *
 *  rct2: 0x006D1845
 */
static void window_track_place_draw_mini_preview(rct_track_td6 *td6)
{
	window_track_place_clear_mini_preview();

	// First pass is used to determine the width and height of the image so it can centre it
	rct_xy16 min = { 0, 0 };
	rct_xy16 max = { 0, 0 };
	for (int pass = 0; pass < 2; pass++) {
		rct_xy16 origin = { 0, 0 };
		if (pass == 1) {
			origin.x -= ((max.x + min.x) >> 6) << 5;
			origin.y -= ((max.y + min.y) >> 6) << 5;
		}

		if (td6->type == RIDE_TYPE_MAZE) {
			window_track_place_draw_mini_preview_maze(td6, pass, origin, &min, &max);
		} else {
			window_track_place_draw_mini_preview_track(td6, pass, origin, &min, &max);
		}
	}
}

static void window_track_place_draw_mini_preview_track(rct_track_td6 *td6, int pass, rct_xy16 origin, rct_xy16 *min, rct_xy16 *max)
{
	uint8 rotation = (_currentTrackPieceDirection + get_current_rotation()) & 3;
	rct_td6_track_element *trackElement = td6->track_elements;
	while (trackElement->type != 255) {
		int trackType = trackElement->type;
		if (trackType == TRACK_ELEM_INVERTED_90_DEG_UP_TO_FLAT_QUARTER_LOOP) {
			trackType = 255;
		}

		// Follow a single track piece shape
		const rct_preview_track *trackBlock = TrackBlocks[trackType];
		while (trackBlock->index != 255) {
			sint16 x = origin.x;
			sint16 y = origin.y;
			map_offset_with_rotation(&x, &y, trackBlock->x, trackBlock->y, rotation);

			if (pass == 0) {
				min->x = min(min->x, x);
				max->x = max(max->x, x);
				min->y = min(min->y, y);
				max->y = max(max->y, y);
			} else {
				rct_xy16 pixelPosition = draw_mini_preview_get_pixel_position(x, y);
				if (draw_mini_preview_is_pixel_in_bounds(pixelPosition)) {
					uint8 *pixel = draw_mini_preview_get_pixel_ptr(pixelPosition);

					uint8 bits = trackBlock->var_08 << (rotation & 3);
					bits = (bits & 0x0F) | ((bits & 0xF0) >> 4);

					// Station track is a lighter colour
					uint8 colour = RCT2_ADDRESS(0x0099BA64, uint8)[trackType * 16] & 0x10 ? 222 : 218;

					for (int i = 0; i < 4; i++) {
						if (bits & 1) pixel[338 + i] = colour;
						if (bits & 2) pixel[168 + i] = colour;
						if (bits & 4) pixel[  2 + i] = colour;
						if (bits & 8) pixel[172 + i] = colour;
					}
				}
			}
			trackBlock++;
		}

		// Change rotation and next position based on track curvature
		rotation &= 3;
		const rct_track_coordinates* track_coordinate = &TrackCoordinates[trackType];

		trackType *= 10;
		map_offset_with_rotation(&origin.x, &origin.y, track_coordinate->x, track_coordinate->y, rotation);
		rotation += track_coordinate->rotation_end - track_coordinate->rotation_begin;
		rotation &= 3;
		if (track_coordinate->rotation_end & 4) {
			rotation |= 4;
		}
		if (!(rotation & 4)) {
			origin.x += TileDirectionDelta[rotation].x;
			origin.y += TileDirectionDelta[rotation].y;
		}
		trackElement++;
	}
}

static void window_track_place_draw_mini_preview_maze(rct_track_td6 *td6, int pass, rct_xy16 origin, rct_xy16 *min, rct_xy16 *max)
{
	uint8 rotation = (_currentTrackPieceDirection + get_current_rotation()) & 3;
	rct_td6_maze_element *mazeElement = td6->maze_elements;
	while (mazeElement->all != 0) {
		sint16 x = mazeElement->x * 32;
		sint16 y = mazeElement->y * 32;
		rotate_map_coordinates(&x, &y, rotation);

		x += origin.x;
		y += origin.y;

		if (pass == 0) {
			min->x = min(min->x, x);
			max->x = max(max->x, x);
			min->y = min(min->y, y);
			max->y = max(max->y, y);
		} else {
			rct_xy16 pixelPosition = draw_mini_preview_get_pixel_position(x, y);
			if (draw_mini_preview_is_pixel_in_bounds(pixelPosition)) {
				uint8 *pixel = draw_mini_preview_get_pixel_ptr(pixelPosition);

				// Entrance or exit is a lighter colour
				uint8 colour = mazeElement->type == 8 || mazeElement->type == 128 ? 222 : 218;

				for (int i = 0; i < 4; i++) {
					pixel[338 + i] = colour;
					pixel[168 + i] = colour;
					pixel[  2 + i] = colour;
					pixel[172 + i] = colour;
				}
			}
		}
		mazeElement++;
	}
}

static rct_xy16 draw_mini_preview_get_pixel_position(sint16 x, sint16 y)
{
	return (rct_xy16) {
		80 + ((y / 32) - (x / 32)) * 4,
		38 + ((y / 32) + (x / 32)) * 2
	};
}

static bool draw_mini_preview_is_pixel_in_bounds(rct_xy16 pixel)
{
	return pixel.x >= 0 && pixel.y >= 0 && pixel.x <= 160 && pixel.y <= 75;
}

static uint8 *draw_mini_preview_get_pixel_ptr(rct_xy16 pixel)
{
	return &_window_track_place_mini_preview[pixel.y * TRACK_MINI_PREVIEW_WIDTH + pixel.x];
}
