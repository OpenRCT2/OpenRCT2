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
#include "../cheats.h"
#include "../config/Config.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/date.h"
#include "../localisation/localisation.h"
#include "../network/network.h"
#include "../object/ObjectManager.h"
#include "../object/ObjectRepository.h"
#include "../peep/staff.h"
#include "../rct1.h"
#include "../rct2.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../ride/track_data.h"
#include "../ride/track_design.h"
#include "../sprites.h"
#include "../windows/error.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "dropdown.h"


enum {
	WINDOW_RIDE_PAGE_MAIN,
	WINDOW_RIDE_PAGE_VEHICLE,
	WINDOW_RIDE_PAGE_OPERATING,
	WINDOW_RIDE_PAGE_MAINTENANCE,
	WINDOW_RIDE_PAGE_COLOUR,
	WINDOW_RIDE_PAGE_MUSIC,
	WINDOW_RIDE_PAGE_MEASUREMENTS,
	WINDOW_RIDE_PAGE_GRAPHS,
	WINDOW_RIDE_PAGE_INCOME,
	WINDOW_RIDE_PAGE_CUSTOMER,
	WINDOW_RIDE_PAGE_COUNT
};

#pragma region Widgets

enum {
	WIDX_BACKGROUND,
	WIDX_TITLE,
	WIDX_CLOSE,
	WIDX_PAGE_BACKGROUND,
	WIDX_TAB_1,
	WIDX_TAB_2,
	WIDX_TAB_3,
	WIDX_TAB_4,
	WIDX_TAB_5,
	WIDX_TAB_6,
	WIDX_TAB_7,
	WIDX_TAB_8,
	WIDX_TAB_9,
	WIDX_TAB_10,

	WIDX_VIEWPORT = 14,
	WIDX_VIEW,
	WIDX_VIEW_DROPDOWN,
	WIDX_STATUS,
	WIDX_OPEN,
	WIDX_CONSTRUCTION,
	WIDX_RENAME,
	WIDX_LOCATE,
	WIDX_DEMOLISH,
	WIDX_CLOSE_LIGHT,
	WIDX_TEST_LIGHT,
	WIDX_OPEN_LIGHT,
	WIDX_RIDE_TYPE,
	WIDX_RIDE_TYPE_INCREASE,
	WIDX_RIDE_TYPE_DECREASE,
	WIDX_RIDE_TYPE_APPLY,

	WIDX_VEHICLE_TYPE = 14,
	WIDX_VEHICLE_TYPE_DROPDOWN,
	WIDX_VEHICLE_TRAINS_PREVIEW,
	WIDX_VEHICLE_TRAINS,
	WIDX_VEHICLE_TRAINS_INCREASE,
	WIDX_VEHICLE_TRAINS_DECREASE,
	WIDX_VEHICLE_CARS_PER_TRAIN,
	WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE,
	WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE,

	WIDX_MODE_TWEAK = 14,
	WIDX_MODE_TWEAK_INCREASE,
	WIDX_MODE_TWEAK_DECREASE,
	WIDX_LIFT_HILL_SPEED,
	WIDX_LIFT_HILL_SPEED_INCREASE,
	WIDX_LIFT_HILL_SPEED_DECREASE,
	WIDX_LOAD_CHECKBOX,
	WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX,
	WIDX_MINIMUM_LENGTH_CHECKBOX,
	WIDX_MINIMUM_LENGTH,
	WIDX_MINIMUM_LENGTH_INCREASE,
	WIDX_MINIMUM_LENGTH_DECREASE,
	WIDX_MAXIMUM_LENGTH_CHECKBOX,
	WIDX_MAXIMUM_LENGTH,
	WIDX_MAXIMUM_LENGTH_INCREASE,
	WIDX_MAXIMUM_LENGTH_DECREASE,
	WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX,
	WIDX_MODE_TWEAK_LABEL,
	WIDX_LIFT_HILL_SPEED_LABEL,
	WIDX_MODE,
	WIDX_MODE_DROPDOWN,
	WIDX_LOAD,
	WIDX_LOAD_DROPDOWN,
	WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL,
	WIDX_OPERATE_NUMBER_OF_CIRCUITS,
	WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE,
	WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE,

	WIDX_INSPECTION_INTERVAL = 14,
	WIDX_INSPECTION_INTERVAL_DROPDOWN,
	WIDX_LOCATE_MECHANIC,
	WIDX_FORCE_BREAKDOWN,

	WIDX_TRACK_PREVIEW = 14,
	WIDX_TRACK_COLOUR_SCHEME,
	WIDX_TRACK_COLOUR_SCHEME_DROPDOWN,
	WIDX_TRACK_MAIN_COLOUR,
	WIDX_TRACK_ADDITIONAL_COLOUR,
	WIDX_TRACK_SUPPORT_COLOUR,
	WIDX_MAZE_STYLE,
	WIDX_MAZE_STYLE_DROPDOWN,
	WIDX_PAINT_INDIVIDUAL_AREA,
	WIDX_ENTRANCE_PREVIEW,
	WIDX_ENTRANCE_STYLE,
	WIDX_ENTRANCE_STYLE_DROPDOWN,
	WIDX_VEHICLE_PREVIEW,
	WIDX_VEHICLE_COLOUR_SCHEME,
	WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN,
	WIDX_VEHICLE_COLOUR_INDEX,
	WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN,
	WIDX_VEHICLE_MAIN_COLOUR,
	WIDX_VEHICLE_ADDITIONAL_COLOUR_1,
	WIDX_VEHICLE_ADDITIONAL_COLOUR_2,

	WIDX_PLAY_MUSIC = 14,
	WIDX_MUSIC,
	WIDX_MUSIC_DROPDOWN,

	WIDX_SAVE_TRACK_DESIGN = 14,
	WIDX_SELECT_NEARBY_SCENERY,
	WIDX_RESET_SELECTION,
	WIDX_SAVE_DESIGN,
	WIDX_CANCEL_DESIGN,

	WIDX_GRAPH = 14,
	WIDX_GRAPH_VELOCITY,
	WIDX_GRAPH_ALTITUDE,
	WIDX_GRAPH_VERTICAL,
	WIDX_GRAPH_LATERAL,

	WIDX_PRIMARY_PRICE_LABEL = 14,
	WIDX_PRIMARY_PRICE,
	WIDX_PRIMARY_PRICE_INCREASE,
	WIDX_PRIMARY_PRICE_DECREASE,
	WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK,
	WIDX_SECONDARY_PRICE_LABEL,
	WIDX_SECONDARY_PRICE,
	WIDX_SECONDARY_PRICE_INCREASE,
	WIDX_SECONDARY_PRICE_DECREASE,
	WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK,

	WIDX_SHOW_GUESTS_THOUGHTS = 14,
	WIDX_SHOW_GUESTS_ON_RIDE,
	WIDX_SHOW_GUESTS_QUEUING
};

#define RCT1_LIGHT_OFFSET 4

#define MAIN_RIDE_WIDGETS \
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0xFFFFFFFF,						STR_NONE									}, \
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		STR_RIDE_WINDOW_TITLE,			STR_WINDOW_TITLE_TIP						}, \
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						}, \
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0xFFFFFFFF,						STR_NONE									}, \
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x20000000 | SPR_TAB,			STR_VIEW_OF_RIDE_ATTRACTION_TIP				}, \
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x20000000 | SPR_TAB,			STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			}, \
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x20000000 | SPR_TAB,			STR_OPERATING_OPTIONS_TIP					}, \
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x20000000 | SPR_TAB,			STR_MAINTENANCE_OPTIONS_TIP					}, \
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x20000000 | SPR_TAB,			STR_COLOUR_SCHEME_OPTIONS_TIP				}, \
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x20000000 | SPR_TAB,			STR_SOUND_AND_MUSIC_OPTIONS_TIP				}, \
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x20000000 | SPR_TAB,			STR_MEASUREMENTS_AND_TEST_DATA_TIP			}, \
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x20000000 | SPR_TAB,			STR_GRAPHS_TIP								}, \
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x20000000 | SPR_TAB,			STR_INCOME_AND_COSTS_TIP					}, \
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x20000000 | SPR_TAB,			STR_CUSTOMER_INFORMATION_TIP				}

// 0x009ADC34
static rct_widget window_ride_main_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_VIEWPORT,			1,	3,		290,	60,		166,	0xFFFFFFFE,						STR_NONE									},
	{ WWT_DROPDOWN,			1,	35,		256,	46,		57,		0xFFFFFFFF,						STR_VIEW_SELECTION							},
	{ WWT_DROPDOWN_BUTTON,	1,	245,	255,	47,		56,		STR_DROPDOWN_GLYPH,				STR_VIEW_SELECTION							},
	{ WWT_12,				1,	3,		290,	167,	177,	0xFFFFFFFF,						STR_NONE									},
	{ WWT_FLATBTN,			1,	291,	314,	46,		69,		0xFFFFFFFF,						STR_OPEN_CLOSE_OR_TEST_RIDE					},
	{ WWT_FLATBTN,			1,	291,	314,	70,		93,		SPR_CONSTRUCTION,				STR_CONSTRUCTION							},
	{ WWT_FLATBTN,			1,	291,	314,	94,		117,	SPR_RENAME,						STR_NAME_RIDE_TIP							},
	{ WWT_FLATBTN,			1,	291,	314,	118,	141,	SPR_LOCATE,						STR_LOCATE_SUBJECT_TIP						},
	{ WWT_FLATBTN,			1,	291,	314,	142,	165,	SPR_DEMOLISH,					STR_DEMOLISH_RIDE_TIP						},
	{ WWT_IMGBTN,			1,	296,	309,	48,		61,		SPR_G2_RCT1_CLOSE_BUTTON_0,		STR_CLOSE_RIDE_TIP							},
	{ WWT_IMGBTN,			1,	296,	309,	62,		75,		SPR_G2_RCT1_TEST_BUTTON_0,		STR_TEST_RIDE_TIP							},
	{ WWT_IMGBTN,			1,	296,	309,	76,		89,		SPR_G2_RCT1_OPEN_BUTTON_0,		STR_OPEN_RIDE_TIP							},

	// Ride type spinner + apply button
	{ WWT_SPINNER,			1,	3,		253,	180,	191,	STR_ARG_6_STRINGID,				STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	242,	252,	181,	185,	STR_NUMERIC_UP,					STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	242,	252,	186,	190,	STR_NUMERIC_DOWN,				STR_NONE									},
	{ WWT_CLOSEBOX,			1,	260,	307,	180,	191,	STR_APPLY,						STR_NONE									},
	{ WIDGETS_END },
};

// 0x009ADDA8
static rct_widget window_ride_vehicle_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_DROPDOWN,			1,	7,		308,	50,		61,		0xFFFFFFFF,									STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	51,		60,		STR_DROPDOWN_GLYPH,							STR_NONE										},
	{ WWT_SCROLL,			1,	7,		308,	141,	183,	0,											STR_NONE										},
	{ WWT_SPINNER,			1,	7,		151,	190,	201,	STR_RIDE_VEHICLE_COUNT,						STR_MAX_VEHICLES_TIP							},
	{ WWT_DROPDOWN_BUTTON,	1,	140,	150,	191,	195,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	140,	150,	196,	200,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_SPINNER,			1,	164,	308,	190,	201,	STR_1_CAR_PER_TRAIN,						STR_MAX_CARS_PER_TRAIN_TIP						},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	191,	195,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	196,	200,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WIDGETS_END },
};

// 0x009ADEFC
static rct_widget window_ride_operating_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_SPINNER,			1,	157,	308,	61,		72,		STR_ARG_18_STRINGID,						STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	62,		66,		STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	67,		71,		STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_SPINNER,			1,	157,	308,	75,		86,		STR_LIFT_HILL_CHAIN_SPEED_VALUE,			STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	76,		80,		STR_NUMERIC_UP,								STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	81,		85,		STR_NUMERIC_DOWN,							STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WWT_CHECKBOX,			1,	7,		86,		109,	120,	STR_WAIT_FOR,								STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP	},
	{ WWT_CHECKBOX,			1,	7,		308,	124,	135,	0xFFFFFFFF,									STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		156,	139,	150,	STR_MINIMUM_WAITING_TIME,					STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP			},
	{ WWT_SPINNER,			1,	157,	308,	139,	150,	STR_ARG_10_STRINGID,						STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	140,	144,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	145,	149,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		156,	154,	165,	STR_MAXIMUM_WAITING_TIME,					STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP			},
	{ WWT_SPINNER,			1,	157,	308,	154,	165,	STR_ARG_14_STRINGID,						STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	155,	159,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	160,	164,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		308,	169,	180,	STR_SYNCHRONISE_WITH_ADJACENT_STATIONS,		STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP		},
	{ WWT_24,				1,	7,		149,	61,		72,		0xFFFFFFFF,									STR_NONE										},
	{ WWT_24,				1,	7,		149,	75,		86,		STR_LIFT_HILL_CHAIN_SPEED,					STR_NONE										},
	{ WWT_DROPDOWN,			1,	7,		308,	47,		58,		0xFFFFFFFF,									STR_SELECT_OPERATING_MODE						},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	48,		57,		STR_DROPDOWN_GLYPH,							STR_SELECT_OPERATING_MODE						},
	{ WWT_DROPDOWN,			1,	87,		308,	109,	120,	0xFFFFFFFF,									STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	110,	119,	STR_DROPDOWN_GLYPH,							STR_NONE										},
	{ WWT_24,				1,	7,		149,	89,		100,	STR_NUMBER_OF_CIRCUITS,						STR_NUMBER_OF_CIRCUITS_TIP						},
	{ WWT_SPINNER,			1,	157,	308,	89,		100,	STR_NUMBER_OF_CIRCUITS_VALUE,				STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	90,		94,		STR_NUMERIC_UP,								STR_SELECT_NUMBER_OF_CIRCUITS_TIP				},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	95,		99,		STR_NUMERIC_DOWN,							STR_SELECT_NUMBER_OF_CIRCUITS_TIP				},
	{ WIDGETS_END },
};

// 0x009AE190
static rct_widget window_ride_maintenance_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_DROPDOWN,			1,	107,	308,	71,		82,		0,								STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE		},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	72,		81,		STR_DROPDOWN_GLYPH,				STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE		},
	{ WWT_FLATBTN,			1,	289,	312,	108,	131,	0xFFFFFFFF,						STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP					},
	{ WWT_FLATBTN,			1,	265,	288,	108,	131,	SPR_NO_ENTRY,					STR_DEBUG_FORCE_BREAKDOWN_TIP								},
	{ WIDGETS_END },
};

// 0x009AE2A4
static rct_widget window_ride_colour_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_SPINNER,			1,	3,		70,		47,		93,		0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN,			1,	74,		312,	49,		60,		STR_ARG_14_STRINGID,			STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	50,		59,		STR_DROPDOWN_GLYPH,				STR_COLOUR_SCHEME_TO_CHANGE_TIP								},
	{ WWT_COLOURBTN,		1,	79,		90,		74,		85,		0xFFFFFFFF,						STR_SELECT_MAIN_COLOUR_TIP									},
	{ WWT_COLOURBTN,		1,	99,		110,	74,		85,		0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_1_TIP							},
	{ WWT_COLOURBTN,		1,	119,	130,	74,		85,		0xFFFFFFFF,						STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP						},
	{ WWT_DROPDOWN,			1,	74,		312,	49,		60,		0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	50,		59,		STR_DROPDOWN_GLYPH,				STR_NONE													},
	{ WWT_FLATBTN,			1,	289,	312,	68,		91,		SPR_PAINTBRUSH,					STR_PAINT_INDIVIDUAL_AREA_TIP								},
	{ WWT_SPINNER,			1,	245,	312,	101,	147,	0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN,			1,	3,		241,	103,	114,	0,								STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	230,	240,	104,	113,	STR_DROPDOWN_GLYPH,				STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP				},
	{ WWT_SCROLL,			1,	3,		70,		157,	203,	0,								STR_NONE													},
	{ WWT_DROPDOWN,			1,	74,		312,	157,	168,	STR_ARG_6_STRINGID,				STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	158,	167,	STR_DROPDOWN_GLYPH,				STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP						},
	{ WWT_DROPDOWN,			1,	74,		312,	173,	184,	0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	174,	183,	STR_DROPDOWN_GLYPH,				STR_SELECT_VEHICLE_TO_MODIFY_TIP							},
	{ WWT_COLOURBTN,		1,	79,		90,		190,	201,	0xFFFFFFFF,						STR_SELECT_MAIN_COLOUR_TIP									},
	{ WWT_COLOURBTN,		1,	99,		110,	190,	201,	0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_1_TIP							},
	{ WWT_COLOURBTN,		1,	119,	130,	190,	201,	0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_2_TIP							},
	{ WIDGETS_END },
};

// 0x009AE4C8
static rct_widget window_ride_music_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_CHECKBOX,			1,	7,		308,	47,		58,		STR_PLAY_MUSIC,					STR_SELECT_MUSIC_TIP						},
	{ WWT_DROPDOWN,			1,	7,		308,	62,		73,		0,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	63,		72,		STR_DROPDOWN_GLYPH,				STR_SELECT_MUSIC_STYLE_TIP					},
	{ WIDGETS_END },
};

// 0x009AE5DC
static rct_widget window_ride_measurements_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_FLATBTN,			1,	288,	311,	164,	187,	SPR_FLOPPY,						STR_SAVE_TRACK_DESIGN						},
	{ WWT_DROPDOWN_BUTTON,	1,	4,		157,	128,	139,	STR_SELECT_NEARBY_SCENERY,		STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	158,	311,	128,	139,	STR_RESET_SELECTION,			STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	4,		157,	178,	189,	STR_DESIGN_SAVE,				STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	158,	311,	178,	189,	STR_DESIGN_CANCEL,				STR_NONE									},
	{ WIDGETS_END },
};

// 0x009AE710
static rct_widget window_ride_graphs_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_SCROLL,			1,	3,		308,	46,		157,	SCROLL_HORIZONTAL,				STR_LOGGING_DATA_FROM_TIP									},
	{ WWT_DROPDOWN_BUTTON,	1,	3,		75,		163,	176,	STR_RIDE_STATS_VELOCITY,		STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP					},
	{ WWT_DROPDOWN_BUTTON,	1,	76,		148,	163,	176,	STR_RIDE_STATS_ALTITUDE,		STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP					},
	{ WWT_DROPDOWN_BUTTON,	1,	149,	221,	163,	176,	STR_RIDE_STATS_VERT_G,			STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	222,	294,	163,	176,	STR_RIDE_STATS_LAT_G,			STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP 	},
	{ WIDGETS_END },
};

// 0x009AE844
static rct_widget window_ride_income_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_24,				1,	5,		144,	50,		61,		0xFFFFFFFF,									STR_NONE													},
	{ WWT_SPINNER,			1,	147,	308,	50,		61,		STR_ARG_6_CURRENCY2DP,						STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	51,		55,		STR_NUMERIC_UP,								STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	56,		60,		STR_NUMERIC_DOWN,							STR_NONE													},
	{ WWT_CHECKBOX,			1,	5,		310,	61,		72,		STR_SAME_PRICE_THROUGHOUT_PARK,				STR_SAME_PRICE_THROUGHOUT_PARK_TIP							},
	{ WWT_24,				1,	5,		144,	89,		100,	0xFFFFFFFF,									STR_NONE													},
	{ WWT_SPINNER,			1,	147,	308,	89,		100,	STR_RIDE_SECONDARY_PRICE_VALUE,				STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	90,		94,		STR_NUMERIC_UP,								STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	95,		99,		STR_NUMERIC_DOWN,							STR_NONE													},
	{ WWT_CHECKBOX,			1,	5,		310,	100,	111,	STR_SAME_PRICE_THROUGHOUT_PARK,				STR_SAME_PRICE_THROUGHOUT_PARK_TIP							},
	{ WIDGETS_END },
};

// 0x009AE9C8
static rct_widget window_ride_customer_widgets[] = {
	MAIN_RIDE_WIDGETS,
	{ WWT_FLATBTN,			1,	289,	312,	54,		77,		SPR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION,	STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP	},
	{ WWT_FLATBTN,			1,	289,	312,	78,		101,	SPR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION,				STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP				},
	{ WWT_FLATBTN,			1,	289,	312,	102,	125,	SPR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION,		STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP	},
	{ WIDGETS_END },
};

static rct_widget *window_ride_page_widgets[] = {
	window_ride_main_widgets,
	window_ride_vehicle_widgets,
	window_ride_operating_widgets,
	window_ride_maintenance_widgets,
	window_ride_colour_widgets,
	window_ride_music_widgets,
	window_ride_measurements_widgets,
	window_ride_graphs_widgets,
	window_ride_income_widgets,
	window_ride_customer_widgets
};

#define MAIN_RIDE_ENABLED_WIDGETS \
	(1ULL << WIDX_CLOSE) | \
	(1ULL << WIDX_TAB_1) | \
	(1ULL << WIDX_TAB_2) | \
	(1ULL << WIDX_TAB_3) | \
	(1ULL << WIDX_TAB_4) | \
	(1ULL << WIDX_TAB_5) | \
	(1ULL << WIDX_TAB_6) | \
	(1ULL << WIDX_TAB_7) | \
	(1ULL << WIDX_TAB_8) | \
	(1ULL << WIDX_TAB_9) | \
	(1ULL << WIDX_TAB_10)

