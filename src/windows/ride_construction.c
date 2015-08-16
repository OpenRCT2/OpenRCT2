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

#include "../addresses.h"
#include "../audio/audio.h"
#include "../cheats.h"
#include "../drawing/drawing.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../ride/track.h"
#include "dropdown.h"

/* move to ride.c */
void sub_6B2FA9(rct_windownumber number)
{
	rct_window* w;

	w = window_find_by_number(WC_RIDE, number);
	if (w != NULL && w->page == 1)
		window_close(w);
}

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_DIRECTION_GROUPBOX,
	WIDX_SLOPE_GROUPBOX,
	WIDX_BANKING_GROUPBOX,
	WIDX_LEFT_CURVE_VERY_SMALL,
	WIDX_LEFT_CURVE_SMALL,
	WIDX_LEFT_CURVE,
	WIDX_STRAIGHT,
	WIDX_RIGHT_CURVE,
	WIDX_RIGHT_CURVE_SMALL,
	WIDX_RIGHT_CURVE_VERY_SMALL,
	WIDX_SPECIAL_TRACK_DROPDOWN,
	WIDX_SLOPE_DOWN_STEEP,
	WIDX_SLOPE_DOWN,
	WIDX_LEVEL,
	WIDX_SLOPE_UP,
	WIDX_SLOPE_UP_STEEP,
	WIDX_CHAIN_LIFT,
	WIDX_BANK_LEFT,
	WIDX_BANK_STRAIGHT,
	WIDX_BANK_RIGHT,
	WIDX_CONSTRUCT,
	WIDX_DEMOLISH,
	WIDX_LEFT_CURVE_LARGE,
	WIDX_PREVIOUS_SECTION,
	WIDX_NEXT_SECTION,
	WIDX_ENTRANCE_EXIT_GROUPBOX,
	WIDX_ENTRANCE,
	WIDX_EXIT,
	WIDX_RIGHT_CURVE_LARGE,
	WIDX_ROTATE,
	WIDX_U_TRACK,
	WIDX_O_TRACK,
	WIDX_SEAT_ROTATION_GROUPBOX,
	WIDX_SEAT_ROTATION_ANGLE_SPINNER,
	WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP,
	WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN,
};