const uint64 window_ride_page_enabled_widgets[] = {
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_VIEW) |
		(1ULL << WIDX_VIEW_DROPDOWN) |
		(1ULL << WIDX_OPEN) |
		(1ULL << WIDX_CONSTRUCTION) |
		(1ULL << WIDX_RENAME) |
		(1ULL << WIDX_LOCATE) |
		(1ULL << WIDX_DEMOLISH) |
		(1ULL << WIDX_CLOSE_LIGHT) |
		(1ULL << WIDX_TEST_LIGHT) |
		(1ULL << WIDX_OPEN_LIGHT) |
		(1ULL << WIDX_RIDE_TYPE) |
		(1ULL << WIDX_RIDE_TYPE_INCREASE) |
		(1ULL << WIDX_RIDE_TYPE_DECREASE) |
		(1ULL << WIDX_RIDE_TYPE_APPLY),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_VEHICLE_TYPE) |
		(1ULL << WIDX_VEHICLE_TYPE_DROPDOWN) |
		(1ULL << WIDX_VEHICLE_TRAINS) |
		(1ULL << WIDX_VEHICLE_TRAINS_INCREASE) |
		(1ULL << WIDX_VEHICLE_TRAINS_DECREASE) |
		(1ULL << WIDX_VEHICLE_CARS_PER_TRAIN) |
		(1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE) |
		(1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_MODE_TWEAK_INCREASE) |
		(1ULL << WIDX_MODE_TWEAK_DECREASE) |
		(1ULL << WIDX_LIFT_HILL_SPEED_INCREASE) |
		(1ULL << WIDX_LIFT_HILL_SPEED_DECREASE) |
		(1ULL << WIDX_LOAD_CHECKBOX) |
		(1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX) |
		(1ULL << WIDX_MINIMUM_LENGTH_CHECKBOX) |
		(1ULL << WIDX_MINIMUM_LENGTH_INCREASE) |
		(1ULL << WIDX_MINIMUM_LENGTH_DECREASE) |
		(1ULL << WIDX_MAXIMUM_LENGTH_CHECKBOX) |
		(1ULL << WIDX_MAXIMUM_LENGTH_INCREASE) |
		(1ULL << WIDX_MAXIMUM_LENGTH_DECREASE) |
		(1ULL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX) |
		(1ULL << WIDX_MODE) |
		(1ULL << WIDX_MODE_DROPDOWN) |
		(1ULL << WIDX_LOAD) |
		(1ULL << WIDX_LOAD_DROPDOWN) |
		(1ULL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE) |
		(1ULL << WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_INSPECTION_INTERVAL) |
		(1ULL << WIDX_INSPECTION_INTERVAL_DROPDOWN) |
		(1ULL << WIDX_LOCATE_MECHANIC) |
		(1ULL << WIDX_FORCE_BREAKDOWN),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_TRACK_COLOUR_SCHEME_DROPDOWN) |
		(1ULL << WIDX_TRACK_MAIN_COLOUR) |
		(1ULL << WIDX_TRACK_ADDITIONAL_COLOUR) |
		(1ULL << WIDX_TRACK_SUPPORT_COLOUR) |
		(1ULL << WIDX_MAZE_STYLE) |
		(1ULL << WIDX_MAZE_STYLE_DROPDOWN) |
		(1ULL << WIDX_PAINT_INDIVIDUAL_AREA) |
		(1ULL << WIDX_ENTRANCE_STYLE) |
		(1ULL << WIDX_ENTRANCE_STYLE_DROPDOWN) |
		(1ULL << WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN) |
		(1ULL << WIDX_VEHICLE_COLOUR_INDEX) |
		(1ULL << WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN) |
		(1ULL << WIDX_VEHICLE_MAIN_COLOUR) |
		(1ULL << WIDX_VEHICLE_ADDITIONAL_COLOUR_1) |
		(1ULL << WIDX_VEHICLE_ADDITIONAL_COLOUR_2),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_PLAY_MUSIC) |
		(1ULL << WIDX_MUSIC) |
		(1ULL << WIDX_MUSIC_DROPDOWN),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_SAVE_TRACK_DESIGN) |
		(1ULL << WIDX_SELECT_NEARBY_SCENERY) |
		(1ULL << WIDX_RESET_SELECTION) |
		(1ULL << WIDX_SAVE_DESIGN) |
		(1ULL << WIDX_CANCEL_DESIGN),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_GRAPH_VELOCITY) |
		(1ULL << WIDX_GRAPH_ALTITUDE) |
		(1ULL << WIDX_GRAPH_VERTICAL) |
		(1ULL << WIDX_GRAPH_LATERAL),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_PRIMARY_PRICE_INCREASE) |
		(1ULL << WIDX_PRIMARY_PRICE_DECREASE) |
		(1ULL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK) |
		(1ULL << WIDX_SECONDARY_PRICE_INCREASE) |
		(1ULL << WIDX_SECONDARY_PRICE_DECREASE) |
		(1ULL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK),
	MAIN_RIDE_ENABLED_WIDGETS |
		(1ULL << WIDX_SHOW_GUESTS_THOUGHTS) |
		(1ULL << WIDX_SHOW_GUESTS_ON_RIDE) |
		(1ULL << WIDX_SHOW_GUESTS_QUEUING),
};

const uint64 window_ride_page_hold_down_widgets[] = {
	0,
	(1ULL << WIDX_VEHICLE_TRAINS_INCREASE) |
		(1ULL << WIDX_VEHICLE_TRAINS_DECREASE) |
		(1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE) |
		(1ULL << WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE),
	(1ULL << WIDX_MODE_TWEAK_INCREASE) |
		(1ULL << WIDX_MODE_TWEAK_DECREASE) |
		(1ULL << WIDX_LIFT_HILL_SPEED_INCREASE) |
		(1ULL << WIDX_LIFT_HILL_SPEED_DECREASE) |
		(1ULL << WIDX_MINIMUM_LENGTH_INCREASE) |
		(1ULL << WIDX_MINIMUM_LENGTH_DECREASE) |
		(1ULL << WIDX_MAXIMUM_LENGTH_INCREASE) |
		(1ULL << WIDX_MAXIMUM_LENGTH_DECREASE),
	0,
	0,
	0,
	0,
	0,
	(1ULL << WIDX_PRIMARY_PRICE_INCREASE) |
		(1ULL << WIDX_PRIMARY_PRICE_DECREASE) |
		(1ULL << WIDX_SECONDARY_PRICE_INCREASE) |
		(1ULL << WIDX_SECONDARY_PRICE_DECREASE),
	0
};

#pragma endregion

#pragma region Events

static void window_ride_init_viewport(rct_window *w);

static void window_ride_main_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_main_resize(rct_window *w);
static void window_ride_main_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_ride_main_unknown_14(rct_window *w);
static void window_ride_main_invalidate(rct_window *w);
static void window_ride_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_vehicle_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_vehicle_resize(rct_window *w);
static void window_ride_vehicle_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_vehicle_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_vehicle_update(rct_window *w);
static void window_ride_vehicle_invalidate(rct_window *w);
static void window_ride_vehicle_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_vehicle_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_ride_operating_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_operating_resize(rct_window *w);
static void window_ride_operating_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_operating_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_operating_update(rct_window *w);
static void window_ride_operating_invalidate(rct_window *w);
static void window_ride_operating_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_maintenance_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_maintenance_resize(rct_window *w);
static void window_ride_maintenance_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_maintenance_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_maintenance_update(rct_window *w);
static void window_ride_maintenance_invalidate(rct_window *w);
static void window_ride_maintenance_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_colour_close(rct_window *w);
static void window_ride_colour_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_colour_resize(rct_window *w);
static void window_ride_colour_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_colour_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_colour_update(rct_window *w);
static void window_ride_colour_tooldown(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_ride_colour_tooldrag(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_ride_colour_invalidate(rct_window *w);
static void window_ride_colour_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_colour_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_ride_music_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_music_resize(rct_window *w);
static void window_ride_music_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_music_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_music_update(rct_window *w);
static void window_ride_music_invalidate(rct_window *w);
static void window_ride_music_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_measurements_close(rct_window *w);
static void window_ride_measurements_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_measurements_resize(rct_window *w);
static void window_ride_measurements_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_measurements_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex);
static void window_ride_measurements_update(rct_window *w);
static void window_ride_measurements_tooldown(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_ride_measurements_tooldrag(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y);
static void window_ride_measurements_toolabort(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_measurements_invalidate(rct_window *w);
static void window_ride_measurements_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_graphs_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_graphs_resize(rct_window *w);
static void window_ride_graphs_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_graphs_update(rct_window *w);
static void window_ride_graphs_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height);
static void window_ride_graphs_15(rct_window *w, sint32 scrollIndex, sint32 scrollAreaType);
static void window_ride_graphs_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_ride_graphs_invalidate(rct_window *w);
static void window_ride_graphs_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_graphs_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex);

static void window_ride_income_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_income_resize(rct_window *w);
static void window_ride_income_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_income_update(rct_window *w);
static void window_ride_income_invalidate(rct_window *w);
static void window_ride_income_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_customer_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_customer_resize(rct_window *w);
static void window_ride_customer_update(rct_window *w);
static void window_ride_customer_invalidate(rct_window *w);
static void window_ride_customer_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_set_colours();
static void window_ride_set_page(rct_window *w, sint32 page);

// 0x0098DFD4
static rct_window_event_list window_ride_main_events = {
	NULL,
	window_ride_main_mouseup,
	window_ride_main_resize,
	window_ride_main_mousedown,
	window_ride_main_dropdown,
	NULL,
	window_ride_main_update,
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
	window_ride_main_textinput,
	window_ride_main_unknown_14,
	NULL,
	NULL,
	NULL,
	NULL,
	window_ride_main_invalidate,
	window_ride_main_paint,
	NULL
};

// 0x0098E204
static rct_window_event_list window_ride_vehicle_events = {
	NULL,
	window_ride_vehicle_mouseup,
	window_ride_vehicle_resize,
	window_ride_vehicle_mousedown,
	window_ride_vehicle_dropdown,
	NULL,
	window_ride_vehicle_update,
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
	NULL,
	window_ride_vehicle_invalidate,
	window_ride_vehicle_paint,
	window_ride_vehicle_scrollpaint
};

// 0x0098E0B4
static rct_window_event_list window_ride_operating_events = {
	NULL,
	window_ride_operating_mouseup,
	window_ride_operating_resize,
	window_ride_operating_mousedown,
	window_ride_operating_dropdown,
	NULL,
	window_ride_operating_update,
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
	NULL,
	window_ride_operating_invalidate,
	window_ride_operating_paint,
	NULL
};

// 0x0098E124
static rct_window_event_list window_ride_maintenance_events = {
	NULL,
	window_ride_maintenance_mouseup,
	window_ride_maintenance_resize,
	window_ride_maintenance_mousedown,
	window_ride_maintenance_dropdown,
	NULL,
	window_ride_maintenance_update,
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
	NULL,
	window_ride_maintenance_invalidate,
	window_ride_maintenance_paint,
	NULL
};

// 0x0098E044
static rct_window_event_list window_ride_colour_events = {
	window_ride_colour_close,
	window_ride_colour_mouseup,
	window_ride_colour_resize,
	window_ride_colour_mousedown,
	window_ride_colour_dropdown,
	NULL,
	window_ride_colour_update,
	NULL,
	NULL,
	NULL,
	window_ride_colour_tooldown,
	window_ride_colour_tooldrag,
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
	window_ride_colour_invalidate,
	window_ride_colour_paint,
	window_ride_colour_scrollpaint
};

// 0x0098E194
static rct_window_event_list window_ride_music_events = {
	NULL,
	window_ride_music_mouseup,
	window_ride_music_resize,
	window_ride_music_mousedown,
	window_ride_music_dropdown,
	NULL,
	window_ride_music_update,
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
	NULL,
	window_ride_music_invalidate,
	window_ride_music_paint,
	NULL
};

// 0x0098DE14
static rct_window_event_list window_ride_measurements_events = {
	window_ride_measurements_close,
	window_ride_measurements_mouseup,
	window_ride_measurements_resize,
	window_ride_measurements_mousedown,
	window_ride_measurements_dropdown,
	NULL,
	window_ride_measurements_update,
	NULL,
	NULL,
	NULL,
	window_ride_measurements_tooldown,
	window_ride_measurements_tooldrag,
	NULL,
	window_ride_measurements_toolabort,
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
	window_ride_measurements_invalidate,
	window_ride_measurements_paint,
	NULL
};

// 0x0098DF64
static rct_window_event_list window_ride_graphs_events = {
	NULL,
	window_ride_graphs_mouseup,
	window_ride_graphs_resize,
	window_ride_graphs_mousedown,
	NULL,
	NULL,
	window_ride_graphs_update,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_ride_graphs_scrollgetheight,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	window_ride_graphs_15,
	window_ride_graphs_tooltip,
	NULL,
	NULL,
	window_ride_graphs_invalidate,
	window_ride_graphs_paint,
	window_ride_graphs_scrollpaint
};

// 0x0098DEF4
static rct_window_event_list window_ride_income_events = {
	NULL,
	window_ride_income_mouseup,
	window_ride_income_resize,
	window_ride_income_mousedown,
	NULL,
	NULL,
	window_ride_income_update,
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
	NULL,
	window_ride_income_invalidate,
	window_ride_income_paint,
	NULL
};

// 0x0098DE84
static rct_window_event_list window_ride_customer_events = {
	NULL,
	window_ride_customer_mouseup,
	window_ride_customer_resize,
	NULL,
	NULL,
	NULL,
	window_ride_customer_update,
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
	NULL,
	window_ride_customer_invalidate,
	window_ride_customer_paint,
	NULL
};

static rct_window_event_list *window_ride_page_events[] = {
	&window_ride_main_events,
	&window_ride_vehicle_events,
	&window_ride_operating_events,
	&window_ride_maintenance_events,
	&window_ride_colour_events,
	&window_ride_music_events,
	&window_ride_measurements_events,
	&window_ride_graphs_events,
	&window_ride_income_events,
	&window_ride_customer_events
};

#pragma endregion

static uint8 _rideType;
static bool _collectTrackDesignScenery = false;
static sint32 _lastSceneryX = 0;
static sint32 _lastSceneryY = 0;

static void set_operating_setting(sint32 rideNumber, uint8 setting, uint8 value);

// Cached overall view for each ride
// (Re)calculated when the ride window is opened
typedef struct ride_overall_view_t {
	sint16 x, y, z;
	uint8 zoom;
} ride_overall_view;

ride_overall_view ride_overall_views[MAX_RIDES] = {0};

const sint32 window_ride_tab_animation_divisor[] = { 0, 0, 2, 2, 4, 2, 8, 8, 2, 0 };
const sint32 window_ride_tab_animation_frames[] = { 0, 0, 4, 16, 8, 16, 8, 8, 8, 0 };

static const rct_string_id RatingNames[] = {
	STR_RATING_LOW,
	STR_RATING_MEDIUM,
	STR_RATING_HIGH,
	STR_RATING_VERY_HIGH,
	STR_RATING_EXTREME,
	STR_RATING_ULTRA_EXTREME,
};

static const rct_string_id RideBreakdownReasonNames[] = {
	STR_RIDE_BREAKDOWN_SAFETY_CUT_OUT ,
	STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_CLOSED,
	STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_OPEN,
	STR_RIDE_BREAKDOWN_DOORS_STUCK_CLOSED,
	STR_RIDE_BREAKDOWN_DOORS_STUCK_OPEN,
	STR_RIDE_BREAKDOWN_VEHICLE_MALFUNCTION,
	STR_RIDE_BREAKDOWN_BRAKES_FAILURE,
	STR_RIDE_BREAKDOWN_CONTROL_FAILURE
};

static const rct_string_id ColourSchemeNames[] = {
	STR_MAIN_COLOUR_SCHEME,
	STR_ALTERNATIVE_COLOUR_SCHEME_1,
	STR_ALTERNATIVE_COLOUR_SCHEME_2,
	STR_ALTERNATIVE_COLOUR_SCHEME_3,
};

static const rct_string_id VehicleLoadNames[] = {
	STR_QUARTER_LOAD,
	STR_HALF_LOAD,
	STR_THREE_QUARTER_LOAD,
	STR_FULL_LOAD,
	STR_ANY_LOAD,
};

static const rct_string_id VehicleColourSchemeNames[] = {
	STR_ALL_VEHICLES_IN_SAME_COLOURS ,
	STR_DIFFERENT_COLOURS_PER ,
	STR_DIFFERENT_COLOURS_PER_VEHICLE ,
};

static const rct_string_id VehicleStatusNames[] = {
	STR_MOVING_TO_END_OF,			// VEHICLE_STATUS_MOVING_TO_END_OF_STATION
	STR_WAITING_FOR_PASSENGERS_AT,	// VEHICLE_STATUS_WAITING_FOR_PASSENGERS
	STR_WAITING_TO_DEPART,			// VEHICLE_STATUS_WAITING_TO_DEPART
	STR_DEPARTING,					// VEHICLE_STATUS_DEPARTING
	STR_TRAVELLING_AT_0,			// VEHICLE_STATUS_TRAVELLING
	STR_ARRIVING_AT,				// VEHICLE_STATUS_ARRIVING
	STR_UNLOADING_PASSENGERS_AT,	// VEHICLE_STATUS_UNLOADING_PASSENGERS
	STR_TRAVELLING_AT_1,			// VEHICLE_STATUS_TRAVELLING_BOAT
	STR_CRASHING,					// VEHICLE_STATUS_CRASHING
	STR_CRASHED_0,					// VEHICLE_STATUS_CRASHED
	STR_TRAVELLING_AT_2,			// VEHICLE_STATUS_TRAVELLING_DODGEMS
	STR_SWINGING,					// VEHICLE_STATUS_SWINGING
	STR_ROTATING_0,					// VEHICLE_STATUS_ROTATING
	STR_ROTATING_1,					// VEHICLE_STATUS_FERRIS_WHEEL_ROTATING
	STR_OPERATING_0,				// VEHICLE_STATUS_SIMULATOR_OPERATING
	STR_SHOWING_FILM,				// VEHICLE_STATUS_SHOWING_FILM
	STR_ROTATING_2,					// VEHICLE_STATUS_SPACE_RINGS_OPERATING
	STR_OPERATING_1,				// VEHICLE_STATUS_TOP_SPIN_OPERATING
	STR_OPERATING_2,				// VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING
	STR_DOING_CIRCUS_SHOW,			// VEHICLE_STATUS_DOING_CIRCUS_SHOW
	STR_OPERATING_3,				// VEHICLE_STATUS_CROOKED_HOUSE_OPERATING
	STR_WAITING_FOR_CABLE_LIFT,		// VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT
	STR_TRAVELLING_AT_3,			// VEHICLE_STATUS_TRAVELLING_CABLE_LIFT
	STR_STOPPING_0,					// VEHICLE_STATUS_STOPPING
	STR_WAITING_FOR_PASSENGERS,		// VEHICLE_STATUS_WAITING_FOR_PASSENGERS_17
	STR_WAITING_TO_START,			// VEHICLE_STATUS_WAITING_TO_START
	STR_STARTING,					// VEHICLE_STATUS_STARTING
	STR_OPERATING,					// VEHICLE_STATUS_OPERATING_1A
	STR_STOPPING_1,					// VEHICLE_STATUS_STOPPING_1B
	STR_UNLOADING_PASSENGERS,		// VEHICLE_STATUS_UNLOADING_PASSENGERS_1C
	STR_STOPPED_BY_BLOCK_BRAKES,	// VEHICLE_STATUS_STOPPED_BY_BLOCK_BRAKES
};

static const rct_string_id SingleSessionVehicleStatusNames[] = {
	STR_STOPPING_0,					// VEHICLE_STATUS_MOVING_TO_END_OF_STATION
	STR_WAITING_FOR_PASSENGERS,		// VEHICLE_STATUS_WAITING_FOR_PASSENGERS
	STR_WAITING_TO_START,			// VEHICLE_STATUS_WAITING_TO_DEPART
	STR_STARTING,					// VEHICLE_STATUS_DEPARTING
	STR_OPERATING,					// VEHICLE_STATUS_TRAVELLING
	STR_STOPPING_1,					// VEHICLE_STATUS_ARRIVING
	STR_UNLOADING_PASSENGERS,		// VEHICLE_STATUS_UNLOADING_PASSENGERS
};

static const rct_string_id MusicStyleNames[] = {
	STR_MUSIC_STYLE_DODGEMS_BEAT,
	STR_MUSIC_STYLE_FAIRGROUND_ORGAN,
	STR_MUSIC_STYLE_ROMAN_FANFARE,
	STR_MUSIC_STYLE_ORIENTAL,
	STR_MUSIC_STYLE_MARTIAN,
	STR_MUSIC_STYLE_JUNGLE_DRUMS,
	STR_MUSIC_STYLE_EGYPTIAN,
	STR_MUSIC_STYLE_TOYLAND,
	STR_MUSIC_STYLE_CIRCUS_SHOW,
	STR_MUSIC_STYLE_SPACE,
	STR_MUSIC_STYLE_HORROR,
	STR_MUSIC_STYLE_TECHNO,
	STR_MUSIC_STYLE_GENTLE,
	STR_MUSIC_STYLE_SUMMER,
	STR_MUSIC_STYLE_WATER,
	STR_MUSIC_STYLE_WILD_WEST,
	STR_MUSIC_STYLE_JURASSIC,
	STR_MUSIC_STYLE_ROCK,
	STR_MUSIC_STYLE_RAGTIME,
	STR_MUSIC_STYLE_FANTASY,
	STR_MUSIC_STYLE_ROCK_STYLE_2,
	STR_MUSIC_STYLE_ICE,
	STR_MUSIC_STYLE_SNOW,
	STR_MUSIC_STYLE_CUSTOM_MUSIC_1,
	STR_MUSIC_STYLE_CUSTOM_MUSIC_2,
	STR_MUSIC_STYLE_MEDIEVAL,
	STR_MUSIC_STYLE_URBAN,
	STR_MUSIC_STYLE_ORGAN,
	STR_MUSIC_STYLE_MECHANICAL,
	STR_MUSIC_STYLE_MODERN,
	STR_MUSIC_STYLE_PIRATES,
	STR_MUSIC_STYLE_ROCK_STYLE_3,
	STR_MUSIC_STYLE_CANDY_STYLE,
};

typedef struct window_ride_maze_design_option {
	rct_string_id text;
	uint32 sprite;
} window_ride_maze_design_option;

static const window_ride_maze_design_option MazeOptions[] = {
	{ STR_RIDE_DESIGN_MAZE_BRICK_WALLS,		SPR_RIDE_DESIGN_PREVIEW_MAZE_BRICK_WALLS },
	{ STR_RIDE_DESIGN_MAZE_HEDGES,			SPR_RIDE_DESIGN_PREVIEW_MAZE_HEDGES },
	{ STR_RIDE_DESIGN_MAZE_ICE_BLOCKS,		SPR_RIDE_DESIGN_PREVIEW_MAZE_ICE_BLOCKS },
	{ STR_RIDE_DESIGN_MAZE_WOODEN_FENCES,	SPR_RIDE_DESIGN_PREVIEW_MAZE_WOODEN_FENCES },
};

typedef struct window_ride_colour_preview
{
	uint32 track;
	uint32 supports;
} window_ride_colour_preview;

static const window_ride_colour_preview TrackColourPreviews[] = {
	{ SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_INVERTED_ROLLER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_INVERTED_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_TRACK,				SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MONORAIL_TRACK,						SPR_RIDE_DESIGN_PREVIEW_MONORAIL_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_BOAT_RIDE_TRACK,						SPR_RIDE_DESIGN_PREVIEW_BOAT_RIDE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_TRACK,				SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_TRACK,					SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK,						SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_TRACK,				SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_TRACK,				SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_TRACK,				SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_LOOPING_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_LOOPING_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_TRACK,					SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINE_TRAIN_COASTER_TRACK,				SPR_RIDE_DESIGN_PREVIEW_MINE_TRAIN_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_TRACK,						SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_SUPPORTS},
	{ 0, 0 }, // MAZE
	{ SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK,					0},
	{ SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK,						SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_TRACK,						SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_TRACK,					SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_DODGEMS_TRACK,						SPR_RIDE_DESIGN_PREVIEW_DODGEMS_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_PIRATE_SHIP_TRACK,					SPR_RIDE_DESIGN_PREVIEW_PIRATE_SHIP_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_TRACK,			SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_SUPPORTS},
	{ 0, 0 }, // FOOD_STALL
	{ 0, 0 }, // 1D
	{ 0, 0 }, // DRINK_STALL
	{ 0, 0 }, // 1F
	{ 0, 0 }, // SHOP
	{ 0, 0 }, // MERRY_GO_ROUND
	{ 0, 0 }, // 22
	{ 0, 0 }, // INFORMATION_KIOSK
	{ 0, 0 }, // TOILETS
	{ SPR_RIDE_DESIGN_PREVIEW_FERRIS_WHEEL_TRACK,					0},
	{ 0, 0 }, // MOTION_SIMULATOR
	{ 0, 0 }, // 3D_CINEMA
	{ SPR_RIDE_DESIGN_PREVIEW_TOP_SPIN_TRACK,						0},
	{ 0, 0 }, // SPACE_RINGS
	{ SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_LIFT_TRACK,							0},
	{ SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_TRACK,	SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_SUPPORTS},
	{ 0, 0 }, // CASH_MACHINE
	{ 0, 0 }, // TWIST_TRACK
	{ 0, 0 }, // HAUNTED_HOUSE
	{ 0, 0 }, // FIRST_AID
	{ 0, 0 }, // CIRCUS_SHOW
	{ SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_TRACK,					SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_TRACK,	SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_TRACK,						SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_TRACK,	SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_SUPPORTS},
	{ 0, 0 }, // 38
	{ SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS},
	{ 0,	 0 }, // 3A
	{ SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_TRACK,					SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_TRACK,					SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINI_HELICOPTERS_TRACK,				SPR_RIDE_DESIGN_PREVIEW_MINI_HELICOPTERS_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_TRACK,				SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_SUPPORTS},
	{ 0, 0 }, // 40
	{ SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_TRACK,						SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_TRACK,					SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_TRACK,						SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_FLYING_SAUCERS_TRACK,					0},
	{ 0, 0 }, // CROOKED_HOUSE
	{ SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_TRACK,				SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_TRACK,					SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_TRACK,	SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_TRACK,					SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_TRACK,					SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_TRACK,					SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_SUPPORTS},
	{ 0, 0 }, // 50
	{ 0, 0 }, // ENTERPRISE
	{ 0, 0 }, // 52
	{ 0, 0 }, // 53
	{ 0, 0 }, // 54
	{ 0, 0 }, // 55
	{ SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_TRACK,		SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINI_ROLLER_COASTER_TRACK,			SPR_RIDE_DESIGN_PREVIEW_MINI_ROLLER_COASTER_SUPPORTS},
	{ SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_TRACK,						SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_SUPPORTS},
	{ 0, 0 }, // 59
	{ SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_TRACK,	SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_SUPPORTS},
};

typedef struct rct_window_graphs_y_axis {
	uint8 interval;
	sint8 unit;
	sint8 unit_interval;
	rct_string_id label;
} rct_window_graphs_y_axis;

/** rct2: 0x0098DD98 */
static const rct_window_graphs_y_axis window_graphs_y_axi[] = {
	{11, 0,  10, STR_RIDE_STATS_VELOCITY_FORMAT},	// GRAPH_VELOCITY
	{10, 0,  15, STR_RIDE_STATS_ALTITUDE_FORMAT},	// GRAPH_ALTITUDE
	{13, -3, 1,  STR_RIDE_STATS_G_FORCE_FORMAT},	// GRAPH_VERTICAL
	{13, -4, 1,  STR_RIDE_STATS_G_FORCE_FORMAT},	// GRAPH_LATERAL
};

static void window_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, sint32 page, sint32 spriteIndex)
{
	rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			sint32 frame = w->frame_no / window_ride_tab_animation_divisor[w->page];
			spriteIndex += (frame % window_ride_tab_animation_frames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 *
 *  rct2: 0x006B2E88
 */
static void window_ride_draw_tab_main(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_MAIN;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		sint32 spriteIndex = 0;
		sint32 rideType = get_ride(w->number)->type;

		switch (gRideClassifications[rideType]) {
		case RIDE_CLASS_RIDE:
			spriteIndex = SPR_TAB_RIDE_0;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_SHOP_OR_STALL:
			spriteIndex = SPR_TAB_SHOPS_AND_STALLS_0;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_KIOSK_OR_FACILITY:
			spriteIndex = SPR_TAB_KIOSKS_AND_FACILITIES_0;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 8;
			break;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 *
 *  rct2: 0x006B2B68
 */
static void window_ride_draw_tab_vehicle(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_VEHICLE;
	rct_widget *widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		sint32 x = widget->left + 1;
		sint32 y = widget->top + 1;
		sint32 width = widget->right - x;
		sint32 height = widget->bottom - 3 - y;
		if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
			height += 4;

		x += w->x;
		y += w->y;

		rct_drawpixelinfo clipDPI;
		if (!clip_drawpixelinfo(&clipDPI, dpi, x, y, width, height)) {
			return;
		}

		x = (widget->right - widget->left) / 2;
		y = (widget->bottom - widget->top) - 12;

		rct_ride *ride = get_ride(w->number);

		rct_ride_entry *rideEntry = get_ride_entry_by_ride(ride);
		if (rideEntry->flags & RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF) {
			clipDPI.zoom_level = 1;
			clipDPI.width *= 2;
			clipDPI.height *= 2;
			x *= 2;
			y *= 2;
			clipDPI.x *= 2;
			clipDPI.y *= 2;
		}

		const uint8 vehicle = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, rideEntry->tab_vehicle);
		rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[vehicle];

		vehicle_colour vehicleColour = ride_get_vehicle_colour(ride, 0);
		sint32 spriteIndex = 32;
		if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
			spriteIndex += w->frame_no;
		spriteIndex /= (rideVehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_11) ? 4 : 2;
		spriteIndex &= rideVehicleEntry->rotation_frame_mask;
		spriteIndex *= rideVehicleEntry->var_16;
		spriteIndex += rideVehicleEntry->base_image_id;
		spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
		spriteIndex |= 0x80000000;

		gfx_draw_sprite(&clipDPI, spriteIndex, x, y, vehicleColour.additional_2);
	}
}

/**
 *
 *  rct2: 0x006B2F42
 */
static void window_ride_draw_tab_customer(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_CUSTOMER;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		rct_widget *widget = &w->widgets[widgetIndex];
		sint32 spriteIndex = 0;
		if (w->page == WINDOW_RIDE_PAGE_CUSTOMER)
			spriteIndex = w->var_492 & ~3;

		spriteIndex += g_peep_animation_entries[PEEP_SPRITE_TYPE_NORMAL].sprite_animation->base_image;
		spriteIndex += 1;
		spriteIndex |= 0xA9E00000;

		gfx_draw_sprite(dpi, spriteIndex, w->x + (widget->left + widget->right) / 2, w->y + widget->bottom - 6, 0);
	}
}

/**
 *
 *  rct2: 0x006B2B35
 */
static void window_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_ride_draw_tab_vehicle(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_OPERATING, SPR_TAB_GEARS_0);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MAINTENANCE, SPR_TAB_WRENCH_0);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_INCOME, SPR_TAB_ADMISSION_0);
	window_ride_draw_tab_main(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MEASUREMENTS, SPR_TAB_TIMER_0);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_COLOUR, SPR_TAB_PAINT_0);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_GRAPHS, SPR_TAB_GRAPH_A_0);
	window_ride_draw_tab_customer(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MUSIC, SPR_TAB_MUSIC_0);
}

/**
*
* rct2: 0x006AEB9F
*/
static void window_ride_disable_tabs(rct_window *w)
{
	uint32 disabled_tabs = 0;
	rct_ride *ride = get_ride(w->number & 0xFF);

	uint8 ride_type = ride->type; // ecx

	if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
		disabled_tabs |= (1 << WIDX_TAB_8); // 0x800

	if (ride_type == RIDE_TYPE_MINI_GOLF)
		disabled_tabs |= (1 << WIDX_TAB_2 | 1 << WIDX_TAB_3 | 1 << WIDX_TAB_4); // 0xE0

	if (ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_NO_VEHICLES))
		disabled_tabs |= (1 << WIDX_TAB_2); // 0x20

	if (
		!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN) &&
		!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL) &&
		!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS) &&
		!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS) &&
		!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ENTRANCE_EXIT)
	) {
		disabled_tabs |= (1 << WIDX_TAB_5); // 0x100
	}

	if (ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_IS_SHOP))
		disabled_tabs |= (1 << WIDX_TAB_3 | 1 << WIDX_TAB_4 | 1 << WIDX_TAB_7); // 0x4C0

	if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MUSIC)) {
		disabled_tabs |= (1 << WIDX_TAB_6); // 0x200
	}

	if (ride_type == RIDE_TYPE_CASH_MACHINE ||
		ride_type == RIDE_TYPE_FIRST_AID ||
		(gParkFlags & PARK_FLAGS_NO_MONEY) != 0)
		disabled_tabs |= (1 << WIDX_TAB_9); // 0x1000

	if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) != 0)
		disabled_tabs |= (1 << WIDX_TAB_4 | 1 << WIDX_TAB_6 | 1 << WIDX_TAB_9 | 1 << WIDX_TAB_10); // 0x3280

	rct_ride_entry *type = get_ride_entry(ride->subtype);

	if (type == NULL) {
		disabled_tabs |= 1 << WIDX_TAB_2 | 1 << WIDX_TAB_3 | 1 << WIDX_TAB_4 | 1 << WIDX_TAB_5 | 1 << WIDX_TAB_6
					   | 1 << WIDX_TAB_7 | 1 << WIDX_TAB_8 | 1 << WIDX_TAB_9 | 1 << WIDX_TAB_10;
	} else if ((type->flags & RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB) != 0)
		disabled_tabs |= (1 << WIDX_TAB_5); // 0x100

	w->disabled_widgets = disabled_tabs;
}

static void window_ride_update_overall_view(uint8 ride_index) {
	// Calculate x, y, z bounds of the entire ride using its track elements
	map_element_iterator it;

	map_element_iterator_begin(&it);

	sint32 minx = INT_MAX, miny = INT_MAX, minz = INT_MAX;
	sint32 maxx = INT_MIN, maxy = INT_MIN, maxz = INT_MIN;

	while (map_element_iterator_next(&it)) {
		if (map_element_get_type(it.element) != MAP_ELEMENT_TYPE_TRACK)
			continue;

		if (it.element->properties.track.ride_index != ride_index)
			continue;

		sint32 x = it.x * 32;
		sint32 y = it.y * 32;
		sint32 z1 = it.element->base_height * 8;
		sint32 z2 = it.element->clearance_height * 8;

		minx = min(minx, x);
		miny = min(miny, y);
		minz = min(minz, z1);

		maxx = max(maxx, x);
		maxy = max(maxy, y);
		maxz = max(maxz, z2);
	}

	ride_overall_view *view = &ride_overall_views[ride_index];
	view->x = (minx + maxx) / 2;
	view->y = (miny + maxy) / 2;
	view->z = (minz + maxz) / 2 + 8;

	// Calculate size to determine from how far away to view the ride
	sint32 dx = maxx - minx;
	sint32 dy = maxy - miny;
	sint32 dz = maxz - minz;

	sint32 size = (sint32) sqrt(dx*dx + dy*dy + dz*dz);

	// Each farther zoom level shows twice as many tiles (log)
	// Appropriate zoom is lowered by one to fill the entire view with the ride
	double zoom = ceil(log(size / 80)) - 1;
	view->zoom = (uint8)clamp(0, zoom, 3);
}

/**
 *
 *  rct2: 0x006AEAB4
 */
static rct_window *window_ride_open(sint32 rideIndex)
{
	rct_window *w;
	rct_ride *ride;
	uint8 *rideEntryIndexPtr;
	sint32 numSubTypes;

	w = window_create_auto_pos(316, 207, window_ride_page_events[0], WC_RIDE, WF_10 | WF_RESIZABLE);
	w->widgets = window_ride_page_widgets[WINDOW_RIDE_PAGE_MAIN];
	w->enabled_widgets = window_ride_page_enabled_widgets[WINDOW_RIDE_PAGE_MAIN];
	w->number = rideIndex;

	w->page = 0;
	w->var_48C = 0;
	w->frame_no = 0;
	w->list_information_type = 0;
	w->var_492 = 0;
	w->ride_colour = 0;
	window_ride_disable_tabs(w);
	w->min_width = 316;
	w->min_height = 180;
	w->max_width = 500;
	w->max_height = 450;

	ride = get_ride(rideIndex);
	_rideType = ride->type;
	numSubTypes = 0;
	rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(ride->type);
	for (; *rideEntryIndexPtr != 0xFF; rideEntryIndexPtr++) {
		if (ride_entry_is_invented(*rideEntryIndexPtr)) {
			numSubTypes++;
		}
	}
	w->var_496 = numSubTypes;

	window_ride_update_overall_view((uint8) rideIndex);

	return w;
}

/**
 *
 *  rct2: 0x006ACC28
 */
rct_window *window_ride_main_open(sint32 rideIndex)
{
	rct_window *w;

	w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) {
		if (w->classification == gCurrentToolWidget.window_classification &&
			w->number == gCurrentToolWidget.window_number
		) {
			tool_cancel();
		}
	}

	if (w->page != WINDOW_RIDE_PAGE_MAIN) {
		window_ride_set_page(w, WINDOW_RIDE_PAGE_MAIN);
	}

	w->ride.view = 0;
	window_ride_init_viewport(w);
	return w;
}

/**
 *
 *  rct2: 0x006ACCCE
 */
rct_window *window_ride_open_station(sint32 rideIndex, sint32 stationIndex)
{
	sint32 i;
	rct_ride *ride;
	rct_window *w;

	ride = get_ride(rideIndex);

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
		return window_ride_main_open(rideIndex);

	w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (
		input_test_flag(INPUT_FLAG_TOOL_ACTIVE) &&
		gCurrentToolWidget.window_classification == w->classification &&
		gCurrentToolWidget.window_number == w->number
	) {
		tool_cancel();
	}
	_rideType = ride->type;

	w->page = WINDOW_RIDE_PAGE_MAIN;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);

	w->widgets = window_ride_page_widgets[w->page];
	w->enabled_widgets = window_ride_page_enabled_widgets[w->page];
	w->hold_down_widgets = window_ride_page_hold_down_widgets[w->page];
	w->event_handlers = window_ride_page_events[w->page];
	w->pressed_widgets = 0;
	window_ride_disable_tabs(w);
	window_init_scroll_widgets(w);

	// View
	for (i = stationIndex; i >= 0; i--) {
		if (ride->station_starts[i] == 0xFFFF)
			stationIndex--;
	}

	w->ride.view = 1 + ride->num_vehicles + stationIndex;
	window_ride_init_viewport(w);

	return w;
}

rct_window *window_ride_open_track(rct_map_element *mapElement)
{
	sint32 rideIndex = mapElement->properties.track.ride_index;
	if (
		(map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) ||
		(TrackSequenceProperties[mapElement->properties.track.type][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
	) {
		// Open ride window in station view
		return window_ride_open_station(rideIndex, map_get_station(mapElement));
	} else {
		// Open ride window in overview mode.
		return window_ride_main_open(rideIndex);
	}
}

/**
 *
 *  rct2: 0x006ACAC2
 */
rct_window *window_ride_open_vehicle(rct_vehicle *vehicle)
{
	rct_vehicle *headVehicle = vehicle_get_head(vehicle);
	uint16 headVehicleSpriteIndex = headVehicle->sprite_index;
	sint32 rideIndex = headVehicle->ride;
	rct_ride *ride = get_ride(rideIndex);

	// Get view index
	sint32 view = 1;
	for (sint32 i = 0; i < 32; i++) {
		if (ride->vehicles[i] == headVehicleSpriteIndex)
			break;

		view++;
	}


	rct_window *w = window_find_by_number(WC_RIDE, rideIndex);
	if (w != NULL) {
		window_invalidate(w);

		if (
			input_test_flag(INPUT_FLAG_TOOL_ACTIVE) &&
			gCurrentToolWidget.window_classification == w->classification &&
			gCurrentToolWidget.window_number == w->number
		) {
			tool_cancel();
		}

		sint32 openedPeepWindow = 0;
		if (w->ride.view == view) {
			sint32 numPeepsLeft = vehicle->num_peeps;
			for (sint32 i = 0; i < 32 && numPeepsLeft > 0; i++) {
				uint16 peepSpriteIndex = vehicle->peep[i];
				if (peepSpriteIndex == SPRITE_INDEX_NULL)
					continue;

				numPeepsLeft--;
				rct_window *w2 = window_find_by_number(WC_PEEP, peepSpriteIndex);
				if (w2 == NULL) {
					rct_peep *peep = &(get_sprite(peepSpriteIndex)->peep);
					window_guest_open(peep);
					openedPeepWindow = 1;

					break;
				}
			}
		}

		w = openedPeepWindow ?
			window_find_by_number(WC_RIDE, rideIndex) :
			window_bring_to_front_by_number(WC_RIDE, rideIndex);
	}

	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	_rideType = ride->type;
	w->page = WINDOW_RIDE_PAGE_MAIN;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);

	w->widgets = window_ride_page_widgets[w->page];
	w->enabled_widgets = window_ride_page_enabled_widgets[w->page];
	w->hold_down_widgets = window_ride_page_hold_down_widgets[w->page];
	w->event_handlers = window_ride_page_events[w->page];
	w->pressed_widgets = 0;
	window_ride_disable_tabs(w);
	window_init_scroll_widgets(w);

	w->ride.view = view;
	window_ride_init_viewport(w);
	window_invalidate(w);

	return w;
}

/**
 *
 *  rct2: 0x006AF1D2
 */
static void window_ride_set_page(rct_window *w, sint32 page)
{
	sint32 listen;

	if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
		if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
			tool_cancel();

	// Set listen only to viewport
	listen = 0;
	if (page == WINDOW_RIDE_PAGE_MAIN && w->page == WINDOW_RIDE_PAGE_MAIN && w->viewport != NULL && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
		listen++;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;

	//There doesn't seem to be any need for this call, and it can sometimes modify the reported number of cars per train, so I've removed it
	//if (page == WINDOW_RIDE_PAGE_VEHICLE) {
		//ride_update_max_vehicles(w->number);
	//}

	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_ride_page_enabled_widgets[page];
	w->hold_down_widgets = window_ride_page_hold_down_widgets[page];
	w->event_handlers = window_ride_page_events[page];
	w->pressed_widgets = 0;
	w->widgets = window_ride_page_widgets[page];
	window_ride_disable_tabs(w);
	window_invalidate(w);

	window_event_resize_call(w);
	window_event_invalidate_call(w);
	window_init_scroll_widgets(w);
	window_invalidate(w);

	if (listen != 0 && w->viewport != NULL)
		w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void window_ride_set_pressed_tab(rct_window *w)
{
	sint32 i;
	for (i = 0; i < WINDOW_RIDE_PAGE_COUNT; i++)
		w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
	w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_ride_anchor_border_widgets(rct_window *w)
{
	w->widgets[WIDX_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].right = w->width - 1;
	w->widgets[WIDX_PAGE_BACKGROUND].bottom = w->height - 1;
	w->widgets[WIDX_TITLE].right = w->width - 2;
	w->widgets[WIDX_CLOSE].left = w->width - 13;
	w->widgets[WIDX_CLOSE].right = w->width - 3;
}

#pragma region Main

/**
 *
 *  rct2: 0x006AF994
 */
static void window_ride_init_viewport(rct_window *w)
{
	if (w->page != WINDOW_RIDE_PAGE_MAIN) return;

	rct_ride* ride = get_ride(w->number);
	sint32 eax = w->viewport_focus_coordinates.var_480 - 1;

	union{
		sprite_focus sprite;
		coordinate_focus coordinate;
	} focus;

	focus.coordinate.x = 0;
	focus.coordinate.y = 0;
	focus.coordinate.z = 0;
	focus.sprite.sprite_id = -1;
	focus.coordinate.zoom = 0;
	focus.coordinate.rotation = get_current_rotation();
	focus.coordinate.width = 0;
	focus.coordinate.height = 0;

	if (eax >= 0 && eax < ride->num_vehicles && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK){
		focus.sprite.sprite_id = ride->vehicles[eax];

		rct_ride_entry* ride_entry = get_ride_entry_by_ride(ride);
		if (ride_entry && ride_entry->tab_vehicle != 0){
			rct_vehicle* vehicle = GET_VEHICLE(focus.sprite.sprite_id);
			focus.sprite.sprite_id = vehicle->next_vehicle_on_train;
		}
		focus.sprite.type |= 0xC0;
	}
	else if (eax >= ride->num_vehicles && eax < (ride->num_vehicles + ride->num_stations)){
		sint32 stationIndex = -1;
		sint32 count = eax - ride->num_vehicles;
		do {
			stationIndex++;
			if (ride->station_starts[stationIndex] != 0xFFFF)
				count--;
		} while (count >= 0);

		eax = ride->station_starts[stationIndex];

		focus.coordinate.x = (eax & 0xFF) << 5;
		focus.coordinate.y = (eax & 0xFF00) >> 3;
		focus.coordinate.z = ride->station_heights[stationIndex] << 3;
		focus.sprite.type |= 0x40;
	}
	else{
		if (eax > 0){
			w->viewport_focus_coordinates.var_480 = 0;
		}

		ride_overall_view *view = &ride_overall_views[w->number];

		focus.coordinate.x = view->x;
		focus.coordinate.y = view->y;
		focus.coordinate.z = view->z;
		focus.coordinate.zoom = view->zoom;

		focus.sprite.type |= 0x40;
	}
	focus.coordinate.var_480 = w->viewport_focus_coordinates.var_480;

	uint16 viewport_flags = 0;

	if (w->viewport != NULL) {
		if (focus.coordinate.x == w->viewport_focus_coordinates.x &&
			focus.coordinate.y == w->viewport_focus_coordinates.y &&
			focus.coordinate.z == w->viewport_focus_coordinates.z &&
			focus.coordinate.rotation == w->viewport_focus_coordinates.rotation &&
			focus.coordinate.zoom == w->viewport_focus_coordinates.zoom &&
			focus.coordinate.width == w->width &&
			focus.coordinate.height == w->height
		) {
			return;
		}
		viewport_flags = w->viewport->flags;
		w->viewport->width = 0;
		w->viewport = NULL;

		viewport_update_pointers();
	} else if (gConfigGeneral.always_show_gridlines) {
		viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
	}

	window_event_invalidate_call(w);

	w->viewport_focus_coordinates.x = focus.coordinate.x;
	w->viewport_focus_coordinates.y = focus.coordinate.y;
	w->viewport_focus_coordinates.z = focus.coordinate.z;
	w->viewport_focus_coordinates.rotation = focus.coordinate.rotation;
	w->viewport_focus_coordinates.zoom = focus.coordinate.zoom;
	w->viewport_focus_coordinates.width = w->width;
	w->viewport_focus_coordinates.height = w->height;

	//rct2: 0x006aec9c only used here so brought it into the function
	if (!w->viewport && ride->overall_view != 0xFFFF){
		rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

		sint32 x = view_widget->left + 1 + w->x;
		sint32 y = view_widget->top + 1 + w->y;
		sint32 width = view_widget->right - view_widget->left - 1;
		sint32 height = view_widget->bottom - view_widget->top - 1;
		viewport_create(
			w,
			x,
			y,
			width,
			height,
			focus.coordinate.zoom,
			focus.coordinate.x,
			focus.coordinate.y & VIEWPORT_FOCUS_Y_MASK,
			focus.coordinate.z,
			focus.sprite.type & VIEWPORT_FOCUS_TYPE_MASK,
			focus.sprite.sprite_id);

		w->flags |= WF_NO_SCROLLING;
		window_invalidate(w);
	}
	if (w->viewport){
		w->viewport->flags = viewport_flags;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006AF315
 */
static void window_ride_rename(rct_window *w)
{
	rct_ride *ride;

	ride = get_ride(w->number);
	set_format_arg(16, uint32, ride->name_arguments);
	window_text_input_open(w, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, ride->name, ride->name_arguments, 32);
}

/**
 *
 *  rct2: 0x006AF17E
 */
static void window_ride_main_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	uint8 rideIndex;
	rct_ride *ride;
	sint32 status;

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_CONSTRUCTION:
		rideIndex = (uint8)w->number;
		ride_construct(rideIndex);
		if (window_find_by_number(WC_RIDE_CONSTRUCTION, rideIndex) != NULL) {
			window_close(w);
		}
		break;
	case WIDX_RENAME:
		window_ride_rename(w);
		break;
	case WIDX_LOCATE:
		window_scroll_to_viewport(w);
		break;
	case WIDX_DEMOLISH:
		window_ride_demolish_prompt_open(w->number);
		break;
	case WIDX_CLOSE_LIGHT:
	case WIDX_TEST_LIGHT:
	case WIDX_OPEN_LIGHT:

		ride = get_ride(w->number);

		switch (widgetIndex) {
		default:
		case WIDX_CLOSE_LIGHT:
			status = RIDE_STATUS_CLOSED;
			gGameCommandErrorTitle = STR_CANT_CLOSE;
			break;
		case WIDX_TEST_LIGHT:
			status = RIDE_STATUS_TESTING;
			gGameCommandErrorTitle = STR_CANT_TEST;
			break;
		case WIDX_OPEN_LIGHT:
			status = RIDE_STATUS_OPEN;
			gGameCommandErrorTitle = STR_CANT_OPEN;
			break;
		}

		set_format_arg(6, rct_string_id, ride->name);
		set_format_arg(8, uint32, ride->name_arguments);
		ride_set_status(w->number, status);
		break;
	}
}

/**
 *
 *  rct2: 0x006AF4A2
 */
static void window_ride_main_resize(rct_window *w)
{
	const sint32 offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
	w->flags |= WF_RESIZABLE;
	sint32 minHeight = 180 + offset;
	if (theme_get_flags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
		minHeight = 200 + offset + RCT1_LIGHT_OFFSET - (ride_type_has_flag(get_ride(w->number)->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? 14 : 0);
	window_set_resize(w, 316, minHeight, 500, 450);
	window_ride_init_viewport(w);
}

/**
 *
 *  rct2: 0x006AF825
 */
static void window_ride_show_view_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget = widget - 1;
	rct_ride *ride = get_ride(w->number);

	sint32 numItems = 1;
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES)) {
		numItems += ride->num_stations;
		numItems += ride->num_vehicles;
	}

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		0,
		numItems,
		widget->right - dropdownWidget->left
	);

	// First item
	gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
	gDropdownItemsArgs[0] = STR_OVERALL_VIEW;
	sint32 currentItem = 1;

	// Vehicles
	sint32 name = RideComponentNames[RideNameConvention[ride->type].vehicle].number;
	for (sint32 i = 1; i <= ride->num_vehicles; i++) {
		gDropdownItemsFormat[currentItem] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[currentItem] = name | (currentItem << 16);
		currentItem++;
	}

	// Stations
	name = RideComponentNames[RideNameConvention[ride->type].station].number;
	for (sint32 i = 1; i <= ride->num_stations; i++) {
		gDropdownItemsFormat[currentItem] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[currentItem] = name | (i << 16);
		currentItem++;
	}

	// Set highlighted item
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
		sint32 j = 2;
		for (sint32 i = 0; i < ride->num_vehicles; i++) {
			gDropdownItemsDisabled |= j;
			j <<= 1;
		}
	}

	// Set checked item
	dropdown_set_checked(w->ride.view, true);
}

/**
 *
 *  rct2: 0x006AF64C
 */
static void window_ride_show_open_dropdown(rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	sint32 numItems, highlightedIndex = 0, checkedIndex;

	ride = get_ride(w->number);

	numItems = 0;
	gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
	gDropdownItemsArgs[numItems] = STR_CLOSE_RIDE;
	numItems++;

	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE)) {
		gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[numItems] = STR_TEST_RIDE;
		numItems++;
	}

	gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
	gDropdownItemsArgs[numItems] = STR_OPEN_RIDE;
	numItems++;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1],
		0,
		numItems
	);

	checkedIndex = ride->status;
	switch (ride->status) {
	case RIDE_STATUS_CLOSED:
		highlightedIndex = 0;
		if ((ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) || (ride->lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE))
			break;

		highlightedIndex = 2;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE))
			break;
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
			break;

		highlightedIndex = 1;
		break;
	case RIDE_STATUS_TESTING:
		highlightedIndex = 2;
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
			break;

		highlightedIndex = 0;
		break;
	case RIDE_STATUS_OPEN:
		highlightedIndex = 0;
		break;
	}

	if (checkedIndex != RIDE_STATUS_CLOSED)
		checkedIndex = 3 - checkedIndex;

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE)) {
		if (checkedIndex != 0)
			checkedIndex--;
		if (highlightedIndex != 0)
			highlightedIndex--;
	}

	dropdown_set_checked(checkedIndex, true);
	gDropdownDefaultIndex = highlightedIndex;
}