static rct_widget window_ride_construction_widgets[] = {
	{ WWT_FRAME,			0,	0,		165,	0,		393,	0xFFFFFFFF,							STR_NONE												},
	{ WWT_CAPTION,			0,	1,		164,	1,		14,		896,								STR_WINDOW_TITLE_TIP									},
	{ WWT_CLOSEBOX,			0,	153,	163,	2,		13,		STR_CLOSE_X,						STR_CLOSE_WINDOW_TIP									},
	{ WWT_GROUPBOX,			0,	3,		162,	17,		73,		STR_RIDE_CONSTRUCTION_DIRECTION,	STR_NONE												},
	{ WWT_GROUPBOX,			0,	3,		162,	76,		116,	STR_RIDE_CONSTRUCTION_SLOPE,		STR_NONE												},
	{ WWT_GROUPBOX,			0,	3,		162,	120,	160,	STR_RIDE_CONSTRUCTION_ROLL_BANKING,	STR_NONE												},
	{ WWT_FLATBTN,			1,	6,		27,		29,		52,		0x1414,								STR_RIDE_CONSTRUCTION_LEFT_CURVE_VERY_SMALL_TIP			},
	{ WWT_FLATBTN,			1,	6,		27,		29,		52,		0x1414,								STR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL_TIP				},
	{ WWT_FLATBTN,			1,	28,		49,		29,		52,		0x1412,								STR_RIDE_CONSTRUCTION_LEFT_CURVE_TIP					},
	{ WWT_FLATBTN,			1,	72,		93,		29,		52,		0x1411,								STR_RIDE_CONSTRUCTION_STRAIGHT_TIP						},
	{ WWT_FLATBTN,			1,	116,	137,	29,		52,		0x1413,								STR_RIDE_CONSTRUCTION_RIGHT_CURVE_TIP					},
	{ WWT_FLATBTN,			1,	138,	159,	29,		52,		0x1415,								STR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL_TIP				},
	{ WWT_FLATBTN,			1,	138,	159,	29,		52,		0x1415,								STR_RIDE_CONSTRUCTION_RIGHT_CURVE_VERY_SMALL_TIP		},
	{ WWT_DROPDOWN_BUTTON,	1,	17,		148,	55,		68,		1384,								STR_RIDE_CONSTRUCTION_OTHER_TRACK_CONFIGURATIONS_TIP	},
	{ WWT_FLATBTN,			1,	23,		46,		88,		111,	0x1418,								STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP				},
	{ WWT_FLATBTN,			1,	47,		70,		88,		111,	0x1419,								STR_RIDE_CONSTRUCTION_SLOPE_DOWN_TIP					},
	{ WWT_FLATBTN,			1,	71,		94,		88,		111,	0x141A,								STR_RIDE_CONSTRUCTION_LEVEL_TIP							},
	{ WWT_FLATBTN,			1,	95,		118,	88,		111,	0x141B,								STR_RIDE_CONSTRUCTION_SLOPE_UP_TIP						},
	{ WWT_FLATBTN,			1,	119,	142,	88,		111,	0x141C,								STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP				},
	{ WWT_FLATBTN,			1,	134,	157,	88,		111,	0x142B,								STR_RIDE_CONSTRUCTION_CHAIN_LIFT_TIP					},
	{ WWT_FLATBTN,			1,	47,		70,		132,	155,	0x1421,								STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP			},
	{ WWT_FLATBTN,			1,	71,		94,		132,	155,	0x1422,								STR_RIDE_CONSTRUCTION_NO_ROLL_TIP						},
	{ WWT_FLATBTN,			1,	95,		118,	132,	155,	0x1423,								STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP			},
	{ WWT_IMGBTN,			1,	3,		162,	164,	333,	0xFFFFFFFF,							STR_RIDE_CONSTRUCTION_CONSTRUCT_SELECTED_SECTION_TIP	},
	{ WWT_FLATBTN,			1,	60,		105,	338,	361,	0x142A,								STR_RIDE_CONSTRUCTION_REMOVE_HIGHLIGHTED_SECTION_TIP	},
	{ WWT_FLATBTN,			1,	50,		71,		29,		52,		0x1416,								STR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE_TIP				},
	{ WWT_FLATBTN,			1,	20,		43,		338,	361,	0x1428,								STR_RIDE_CONSTRUCTION_MOVE_TO_PREVIOUS_SECTION_TIP		},
	{ WWT_FLATBTN,			1,	122,	145,	338,	361,	0x1429,								STR_RIDE_CONSTRUCTION_MOVE_TO_NEXT_SECTION_TIP			},
	{ WWT_GROUPBOX,			0,	3,		162,	362,	389,	0xFFFFFFFF,							STR_NONE												},
	{ WWT_DROPDOWN_BUTTON,	1,	9,		78,		372,	383,	STR_RIDE_CONSTRUCTION_ENTRANCE,		STR_RIDE_CONSTRUCTION_ENTRANCE_TIP						},
	{ WWT_DROPDOWN_BUTTON,	1,	87,		156,	372,	383,	STR_RIDE_CONSTRUCTION_EXIT,			STR_RIDE_CONSTRUCTION_EXIT_TIP							},
	{ WWT_FLATBTN,			1,	94,		115,	29,		52,		0x1417,								STR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE_TIP				},
	{ WWT_FLATBTN,			1,	72,		95,		338,	361,	0x1431,								STR_ROTATE_90_TIP										},
	{ WWT_FLATBTN,			1,	19,		42,		132,	155,	0x1424,								STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP			},
	{ WWT_FLATBTN,			1,	123,	146,	132,	155,	0x1425,								STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP		},
	{ WWT_GROUPBOX,			0,	96,		162,	120,	160,	STR_RIDE_CONSTRUCTION_SEAT_ROT,		STR_NONE												},
	{ WWT_SPINNER,			1,	101,	158,	138,	149,	0,									STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	147,	157,	139,	143,	STR_NUMERIC_UP,						STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	147,	157,	144,	148,	STR_NUMERIC_DOWN,					STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP	},
	{ WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_ride_construction_close(rct_window *w);
static void window_ride_construction_mouseup(rct_window *w, int widgetIndex);
static void window_ride_construction_resize(rct_window *w);
static void window_ride_construction_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_construction_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_construction_update(rct_window *w);
static void window_ride_construction_toolupdate(rct_window* w, int widgetIndex, int x, int y);
static void window_ride_construction_tooldown(rct_window* w, int widgetIndex, int x, int y);
static void window_ride_construction_invalidate(rct_window *w);
static void window_ride_construction_paint(rct_window *w, rct_drawpixelinfo *dpi);

//0x993EEC
static rct_window_event_list window_ride_construction_events = {
	window_ride_construction_close,
	window_ride_construction_mouseup,
	window_ride_construction_resize,
	window_ride_construction_mousedown,
	window_ride_construction_dropdown,
	NULL,
	window_ride_construction_update,
	NULL,
	NULL,
	window_ride_construction_toolupdate,
	window_ride_construction_tooldown,
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
	window_ride_construction_invalidate,
	window_ride_construction_paint,
	NULL
};

#pragma endregion

#pragma region RideConfigurationStringIds

// rct2: 0x00999492
rct_string_id RideConfigurationStringIds[] = {
	0,										// 0
	STR_STATION_PLATFORM,					// 1
	0,										// 2
	0,										// 3
	0,										// 4
	0,										// 5
	0,										// 6
	0,										// 7
	0,										// 8
	0,										// 9
	0,										// 10
	0,										// 11
	0,										// 12
	0,										// 13
	0,										// 14
	0,										// 15
	0,										// 16
	0,										// 17
	0,										// 18
	0,										// 19
	0,										// 20
	0,										// 21
	0,										// 22
	0,										// 23
	0,										// 24
	0,										// 25
	0,										// 26
	0,										// 27
	0,										// 28
	0,										// 29
	0,										// 30
	0,										// 31
	0,										// 32
	0,										// 33
	0,										// 34
	0,										// 35
	0,										// 36
	0,										// 37
	STR_S_BEND_LEFT,						// 38
	STR_S_BEND_RIGHT,						// 39
	STR_VERTICAL_LOOP_LEFT,					// 40
	STR_VERTICAL_LOOP_RIGHT,				// 41
	0,										// 42
	0,										// 43
	0,										// 44
	0,										// 45
	0,										// 46
	0,										// 47
	0,										// 48
	0,										// 49
	0,										// 50
	0,										// 51
	STR_IN_LINE_TWIST_LEFT,					// 52
	STR_IN_LINE_TWIST_RIGHT,				// 53
	STR_IN_LINE_TWIST_LEFT,					// 54
	STR_IN_LINE_TWIST_RIGHT,				// 55
	STR_HALF_LOOP,							// 56
	STR_HALF_LOOP,							// 57
	STR_HALF_CORKSCREW_LEFT,				// 58
	STR_HALF_CORKSCREW_RIGHT,				// 59
	STR_HALF_CORKSCREW_LEFT,				// 60
	STR_HALF_CORKSCREW_RIGHT,				// 61
	0,										// 62
	0,										// 63
	0,										// 64
	0,										// 65
	STR_ENTRY_EXIT_PLATFORM,				// 66
	STR_VERTICAL_TOWER,						// 67
	0,										// 68
	0,										// 69
	0,										// 70
	0,										// 71
	0,										// 72
	0,										// 73
	0,										// 74
	0,										// 75
	0,										// 76
	0,										// 77
	0,										// 78
	0,										// 79
	0,										// 80
	0,										// 81
	0,										// 82
	STR_S_BEND_LEFT,						// 83
	STR_S_BEND_RIGHT,						// 84
	0,										// 85
	0,										// 86
	STR_HELIX_UP_SMALL,						// 87
	STR_HELIX_UP_SMALL,						// 88
	STR_HELIX_DOWN_SMALL,					// 89
	STR_HELIX_DOWN_SMALL,					// 90
	STR_HELIX_UP_LARGE,						// 91
	STR_HELIX_UP_LARGE,						// 92
	STR_HELIX_DOWN_LARGE,					// 93
	STR_HELIX_DOWN_LARGE,					// 94
	0,										// 95
	0,										// 96
	0,										// 97
	0,										// 98
	STR_BRAKES,								// 99
	STR_SPINNING_CONTROL_TOGGLE_TRACK,		// 100
	0,										// 101
	STR_HELIX_UP_LARGE,						// 102
	STR_HELIX_UP_LARGE,						// 103
	STR_HELIX_DOWN_LARGE,					// 104
	STR_HELIX_DOWN_LARGE,					// 105
	STR_HELIX_UP_LEFT,						// 106
	STR_HELIX_UP_RIGHT,						// 107
	STR_HELIX_DOWN_LEFT,					// 108
	STR_HELIX_DOWN_RIGHT,					// 109
	STR_BASE_SIZE_2_X_2,					// 110
	STR_BASE_SIZE_4_X_4,					// 111
	STR_WATERFALLS,							// 112
	STR_RAPIDS,								// 113
	STR_ON_RIDE_PHOTO_SECTION,				// 114
	STR_BASE_SIZE_2_X_4,					// 115
	STR_BASE_SIZE_5_X_1,					// 116
	STR_WATER_SPLASH,						// 117
	0,										// 118
	0,										// 119
	STR_WHIRLPOOL,							// 120
	0,										// 121
	0,										// 122
	STR_CABLE_LIFT_HILL,					// 123
	STR_SLOPE_UP_TO_VERTICAL,				// 124
	STR_VERTICAL_TRACK,						// 125
	0,										// 126
	0,										// 127
	0,										// 128
	0,										// 129
	0,										// 130
	0,										// 131
	STR_HOLDING_BRAKE_FOR_DROP,				// 132
	0,										// 133
	0,										// 134
	0,										// 135
	0,										// 136
	0,										// 137
	0,										// 138
	0,										// 139
	0,										// 140
	0,										// 141
	0,										// 142
	0,										// 143
	0,										// 144
	0,										// 145
	0,										// 146
	0,										// 147
	0,										// 148
	0,										// 149
	0,										// 150
	0,										// 151
	0,										// 152
	0,										// 153
	0,										// 154
	0,										// 155
	0,										// 156
	0,										// 157
	0,										// 158
	0,										// 159
	0,										// 160
	0,										// 161
	0,										// 162
	0,										// 163
	0,										// 164
	0,										// 165
	0,										// 166
	0,										// 167
	0,										// 168
	0,										// 169
	0,										// 170
	0,										// 171
	STR_REVERSER_TURNTABLE,					// 172
	STR_SPINNING_TUNNEL,					// 173
	STR_BARREL_ROLL_LEFT,					// 174
	STR_BARREL_ROLL_RIGHT,					// 175
	STR_BARREL_ROLL_LEFT,					// 176
	STR_BARREL_ROLL_RIGHT,					// 177
	0,										// 178
	0,										// 179
	0,										// 180
	0,										// 181
	STR_LAUNCHED_LIFT_HILL, 				// 182
	STR_LARGE_HALF_LOOP_LEFT,				// 183
	STR_LARGE_HALF_LOOP_RIGHT,				// 184
	STR_LARGE_HALF_LOOP_LEFT,				// 185
	STR_LARGE_HALF_LOOP_RIGHT,				// 186
	STR_IN_LINE_TWIST_LEFT,					// 187
	STR_IN_LINE_TWIST_RIGHT,				// 188
	STR_IN_LINE_TWIST_LEFT,					// 189
	STR_IN_LINE_TWIST_RIGHT,				// 190
	STR_HALF_LOOP,							// 191
	STR_HALF_LOOP,							// 192
	STR_HALF_CORKSCREW_LEFT,				// 193
	STR_HALF_CORKSCREW_RIGHT,				// 194
	STR_HALF_CORKSCREW_LEFT,				// 195
	STR_HALF_CORKSCREW_RIGHT,				// 196
	STR_UPPER_TRANSFER,						// 197
	STR_LOWER_TRANSFER,						// 198
	STR_HEARTLINE_ROLL_LEFT,				// 199
	STR_HEARTLINE_ROLL_RIGHT,				// 200
	STR_GOLF_HOLE_A,						// 201
	STR_GOLF_HOLE_B,						// 202
	STR_GOLF_HOLE_C,						// 203
	STR_GOLF_HOLE_D,						// 204
	STR_GOLF_HOLE_E,						// 205
	STR_QUARTER_LOOP,						// 206
	STR_QUARTER_LOOP,						// 207
	STR_QUARTER_LOOP,						// 208
	STR_CURVED_LIFT_HILL_LEFT,				// 209
	STR_CURVED_LIFT_HILL_RIGHT,				// 210
	STR_REVERSER_LEFT,						// 211
	STR_REVERSER_RIGHT,						// 212
	STR_TOP_SECTION,						// 213
	STR_VERTICAL_TRACK,						// 214
	STR_SLOPE_TO_LEVEL,						// 215
	STR_BLOCK_BRAKES,						// 216
	0,										// 217
	0,										// 218
	0,										// 219
	0,										// 220
	0,										// 221
	0,										// 222
	0,										// 223
	0,										// 224
	0,										// 225
	0,										// 226
	0,										// 227
	0,										// 228
	0,										// 229
	0,										// 230
	0,										// 231
	0,										// 232
	0,										// 233
	0,										// 234
	0,										// 235
	0,										// 236
	0,										// 237
	0,										// 238
	0,										// 239
	0,										// 240
	0,										// 241
	0,										// 242
	0,										// 243
	0,										// 244
	0,										// 245
	0,										// 246
	0,										// 247
	0,										// 248
	0,										// 249
	0,										// 250
	0,										// 251
	0,										// 252
	STR_QUARTER_LOOP,						// 253
	STR_QUARTER_LOOP,						// 254
	STR_QUARTER_LOOP						// 255
};

#pragma endregion

#define		_trackPlaceCtrlState				RCT2_GLOBAL(0x00F44159, uint8)
static int	_trackPlaceCtrlZ;
#define		_trackPlaceShiftState				RCT2_GLOBAL(0x00F4415C, uint8)
static int	_trackPlaceShiftStartScreenX;
static int	_trackPlaceShiftStartScreenY;
static int	_trackPlaceShiftZ;
#define		_trackPlaceZ						RCT2_GLOBAL(0x00F44163, sint16)

static void window_ride_construction_next_section(rct_window *w);
static void window_ride_construction_previous_section(rct_window *w);
static void window_ride_construction_construct(rct_window *w);
static void window_ride_construction_mouseup_demolish(rct_window* w);
static void window_ride_construction_rotate(rct_window *w);
static void window_ride_construction_entrance_click(rct_window *w);
static void window_ride_construction_exit_click(rct_window *w);

static void window_ride_construction_draw_track_piece(
	rct_window *w, rct_drawpixelinfo *dpi,
	int rideIndex, int trackType, int trackRotation, int unknown,
	int width, int height
);
static void window_ride_construction_update_enabled_track_pieces();
static bool sub_6CA2DF(int *trackType, int *trackDirection, int *rideIndex, int *edxRS16, int *x, int *y, int *z, int *properties);
static void sub_6CBCE2(
	int rideIndex, int trackType, int trackDirection, int edx,
	int originX, int originY, int originZ
);
static void window_ride_construction_update_map_selection();
static void window_ride_construction_update_possible_ride_configurations();
static void window_ride_construction_update_widgets(rct_window *w);
static void window_ride_construction_select_map_tiles(rct_ride *ride, int trackType, int trackDirection, int x, int y);
money32 sub_6CA162(int rideIndex, int trackType, int trackDirection, int edxRS16, int x, int y, int z);
static void window_ride_construction_show_special_track_dropdown(rct_window *w, rct_widget *widget);
static void ride_selected_track_set_seat_rotation(int seatRotation);
static void loc_6C7502(int al);
static void ride_construction_set_brakes_speed(int brakesSpeed);
static void ride_construction_tooldown_entrance_exit(int screenX, int screenY);

uint8 *_currentPossibleRideConfigurations = (uint8*)0x00F4407C;

static bool is_track_enabled(int trackFlagIndex)
{
	return _enabledRidePieces & (1ULL << trackFlagIndex);
}

static int ride_get_alternative_type(rct_ride *ride)
{
	return _currentTrackCovered & 2 ?
		RCT2_ADDRESS(0x0097D4F5, uint8)[ride->type * 8] :
		ride->type;;
}

/**
 *
 * rct2: 0x006CB481
 */
rct_window *window_ride_construction_open()
{
	int rideIndex = _currentRideIndex;
	sub_6B2FA9(rideIndex);

	rct_window *w;
	rct_ride* ride = GET_RIDE(rideIndex);

	if (ride->type == RIDE_TYPE_MAZE)
		return window_maze_construction_open();

	w = window_create(0, 29, 166, 394, &window_ride_construction_events, WC_RIDE_CONSTRUCTION, WF_9);

	w->widgets = window_ride_construction_widgets;
	w->enabled_widgets = 0x67EFFFFFC4;

	window_init_scroll_widgets(w);

	w->colours[0] = 24;
	w->colours[1] = 24;
	w->colours[2] = 24;

	w->number = rideIndex;

	window_push_others_right(w);
	show_gridlines();

	_currentTrackPrice = MONEY32_UNDEFINED;
	RCT2_GLOBAL(0x00F440CD, uint8) = 8;
	RCT2_GLOBAL(0x00F440CE, uint8) = 18;
	RCT2_GLOBAL(0x00F440CF, uint8) = 4;

	if (ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER)
		RCT2_GLOBAL(0x00F440CE, uint8) = 30;

	if (ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
		RCT2_GLOBAL(0x00F440CE, uint8) = 30;

	_currentTrackCurve = RCT2_ADDRESS(0x0097CC68, uint8)[ride->type * 2] | 0x100;
	_currentTrackSlopeEnd = 0;
	RCT2_GLOBAL(0x00F440B3, uint8) = 0;
	_currentTrackLiftHill = 0;
	_currentTrackCovered = 0;

	if (RCT2_ADDRESS(0x0097D4F2, uint16)[ride->type * 4] & 0x8000)
		_currentTrackCovered |= 2;

	RCT2_GLOBAL(0x00F440B6, uint8) = 0;
	RCT2_GLOBAL(0x00F440B7, uint8) = 0;

	RCT2_GLOBAL(RCT2_ADDRESS_TRACK_PREVIEW_ROTATION, uint8) = 0;
	_rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
	_currentTrackSelectionFlags = 0;
	_rideConstructionArrowPulseTime = 0;
	RCT2_GLOBAL(0x00F44159, uint8) = 0;
	RCT2_GLOBAL(0x00F4415C, uint8) = 0;
	colour_scheme_update(w);
	return w;
}

/**
 *
 * rct2: 0x006C845D
 */
static void window_ride_construction_close(rct_window *w)
{
	rct_xy_element mapElement;

	sub_6C9627();
	viewport_set_visibility(0);

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 << 1);

	// In order to cancel the yellow arrow correctly the
	// selection tool should be cancelled.
	tool_cancel();

	hide_gridlines();

	uint8 rideIndex = _currentRideIndex;
	if (sub_6CAF80(rideIndex, &mapElement) || network_get_mode() == NETWORK_MODE_CLIENT) {
		window_ride_main_open(rideIndex);
	} else {
		int eax = RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = 0;
		game_do_command(0, 9, 0, rideIndex, GAME_COMMAND_DEMOLISH_RIDE, 0, 0);

		RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) = eax;
	}
}

/**
 *
 * rct2: 0x006C6E14
 */
static void window_ride_construction_mouseup(rct_window *w, int widgetIndex)
{
	window_ride_construction_update_enabled_track_pieces();
	switch (widgetIndex){
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_NEXT_SECTION:
		ride_select_next_section();
		break;
	case WIDX_PREVIOUS_SECTION:
		ride_select_previous_section();
		break;
	case WIDX_CONSTRUCT:
		window_ride_construction_construct(w);
		break;
	case WIDX_DEMOLISH:
		window_ride_construction_mouseup_demolish(w);
		break;
	case WIDX_ROTATE:
		window_ride_construction_rotate(w);
		break;
	case WIDX_ENTRANCE:
		window_ride_construction_entrance_click(w);
		break;
	case WIDX_EXIT:
		window_ride_construction_exit_click(w);
		break;
	}
}

/**
 *
 * rct2: 0x006C7934
 */
static void window_ride_construction_resize(rct_window *w)
{
	window_ride_construction_update_enabled_track_pieces();
	w->enabled_widgets &= ~(1 << WIDX_CONSTRUCT);
	if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_PLACE) {
		w->enabled_widgets |= (1 << WIDX_CONSTRUCT);
	}

	rct_ride *ride = GET_RIDE(_currentRideIndex);
	int rideType = ride_get_alternative_type(ride);

	uint64 disabledWidgets = 0;

	if (_currentTrackCurve >= 256) {
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_GROUPBOX) |
			(1ULL << WIDX_BANKING_GROUPBOX) |
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_SLOPE_DOWN) |
			(1ULL << WIDX_LEVEL) |
			(1ULL << WIDX_SLOPE_UP) |
			(1ULL << WIDX_SLOPE_UP_STEEP) |
			(1ULL << WIDX_CHAIN_LIFT) |
			(1ULL << WIDX_BANK_LEFT) |
			(1ULL << WIDX_BANK_STRAIGHT) |
			(1ULL << WIDX_BANK_RIGHT);
	}

	// Disable large curves if the start or end of the track is sloped.
	if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE) {
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE);
	}
	if (is_track_enabled(TRACK_SLOPE_CURVE) && is_track_enabled(TRACK_CURVE_VERY_SMALL)) {
		// Disable small curves if the start or end of the track is sloped.
		if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE) {
			disabledWidgets |=
				(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
				(1ULL << WIDX_LEFT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
		}
	}
	if (!is_track_enabled(TRACK_SLOPE_CURVE)) {
		if (is_track_enabled(TRACK_CURVE_VERTICAL)) {
			// Disable all curves only on vertical track
			if (_previousTrackSlopeEnd != TRACK_SLOPE_UP_90 || _currentTrackSlopeEnd != TRACK_SLOPE_UP_90) {
				if (_previousTrackSlopeEnd != TRACK_SLOPE_DOWN_90 || _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_90) {
					disabledWidgets |=
						(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
						(1ULL << WIDX_LEFT_CURVE_SMALL) |
						(1ULL << WIDX_LEFT_CURVE) |
						(1ULL << WIDX_RIGHT_CURVE) |
						(1ULL << WIDX_RIGHT_CURVE_SMALL) |
						(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
				}
			}
		} else {
			// Disable all curves on sloped track
			if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE) {
				disabledWidgets |=
					(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
					(1ULL << WIDX_LEFT_CURVE_SMALL) |
					(1ULL << WIDX_LEFT_CURVE) |
					(1ULL << WIDX_RIGHT_CURVE) |
					(1ULL << WIDX_RIGHT_CURVE_SMALL) |
					(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
			}
		}
	}
	if (!is_track_enabled(TRACK_FLAT_ROLL_BANKING)) {
		// Disable banking
		disabledWidgets |=
			(1ULL << WIDX_BANKING_GROUPBOX) |
			(1ULL << WIDX_BANK_LEFT) |
			(1ULL << WIDX_BANK_STRAIGHT) |
			(1ULL << WIDX_BANK_RIGHT);
	}
	if (!is_track_enabled(TRACK_SLOPE) && !is_track_enabled(TRACK_SLOPE_STEEP)) {
		if (rideType != RIDE_TYPE_REVERSE_FREEFALL_COASTER && rideType != RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
			// Disable all slopes
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_GROUPBOX) |
				(1ULL << WIDX_SLOPE_DOWN_STEEP) |
				(1ULL << WIDX_SLOPE_DOWN) |
				(1ULL << WIDX_LEVEL) |
				(1ULL << WIDX_SLOPE_UP) |
				(1ULL << WIDX_SLOPE_UP_STEEP);
		}
	}
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0) {
		disabledWidgets |=
			(1ULL << WIDX_CONSTRUCT) |
			(1ULL << WIDX_DEMOLISH) |
			(1ULL << WIDX_PREVIOUS_SECTION) |
			(1ULL << WIDX_NEXT_SECTION);
	}
	switch (_currentTrackCurve) {
	case TRACK_CURVE_LEFT_VERY_SMALL:
	case TRACK_CURVE_LEFT_SMALL:
	case TRACK_CURVE_LEFT:
	case TRACK_CURVE_LEFT_LARGE:
		disabledWidgets |= (1ULL << WIDX_BANK_RIGHT);
		if (_previousTrackBankEnd == TRACK_BANK_NONE) {
			disabledWidgets |= (1ULL << WIDX_BANK_LEFT);
		} else {
			disabledWidgets |= (1ULL << WIDX_BANK_STRAIGHT);
		}
		break;
	case TRACK_CURVE_RIGHT_LARGE:
	case TRACK_CURVE_RIGHT:
	case TRACK_CURVE_RIGHT_SMALL:
	case TRACK_CURVE_RIGHT_VERY_SMALL:
		disabledWidgets |= (1ULL << WIDX_BANK_LEFT);
		if (_previousTrackBankEnd == TRACK_BANK_NONE) {
			disabledWidgets |= (1ULL << WIDX_BANK_RIGHT);
		} else {
			disabledWidgets |= (1ULL << WIDX_BANK_STRAIGHT);
		}
		break;
	}
	if (!is_track_enabled(TRACK_SLOPE_ROLL_BANKING)) {
		if (_currentTrackBankEnd != TRACK_BANK_NONE) {
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_DOWN) |
				(1ULL << WIDX_SLOPE_UP);
		}
	}
	if (_previousTrackSlopeEnd == _currentTrackSlopeEnd) {
		switch (_currentTrackSlopeEnd) {
		case TRACK_SLOPE_UP_60:
		case TRACK_SLOPE_DOWN_60:
			disabledWidgets |=
				(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
				(1ULL << WIDX_LEFT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
			if (!is_track_enabled(TRACK_SLOPE_CURVE_STEEP)) {
				disabledWidgets |=
					(1ULL << WIDX_LEFT_CURVE_SMALL) |
					(1ULL << WIDX_RIGHT_CURVE_SMALL);
			}
			break;
		case TRACK_SLOPE_UP_90:
		case TRACK_SLOPE_DOWN_90:
			disabledWidgets |=
				(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
				(1ULL << WIDX_LEFT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
			if (!is_track_enabled(TRACK_CURVE_VERTICAL)) {
				disabledWidgets |=
					(1ULL << WIDX_LEFT_CURVE_SMALL) |
					(1ULL << WIDX_RIGHT_CURVE_SMALL);
			}
			break;
		}
	} else {
		// Disable all curves
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
	}

	switch (_previousTrackSlopeEnd) {
	case TRACK_SLOPE_NONE:
		if (_currentTrackCurve != TRACK_CURVE_NONE) {
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_DOWN_STEEP) |
				(1ULL << WIDX_SLOPE_UP_STEEP);
		}
		break;
	case TRACK_SLOPE_DOWN_25:
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_UP) |
			(1ULL << WIDX_SLOPE_UP_STEEP);
		break;
	case TRACK_SLOPE_DOWN_60:
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_UP) |
			(1ULL << WIDX_SLOPE_UP_STEEP);
		if (!is_track_enabled(TRACK_SLOPE_LONG) && !is_track_enabled(TRACK_SLOPE_STEEP_LONG)) {
			disabledWidgets |= (1ULL << WIDX_LEVEL);
		}
		break;
	case TRACK_SLOPE_UP_25:
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_SLOPE_DOWN);
		break;
	case TRACK_SLOPE_UP_60:
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_SLOPE_DOWN);
		if (!is_track_enabled(TRACK_SLOPE_LONG) && !is_track_enabled(TRACK_SLOPE_STEEP_LONG)) {
			disabledWidgets |= (1ULL << WIDX_LEVEL);
		}
		break;
	case TRACK_SLOPE_DOWN_90:
	case TRACK_SLOPE_UP_90:
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_DOWN) |
			(1ULL << WIDX_LEVEL) |
			(1ULL << WIDX_SLOPE_UP);
		break;
	}
	if (_previousTrackSlopeEnd == TRACK_SLOPE_NONE) {
		if (!is_track_enabled(TRACK_SLOPE_LONG) && !is_track_enabled(TRACK_SLOPE_STEEP_LONG)) {
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_DOWN_STEEP) |
				(1ULL << WIDX_SLOPE_UP_STEEP);
		}
	}
	if (is_track_enabled(TRACK_SLOPE_VERTICAL)) {
		if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 && _currentTrackPieceDirection < 4) {
			disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
		}
		if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_90) {
			disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
		}
		if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 && _currentTrackPieceDirection < 4) {
			disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
		}
	}
	if (_previousTrackBankEnd == TRACK_BANK_LEFT) {
		disabledWidgets |=
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE) |
			(1ULL << WIDX_BANK_RIGHT);
	}
	if (_previousTrackBankEnd == TRACK_BANK_RIGHT) {
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_BANK_LEFT);
	}
	if (_currentTrackBankEnd != _previousTrackBankEnd) {
		disabledWidgets |=
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE) |
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_LEFT_CURVE_LARGE);
	}
	if (_currentTrackSlopeEnd != TRACK_SLOPE_NONE) {
		if (is_track_enabled(TRACK_SLOPE_ROLL_BANKING)) {
			if (_previousTrackSlopeEnd == TRACK_SLOPE_NONE) {
				if (_currentTrackSlopeEnd != TRACK_SLOPE_UP_25 && _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_25) {
					disabledWidgets |=
						(1ULL << WIDX_BANK_LEFT) |
						(1ULL << WIDX_BANK_RIGHT);
				}
			} else {
				if (_currentTrackSlopeEnd != _previousTrackSlopeEnd) {
					disabledWidgets |=
						(1ULL << WIDX_BANK_LEFT) |
						(1ULL << WIDX_BANK_RIGHT);
				} else {
					if (_currentTrackSlopeEnd != TRACK_SLOPE_UP_25 && _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_25) {
						disabledWidgets |=
							(1ULL << WIDX_BANK_LEFT) |
							(1ULL << WIDX_BANK_RIGHT);
					}
				}
			}
		} else {
			disabledWidgets |=
				(1ULL << WIDX_BANK_LEFT) |
				(1ULL << WIDX_BANK_RIGHT);
		}
	}
	if (_currentTrackBankEnd != TRACK_BANK_NONE || _previousTrackBankEnd != TRACK_BANK_NONE) {
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_SLOPE_UP_STEEP) |
			(1ULL << WIDX_CHAIN_LIFT);
	}
	if (_currentTrackCurve != TRACK_CURVE_NONE) {
		if (!is_track_enabled(TRACK_LIFT_HILL_CURVE)) {
			disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
		}
		if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE) {
			disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
		}
		if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_60) {
			disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
		}
		if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_60) {
			disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
		}
	}
	if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_90 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90) {
		disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
	}
	if (_previousTrackBankEnd == TRACK_BANK_UPSIDE_DOWN) {
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_STRAIGHT) |
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE);
	}
	if (_currentTrackCurve != TRACK_CURVE_NONE) {
		if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE) {
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_DOWN) |
				(1ULL << WIDX_SLOPE_UP);
		}
		if (_currentTrackSlopeEnd == _previousTrackSlopeEnd) {
			if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_25) {
				disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
				if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL || _rideConstructionState != RIDE_CONSTRUCTION_STATE_BACK || !is_track_enabled(TRACK_SLOPE_CURVE_BANKED)) {
					disabledWidgets |= (1ULL << WIDX_LEVEL);
				}
			}
			if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_25) {
				disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP);
				if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL || _rideConstructionState != RIDE_CONSTRUCTION_STATE_FRONT || !is_track_enabled(TRACK_SLOPE_CURVE_BANKED)) {
					disabledWidgets |= (1ULL << WIDX_LEVEL);
				}
			}
		} else if (is_track_enabled(TRACK_SLOPE_CURVE_BANKED)) {
			disabledWidgets |=
				(1ULL << WIDX_SLOPE_DOWN_STEEP) |
				(1ULL << WIDX_SLOPE_UP_STEEP);
			if (_currentTrackBankEnd == TRACK_BANK_LEFT) {
				disabledWidgets |=
					(1ULL << WIDX_BANK_STRAIGHT) |
					(1ULL << WIDX_BANK_RIGHT);
				disabledWidgets &= ~(1ULL << WIDX_BANK_LEFT);
			}
			if (_currentTrackBankEnd == TRACK_BANK_RIGHT) {
				disabledWidgets |=
					(1ULL << WIDX_BANK_LEFT) |
					(1ULL << WIDX_BANK_STRAIGHT);
				disabledWidgets &= ~(1ULL << WIDX_BANK_RIGHT);
			}
			if (_currentTrackBankEnd == TRACK_BANK_NONE) {
				disabledWidgets |=
					(1ULL << WIDX_BANK_LEFT) |
					(1ULL << WIDX_BANK_RIGHT);
				disabledWidgets &= ~(1ULL << WIDX_BANK_STRAIGHT);
			}
			if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE) {
				disabledWidgets |=
					(1ULL << WIDX_SLOPE_DOWN) |
					(1ULL << WIDX_SLOPE_UP);
				disabledWidgets &= ~(1ULL << WIDX_LEVEL);
			}
			if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_25) {
				disabledWidgets |=
					(1ULL << WIDX_SLOPE_DOWN) |
					(1ULL << WIDX_LEVEL);
				disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP);
			}
			if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_25) {
				disabledWidgets |=
					(1ULL << WIDX_LEVEL) |
					(1ULL << WIDX_SLOPE_UP);
				disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN);
			}
			if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL) {
				disabledWidgets &= ~(1ULL << WIDX_LEFT_CURVE_SMALL);
			}
			if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL) {
				disabledWidgets &= ~(1ULL << WIDX_RIGHT_CURVE_SMALL);
			}
		}
	}
	if (_currentTrackCurve != TRACK_CURVE_NONE && _currentTrackSlopeEnd == TRACK_SLOPE_UP_60) {
		disabledWidgets |= (1ULL << WIDX_SLOPE_UP);
	}
	if (_currentTrackCurve != TRACK_CURVE_NONE && _currentTrackSlopeEnd == TRACK_SLOPE_DOWN_60) {
		disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN);
	}
	if (_currentTrackLiftHill & 1) {
		if (_currentTrackSlopeEnd != TRACK_SLOPE_NONE && !is_track_enabled(TRACK_LIFT_HILL_CURVE)) {
			disabledWidgets |=
				(1ULL << WIDX_LEFT_CURVE_SMALL) |
				(1ULL << WIDX_LEFT_CURVE) |
				(1ULL << WIDX_LEFT_CURVE_LARGE) |
				(1ULL << WIDX_RIGHT_CURVE_SMALL) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_LARGE);
		}
		if (!is_track_enabled(TRACK_LIFT_HILL_STEEP)) {
			if (w->widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP) {
				disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
			}
		}
	}
	if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 && _currentTrackCurve != TRACK_CURVE_NONE) {
		disabledWidgets |=
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_LEVEL);
	}
	if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 && _currentTrackCurve != TRACK_CURVE_NONE) {
		disabledWidgets |=
			(1ULL << WIDX_LEVEL) |
			(1ULL << WIDX_SLOPE_UP_STEEP);
	}
	if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_90 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90) {
		if (_currentTrackCurve != TRACK_CURVE_NONE) {
			disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
		}
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE);
		if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
			disabledWidgets |=
				(1ULL << WIDX_STRAIGHT) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_SMALL) |
				(1ULL << WIDX_LEFT_CURVE_SMALL) |
				(1ULL << WIDX_LEFT_CURVE);
		}
	} else if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_90 || _previousTrackSlopeEnd == TRACK_SLOPE_DOWN_90) {
		if (_currentTrackCurve != TRACK_CURVE_NONE) {
			disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP);
		}
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE);
		if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
			disabledWidgets |=
				(1ULL << WIDX_STRAIGHT) |
				(1ULL << WIDX_RIGHT_CURVE) |
				(1ULL << WIDX_RIGHT_CURVE_SMALL) |
				(1ULL << WIDX_LEFT_CURVE_SMALL) |
				(1ULL << WIDX_LEFT_CURVE);
		}
	}
	if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED)) {
		if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_SLOPE_NONE) {
			if (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT) {
				if (_currentTrackSlopeEnd == _previousTrackSlopeEnd) {
					disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
					disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
				}
			}
		}
	} else if (
		(
			is_track_enabled(TRACK_HELIX_SMALL) ||
			(
				is_track_enabled(TRACK_HELIX_LARGE) &&
				_currentTrackCurve != TRACK_CURVE_LEFT_SMALL &&
				_currentTrackCurve != TRACK_CURVE_RIGHT_SMALL
			)
		) &&
		(
			_currentTrackCurve == TRACK_CURVE_LEFT ||
			_currentTrackCurve == TRACK_CURVE_RIGHT ||
			_currentTrackCurve == TRACK_CURVE_LEFT_SMALL ||
			_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL
		) &&
		(_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd != TRACK_BANK_NONE)
	) {
		if (_previousTrackSlopeEnd == _currentTrackSlopeEnd) {
			// Enable helix
			disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
			disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
		}
	}
	if (is_track_enabled(TRACK_SLOPE_CURVE_BANKED)) {
		if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
			if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL) {
				if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _previousTrackBankEnd != TRACK_BANK_NONE) {
					disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP);
				}
			}
		} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
			if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL) {
				if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _previousTrackBankEnd != TRACK_BANK_NONE) {
					disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN);
				}
			}
		}
	}
	if (_currentTrackPieceDirection >= 4) {
		disabledWidgets |=
			(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
	}
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
		disabledWidgets |= (1ULL << WIDX_NEXT_SECTION);
		if (sub_6CA2DF(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
			disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
		}
	} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
		disabledWidgets |= (1ULL << WIDX_PREVIOUS_SECTION);
		if (sub_6CA2DF(NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
			disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
		}
	}
	if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES)) {
		disabledWidgets &= ~(1ULL << WIDX_BANKING_GROUPBOX);
	}
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT || _rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
		disabledWidgets |=
			(1ULL << WIDX_DIRECTION_GROUPBOX) |
			(1ULL << WIDX_SLOPE_GROUPBOX) |
			(1ULL << WIDX_BANKING_GROUPBOX) |
			(1ULL << WIDX_LEFT_CURVE_VERY_SMALL) |
			(1ULL << WIDX_LEFT_CURVE_SMALL) |
			(1ULL << WIDX_LEFT_CURVE) |
			(1ULL << WIDX_STRAIGHT) |
			(1ULL << WIDX_RIGHT_CURVE) |
			(1ULL << WIDX_RIGHT_CURVE_SMALL) |
			(1ULL << WIDX_RIGHT_CURVE_VERY_SMALL) |
			(1ULL << WIDX_SPECIAL_TRACK_DROPDOWN) |
			(1ULL << WIDX_SLOPE_DOWN_STEEP) |
			(1ULL << WIDX_SLOPE_DOWN) |
			(1ULL << WIDX_LEVEL) |
			(1ULL << WIDX_SLOPE_UP) |
			(1ULL << WIDX_SLOPE_UP_STEEP) |
			(1ULL << WIDX_CHAIN_LIFT) |
			(1ULL << WIDX_BANK_LEFT) |
			(1ULL << WIDX_BANK_STRAIGHT) |
			(1ULL << WIDX_BANK_RIGHT) |
			(1ULL << WIDX_LEFT_CURVE_LARGE) |
			(1ULL << WIDX_RIGHT_CURVE_LARGE);
	}
	if (RCT2_GLOBAL(0x00F440D3, uint8) != 0) {
		disabledWidgets &= ~(1ULL << WIDX_BANKING_GROUPBOX);
		disabledWidgets &= ~(1ULL << WIDX_BANK_LEFT);
		disabledWidgets &= ~(1ULL << WIDX_BANK_STRAIGHT);
		disabledWidgets &= ~(1ULL << WIDX_BANK_RIGHT);
	}

	// Set and invalidate the changed widgets
	uint64 currentDisabledWidgets = w->disabled_widgets;
	if (currentDisabledWidgets == disabledWidgets)
		return;

	for (int i = 0; i < 64; i++) {
		if ((disabledWidgets & (1ULL << i)) != (currentDisabledWidgets & (1ULL << i))) {
			widget_invalidate(w, i);
		}
	}
	w->disabled_widgets = disabledWidgets;
}