/**
 *
 *  rct2: 0x006AF1BD
 */
static void window_ride_main_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		window_ride_show_view_dropdown(w, widget);
		break;
	case WIDX_OPEN:
		window_ride_show_open_dropdown(w, widget);
		break;
	case WIDX_RIDE_TYPE_INCREASE:
		if (_rideType >= 90) {
			_rideType = 90;
		} else {
			_rideType++;
		}
		widget_invalidate(w, WIDX_RIDE_TYPE);
		break;
	case WIDX_RIDE_TYPE_DECREASE:
		if (_rideType == 0) {
			_rideType = 0;
		} else {
			_rideType--;
		}
		widget_invalidate(w, WIDX_RIDE_TYPE);
		break;
	case WIDX_RIDE_TYPE_APPLY:
		if (_rideType <= 90) {
			set_operating_setting(w->number, RIDE_SETTING_RIDE_TYPE, _rideType);
		}
		window_invalidate_all();
		break;
	}
}

/**
 *
 *  rct2: 0x006AF300
 */
static void window_ride_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	rct_ride *ride;
	sint32 status = 0;

	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		if (dropdownIndex == -1) {
			dropdownIndex = w->ride.view;
			ride = get_ride(w->number);
			dropdownIndex++;
			if (dropdownIndex != 0 && dropdownIndex <= ride->num_vehicles && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
				dropdownIndex = ride->num_vehicles + 1;

			if (dropdownIndex >= gDropdownNumItems)
				dropdownIndex = 0;
		}

		w->ride.view = dropdownIndex;
		window_ride_init_viewport(w);
		window_invalidate(w);
		break;
	case WIDX_OPEN:
		if (dropdownIndex == -1)
			dropdownIndex = gDropdownHighlightedIndex;

		ride = get_ride(w->number);
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE) && dropdownIndex != 0)
			dropdownIndex++;

		switch (dropdownIndex) {
		case 0:
			status = RIDE_STATUS_CLOSED;
			gGameCommandErrorTitle = STR_CANT_CLOSE;
			break;
		case 1:
			status = RIDE_STATUS_TESTING;
			gGameCommandErrorTitle = STR_CANT_TEST;
			break;
		case 2:
			status = RIDE_STATUS_OPEN;
			gGameCommandErrorTitle = STR_CANT_OPEN;
			break;
		}

		set_format_arg(6, rct_string_id, ride->name);
		set_format_arg(8, uint32, ride->name_arguments);
		ride_set_status(w->number, status);
		break;
	}
}

/**
 *
 *  rct2: 0x006AF40F
 */
static void window_ride_main_update(rct_window *w)
{
	// Update tab animation
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_1);

	// Update status
	rct_ride *ride = get_ride(w->number);
	if (!(ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN)) {
		if (w->ride.view == 0)
			return;

		if (w->ride.view <= ride->num_vehicles) {
			sint32 vehicleIndex = w->ride.view - 1;
			uint16 vehicleSpriteIndex = ride->vehicles[vehicleIndex];
			if (vehicleSpriteIndex == SPRITE_INDEX_NULL)
				return;

			rct_vehicle *vehicle = &(get_sprite(vehicleSpriteIndex)->vehicle);
			if (
				vehicle->status != 4 &&
				vehicle->status != 22 &&
				vehicle->status != 10 &&
				vehicle->status != 7
			) {
				return;
			}
		}
	}

	ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAIN;
	widget_invalidate(w, WIDX_STATUS);
}

/**
 *
 *  rct2: 0x006AF2F9
 */
static void window_ride_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
	if (widgetIndex != WIDX_RENAME || text == NULL)
		return;

	ride_set_name(w->number, text);
}

/**
 *
 *  rct2: 0x006AF55A
 */
static void window_ride_main_unknown_14(rct_window *w)
{
	window_ride_init_viewport(w);
}

/**
 *
 *  rct2: 0x006AECF6
 */
static void window_ride_main_invalidate(rct_window *w)
{
	rct_widget *widgets;
	sint32 i, height;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	w->disabled_widgets &= ~((1 << WIDX_DEMOLISH) | (1 << WIDX_CONSTRUCTION));
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK))
		w->disabled_widgets |= (1 << WIDX_DEMOLISH);

	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);
	set_format_arg(6, uint16, RideNaming[_rideType].name);
	uint32 spriteIds[] = {
		SPR_CLOSED,
		SPR_OPEN,
		SPR_TESTING,
	};
	window_ride_main_widgets[WIDX_OPEN].image = spriteIds[ride->status];

	window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RIDE_STATUS_CLOSED) * 2 + widget_is_pressed(w, WIDX_CLOSE_LIGHT);
	window_ride_main_widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0 + (ride->status == RIDE_STATUS_TESTING) * 2 + widget_is_pressed(w, WIDX_TEST_LIGHT);
	window_ride_main_widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + (ride->status == RIDE_STATUS_OPEN) * 2 + widget_is_pressed(w, WIDX_OPEN_LIGHT);

	window_ride_anchor_border_widgets(w);

	const sint32 offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
	// Anchor main page specific widgets
	window_ride_main_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_ride_main_widgets[WIDX_VIEWPORT].bottom = w->height - (14 + offset);
	window_ride_main_widgets[WIDX_STATUS].right = w->width - 26;
	window_ride_main_widgets[WIDX_STATUS].top = w->height - (13 + offset);
	window_ride_main_widgets[WIDX_STATUS].bottom = w->height - (3 + offset);
	window_ride_main_widgets[WIDX_VIEW].right = w->width - 60;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = w->width - 61;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = w->width - 71;
	window_ride_main_widgets[WIDX_RIDE_TYPE].right = w->width - 87;
	window_ride_main_widgets[WIDX_RIDE_TYPE].top = w->height - 17;
	window_ride_main_widgets[WIDX_RIDE_TYPE].bottom = w->height - 4;
	window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].right = w->width - 88;
	window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].left = w->width - 98;
	window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].top = w->height - 16;
	window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].bottom = w->height - 11;
	window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].right = w->width - 88;
	window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].left = w->width - 98;
	window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].top = w->height - 10;
	window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].bottom = w->height - 5;
	window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].left = w->width - 83;
	window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].right = w->width - 26;
	window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].top = w->height - 17;
	window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].bottom = w->height - 4;

	if (!gCheatsAllowArbitraryRideTypeChanges) {
		window_ride_main_widgets[WIDX_RIDE_TYPE].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].type = WWT_EMPTY;
	} else {
		window_ride_main_widgets[WIDX_RIDE_TYPE].type = WWT_SPINNER;
		window_ride_main_widgets[WIDX_RIDE_TYPE_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_main_widgets[WIDX_RIDE_TYPE_DECREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_main_widgets[WIDX_RIDE_TYPE_APPLY].type = WWT_CLOSEBOX;
	}

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

	if (theme_get_flags() & UITHEME_FLAG_USE_LIGHTS_RIDE) {
		window_ride_main_widgets[WIDX_OPEN].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WWT_IMGBTN;
		window_ride_main_widgets[WIDX_TEST_LIGHT].type = (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? WWT_EMPTY : WWT_IMGBTN);
		window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WWT_IMGBTN;

		height = 62;
		if (window_ride_main_widgets[WIDX_TEST_LIGHT].type != WWT_EMPTY) {
			window_ride_main_widgets[WIDX_TEST_LIGHT].top = height;
			window_ride_main_widgets[WIDX_TEST_LIGHT].bottom = height + 13;
			height += 14;
		}
		window_ride_main_widgets[WIDX_OPEN_LIGHT].top = height;
		window_ride_main_widgets[WIDX_OPEN_LIGHT].bottom = height + 13;
		height += 14 - 24 + RCT1_LIGHT_OFFSET;

		w->min_height = 200 + RCT1_LIGHT_OFFSET - (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? 14 : 0);
		if (w->height < w->min_height)
			window_event_resize_call(w);
	}
	else {
		window_ride_main_widgets[WIDX_OPEN].type = WWT_FLATBTN;
		window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_TEST_LIGHT].type = WWT_EMPTY;
		window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WWT_EMPTY;
		height = 46;
	}
	for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++) {
		window_ride_main_widgets[i].left = w->width - 20;
		window_ride_main_widgets[i].right = w->width - 7;
	}
	for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++, height += 24) {
		window_ride_main_widgets[i].left = w->width - 25;
		window_ride_main_widgets[i].right = w->width - 2;
		window_ride_main_widgets[i].top = height;
		window_ride_main_widgets[i].bottom = height + 23;
	}
}

/**
 *
 *  rct2: 0x006AF10A
 */
static rct_string_id window_ride_get_status_overall_view(rct_window *w, void *arguments)
{
	sint32 argument;
	rct_string_id formatSecondary, stringId;

	ride_get_status(w->number, &formatSecondary, &argument);
	*(uint16*)((uintptr_t)arguments + 0) = formatSecondary;
	*(uintptr_t*)((uintptr_t)arguments + 2) = argument;
	stringId = STR_RED_OUTLINED_STRING;
	if (formatSecondary != STR_BROKEN_DOWN && formatSecondary != STR_CRASHED)
		stringId = STR_BLACK_STRING;

	return stringId;
}

/**
 *
 *  rct2: 0x006AEFEF
 */
static rct_string_id window_ride_get_status_vehicle(rct_window *w, void *arguments)
{
	rct_ride *ride;
	rct_vehicle *vehicle;
	sint32 vehicleIndex;
	uint16 vehicleSpriteIndex;
	rct_string_id stringId;

	ride = get_ride(w->number);

	vehicleIndex = w->ride.view - 1;
	vehicleSpriteIndex = ride->vehicles[vehicleIndex];
	if (vehicleSpriteIndex == SPRITE_INDEX_NULL)
		return 0;

	vehicle = &(get_sprite(vehicleSpriteIndex)->vehicle);
	if (vehicle->status != VEHICLE_STATUS_CRASHING && vehicle->status != VEHICLE_STATUS_CRASHED) {
		sint32 trackType = vehicle->track_type >> 2;
		if (trackType == TRACK_ELEM_BLOCK_BRAKES ||
			trackType == TRACK_ELEM_CABLE_LIFT_HILL ||
			trackType == TRACK_ELEM_25_DEG_UP_TO_FLAT ||
			trackType == TRACK_ELEM_60_DEG_UP_TO_FLAT ||
			trackType == TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT ||
			trackType == TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT) {
			if (track_piece_is_available_for_ride_type(ride->type, TRACK_BLOCK_BRAKES) && vehicle->velocity == 0) {
				*(rct_string_id*)(uintptr_t)arguments = STR_STOPPED_BY_BLOCK_BRAKES;
				return STR_BLACK_STRING;
			}
		}
	}

	stringId = VehicleStatusNames[vehicle->status];

	// Get speed in mph
	*((uint16*)((uintptr_t)arguments + 2)) = (abs(vehicle->velocity) * 9) >> 18;

	if (ride->type == RIDE_TYPE_MINI_GOLF)
		return 0;

	if ((RideData4[ride->type].flags & RIDE_TYPE_FLAG4_SINGLE_SESSION) && vehicle->status <= VEHICLE_STATUS_UNLOADING_PASSENGERS) {
		stringId = SingleSessionVehicleStatusNames[vehicle->status];
	}

	const ride_component_name stationName = RideComponentNames[RideNameConvention[ride->type].station];
	*(rct_string_id*)((uintptr_t)arguments + 4) = (ride->num_stations > 1) ? stationName.number : stationName.singular;
	*((uint16*)((uintptr_t)arguments + 6)) = vehicle->current_station + 1;
	*(rct_string_id*)((uintptr_t)arguments + 0) = stringId;
	return stringId != STR_CRASHING && stringId != STR_CRASHED_0 ? STR_BLACK_STRING : STR_RED_OUTLINED_STRING;
}

/**
 *
 *  rct2: 0x006AEF65
 */
static rct_string_id window_ride_get_status_station(rct_window *w, void *arguments)
{
	rct_ride *ride = get_ride(w->number);
	sint32 count = w->ride.view - ride->num_vehicles - 1;
	sint32 stationIndex = -1;
	rct_string_id stringId = 0;

	do {
		stationIndex++;
		if (ride->station_starts[stationIndex] != 0xFFFF)
			count--;
	} while (count >= 0);

	// Entrance / exit
	if (ride->status == RIDE_STATUS_CLOSED) {
		if (ride->entrances[stationIndex] == 0xFFFF)
			stringId = STR_NO_ENTRANCE;
		else if (ride->exits[stationIndex] == 0xFFFF)
			stringId = STR_NO_EXIT;
	} else {
		if (ride->entrances[stationIndex] == 0xFFFF)
			stringId = STR_EXIT_ONLY;
	}

	// Queue length
	if (stringId == 0) {
		sint32 queueLength = ride->queue_length[stationIndex];
		set_format_arg_body(arguments, 2, (uintptr_t)queueLength, sizeof(uint16));
		stringId = STR_QUEUE_EMPTY;
		if (queueLength == 1)
			stringId = STR_QUEUE_ONE_PERSON;
		else if (queueLength > 1)
			stringId = STR_QUEUE_PEOPLE;
	}

	set_format_arg_body(arguments, 0, (uintptr_t)stringId, sizeof(rct_string_id));
	return STR_BLACK_STRING;
}

/**
 *
 *  rct2: 0x006AEE73
 */
static rct_string_id window_ride_get_status(rct_window *w, void *arguments)
{
	rct_ride *ride = get_ride(w->number);

	if (w->ride.view == 0)
		return window_ride_get_status_overall_view(w, arguments);
	if (w->ride.view <= ride->num_vehicles)
		return window_ride_get_status_vehicle(w, arguments);
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
		return window_ride_get_status_overall_view(w, arguments);
	return window_ride_get_status_station(w, arguments);
}

/**
 *
 *  rct2: 0x006AEE73
 */
static void window_ride_main_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_widget *widget;
	rct_string_id stringId;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	// Viewport and ear icon
	if (w->viewport != NULL) {
		window_draw_viewport(dpi, w);
		if (w->viewport->flags & VIEWPORT_FLAG_SOUND_ON)
			gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2, 0);
	}

	// View dropdown
	ride = get_ride(w->number);
	stringId = STR_OVERALL_VIEW;
	if (w->ride.view != 0) {
		stringId = RideComponentNames[RideNameConvention[ride->type].vehicle].number;
		if (w->ride.view > ride->num_vehicles) {
			set_format_arg(2, uint16, w->ride.view - ride->num_vehicles);
			stringId = RideComponentNames[RideNameConvention[ride->type].station].number;
		} else {
			set_format_arg(2, uint16, w->ride.view);
		}
	}
	set_format_arg(0, uint16, stringId);

	widget = &window_ride_main_widgets[WIDX_VIEW];
	gfx_draw_string_centred(
		dpi,
		STR_WINDOW_COLOUR_2_STRINGID,
		w->x + (widget->left + widget->right - 11) / 2,
		w->y + widget->top,
		COLOUR_BLACK,
		gCommonFormatArgs
	);

	// Status
	widget = &window_ride_main_widgets[WIDX_STATUS];
	gfx_draw_string_centred_clipped(
		dpi,
		window_ride_get_status(w, gCommonFormatArgs),
		gCommonFormatArgs,
		COLOUR_BLACK,
		w->x + (widget->left + widget->right) / 2,
		w->y + widget->top,
		widget->right - widget->left
	);
}

#pragma endregion

#pragma region Vehicle

/**
 *
 *  rct2: 0x006B272D
 */
static void window_ride_vehicle_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/**
 *
 *  rct2: 0x006B2ABB
 */
static void window_ride_vehicle_resize(rct_window *w)
{
	window_set_resize(w, 316, 208, 316, 208);
}

/**
 *
 *  rct2: 0x006B2748
 */
static void window_ride_vehicle_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget = widget - 1;
	rct_ride *ride;
	rct_ride_entry *rideEntry, *currentRideEntry;
	sint32 numItems, rideEntryIndex, selectedIndex, rideTypeIterator, rideTypeIteratorMax;
	uint8 *rideEntryIndexPtr;
	bool selectionShouldBeExpanded;

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	if(gCheatsShowVehiclesFromOtherTrackTypes && !(ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type==RIDE_TYPE_MAZE || ride->type==RIDE_TYPE_MINI_GOLF)) {
		selectionShouldBeExpanded=true;
		rideTypeIterator=0;
		rideTypeIteratorMax=90;
	}
	else {
		selectionShouldBeExpanded=false;
		rideTypeIterator=ride->type;
		rideTypeIteratorMax=ride->type;
	}

	switch (widgetIndex) {
	case WIDX_VEHICLE_TYPE_DROPDOWN:
		selectedIndex = -1;
		numItems = 0;

		// Dropdowns with more items start acting weird, so cap it to 63.
		for (; rideTypeIterator<=rideTypeIteratorMax && numItems<=63; rideTypeIterator++) {

			if(selectionShouldBeExpanded && ride_type_has_flag(rideTypeIterator, RIDE_TYPE_FLAG_FLAT_RIDE))
				continue;
			if(selectionShouldBeExpanded && (rideTypeIterator==RIDE_TYPE_MAZE || rideTypeIterator==RIDE_TYPE_MINI_GOLF))
				continue;

			rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideTypeIterator);

			for (uint8 *currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != 0xFF && numItems <= 63; currentRideEntryIndex++) {
				rideEntryIndex = *currentRideEntryIndex;
				currentRideEntry = get_ride_entry(rideEntryIndex);
				// Skip if vehicle has the same track type, but not same subtype, unless subtype switching is enabled
				if ((currentRideEntry->flags & (RIDE_ENTRY_FLAG_SEPARATE_RIDE | RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME)) && !(gConfigInterface.select_by_track_type || selectionShouldBeExpanded))
					continue;

				// Skip if vehicle type is not invented yet
				if (!ride_entry_is_invented(rideEntryIndex))
					continue;

				if (ride->subtype == rideEntryIndex)
					selectedIndex = numItems;

				gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[numItems] = (rideEntryIndex << 16) | currentRideEntry->name;

				numItems++;
			}
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			numItems,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(selectedIndex, true);
		break;
	case WIDX_VEHICLE_TRAINS_INCREASE:
		if (ride->num_vehicles < 32)
			ride_set_num_vehicles(w->number, ride->num_vehicles + 1);
		break;
	case WIDX_VEHICLE_TRAINS_DECREASE:
		if (ride->num_vehicles > 1)
			ride_set_num_vehicles(w->number, ride->num_vehicles - 1);
		break;
	case WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE:
		if (ride->num_cars_per_train < 255)
			ride_set_num_cars_per_vehicle(w->number, ride->num_cars_per_train + 1);
		break;
	case WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE:
		if (ride->num_cars_per_train > rideEntry->zero_cars + 1)
			ride_set_num_cars_per_vehicle(w->number, ride->num_cars_per_train - 1);
		break;
	}
}

/**
 *
 *  rct2: 0x006B2767
 */
static void window_ride_vehicle_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_VEHICLE_TYPE_DROPDOWN:
		dropdownIndex = (gDropdownItemsArgs[dropdownIndex] >> 16) & 0xFFFF;
		ride_set_ride_entry(w->number, dropdownIndex);
		break;
	}
}

/**
 *
 *  rct2: 0x006B2AA1
 */
static void window_ride_vehicle_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006B222C
 */
static void window_ride_vehicle_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	rct_ride_entry *rideEntry;
	rct_string_id stringId;
	sint32 carsPerTrain;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	// Widget setup
	carsPerTrain = ride->num_cars_per_train - rideEntry->zero_cars;

	// Vehicle type
	window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].text = rideEntry->name;
	// Always show a dropdown button when changing subtypes is allowed
	if ((w->var_496 <= 1 || (rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)) && !(gConfigInterface.select_by_track_type || gCheatsShowVehiclesFromOtherTrackTypes)) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].type = WWT_14;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE_DROPDOWN].type = WWT_EMPTY;
		w->enabled_widgets &= ~(1 << WIDX_VEHICLE_TYPE);
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].type = WWT_DROPDOWN;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		w->enabled_widgets |= (1 << WIDX_VEHICLE_TYPE);
	}

	// Trains
	if (rideEntry->cars_per_flat_ride > 1 || gCheatsDisableTrainLengthLimit) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_SPINNER;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WWT_DROPDOWN_BUTTON;

	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WWT_EMPTY;
	}

	// Cars per train
	if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train||gCheatsDisableTrainLengthLimit) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_SPINNER;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WWT_DROPDOWN_BUTTON;
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WWT_EMPTY;

	}

	set_format_arg(6, uint16, carsPerTrain);
	ride_component_type vehicleType = RideNameConvention[ride->type].vehicle;
	stringId = RideComponentNames[vehicleType].count;
	if (ride->num_vehicles > 1) {
		stringId = RideComponentNames[vehicleType].count_plural;
	}
	set_format_arg(8, rct_string_id, stringId);
	set_format_arg(10, uint16, ride->num_vehicles);

	stringId = RideComponentNames[vehicleType].count;
	if (ride->max_trains > 1) {
		stringId = RideComponentNames[vehicleType].count_plural;
	}
	set_format_arg(12, rct_string_id, stringId);
	set_format_arg(14, uint16, ride->max_trains);

	set_format_arg(16, uint16, max(1, ride->min_max_cars_per_train & 0xF) - rideEntry->zero_cars);

	stringId = RideComponentNames[RIDE_COMPONENT_TYPE_CAR].singular;
	if ((ride->min_max_cars_per_train & 0xF) - rideEntry->zero_cars > 1) {
		stringId = RideComponentNames[RIDE_COMPONENT_TYPE_CAR].plural;
	}

	set_format_arg(18, rct_string_id, stringId);

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

	if (ride->num_cars_per_train > (rideEntry->zero_cars + 1)) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_X_CARS_PER_TRAIN;
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_1_CAR_PER_TRAIN;
	}
}

/**
 *
 *  rct2: 0x006B23DC
 */
static void window_ride_vehicle_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_ride_entry *rideEntry;
	sint32 x, y;
	sint16 factor;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	x = w->x + 8;
	y = w->y + 64;

	// Description
	y += gfx_draw_string_left_wrapped(dpi, &rideEntry->description, x, y, 300, STR_BLACK_STRING, COLOUR_BLACK);
	y += 5;

	// Capacity
	void * loadedObject = object_manager_get_loaded_object_by_index(ride->subtype);
	if (loadedObject != NULL)
	{
		const utf8 * capacity = object_get_capacity(loadedObject);
		set_format_arg(0, rct_string_id, STR_STRING);
		set_format_arg(2, utf8 *, capacity);
		gfx_draw_string_left(dpi, STR_CAPACITY, gCommonFormatArgs, COLOUR_BLACK, x, y);
	}
	y += 5;

	if ((!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) || rideTypeShouldLoseSeparateFlag(rideEntry)) && w->var_496 > 1) {
		// Excitement Factor
		factor = rideEntry->excitement_multipler;
		if (factor > 0) {
			y += 10;
			gfx_draw_string_left(dpi, STR_EXCITEMENT_FACTOR, &factor, COLOUR_BLACK, x, y);
		}

		// Intensity Factor
		factor = rideEntry->intensity_multipler;
		if (factor > 0) {
			y += 10;
			gfx_draw_string_left(dpi, STR_INTENSITY_FACTOR, &factor, COLOUR_BLACK, x, y);
		}

		// Nausea Factor
		factor = rideEntry->nausea_multipler;
		if (factor > 0) {
			y += 10;
			gfx_draw_string_left(dpi, STR_NAUSEA_FACTOR, &factor, COLOUR_BLACK, x, y);
		}
	}
}

typedef struct rct_vehichle_paintinfo {
	sint16 x;
	sint16 y;
	sint32 sprite_index;
	sint32 tertiary_colour;
} rct_vehichle_paintinfo;

rct_vehichle_paintinfo _sprites_to_draw[144];

/**
 *
 *  rct2: 0x006B2502
 */
static void window_ride_vehicle_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	rct_ride *ride = get_ride(w->number);
	rct_ride_entry *rideEntry = get_ride_entry_by_ride(ride);

	// Background
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, PALETTE_INDEX_12);

	rct_widget *widget = &window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
	sint32 startX = max(2, ((widget->right - widget->left) - ((ride->num_vehicles - 1) * 36)) / 2 - 25);
	sint32 startY = widget->bottom - widget->top - 4;

	rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, 0)];
	startY += rideVehicleEntry->tab_height;

	// For each train
	for (sint32 i = 0; i < ride->num_vehicles; i++) {
		rct_vehichle_paintinfo *nextSpriteToDraw = _sprites_to_draw;
		sint32 x = startX;
		sint32 y = startY;

		// For each car in train
		for (sint32 j = 0; j < ride->num_cars_per_train; j++) {
			rideVehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, j)];
			x += rideVehicleEntry->spacing / 17432;
			y -= (rideVehicleEntry->spacing / 2) / 17432;

			// Get colour of vehicle
			sint32 vehicleColourIndex = 0;
			switch (ride->colour_scheme_type & 3) {
			case VEHICLE_COLOUR_SCHEME_SAME:
				vehicleColourIndex = 0;
				break;
			case VEHICLE_COLOUR_SCHEME_PER_TRAIN:
				vehicleColourIndex = i;
				break;
			case VEHICLE_COLOUR_SCHEME_PER_VEHICLE:
				vehicleColourIndex = j;
				break;
			}
			vehicle_colour vehicleColour = ride_get_vehicle_colour(ride, vehicleColourIndex);

			sint32 spriteIndex = 16;
			if (rideVehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_11)
				spriteIndex /= 2;

			spriteIndex &= rideVehicleEntry->rotation_frame_mask;
			spriteIndex *= rideVehicleEntry->var_16;
			spriteIndex += rideVehicleEntry->base_image_id;
			spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
			spriteIndex |= 0x80000000;

			nextSpriteToDraw->x = x;
			nextSpriteToDraw->y = y;
			nextSpriteToDraw->sprite_index = spriteIndex;
			nextSpriteToDraw->tertiary_colour = vehicleColour.additional_2;
			nextSpriteToDraw++;

			x += rideVehicleEntry->spacing / 17432;
			y -= (rideVehicleEntry->spacing / 2) / 17432;
		}

		if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER) {
			rct_vehichle_paintinfo tmp = *(nextSpriteToDraw - 1);
			*(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
			*(nextSpriteToDraw - 2) = tmp;
		}

		rct_vehichle_paintinfo *current = nextSpriteToDraw;
		while (--current >= _sprites_to_draw)
			gfx_draw_sprite(dpi, current->sprite_index, current->x, current->y, current->tertiary_colour);

		startX += 36;
	}
}

#pragma endregion

#pragma region Operating

static void set_operating_setting(sint32 rideNumber, uint8 setting, uint8 value)
{
	gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (value << 8) | 1, 0, (setting << 8) | rideNumber, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

static void window_ride_mode_tweak_set(rct_window *w, uint8 value)
{
	rct_ride *ride = get_ride(w->number);

	gGameCommandErrorTitle = STR_CANT_CHANGE_LAUNCH_SPEED;
	if (ride->mode == RIDE_MODE_STATION_TO_STATION)
		gGameCommandErrorTitle = STR_CANT_CHANGE_SPEED;
	if (ride->mode == RIDE_MODE_RACE)
		gGameCommandErrorTitle = STR_CANT_CHANGE_NUMBER_OF_LAPS;
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
		gGameCommandErrorTitle = STR_CANT_CHANGE_THIS;
	if (ride->mode == RIDE_MODE_BUMPERCAR)
		gGameCommandErrorTitle = STR_CANT_CHANGE_TIME_LIMIT;
	if (ride->mode == RIDE_MODE_SWING)
		gGameCommandErrorTitle = STR_CANT_CHANGE_NUMBER_OF_SWINGS;
	if (
		ride->mode == RIDE_MODE_ROTATION ||
		ride->mode == RIDE_MODE_FORWARD_ROTATION ||
		ride->mode == RIDE_MODE_BACKWARD_ROTATION
	)
		gGameCommandErrorTitle = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;

	set_operating_setting(w->number, RIDE_SETTING_OPERATION_OPTION, value);
}

/**
 *
 *  rct2: 0x006B11D5
 */
static void window_ride_mode_tweak_increase(rct_window *w)
{
	rct_ride *ride = get_ride(w->number);

	uint8 maxValue = RideProperties[ride->type].max_value;
	uint8 minValue = gCheatsFastLiftHill ? 0 : RideProperties[ride->type].min_value;

	if (gCheatsFastLiftHill) {
		maxValue = 255;
	}

	uint8 increment = ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

	window_ride_mode_tweak_set(w, clamp(minValue, ride->operation_option + increment, maxValue));
}

/**
 *
 *  rct2: 0x006B120A
 */
static void window_ride_mode_tweak_decrease(rct_window *w)
{
	rct_ride *ride = get_ride(w->number);

	uint8 maxValue = RideProperties[ride->type].max_value;
	uint8 minValue = gCheatsFastLiftHill ? 0 : RideProperties[ride->type].min_value;
	if (gCheatsFastLiftHill) {
		maxValue = 255;
	}

	uint8 decrement = ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

	window_ride_mode_tweak_set(w, clamp(minValue, ride->operation_option - decrement, maxValue));
}

/**
 *
 *  rct2: 0x006B1631
 */
static void window_ride_mode_dropdown(rct_window *w, rct_widget *widget)
{
	rct_ride_entry *rideEntry;
	rct_widget *dropdownWidget;
	rct_ride *ride;
	const uint8 *availableModes, *mode;
	sint32 i, numAvailableModes;

	dropdownWidget = widget - 1;
	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	// Seek to available modes for this ride
	availableModes = ride_seek_available_modes(ride);

	// Count number of available modes
	mode = availableModes;
	numAvailableModes = -1;
	do {
		numAvailableModes++;
	} while (*(mode++) != 255);

	// Hide the last operating mode if the vehicle is not intended for it.
	if (rideEntry->flags & RIDE_ENTRY_DISABLE_LAST_OPERATING_MODE && !gCheatsShowAllOperatingModes)
		numAvailableModes--;

	// If the vehicle is not intended for them, hide those two modes (these are usually (or perhaps always) both continuous circuit modes).
	if ((rideEntry->flags & RIDE_ENTRY_DISABLE_FIRST_TWO_OPERATING_MODES) && !gCheatsShowAllOperatingModes) {
		availableModes += 2;
		numAvailableModes -= 2;
	}

	// Create dropdown list
	for (i = 0; i < numAvailableModes; i++) {
		gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[i] = RideModeNames[availableModes[i]];
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		DROPDOWN_FLAG_STAY_OPEN,
		numAvailableModes,
		widget->right - dropdownWidget->left
	);

	// Set checked item
	for (i = 0; i < numAvailableModes; i++) {
		if (ride->mode == availableModes[i]) {
			dropdown_set_checked(i, true);
		}
	}
}

/**
 *
 *  rct2: 0x006B15C0
 */
static void window_ride_load_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	sint32 i;

	dropdownWidget = widget - 1;
	rct_ride *ride = get_ride(w->number);

	for (i = 0; i < 5; i++) {
		gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[i] = VehicleLoadNames[i];
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		DROPDOWN_FLAG_STAY_OPEN,
		5,
		widget->right - dropdownWidget->left
	);

	dropdown_set_checked(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK, true);
}

/**
 *
 *  rct2: 0x006B10A7
 */
static void window_ride_operating_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	rct_ride *ride;

	ride = get_ride(w->number);

	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOAD_CHECKBOX:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_LOAD);
		break;
	case WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
		break;
	case WIDX_MINIMUM_LENGTH_CHECKBOX:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
		break;
	case WIDX_MAXIMUM_LENGTH_CHECKBOX:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
		break;
	case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
		break;
	}
}

/**
 *
 *  rct2: 0x006B1715
 */
static void window_ride_operating_resize(rct_window *w)
{
	window_set_resize(w, 316, 186, 316, 186);
}

/**
 *
 *  rct2: 0x006B10F4
 */
static void window_ride_operating_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride = get_ride(w->number);
	uint8 upper_bound, lower_bound;

	switch (widgetIndex) {
	case WIDX_MODE_TWEAK_INCREASE:
		window_ride_mode_tweak_increase(w);
		break;
	case WIDX_MODE_TWEAK_DECREASE:
		window_ride_mode_tweak_decrease(w);
		break;
	case WIDX_LIFT_HILL_SPEED_INCREASE:
		upper_bound = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
		lower_bound = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
		set_operating_setting(w->number, RIDE_SETTING_LIFT_HILL_SPEED, clamp(lower_bound, ride->lift_hill_speed + 1, upper_bound));
		break;
	case WIDX_LIFT_HILL_SPEED_DECREASE:
		upper_bound = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
		lower_bound = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
		set_operating_setting(w->number, RIDE_SETTING_LIFT_HILL_SPEED, clamp(lower_bound, ride->lift_hill_speed - 1, upper_bound));
		break;
	case WIDX_MINIMUM_LENGTH_INCREASE:
		upper_bound = 250;
		lower_bound = 0;
		set_operating_setting(w->number, RIDE_SETTING_MIN_WAITING_TIME, clamp(lower_bound, ride->min_waiting_time + 1, upper_bound));
		break;
	case WIDX_MINIMUM_LENGTH_DECREASE:
		upper_bound = 250;
		lower_bound = 0;
		set_operating_setting(w->number, RIDE_SETTING_MIN_WAITING_TIME, clamp(lower_bound, ride->min_waiting_time - 1, upper_bound));
		break;
	case WIDX_MAXIMUM_LENGTH_INCREASE:
		upper_bound = 250;
		lower_bound = 0;
		set_operating_setting(w->number, RIDE_SETTING_MAX_WAITING_TIME, clamp(lower_bound, ride->max_waiting_time + 1, upper_bound));
		break;
	case WIDX_MAXIMUM_LENGTH_DECREASE:
		upper_bound = 250;
		lower_bound = 0;
		set_operating_setting(w->number, RIDE_SETTING_MAX_WAITING_TIME, clamp(lower_bound, ride->max_waiting_time - 1, upper_bound));
		break;
	case WIDX_MODE_DROPDOWN:
		window_ride_mode_dropdown(w, widget);
		break;
	case WIDX_LOAD_DROPDOWN:
		window_ride_load_dropdown(w, widget);
		break;
	case WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE:
		upper_bound = gCheatsFastLiftHill ? 255 : 20;
		lower_bound = 1;
		set_operating_setting(w->number, RIDE_SETTING_NUM_CIRCUITS, clamp(lower_bound, ride->num_circuits + 1, upper_bound));
		break;
	case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
		upper_bound = gCheatsFastLiftHill ? 255 : 20;
		lower_bound = 1;
		set_operating_setting(w->number, RIDE_SETTING_NUM_CIRCUITS, clamp(lower_bound, ride->num_circuits - 1, upper_bound));
		break;
	}
}

/**
 *
 *  rct2: 0x006B1165
 */
static void window_ride_operating_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	rct_ride *ride;
	const uint8 *availableModes;

	if (dropdownIndex == -1)
		return;

	ride = get_ride(w->number);

	switch (widgetIndex) {
	case WIDX_MODE_DROPDOWN:
		// Seek to available modes for this ride
		availableModes = ride_seek_available_modes(ride);

		set_operating_setting(w->number, RIDE_SETTING_MODE, availableModes[dropdownIndex]);
		break;
	case WIDX_LOAD_DROPDOWN:
		set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, (ride->depart_flags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
		break;
	}
}

/**
 *
 *  rct2: 0x006B178E
 */
static void window_ride_operating_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_3);

	ride = get_ride(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_OPERATING) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_OPERATING;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006B0B30
 */
static void window_ride_operating_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	rct_ride_entry *rideEntry;
	rct_string_id format, caption, tooltip;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	// Widget setup
	w->pressed_widgets &= ~(
		(1ULL << WIDX_LOAD_CHECKBOX) |
		(1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX) |
		(1ULL << WIDX_MINIMUM_LENGTH_CHECKBOX) |
		(1ULL << WIDX_MAXIMUM_LENGTH_CHECKBOX) |
		(1ULL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX)
		);

	// Lift hill speed
	if ((rideEntry->enabledTrackPiecesA & (1UL << TRACK_LIFT_HILL)) &&
            track_piece_is_available_for_ride_type(ride->type, TRACK_LIFT_HILL)) {
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WWT_24;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WWT_DROPDOWN_BUTTON;
		set_format_arg(20, uint16, ride->lift_hill_speed);
	} else {
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WWT_EMPTY;
	}

	// Number of circuits
	if (ride_can_have_multiple_circuits(ride)) {
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WWT_24;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WWT_DROPDOWN_BUTTON;
		set_format_arg(22, uint16, ride->num_circuits);
	} else {
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WWT_EMPTY;
	}

	// Leave if another vehicle arrives at station
	if (
		ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION) &&
		ride->num_vehicles > 1 &&
		ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED &&
		ride->mode != RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED
	) {
		window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WWT_CHECKBOX;
		window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].tooltip = STR_LEAVE_IF_ANOTHER_VEHICLE_ARRIVES_TIP;
		window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].text = RideNameConvention[ride->type].vehicle == RIDE_COMPONENT_TYPE_BOAT ?
			STR_LEAVE_IF_ANOTHER_BOAT_ARRIVES :
			STR_LEAVE_IF_ANOTHER_TRAIN_ARRIVES;
	} else {
		window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WWT_EMPTY;
	}

	// Synchronise with adjacent stations
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS)) {
		window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WWT_CHECKBOX;
		window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].image = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS;
		window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].tooltip = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP;
	} else {
		window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WWT_EMPTY;
	}

	// Mode
	window_ride_operating_widgets[WIDX_MODE].text = RideModeNames[ride->mode];

	// Waiting
	window_ride_operating_widgets[WIDX_LOAD].text = VehicleLoadNames[(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK)];
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS)) {
		window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WWT_CHECKBOX;
		window_ride_operating_widgets[WIDX_LOAD].type = WWT_DROPDOWN;
		window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WWT_DROPDOWN_BUTTON;

		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WWT_CHECKBOX;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WWT_DROPDOWN_BUTTON;

		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WWT_CHECKBOX;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WWT_DROPDOWN_BUTTON;

		set_format_arg(10, rct_string_id, STR_FORMAT_SECONDS);
		set_format_arg(12, uint16, ride->min_waiting_time);
		set_format_arg(14, rct_string_id, STR_FORMAT_SECONDS);
		set_format_arg(16, uint16, ride->max_waiting_time);

		if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD)
			w->pressed_widgets |= (1 << WIDX_LOAD_CHECKBOX);
	} else {
		window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_LOAD].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WWT_EMPTY;

		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WWT_EMPTY;

		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WWT_EMPTY;
	}

	if (ride->depart_flags & RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES)
		w->pressed_widgets |= (1 << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
	if (ride->depart_flags & RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS)
		w->pressed_widgets |= (1 << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX);
	if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH)
		w->pressed_widgets |= (1 << WIDX_MINIMUM_LENGTH_CHECKBOX);
	if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH)
		w->pressed_widgets |= (1 << WIDX_MAXIMUM_LENGTH_CHECKBOX);

	// Mode specific functionality
	set_format_arg(18, uint16, ride->operation_option);
	switch (ride->mode) {
	case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
	case RIDE_MODE_POWERED_LAUNCH:
	case RIDE_MODE_UPWARD_LAUNCH:
	case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
		set_format_arg(18, uint16, (ride->launch_speed * 9) / 4);
		format = STR_RIDE_MODE_SPEED_VALUE;
		caption = STR_LAUNCH_SPEED;
		tooltip = STR_LAUNCH_SPEED_TIP;
		break;
	case RIDE_MODE_STATION_TO_STATION:
		set_format_arg(18, uint16, (ride->speed * 9) / 4);
		format = STR_RIDE_MODE_SPEED_VALUE;
		caption = STR_SPEED;
		tooltip = STR_SPEED_TIP;
		break;
	case RIDE_MODE_RACE:
		set_format_arg(18, uint16, ride->num_laps);
		format = STR_NUMBER_OF_LAPS_VALUE;
		caption = STR_NUMBER_OF_LAPS;
		tooltip = STR_NUMBER_OF_LAPS_TIP;
		break;
	case RIDE_MODE_BUMPERCAR:
		format = STR_RIDE_MODE_TIME_LIMIT_VALUE;
		caption = STR_TIME_LIMIT;
		tooltip = STR_TIME_LIMIT_TIP;
		break;
	case RIDE_MODE_SWING:
		format = STR_RIDE_MODE_NUMBER_OF_SWINGS_VALUE;
		caption = STR_NUMBER_OF_SWINGS;
		tooltip = STR_NUMBER_OF_SWINGS_TIP;
		break;
	case RIDE_MODE_ROTATION:
	case RIDE_MODE_FORWARD_ROTATION:
	case RIDE_MODE_BACKWARD_ROTATION:
		format = STR_NUMBER_OF_ROTATIONS_VALUE;
		caption = STR_NUMBER_OF_ROTATIONS;
		tooltip = STR_NUMBER_OF_ROTATIONS_TIP;
		break;
	default:
		format = STR_MAX_PEOPLE_ON_RIDE_VALUE;
		caption = STR_MAX_PEOPLE_ON_RIDE;
		tooltip = STR_MAX_PEOPLE_ON_RIDE_TIP;
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
			format = 0;
		break;
	}

	if (format != 0) {
		if (ride->type == RIDE_TYPE_TWIST) {
			uint16 arg;
			memcpy(&arg, gCommonFormatArgs + 18, sizeof(uint16));
			set_format_arg(18, uint16, arg * 3);
		}

		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WWT_24;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text = caption;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
		window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_MODE_TWEAK].text = format;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WWT_DROPDOWN_BUTTON;
		w->pressed_widgets &= ~(1 << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
	} else {
		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WWT_EMPTY;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WWT_EMPTY;
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006B1001
 */
static void window_ride_operating_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	uint16 blockSections;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = get_ride(w->number);

	// Horizontal rule between mode settings and depart settings
	gfx_fill_rect_inset(
		dpi,
		w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].left + 4,
		w->y + 103,
		w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].right - 5,
		w->y + 104,
		w->colours[1],
		INSET_RECT_FLAG_BORDER_INSET
	);

	// Number of block sections
	if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED) {
		blockSections = ride->num_block_brakes + ride->num_stations;
		gfx_draw_string_left(dpi, STR_BLOCK_SECTIONS, &blockSections, COLOUR_BLACK, w->x + 21,
							 ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED ? w->y + 89 : w->y + 61);
	}
}

#pragma endregion

#pragma region Maintenance

/**
 *
 *  rct2: 0x006B1AE4
 */
static void window_ride_locate_mechanic(rct_window *w)
{
	rct_ride *ride;
	rct_peep *mechanic;

	ride = get_ride(w->number);

	// First check if there is a mechanic assigned
	mechanic = ride_get_assigned_mechanic(ride);

	// Otherwise find the closest mechanic
	if (mechanic == NULL)
		mechanic = ride_find_closest_mechanic(ride, 1);

	if (mechanic == NULL)
		window_error_open(STR_UNABLE_TO_LOCATE_MECHANIC, STR_NONE);
	else
		window_staff_open(mechanic);
}

/**
 *
 *  rct2: 0x006B7D08
 */
static void window_ride_maintenance_draw_bar(rct_window *w, rct_drawpixelinfo *dpi, sint32 x, sint32 y, sint32 value, sint32 unk)
{
	gfx_fill_rect_inset(dpi, x, y, x + 149, y + 8, w->colours[1], INSET_RECT_F_30);
	if (unk & (1u << 31)) {
		unk &= ~(1u << 31);
		if (game_is_not_paused() && (gCurrentTicks & 8))
			return;
	}

	value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
	if (value > 2) {
		gfx_fill_rect_inset(dpi, x + 2, y + 1, x + value + 1, y + 7, unk, 0);
	}
}