/**
 *
 * rct2: 0x006C6E6A
 */
static void window_ride_construction_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride = GET_RIDE(_currentRideIndex);
	int rideType;

	window_ride_construction_update_enabled_track_pieces();
	switch (widgetIndex) {
	case WIDX_LEFT_CURVE:
		sub_6C9627();
		_currentTrackCurve = 1;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_RIGHT_CURVE:
		sub_6C9627();
		_currentTrackCurve = 2;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_LEFT_CURVE_SMALL:
		sub_6C9627();
		_currentTrackCurve = 3;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_RIGHT_CURVE_SMALL:
		sub_6C9627();
		_currentTrackCurve = 4;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_LEFT_CURVE_VERY_SMALL:
		sub_6C9627();
		_currentTrackCurve = 5;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_RIGHT_CURVE_VERY_SMALL:
		sub_6C9627();
		_currentTrackCurve = 6;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_LEFT_CURVE_LARGE:
		sub_6C9627();
		_currentTrackCurve = 7;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_RIGHT_CURVE_LARGE:
		sub_6C9627();
		_currentTrackCurve = 8;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_STRAIGHT:
		sub_6C9627();
		if (_currentTrackCurve != 0)
			_currentTrackBankEnd = TRACK_BANK_NONE;
		_currentTrackCurve = 0;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_SLOPE_DOWN_STEEP:
		sub_6C9627();
		rideType = _currentTrackCovered & 2 ? RCT2_ADDRESS(0x0097D4F5, uint8)[ride->type * 8] : ride->type;
		if (is_track_enabled(TRACK_HELIX_SMALL)) {
			if (_currentTrackCurve == 1 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 349;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 2 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 350;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 3 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 345;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 4 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 346;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			}
		}
		if (is_track_enabled(TRACK_HELIX_LARGE)) {
			if (_currentTrackCurve == 1 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 360;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 2 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 361;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			}
		}
		if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED)) {
			if (_currentTrackBankEnd == TRACK_BANK_NONE) {
				if (_currentTrackCurve == 1) {
					_currentTrackCurve = 364;
					_currentTrackPrice = MONEY32_UNDEFINED;
					sub_6C84CE();
					break;
				} else if (_currentTrackCurve == 2) {
					_currentTrackCurve = 365;
					_currentTrackPrice = MONEY32_UNDEFINED;
					sub_6C84CE();
					break;
				}
			}
		}
		if (w->widgets[WIDX_SLOPE_DOWN_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP) {
			loc_6C7502(8);
		} else {
			loc_6C7502(10);
		}
		break;
	case WIDX_SLOPE_DOWN:
		sub_6C9627();
		if (_rideConstructionState == 2 && _currentTrackBankEnd != TRACK_BANK_NONE) {
			_currentTrackBankEnd = TRACK_BANK_NONE;
		}
		loc_6C7502(6);
		break;
	case WIDX_LEVEL:
		sub_6C9627();
		if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT && _previousTrackSlopeEnd == 6) {
			if (_currentTrackCurve == 3) {
				_currentTrackBankEnd = TRACK_BANK_LEFT;
			} else if (_currentTrackCurve == 4) {
				_currentTrackBankEnd = TRACK_BANK_RIGHT;
			}
		} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK && _previousTrackSlopeEnd == 2) {
			if (_currentTrackCurve == 3) {
				_currentTrackBankEnd = TRACK_BANK_LEFT;
			} else if (_currentTrackCurve == 4) {
				_currentTrackBankEnd = TRACK_BANK_RIGHT;
			}
		}
		loc_6C7502(0);
		break;
	case WIDX_SLOPE_UP:
		sub_6C9627();
		if (_rideConstructionState == 1 && _currentTrackBankEnd != TRACK_BANK_NONE) {
			_currentTrackBankEnd = TRACK_BANK_NONE;
		}
		if (ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER || ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
			if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT && _currentTrackCurve == 0) {
				_currentTrackCurve = 124 | 0x100;
				sub_6C84CE();
			}
		} else {
			loc_6C7502(2);
		}
		break;
	case WIDX_SLOPE_UP_STEEP:
		sub_6C9627();
		rideType = _currentTrackCovered & 2 ? RCT2_ADDRESS(0x0097D4F5, uint8)[ride->type * 8] : ride->type;
		if (is_track_enabled(TRACK_HELIX_SMALL)) {
			if (_currentTrackCurve == 1 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 347;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 2 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 348;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 3 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 343;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 4 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 344;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			}
		}
		if (is_track_enabled(TRACK_HELIX_LARGE)) {
			if (_currentTrackCurve == 1 && _currentTrackBankEnd == TRACK_BANK_LEFT) {
				_currentTrackCurve = 358;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			} else if (_currentTrackCurve == 2 && _currentTrackBankEnd == TRACK_BANK_RIGHT) {
				_currentTrackCurve = 359;
				_currentTrackPrice = MONEY32_UNDEFINED;
				sub_6C84CE();
				break;
			}
		}
		if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED)) {
			if (_currentTrackBankEnd == TRACK_BANK_NONE) {
				if (_currentTrackCurve == 1) {
					_currentTrackCurve = 362;
					_currentTrackPrice = MONEY32_UNDEFINED;
					sub_6C84CE();
					break;
				} else if (_currentTrackCurve == 2) {
					_currentTrackCurve = 363;
					_currentTrackPrice = MONEY32_UNDEFINED;
					sub_6C84CE();
					break;
				}
			}
		}
		if (w->widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP) {
			loc_6C7502(4);
		} else {
			loc_6C7502(18);
		}
		break;
	case WIDX_CHAIN_LIFT:
		sub_6C9627();
		_currentTrackLiftHill ^= 1;
		if (_currentTrackLiftHill & 1) {
			_currentTrackCovered &= ~1;
		}
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_BANK_LEFT:
		sub_6C9627();
		if (RCT2_GLOBAL(0x00F440D3, uint8) == 0) {
			_currentTrackBankEnd = TRACK_BANK_LEFT;
			_currentTrackPrice = MONEY32_UNDEFINED;
			sub_6C84CE();
		}
		break;
	case WIDX_BANK_STRAIGHT:
		sub_6C9627();
		if (RCT2_GLOBAL(0x00F440D3, uint8) == 0) {
			_currentTrackBankEnd = TRACK_BANK_NONE;
			_currentTrackPrice = MONEY32_UNDEFINED;
			sub_6C84CE();
		} else {
			uint8 *brakesSpeedPtr = (uint8*)0x00F440CD;
			uint8 maxBrakesSpeed = 30;
			if (RCT2_GLOBAL(0x00F440D3, uint8) != 1) {
				brakesSpeedPtr = (uint8*)0x00F440CE;
				maxBrakesSpeed = RCT2_GLOBAL(0x0097CF40 + 6 + (ride->type * 8), uint8);
			}
			uint8 brakesSpeed = *brakesSpeedPtr + 2;
			if (brakesSpeed <= maxBrakesSpeed) {
				if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
					ride_construction_set_brakes_speed(brakesSpeed);
				} else {
					*brakesSpeedPtr = brakesSpeed;
					sub_6C84CE();
				}
			}
		}
		break;
	case WIDX_BANK_RIGHT:
		sub_6C9627();
		if (RCT2_GLOBAL(0x00F440D3, uint8) == 0) {
			_currentTrackBankEnd = TRACK_BANK_RIGHT;
			_currentTrackPrice = MONEY32_UNDEFINED;
			sub_6C84CE();
		} else {
			uint8 *brakesSpeedPtr = (uint8*)0x00F440CD;
			if (RCT2_GLOBAL(0x00F440D3, uint8) != 1) {
				brakesSpeedPtr = (uint8*)0x00F440CE;
			}
			uint8 brakesSpeed = *brakesSpeedPtr - 2;
			if (brakesSpeed >= 2) {
				if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
					ride_construction_set_brakes_speed(brakesSpeed);
				} else {
					*brakesSpeedPtr = brakesSpeed;
					sub_6C84CE();
				}
			}
		}
		break;
	case WIDX_SPECIAL_TRACK_DROPDOWN:
		window_ride_construction_show_special_track_dropdown(w, widget);
		break;
	case WIDX_U_TRACK:
		sub_6C9627();
		_currentTrackCovered &= ~1;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_O_TRACK:
		sub_6C9627();
		_currentTrackCovered |= 1;
		_currentTrackLiftHill &= ~1;
		_currentTrackPrice = MONEY32_UNDEFINED;
		sub_6C84CE();
		break;
	case WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP:
		if (_currentSeatRotationAngle < 15) {
			if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
				ride_selected_track_set_seat_rotation(_currentSeatRotationAngle + 1);
			} else {
				_currentSeatRotationAngle++;
				sub_6C84CE();
			}
		}
		break;
	case WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN:
		if (_currentSeatRotationAngle > 0) {
			if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
				ride_selected_track_set_seat_rotation(_currentSeatRotationAngle - 1);
			} else {
				_currentSeatRotationAngle--;
				sub_6C84CE();
			}
		}
		break;
	}
}

/**
 *
 * rct2: 0x006C78CD
 */
static void window_ride_construction_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (widgetIndex != WIDX_SPECIAL_TRACK_DROPDOWN)
		return;
	if (dropdownIndex == -1)
		return;

	sub_6C9627();
	_currentTrackPrice = MONEY32_UNDEFINED;
	int trackPiece = _currentPossibleRideConfigurations[dropdownIndex];
	switch (trackPiece) {
	case TRACK_ELEM_END_STATION:
	case TRACK_ELEM_S_BEND_LEFT:
	case TRACK_ELEM_S_BEND_RIGHT:
		_currentTrackSlopeEnd = 0;
		break;
	case TRACK_ELEM_LEFT_VERTICAL_LOOP:
	case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
		_currentTrackBankEnd = TRACK_BANK_NONE;
		_currentTrackLiftHill &= ~1;
		break;
	}
	_currentTrackCurve = trackPiece | 0x100;
	sub_6C84CE();
}

/**
 *
 * rct2: 0x006C9F72
 */
static void window_ride_construction_construct(rct_window *w)
{
	int trackType, trackDirection, rideIndex, edxRS16, x, y, z, properties;
	track_begin_end trackBeginEnd;

	_currentTrackPrice = MONEY32_UNDEFINED;
	RCT2_GLOBAL(0x00F44074, money32) = MONEY32_UNDEFINED;
	sub_6C9627();
	if (sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, &x, &y, &z, &properties)) {
		sub_6C84CE();
		return;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = STR_RIDE_CONSTRUCTION_CANT_CONSTRUCT_THIS_HERE;
	RCT2_GLOBAL(0x00F44074, money32) = game_do_command(
		x,
		(GAME_COMMAND_FLAG_APPLY) | (trackDirection << 8),
		y,
		rideIndex | (trackType << 8) | (edxRS16 << 16),
		GAME_COMMAND_PLACE_TRACK,
		z | (properties << 16),
		0
	);
	if (RCT2_GLOBAL(0x00F44074, money32) == MONEY32_UNDEFINED) {
		sub_6C84CE();
		return;
	}

	sound_play_panned(SOUND_PLACE_ITEM, 0x8001, x, y, z);

	if (RCT2_GLOBAL(0x00F441D4, uint8) & 2) {
		viewport_set_visibility(1);
	}

	if (
		(_currentTrackCurve >= 343 && _currentTrackCurve <= 350) ||
		(_currentTrackCurve >= 358 && _currentTrackCurve <= 365) ||
		(_currentTrackSlopeEnd != TRACK_SLOPE_NONE)
	) {
		viewport_set_visibility(2);
	}

	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
		RCT2_GLOBAL(0x00F441D2, uint8) = _currentRideIndex;
		trackDirection = _currentTrackPieceDirection ^ 2;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		if (!(trackDirection & 4)) {
			x += TileDirectionDelta[trackDirection].x;
			y += TileDirectionDelta[trackDirection].y;
		}

		if (track_block_get_previous_from_zero(x, y, z, _currentRideIndex, trackDirection, &trackBeginEnd)) {
			_currentTrackBeginX = trackBeginEnd.begin_x;
			_currentTrackBeginY = trackBeginEnd.begin_y;
			_currentTrackBeginZ = trackBeginEnd.begin_z;
			_currentTrackPieceDirection = trackBeginEnd.begin_direction;
			_currentTrackPieceType = trackBeginEnd.begin_element->properties.track.type;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
			ride_select_previous_section();
		} else {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
		}
	} else {
		RCT2_GLOBAL(0x00F441D2, uint8) = _currentRideIndex;
		trackDirection = _currentTrackPieceDirection;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		if (!(trackDirection & 4)) {
			x -= TileDirectionDelta[trackDirection].x;
			y -= TileDirectionDelta[trackDirection].y;
		}

		rct_xy_element next_track;
		if (track_block_get_next_from_zero(x, y, z, _currentRideIndex, trackDirection, &next_track, &z, &trackDirection)) {
			_currentTrackBeginX = next_track.x;
			_currentTrackBeginY = next_track.y;
			_currentTrackBeginZ = z;
			_currentTrackPieceDirection = next_track.element->type & MAP_ELEMENT_DIRECTION_MASK;
			_currentTrackPieceType = next_track.element->properties.track.type;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
			ride_select_next_section();
		} else {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
		}
	}

	sub_6C84CE();
}

/**
 *
 * rct2: 0x006C9BA5
 */
static void window_ride_construction_mouseup_demolish(rct_window* w)
{
	int x, y, z, direction, type, slope, slopeEnd, b2, bankEnd, bankStart, b5, b4;
	rct_map_element *mapElement;
	rct_xy_element inputElement, outputElement;
	track_begin_end trackBeginEnd;
	bool gotoStartPlacementMode;

	_currentTrackPrice = MONEY32_UNDEFINED;
	sub_6C9627();

	// Select the track element that is to be deleted
	RCT2_GLOBAL(0x00F440B8, uint8) = 3;
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
		if (!ride_select_backwards_from_front()) {
			sub_6C84CE();
			return;
		}
		RCT2_GLOBAL(0x00F440B8, uint8) = 1;
	} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
		if (!ride_select_forwards_from_back()) {
			sub_6C84CE();
			return;
		}
		RCT2_GLOBAL(0x00F440B8, uint8) = 2;
	}

	// Invalidate the selected track element or make sure its at origin???
	x = _currentTrackBeginX;
	y = _currentTrackBeginY;
	z = _currentTrackBeginZ;
	direction = _currentTrackPieceDirection;
	type = _currentTrackPieceType;
	if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &mapElement, 0)) {
		sub_6C84CE();
		return;
	}

	// Get the previous track element to go to after the selected track element is deleted
	inputElement.x = x;
	inputElement.y = y;
	inputElement.element = mapElement;
	if (track_block_get_previous(x, y, mapElement, &trackBeginEnd)) {
		x = trackBeginEnd.begin_x;
		y = trackBeginEnd.begin_y;
		z = trackBeginEnd.begin_z;
		direction = trackBeginEnd.begin_direction;
		type = trackBeginEnd.begin_element->properties.track.type;
		gotoStartPlacementMode = false;
	}
	else if (track_block_get_next(&inputElement, &outputElement, &z, &direction)) {
		x = outputElement.x;
		y = outputElement.y;
		direction = outputElement.element->type & MAP_ELEMENT_DIRECTION_MASK;
		type = outputElement.element->properties.track.type;
		gotoStartPlacementMode = false;
	} else {
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection;
		type = _currentTrackPieceType;

		if (sub_6C683D(&x, &y, &z, direction, type, 0, &mapElement, 0)) {
			sub_6C84CE();
			return;
		}

		const rct_preview_track *trackBlock = get_track_def_from_ride_index(_currentRideIndex, mapElement->properties.track.type);
		z = (mapElement->base_height * 8) - trackBlock->z;
		gotoStartPlacementMode = true;
	}

	money32 cost = ride_remove_track_piece(
		_currentTrackBeginX,
		_currentTrackBeginY,
		_currentTrackBeginZ,
		_currentTrackPieceDirection,
		_currentTrackPieceType
	);
	if (cost == MONEY32_UNDEFINED) {
		sub_6C84CE();
		return;
	}
	
	if (gotoStartPlacementMode) {
		z &= 0xFFF0;
		_currentTrackBeginZ = z;
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		direction = _currentTrackPieceDirection;
		slope = _currentTrackCurve;
		slopeEnd = _previousTrackSlopeEnd;
		b2 = _currentTrackSlopeEnd;
		bankEnd = _previousTrackBankEnd;
		bankStart = _currentTrackBankEnd;
		b5 = _currentTrackCovered;
		b4 = _currentTrackLiftHill;
		ride_construction_set_default_next_piece();
		sub_6C84CE();
		if (!sub_6CAF80(_currentRideIndex, &outputElement)) {
			sub_6CC3FB(_currentRideIndex);
			_currentTrackPieceDirection = _currentTrackPieceDirection;
			if (!(slope & 0x100)) {
				_currentTrackCurve = slope;
				_previousTrackSlopeEnd = slopeEnd;
				_currentTrackSlopeEnd = b2;
				_previousTrackBankEnd = bankEnd;
				_currentTrackBankEnd = bankStart;
				_currentTrackCovered = b5;
				_currentTrackLiftHill = b4;
				sub_6C84CE();
			}
		}
	} else {
		if (RCT2_GLOBAL(0x00F440B8, uint8) == 3 || RCT2_GLOBAL(0x00F440B8, uint8) == 1) {
			if (type == TRACK_ELEM_MIDDLE_STATION || type == TRACK_ELEM_BEGIN_STATION) {
				type = TRACK_ELEM_END_STATION;
			}
		}
		if (RCT2_GLOBAL(0x00F440B8, uint8) == 2) {
			if (type == TRACK_ELEM_MIDDLE_STATION) {
				type = TRACK_ELEM_BEGIN_STATION;
			}
		}
		_currentTrackBeginX = x;
		_currentTrackBeginY = y;
		_currentTrackBeginZ = z;
		_currentTrackPieceDirection = direction;
		_currentTrackPieceType = type;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		if (RCT2_GLOBAL(0x00F440B8, uint8) == 1) {
			ride_select_next_section();
		} else if (RCT2_GLOBAL(0x00F440B8, uint8) == 2) {
			ride_select_previous_section();
		}
		sub_6C84CE();
	}
}

/**
 *
 * rct2: 0x006C78AA
 */
static void window_ride_construction_rotate(rct_window *w)
{
	_currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
	sub_6C9627();
	_currentTrackPrice = MONEY32_UNDEFINED;
	sub_6C84CE();
}

/**
 *
 * rct2: 0x006C7802
 */