/**
 *
 *  rct2: 0x006B1AAD
 */
static void window_ride_maintenance_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_LOCATE_MECHANIC:
		window_ride_locate_mechanic(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006B1D70
 */
static void window_ride_maintenance_resize(rct_window *w)
{
	window_set_resize(w, 316, 135, 316, 135);
}

/**
 *
 *  rct2: 0x006B1ACE
 */
static void window_ride_maintenance_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride = get_ride(w->number);
	rct_ride_entry *ride_type = get_ride_entry(ride->subtype);
	if (ride_type == NULL) {
		return;
	}

	rct_widget *dropdownWidget = widget;
	sint32 j, num_items;

	switch (widgetIndex) {
	case WIDX_INSPECTION_INTERVAL_DROPDOWN:
		dropdownWidget--;
		for (sint32 i = 0; i < 7; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = RideInspectionIntervalNames[i];
		}
		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			7,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(ride->inspection_interval, true);
		break;

	case WIDX_FORCE_BREAKDOWN:
		num_items = 1;
		for (j = 0; j < 3; j++) {
			if (ride_type->ride_type[j] != 0xFF)
				break;
		}
		gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[0] = STR_DEBUG_FIX_RIDE;
		for (sint32 i = 0; i < 8; i++) {
			assert(j < countof(ride_type->ride_type));
			if (RideAvailableBreakdowns[ride_type->ride_type[j]] & (uint8)(1 << i)) {
				if (i == BREAKDOWN_BRAKES_FAILURE && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
					if (ride->num_vehicles != 1)
						continue;
				}
				gDropdownItemsFormat[num_items] = STR_DROPDOWN_MENU_LABEL;
				gDropdownItemsArgs[num_items] = RideBreakdownReasonNames[i];
				num_items++;
			}
		}
		if (num_items == 1) {
			window_error_open(STR_DEBUG_NO_BREAKDOWNS_AVAILABLE, STR_NONE);
		}
		else {
			window_dropdown_show_text(
				w->x + dropdownWidget->left,
				w->y + dropdownWidget->top,
				dropdownWidget->bottom - dropdownWidget->top + 1,
				w->colours[1],
				DROPDOWN_FLAG_STAY_OPEN,
				num_items
				);

			num_items = 1;
			sint32 breakdownReason = ride->breakdown_reason_pending;
			if (breakdownReason != BREAKDOWN_NONE && (ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING)) {
				for (sint32 i = 0; i < 8; i++) {
					if (RideAvailableBreakdowns[ride_type->ride_type[j]] & (uint8)(1 << i)) {
						if (i == BREAKDOWN_BRAKES_FAILURE && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
							if (ride->num_vehicles != 1)
								continue;
						}
						if (i == breakdownReason) {
							dropdown_set_checked(num_items, true);
							break;
						}
						gDropdownItemsFormat[num_items] = STR_DROPDOWN_MENU_LABEL;
						gDropdownItemsArgs[num_items] = RideBreakdownReasonNames[i];
						num_items++;
					}
				}
			}

			if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0) {
				gDropdownItemsDisabled = (1 << 0);
			}
		}
		break;
	}

}

/**
 *
 *  rct2: 0x006B1AD9
 */
static void window_ride_maintenance_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	rct_vehicle *vehicle;
	rct_ride *ride = get_ride(w->number);
	rct_ride_entry *ride_type = get_ride_entry(ride->subtype);

	switch (widgetIndex) {
	case WIDX_INSPECTION_INTERVAL_DROPDOWN:
		gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
		game_do_command(0, (dropdownIndex << 8) | 1, 0, (5 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
		break;

	case WIDX_FORCE_BREAKDOWN:
		if (dropdownIndex == 0) {
			switch (ride->breakdown_reason_pending) {
			case BREAKDOWN_SAFETY_CUT_OUT:
				if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
					break;
				for (sint32 i = 0; i < ride->num_vehicles; ++i) {
					uint16 spriteId = ride->vehicles[i];
					do {
						vehicle = GET_VEHICLE(spriteId);
						vehicle->update_flags &= ~(
							VEHICLE_UPDATE_FLAG_BROKEN_CAR |
							VEHICLE_UPDATE_FLAG_7 |
							VEHICLE_UPDATE_FLAG_BROKEN_TRAIN
							);
					} while ((spriteId = vehicle->next_vehicle_on_train) != 0xFFFF);
				}
				break;
			case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
			case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
			case BREAKDOWN_DOORS_STUCK_CLOSED:
			case BREAKDOWN_DOORS_STUCK_OPEN:
				vehicle = &(get_sprite(ride->vehicles[ride->broken_vehicle])->vehicle);
				vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_CAR;
				break;
			case BREAKDOWN_VEHICLE_MALFUNCTION:
				vehicle = &(get_sprite(ride->vehicles[ride->broken_vehicle])->vehicle);
				vehicle->update_flags &= ~VEHICLE_UPDATE_FLAG_BROKEN_TRAIN;
				break;
			}
			ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
			window_invalidate_by_number(WC_RIDE, w->number);
			break;
		}
		if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) {
			window_error_open(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_ALREADY_BROKEN);
		}
		else if (ride->status == RIDE_STATUS_CLOSED) {
			window_error_open(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_IS_CLOSED);
		}
		else {
			sint32 j;
			for (j = 0; j < 3; j++) {
				if (ride_type->ride_type[j] != 0xFF)
					break;
			}
			sint32 i;
			sint32 num_items = 1;
			for (i = 0; i < 8; i++) {
				assert(j < countof(ride_type->ride_type));
				if (RideAvailableBreakdowns[ride_type->ride_type[j]] & (uint8)(1 << i)) {
					if (i == BREAKDOWN_BRAKES_FAILURE && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
						if (ride->num_vehicles != 1)
							continue;
					}
					if (num_items == dropdownIndex)
						break;
					num_items++;
				}
			}
			ride_prepare_breakdown(w->number, i);
		}
		break;
	}

}

/**
 *
 *  rct2: 0x006B1D37
 */
static void window_ride_maintenance_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_4);

	ride = get_ride(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAINTENANCE) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAINTENANCE;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006B17C8
 */
static void window_ride_maintenance_invalidate(rct_window *w)
{
	rct_widget *widgets;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL].text = RideInspectionIntervalNames[ride->inspection_interval];

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

	if (gConfigGeneral.debugging_tools && network_get_mode() == NETWORK_MODE_NONE) {
		window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_FLATBTN;
	}
	else {
		window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_EMPTY;

	}
}

/**
 *
 *  rct2: 0x006B1877
 */
static void window_ride_maintenance_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	rct_ride *ride = get_ride(w->number);

	// Locate mechanic button image
	rct_widget *widget = &window_ride_maintenance_widgets[WIDX_LOCATE_MECHANIC];
	sint32 x = w->x + widget->left;
	sint32 y = w->y + widget->top;
	gfx_draw_sprite(dpi, (gStaffMechanicColour << 24) | 0xA0000000 | SPR_MECHANIC, x, y, 0);

	// Inspection label
	widget = &window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL];
	x = w->x + 4;
	y = w->y + widget->top + 1;
	gfx_draw_string_left(dpi, STR_INSPECTION, NULL, COLOUR_BLACK, x, y);

	// Reliability
	widget = &window_ride_maintenance_widgets[WIDX_PAGE_BACKGROUND];
	x = w->x + widget->left + 4;
	y = w->y + widget->top + 4;

	uint16 reliability = ride->reliability >> 8;
	gfx_draw_string_left(dpi, STR_RELIABILITY_LABEL_1757, &reliability, COLOUR_BLACK, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, max(10, reliability), COLOUR_BRIGHT_GREEN);
	y += 11;

	uint16 downTime = ride->downtime;
	gfx_draw_string_left(dpi, STR_DOWN_TIME_LABEL_1889, &downTime, COLOUR_BLACK, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, downTime, COLOUR_BRIGHT_RED);
	y += 26;

	// Last inspection
	uint16 lastInspection = ride->last_inspection;

	// Use singular form for 1 minute of time or less
	rct_string_id stringId;
	if (lastInspection <= 1)
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTE;
	else if (lastInspection <= 240)
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTES;
	else
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;

	gfx_draw_string_left(dpi, stringId, &lastInspection, COLOUR_BLACK, x, y);
	y += 12;

	// Last / current breakdown
	if (ride->breakdown_reason == BREAKDOWN_NONE)
		return;

	stringId = (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ?
		STR_CURRENT_BREAKDOWN :
		STR_LAST_BREAKDOWN;
	rct_string_id breakdownMessage = RideBreakdownReasonNames[ride->breakdown_reason];
	gfx_draw_string_left(dpi, stringId, &breakdownMessage, COLOUR_BLACK, x, y);
	y += 10;

	// Mechanic status
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		rct_peep *peep;
		uint16 spriteIndex;
		
		switch (ride->mechanic_status) {
		case RIDE_MECHANIC_STATUS_CALLING:
			stringId = STR_NO_MECHANICS_ARE_HIRED_MESSAGE;
			
			FOR_ALL_STAFF(spriteIndex, peep) {
				if (peep->staff_type == STAFF_TYPE_MECHANIC) {
					stringId = STR_CALLING_MECHANIC;
					break;
				}
			}
			break;
		case RIDE_MECHANIC_STATUS_HEADING:
			stringId = STR_MEHCANIC_IS_HEADING_FOR_THE_RIDE;
			break;
		case RIDE_MECHANIC_STATUS_FIXING:
		case RIDE_MECHANIC_STATUS_HAS_FIXED_STATION_BRAKES:
			stringId = STR_MEHCANIC_IS_FIXING_THE_RIDE;
			break;
		default:
			stringId = 0;
			break;
		}

		if (stringId != 0) {
			if (stringId == STR_CALLING_MECHANIC || stringId == STR_NO_MECHANICS_ARE_HIRED_MESSAGE) {
				gfx_draw_string_left_wrapped(dpi, NULL, x + 4, y, 280, stringId, COLOUR_BLACK);
			} else {
				rct_peep *mechanicSprite = &(get_sprite(ride->mechanic)->peep);
				if (peep_is_mechanic(mechanicSprite)) {
					set_format_arg(0, rct_string_id, mechanicSprite->name_string_idx);
					set_format_arg(2, uint32, mechanicSprite->id);
					gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x + 4, y, 280, stringId, COLOUR_BLACK);
				}
			}
		}
	}
}

#pragma endregion

#pragma region Colour

const uint8 window_ride_entrance_style_list[] = {
	RIDE_ENTRANCE_STYLE_PLAIN,
	RIDE_ENTRANCE_STYLE_CANVAS_TENT,
	RIDE_ENTRANCE_STYLE_WOODEN,
	RIDE_ENTRANCE_STYLE_CASTLE_BROWN,
	RIDE_ENTRANCE_STYLE_CASTLE_GREY,
	RIDE_ENTRANCE_STYLE_LOG_CABIN,
	RIDE_ENTRANCE_STYLE_JUNGLE,
	RIDE_ENTRANCE_STYLE_CLASSICAL_ROMAN,
	RIDE_ENTRANCE_STYLE_ABSTRACT,
	RIDE_ENTRANCE_STYLE_SNOW_ICE,
	RIDE_ENTRANCE_STYLE_PAGODA,
	RIDE_ENTRANCE_STYLE_SPACE,
	RIDE_ENTRANCE_STYLE_NONE
};

static uint32 window_ride_get_colour_button_image(sint32 colour)
{
	return 0x60000000 | (colour << 19) | SPR_PALETTE_BTN;
}

static sint32 window_ride_has_track_colour(rct_ride *ride, sint32 trackColour)
{
	uint16 colourUse = RideEntranceDefinitions[ride->entrance_style].colour_use_flags;

	switch (trackColour) {
	case 0: return ((colourUse & 1) && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN);
	case 1: return ((colourUse & 2) && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL);
	case 2: return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS);
	default: return 0;
	}
}

static void window_ride_set_track_colour_scheme(rct_window *w, sint32 x, sint32 y)
{
	rct_map_element *mapElement;
	uint8 newColourScheme;
	sint32 interactionType, z, direction;

	newColourScheme = (uint8)w->ride_colour;

	rct_xy16 mapCoord = { 0 };
	get_map_coordinates_from_pos(x, y, VIEWPORT_INTERACTION_MASK_RIDE, &mapCoord.x, &mapCoord.y, &interactionType, &mapElement, NULL);
	x = mapCoord.x;
	y = mapCoord.y;

	if (interactionType != VIEWPORT_INTERACTION_ITEM_RIDE)
		return;
	if (mapElement->properties.track.ride_index != w->number)
		return;
	if ((mapElement->properties.track.colour & 3) == newColourScheme)
		return;

	z = mapElement->base_height * 8;
	direction = mapElement->type & MAP_ELEMENT_DIRECTION_MASK;
	sub_6C683D(&x, &y, &z, direction, mapElement->properties.track.type, newColourScheme, NULL, 4);
}

/**
 *
 *  rct2: 0x006B04FA
 */
static void window_ride_colour_close(rct_window *w)
{
	if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
		return;

	if (gCurrentToolWidget.window_classification != w->classification)
		return;

	if (gCurrentToolWidget.window_number != w->number)
		return;

	tool_cancel();
}

/**
 *
 *  rct2: 0x006B02A1
 */
static void window_ride_colour_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PAINT_INDIVIDUAL_AREA:
		tool_set(w, WIDX_PAINT_INDIVIDUAL_AREA, TOOL_PAINT_DOWN);
		break;
	}
}

/**
 *
 *  rct2: 0x006B0AB6
 */
static void window_ride_colour_resize(rct_window *w)
{
	window_set_resize(w, 316, 207, 316, 207);
}

/**
 *
 *  rct2: 0x006B02C6
 */
static void window_ride_colour_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	uint16 colourSchemeIndex;
	vehicle_colour vehicleColour;
	rct_widget *dropdownWidget;
	rct_ride_entry *rideEntry;
	sint32 i, numItems;
	rct_string_id stringId;

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);
	colourSchemeIndex = w->ride_colour;
	dropdownWidget = widget - 1;

	switch (widgetIndex) {
	case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = ColourSchemeNames[i];
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			4,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(colourSchemeIndex, true);
		break;
	case WIDX_TRACK_MAIN_COLOUR:
		window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour_main[colourSchemeIndex]);
		break;
	case WIDX_TRACK_ADDITIONAL_COLOUR:
		window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour_additional[colourSchemeIndex]);
		break;
	case WIDX_TRACK_SUPPORT_COLOUR:
		window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour_supports[colourSchemeIndex]);
		break;
	case WIDX_MAZE_STYLE_DROPDOWN:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = MazeOptions[i].text;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			4,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(ride->track_colour_supports[colourSchemeIndex], true);
		break;
	case WIDX_ENTRANCE_STYLE_DROPDOWN:
		gDropdownItemsChecked = 0;
		for (i = 0; i < countof(window_ride_entrance_style_list); i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = RideEntranceDefinitions[window_ride_entrance_style_list[i]].string_id;

			if (ride->entrance_style == window_ride_entrance_style_list[i]) {
				dropdown_set_checked(i, true);
			}
		}
		uint64 checked = gDropdownItemsChecked;

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			countof(window_ride_entrance_style_list),
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = checked;
		break;
	case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
		for (i = 0; i < 3; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = (RideComponentNames[RideNameConvention[ride->type].vehicle].singular << 16) | VehicleColourSchemeNames[i];
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			rideEntry->max_cars_in_train > 1 ? 3 : 2,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(ride->colour_scheme_type & 3, true);
		break;
	case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
		numItems = ride->num_vehicles;
		if ((ride->colour_scheme_type & 3) != VEHICLE_COLOUR_SCHEME_PER_TRAIN)
			numItems = ride->num_cars_per_train;

		stringId = (ride->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_TRAIN ? STR_RIDE_COLOUR_TRAIN_OPTION : STR_RIDE_COLOUR_VEHICLE_OPTION;
		for (i = 0; i < 32; i++) {
			gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
			gDropdownItemsArgs[i] = ((sint64)(i + 1) << 32) | ((RideComponentNames[RideNameConvention[ride->type].vehicle].capitalised) << 16) | stringId;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			0,
			DROPDOWN_FLAG_STAY_OPEN,
			numItems,
			widget->right - dropdownWidget->left
		);

		dropdown_set_checked(w->var_48C, true);
		break;
	case WIDX_VEHICLE_MAIN_COLOUR:
		vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);
		window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.main);
		break;
	case WIDX_VEHICLE_ADDITIONAL_COLOUR_1:
		vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);
		window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.additional_1);
		break;
	case WIDX_VEHICLE_ADDITIONAL_COLOUR_2:
		vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);
		window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.additional_2);
		break;
	}
}

/**
 *
 *  rct2: 0x006B0331
 */
static void window_ride_colour_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
		w->ride_colour = (uint16)dropdownIndex;
		window_invalidate(w);
		break;
	case WIDX_TRACK_MAIN_COLOUR:
		game_do_command(0, (0 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->ride_colour, 0);
		break;
	case WIDX_TRACK_ADDITIONAL_COLOUR:
		game_do_command(0, (1 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->ride_colour, 0);
		break;
	case WIDX_TRACK_SUPPORT_COLOUR:
		game_do_command(0, (4 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->ride_colour, 0);
		break;
	case WIDX_MAZE_STYLE_DROPDOWN:
		game_do_command(0, (4 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->ride_colour, 0);
		break;
	case WIDX_ENTRANCE_STYLE_DROPDOWN:
		game_do_command(0, (6 << 8) | 1, 0, (window_ride_entrance_style_list[dropdownIndex] << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, 0, 0);
		break;
	case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
		game_do_command(0, (5 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, 0, 0);
		w->var_48C = 0;
		break;
	case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
		w->var_48C = dropdownIndex;
		window_invalidate(w);
		break;
	case WIDX_VEHICLE_MAIN_COLOUR:
		game_do_command(0, (2 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->var_48C, 0);
		break;
	case WIDX_VEHICLE_ADDITIONAL_COLOUR_1:
		game_do_command(0, (3 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->var_48C, 0);
		break;
	case WIDX_VEHICLE_ADDITIONAL_COLOUR_2:
		game_do_command(0, (7 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, w->var_48C, 0);
		break;
	}
}

/**
 *
 *  rct2: 0x006B0A8F
 */
static void window_ride_colour_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_5);
	widget_invalidate(w, WIDX_VEHICLE_PREVIEW);
}

/**
 *
 *  rct2: 0x006B04EC
 */
static void window_ride_colour_tooldown(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
		window_ride_set_track_colour_scheme(w, x, y);
}

/**
 *
 *  rct2: 0x006B04F3
 */
static void window_ride_colour_tooldrag(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
		window_ride_set_track_colour_scheme(w, x, y);
}

/**
 *
 *  rct2: 0x006AFB36
 */
static void window_ride_colour_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride_entry *rideEntry;
	rct_ride *ride;
	track_colour trackColour;
	vehicle_colour vehicleColour;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	// Track colours
	sint32 colourScheme = w->ride_colour;
	trackColour = ride_get_track_colour(ride, colourScheme);

	// Maze style
	if (ride->type == RIDE_TYPE_MAZE) {
		window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WWT_DROPDOWN;
		window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_ride_colour_widgets[WIDX_MAZE_STYLE].text = MazeOptions[trackColour.supports].text;
	} else {
		window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WWT_EMPTY;
	}

	// Track, multiple colour schemes
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR)) {
		window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WWT_DROPDOWN;
		window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WWT_FLATBTN;
	} else {
		window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WWT_EMPTY;
	}

	// Track main colour
	if (window_ride_has_track_colour(ride, 0)) {
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_COLOURBTN;
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].image = window_ride_get_colour_button_image(trackColour.main);
	} else {
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_EMPTY;
	}

	// Track additional colour
	if (window_ride_has_track_colour(ride, 1)) {
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_COLOURBTN;
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = window_ride_get_colour_button_image(trackColour.additional);
	} else {
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_EMPTY;
	}

	// Track supports colour
	if (window_ride_has_track_colour(ride, 2) && ride->type != RIDE_TYPE_MAZE) {
		window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WWT_COLOURBTN;
		window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].image = window_ride_get_colour_button_image(trackColour.supports);
	} else {
		window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WWT_EMPTY;
	}

	// Track preview
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS))
		window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WWT_SPINNER;
	else
		window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WWT_EMPTY;

	// Entrance style
	if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ENTRANCE_EXIT) {
		window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_SPINNER;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_DROPDOWN;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;

		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].text = RideEntranceDefinitions[ride->entrance_style].string_id;
	} else {
		window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_EMPTY;
	}

	// Vehicle colours
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES) && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS)) {
		sint32 vehicleColourSchemeType = ride->colour_scheme_type & 3;
		if (vehicleColourSchemeType == 0)
			w->var_48C = 0;

		vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);

		window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WWT_SCROLL;
		window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].type = WWT_COLOURBTN;
		window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].image = window_ride_get_colour_button_image(vehicleColour.main);

		uint32 colourFlags = 0;
		for (sint32 i = 0; i < ride->num_cars_per_train; i++) {
			uint8 vehicleTypeIndex = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, i);

			colourFlags |= rideEntry->vehicles[vehicleTypeIndex].flags_b;
			colourFlags = ror32(colourFlags, 16);
			colourFlags |= rideEntry->vehicles[vehicleTypeIndex].flags_a;
			colourFlags = ror32(colourFlags, 16);
		}

		// Additional colours
		if (colourFlags & 1) {
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_COLOURBTN;
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].image = window_ride_get_colour_button_image(vehicleColour.additional_1);
			if (colourFlags & 0x2000000) {
				window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_COLOURBTN;
				window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].image = window_ride_get_colour_button_image(vehicleColour.additional_2);
			} else {
				window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_EMPTY;
			}
		} else {
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_EMPTY;
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_EMPTY;
		}

		// Vehicle colour scheme type
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16) && (ride->num_cars_per_train | ride->num_vehicles) > 1) {
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WWT_DROPDOWN;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		} else {
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WWT_EMPTY;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WWT_EMPTY;
		}
		set_format_arg( 6, rct_string_id, VehicleColourSchemeNames[vehicleColourSchemeType]);
		set_format_arg( 8, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].singular);
		set_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].capitalised);
		set_format_arg(12, uint16, w->var_48C + 1);
		// Vehicle index
		if (vehicleColourSchemeType != 0) {
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_DROPDOWN;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].text = vehicleColourSchemeType == 1 ? STR_RIDE_COLOUR_TRAIN_VALUE : STR_RIDE_COLOUR_VEHICLE_VALUE;
		} else {
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_EMPTY;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_EMPTY;
		}
	} else {
		window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_EMPTY;
	}

	set_format_arg(14, rct_string_id, ColourSchemeNames[colourScheme]);

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006AFF3E
 */