static void window_ride_construction_entrance_click(rct_window *w)
{
	if (tool_set(w, WIDX_ENTRANCE, 12)) {
		if (!sub_6CAF80(_currentRideIndex, NULL)) {
			sub_6CC3FB(_currentRideIndex);
		}
	} else {
		RCT2_GLOBAL(0x00F44191, uint8) = 0;
		RCT2_GLOBAL(0x00F44192, uint8) = w->number & 0xFF;
		RCT2_GLOBAL(0x00F44193, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) |= INPUT_FLAG_6;
		sub_6C9627();
		if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT) {
			RCT2_GLOBAL(0x00F440CC, uint8) = _rideConstructionState;
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
		}
		sub_6C84CE();
	}
}

/**
 *
 * rct2: 0x006C7866
 */
static void window_ride_construction_exit_click(rct_window *w)
{
	if (tool_set(w, WIDX_EXIT, 12)) {
		if (!sub_6CAF80(_currentRideIndex, NULL)) {
			sub_6CC3FB(_currentRideIndex);
		}
	} else {
		RCT2_GLOBAL(0x00F44191, uint8) = 1;
		RCT2_GLOBAL(0x00F44192, uint8) = w->number & 0xFF;
		RCT2_GLOBAL(0x00F44193, uint8) = 0;
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) |= INPUT_FLAG_6;
		sub_6C9627();
		if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT) {
			RCT2_GLOBAL(0x00F440CC, uint8) = _rideConstructionState;
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
		}
		sub_6C84CE();
	}
}

/**
 *
 * rct2: 0x006C8374
 */
static void window_ride_construction_update(rct_window *w)
{
	switch (_currentTrackCurve) {
	case 429:
	case 376:
	case 369:
	case 368:
		widget_invalidate(w, WIDX_CONSTRUCT);
		break;
	}

	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE) {
		if (!widget_is_active_tool(w, WIDX_CONSTRUCT)) {
			window_close(w);
			return;
		}
	}

	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT) {
		if (!widget_is_active_tool(w, WIDX_ENTRANCE) && !widget_is_active_tool(w, WIDX_EXIT)) {
			_rideConstructionState = RCT2_GLOBAL(0x00F440CC, uint8);
			sub_6C84CE();
		}
	}

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_FRONT:
	case RIDE_CONSTRUCTION_STATE_BACK:
	case RIDE_CONSTRUCTION_STATE_SELECTED:
		if (
			(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) == WC_RIDE_CONSTRUCTION
		) {
			tool_cancel();
		}
		break;
	}

	sub_6C94D8();
}

/**
 *
 * rct2: 0x006CC538
 */
static bool ride_get_place_position_from_screen_position(int screenX, int screenY, int *outX, int *outY)
{
	short mapX, mapY, mapZ;
	int interactionType, direction;
	rct_map_element *mapElement;
	rct_viewport *viewport;

	if (!_trackPlaceCtrlState) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 2) {
			get_map_coordinates_from_pos(screenX, screenY, 0xFCCA, &mapX, &mapY, &interactionType, &mapElement, &viewport);
			if (interactionType != 0) {
				_trackPlaceCtrlZ = mapElement->base_height * 8;
				_trackPlaceCtrlState = true;
			}
		}
	} else {
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 2)) {
			_trackPlaceCtrlState = false;
		}
	}

	if (!_trackPlaceShiftState) {
		if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 1) {
			_trackPlaceShiftState = true;
			_trackPlaceShiftStartScreenX = screenX;
			_trackPlaceShiftStartScreenY = screenY;
			_trackPlaceShiftZ = 0;
		}
	} else {
		if (RCT2_GLOBAL(RCT2_ADDRESS_PLACE_OBJECT_MODIFIER, uint8) & 1) {
			_trackPlaceShiftZ = floor2(_trackPlaceShiftStartScreenY - screenY + 4, 8);
			screenX = _trackPlaceShiftStartScreenX;
			screenY = _trackPlaceShiftStartScreenY;
		} else {
			_trackPlaceShiftState = false;
		}
	}

	if (!_trackPlaceCtrlState) {
		sub_68A15E(screenX, screenY, &mapX, &mapY, &direction, &mapElement);
		if (mapX == (short)0x8000)
			return false;

		_trackPlaceZ = 0;
		if (_trackPlaceShiftState) {
			mapElement = map_get_surface_element_at(mapX >> 5, mapY >> 5);
			mapZ = floor2(mapElement->base_height * 8, 16);
			mapZ += _trackPlaceShiftZ;
			mapZ = max(mapZ, 16);
			_trackPlaceZ = mapZ;
		}
	} else {
		mapZ = _trackPlaceCtrlZ;
		screen_get_map_xy_with_z(screenX, screenY, mapZ, &mapX, &mapY);
		if (_trackPlaceShiftState != 0) {
			mapZ += _trackPlaceShiftZ;
		}
		_trackPlaceZ = max(mapZ, 16);
	}

	if (mapX == (short)0x8000)
		return false;

	*outX = floor2(mapX, 32);
	*outY = floor2(mapY, 32);
	return true;
}

/**
 *
 * rct2: 0x006C8229
 */
static void window_ride_construction_toolupdate(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_CONSTRUCT:
		ride_construction_toolupdate_construct(x, y);
		break;
	case WIDX_ENTRANCE:
	case WIDX_EXIT:
		ride_construction_toolupdate_entrance_exit(x, y);
		break;
	}
}

/**
 * 
 *  rct2: 0x006C8248
 */
static void window_ride_construction_tooldown(rct_window* w, int widgetIndex, int x, int y)
{
	switch (widgetIndex) {
	case WIDX_CONSTRUCT:
		ride_construction_tooldown_construct(x, y);
		break;
	case WIDX_ENTRANCE:
	case WIDX_EXIT:
		ride_construction_tooldown_entrance_exit(x, y);
		break;
	}
}

/**
 *
 * rct2: 0x006C6AD5
 */
static void window_ride_construction_invalidate(rct_window *w)
{
	rct_ride *ride;
	rct_string_id stringId;

	ride = GET_RIDE(_currentRideIndex);

	stringId = STR_RIDE_CONSTRUCTION_SPECIAL;
	if (_currentTrackCurve >= 256) {
		stringId = RCT2_ADDRESS(0x00999492, rct_string_id)[_currentTrackCurve - 256];
		if (stringId == STR_RAPIDS && ride->type == RIDE_TYPE_CAR_RIDE)
			stringId = STR_LOG_BUMPS;
	}
	RCT2_GLOBAL(0x013CE952, uint16) = stringId;

	if (RCT2_GLOBAL(0x00F440D3, uint8) == 1)
		RCT2_GLOBAL(0x013CE952 + 2, uint16) = ((RCT2_GLOBAL(0x00F440CD, uint8) * 9) >> 2) & 0xFFFF;

	window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].image =
		STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_180 + RCT2_GLOBAL(0x00F440CF, uint8);

	if (RCT2_GLOBAL(0x00F440D3, uint8) == 2)
		RCT2_GLOBAL(0x013CE952 + 2, uint16) = ((RCT2_GLOBAL(0x00F440CE, uint8) * 9) >> 2) & 0xFFFF;

	// Set window title arguments
	RCT2_GLOBAL(0x013CE952 + 4, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 6, uint32) = ride->name_arguments;
}

/**
 *
 * rct2: 0x006C6B86
 */
static void window_ride_construction_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_drawpixelinfo *clipdpi;
	rct_widget *widget;
	int x, y, width, height;

	window_draw_widgets(w, dpi);
	
	widget = &window_ride_construction_widgets[WIDX_CONSTRUCT];
	if (widget->type == WWT_EMPTY)
		return;

	int trackType, trackDirection, rideIndex, edxRS16;
	if (sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, NULL, NULL, NULL, NULL))
		return;

	// Draw track piece
	x = w->x + widget->left + 1;
	y = w->y + widget->top + 1;
	width = widget->right - widget->left - 1;
	height = widget->bottom - widget->top - 1;
	clipdpi = clip_drawpixelinfo(dpi, x, width, y, height);
	if (clipdpi != NULL) {
		window_ride_construction_draw_track_piece(w, clipdpi, rideIndex, trackType, trackDirection, edxRS16, width, height);
		rct2_free(clipdpi);
	}

	// Draw cost
	x = w->x + (widget->left + widget->right) / 2;
	y = w->y + widget->bottom - 23;
	if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_PLACE)
		gfx_draw_string_centred(dpi, 1407, x, y, 0, w);

	y += 11;
	if (
		_currentTrackPrice != MONEY32_UNDEFINED &&
		!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY)
	) {
		gfx_draw_string_centred(dpi, 1408, x, y, 0, (void*)&_currentTrackPrice);
	}
}

static void window_ride_construction_draw_track_piece(
	rct_window *w, rct_drawpixelinfo *dpi,
	int rideIndex, int trackType, int trackDirection, int unknown,
	int width, int height
) {
	const rct_preview_track *trackBlock;
	rct_ride *ride;

	ride = GET_RIDE(rideIndex);

	trackBlock = get_track_def_from_ride(ride, trackType);
	while ((trackBlock + 1)->index != 0xFF)
		trackBlock++;

	short x = trackBlock->x;
	short z = trackBlock->z;
	short y = trackBlock->y;
	if (trackBlock->var_09 & 2) {
		x = 0;
		y = 0;
	}

	short tmp;
	switch (trackDirection & 3) {
	case 1:
		tmp = x;
		x = y;
		y = -tmp;
		break;
	case 2:
		x = -x;
		y = -y;
		break;
	case 3:
		tmp = x;
		x = -y;
		y = tmp;
		break;
	case 0:
		break;
	}
	//this is actually case 0, but the other cases all jump to it
	x = 4112 + (x / 2);
	y = 4112 + (y / 2);
	z = 1024 + z;

	short bx = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
		RCT2_GLOBAL(0x009984A2 + trackType * 8, sint8) :
		RCT2_GLOBAL(0x00997CA2 + trackType * 8, sint8);

	z -= bx;
	int start_x = x;
	switch (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint32_t)) {
	case 0:
		x = y - x;
		y = (y + start_x) / 2 - z;
		break;
	case 1:
		x = -x - y;
		y = (y - start_x) / 2 - z;
		break;
	case 2:
		x -= y;
		y = (-y - start_x) / 2 - z;
		break;
	case 3:
		x += y;
		y = (-y + start_x) / 2 - z;
		break;
	}
	dpi->x += x - width / 2;
	dpi->y += y - height / 2 - 16;
	RCT2_GLOBAL(0x0140E9A8, rct_drawpixelinfo*) = dpi;
	uint32_t d = unknown << 16;
	d |= rideIndex;
	d |= trackType << 8;

	sub_6CBCE2(rideIndex, trackType, trackDirection, d, 4096, 4096, 1024);
}

static rct_map_element _tempTrackMapElement;
static rct_map_element _tempSideTrackMapElement = { 0x80, 0x8F, 128, 128, 0, 0, 0, 0 };
static rct_map_element *_backupMapElementArrays[5];

/**
 *
 * rct2: 0x006CBCE2
 * bh: trackDirection
 * dl: rideIndex
 * dh: trackType
 */
static void sub_6CBCE2(
	int rideIndex, int trackType, int trackDirection, int edx,
	int originX, int originY, int originZ
) {
	rct_ride *ride;
	const rct_preview_track *trackBlock;
	int preserve_word_141E9E4;
	int x, y, baseZ, clearanceZ, offsetX, offsetY;
	uint64 preserve_map_size_vars;

	preserve_word_141E9E4 = RCT2_GLOBAL(0x0141E9E4, uint16);
	RCT2_GLOBAL(0x0141E9E4, uint16) = 0;
	trackDirection &= 3;

	RCT2_GLOBAL(0x00EE7880, uint32) = 0x00F1A4CC;
	painter_setup();

	ride = GET_RIDE(rideIndex);

	preserve_map_size_vars = RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint64);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint16) = 255 * 32;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_MINUS_2, uint16) = (255 * 32) + 286;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE, uint16) = 256;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_MAX_XY, uint16) = (256 * 32) - 1;

	trackBlock = get_track_def_from_ride(ride, trackType);
	while (trackBlock->index != 255) {
		int bl = trackBlock->var_08;
		int bh;
		switch (trackDirection) {
		case 0:
			offsetX =  trackBlock->x;
			offsetY =  trackBlock->y;
			break;
		case 1:
			offsetX =  trackBlock->y;
			offsetY = -trackBlock->x;
			bl = rol8(bl, 1);
			bh = bl;
			bh = ror8(bh, 4);
			bl &= 0xEE;
			bh &= 0x11;
			bl |= bh;
			break;
		case 2:
			offsetX = -trackBlock->x;
			offsetY = -trackBlock->y;
			bl = rol8(bl, 2);
			bh = bl;
			bh = ror8(bh, 4);
			bl &= 0xCC;
			bh &= 0x33;
			bl |= bh;
			break;
		case 3:
			offsetX = -trackBlock->y;
			offsetY =  trackBlock->x;
			bl = rol8(bl, 3);
			bh |= bl;
			bh = ror8(bh, 4);
			bl &= 0x88;
			bh &= 0x77;
			bl |= bh;
			break;
		}
		x = originX + offsetX;
		y = originY + offsetY;
		baseZ = (originZ + trackBlock->z) >> 3;
		clearanceZ = ((trackBlock->var_07 + RCT2_GLOBAL(0x0097D219 + (ride->type * 8), uint8)) >> 3) + baseZ + 4;

		int tileX = x >> 5;
		int tileY = y >> 5;

		// Replace map elements with temporary ones containing track
		_backupMapElementArrays[0] = map_get_first_element_at(tileX + 0, tileY + 0);
		_backupMapElementArrays[1] = map_get_first_element_at(tileX + 1, tileY + 0);
		_backupMapElementArrays[2] = map_get_first_element_at(tileX - 1, tileY + 0);
		_backupMapElementArrays[3] = map_get_first_element_at(tileX + 0, tileY + 1);
		_backupMapElementArrays[4] = map_get_first_element_at(tileX + 0, tileY - 1);
		map_set_tile_elements(tileX + 0, tileY + 0, &_tempTrackMapElement);
		map_set_tile_elements(tileX + 1, tileY + 0, &_tempSideTrackMapElement);
		map_set_tile_elements(tileX - 1, tileY + 0, &_tempSideTrackMapElement);
		map_set_tile_elements(tileX + 0, tileY + 1, &_tempSideTrackMapElement);
		map_set_tile_elements(tileX + 0, tileY - 1, &_tempSideTrackMapElement);

		// Set the temporary track element
		_tempTrackMapElement.type = trackDirection | MAP_ELEMENT_TYPE_TRACK | (edx & 0x10000 ? 0x80 : 0);
		_tempTrackMapElement.flags = (bl & 0x0F) | MAP_ELEMENT_FLAG_LAST_TILE;
		_tempTrackMapElement.base_height = baseZ;
		_tempTrackMapElement.clearance_height = clearanceZ;
		_tempTrackMapElement.properties.track.type = trackType;
		_tempTrackMapElement.properties.track.sequence = trackBlock->index;
		_tempTrackMapElement.properties.track.colour = (edx & 0x20000 ? 4 : 0);
		_tempTrackMapElement.properties.track.ride_index = rideIndex;

		// Draw this map tile
		sub_68B2B7(x, y);

		// Restore map elements
		map_set_tile_elements(tileX + 0, tileY + 0, _backupMapElementArrays[0]);
		map_set_tile_elements(tileX + 1, tileY + 0, _backupMapElementArrays[1]);
		map_set_tile_elements(tileX - 1, tileY + 0, _backupMapElementArrays[2]);
		map_set_tile_elements(tileX + 0, tileY + 1, _backupMapElementArrays[3]);
		map_set_tile_elements(tileX + 0, tileY - 1, _backupMapElementArrays[4]);

		trackBlock++;
	}

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SIZE_UNITS, uint64) = preserve_map_size_vars;

	sub_688217();
	sub_688485();

	RCT2_GLOBAL(0x0141E9E4, uint16) = preserve_word_141E9E4;
}

/**
 * 
 * rct2: 0x006C84CE
 */
void sub_6C84CE()
{
	rct_window *w;
	rct_map_element *mapElement;

	window_ride_construction_update_enabled_track_pieces();
	w = window_find_by_class(WC_RIDE_CONSTRUCTION);
	if (w == NULL)
		return;

	window_ride_construction_update_map_selection();

	RCT2_GLOBAL(0x00F440D0, uint8) = 255;
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED) {
		int x = _currentTrackBeginX;
		int y = _currentTrackBeginY;
		int z = _currentTrackBeginZ;
		if (!sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, 0, &mapElement, 0)) {
			RCT2_GLOBAL(0x00F440D0, uint8) = mapElement->properties.track.type;
			if (mapElement->properties.track.type == 99)
				RCT2_GLOBAL(0x00F440CD, uint8) = (mapElement->properties.track.sequence >> 4) << 1;
			_currentSeatRotationAngle = mapElement->properties.track.colour >> 4;
		}
	}

	window_ride_construction_update_possible_ride_configurations();
	window_ride_construction_update_widgets(w);	
}

/**
 * 
 * rct2: 0x006CA2DF
 * bh: trackRotation (out)
 * dl: ??? (out)
 * dh: trackType (out)
 * edx >> 16: ??? (out)
 */
static bool sub_6CA2DF(int *trackType, int *trackDirection, int *rideIndex, int *edxRS16, int *x, int *y, int *z, int *properties)
{
	int eax, ebx, ecx, edx, esi, edi, ebp;
	if (RCT2_CALLFUNC_X(0x006CA2DF, &eax, &ebx, &ecx, &edx, &esi, &edi, &ebp) & 0x100)
		return true;

	if (trackType != NULL) *trackType = (edx >> 8) & 0xFF;
	if (trackDirection != NULL) *trackDirection = (ebx >> 8) & 0xFF;
	if (rideIndex != NULL) *rideIndex = edx & 0xFF;
	if (edxRS16 != NULL) *edxRS16 = (edx >> 16) & 0xFFFF;
	if (x != NULL) *x = eax & 0xFFFF;
	if (y != NULL) *y = ecx & 0xFFFF;
	if (z != NULL) *z = edi & 0xFFFF;
	if (properties != NULL) *properties = (edi >> 16) & 0xFFFF;
	return false;
}

/**
 * 
 * rct2: 0x006C6A77
 */
static void window_ride_construction_update_enabled_track_pieces()
{
	rct_ride *ride = GET_RIDE(_currentRideIndex);
	rct_ride_type *rideEntry = ride_get_entry(ride);

	int rideType = _currentTrackCovered & 2 ? RCT2_ADDRESS(0x0097D4F5, uint8)[ride->type * 8] : ride->type;
	_enabledRidePiecesA = rideEntry->enabledTrackPiecesA & RCT2_ADDRESS(0x01357444, uint32)[rideType];
	_enabledRidePiecesB = rideEntry->enabledTrackPiecesB & RCT2_ADDRESS(0x01357644, uint32)[rideType];
}

/**
 * 
 * rct2: 0x006CA162
 */
money32 sub_6CA162(int rideIndex, int trackType, int trackDirection, int edxRS16, int x, int y, int z)
{
	rct_ride *ride;
	money32 result;

	sub_6C96C0();
	ride = GET_RIDE(rideIndex);
	if (ride->type == RIDE_TYPE_MAZE) {
		result = game_do_command(x, 105 | (4 << 8), y, rideIndex | (trackType << 8) | (edxRS16 << 16), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
		if (result == MONEY32_UNDEFINED)
			return result;

		RCT2_GLOBAL(0x00F440C5, uint16) = x;
		RCT2_GLOBAL(0x00F440C7, uint16) = y;
		RCT2_GLOBAL(0x00F440C9, uint16) = z;
		RCT2_GLOBAL(0x00F440CB, uint8) = trackDirection;
		_currentTrackSelectionFlags |= 2;
		viewport_set_visibility(RCT2_GLOBAL(0x00F441D4, uint8) & 2 ? 1 : 3);
		if (_currentTrackSlopeEnd != 0)
			viewport_set_visibility(2);

		return result;
	} else {
		result = game_do_command(x, 105 | (trackDirection << 8), y, rideIndex | (trackType << 8) | (edxRS16 << 16), GAME_COMMAND_PLACE_TRACK, z, 0);
		if (result == MONEY32_UNDEFINED)
			return result;

		RCT2_GLOBAL(0x00F440C5, uint16) = x;
		RCT2_GLOBAL(0x00F440C7, uint16) = y;
		z += ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
			RCT2_GLOBAL(0x009972BD + (trackType * 10), uint16) :
			RCT2_GLOBAL(0x009968BD + (trackType * 10), uint16);
		
		RCT2_GLOBAL(0x00F440C9, uint16) = z;
		RCT2_GLOBAL(0x00F440CB, uint8) = trackDirection;
		_currentTrackSelectionFlags |= 2;
		viewport_set_visibility(RCT2_GLOBAL(0x00F441D4, uint8) & 2 ? 1 : 3);
		if (_currentTrackSlopeEnd != 0)
			viewport_set_visibility(2);

		return result;
	}
}

/**
 * 
 * rct2: 0x006C94D8
 */
void sub_6C94D8()
{
	int x, y, z, direction, type, rideIndex, edxRS16;

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_FRONT:
	case RIDE_CONSTRUCTION_STATE_BACK:
		if (!(_currentTrackSelectionFlags & 2)) {
			if (sub_6CA2DF(&type, &direction, &rideIndex, &edxRS16, &x, &y, &z, NULL)) {
				sub_6C96C0();
			} else {
				_currentTrackPrice = sub_6CA162(rideIndex, type, direction, edxRS16, x, y, z);
				sub_6C84CE();
			}
		}
		_rideConstructionArrowPulseTime--;
		if (_rideConstructionArrowPulseTime >= 0)
			break;

		_rideConstructionArrowPulseTime = 5;
		_currentTrackSelectionFlags ^= 1;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection;
		type = _currentTrackPieceType;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z;
		if (direction >= 4)
			direction += 4;
		if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
			direction ^= 2;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = direction;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~4;
		if (_currentTrackSelectionFlags & 1)
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 4;
		map_invalidate_tile_full(x, y);
		break;
	case RIDE_CONSTRUCTION_STATE_SELECTED:
		_rideConstructionArrowPulseTime--;
		if (_rideConstructionArrowPulseTime >= 0)
			break;

		_rideConstructionArrowPulseTime = 5;
		_currentTrackSelectionFlags ^= 1;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		z = _currentTrackBeginZ;
		direction = _currentTrackPieceDirection & 3;
		type = _currentTrackPieceType;
		if (sub_6C683D(&x, &y, &z, direction, type, 0, NULL, _currentTrackSelectionFlags & 1 ? 2 : 1)) {
			sub_6C96C0();
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
		}
		break;
	case 6:
	case 7:
	case 8:
		_rideConstructionArrowPulseTime--;
		if (_rideConstructionArrowPulseTime >= 0)
			break;

		_rideConstructionArrowPulseTime = 5;
		_currentTrackSelectionFlags ^= 1;
		x = _currentTrackBeginX & 0xFFE0;
		y = _currentTrackBeginY & 0xFFE0;
		z = _currentTrackBeginZ + 15;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z;
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = 4;
		if (((_currentTrackBeginX & 0x1F) | (_currentTrackBeginY & 0x1F)) != 0) {
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = 6;
			if (((_currentTrackBeginX & 0x1F) & (_currentTrackBeginY & 0x1F)) == 0) {
				RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = 5;
				if ((_currentTrackBeginY & 0x1F) == 0)
					RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = 7;
			}
		}
		RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~4;
		if (_currentTrackSelectionFlags & 1)
			RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 4;
		map_invalidate_tile_full(x, y);
		break;
	}
}

/**
 * 
 * rct2: 0x006C84E2
 */
static void window_ride_construction_update_map_selection()
{
	rct_ride *ride;
	int trackType, trackDirection, x, y;

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 10;

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_0:
		trackDirection = _currentTrackPieceDirection;
		trackType = 0;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		break;
	case RIDE_CONSTRUCTION_STATE_SELECTED:
		trackDirection = _currentTrackPieceDirection;
		trackType = _currentTrackPieceType;
		x = _currentTrackBeginX;
		y = _currentTrackBeginY;
		break;
	default:
		if (sub_6CA2DF(&trackType, &trackDirection, NULL, NULL, &x, &y, NULL, NULL)) {
			trackDirection = _currentTrackPieceDirection;
			trackType = 0;
			x = _currentTrackBeginX;
			y = _currentTrackBeginY;
		}
		break;
	}

	ride = GET_RIDE(_currentRideIndex);
	window_ride_construction_select_map_tiles(ride, trackType, trackDirection, x, y);
	map_invalidate_map_selection_tiles();
}

/**
 * 
 * rct2: 0x006C8648
 */
static void window_ride_construction_update_possible_ride_configurations()
{
	rct_ride *ride;
	int trackType;
	int edx, edi;

	ride = GET_RIDE(_currentRideIndex);

	RCT2_GLOBAL(0x00F440D3, uint8) = 0;
	if (_currentTrackCovered & 2)
		edi = RCT2_GLOBAL(0x0097D4F5 + (ride->type * 8), uint8);
	else
		edi = ride->type;

	int currentPossibleRideConfigurationIndex = 0;
	_numCurrentPossibleSpecialTrackPieces = 0;
	for (trackType = 0; trackType < 256; trackType++) {
		edx = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
			gFlatRideTrackDefinitions[trackType].type :
			gTrackDefinitions[trackType].type;

		if (edx == 0)
			continue;

		if (edx & 0x80) {
			edx &= 0x7F;
			if (edx != edi)
				continue;
		} else if (!is_track_enabled(edx)) {
			continue;
		}

		int slope, bank;
		if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT || _rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE) {
			if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
				slope = gFlatRideTrackDefinitions[trackType].vangle_start;
				bank = gFlatRideTrackDefinitions[trackType].bank_start;
			} else {
				slope = gTrackDefinitions[trackType].vangle_start;
				bank = gTrackDefinitions[trackType].bank_start;
			}
		} else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
			if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
				slope = gFlatRideTrackDefinitions[trackType].vangle_end;
				bank = gFlatRideTrackDefinitions[trackType].bank_end;
			} else {
				slope = gTrackDefinitions[trackType].vangle_end;
				bank = gTrackDefinitions[trackType].bank_end;
			}
		} else {
			continue;
		}

		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)) {
			if (
				gTrackDefinitions[trackType].type == TRACK_HELIX_SMALL ||
				gTrackDefinitions[trackType].type == TRACK_HELIX_LARGE
			) {
				if (bank != _previousTrackBankEnd) {
					if (_previousTrackBankEnd != TRACK_BANK_NONE)
						continue;

					if (bank != TRACK_BANK_LEFT)
						continue;
				}
			}
		}

		if (bank == TRACK_BANK_UPSIDE_DOWN && bank != _previousTrackBankEnd)
			continue;
		
		_currentPossibleRideConfigurations[currentPossibleRideConfigurationIndex] = trackType;
		RCT2_GLOBAL(0x00F4409C, uint32) |= (1 << currentPossibleRideConfigurationIndex);
		if (
			_currentTrackPieceDirection < 4 &&
			slope == _previousTrackSlopeEnd &&
			bank == _previousTrackBankEnd &&
			(trackType != TRACK_ELEM_TOWER_BASE || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_29))
		) {
			RCT2_GLOBAL(0x00F4409C, uint32) &= ~(1 << currentPossibleRideConfigurationIndex);
			_numCurrentPossibleSpecialTrackPieces++;
		}
		currentPossibleRideConfigurationIndex++;
	}
	_numCurrentPossibleRideConfigurations = currentPossibleRideConfigurationIndex;
}

/**
 * 
 * rct2: 0x006C87F5
 */