static void window_ride_colour_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	// TODO: This should use lists and identified sprites
	rct_drawpixelinfo clippedDpi;
	rct_widget *widget;
	rct_ride *ride;
	rct_ride_entry *rideEntry;

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	// Track / shop item preview
	widget = &window_ride_colour_widgets[WIDX_TRACK_PREVIEW];
	if (widget->type != WWT_EMPTY)
		gfx_fill_rect(dpi, w->x + widget->left + 1, w->y + widget->top + 1, w->x + widget->right - 1, w->y + widget->bottom - 1, PALETTE_INDEX_12);

	track_colour trackColour = ride_get_track_colour(ride, w->ride_colour);

	//
	if (rideEntry->shop_item == 0xFF) {
		sint32 x = w->x + widget->left;
		sint32 y = w->y + widget->top;

		// Track
		if (ride->type == RIDE_TYPE_MAZE) {
			sint32 spriteIndex = MazeOptions[trackColour.supports].sprite;
			gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
		} else {
			sint32 spriteIndex = TrackColourPreviews[ride->type].track;
			if (spriteIndex != 0) {
				spriteIndex |= (trackColour.additional << 24) | (trackColour.main << 19);
				spriteIndex |= 0xA0000000;
				gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
			}

			// Supports
			spriteIndex = TrackColourPreviews[ride->type].supports;
			if (spriteIndex != 0) {
				spriteIndex |= trackColour.supports << 19;
				spriteIndex |= 0x20000000;
				gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
			}
		}
	} else {
		sint32 x = w->x + (widget->left + widget->right) / 2 - 8;
		sint32 y = w->y + (widget->bottom + widget->top) / 2 - 6;

		uint8 shopItem = rideEntry->shop_item_secondary == 255 ? rideEntry->shop_item : rideEntry->shop_item_secondary;
		sint32 spriteIndex = ShopItemImage[shopItem];
		spriteIndex |= ride->track_colour_main[0] << 19;
		spriteIndex |= 0x20000000;

		gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
	}

	// Entrance preview
	trackColour = ride_get_track_colour(ride, 0);
	widget = &w->widgets[WIDX_ENTRANCE_PREVIEW];
	if (widget->type != WWT_EMPTY) {
		if (clip_drawpixelinfo(
			&clippedDpi,
			dpi,
			w->x + widget->left + 1,
			w->y + widget->top + 1,
			widget->right - widget->left,
			widget->bottom - widget->top
		)) {
			gfx_clear(&clippedDpi, PALETTE_INDEX_12);

			if (ride->entrance_style != RIDE_ENTRANCE_STYLE_NONE) {
				const rct_ride_entrance_definition *entranceStyle = &RideEntranceDefinitions[ride->entrance_style];

				sint32 terniaryColour = 0;
				if (entranceStyle->base_image_id & 0x40000000) {
					terniaryColour = 0x40000000 | (GlassPaletteIds[trackColour.main] << 19);
				}

				sint32 spriteIndex = (trackColour.additional << 24) | (trackColour.main << 19);
				spriteIndex |= 0xA0000000;
				spriteIndex += RideEntranceDefinitions[ride->entrance_style].sprite_index;

				// Back
				gfx_draw_sprite(&clippedDpi, spriteIndex, 34, 20, terniaryColour);

				// Front
				gfx_draw_sprite(&clippedDpi, spriteIndex + 4, 34, 20, terniaryColour);

				// Glass
				if (terniaryColour != 0)
					gfx_draw_sprite(&clippedDpi, ((spriteIndex + 20) & 0x7FFFF) + terniaryColour, 34, 20, terniaryColour);
			}
		}
	}
}

/**
 *
 *  rct2: 0x006B0192
 */
static void window_ride_colour_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	rct_ride *ride;
	rct_ride_entry *rideEntry;
	rct_widget *vehiclePreviewWidget;
	sint32 trainCarIndex, x, y, spriteIndex;
	vehicle_colour vehicleColour;

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);
	vehiclePreviewWidget = &window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW];
	vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);

	// Background colour
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, PALETTE_INDEX_12);

	// ?
	x = (vehiclePreviewWidget->right - vehiclePreviewWidget->left) / 2;
	y = vehiclePreviewWidget->bottom - vehiclePreviewWidget->top - 15;

	// ?
	trainCarIndex = (ride->colour_scheme_type & 3) == RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR ?
		w->var_48C : rideEntry->tab_vehicle;

	rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, trainCarIndex)];

	y += rideVehicleEntry->tab_height;

	// Draw the coloured spinning vehicle
	spriteIndex = (rideVehicleEntry->flags_a & VEHICLE_ENTRY_FLAG_A_11) ? w->frame_no / 4 : w->frame_no / 2;
	spriteIndex &= rideVehicleEntry->rotation_frame_mask;
	spriteIndex *= rideVehicleEntry->var_16;
	spriteIndex += rideVehicleEntry->base_image_id;
	spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
	spriteIndex |= 0x80000000;
	gfx_draw_sprite(dpi, spriteIndex, x, y, vehicleColour.additional_2);
}

#pragma endregion

#pragma region Music

const uint8 MusicStyleOrder[] = {
	MUSIC_STYLE_GENTLE,
	MUSIC_STYLE_SUMMER,
	MUSIC_STYLE_WATER,
	MUSIC_STYLE_RAGTIME,
	MUSIC_STYLE_TECHNO,
	MUSIC_STYLE_MECHANICAL,
	MUSIC_STYLE_MODERN,
	MUSIC_STYLE_WILD_WEST,
	MUSIC_STYLE_PIRATES,
	MUSIC_STYLE_ROCK,
	MUSIC_STYLE_ROCK_STYLE_2,
	MUSIC_STYLE_ROCK_STYLE_3,
	MUSIC_STYLE_FANTASY,
	MUSIC_STYLE_HORROR,
	MUSIC_STYLE_TOYLAND,
	MUSIC_STYLE_CANDY_STYLE,
	MUSIC_STYLE_ROMAN_FANFARE,
	MUSIC_STYLE_ORIENTAL,
	MUSIC_STYLE_MARTIAN,
	MUSIC_STYLE_SPACE,
	MUSIC_STYLE_JUNGLE_DRUMS,
	MUSIC_STYLE_JURASSIC,
	MUSIC_STYLE_EGYPTIAN,
	MUSIC_STYLE_DODGEMS_BEAT,
	MUSIC_STYLE_SNOW,
	MUSIC_STYLE_ICE,
	MUSIC_STYLE_MEDIEVAL,
	MUSIC_STYLE_URBAN,
	MUSIC_STYLE_ORGAN
};

static uint8 window_ride_current_music_style_order[42];

/**
 *
 *  rct2: 0x006B215D
 */
static void window_ride_toggle_music(rct_window *w)
{
	rct_ride *ride = get_ride(w->number);

	sint32 activateMusic = (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;

	gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (activateMusic << 8) | 1, 0, (6 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 *
 *  rct2: 0x006B1ED7
 */
static void window_ride_music_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PLAY_MUSIC:
		window_ride_toggle_music(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006AF4A2
 */
static void window_ride_music_resize(rct_window *w)
{
	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 81, 316, 81);
}

/**
 *
 *  rct2: 0x006B1EFC
 */
static void window_ride_music_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	sint32 i;

	if (widgetIndex != WIDX_MUSIC_DROPDOWN)
		return;

	dropdownWidget = widget - 1;
	rct_ride *ride = get_ride(w->number);

	sint32 numItems = 0;
	if (ride->type == RIDE_TYPE_MERRY_GO_ROUND) {
		window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_FAIRGROUND_ORGAN;
	} else {
		for (i = 0; i < countof(MusicStyleOrder); i++)
			window_ride_current_music_style_order[numItems++] = MusicStyleOrder[i];

		if (gRideMusicInfoList[36]->length != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_1;
		if (gRideMusicInfoList[37]->length != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_2;
	}

	for (i = 0; i < numItems; i++) {
		gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
		gDropdownItemsArgs[i] = MusicStyleNames[window_ride_current_music_style_order[i]];
	}

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		DROPDOWN_FLAG_STAY_OPEN,
		numItems,
		widget->right - dropdownWidget->left
	);

	for (i = 0; i < numItems; i++) {
		if (window_ride_current_music_style_order[i] == ride->music) {
			dropdown_set_checked(i, true);
		}
	}
}

/**
 *
 *  rct2: 0x006B1F03
 */
static void window_ride_music_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	uint8 musicStyle;

	if (widgetIndex != WIDX_MUSIC_DROPDOWN || dropdownIndex == -1)
		return;

	musicStyle = window_ride_current_music_style_order[dropdownIndex];
	gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (musicStyle << 8) | 1, 0, (7 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 *
 *  rct2: 0x006B2198
 */
static void window_ride_music_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x006B1DEA
 */
static void window_ride_music_invalidate(rct_window *w)
{
	rct_widget *widgets;
	sint32 isMusicActivated;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	// Set selected music
	window_ride_music_widgets[WIDX_MUSIC].text = MusicStyleNames[ride->music];

	// Set music activated
	isMusicActivated = ride->lifecycle_flags & (RIDE_LIFECYCLE_MUSIC);
	if (isMusicActivated) {
		w->pressed_widgets |= (1 << WIDX_PLAY_MUSIC);
		w->disabled_widgets &= ~(1 << WIDX_MUSIC);
		w->disabled_widgets &= ~(1 << WIDX_MUSIC_DROPDOWN);
	} else {
		w->pressed_widgets &= ~(1 << WIDX_PLAY_MUSIC);
		w->disabled_widgets |= (1 << WIDX_MUSIC);
		w->disabled_widgets |= (1 << WIDX_MUSIC_DROPDOWN);
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006B1ECC
 */
static void window_ride_music_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Measurements

/**
 *
 *  rct2: 0x006D2804
  when al == 0*/
static void cancel_scenery_selection(){
	gGamePaused &= ~GAME_PAUSED_SAVING_TRACK;
	gTrackDesignSaveMode = false;
	audio_unpause_sounds();

	rct_window* main_w = window_get_main();

	if (main_w){
		main_w->viewport->flags &= ~(VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
	}

	gfx_invalidate_screen();
	tool_cancel();
}

/**
 *
 *  rct2: 0x006D27A3
 */
static void setup_scenery_selection(rct_window* w)
{
	if (gTrackDesignSaveMode){
		cancel_scenery_selection();
	}

	while (tool_set(w, WIDX_BACKGROUND, TOOL_CROSSHAIR));

	gTrackDesignSaveRideIndex = (uint8)w->number;

	track_design_save_init();
	gGamePaused |= GAME_PAUSED_SAVING_TRACK;
	gTrackDesignSaveMode = true;

	audio_pause_sounds();

	rct_window* w_main = window_get_main();

	if (w_main){
		w_main->viewport->flags |= (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
	}

	gfx_invalidate_screen();
}

/**
 *
 *  rct2: 0x006D3026
 */
static void window_ride_measurements_design_reset()
{
	track_design_save_reset_scenery();
}

/**
 *
 *  rct2: 0x006D303D
 */
static void window_ride_measurements_design_select_nearby_scenery()
{
	track_design_save_select_nearby_scenery(gTrackDesignSaveRideIndex);
}

/**
 *
 *  rct2: 0x006AD4DA
 */
void window_ride_measurements_design_cancel()
{
	if (gTrackDesignSaveMode) {
		cancel_scenery_selection();
	}
}

/**
 *
 *  rct2: 0x006AD4CD
 */
static void window_ride_measurements_design_save(rct_window *w)
{
	track_design_save((uint8)w->number);
}

/**
 *
 *  rct2: 0x006AD4DA
 */
static void window_ride_measurements_close(rct_window *w)
{
	window_ride_measurements_design_cancel();
}

/**
 *
 *  rct2: 0x006AD478
 */
static void window_ride_measurements_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_SELECT_NEARBY_SCENERY:
		window_ride_measurements_design_select_nearby_scenery();
		break;
	case WIDX_RESET_SELECTION:
		window_ride_measurements_design_reset();
		break;
	case WIDX_SAVE_DESIGN:
		window_ride_measurements_design_save(w);
		break;
	case WIDX_CANCEL_DESIGN:
		window_ride_measurements_design_cancel();
		break;
	}
}

/**
 *
 *  rct2: 0x006AD564
 */
static void window_ride_measurements_resize(rct_window *w)
{
	window_set_resize(w, 316, 202, 316, 202);
}

/**
 *
 *  rct2: 0x006AD4AB
 */
static void window_ride_measurements_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
		return;

	gDropdownItemsFormat[0] = STR_SAVE_TRACK_DESIGN_ITEM;
	gDropdownItemsFormat[1] = STR_SAVE_TRACK_DESIGN_WITH_SCENERY_ITEM;

	window_dropdown_show_text(
		w->x + widget->left,
		w->y + widget->top,
		widget->bottom - widget->top + 1,
		w->colours[1],
		0,
		2
	);
	gDropdownDefaultIndex = 0;
	if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
		gDropdownItemsDisabled |= 2;
}

/**
 *
 *  rct2: 0x006AD4B2
 */
static void window_ride_measurements_dropdown(rct_window *w, rct_widgetindex widgetIndex, sint32 dropdownIndex)
{
	if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
		return;

	if (dropdownIndex == -1)
		dropdownIndex = gDropdownHighlightedIndex;

	if (dropdownIndex == 0)
		track_design_save((uint8)w->number);
	else
		setup_scenery_selection(w);
}

/**
 *
 *  rct2: 0x006AD5DD
 */
static void window_ride_measurements_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_7);
}

/**
 *
 *  rct2: 0x006D2AE7
 */
static void window_ride_measurements_tooldown(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	rct_map_element *mapElement;
	sint16 mapX, mapY;
	sint32 interactionType;

	_lastSceneryX = x;
	_lastSceneryY = y;
	_collectTrackDesignScenery = true; // Default to true in case user does not select anything valid

	get_map_coordinates_from_pos(x, y, 0xFCCF, &mapX, &mapY, &interactionType, &mapElement, NULL);
	switch (interactionType) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_WALL:
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		_collectTrackDesignScenery = !track_design_save_contains_map_element(mapElement);
		track_design_save_select_map_element(interactionType, mapX, mapY, mapElement, _collectTrackDesignScenery);
		break;
	}
}

static void window_ride_measurements_tooldrag(rct_window *w, rct_widgetindex widgetIndex, sint32 x, sint32 y)
{
	if (x == _lastSceneryX && y == _lastSceneryY)
		return;
	_lastSceneryX = x;
	_lastSceneryY = y;

	rct_map_element *mapElement;
	sint16 mapX, mapY;
	sint32 interactionType;

	get_map_coordinates_from_pos(x, y, 0xFCCF, &mapX, &mapY, &interactionType, &mapElement, NULL);
	switch (interactionType) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_WALL:
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		track_design_save_select_map_element(interactionType, mapX, mapY, mapElement, _collectTrackDesignScenery);
		break;
	}
}

/**
 *
 *  rct2: 0x006AD4DA
 */
static void window_ride_measurements_toolabort(rct_window *w, rct_widgetindex widgetIndex)
{
	window_ride_measurements_design_cancel();
}

/**
 *
 *  rct2: 0x006ACDBC
 */
static void window_ride_measurements_invalidate(rct_window *w)
{
	rct_widget *widgets;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
	window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WWT_EMPTY;
	if (gTrackDesignSaveMode && gTrackDesignSaveRideIndex == w->number) {
		window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WWT_DROPDOWN_BUTTON;
		window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WWT_DROPDOWN_BUTTON;
		window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WWT_DROPDOWN_BUTTON;
		window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WWT_DROPDOWN_BUTTON;
	} else {
		window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WWT_EMPTY;
		window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WWT_EMPTY;
		window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WWT_EMPTY;
		window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WWT_EMPTY;

		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)) {
			window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WWT_FLATBTN;
			w->disabled_widgets |= (1 << WIDX_SAVE_TRACK_DESIGN);
			if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED) {
				if (ride->excitement != RIDE_RATING_UNDEFINED) {
					w->disabled_widgets &= ~(1 << WIDX_SAVE_TRACK_DESIGN);
					window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN;
				}
			}
		}
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006ACF07
 */
static void window_ride_measurements_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	sint16 holes, maxSpeed, averageSpeed, drops, highestDropHeight, inversions, time;
	sint32 maxPositiveVerticalGs, maxNegativeVerticalGs, maxLateralGs, totalAirTime, length;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	if (window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type == WWT_DROPDOWN_BUTTON) {
		rct_widget *widget = &window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND];

		sint32 x = w->x + (widget->right - widget->left) / 2;
		sint32 y = w->y + widget->top + 40;
		gfx_draw_string_centred_wrapped(dpi, NULL, x, y, w->width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, COLOUR_BLACK);

		x = w->x + 4;
		y = w->y + window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
		gfx_fill_rect_inset(dpi, x, y, w->x + 312, y + 1, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);
	} else {
		rct_ride *ride = get_ride(w->number);
		sint32 x = w->x + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].left + 4;
		sint32 y = w->y + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].top + 4;

		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED) {
			// Excitement
			set_format_arg(0, uint32, ride->excitement);
			set_format_arg(4, rct_string_id, RatingNames[min(ride->excitement >> 8, 5)]);
			rct_string_id stringId = ride->excitement == RIDE_RATING_UNDEFINED ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE : STR_EXCITEMENT_RATING;
			gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
			y += 10;

			// Intensity
			set_format_arg(0, uint32, ride->intensity);
			set_format_arg(4, rct_string_id, RatingNames[min(ride->intensity >> 8, 5)]);

			stringId = STR_INTENSITY_RATING;
			if (ride->excitement == RIDE_RATING_UNDEFINED)
				stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
			else if (ride->intensity >= RIDE_RATING(10,00))
				stringId = STR_INTENSITY_RATING_RED;

			gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
			y += 10;

			// Nausea
			set_format_arg(0, uint32, ride->nausea);
			set_format_arg(4, rct_string_id, RatingNames[min(ride->nausea >> 8, 5)]);
			stringId = ride->excitement == RIDE_RATING_UNDEFINED ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
			gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
			y += 20;

			// Horizontal rule
			gfx_fill_rect_inset(dpi, x, y - 6, x + 303, y - 5, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

			if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_NO_RAW_STATS)) {
				if (ride->type == RIDE_TYPE_MINI_GOLF) {
					// Holes
					holes = ride->holes & 0x1F;
					gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
					y += 10;
				} else {
					// Max speed
					maxSpeed = (ride->max_speed * 9) >> 18;
					gfx_draw_string_left(dpi, STR_MAX_SPEED, &maxSpeed, COLOUR_BLACK, x, y);
					y += 10;

					// Average speed
					averageSpeed = (ride->average_speed * 9) >> 18;
					gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &averageSpeed, COLOUR_BLACK, x, y);
					y += 10;

					// Ride time
					sint32 numTimes = 0;
					for (sint32 i = 0; i < ride->num_stations; i++) {
						time = ride->time[numTimes];
						if (time != 0) {
							set_format_arg(0 + (numTimes * 4), uint16, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR);
							set_format_arg(2 + (numTimes * 4), uint16, time);
							numTimes++;
						}
					}
					if (numTimes == 0) {
						set_format_arg(0, rct_string_id, STR_RIDE_TIME_ENTRY);
						set_format_arg(2, uint16, 0);
						numTimes++;
					} else {
						//sadly, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
						//therefore we set the last entry to use the no-separator format now, post-format
						set_format_arg(0 + ((numTimes - 1) * 4), uint16, STR_RIDE_TIME_ENTRY);
					}
					set_format_arg(0 + (numTimes * 4), uint16, 0);
					set_format_arg(2 + (numTimes * 4), uint16, 0);
					set_format_arg(4 + (numTimes * 4), uint16, 0);
					set_format_arg(6 + (numTimes * 4), uint16, 0);
					gfx_draw_string_left_clipped(dpi, STR_RIDE_TIME, gCommonFormatArgs, COLOUR_BLACK, x, y, 308);
					y += 10;
				}

				// Ride length
				sint32 numLengths = 0;
				for (sint32 i = 0; i < ride->num_stations; i++) {
					length = ride->length[numLengths];
					if (length != 0) {
						length >>= 16;
						set_format_arg(0 + (numLengths * 4), uint16, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR);
						set_format_arg(2 + (numLengths * 4), uint16, (length & 0xFFFF));
						numLengths++;
					}
				}
				if (numLengths == 0) {
					set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
					set_format_arg(2, uint16, 0);
					numLengths++;
				} else {
					//sadly, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
					//therefore we set the last entry to use the no-separator format now, post-format
					set_format_arg(0 + ((numLengths - 1) * 4), rct_string_id, STR_RIDE_LENGTH_ENTRY);
				}
				set_format_arg(0 + (numLengths * 4), uint16, 0);
				set_format_arg(2 + (numLengths * 4), uint16, 0);
				set_format_arg(4 + (numLengths * 4), uint16, 0);
				set_format_arg(6 + (numLengths * 4), uint16, 0);
				gfx_draw_string_left_clipped(dpi, STR_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 308);
				y += 10;

				if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
					// Max. positive vertical G's
					maxPositiveVerticalGs = ride->max_positive_vertical_g;
					stringId = maxPositiveVerticalGs >= FIXED_2DP(5,00) ?
						STR_MAX_POSITIVE_VERTICAL_G_RED : STR_MAX_POSITIVE_VERTICAL_G;
					gfx_draw_string_left(dpi, stringId, &maxPositiveVerticalGs, COLOUR_BLACK, x, y);
					y += 10;

					// Max. negative vertical G's
					maxNegativeVerticalGs = ride->max_negative_vertical_g;
					stringId = maxNegativeVerticalGs <= -FIXED_2DP(2,00) ?
						STR_MAX_NEGATIVE_VERTICAL_G_RED : STR_MAX_NEGATIVE_VERTICAL_G;
					gfx_draw_string_left(dpi, stringId, &maxNegativeVerticalGs, COLOUR_BLACK, x, y);
					y += 10;

					// Max lateral G's
					maxLateralGs = ride->max_lateral_g;
					stringId = maxLateralGs >= FIXED_2DP(2,80) ?
						STR_MAX_LATERAL_G_RED : STR_MAX_LATERAL_G;
					gfx_draw_string_left(dpi, stringId, &maxLateralGs, COLOUR_BLACK, x, y);
					y += 10;

					// Total 'air' time
					totalAirTime = ride->total_air_time * 3;
					gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &totalAirTime, COLOUR_BLACK, x, y);
					y += 10;
				}

				if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
					// Drops
					drops = ride->drops & 0x3F;
					gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
					y += 10;

					// Highest drop height
					highestDropHeight = (ride->highest_drop_height * 3) / 4;
					gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &highestDropHeight, COLOUR_BLACK, x, y);
					y += 10;
				}

				if (ride->type != RIDE_TYPE_MINI_GOLF) {
					// Inversions
					inversions = ride->inversions & 0x1F;
					if (inversions != 0) {
						gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
						y += 10;
					}
				}
			}
		} else {
			gfx_draw_string_left(dpi, STR_NO_TEST_RESULTS_YET, NULL, COLOUR_BLACK, x, y);
		}
	}
}

#pragma endregion

#pragma region Graphs

enum {
	GRAPH_VELOCITY,
	GRAPH_ALTITUDE,
	GRAPH_VERTICAL,
	GRAPH_LATERAL
};

/**
 *
 *  rct2: 0x006AE8A6
 */
static void window_ride_set_graph(rct_window *w, sint32 type)
{
	if ((w->list_information_type & 0xFF) == type) {
		w->list_information_type ^= 0x8000;
	} else {
		w->list_information_type &= 0xFF00;
		w->list_information_type |= type;
	}
	window_invalidate(w);
}

/**
 *
 *  rct2: 0x006AE85D
 */
static void window_ride_graphs_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	}
}

/**
 *
 *  rct2: 0x006AE8DA
 */
static void window_ride_graphs_resize(rct_window *w)
{
	window_set_resize(w, 316, 180, 500, 450);
}

/**
 *
 *  rct2: 0x006AE878
 */
static void window_ride_graphs_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_GRAPH_VELOCITY:
		window_ride_set_graph(w, GRAPH_VELOCITY);
		break;
	case WIDX_GRAPH_ALTITUDE:
		window_ride_set_graph(w, GRAPH_ALTITUDE);
		break;
	case WIDX_GRAPH_VERTICAL:
		window_ride_set_graph(w, GRAPH_VERTICAL);
		break;
	case WIDX_GRAPH_LATERAL:
		window_ride_set_graph(w, GRAPH_LATERAL);
		break;
	}
}

/**
 *
 *  rct2: 0x006AE95D
 */