static void window_ride_construction_update_widgets(rct_window *w)
{
	uint8 rideIndex = _currentRideIndex;
	rct_ride *ride = GET_RIDE(rideIndex);
	int rideType = ride_get_alternative_type(ride);

	w->hold_down_widgets = 0;
	if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_IS_SHOP)) {
		window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_ENTRANCE].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_EXIT].type = WWT_EMPTY;
	} else {
		window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WWT_GROUPBOX;
		window_ride_construction_widgets[WIDX_ENTRANCE].type = WWT_DROPDOWN_BUTTON;
		window_ride_construction_widgets[WIDX_EXIT].type = WWT_DROPDOWN_BUTTON;
	}
	
	if (_numCurrentPossibleSpecialTrackPieces == 0) {
		window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WWT_EMPTY;
	} else {
		window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
	}

	if (is_track_enabled(TRACK_STRAIGHT)) {
		window_ride_construction_widgets[WIDX_STRAIGHT].type = WWT_FLATBTN;
	} else {
		window_ride_construction_widgets[WIDX_STRAIGHT].type = WWT_EMPTY;
	}

	if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_30)) {
		window_ride_construction_widgets[WIDX_LEFT_CURVE_LARGE].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_LARGE].type = WWT_FLATBTN;
	} else {
		window_ride_construction_widgets[WIDX_LEFT_CURVE_LARGE].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_LARGE].type = WWT_EMPTY;
	}

	window_ride_construction_widgets[WIDX_LEFT_CURVE].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 28;
	window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 49;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 116;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 137;
	window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = 5138;
	window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = 5139;
	if (is_track_enabled(TRACK_CURVE_VERTICAL)) {
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = 5140;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = 5141;
	}
	if (is_track_enabled(TRACK_CURVE)) {
		window_ride_construction_widgets[WIDX_LEFT_CURVE].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = 5140;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = 5141;
	}
	if (is_track_enabled(TRACK_CURVE_SMALL)) {
		window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WWT_FLATBTN;
	}
	if (is_track_enabled(TRACK_CURVE_VERY_SMALL)) {
		window_ride_construction_widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WWT_FLATBTN;
	}

	window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_LEVEL].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = 5144;
	window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
	window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = 5148;
	window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
	if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER) {
		window_ride_construction_widgets[WIDX_LEVEL].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_FLATBTN;
	}
	if (is_track_enabled(TRACK_SLOPE) || is_track_enabled(TRACK_SLOPE_STEEP)) {
		window_ride_construction_widgets[WIDX_LEVEL].type = WWT_FLATBTN;
	}
	if (is_track_enabled(TRACK_SLOPE)) {
		window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_FLATBTN;
	}
	if (
		is_track_enabled(TRACK_HELIX_SMALL) &&
		_currentTrackBankEnd != TRACK_BANK_NONE &&
		_currentTrackSlopeEnd == TRACK_SLOPE_NONE
	) {
		if (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL) {
			// Enable helix
			window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_FLATBTN;
			if (rideType != RIDE_TYPE_SPLASH_BOATS)
				window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_FLATBTN;
		}
	}

	if (is_track_enabled(TRACK_SLOPE_STEEP)) {
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_FLATBTN;
		if (rideType != RIDE_TYPE_SPLASH_BOATS)
			window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_FLATBTN;
	}

	int x;
	if (is_track_enabled(TRACK_LIFT_HILL) && _currentTrackCurve < 256) {
		window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WWT_FLATBTN;
		x = 9;
	} else {
		window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WWT_EMPTY;
		x = 23;
	}

	for (int i = WIDX_SLOPE_DOWN_STEEP; i <= WIDX_SLOPE_UP_STEEP; i++) {
		window_ride_construction_widgets[i].left = x;
		window_ride_construction_widgets[i].right = x + 23;
		x += 24;
	}

	window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = 5148;
	window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
	window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = 5144;
	window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
	if (is_track_enabled(TRACK_SLOPE_VERTICAL)) {
		if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90) {
			int originalSlopeUpSteepLeft = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left;
			int originalSlopeUpSteepRight = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right;
			for (int i = WIDX_SLOPE_UP_STEEP; i > WIDX_SLOPE_DOWN_STEEP; i--) {
				window_ride_construction_widgets[i].left = window_ride_construction_widgets[i - 1].left;
				window_ride_construction_widgets[i].right = window_ride_construction_widgets[i - 1].right;
			}
			window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left = originalSlopeUpSteepLeft;
			window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right = originalSlopeUpSteepRight;
			window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = 5149;
			window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_RISE_TIP;
		} else if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 || _previousTrackSlopeEnd == TRACK_SLOPE_DOWN_90) {
			int originalSlopeDownSteepLeft = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
			int originalSlopeDownSteepRight = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right;
			for (int i = WIDX_SLOPE_DOWN_STEEP; i < WIDX_SLOPE_UP_STEEP; i++) {
				window_ride_construction_widgets[i].left = window_ride_construction_widgets[i + 1].left;
				window_ride_construction_widgets[i].right = window_ride_construction_widgets[i + 1].right;
			}
			window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left = originalSlopeDownSteepLeft;
			window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right = originalSlopeDownSteepRight;
			window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = 5150;
			window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_DROP_TIP;
		}
	}

	if (
		is_track_enabled(TRACK_HELIX_LARGE_UNBANKED) &&
		_currentTrackSlopeEnd == TRACK_SLOPE_NONE &&
		_currentTrackBankEnd == TRACK_BANK_NONE &&
		(_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT)
	) {
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = 5151;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = 5152;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

		int tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left = window_ride_construction_widgets[WIDX_SLOPE_DOWN].left;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN].left = tmp;

		tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right = window_ride_construction_widgets[WIDX_SLOPE_DOWN].right;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN].right = tmp;

		tmp = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right = window_ride_construction_widgets[WIDX_SLOPE_UP].right;
		window_ride_construction_widgets[WIDX_SLOPE_UP].right = tmp;
	}

	if (
		(is_track_enabled(TRACK_HELIX_LARGE) || is_track_enabled(TRACK_HELIX_SMALL)) &&
		(_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL) &&
		_currentTrackSlopeEnd == TRACK_SLOPE_NONE &&
		_currentTrackBankEnd != TRACK_BANK_NONE
	) {
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = 5151;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = 5152;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

		int tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left = window_ride_construction_widgets[WIDX_SLOPE_DOWN].left;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN].left = tmp;

		tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right = window_ride_construction_widgets[WIDX_SLOPE_DOWN].right;
		window_ride_construction_widgets[WIDX_SLOPE_DOWN].right = tmp;

		tmp = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left = window_ride_construction_widgets[WIDX_SLOPE_UP].left;
		window_ride_construction_widgets[WIDX_SLOPE_UP].left = tmp;

		tmp = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right;
		window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right = window_ride_construction_widgets[WIDX_SLOPE_UP].right;
		window_ride_construction_widgets[WIDX_SLOPE_UP].right = tmp;
	}

	window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].image = STR_RIDE_CONSTRUCTION_ROLL_BANKING;
	window_ride_construction_widgets[WIDX_BANK_LEFT].image = 5153;
	window_ride_construction_widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP;
	window_ride_construction_widgets[WIDX_BANK_LEFT].left = 47;
	window_ride_construction_widgets[WIDX_BANK_LEFT].right = 70;
	window_ride_construction_widgets[WIDX_BANK_LEFT].top = 132;
	window_ride_construction_widgets[WIDX_BANK_LEFT].bottom = 155;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].image = 5154;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_NO_ROLL_TIP;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].left = 71;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].right = 94;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].top = 132;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].bottom = 155;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].image = 5155;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].left = 95;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].right = 118;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].top = 132;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].bottom = 155;
	window_ride_construction_widgets[WIDX_BANK_LEFT].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_U_TRACK].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_O_TRACK].type = WWT_EMPTY;
	if (RCT2_GLOBAL(0x00F440D0, uint8) != 99 && _currentTrackCurve != 355) {
		if (is_track_enabled(TRACK_FLAT_ROLL_BANKING)) {
			window_ride_construction_widgets[WIDX_BANK_LEFT].type = WWT_FLATBTN;
			window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WWT_FLATBTN;
			window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WWT_FLATBTN;
		}
		if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES)) {
			if (rideType == RIDE_TYPE_WATER_COASTER) {
				window_ride_construction_widgets[WIDX_U_TRACK].image = 5158;
				window_ride_construction_widgets[WIDX_O_TRACK].image = 5159;
				window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_STANDARD_RC_TRACK_TIP;
				window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_WATER_CHANNEL_TIP;
				if (_currentTrackCurve < TRACK_CURVE_LEFT_SMALL && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_BANK_NONE) {
					window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].image = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
					window_ride_construction_widgets[WIDX_U_TRACK].type = WWT_FLATBTN;
					window_ride_construction_widgets[WIDX_O_TRACK].type = WWT_FLATBTN;
				}
			} else {
				window_ride_construction_widgets[WIDX_U_TRACK].image = 5156;
				window_ride_construction_widgets[WIDX_O_TRACK].image = 5157;
				window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP;
				window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP;
				window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].image = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
				window_ride_construction_widgets[WIDX_U_TRACK].type = WWT_FLATBTN;
				window_ride_construction_widgets[WIDX_O_TRACK].type = WWT_FLATBTN;
			}
		}
	} else {
		window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].image = STR_RIDE_CONSTRUCTION_BRAKE_SPEED;
		RCT2_GLOBAL(0x00F440D3, uint8) = 1;
		window_ride_construction_widgets[WIDX_BANK_LEFT].image = 1675;
		window_ride_construction_widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
		window_ride_construction_widgets[WIDX_BANK_LEFT].type = WWT_SPINNER;
		window_ride_construction_widgets[WIDX_BANK_LEFT].left = 12;
		window_ride_construction_widgets[WIDX_BANK_LEFT].right = 83;
		window_ride_construction_widgets[WIDX_BANK_LEFT].top = 138;
		window_ride_construction_widgets[WIDX_BANK_LEFT].bottom = 149;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WWT_DROPDOWN_BUTTON;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].image = STR_NUMERIC_UP;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].left = 72;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].right = 82;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].top = 139;
		window_ride_construction_widgets[WIDX_BANK_STRAIGHT].bottom = 143;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WWT_DROPDOWN_BUTTON;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].image = STR_NUMERIC_DOWN;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].left = 72;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].right = 82;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].top = 144;
		window_ride_construction_widgets[WIDX_BANK_RIGHT].bottom = 148;
		w->hold_down_widgets |= (1 << WIDX_BANK_STRAIGHT) | (1 << WIDX_BANK_RIGHT);
	}

	window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 162;
	window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = 0;
	window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = 0;
	window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = 0;
	if (
		(rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER || rideType == RIDE_TYPE_38) &&
		RCT2_GLOBAL(0x00F440D0, uint8) != 99 &&
		_currentTrackCurve != 355
	) {
		window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WWT_GROUPBOX;
		window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WWT_SPINNER;
		window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WWT_DROPDOWN_BUTTON;
		window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WWT_DROPDOWN_BUTTON;
		window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 92;
		if (window_ride_construction_widgets[WIDX_BANK_LEFT].type != WWT_SPINNER) {
			for (int i = WIDX_BANK_LEFT; i <= WIDX_BANK_RIGHT; i++) {
				window_ride_construction_widgets[i].left -= 36;
				window_ride_construction_widgets[i].right -= 36;
			}
		}
	}

	uint64 pressedWidgets = w->pressed_widgets & (
		(1 << WIDX_BACKGROUND) |
		(1 << WIDX_TITLE) |
		(1 << WIDX_CLOSE) |
		(1 << WIDX_DIRECTION_GROUPBOX) |
		(1 << WIDX_SLOPE_GROUPBOX) |
		(1 << WIDX_BANKING_GROUPBOX) |
		(1 << WIDX_CONSTRUCT) |
		(1 << WIDX_DEMOLISH) |
		(1 << WIDX_PREVIOUS_SECTION) |
		(1 << WIDX_NEXT_SECTION) |
		(1 << WIDX_ENTRANCE_EXIT_GROUPBOX) |
		(1 << WIDX_ENTRANCE) |
		(1 << WIDX_EXIT)
	);

	window_ride_construction_widgets[WIDX_CONSTRUCT].type = WWT_EMPTY;
	window_ride_construction_widgets[WIDX_DEMOLISH].type = WWT_FLATBTN;
	window_ride_construction_widgets[WIDX_ROTATE].type = WWT_EMPTY;
	if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS)) {
		window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_EMPTY;
	} else {
		window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_FLATBTN;
		window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_FLATBTN;
	}

	switch (_rideConstructionState) {
	case RIDE_CONSTRUCTION_STATE_FRONT:
		window_ride_construction_widgets[WIDX_CONSTRUCT].type = WWT_IMGBTN;
		window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_EMPTY;
		break;
	case RIDE_CONSTRUCTION_STATE_BACK:
		window_ride_construction_widgets[WIDX_CONSTRUCT].type = WWT_IMGBTN;
		window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_EMPTY;
		break;
	case RIDE_CONSTRUCTION_STATE_PLACE:
		window_ride_construction_widgets[WIDX_CONSTRUCT].type = WWT_IMGBTN;
		window_ride_construction_widgets[WIDX_DEMOLISH].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_ROTATE].type = WWT_FLATBTN;
		break;
	case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
		window_ride_construction_widgets[WIDX_DEMOLISH].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_EMPTY;
		window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_EMPTY;
		break;
	default:
		w->pressed_widgets = pressedWidgets;
		window_invalidate(w);
		return;
	}

	int widgetIndex;
	switch (_currentTrackCurve) {
	case TRACK_CURVE_NONE:
		widgetIndex = WIDX_STRAIGHT;
		break;
	case TRACK_CURVE_LEFT:
		widgetIndex = WIDX_LEFT_CURVE;
		break;
	case TRACK_CURVE_RIGHT:
		widgetIndex = WIDX_RIGHT_CURVE;
		break;
	case TRACK_CURVE_LEFT_SMALL:
		widgetIndex = WIDX_LEFT_CURVE_SMALL;
		break;
	case TRACK_CURVE_RIGHT_SMALL:
		widgetIndex = WIDX_RIGHT_CURVE_SMALL;
		break;
	case TRACK_CURVE_LEFT_VERY_SMALL:
		widgetIndex = WIDX_LEFT_CURVE_VERY_SMALL;
		break;
	case TRACK_CURVE_RIGHT_VERY_SMALL:
		widgetIndex = WIDX_RIGHT_CURVE_VERY_SMALL;
		break;
	case TRACK_CURVE_LEFT_LARGE:
		widgetIndex = WIDX_LEFT_CURVE_LARGE;
		break;
	case TRACK_CURVE_RIGHT_LARGE:
		widgetIndex = WIDX_RIGHT_CURVE_LARGE;
		break;
	default:
		widgetIndex = WIDX_SPECIAL_TRACK_DROPDOWN;
		break;
	}
	pressedWidgets |= (1ULL << widgetIndex);

	switch (_currentTrackSlopeEnd) {
	case TRACK_SLOPE_DOWN_60:
	case TRACK_SLOPE_UP_90:
		widgetIndex = WIDX_SLOPE_DOWN_STEEP;
		break;
	case TRACK_SLOPE_DOWN_25:
		widgetIndex = WIDX_SLOPE_DOWN;
		break;
	case TRACK_SLOPE_UP_25:
		widgetIndex = WIDX_SLOPE_UP;
		break;
	case TRACK_SLOPE_UP_60:
	case TRACK_SLOPE_DOWN_90:
		widgetIndex = WIDX_SLOPE_UP_STEEP;
		break;
	default:
		widgetIndex = WIDX_LEVEL;
		break;
	}
	pressedWidgets |= (1ULL << widgetIndex);

	if (RCT2_GLOBAL(0x00F440D3, uint8) == 0) {
		if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES)) {
			if (_currentTrackCovered & 1) {
				w->pressed_widgets |= (1ULL << WIDX_O_TRACK);
			} else {
				w->pressed_widgets |= (1ULL << WIDX_U_TRACK);
			}
		}
		switch (_currentTrackBankEnd) {
		case TRACK_BANK_LEFT:
			widgetIndex = WIDX_BANK_LEFT;
			break;
		case TRACK_BANK_NONE:
			widgetIndex = WIDX_BANK_STRAIGHT;
			break;
		default:
			widgetIndex = WIDX_BANK_RIGHT;
			break;
		}
		pressedWidgets |= (1ULL << widgetIndex);
	}

	if (_currentTrackLiftHill & 1)
		pressedWidgets |= (1 << WIDX_CHAIN_LIFT);

	w->pressed_widgets = pressedWidgets;
	window_invalidate(w);
}

static void window_ride_construction_select_map_tiles(rct_ride *ride, int trackType, int trackDirection, int x, int y)
{
	const rct_preview_track *trackBlock;
	int offsetX, offsetY;

	trackBlock = get_track_def_from_ride(ride, trackType);
	trackDirection &= 3;
	int selectionTileIndex = 0;
	while (trackBlock->index != 255) {
		switch (trackDirection) {
		case 0:
			offsetX = trackBlock->x;
			offsetY = trackBlock->y;
			break;
		case 1:
			offsetX = trackBlock->y;
			offsetY = -trackBlock->x;
			break;
		case 2:
			offsetX = -trackBlock->x;
			offsetY = -trackBlock->y;
			break;
		case 3:
			offsetX = -trackBlock->y;
			offsetY = trackBlock->x;
			break;
		}
		gMapSelectionTiles[selectionTileIndex].x = x + offsetX;
		gMapSelectionTiles[selectionTileIndex].y = y + offsetY;
		selectionTileIndex++;
		trackBlock++;
	}
	gMapSelectionTiles[selectionTileIndex].x = -1;
	gMapSelectionTiles[selectionTileIndex].y = -1;
}

/**
 * 
 * rct2: 0x006C776D
 */
static void window_ride_construction_show_special_track_dropdown(rct_window *w, rct_widget *widget)
{
	window_dropdown_show_text_custom_width(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1],
		0,
		_numCurrentPossibleRideConfigurations,
		widget->right - widget->left
	);

	for (int i = 0; i < _numCurrentPossibleRideConfigurations; i++) {
		uint8 trackPiece = _currentPossibleRideConfigurations[i];
		rct_string_id trackPieceStringId = RideConfigurationStringIds[trackPiece];
		if (trackPieceStringId == STR_RAPIDS) {
			rct_ride *ride = GET_RIDE(_currentRideIndex);
			if (ride->type == RIDE_TYPE_CAR_RIDE)
				trackPieceStringId = STR_LOG_BUMPS;
		}
		gDropdownItemsFormat[i] = trackPieceStringId;
		if ((trackPiece | 0x100) == _currentTrackCurve) {
			RCT2_GLOBAL(0x009DEBA2, sint16) = i;
		}
	}
	*gDropdownItemsDisabled = RCT2_GLOBAL(0x00F4409C, uint32);
}

/**
 * 
 * rct2: 0x006C7630
 */
static void ride_selected_track_set_seat_rotation(int seatRotation)
{
	int x, y, z;
	x = _currentTrackBeginX;
	y = _currentTrackBeginY;
	z = _currentTrackBeginZ;
	sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, seatRotation, NULL, (1 << 5));
	sub_6C84CE();
}

/**
 * 
 * rct2: 0x006C7502
 */
static void loc_6C7502(int al)
{
	_currentTrackSlopeEnd = al;
	_currentTrackPrice = MONEY32_UNDEFINED;
	if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT) {
		if (al != 2 && al != 4 && al != 0) {
			_currentTrackLiftHill &= ~1;
		}
	}
	sub_6C84CE();
}

/**
 * 
 * rct2: 0x006C76E9
 */
static void ride_construction_set_brakes_speed(int brakesSpeed)
{
	rct_map_element *mapElement;
	int x, y, z;

	x = _currentTrackBeginX;
	y = _currentTrackBeginY;
	z = _currentTrackBeginZ;
	if (!sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, 0, &mapElement, 0)) {
		game_do_command(
			_currentTrackBeginX,
			GAME_COMMAND_FLAG_APPLY | ((brakesSpeed) << 8),
			_currentTrackBeginY,
			mapElement->properties.track.type,
			GAME_COMMAND_SET_BRAKES_SPEED,
			_currentTrackBeginZ,
			0
		);
	}
	sub_6C84CE();
}

/**
 * 
 * rct2: 0x006CC6A8
 */
void ride_construction_toolupdate_construct(int screenX, int screenY)
{
	int x, y, z, highestZ;
	rct_ride *ride;
	const rct_preview_track *trackBlock;

	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 | 2 | 4);
	if (!ride_get_place_position_from_screen_position(screenX, screenY, &x, &y)) {
		sub_6C9627();
		map_invalidate_map_selection_tiles();
		return;
	}
		
	z = RCT2_GLOBAL(0x00F44163, uint16);
	if (z == 0)
		z = map_get_highest_z(x >> 5, y >> 5);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 2;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~8;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 4;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = _currentTrackPieceDirection;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z;
	gMapSelectionTiles[0].x = x;
	gMapSelectionTiles[0].y = y;
	gMapSelectionTiles[1].x = -1;
	gMapSelectionTiles[1].y = -1;

	int trackType, trackDirection, rideIndex, edxRS16;
	if (sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, NULL, NULL, NULL, NULL)) {
		sub_6C9627();
		map_invalidate_map_selection_tiles();
		return;
	}
	_currentTrackPieceType = trackType;
	ride = GET_RIDE(_currentRideIndex);
		
	// Re-using this other code, very slight difference from original
	//   - Original code checks for MSB mask instead of 255 on trackPart->var_00
	//   - Original code checks this first as its already set origin tile, probably just a micro optimisation
	window_ride_construction_select_map_tiles(ride, trackType, trackDirection, x, y);

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z;
	if (RCT2_GLOBAL(0x00F44163, uint16) == 0) {
		// Raise z above all slopes and water
		highestZ = 0;
		if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & 2) {
			rct_xy16 *selectedTile = gMapSelectionTiles;
			while (selectedTile->x != -1) {
				if (selectedTile->x < (256 * 32) && selectedTile->y < (256 * 32)) {
					z = map_get_highest_z(selectedTile->x >> 5, selectedTile->y >> 5);
					if (z > highestZ)
						highestZ = z;
				}
				selectedTile++;
			}
		}
		// loc_6CC8BF:
		// z = map_get_highest_z(x >> 5, y >> 5);
	}
	// loc_6CC91B:
	trackBlock = get_track_def_from_ride(ride, trackType);
	int bx = 0;
	do {
		bx = min(bx, trackBlock->z);
		trackBlock++;
	} while (trackBlock->index != 255);
	z -= bx;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = z;
	bx = 41;
	_currentTrackBeginX = x;
	_currentTrackBeginY = y;
	_currentTrackBeginZ = z;
	if (
		(_currentTrackSelectionFlags & 2) &&
		x == _previousTrackPieceX &&
		y == _previousTrackPieceY &&
		z == _previousTrackPieceZ
	) {
		map_invalidate_map_selection_tiles();
		return;
	}

	_previousTrackPieceX = x;
	_previousTrackPieceY = y;
	_previousTrackPieceZ = z;
	if (ride->type == RIDE_TYPE_MAZE) {
		for (;;) {
			sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, &x, &y, &z, NULL);
			_currentTrackPrice = sub_6CA162(rideIndex, trackType, trackDirection, edxRS16, x, y, z);
			if (_currentTrackPrice != MONEY32_UNDEFINED)
				break;

			bx--;
			if (bx == 0)
				break;

			_currentTrackBeginZ -= 8;
			if (_currentTrackBeginZ & 0x8000)
				break;

			if (bx >= 0)
				_currentTrackBeginZ += 16;
		}

		window_maze_construction_update_pressed_widgets();
		map_invalidate_map_selection_tiles();
		return;
	}

	for (;;) {
		sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, &x, &y, &z, NULL);
		_currentTrackPrice = sub_6CA162(rideIndex, trackType, trackDirection, edxRS16, x, y, z);
		if (_currentTrackPrice != MONEY32_UNDEFINED)
			break;

		bx--;
		if (bx == 0)
			break;

		_currentTrackBeginZ -= 8;
		if (_currentTrackBeginZ & 0x8000)
			break;

		if (bx >= 0)
			_currentTrackBeginZ += 16;
	}

	sub_6C84CE();
	map_invalidate_map_selection_tiles();
}

/**
 * 
 * rct2: 0x006CD354
 */
void ride_construction_toolupdate_entrance_exit(int screenX, int screenY)
{
	int x, y, direction;
	uint8 unk;

	map_invalidate_selection_rect();
	map_invalidate_map_selection_tiles();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 | 2 | 4);
	ride_get_entrance_or_exit_position_from_screen_position(screenX, screenY, &x, &y, &direction);
	if (RCT2_GLOBAL(0x00F44194, uint8) == 255) {
		sub_6C9627();
		return;
	}
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) |= 1 | 4;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_TYPE, uint16) = 4;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_A_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_B_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_DIRECTION, uint8) = direction ^ 2;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_X, uint16) = x;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Y, uint16) = y;
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_ARROW_Z, uint16) = RCT2_GLOBAL(0x00F44190, uint8) * 8;
	map_invalidate_selection_rect();

	direction = RCT2_GLOBAL(0x00F44194, uint8) ^ 2;
	unk = RCT2_GLOBAL(0x00F44193, uint8);
	if (
		!(_currentTrackSelectionFlags & 4) ||
		x != RCT2_GLOBAL(0x00F440BF, uint16) ||
		y != RCT2_GLOBAL(0x00F440C1, uint16) ||
		direction != RCT2_GLOBAL(0x00F440C3, uint8) ||
		unk != RCT2_GLOBAL(0x00F440C4, uint8)
	) {
		_currentTrackPrice = ride_get_entrance_or_exit_price(
			_currentRideIndex, x, y, direction, RCT2_GLOBAL(0x00F44191, uint8), unk
		);
		sub_6C84CE();
	}
}

/**
 * 
 * rct2: 0x006CCA73
 */
void ride_construction_tooldown_construct(int screenX, int screenY)
{
	int trackType, trackDirection, rideIndex, edxRS16, x, y, z, properties, highestZ;
	rct_window *w;

	map_invalidate_map_selection_tiles();
	sub_6C9627();

	if (sub_6CA2DF(&trackType, &trackDirection, &rideIndex, &edxRS16, &x, &y, &z, &properties))
		return;

	_currentTrackPieceType = trackType;

	// Raise z above all slopes and water
	highestZ = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) & 2) {
		rct_xy16 *selectedTile = gMapSelectionTiles;
		while (selectedTile->x != -1) {
			if (selectedTile->x >= (256 * 32) || selectedTile->y >= (256 * 32))
				continue;

			z = map_get_highest_z(selectedTile->x >> 5, selectedTile->y >> 5);
			if (z > highestZ)
				highestZ = z;

			selectedTile++;
		}
	}
	RCT2_GLOBAL(0x00F440E2, uint16) = z;

	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 | 2 | 4);
	if (!ride_get_place_position_from_screen_position(screenX, screenY, &x, &y))
		return;

	z = RCT2_GLOBAL(0x00F44163, uint16);
	if (z == 0)
		z = map_get_highest_z(x >> 5, y >> 5);

	tool_cancel();

	rct_ride *ride = GET_RIDE(_currentRideIndex);
	if (RCT2_GLOBAL(0x00F44163, uint16) == 0) {
		const rct_preview_track *trackBlock = get_track_def_from_ride(ride, _currentTrackPieceType);
		int bx = 0;
		do {
			bx = min(bx, trackBlock->z);
			trackBlock++;
		} while (trackBlock->index != 255);
		z -= bx;

		// FIX not sure exactly why it starts trial and error place from a lower Z, but it causes issues with disable clearance
		if (!gCheatsDisableClearanceChecks) {
			z -= 16;
		}
	} else {
		z = RCT2_GLOBAL(0x00F44163, uint16);
	}

	if (ride->type == RIDE_TYPE_MAZE) {
		for (int zAttempts = 41; zAttempts >= 0; zAttempts--) {
			_rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
			_currentTrackBeginX = x;
			_currentTrackBeginY = y;
			_currentTrackBeginZ = z;
			_currentTrackSelectionFlags = 0;
			_rideConstructionArrowPulseTime = 0;
			window_maze_construction_update_pressed_widgets();
			w = window_find_by_class(WC_RIDE_CONSTRUCTION);
			if (w == NULL)
				break;

			RCT2_GLOBAL(0x009A8C29, uint8) |= 1;
			
			RCT2_GLOBAL(0x00141E9AE, rct_string_id) = 927;
			RCT2_GLOBAL(0x00F44074, money32) = game_do_command(
				_currentTrackBeginX, 
				GAME_COMMAND_FLAG_APPLY | (4 << 8), 
				_currentTrackBeginY, 
				_currentRideIndex, 
				GAME_COMMAND_SET_MAZE_TRACK, 
				_currentTrackBeginZ, 
				0);

			RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

			if (RCT2_GLOBAL(0x00F44074, money32) == MONEY32_UNDEFINED) {
				rct_string_id errorText = RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id);
				z -= 8;
				if (
					errorText == STR_NOT_ENOUGH_CASH_REQUIRES ||
					errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER ||
					errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER ||
					errorText == STR_RIDE_CANT_BUILD_THIS_UNDERWATER ||
					errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND ||
					errorText == STR_TOO_HIGH_FOR_SUPPORTS ||
					zAttempts == 0 ||
					z < 0
					) {
					sound_play_panned(SOUND_ERROR, RCT2_GLOBAL(0x0142406C, sint32), x, y, z);
					w = window_find_by_class(WC_RIDE_CONSTRUCTION);
					if (w != NULL){
						tool_set(w, 23, 12);
						RCT2_GLOBAL(0x009DE518, uint32) |= (1 << 6);
						RCT2_GLOBAL(0x00F44159, uint8) = 0;
						RCT2_GLOBAL(0x00F4415C, uint8) = 0;
					}
					window_maze_construction_update_pressed_widgets();
					break;
				}
				else if (zAttempts >= 0) {
					z += 16;
				}
			}
			else {
				window_close_by_class(WC_ERROR);
				sound_play_panned(SOUND_PLACE_ITEM, 0x8001, _currentTrackBeginX, _currentTrackBeginY, _currentTrackBeginZ);
				break;
			}
		}
		return;
	}

	for (int zAttempts = 41; zAttempts >= 0; zAttempts--) {
		_rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
		_currentTrackBeginX = x;
		_currentTrackBeginY = y;
		_currentTrackBeginZ = z;
		_currentTrackSelectionFlags = 0;
		_rideConstructionArrowPulseTime = 0;
		sub_6C84CE();
		w = window_find_by_class(WC_RIDE_CONSTRUCTION);
		if (w == NULL)
			break;

		RCT2_GLOBAL(0x009A8C29, uint8) |= 1;
		window_event_mouse_up_call(w, WIDX_CONSTRUCT);
		RCT2_GLOBAL(0x009A8C29, uint8) &= ~1;

		if (RCT2_GLOBAL(0x00F44074, money32) == MONEY32_UNDEFINED) {
			rct_string_id errorText = RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TEXT, rct_string_id);
			z -= 8;
			if (
				errorText == STR_NOT_ENOUGH_CASH_REQUIRES ||
				errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER ||
				errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER ||
				errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND ||
				errorText == STR_TOO_HIGH_FOR_SUPPORTS ||
				zAttempts == 0 ||
				z < 0
			) {
				int saveTrackDirection = _currentTrackPieceDirection;
				int saveCurrentTrackCurve = _currentTrackCurve;
				int savePreviousTrackSlopeEnd = _previousTrackSlopeEnd;
				int saveCurrentTrackSlopeEnd = _currentTrackSlopeEnd;
				int savePreviousTrackBankEnd = _previousTrackBankEnd;
				int saveCurrentTrackBankEnd = _currentTrackBankEnd;
				int saveCurrentTrackCovered = _currentTrackCovered;
				int saveCurrentTrackLiftHill = _currentTrackLiftHill;
				
				sub_6CC3FB(_currentRideIndex);

				_currentTrackPieceDirection = saveTrackDirection;
				_currentTrackCurve = saveCurrentTrackCurve;
				_previousTrackSlopeEnd = savePreviousTrackSlopeEnd;
				_currentTrackSlopeEnd = saveCurrentTrackSlopeEnd;
				_previousTrackBankEnd = savePreviousTrackBankEnd;
				_currentTrackBankEnd = saveCurrentTrackBankEnd;
				_currentTrackCovered = saveCurrentTrackCovered;
				_currentTrackLiftHill = saveCurrentTrackLiftHill;

				sound_play_panned(SOUND_ERROR, RCT2_GLOBAL(0x0142406C, sint32), x, y, z);
				break;
			} else if (zAttempts >= 0) {
				z += 16;
			}
		} else {
			window_close_by_class(WC_ERROR);
			if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0) {
				w = window_find_by_class(WC_RIDE_CONSTRUCTION);
				if (w != NULL) {
					if (ride_are_all_possible_entrances_and_exits_built(ride)) {
						window_close(w);
					} else {
						window_event_mouse_up_call(w, WIDX_ENTRANCE);
					}
				}
			}
			break;
		}
	}
}

/**
 * 
 * rct2: 0x006CCA73
 */
static void ride_construction_tooldown_entrance_exit(int screenX, int screenY)
{
	sub_6C9627();
	map_invalidate_selection_rect();
	RCT2_GLOBAL(RCT2_ADDRESS_MAP_SELECTION_FLAGS, uint16) &= ~(1 | 4);

	int mapX, mapY, direction;
	ride_get_entrance_or_exit_position_from_screen_position(screenX, screenY, &mapX, &mapY, &direction);
	if (RCT2_GLOBAL(0x00F44194, uint8) == 255)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = (RCT2_GLOBAL(0x00F44191, uint8) == 0) ?
		STR_CANT_BUILD_MOVE_ENTRANCE_FOR_THIS_RIDE_ATTRACTION :
		STR_CANT_BUILD_MOVE_EXIT_FOR_THIS_RIDE_ATTRACTION;

	money32 cost = game_do_command(
		RCT2_GLOBAL(0x00F44188, uint16),
		(GAME_COMMAND_FLAG_APPLY) | ((RCT2_GLOBAL(0x00F44194, uint8) ^ 2) << 8),
		RCT2_GLOBAL(0x00F4418A, uint16),
		RCT2_GLOBAL(0x00F44192, uint8) | (RCT2_GLOBAL(0x00F44191, uint8) << 8),
		GAME_COMMAND_PLACE_RIDE_ENTRANCE_OR_EXIT,
		RCT2_GLOBAL(0x00F44193, uint8),
		0
	);
	if (cost == MONEY32_UNDEFINED) {
		return;
	}

	sound_play_panned(
		SOUND_PLACE_ITEM,
		0x8001,
		RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_X, uint16),
		RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_Y, uint16),
		RCT2_GLOBAL(RCT2_ADDRESS_COMMAND_MAP_Z, uint16)
	);

	rct_ride *ride = GET_RIDE(RCT2_GLOBAL(0x00F44192, uint8));
	if (ride_are_all_possible_entrances_and_exits_built(ride)) {
		tool_cancel();
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15)) {
			window_close_by_class(WC_RIDE_CONSTRUCTION);
		}
	} else {
		RCT2_GLOBAL(0x00F44191, uint8) ^= 1;
		window_invalidate_by_class(77);
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WIDGETINDEX, uint16) = (RCT2_GLOBAL(0x00F44191, uint8) == 0) ?
			WIDX_ENTRANCE : WIDX_EXIT;
	}
}