static void window_ride_graphs_update(rct_window *w)
{
	rct_widget *widget;
	rct_ride_measurement *measurement;
	sint32 x;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_8);
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_GRAPH);

	widget = &window_ride_graphs_widgets[WIDX_GRAPH];
	x = w->scrolls[0].h_left;
	if (!(w->list_information_type & 0x8000)) {
		measurement = ride_get_measurement(w->number, NULL);
		x = measurement == NULL ?
			0 :
			measurement->current_item - (((widget->right - widget->left) / 4) * 3);
	}

	w->scrolls[0].h_left = clamp(0, x, w->scrolls[0].h_right - ((widget->right - widget->left) - 2));
	widget_scroll_update_thumbs(w, WIDX_GRAPH);
}

/**
 *
 *  rct2: 0x006AEA75
 */
static void window_ride_graphs_scrollgetheight(rct_window *w, sint32 scrollIndex, sint32 *width, sint32 *height)
{
	rct_ride_measurement *measurement;

	window_event_invalidate_call(w);

	// Set minimum size
	*width = window_ride_graphs_widgets[WIDX_GRAPH].right - window_ride_graphs_widgets[WIDX_GRAPH].left - 2;

	// Get measurement size
	measurement = ride_get_measurement(w->number, NULL);
	if (measurement != NULL)
		*width = max(*width, measurement->num_items);
}

/**
 *
 *  rct2: 0x006AE953
 */
static void window_ride_graphs_15(rct_window *w, sint32 scrollIndex, sint32 scrollAreaType)
{
	w->list_information_type |= 0x8000;
}

/**
 *
 *  rct2: 0x006AEA05
 */
static void window_ride_graphs_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
	if (widgetIndex == WIDX_GRAPH) {
		set_format_arg(0, rct_string_id, STR_GRAPH);
		rct_string_id message;
		rct_ride_measurement *measurement = ride_get_measurement(w->number, &message);
		if (measurement != NULL && (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING)) {
			set_format_arg(4, uint16, measurement->vehicle_index + 1);
			rct_ride *ride = get_ride(w->number);
			set_format_arg(2, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].count);
		} else {
			*stringId = message;
		}
	} else {
		*stringId = STR_NONE;
	}
}

/**
 *
 *  rct2: 0x006AE372
 */
static void window_ride_graphs_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	sint32 x, y;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = get_ride(w->number);

	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	// Set pressed graph button type
	w->pressed_widgets &= ~(1 << WIDX_GRAPH_VELOCITY);
	w->pressed_widgets &= ~(1 << WIDX_GRAPH_ALTITUDE);
	w->pressed_widgets &= ~(1 << WIDX_GRAPH_VERTICAL);
	w->pressed_widgets &= ~(1 << WIDX_GRAPH_LATERAL);
	w->pressed_widgets |= (1LL << (WIDX_GRAPH_VELOCITY + (w->list_information_type & 0xFF)));

	// Hide graph buttons that are not applicable
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
		window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WWT_DROPDOWN_BUTTON;
		window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WWT_DROPDOWN_BUTTON;
	} else {
		window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WWT_EMPTY;
		window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WWT_EMPTY;
	}

	// Anchor graph widget
	x = w->width - 4;
	y = w->height - 18;

	window_ride_graphs_widgets[WIDX_GRAPH].right = x;
	window_ride_graphs_widgets[WIDX_GRAPH].bottom = y;
	y += 3;
	window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].top = y;
	window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].top = y;
	window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].top = y;
	window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].top = y;
	y += 11;
	window_ride_graphs_widgets[WIDX_GRAPH_VELOCITY].bottom = y;
	window_ride_graphs_widgets[WIDX_GRAPH_ALTITUDE].bottom = y;
	window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].bottom = y;
	window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].bottom = y;

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006AE4BC
 */
static void window_ride_graphs_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

/**
 *
 *  rct2: 0x006AE4C7
 */
static void window_ride_graphs_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, sint32 scrollIndex)
{
	gfx_clear(dpi, ColourMapA[COLOUR_SATURATED_GREEN].darker);

	rct_widget *widget = &window_ride_graphs_widgets[WIDX_GRAPH];
	rct_string_id stringId;
	rct_ride_measurement *measurement = ride_get_measurement(w->number, &stringId);
	if (measurement == NULL) {
		// No measurement message
		sint32 x = (widget->right - widget->left) / 2;
		sint32 y = (widget->bottom - widget->top) / 2 - 5;
		sint32 width = widget->right - widget->left - 2;
		gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, width, stringId, COLOUR_BLACK);
		return;
	}

	// Vertical grid lines
	const uint8 lightColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_light;
	const uint8 darkColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_dark;

	sint32 time = 0;
	for (sint32 x = 0; x < dpi->x + dpi->width; x += 80) {
		if (x + 80 >= dpi->x) {
			gfx_fill_rect(dpi, x +  0, dpi->y, x +  0, dpi->y + dpi->height - 1, lightColour);
			gfx_fill_rect(dpi, x + 16, dpi->y, x + 16, dpi->y + dpi->height - 1, darkColour);
			gfx_fill_rect(dpi, x + 32, dpi->y, x + 32, dpi->y + dpi->height - 1, darkColour);
			gfx_fill_rect(dpi, x + 48, dpi->y, x + 48, dpi->y + dpi->height - 1, darkColour);
			gfx_fill_rect(dpi, x + 64, dpi->y, x + 64, dpi->y + dpi->height - 1, darkColour);
		}
		time += 5;
	}

	// Horizontal grid lines
	sint32 listType = w->list_information_type & 0xFF;
	sint16 yUnit = window_graphs_y_axi[listType].unit;
	rct_string_id stringID = window_graphs_y_axi[listType].label;
	sint16 yUnitInterval = window_graphs_y_axi[listType].unit_interval;
	sint16 yInterval = window_graphs_y_axi[listType].interval;

	// Scale modifier
	if (listType == GRAPH_ALTITUDE) {
		yUnit -= gMapBaseZ * 3;
	}

	for (sint32 y = widget->bottom - widget->top - 13; y >= 8; y -= yInterval, yUnit += yUnitInterval) {
		// Minor / major line
		sint32 colour = yUnit == 0 ? lightColour : darkColour;
		gfx_fill_rect(dpi, dpi->x, y, dpi->x + dpi->width - 1, y, colour);

		sint16 scaled_yUnit = yUnit;
		// Scale modifier
		if (listType == GRAPH_ALTITUDE)
			scaled_yUnit /= 2;

		gfx_draw_string_left(dpi, stringID, &scaled_yUnit, COLOUR_BLACK, w->scrolls[0].h_left + 1, y - 4);
	}

	// Time marks
	time = 0;
	for (sint32 x = 0; x < dpi->x + dpi->width; x += 80) {
		if (x + 80 >= dpi->x)
			gfx_draw_string_left(dpi, STR_RIDE_STATS_TIME, &time, COLOUR_BLACK, x + 2, 1);
		time += 5;
	}

	// Plot
	sint32 x = dpi->x;
	sint32 top, bottom;
	for (sint32 width = 0; width < dpi->width; width++, x++) {
		if (x < 0 || x >= measurement->num_items - 1)
			continue;

		switch (listType) {
		case GRAPH_VELOCITY:
			top = measurement->velocity[x] / 2;
			bottom = measurement->velocity[x + 1] / 2;
			break;
		case GRAPH_ALTITUDE:
			top = measurement->altitude[x];
			bottom = measurement->altitude[x + 1];
			break;
		case GRAPH_VERTICAL:
			top = measurement->vertical[x] + 39;
			bottom = measurement->vertical[x + 1] + 39;
			break;
		case GRAPH_LATERAL:
			top = measurement->lateral[x] + 52;
			bottom = measurement->lateral[x + 1] + 52;
			break;
		default:
			log_error("Wrong graph type %d", listType);
			top = bottom = 0;
		}

		top = widget->bottom - widget->top - top - 13;
		bottom = widget->bottom - widget->top - bottom - 13;
		if (top > bottom) {
			sint32 tmp = top;
			top = bottom;
			bottom = tmp;
		}
		gfx_fill_rect(dpi, x, top, x, bottom, x > measurement->current_item ? PALETTE_INDEX_17 : PALETTE_INDEX_21);
	}
}

#pragma endregion

#pragma region Income

static void update_same_price_throughout_flags(uint32 shop_item)
{
	uint32 newFlags;

	if (shop_item_is_photo(shop_item)) {
		newFlags = gSamePriceThroughoutParkA;
		newFlags ^= (1 << SHOP_ITEM_PHOTO);
		game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);

		newFlags = gSamePriceThroughoutParkB;
		newFlags ^= (1 << (SHOP_ITEM_PHOTO2 - 32)) |
					(1 << (SHOP_ITEM_PHOTO3 - 32)) |
					(1 << (SHOP_ITEM_PHOTO4 - 32));
		game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
	}
	else {
		if (shop_item < 32) {
			newFlags = gSamePriceThroughoutParkA;
			newFlags ^= (1u << shop_item);
			game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
		else {
			newFlags = gSamePriceThroughoutParkB;
			newFlags ^= (1u << (shop_item - 32));
			game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
	}
}

/**
 *
 *  rct2: 0x006ADEFD
 */
static void window_ride_income_toggle_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_entry *ride_type;
	uint32 shop_item;
	money16 price;

	ride = get_ride(w->number);
	ride_type = get_ride_entry(ride->subtype);

	if (ride->type == RIDE_TYPE_TOILETS) {
		shop_item = SHOP_ITEM_ADMISSION;
	}
	else {
		shop_item = ride_type->shop_item;
		if (shop_item == 0xFFFF)
			return;
	}

	update_same_price_throughout_flags(shop_item);

	price = ride->price;
	game_do_command(0, 1, 0, w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE06E
 */
static void window_ride_income_toggle_secondary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_entry *ride_type;
	uint32 shop_item;
	money16 price;

	ride = get_ride(w->number);
	ride_type = get_ride_entry(ride->subtype);

	shop_item = ride_type->shop_item_secondary;
	if (shop_item == 0xFF)
		shop_item = RidePhotoItems[ride->type];

	update_same_price_throughout_flags(shop_item);

	price = ride->price_secondary;
	game_do_command(0, 1, 0, (1 << 8) | w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE1E4
 */
static void window_ride_income_increase_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_entry *ride_type;

	ride = get_ride(w->number);
	ride_type = get_ride_entry(ride->subtype);

	if ((gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item == 0xFF) {
			if (!gCheatsUnlockAllPrices)
				return;
		}
	}
	money16 price = ride->price;
	if (price < MONEY(20, 00))
		price++;

	game_do_command(0, 1, 0, w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE237
 */
static void window_ride_income_decrease_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_entry *ride_type;

	ride = get_ride(w->number);
	ride_type = get_ride_entry(ride->subtype);

	if ((gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item == 0xFF) {
			if (!gCheatsUnlockAllPrices)
				return;
		}
	}
	money16 price = ride->price;
	if (price > MONEY(0, 00))
		price--;

	game_do_command(0, 1, 0, w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE269
 */
static void window_ride_income_increase_secondary_price(rct_window *w)
{
	rct_ride *ride;

	ride = get_ride(w->number);

	money16 price = ride->price_secondary;
	if (price < MONEY(20, 00))
		price++;

	game_do_command(0, 1, 0, (w->number & 0x00FF) | 0x0100, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE28D
 */
static void window_ride_income_decrease_secondary_price(rct_window *w)
{
	rct_ride *ride;

	ride = get_ride(w->number);

	money16 price = ride->price_secondary;
	if (price > MONEY(0, 00))
		price--;

	game_do_command(0, 1, 0, (w->number & 0x00FF) | 0x0100, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006ADEA9
 */
static void window_ride_income_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK:
		window_ride_income_toggle_primary_price(w);
		break;
	case WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK:
		window_ride_income_toggle_secondary_price(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006AE2F8
 */
static void window_ride_income_resize(rct_window *w)
{
	window_set_resize(w, 316, 177, 316, 177);
}

/**
 *
 *  rct2: 0x006ADED4
 */
static void window_ride_income_mousedown(rct_widgetindex widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_PRIMARY_PRICE_INCREASE:
		window_ride_income_increase_primary_price(w);
		break;
	case WIDX_PRIMARY_PRICE_DECREASE:
		window_ride_income_decrease_primary_price(w);
		break;
	case WIDX_SECONDARY_PRICE_INCREASE:
		window_ride_income_increase_secondary_price(w);
		break;
	case WIDX_SECONDARY_PRICE_DECREASE:
		window_ride_income_decrease_secondary_price(w);
		break;
	}
}

/**
 *
 *  rct2: 0x006AE2BF
 */
static void window_ride_income_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_9);

	ride = get_ride(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_INCOME) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_INCOME;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006ADAA3
 */
static void window_ride_income_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride_entry *rideEntry;
	sint32 primaryItem, secondaryItem;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	rideEntry = get_ride_entry_by_ride(ride);

	// Primary item
	w->pressed_widgets &= ~(1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
	w->disabled_widgets &= ~(1 << WIDX_PRIMARY_PRICE);

	window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_NONE;
	window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_NONE;

	// If the park doesn't have free entry, lock the admission price, unless the cheat to unlock all prices is activated.
	if ((!(gParkFlags & PARK_FLAGS_PARK_FREE_ENTRY) && rideEntry->shop_item == SHOP_ITEM_NONE && ride->type != RIDE_TYPE_TOILETS)
		&& (!gCheatsUnlockAllPrices))
	{
		w->disabled_widgets |= (1 << WIDX_PRIMARY_PRICE);
		window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
		window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_RIDE_INCOME_ADMISSION_PAY_FOR_ENTRY_TIP;
	}

	window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = STR_RIDE_INCOME_ADMISSION_PRICE;
	window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = STR_SHOP_ITEM_PRICE_LABEL_ON_RIDE_PHOTO;
	window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_EMPTY;

	window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_ARG_6_CURRENCY2DP;
	money16 ridePrimaryPrice = ride_get_price(ride);
	set_format_arg(6, money32, ridePrimaryPrice);
	if (ridePrimaryPrice == 0)
		window_ride_income_widgets[WIDX_PRIMARY_PRICE].text = STR_FREE;

	primaryItem = SHOP_ITEM_ADMISSION;
	if (ride->type == RIDE_TYPE_TOILETS || ((primaryItem = rideEntry->shop_item) != SHOP_ITEM_NONE)) {
		window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;

		if (shop_item_has_common_price(primaryItem))
			w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

		window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = ShopItemStringIds[primaryItem].price_label;
	}

	// Get secondary item
	secondaryItem = RidePhotoItems[ride->type];
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)) {
		if ((secondaryItem = rideEntry->shop_item_secondary) != SHOP_ITEM_NONE) {
			window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = ShopItemStringIds[secondaryItem].price_label;
		}
	}

	if (secondaryItem == SHOP_ITEM_NONE) {
		// Hide secondary item widgets
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_EMPTY;
	} else {
		// Set same price throughout park checkbox
		w->pressed_widgets &= ~(1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
		if (shop_item_has_common_price(secondaryItem))
			w->pressed_widgets |= (1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);

		// Show widgets
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_24;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_SPINNER;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;

		// Set secondary item price
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_RIDE_SECONDARY_PRICE_VALUE;
		set_format_arg(10, money32, ride->price_secondary);
		if (ride->price_secondary == 0)
			window_ride_income_widgets[WIDX_SECONDARY_PRICE].text = STR_FREE;
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006ADCE5
 */
static void window_ride_income_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_ride_entry *rideEntry;
	rct_string_id stringId;
	money32 profit, costPerHour;
	sint32 x, y, primaryItem, secondaryItem;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = get_ride(w->number);
	rideEntry = get_ride_entry_by_ride(ride);

	x = w->x + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].top + 29;

	// Primary item profit / loss per item sold
	primaryItem = rideEntry->shop_item;
	if (primaryItem != SHOP_ITEM_NONE) {
		profit = ride->price;

		stringId = STR_PROFIT_PER_ITEM_SOLD;
		profit -= get_shop_item_cost(primaryItem);
		if (profit < 0) {
			profit *= -1;
			stringId = STR_LOSS_PER_ITEM_SOLD;
		}

		gfx_draw_string_left(dpi, stringId, &profit, COLOUR_BLACK, x, y);
	}
	y += 39;

	// Secondary item profit / loss per item sold
	secondaryItem = RidePhotoItems[ride->type];
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
		secondaryItem = rideEntry->shop_item_secondary;

	if (secondaryItem != SHOP_ITEM_NONE) {
		profit = ride->price_secondary;

		stringId = STR_PROFIT_PER_ITEM_SOLD;
		profit -= get_shop_item_cost(secondaryItem);
		if (profit < 0) {
			profit *= -1;
			stringId = STR_LOSS_PER_ITEM_SOLD;
		}

		gfx_draw_string_left(dpi, stringId, &profit, COLOUR_BLACK, x, y);
	}
	y += 15;

	// Income per hour
	if (ride->income_per_hour != MONEY32_UNDEFINED) {
		gfx_draw_string_left(dpi, STR_INCOME_PER_HOUR, &ride->income_per_hour, COLOUR_BLACK, x, y);
		y += 10;
	}

	// Running cost per hour
	costPerHour = ride->upkeep_cost * 16;
	stringId = ride->upkeep_cost == (money16)(uint16)0xFFFF ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
	gfx_draw_string_left(dpi, stringId, &costPerHour, COLOUR_BLACK, x, y);
	y += 10;

	// Profit per hour
	if (ride->profit != MONEY32_UNDEFINED) {
		gfx_draw_string_left(dpi, STR_PROFIT_PER_HOUR, &ride->profit, COLOUR_BLACK, x, y);
		y += 10;
	}
	y += 5;

	// Total profit
	gfx_draw_string_left(dpi, STR_TOTAL_PROFIT, &ride->total_profit, COLOUR_BLACK, x, y);
}

#pragma endregion

#pragma region Customer

/**
 *
 *  rct2: 0x006AD986
 */
static void window_ride_customer_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
	switch (widgetIndex) {
	case WIDX_CLOSE:
		window_close(w);
		break;
	case WIDX_TAB_1:
	case WIDX_TAB_2:
	case WIDX_TAB_3:
	case WIDX_TAB_4:
	case WIDX_TAB_5:
	case WIDX_TAB_6:
	case WIDX_TAB_7:
	case WIDX_TAB_8:
	case WIDX_TAB_9:
	case WIDX_TAB_10:
		window_ride_set_page(w, widgetIndex - WIDX_TAB_1);
		break;
	case WIDX_SHOW_GUESTS_THOUGHTS:
		window_guest_list_open_with_filter(GLFT_GUESTS_THINKING_ABOUT_RIDE, w->number);
		break;
	case WIDX_SHOW_GUESTS_ON_RIDE:
		window_guest_list_open_with_filter(GLFT_GUESTS_ON_RIDE, w->number);
		break;
	case WIDX_SHOW_GUESTS_QUEUING:
		window_guest_list_open_with_filter(GLFT_GUESTS_IN_QUEUE, w->number);
		break;
	}
}

/**
 *
 *  rct2: 0x006ADA29
 */
static void window_ride_customer_resize(rct_window *w)
{
	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 149, 316, 149);
}

/**
 *
 *  rct2: 0x006AD9DD
 */
static void window_ride_customer_update(rct_window *w)
{
	rct_ride *ride;

	w->var_492++;
	if (w->var_492 >= 24)
		w->var_492 = 0;

	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_10);

	ride = get_ride(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_CUSTOMER) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_CUSTOMER;
		window_invalidate(w);
	}
}

/**
 *
 *  rct2: 0x006AD5F8
 */
static void window_ride_customer_invalidate(rct_window *w)
{
	rct_widget *widgets;

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = get_ride(w->number);
	set_format_arg(0, rct_string_id, ride->name);
	set_format_arg(2, uint32, ride->name_arguments);

	window_ride_customer_widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WWT_FLATBTN;
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) {
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WWT_EMPTY;
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WWT_EMPTY;
	} else {
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WWT_FLATBTN;
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WWT_FLATBTN;
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 *
 *  rct2: 0x006AD6CD
 */
static void window_ride_customer_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	sint32 x, y;
	uint8 shopItem;
	sint16 popularity, satisfaction, queueTime, age;
	sint32 customersPerHour;
	rct_string_id stringId;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = get_ride(w->number);
	x = w->x + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	// Customers currently on ride
	if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
		sint16 customersOnRide = ride->num_riders;
		gfx_draw_string_left(dpi, STR_CUSTOMERS_ON_RIDE, &customersOnRide, COLOUR_BLACK, x, y);
		y += 10;
	}

	// Customers per hour
	customersPerHour = ride_customers_per_hour(ride);
	gfx_draw_string_left(dpi, STR_CUSTOMERS_PER_HOUR, &customersPerHour, COLOUR_BLACK, x, y);
	y += 10;

	// Popularity
	popularity = ride->popularity;
	if (popularity == 255) {
		stringId = STR_POPULARITY_UNKNOWN;
	} else {
		stringId = STR_POPULARITY_PERCENT;
		popularity *= 4;
	}
	gfx_draw_string_left(dpi, stringId, &popularity, COLOUR_BLACK, x, y);
	y += 10;

	// Satisfaction
	satisfaction = ride->satisfaction;
	if (satisfaction == 255) {
		stringId = STR_SATISFACTION_UNKNOWN;
	} else {
		stringId = STR_SATISFACTION_PERCENT;
		satisfaction *= 5;
	}
	gfx_draw_string_left(dpi, stringId, &satisfaction, COLOUR_BLACK, x, y);
	y += 10;

	// Queue time
	if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
		queueTime = ride_get_max_queue_time(ride);
		stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
		y += gfx_draw_string_left_wrapped(dpi, &queueTime, x, y, 308, stringId, COLOUR_BLACK);
		y += 5;
	}

	// Primary shop items sold
	shopItem = get_ride_entry_by_ride(ride)->shop_item;
	if (shopItem != SHOP_ITEM_NONE) {
		set_format_arg(0, rct_string_id, ShopItemStringIds[shopItem].plural);
		set_format_arg(2, uint32, ride->no_primary_items_sold);
		gfx_draw_string_left(dpi, STR_ITEMS_SOLD, gCommonFormatArgs, COLOUR_BLACK, x, y);
		y += 10;
	}

	// Secondary shop items sold / on-ride photos sold
	shopItem = (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ?
		RidePhotoItems[ride->type] :
		get_ride_entry_by_ride(ride)->shop_item_secondary;
	if (shopItem != SHOP_ITEM_NONE) {
		set_format_arg(0, rct_string_id, ShopItemStringIds[shopItem].plural);
		set_format_arg(2, uint32, ride->no_secondary_items_sold);
		gfx_draw_string_left(dpi, STR_ITEMS_SOLD, gCommonFormatArgs, COLOUR_BLACK, x, y);
		y += 10;
	}

	// Total customers
	gfx_draw_string_left(dpi, STR_TOTAL_CUSTOMERS, &ride->total_customers, COLOUR_BLACK, x, y);
	y += 10;

	// Guests favourite
	if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
		stringId = ride->guests_favourite == 1 ?
			STR_FAVOURITE_RIDE_OF_GUEST :
			STR_FAVOURITE_RIDE_OF_GUESTS;
		gfx_draw_string_left(dpi, stringId, &ride->guests_favourite, COLOUR_BLACK, x, y);
		y += 10;
	}
	y += 2;

	// Age
	//If the ride has a build date that is in the future, show it as built this year.
	age = max((gDateMonthsElapsed - ride->build_date) / 8, 0);
	stringId = age == 0 ?
		STR_BUILT_THIS_YEAR :
		age == 1 ?
			STR_BUILT_LAST_YEAR :
			STR_BUILT_YEARS_AGO;
	gfx_draw_string_left(dpi, stringId, &age, COLOUR_BLACK, x, y);
}

#pragma endregion
