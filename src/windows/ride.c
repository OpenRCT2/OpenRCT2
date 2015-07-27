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
#include "../config.h"
#include "../game.h"
#include "../input.h"
#include "../interface/themes.h"
#include "../interface/viewport.h"
#include "../interface/widget.h"
#include "../interface/window.h"
#include "../localisation/localisation.h"
#include "../peep/staff.h"
#include "../ride/ride.h"
#include "../ride/ride_data.h"
#include "../ride/track.h"
#include "../sprites.h"
#include "../windows/error.h"
#include "../world/map.h"
#include "../world/sprite.h"
#include "dropdown.h"
#include "../rct1.h"

#define var_496(w)	RCT2_GLOBAL((int)w + 0x496, uint16)

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

	WIDX_VEHICLE_TYPE = 14,
	WIDX_VEHICLE_TYPE_DROPDOWN,
	WIDX_VEHICLE_TRAINS_PREVIEW,
	WIDX_VEHICLE_TRAINS,
	WIDX_VEHICLE_TRAINS_DROPDOWN,
	WIDX_VEHICLE_CARS_PER_TRAIN,
	WIDX_VEHICLE_CARS_PER_TRAIN_DROPDOWN,

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

// 0x009ADC34
static rct_widget window_ride_main_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP					},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP					},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP				},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP			},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP								},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP					},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP				},

	{ WWT_VIEWPORT,			1,	3,		290,	60,		166,	0x0FFFFFFFE,					STR_NONE									},
	{ WWT_DROPDOWN,			1,	35,		256,	46,		57,		0x0FFFFFFFF,					STR_VIEW_SELECTION							},
	{ WWT_DROPDOWN_BUTTON,	1,	245,	255,	47,		56,		876,							STR_VIEW_SELECTION							},
	{ WWT_12,				1,	3,		290,	167,	177,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_FLATBTN,			1,	291,	314,	46,		69,		0x0FFFFFFFF,					STR_OPEN_CLOSE_OR_TEST_RIDE					},
	{ WWT_FLATBTN,			1,	291,	314,	70,		93,		SPR_CONSTRUCTION,				STR_CONSTRUCTION							},
	{ WWT_FLATBTN,			1,	291,	314,	94,		117,	SPR_RENAME,						STR_NAME_RIDE_TIP							},
	{ WWT_FLATBTN,			1,	291,	314,	118,	141,	SPR_LOCATE,						STR_LOCATE_SUBJECT_TIP						},
	{ WWT_FLATBTN,			1,	291,	314,	142,	165,	SPR_DEMOLISH,					STR_DEMOLISH_RIDE_TIP						},
	{ WWT_IMGBTN,			1,	296,	309,	48,		61,		SPR_G2_RCT1_CLOSE_BUTTON_0,		5293										},
	{ WWT_IMGBTN,			1,	296,	309,	62,		75,		SPR_G2_RCT1_TEST_BUTTON_0,		5294										},
	{ WWT_IMGBTN,			1,	296,	309,	76,		89,		SPR_G2_RCT1_OPEN_BUTTON_0,		5295										},
	{ WIDGETS_END },
};

// 0x009ADDA8
static rct_widget window_ride_vehicle_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,								STR_NONE										},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,										STR_WINDOW_TITLE_TIP							},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,								STR_CLOSE_WINDOW_TIP							},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,								STR_NONE										},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,									STR_VIEW_OF_RIDE_ATTRACTION_TIP					},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,									STR_VEHICLE_DETAILS_AND_OPTIONS_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,									STR_OPERATING_OPTIONS_TIP						},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,									STR_MAINTENANCE_OPTIONS_TIP						},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,									STR_COLOUR_SCHEME_OPTIONS_TIP					},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,									STR_SOUND_AND_MUSIC_OPTIONS_TIP					},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,									STR_MEASUREMENTS_AND_TEST_DATA_TIP				},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,									STR_GRAPHS_TIP									},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,									STR_INCOME_AND_COSTS_TIP						},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,									STR_CUSTOMER_INFORMATION_TIP					},

	{ WWT_DROPDOWN,			1,	7,		308,	50,		61,		0xFFFFFFFF,									STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	51,		60,		876,										STR_NONE										},
	{ WWT_SCROLL,			1,	7,		308,	141,	183,	0,											STR_NONE										},
	{ WWT_DROPDOWN,			1,	7,		151,	190,	201,	1021,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	140,	150,	191,	200,	876,										STR_NONE										},
	{ WWT_DROPDOWN,			1,	164,	308,	190,	201,	1022,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	191,	200,	876,										STR_NONE										},
	{ WIDGETS_END },
};

// 0x009ADEFC
static rct_widget window_ride_operating_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,								STR_NONE										},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,										STR_WINDOW_TITLE_TIP							},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,								STR_CLOSE_WINDOW_TIP							},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,								STR_NONE										},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,									STR_VIEW_OF_RIDE_ATTRACTION_TIP					},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,									STR_VEHICLE_DETAILS_AND_OPTIONS_TIP				},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,									STR_OPERATING_OPTIONS_TIP						},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,									STR_MAINTENANCE_OPTIONS_TIP						},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,									STR_COLOUR_SCHEME_OPTIONS_TIP					},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,									STR_SOUND_AND_MUSIC_OPTIONS_TIP					},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,									STR_MEASUREMENTS_AND_TEST_DATA_TIP				},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,									STR_GRAPHS_TIP									},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,									STR_INCOME_AND_COSTS_TIP						},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,									STR_CUSTOMER_INFORMATION_TIP					},

	{ WWT_SPINNER,			1,	157,	308,	61,		72,		874,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	62,		66,		STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	67,		71,		STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_SPINNER,			1,	157,	308,	75,		86,		3096,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	76,		80,		STR_NUMERIC_UP,								STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	81,		85,		STR_NUMERIC_DOWN,							STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WWT_CHECKBOX,			1,	7,		86,		109,	120,	STR_WAIT_FOR,								STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP	},
	{ WWT_CHECKBOX,			1,	7,		308,	124,	135,	0xFFFFFFFF,									STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		156,	139,	150,	STR_MINIMUM_WAITING_TIME,					STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP			},
	{ WWT_SPINNER,			1,	157,	308,	139,	150,	870,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	140,	144,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	145,	149,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		156,	154,	165,	STR_MAXIMUM_WAITING_TIME,					STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP			},
	{ WWT_SPINNER,			1,	157,	308,	154,	165,	872,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	155,	159,	STR_NUMERIC_UP,								STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	160,	164,	STR_NUMERIC_DOWN,							STR_NONE										},
	{ WWT_CHECKBOX,			1,	7,		308,	169,	180,	STR_SYNCHRONISE_WITH_ADJACENT_STATIONS,		STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP		},
	{ WWT_24,				1,	7,		149,	61,		72,		0xFFFFFFFF,									STR_NONE										},
	{ WWT_24,				1,	7,		149,	75,		86,		3095,										STR_NONE										},
	{ WWT_DROPDOWN,			1,	7,		308,	47,		58,		0xFFFFFFFF,									STR_SELECT_OPERATING_MODE						},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	48,		57,		876,										STR_SELECT_OPERATING_MODE						},
	{ WWT_DROPDOWN,			1,	87,		308,	109,	120,	0xFFFFFFFF,									STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	110,	119,	876,										STR_NONE										},
	{ WWT_24,				1,	7,		149,	89,		100,	STR_NUMBER_OF_CIRCUITS,						STR_NUMBER_OF_CIRCUITS_TIP						},
	{ WWT_SPINNER,			1,	157,	308,	89,		100,	1957,										STR_NONE										},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	90,		94,		STR_NUMERIC_UP,								STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	95,		99,		STR_NUMERIC_DOWN,							STR_SELECT_LIFT_HILL_CHAIN_SPEED_TIP			},
	{ WIDGETS_END },
};

// 0x009AE190
static rct_widget window_ride_maintenance_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP										},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP										},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP								},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP							},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP									},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP									},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP								},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP								},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP							},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP												},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP									},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP								},

	{ WWT_DROPDOWN,			1,	107,	308,	71,		82,		0,								STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE		},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	72,		81,		876,							STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE		},
	{ WWT_FLATBTN,			1,	289,	312,	108,	131,	0xFFFFFFFF,						STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP					},
	{ WWT_FLATBTN,			1,	265,	288,	108,	131,	SPR_NO_ENTRY,					5292														},
	{ WIDGETS_END },
};

// 0x009AE2A4
static rct_widget window_ride_colour_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP										},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP										},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP								},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP							},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP									},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP									},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP								},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP								},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP							},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP												},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP									},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP								},

	{ WWT_SPINNER,			1,	3,		70,		47,		93,		0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN,			1,	74,		312,	49,		60,		872,							STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	50,		59,		876,							STR_COLOUR_SCHEME_TO_CHANGE_TIP								},
	{ WWT_COLORBTN,			1,	79,		90,		74,		85,		0xFFFFFFFF,						STR_SELECT_MAIN_COLOUR_TIP									},
	{ WWT_COLORBTN,			1,	99,		110,	74,		85,		0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_1_TIP							},
	{ WWT_COLORBTN,			1,	119,	130,	74,		85,		0xFFFFFFFF,						STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP						},
	{ WWT_DROPDOWN,			1,	74,		312,	49,		60,		0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	50,		59,		876,							STR_NONE													},
	{ WWT_FLATBTN,			1,	289,	312,	68,		91,		5173,							STR_PAINT_INDIVIDUAL_AREA_TIP								},
	{ WWT_SPINNER,			1,	245,	312,	101,	147,	0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN,			1,	3,		241,	103,	114,	0,								STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	230,	240,	104,	113,	876,							STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP				},
	{ WWT_SCROLL,			1,	3,		70,		157,	203,	0,								STR_NONE													},
	{ WWT_DROPDOWN,			1,	74,		312,	157,	168,	868,							STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	158,	167,	876,							STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP						},
	{ WWT_DROPDOWN,			1,	74,		312,	173,	184,	0xFFFFFFFF,						STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	301,	311,	174,	183,	876,							STR_SELECT_VEHICLE_TO_MODIFY_TIP							},
	{ WWT_COLORBTN,			1,	79,		90,		190,	201,	0xFFFFFFFF,						STR_SELECT_MAIN_COLOUR_TIP									},
	{ WWT_COLORBTN,			1,	99,		110,	190,	201,	0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_1_TIP							},
	{ WWT_COLORBTN,			1,	119,	130,	190,	201,	0xFFFFFFFF,						STR_SELECT_ADDITIONAL_COLOUR_2_TIP							},
	{ WIDGETS_END },
};

// 0x009AE4C8
static rct_widget window_ride_music_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP					},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP					},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP				},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP			},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP								},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP					},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP				},

	{ WWT_CHECKBOX,			1,	7,		308,	47,		58,		STR_PLAY_MUSIC,					STR_SELECT_MUSIC_TIP						},
	{ WWT_DROPDOWN,			1,	7,		308,	62,		73,		0,								STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	63,		72,		876,							STR_SELECT_MUSIC_STYLE_TIP					},
	{ WIDGETS_END },
};

// 0x009AE5DC
static rct_widget window_ride_measurements_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP						},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP						},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE									},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP				},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP			},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP					},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP					},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP				},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP				},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP			},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP								},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP					},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP				},

	{ WWT_FLATBTN,			1,	288,	311,	164,	187,	5183,							STR_SAVE_TRACK_DESIGN						},
	{ WWT_DROPDOWN_BUTTON,	1,	4,		157,	128,	139,	STR_SELECT_NEARBY_SCENERY,		STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	158,	311,	128,	139,	STR_RESET_SELECTION,			STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	4,		157,	178,	189,	STR_DESIGN_SAVE,				STR_NONE									},
	{ WWT_DROPDOWN_BUTTON,	1,	158,	311,	178,	189,	STR_DESIGN_CANCEL,				STR_NONE									},
	{ WIDGETS_END },
};

// 0x009AE710
static rct_widget window_ride_graphs_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP										},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP										},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP								},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP							},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP									},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP									},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP								},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP								},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP							},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP												},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP									},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP								},

	{ WWT_SCROLL,			1,	3,		308,	46,		157,	1,								STR_LOGGING_DATA_FROM_TIP									},
	{ WWT_DROPDOWN_BUTTON,	1,	3,		75,		163,	176,	1415,							STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP					},
	{ WWT_DROPDOWN_BUTTON,	1,	76,		148,	163,	176,	1416,							STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP					},
	{ WWT_DROPDOWN_BUTTON,	1,	149,	221,	163,	176,	1417,							STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP	},
	{ WWT_DROPDOWN_BUTTON,	1,	222,	294,	163,	176,	1418,							STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP 	},
	{ WIDGETS_END },
};

// 0x009AE844
static rct_widget window_ride_income_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP										},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP										},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP								},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP							},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP									},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP									},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP								},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP								},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP							},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP												},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP									},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP								},

	{ WWT_24,				1,	5,		144,	50,		61,		0xFFFFFFFF,						STR_NONE													},
	{ WWT_SPINNER,			1,	147,	308,	50,		61,		1429,							STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	51,		55,		STR_NUMERIC_UP,					STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	56,		60,		STR_NUMERIC_DOWN,				STR_NONE													},
	{ WWT_CHECKBOX,			1,	5,		310,	61,		72,		STR_SAME_PRICE_THROUGHOUT_PARK,	STR_SAME_PRICE_THROUGHOUT_PARK_TIP							},
	{ WWT_24,				1,	5,		144,	89,		100,	0xFFFFFFFF,						STR_NONE													},
	{ WWT_SPINNER,			1,	147,	308,	89,		100,	1799,							STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	90,		94,		STR_NUMERIC_UP,					STR_NONE													},
	{ WWT_DROPDOWN_BUTTON,	1,	297,	307,	95,		99,		STR_NUMERIC_DOWN,				STR_NONE													},
	{ WWT_CHECKBOX,			1,	5,		310,	100,	111,	STR_SAME_PRICE_THROUGHOUT_PARK,	STR_SAME_PRICE_THROUGHOUT_PARK_TIP							},
	{ WIDGETS_END },
};

// 0x009AE9C8
static rct_widget window_ride_customer_widgets[] = {
	{ WWT_FRAME,			0,	0,		315,	0,		206,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_CAPTION,			0,	1,		314,	1,		14,		0x3DD,							STR_WINDOW_TITLE_TIP										},
	{ WWT_CLOSEBOX,			0,	303,	313,	2,		13,		STR_CLOSE_X,					STR_CLOSE_WINDOW_TIP										},
	{ WWT_RESIZE,			1,	0,		315,	43,		179,	0x0FFFFFFFF,					STR_NONE													},
	{ WWT_TAB,				1,	3,		33,		17,		43,		0x2000144E,						STR_VIEW_OF_RIDE_ATTRACTION_TIP								},
	{ WWT_TAB,				1,	34,		64,		17,		46,		0x2000144E,						STR_VEHICLE_DETAILS_AND_OPTIONS_TIP							},
	{ WWT_TAB,				1,	65,		95,		17,		43,		0x2000144E,						STR_OPERATING_OPTIONS_TIP									},
	{ WWT_TAB,				1,	96,		126,	17,		43,		0x2000144E,						STR_MAINTENANCE_OPTIONS_TIP									},
	{ WWT_TAB,				1,	127,	157,	17,		43,		0x2000144E,						STR_COLOUR_SCHEME_OPTIONS_TIP								},
	{ WWT_TAB,				1,	158,	188,	17,		43,		0x2000144E,						STR_SOUND_AND_MUSIC_OPTIONS_TIP								},
	{ WWT_TAB,				1,	189,	219,	17,		43,		0x2000144E,						STR_MEASUREMENTS_AND_TEST_DATA_TIP							},
	{ WWT_TAB,				1,	220,	250,	17,		43,		0x2000144E,						STR_GRAPHS_TIP												},
	{ WWT_TAB,				1,	251,	281,	17,		43,		0x2000144E,						STR_INCOME_AND_COSTS_TIP									},
	{ WWT_TAB,				1,	282,	312,	17,		43,		0x2000144E,						STR_CUSTOMER_INFORMATION_TIP								},

	{ WWT_FLATBTN,			1,	289,	312,	54,		77,		5184,							STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP		},
	{ WWT_FLATBTN,			1,	289,	312,	78,		101,	5186,							STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP					},
	{ WWT_FLATBTN,			1,	289,	312,	102,	125,	5185,							STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP		},
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

const uint64 window_ride_page_enabled_widgets[] = {
	0x0000000003FDBFF4,
	0x00000000001EFFF4,
	0x0000019E777DBFF4,
	0x000000000003FFF4,
	0x00000003F37F3FF4,
	0x000000000001FFF4,
	0x000000000007FFF4,
	0x000000000007BFF4,
	0x0000000000E73FF4,
	0x000000000001FFF4
};

const uint64 window_ride_page_hold_down_widgets[] = {
	0x0000000000000000,
	0x0000000000000000,
	0x00000000330D8000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000000000,
	0x0000000000630000,
	0x0000000000000000
};

#pragma endregion

#pragma region Events

static void window_ride_init_viewport(rct_window *w);

static void window_ride_main_mouseup(rct_window *w, int widgetIndex);
static void window_ride_main_resize(rct_window *w);
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_main_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput(rct_window *w, int widgetIndex, char *text);
static void window_ride_main_unknown_14(rct_window *w);
static void window_ride_main_invalidate(rct_window *w);
static void window_ride_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_vehicle_mouseup(rct_window *w, int widgetIndex);
static void window_ride_vehicle_resize(rct_window *w);
static void window_ride_vehicle_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_vehicle_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_vehicle_update(rct_window *w);
static void window_ride_vehicle_invalidate(rct_window *w);
static void window_ride_vehicle_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_vehicle_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static void window_ride_operating_mouseup(rct_window *w, int widgetIndex);
static void window_ride_operating_resize(rct_window *w);
static void window_ride_operating_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_operating_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_operating_update(rct_window *w);
static void window_ride_operating_invalidate(rct_window *w);
static void window_ride_operating_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_maintenance_mouseup(rct_window *w, int widgetIndex);
static void window_ride_maintenance_resize(rct_window *w);
static void window_ride_maintenance_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_maintenance_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_maintenance_update(rct_window *w);
static void window_ride_maintenance_invalidate(rct_window *w);
static void window_ride_maintenance_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_colour_close(rct_window *w);
static void window_ride_colour_mouseup(rct_window *w, int widgetIndex);
static void window_ride_colour_resize(rct_window *w);
static void window_ride_colour_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_colour_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_colour_update(rct_window *w);
static void window_ride_colour_tooldown(rct_window *w, int widgetIndex, int x, int y);
static void window_ride_colour_tooldrag(rct_window *w, int widgetIndex, int x, int y);
static void window_ride_colour_invalidate(rct_window *w);
static void window_ride_colour_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_colour_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static void window_ride_music_mouseup(rct_window *w, int widgetIndex);
static void window_ride_music_resize(rct_window *w);
static void window_ride_music_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_music_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_music_update(rct_window *w);
static void window_ride_music_invalidate(rct_window *w);
static void window_ride_music_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_measurements_close(rct_window *w);
static void window_ride_measurements_mouseup(rct_window *w, int widgetIndex);
static void window_ride_measurements_resize(rct_window *w);
static void window_ride_measurements_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_measurements_dropdown(rct_window *w, int widgetIndex, int dropdownIndex);
static void window_ride_measurements_update(rct_window *w);
static void window_ride_measurements_tooldown(rct_window *w, int widgetIndex, int x, int y);
static void window_ride_measurements_toolabort(rct_window *w, int widgetIndex);
static void window_ride_measurements_invalidate(rct_window *w);
static void window_ride_measurements_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_graphs_mouseup(rct_window *w, int widgetIndex);
static void window_ride_graphs_resize(rct_window *w);
static void window_ride_graphs_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_graphs_update(rct_window *w);
static void window_ride_graphs_scrollgetheight(rct_window *w, int scrollIndex, int *width, int *height);
static void window_ride_graphs_15(rct_window *w, int scrollIndex, int scrollAreaType);
static void window_ride_graphs_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId);
static void window_ride_graphs_invalidate(rct_window *w);
static void window_ride_graphs_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_graphs_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex);

static void window_ride_income_mouseup(rct_window *w, int widgetIndex);
static void window_ride_income_resize(rct_window *w);
static void window_ride_income_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_income_update(rct_window *w);
static void window_ride_income_invalidate(rct_window *w);
static void window_ride_income_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_customer_mouseup(rct_window *w, int widgetIndex);
static void window_ride_customer_resize(rct_window *w);
static void window_ride_customer_update(rct_window *w);
static void window_ride_customer_invalidate(rct_window *w);
static void window_ride_customer_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_set_colours();

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
	NULL,
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

const int window_ride_tab_animation_divisor[] = { 0, 0, 2, 2, 4, 2, 8, 8, 2, 0 };
const int window_ride_tab_animation_frames[] = { 0, 0, 4, 16, 8, 16, 8, 8, 8, 0 };

static void window_ride_draw_tab_image(rct_drawpixelinfo *dpi, rct_window *w, int page, int spriteIndex)
{
	int widgetIndex = WIDX_TAB_1 + page;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		if (w->page == page) {
			int frame = w->frame_no / window_ride_tab_animation_divisor[w->page];
			spriteIndex += (frame % window_ride_tab_animation_frames[w->page]);
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 * 
 * rct2: 0x006B2E88
 */
static void window_ride_draw_tab_main(rct_drawpixelinfo *dpi, rct_window *w)
{
	int widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_MAIN;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		int spriteIndex;
		int rideType = GET_RIDE(w->number)->type;

		switch (gRideClassifications[rideType]) {
		case RIDE_CLASS_RIDE:
			spriteIndex = 5442;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_SHOP_OR_STALL:
			spriteIndex = 5351;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_KIOSK_OR_FACILITY:
			spriteIndex = 5367;
			if (w->page == WINDOW_RIDE_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 8;
			break;
		}

		gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
	}
}

/**
 * 
 * rct2: 0x006B2B68
 */
static void window_ride_draw_tab_vehicle(rct_drawpixelinfo *dpi, rct_window *w)
{
	rct_ride *ride;
	rct_widget *widget;
	int widgetIndex, spriteIndex, x, y, width, height;
	rct_ride_type *rideEntry;
	vehicle_colour vehicleColour;

	widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_VEHICLE;
	widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		x = widget->left + 1;
		y = widget->top + 1;
		width = widget->right - x;
		height = widget->bottom - 3 - y;
		if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
			height += 4;

		x += w->x;
		y += w->y;

		dpi = clip_drawpixelinfo(dpi, x, width, y, height);
		if (dpi == NULL)
			return;

		x = (widget->right - widget->left) / 2;
		y = (widget->bottom - widget->top) - 12;

		ride = GET_RIDE(w->number);

		uint8 trainLayout[16];
		ride_entry_get_train_layout(ride->subtype, ride->num_cars_per_train, trainLayout);

		rideEntry = ride_get_entry(ride);
		if (rideEntry->flags & RIDE_ENTRY_FLAG_0) {
			dpi->zoom_level = 1;
			dpi->width *= 2;
			dpi->height *= 2;
			x *= 2;
			y *= 2;
			dpi->x *= 2;
			dpi->y *= 2;
		}

		rct_ride_type_vehicle* rideVehicleEntry = &rideEntry->vehicles[trainLayout[rideEntry->tab_vehicle]];
		height += rideVehicleEntry->var_0A;

		vehicleColour = ride_get_vehicle_colour(ride, 0);

		spriteIndex = 32;
		if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
			spriteIndex += w->frame_no;
		spriteIndex /= (rideVehicleEntry->var_12 & 0x800) ? 4 : 2;
		spriteIndex &= rideVehicleEntry->var_00;
		spriteIndex *= rideVehicleEntry->var_16;
		spriteIndex += rideVehicleEntry->base_image_id;
		spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
		spriteIndex |= 0x80000000;

		gfx_draw_sprite(dpi, spriteIndex, x, y, vehicleColour.additional_2);
		rct2_free(dpi);
	}
}

/**
 * 
 * rct2: 0x006B2F42
 */
static void window_ride_draw_tab_customer(rct_drawpixelinfo *dpi, rct_window *w)
{
	int spriteIndex;
	int widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_CUSTOMER;
	rct_widget *widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		spriteIndex = 0;
		if (w->page == WINDOW_RIDE_PAGE_CUSTOMER)
			spriteIndex = w->var_492 & ~3;

		spriteIndex += RCT2_GLOBAL(RCT2_GLOBAL(0x00982708, uint32), uint32);
		spriteIndex += 1;
		spriteIndex |= 0xA9E00000;
		
		gfx_draw_sprite(dpi, spriteIndex, w->x + (widget->left + widget->right) / 2, w->y + widget->bottom - 6, 0);
	}
}

/**
 * 
 * rct2: 0x006B2B35
 */
static void window_ride_draw_tab_images(rct_drawpixelinfo *dpi, rct_window *w)
{
	window_ride_draw_tab_vehicle(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_OPERATING, 5201);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MAINTENANCE, 5205);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_INCOME, 5253);
	window_ride_draw_tab_main(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MEASUREMENTS, 5229);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_COLOUR, 5221);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_GRAPHS, 5237);
	window_ride_draw_tab_customer(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_RIDE_PAGE_MUSIC, 5335);
}

/**
*
* rct2: 0x006AEB9F
*/
void window_ride_disable_tabs(rct_window *w)
{
	uint32 disabled_tabs = 0;
	rct_ride *ride = GET_RIDE(w->number & 0xFF);

	uint8 ride_type = ride->type; // ecx

	if ((RCT2_GLOBAL(0x97CF40 + ride_type * 8, uint32) & 0x200) == 0)
		disabled_tabs |= (1 << WIDX_TAB_8); // 0x800

	if (ride_type == RIDE_TYPE_MINI_GOLF)
		disabled_tabs |= (1 << WIDX_TAB_2 | 1 << WIDX_TAB_3 | 1 << WIDX_TAB_4); // 0xE0

	if ((RCT2_GLOBAL(0x97CF40 + ride_type * 8, uint32) & 0x2000) != 0)
		disabled_tabs |= (1 << WIDX_TAB_2); // 0x20

	if ((RCT2_GLOBAL(0x97CF40 + ride_type * 8, uint32) & 0x4000007) == 0 &&
		(RCT2_GLOBAL(0x97D4F2 + ride_type * 8, uint16) & 0x20) == 0)
		disabled_tabs |= (1 << WIDX_TAB_5); // 0x100

	if ((RCT2_GLOBAL(0x97CF40 + ride_type * 8, uint32) & 0x20000) != 0)
		disabled_tabs |= (1 << WIDX_TAB_3 | 1 << WIDX_TAB_4 | 1 << WIDX_TAB_7); // 0x4C0

	if ((RCT2_GLOBAL(0x97D4F2 + ride_type * 8, uint32) & 0x4) == 0)
		disabled_tabs |= (1 << WIDX_TAB_6); // 0x200

	if (ride_type == RIDE_TYPE_CASH_MACHINE ||
		ride_type == RIDE_TYPE_FIRST_AID ||
		(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_NO_MONEY) != 0)
		disabled_tabs |= (1 << WIDX_TAB_9); // 0x1000

	if ((RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint32) & SCREEN_FLAGS_TRACK_DESIGNER) != 0)
		disabled_tabs |= (1 << WIDX_TAB_4 | 1 << WIDX_TAB_6 | 1 << WIDX_TAB_9 | 1 << WIDX_TAB_10); // 0x3280

	rct_ride_type *type = GET_RIDE_ENTRY(ride->subtype);

	if ((type->flags & RIDE_ENTRY_FLAG_19) != 0)
		disabled_tabs |= (1 << WIDX_TAB_5); // 0x100

	w->disabled_widgets = disabled_tabs;
}

/**
 * 
 * rct2: 0x006AEAB4
 */
rct_window *window_ride_open(int rideIndex)
{
	rct_window *w;
	rct_ride *ride;
	uint8 *rideEntryIndexPtr;
	int numSubTypes, quadIndex, bitIndex;

	w = window_create_auto_pos(316, 180, window_ride_page_events[0], WC_RIDE, WF_10 | WF_RESIZABLE);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = window_ride_page_enabled_widgets[0];
	w->number = rideIndex;

	w->page = 0;
	w->var_48C = 0;
	w->frame_no = 0;
	w->list_information_type = 0;
	w->var_492 = 0;
	w->var_494 = 0;
	window_ride_disable_tabs(w);
	w->min_width = 316;
	w->min_height = 180;
	w->max_width = 500;
	w->max_height = 450;

	ride = GET_RIDE(rideIndex);
	numSubTypes = 0;
	rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(ride->type);
	for (; *rideEntryIndexPtr != 0xFF; rideEntryIndexPtr++) {
		quadIndex = *rideEntryIndexPtr >> 5;
		bitIndex = *rideEntryIndexPtr & 0x1F;
		if (!(RCT2_ADDRESS(0x01357424, uint32)[quadIndex] & (1 << bitIndex)))
			continue;

		numSubTypes++;
	}
	var_496(w) = numSubTypes;
	return w;
}

/**
 * 
 * rct2: 0x006ACC28
 */
rct_window *window_ride_main_open(int rideIndex)
{
	rct_window *w;

	w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE) {
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
		) {
			tool_cancel();
		}
	}

	w->page = WINDOW_RIDE_PAGE_MAIN;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);
	w->widgets = window_ride_page_widgets[WINDOW_RIDE_PAGE_MAIN];
	w->enabled_widgets = window_ride_page_enabled_widgets[WINDOW_RIDE_PAGE_MAIN];
	w->hold_down_widgets = window_ride_page_hold_down_widgets[WINDOW_RIDE_PAGE_MAIN];
	w->event_handlers = window_ride_page_events[WINDOW_RIDE_PAGE_MAIN];
	w->pressed_widgets = 0;
	window_ride_disable_tabs(w);
	window_init_scroll_widgets(w);
	w->ride.view = 0;
	window_ride_init_viewport(w);

	return w;
}

/**
 * 
 * rct2: 0x006ACCCE
 */
rct_window *window_ride_open_station(int rideIndex, int stationIndex)
{
	int i;
	rct_ride *ride;
	rct_window *w;

	ride = GET_RIDE(rideIndex);

	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13))
		return window_ride_main_open(rideIndex);

	w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (
		RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) & INPUT_FLAG_TOOL_ACTIVE &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) == w->classification &&
		RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) == w->number
	) {
		tool_cancel();
	}

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
	int rideIndex = mapElement->properties.track.ride_index;
	if (
		(map_element_get_type(mapElement) == MAP_ELEMENT_TYPE_ENTRANCE) ||
		(RCT2_ADDRESS(0x0099BA64, uint8)[mapElement->properties.track.type * 16] & 0x10)
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
 * rct2: 0x006ACAC2
 */
rct_window *window_ride_open_vehicle(rct_vehicle *vehicle)
{
	int i, rideIndex, view, numPeepsLeft, openedPeepWindow;
	uint16 headVehicleSpriteIndex, peepSpriteIndex;
	rct_ride *ride;
	rct_vehicle *headVehicle;
	rct_window *w, *w2;

	headVehicle = vehicle_get_head(vehicle);
	headVehicleSpriteIndex = headVehicle->sprite_index;
	rideIndex = headVehicle->ride;
	ride = GET_RIDE(rideIndex);

	// Get view index
	view = 1;
	for (i = 0; i < 32; i++) {
		if (ride->vehicles[i] == headVehicleSpriteIndex)
			break;
		
		view++;
	}


	w = window_find_by_number(WC_RIDE, rideIndex);
	if (w != NULL) {
		window_invalidate(w);

		if (
			RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint8) & INPUT_FLAG_TOOL_ACTIVE &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) == w->classification &&
			RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) == w->number
		) {
			tool_cancel();
		}

		openedPeepWindow = 0;
		if (w->ride.view == view) {
			numPeepsLeft = vehicle->num_peeps;
			for (i = 0; i < 32 && numPeepsLeft > 0; i++) {
				peepSpriteIndex = vehicle->peep[i];
				if (peepSpriteIndex == SPRITE_INDEX_NULL)
					continue;

				numPeepsLeft--;
				w2 = window_find_by_number(WC_PEEP, peepSpriteIndex);
				if (w2 == NULL) {
					rct_peep *peep = &(g_sprite_list[peepSpriteIndex].peep);
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
static void window_ride_set_page(rct_window *w, int page)
{
	int listen;

	if (RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE)
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) && w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber))
			tool_cancel();

	// Set listen only to viewport
	listen = 0;
	if (page == WINDOW_RIDE_PAGE_MAIN && w->page == WINDOW_RIDE_PAGE_MAIN && w->viewport != NULL && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
		listen++;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;

	if (page == WINDOW_RIDE_PAGE_VEHICLE) {
		ride_update_max_vehicles(w->number);
	}

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
	int i;
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
 * rct2: 0x006AF994
 */
static void window_ride_init_viewport(rct_window *w)
{
	if (w->page != WINDOW_RIDE_PAGE_MAIN) return;

	rct_ride* ride = GET_RIDE(w->number);
	int eax = w->viewport_focus_coordinates.var_480 - 1;

	union{
		sprite_focus sprite;
		coordinate_focus coordinate;
	} focus;

	focus.sprite.sprite_id = -1;
	focus.coordinate.zoom = 0;
	focus.coordinate.rotation = RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_ROTATION, uint8);
	

	if (eax >= 0 && eax < ride->num_vehicles && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK){
		focus.sprite.sprite_id = ride->vehicles[eax];

		rct_ride_type* ride_entry = ride_get_entry(ride);
		if (ride_entry->tab_vehicle != 0){
			rct_vehicle* vehicle = GET_VEHICLE(focus.sprite.sprite_id);
			focus.sprite.sprite_id = vehicle->next_vehicle_on_train;
		}
		focus.sprite.type |= 0xC0;
	}
	else if (eax >= ride->num_vehicles && eax < (ride->num_vehicles + ride->num_stations)){
		int stationIndex = -1;
		int count = eax - ride->num_vehicles;
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
		focus.coordinate.x = (ride->overall_view & 0xFF) << 5;
		focus.coordinate.y = (ride->overall_view & 0xFF00) >> 3;
		focus.coordinate.x += 16;
		focus.coordinate.y += 16;
		focus.coordinate.z = map_element_height(focus.coordinate.x, focus.coordinate.y) & 0xFFFF;
		focus.sprite.type |= 0x40;
		focus.coordinate.zoom = 1;
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_15))
			focus.coordinate.zoom = 0;
	}
	focus.coordinate.var_480 = w->viewport_focus_coordinates.var_480;

	uint16 viewport_flags = 0;

	if (w->viewport != 0){
		if (focus.coordinate.x == w->viewport_focus_coordinates.x &&
			focus.coordinate.y == w->viewport_focus_coordinates.y &&
			focus.coordinate.z == w->viewport_focus_coordinates.z &&
			focus.coordinate.rotation == w->viewport_focus_coordinates.rotation &&
			focus.coordinate.zoom == w->viewport_focus_coordinates.zoom )
			return;
		viewport_flags = w->viewport->flags;
		w->viewport->width = 0;
		w->viewport = 0;

		viewport_update_pointers();
	}
	else{
		if (RCT2_GLOBAL(RCT2_ADDRESS_CONFIG_FLAGS, uint8) & 0x1)
			viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
	}

	window_event_invalidate_call(w);

	w->viewport_focus_coordinates.x = focus.coordinate.x;
	w->viewport_focus_coordinates.y = focus.coordinate.y;
	w->viewport_focus_coordinates.z = focus.coordinate.z;
	w->viewport_focus_coordinates.rotation = focus.coordinate.rotation;
	w->viewport_focus_coordinates.zoom = focus.coordinate.zoom;

	//rct2: 0x006aec9c only used here so brought it into the function
	if (!w->viewport && ride->overall_view != 0xFFFF){
		rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

		int x = view_widget->left + 1 + w->x;
		int y = view_widget->top + 1 + w->y;
		int width = view_widget->right - view_widget->left - 1;
		int height = view_widget->bottom - view_widget->top - 1;
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

		w->flags |= WF_2;
		window_invalidate(w);
	}
	if (w->viewport){
		w->viewport->flags = viewport_flags;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006B4971
 */
void window_ride_construct(rct_window *w)
{
	window_close_by_class(WC_RIDE_CONSTRUCTION);
	ride_construct(w->number);
}

/**
 * 
 * rct2: 0x006AF315
 */
static void window_ride_rename(rct_window *w)
{
	rct_ride *ride;

	ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE962, uint32) = ride->name_arguments;
	window_text_input_open(w, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, ride->name, ride->name_arguments, 32);
}

/**
 * 
 * rct2: 0x006AF17E
 */
static void window_ride_main_mouseup(rct_window *w, int widgetIndex)
{
	rct_ride *ride;
	int status;

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
		window_ride_construct(w);
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

		ride = GET_RIDE(w->number);

		switch (widgetIndex - WIDX_CLOSE_LIGHT) {
		case 0:
			status = RIDE_STATUS_CLOSED;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1004;
			break;
		case 1:
			status = RIDE_STATUS_TESTING;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1003;
			break;
		case 2:
			status = RIDE_STATUS_OPEN;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1002;
			break;
		}

		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		ride_set_status(w->number, status);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF4A2
 */
static void window_ride_main_resize(rct_window *w)
{
	rct_viewport *viewport;
	int width, height;

	w->flags |= WF_RESIZABLE;
	int minHeight = 180;
	if (theme_get_preset()->features.rct1_ride_lights)
		minHeight = 200 + RCT1_LIGHT_OFFSET - (ride_type_has_flag(GET_RIDE(w->number)->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? 14 : 0);
	window_set_resize(w, 316, minHeight, 500, 450);

	viewport = w->viewport;
	if (viewport != NULL) {
		width = w->width - 30;
		height = w->height - 75;
		if (viewport->width != width || viewport->height != height) {
			viewport->width = width;
			viewport->height = height;
			viewport->view_width = width << viewport->zoom;
			viewport->view_height = height << viewport->zoom;
		}
	}

	window_ride_init_viewport(w);
}

/**
 * 
 * rct2: 0x006AF825
 */
static void window_ride_show_view_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	rct_ride *ride;
	int numItems, currentItem, i, j, name;

	dropdownWidget = widget - 1;
	ride = GET_RIDE(w->number);

	numItems = 1;
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13)) {
		numItems += ride->num_stations;
		numItems += ride->num_vehicles;
	}

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		numItems,
		widget->right - dropdownWidget->left
	);

	// First item
	gDropdownItemsFormat[0] = 1142;
	gDropdownItemsArgs[0] = STR_OVERALL_VIEW;
	currentItem = 1;

	// Vehicles
	name = RideNameConvention[ride->type].vehicle_name + 6;
	for (i = 1; i <= ride->num_vehicles; i++) {
		gDropdownItemsFormat[currentItem] = 1142;
		gDropdownItemsArgs[currentItem] = name | (currentItem << 16);
		currentItem++;
	}

	// Stations
	name = RideNameConvention[ride->type].station_name + 6;
	for (i = 1; i <= ride->num_stations; i++) {
		gDropdownItemsFormat[currentItem] = 1142;
		gDropdownItemsArgs[currentItem] = name | (i << 16);
		currentItem++;
	}

	// Set highlighted item
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)) {
		j = 2;
		for (i = 0; i < ride->num_vehicles; i++) {
			RCT2_GLOBAL(0x009DED34, uint32) |= j;
			j <<= 1;
		}
	}

	// Set checked item
	gDropdownItemsChecked |= (1 << w->ride.view);
}

/**
 * 
 * rct2: 0x006AF64C
 */
static void window_ride_show_open_dropdown(rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	int numItems, highlightedIndex, checkedIndex;

	ride = GET_RIDE(w->number);

	numItems = 0;
	gDropdownItemsFormat[numItems] = 1142;
	gDropdownItemsArgs[numItems] = STR_CLOSE_RIDE;
	numItems++;

	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE)) {
		gDropdownItemsFormat[numItems] = 1142;
		gDropdownItemsArgs[numItems] = STR_TEST_RIDE;
		numItems++;
	}

	gDropdownItemsFormat[numItems] = 1142;
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
		if ((ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED) || (ride->lifecycle_flags & RIDE_LIFECYCLE_11))
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

	gDropdownItemsChecked |= (1 << checkedIndex);
	RCT2_GLOBAL(0x009DEBA2, sint16) = highlightedIndex;
}

/**
 * 
 * rct2: 0x006AF1BD
 */
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		window_ride_show_view_dropdown(w, widget);
		break;
	case WIDX_OPEN:
		window_ride_show_open_dropdown(w, widget);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF300
 */
static void window_ride_main_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	rct_ride *ride;
	int status;

	switch (widgetIndex) {
	case WIDX_VIEW_DROPDOWN:
		if (dropdownIndex == -1) {
			dropdownIndex = w->ride.view;
			ride = GET_RIDE(w->number);
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
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		ride = GET_RIDE(w->number);
		if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE) && dropdownIndex != 0)
			dropdownIndex++;

		switch (dropdownIndex) {
		case 0:
			status = RIDE_STATUS_CLOSED;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1004;
			break;
		case 1:
			status = RIDE_STATUS_TESTING;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1003;
			break;
		case 2:
			status = RIDE_STATUS_OPEN;
			RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, rct_string_id) = 1002;
			break;
		}

		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->name;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		ride_set_status(w->number, status);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF40F
 */
static void window_ride_main_update(rct_window *w)
{
	rct_ride *ride;
	int vehicleIndex;
	uint16 vehicleSpriteIndex;
	rct_vehicle *vehicle;

	// Update tab animation
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_1);

	// Update status
	ride = GET_RIDE(w->number);
	if (!(ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN)) {
		if (w->ride.view == 0)
			return;

		if (w->ride.view <= ride->num_vehicles) {
			vehicleIndex = w->ride.view - 1;
			vehicleSpriteIndex = ride->vehicles[vehicleIndex];
			if (vehicleSpriteIndex == 0xFFFF)
				return;

			vehicle = &(g_sprite_list[vehicleSpriteIndex].vehicle);
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
 * rct2: 0x006AF2F9
 */
static void window_ride_main_textinput(rct_window *w, int widgetIndex, char *text)
{
	if (widgetIndex != WIDX_RENAME || text == NULL)
		return;

	ride_set_name(w->number, text);
}

/**
 * 
 * rct2: 0x006AF55A
 */
static void window_ride_main_unknown_14(rct_window *w)
{
	window_ride_init_viewport(w);
}

/**
 * 
 * rct2: 0x006AECF6
 */
static void window_ride_main_invalidate(rct_window *w)
{
	rct_widget *widgets;
	int i, height;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	w->disabled_widgets &= ~((1 << 22) | (1 << 19));
	if (ride->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK))
		w->disabled_widgets |= (1 << 22);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;
	window_ride_main_widgets[WIDX_OPEN].image = SPR_CLOSED + ride->status;

	window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RIDE_STATUS_CLOSED) * 2 + widget_is_pressed(w, WIDX_CLOSE_LIGHT);
	window_ride_main_widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0 + (ride->status == RIDE_STATUS_TESTING) * 2 + widget_is_pressed(w, WIDX_TEST_LIGHT);
	window_ride_main_widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + (ride->status == RIDE_STATUS_OPEN) * 2 + widget_is_pressed(w, WIDX_OPEN_LIGHT);

	window_ride_anchor_border_widgets(w);

	// Anchor main page specific widgets
	window_ride_main_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_ride_main_widgets[WIDX_VIEWPORT].bottom = w->height - 14;
	window_ride_main_widgets[WIDX_STATUS].right = w->width - 26;
	window_ride_main_widgets[WIDX_STATUS].top = w->height - 13;
	window_ride_main_widgets[WIDX_STATUS].bottom = w->height - 3;
	window_ride_main_widgets[WIDX_VIEW].right = w->width - 60;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = w->width - 61;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = w->width - 71;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

	if (theme_get_preset()->features.rct1_ride_lights) {
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
 * rct2: 0x006AF10A
 */
static rct_string_id window_ride_get_status_overall_view(rct_window *w, void *arguments)
{
	int formatSecondary, argument;
	rct_string_id stringId;

	ride_get_status(w->number, &formatSecondary, &argument);
	RCT2_GLOBAL((int)arguments + 0, uint16) = formatSecondary;
	RCT2_GLOBAL((int)arguments + 2, uint32) = argument;
	stringId = STR_LOSS;
	if (formatSecondary != STR_BROKEN_DOWN && formatSecondary != STR_CRASHED)
		stringId = STR_BLACK_STRING;

	return stringId;
}

/**
 * 
 * rct2: 0x006AEFEF
 */
static rct_string_id window_ride_get_status_vehicle(rct_window *w, void *arguments)
{
	rct_ride *ride;
	rct_vehicle *vehicle;
	int vehicleIndex;
	uint16 vehicleSpriteIndex;
	rct_string_id stringId;

	ride = GET_RIDE(w->number);

	vehicleIndex = w->ride.view - 1;
	vehicleSpriteIndex = ride->vehicles[vehicleIndex];
	if (vehicleSpriteIndex == 0xFFFF)
		return 0;

	vehicle = &(g_sprite_list[vehicleSpriteIndex].vehicle);
	if (vehicle->status != VEHICLE_STATUS_CRASHING && vehicle->status != VEHICLE_STATUS_CRASHED) {
		int ax = vehicle->var_36 / 4;
		if (ax == 216 || ax == 123 || ax == 9 || ax == 63 || ax == 147 || ax == 155) {
			if ((RCT2_ADDRESS(0x01357644, uint32)[ride->type] & 0x40) && vehicle->velocity == 0) {
				RCT2_GLOBAL((int)arguments + 0, uint16) = STR_STOPPED_BY_BLOCK_BRAKES;
				return 1191;
			}
		}
	}

	stringId = STR_MOVING_TO_END_OF + vehicle->status;

	// Get speed in mph
	RCT2_GLOBAL((int)arguments + 2, uint16) = (abs(vehicle->velocity) * 9) >> 18;

	if (ride->type == RIDE_TYPE_MINI_GOLF)
		return 0;

	if ((RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x100) && stringId <= 1104)
		stringId += 23;

	RCT2_GLOBAL((int)arguments + 4, uint16) = RideNameConvention[ride->type].station_name;
	RCT2_GLOBAL((int)arguments + 6, uint16) = vehicle->current_station + 1;
	if (ride->num_stations > 1)
		RCT2_GLOBAL((int)arguments + 4, uint16) += 6;

	RCT2_GLOBAL((int)arguments + 0, uint16) = stringId;
	return stringId != 1106 && stringId != 1107 ? 1191 : 1192;
}

/**
 * 
 * rct2: 0x006AEF65
 */
static rct_string_id window_ride_get_status_station(rct_window *w, void *arguments)
{
	rct_ride *ride;
	int stationIndex, count, queueLength;
	rct_string_id stringId;

	ride = GET_RIDE(w->number);

	count = w->ride.view - ride->num_vehicles - 1;
	stationIndex = -1;
	do {
		stationIndex++;
		if (ride->station_starts[stationIndex] != 0xFFFF)
			count--;
	} while (count >= 0);

	stringId = 0;

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
		queueLength = ride->queue_length[stationIndex];
		RCT2_GLOBAL((int)arguments + 2, uint16) = queueLength;
		stringId = STR_QUEUE_EMPTY;
		if (queueLength == 1)
			stringId++;
		else if (queueLength > 1)
			stringId += 2;
	}

	RCT2_GLOBAL((int)arguments + 0, uint16) = stringId;
	return 1191;
}

/**
 * 
 * rct2: 0x006AEE73
 */
static rct_string_id window_ride_get_status(rct_window *w, void *arguments)
{
	rct_ride *ride = GET_RIDE(w->number);

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
 * rct2: 0x006AEE73
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
	ride = GET_RIDE(w->number);
	stringId = STR_OVERALL_VIEW;
	if (w->ride.view != 0) {
		stringId = RideNameConvention[ride->type].vehicle_name + 6;
		if (w->ride.view > ride->num_vehicles) {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = w->ride.view - ride->num_vehicles;
			stringId = RideNameConvention[ride->type].station_name + 6;
		} else {
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = w->ride.view;
		}
	}
	RCT2_GLOBAL(0x013CE952, uint16) = stringId;

	widget = &window_ride_main_widgets[WIDX_VIEW];
	gfx_draw_string_centred(
		dpi,
		1193,
		w->x + (widget->left + widget->right - 11) / 2,
		w->y + widget->top,
		0,
		(void*)0x013CE952
	);

	// Status
	widget = &window_ride_main_widgets[WIDX_STATUS];
	gfx_draw_string_centred_clipped(
		dpi,
		window_ride_get_status(w, (void*)0x013CE952),
		(void*)0x013CE952,
		0,
		w->x + (widget->left + widget->right) / 2,
		w->y + widget->top,
		widget->right - widget->left
	);
}

#pragma endregion

#pragma region Vehicle

/**
 * 
 * rct2: 0x006B272D
 */
static void window_ride_vehicle_mouseup(rct_window *w, int widgetIndex)
{
	rct_ride *ride;

	ride = GET_RIDE(w->number);

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
 * rct2: 0x006B2ABB
 */
static void window_ride_vehicle_resize(rct_window *w)
{
	window_set_resize(w, 316, 208, 316, 208);
}

/**
 * 
 * rct2: 0x006B2748
 */
static void window_ride_vehicle_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget = widget - 1;
	rct_ride *ride;
	rct_ride_type *rideEntry, *currentRideEntry;
	rct_string_id stringId;
	int i, minCars, maxCars, cars, numItems, quadIndex, bitIndex, rideEntryIndex, selectedIndex;
	uint8 *rideEntryIndexPtr, *currentRideEntryIndex;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	switch (widgetIndex) {
	case WIDX_VEHICLE_TYPE_DROPDOWN:
		rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(ride->type);
		currentRideEntryIndex;

		selectedIndex = -1;
		numItems = 0;
		for (currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != 0xFF; currentRideEntryIndex++) {
			rideEntryIndex = *currentRideEntryIndex;
			currentRideEntry = GET_RIDE_ENTRY(rideEntryIndex);
			// Skip if vehicle has the same track type, but not same subtype, unless subtype switching is enabled
			if ((currentRideEntry->flags & (RIDE_ENTRY_FLAG_SEPARATE_RIDE | RIDE_ENTRY_FLAG_SEPARATE_RIDE_NAME)) && !gConfigInterface.select_by_track_type)
				continue;

			quadIndex = rideEntryIndex >> 5;
			bitIndex = rideEntryIndex & 0x1F;
			if (!(RCT2_ADDRESS(0x01357424, uint32)[quadIndex] & (1 << bitIndex)))
				continue;

			if (ride->subtype == rideEntryIndex)
				selectedIndex = numItems;

			gDropdownItemsFormat[numItems] = 1142;
			gDropdownItemsArgs[numItems] = (rideEntryIndex << 16) | currentRideEntry->name;

			numItems++;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			numItems,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = (1 << selectedIndex);
		break;
	case WIDX_VEHICLE_TRAINS_DROPDOWN:
		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			ride->max_trains,
			widget->right - dropdownWidget->left
		);

		stringId = RideNameConvention[ride->type].vehicle_name + 4;
		for (i = 0; i < 32; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = ((i + 1) << 16) | (i == 0 ? stringId : stringId + 1);
		}

		gDropdownItemsChecked = (1 << (ride->num_vehicles - 1));
		break;
	case WIDX_VEHICLE_CARS_PER_TRAIN_DROPDOWN:
		minCars = (ride->min_max_cars_per_train >> 4);
		maxCars = (ride->min_max_cars_per_train & 0x0F);

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			maxCars - minCars + 1,
			widget->right - dropdownWidget->left
		);

		for (i = 0; i < 12; i++) {
			cars = minCars + i;

			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = 1024;
			if (cars - rideEntry->zero_cars > 1)
				gDropdownItemsArgs[i]++;
			gDropdownItemsArgs[i] |= (cars - rideEntry->zero_cars) << 16;
		}

		gDropdownItemsChecked = (1 << (ride->num_cars_per_train - minCars));
		break;
	}
}

/**
 * 
 * rct2: 0x006B2767
 */
static void window_ride_vehicle_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;

	if (dropdownIndex == -1)
		return;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	switch (widgetIndex) {
	case WIDX_VEHICLE_TYPE_DROPDOWN:
		dropdownIndex = (gDropdownItemsArgs[dropdownIndex] >> 16) & 0xFFFF;
		ride_set_ride_entry(w->number, dropdownIndex);
		break;
	case WIDX_VEHICLE_TRAINS_DROPDOWN:
		ride_set_num_vehicles(w->number, dropdownIndex + 1);
		break;
	case WIDX_VEHICLE_CARS_PER_TRAIN_DROPDOWN:
		ride_set_num_cars_per_vehicle(w->number, rideEntry->min_cars_in_train + dropdownIndex);
		break;
	}
}

/**
 * 
 * rct2: 0x006B2AA1
 */
static void window_ride_vehicle_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_2);
}

/**
 * 
 * rct2: 0x006B222C
 */
static void window_ride_vehicle_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_string_id stringId;
	int carsPerTrain;
	
	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// Widget setup
	carsPerTrain = ride->num_cars_per_train - rideEntry->zero_cars;

	// Vehicle type
	window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].image = rideEntry->name;
	// Always show a dropdown button when changing subtypes is allowed
	if ((var_496(w) <= 1 || (rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE)) && !gConfigInterface.select_by_track_type) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].type = WWT_14;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE_DROPDOWN].type = WWT_EMPTY;
		w->enabled_widgets &= ~(1 << WIDX_VEHICLE_TYPE);
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].type = WWT_DROPDOWN;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		w->enabled_widgets |= (1 << WIDX_VEHICLE_TYPE);
	}

	// Trains
	if (rideEntry->cars_per_flat_ride > 1) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_DROPDOWN;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DROPDOWN].type = WWT_EMPTY;
	}

	// Cars per train
	if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train) {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].image = carsPerTrain > 1 ? 1023 : 1022;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_DROPDOWN;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
	} else {
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_EMPTY;
		window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DROPDOWN].type = WWT_EMPTY;
	}

	RCT2_GLOBAL(0x013CE952 + 6, uint16) = carsPerTrain;
	stringId = RideNameConvention[ride->type].vehicle_name + 4;
	if (ride->num_vehicles > 1)
		stringId++;
	RCT2_GLOBAL(0x013CE952 + 8, uint16) = stringId;
	RCT2_GLOBAL(0x013CE952 + 10, uint16) = ride->num_vehicles;

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006B23DC
 */
static void window_ride_vehicle_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_string_id stringId;
	int x, y;
	sint16 factor;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	x = w->x + 8;
	y = w->y + 64;

	// Description
	y += gfx_draw_string_left_wrapped(dpi, &rideEntry->description, x, y, 300, 1191, 0);
	y += 5;

	// Capacity
	stringId = rideEntry->description + 1;
	gfx_draw_string_left(dpi, 3142, &stringId, 0, x, y);
	y += 15;

	if ((!(rideEntry->flags & RIDE_ENTRY_FLAG_SEPARATE_RIDE) || rideTypeShouldLoseSeparateFlag(rideEntry)) && var_496(w) > 1) {
		// Excitement Factor
		factor = rideEntry->excitement_multipler;
		if (factor > 0) {
			gfx_draw_string_left(dpi, 3125, &factor, 0, x, y);
			y += 10;
		}

		// Intensity Factor
		factor = rideEntry->intensity_multipler;
		if (factor > 0) {
			gfx_draw_string_left(dpi, 3126, &factor, 0, x, y);
			y += 10;
		}

		// Nausea Factor
		factor = rideEntry->nausea_multipler;
		if (factor > 0) {
			gfx_draw_string_left(dpi, 3127, &factor, 0, x, y);
			y += 10;
		}
	}
}

typedef struct {
	short x;
	short y;
	int sprite_index;
	int tertiary_colour;
} rct_vehichle_paintinfo;

rct_vehichle_paintinfo _sprites_to_draw[144];

/**
 * 
 * rct2: 0x006B2502
 */
static void window_ride_vehicle_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_widget *widget;
	int x, y, startX, startY, i, j, vehicleColourIndex, spriteIndex;
	rct_vehichle_paintinfo *nextSpriteToDraw, *current, tmp;
	vehicle_colour vehicleColour;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	// Background
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, 12);

	uint8 trainLayout[16];
	ride_entry_get_train_layout(ride->subtype, ride->num_cars_per_train, trainLayout);

	widget = &window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
	startX = max(2, ((widget->right - widget->left) - ((ride->num_vehicles - 1) * 36)) / 2 - 25);
	startY = widget->bottom - widget->top - 4;

	rct_ride_type_vehicle* rideVehicleEntry = &rideEntry->vehicles[trainLayout[0]];
	startY += rideVehicleEntry->var_0A;

	// For each train
	for (i = 0; i < ride->num_vehicles; i++) {
		nextSpriteToDraw = _sprites_to_draw;
		x = startX;
		y = startY;

		// For each car in train
		for (j = 0; j < ride->num_cars_per_train; j++) {
			rct_ride_type_vehicle* rideVehicleEntry = &rideEntry->vehicles[trainLayout[j]];
			x += rideVehicleEntry->var_04 / 17432;
			y -= (rideVehicleEntry->var_04 / 2) / 17432;

			// Get colour of vehicle
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
			vehicleColour = ride_get_vehicle_colour(ride, vehicleColourIndex);

			spriteIndex = 16;
			if (rideVehicleEntry->var_12 & 0x800)
				spriteIndex /= 2;

			spriteIndex &= rideVehicleEntry->var_00;
			spriteIndex *= rideVehicleEntry->var_16;
			spriteIndex += rideVehicleEntry->base_image_id;
			spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
			spriteIndex |= 0x80000000;

			nextSpriteToDraw->x = x;
			nextSpriteToDraw->y = y;
			nextSpriteToDraw->sprite_index = spriteIndex;
			nextSpriteToDraw->tertiary_colour = vehicleColour.additional_2;
			nextSpriteToDraw++;

			x += rideVehicleEntry->var_04 / 17432;
			y -= (rideVehicleEntry->var_04 / 2) / 17432;
		}

		if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER) {
			tmp = *(nextSpriteToDraw - 1);
			*(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
			*(nextSpriteToDraw - 2) = tmp;
		}

		current = nextSpriteToDraw;
		while (--current >= _sprites_to_draw)
			gfx_draw_sprite(dpi, current->sprite_index, current->x, current->y, current->tertiary_colour);

		startX += 36;
	}
}

#pragma endregion

#pragma region Operating

static void set_operating_setting(int rideNumber, uint8 setting, uint8 value)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (value << 8) | 1, 0, (setting << 8) | rideNumber, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

static void window_ride_mode_tweak_set(rct_window *w, uint8 value)
{
	rct_ride *ride = GET_RIDE(w->number);

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1362;
	if (ride->mode == RIDE_MODE_STATION_TO_STATION)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1361;
	if (ride->mode == RIDE_MODE_RACE)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1738;
	if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13))
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1746;
	if (ride->mode == RIDE_MODE_BUMPERCAR)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1751;
	if (ride->mode == RIDE_MODE_SWING)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1768;
	if (ride->mode == RIDE_MODE_ROTATION)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1868;
	if (
		ride->mode == RIDE_MODE_ROTATION ||
		ride->mode == RIDE_MODE_FORWARD_ROTATION ||
		ride->mode == RIDE_MODE_BACKWARD_ROTATION
	)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1868;

	game_do_command(0, (value << 8) | 1, 0, (4 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 * 
 * rct2: 0x006B11D5
 */
static void window_ride_mode_tweak_increase(rct_window *w)
{
	rct_ride *ride = GET_RIDE(w->number);
	uint8 value = ride->operation_option;
	//fast_lift_hill is the cheat that allows maxing out many limits on the Operating tab.
	uint8 max_value = gConfigCheat.fast_lift_hill ? 255 : RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8) + 5, uint8);

	//Allow 64 people in mazes under non-cheat settings. The old maximum of 16 was too little for even moderately big mazes.
	if(ride->mode == RIDE_MODE_MAZE && !gConfigCheat.fast_lift_hill)
		max_value = 64;

	if (value < max_value)
		value += ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

	window_ride_mode_tweak_set(w, value);
}

/**
 * 
 * rct2: 0x006B120A
 */
static void window_ride_mode_tweak_decrease(rct_window *w)
{
	rct_ride *ride = GET_RIDE(w->number);
	uint8 value = ride->operation_option;
	if (value > RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8) + 4, uint8))
		value -= ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

	window_ride_mode_tweak_set(w, value);
}

/**
 * 
 * rct2: 0x006B1631
 */
static void window_ride_mode_dropdown(rct_window *w, rct_widget *widget)
{
	rct_ride_type *rideEntry;
	rct_widget *dropdownWidget;
	rct_ride *ride;
	const uint8 *availableModes, *mode;
	int i, numAvailableModes;

	dropdownWidget = widget - 1;
	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	// Seek to available modes for this ride
	availableModes = RideAvailableModes;
	for (i = 0; i < ride->type; i++) {
		while (*(availableModes++) != 255) { }
	}

	// Count number of available modes
	mode = availableModes;
	numAvailableModes = -1;	
	do {
		numAvailableModes++;
	} while (*(mode++) != 255);

	// ?
	if (rideEntry->flags & RIDE_ENTRY_FLAG_15)
		numAvailableModes--;

	// ?
	if (rideEntry->flags & RIDE_ENTRY_FLAG_17) {
		availableModes += 2;
		numAvailableModes -= 2;
	}

	// Create dropdown list
	for (i = 0; i < numAvailableModes; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_RIDE_MODE_START + availableModes[i];
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		DROPDOWN_FLAG_STAY_OPEN,
		numAvailableModes,
		widget->right - dropdownWidget->left
	);

	// Set checked item
	for (i = 0; i < numAvailableModes; i++)
		if (ride->mode == availableModes[i])
			gDropdownItemsChecked = 1 << i;
}

/**
 * 
 * rct2: 0x006B15C0
 */
static void window_ride_load_dropdown(rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	int i;

	dropdownWidget = widget - 1;
	rct_ride *ride = GET_RIDE(w->number);

	for (i = 0; i < 5; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_QUARTER_LOAD + i;
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		DROPDOWN_FLAG_STAY_OPEN,
		5,
		widget->right - dropdownWidget->left
	);

	gDropdownItemsChecked = (1 << (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK));
}

/**
 * 
 * rct2: 0x006B10A7
 */
static void window_ride_operating_mouseup(rct_window *w, int widgetIndex)
{
	rct_ride *ride;

	ride = GET_RIDE(w->number);

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
		set_operating_setting(w->number, 1, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_LOAD);
		break;
	case WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX:
		set_operating_setting(w->number, 1, ride->depart_flags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
		break;
	case WIDX_MINIMUM_LENGTH_CHECKBOX:
		set_operating_setting(w->number, 1, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
		break;
	case WIDX_MAXIMUM_LENGTH_CHECKBOX:
		set_operating_setting(w->number, 1, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
		break;
	case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
		set_operating_setting(w->number, 1, ride->depart_flags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
		break;
	}
}

/**
 * 
 * rct2: 0x006B1715
 */
static void window_ride_operating_resize(rct_window *w)
{
	window_set_resize(w, 316, 186, 316, 186);
}

/**
 * 
 * rct2: 0x006B10F4
 */
static void window_ride_operating_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride = GET_RIDE(w->number);
	uint8 max_lift_hill_speed;

	switch (widgetIndex) {
	case WIDX_MODE_TWEAK_INCREASE:
		window_ride_mode_tweak_increase(w);
		break;
	case WIDX_MODE_TWEAK_DECREASE:
		window_ride_mode_tweak_decrease(w);
		break;
	case WIDX_LIFT_HILL_SPEED_INCREASE:

		if(gConfigCheat.fast_lift_hill)
			max_lift_hill_speed = 255;
		else
			max_lift_hill_speed = RCT2_GLOBAL(0x0097D7CA + (ride->type * 4), uint8);

		set_operating_setting(w->number, 8, min(ride->lift_hill_speed + 1, max_lift_hill_speed));
		break;
	case WIDX_LIFT_HILL_SPEED_DECREASE:
		set_operating_setting(w->number, 8, max(RCT2_GLOBAL(0x0097D7C9 + (ride->type * 4), uint8), ride->lift_hill_speed - 1));
		break;
	case WIDX_MINIMUM_LENGTH_INCREASE:
		set_operating_setting(w->number, 2, min(ride->min_waiting_time + 1, 250));
		break;
	case WIDX_MINIMUM_LENGTH_DECREASE:
		set_operating_setting(w->number, 2, max(0, ride->min_waiting_time - 1));
		break;
	case WIDX_MAXIMUM_LENGTH_INCREASE:
		set_operating_setting(w->number, 3, min(ride->max_waiting_time + 1, 250));
		break;
	case WIDX_MAXIMUM_LENGTH_DECREASE:
		set_operating_setting(w->number, 3, max(0, ride->max_waiting_time - 1));
		break;
	case WIDX_MODE_DROPDOWN:
		window_ride_mode_dropdown(w, widget);
		break;
	case WIDX_LOAD_DROPDOWN:
		window_ride_load_dropdown(w, widget);
		break;
	case WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE:
		set_operating_setting(w->number, 9, min(ride->num_circuits + 1, 20));
		break;
	case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
		set_operating_setting(w->number, 9, max(1, ride->num_circuits - 1));
		break;
	}
}

/**
 * 
 * rct2: 0x006B1165
 */
static void window_ride_operating_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	rct_ride *ride;
	const uint8 *availableModes;
	int i;

	if (dropdownIndex == -1)
		return;

	ride = GET_RIDE(w->number);

	switch (widgetIndex) {
	case WIDX_MODE_DROPDOWN:
		// Seek to available modes for this ride
		availableModes = RideAvailableModes;
		for (i = 0; i < ride->type; i++) {
			while (*(availableModes++) != 255) { }
		}
		set_operating_setting(w->number, 0, availableModes[dropdownIndex]);
		break;
	case WIDX_LOAD_DROPDOWN:
		set_operating_setting(w->number, 1, (ride->depart_flags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
		break;
	}
}

/**
 * 
 * rct2: 0x006B178E
 */
static void window_ride_operating_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_3);

	ride = GET_RIDE(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_OPERATING) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_OPERATING;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006B0B30
 */
static void window_ride_operating_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_string_id format, caption, tooltip;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// Widget setup
	w->pressed_widgets &= ~0x44700000;

	// Lift hill speed
	if ((rideEntry->enabledTrackPiecesA & RCT2_ADDRESS(0x01357444, uint32)[ride->type]) & 8) {
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WWT_24;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WWT_DROPDOWN_BUTTON;
		RCT2_GLOBAL(0x013CE966, uint16) = ride->lift_hill_speed;
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
		RCT2_GLOBAL(0x013CE968, uint16) = ride->num_circuits;
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
		window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].image = RideNameConvention[ride->type].vehicle_name == 1236 ?
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
	window_ride_operating_widgets[WIDX_MODE].image = STR_RIDE_MODE_START + ride->mode;

	// Waiting
	window_ride_operating_widgets[WIDX_LOAD].image = STR_QUARTER_LOAD + (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK);
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

		RCT2_GLOBAL(0x013CE952 + 10, uint16) = 1217;
		RCT2_GLOBAL(0x013CE95E, uint16) = ride->min_waiting_time;
		RCT2_GLOBAL(0x013CE960, uint16) = 1217;
		RCT2_GLOBAL(0x013CE962, uint16) = ride->max_waiting_time;

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
	RCT2_GLOBAL(0x013CE964, uint16) = ride->operation_option;
	switch (ride->mode) {
	case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
	case RIDE_MODE_POWERED_LAUNCH:
	case RIDE_MODE_UPWARD_LAUNCH:
	case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
		RCT2_GLOBAL(0x013CE964, uint16) = (ride->launch_speed * 9) / 4;
		format = 1331;
		caption = STR_LAUNCH_SPEED;
		tooltip = STR_LAUNCH_SPEED_TIP;
		break;
	case RIDE_MODE_STATION_TO_STATION:
		RCT2_GLOBAL(0x013CE964, uint16) = (ride->speed * 9) / 4;
		format = 1331;
		caption = STR_SPEED;
		tooltip = STR_SPEED_TIP;
		break;
	case RIDE_MODE_RACE:
		RCT2_GLOBAL(0x013CE964, uint16) = ride->num_laps;
		format = 1736;
		caption = STR_NUMBER_OF_LAPS;
		tooltip = STR_NUMBER_OF_LAPS_TIP;
		break;
	case RIDE_MODE_BUMPERCAR:
		format = 1749;
		caption = STR_TIME_LIMIT;
		tooltip = STR_TIME_LIMIT_TIP;
		break;
	case RIDE_MODE_SWING:
		format = 1771;
		caption = STR_NUMBER_OF_SWINGS;
		tooltip = STR_NUMBER_OF_SWINGS_TIP;
		break;
	case RIDE_MODE_ROTATION:
	case RIDE_MODE_FORWARD_ROTATION:
	case RIDE_MODE_BACKWARD_ROTATION:
		format = 1871;
		caption = STR_NUMBER_OF_ROTATIONS;
		tooltip = STR_NUMBER_OF_ROTATIONS_TIP;
		break;
	default:
		format = 1736;
		caption = STR_MAX_PEOPLE_ON_RIDE;
		tooltip = STR_MAX_PEOPLE_ON_RIDE_TIP;
		if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13))
			format = 0;
		break;
	}

	if (format != 0) {
		if (ride->type == RIDE_TYPE_TWIST)
			RCT2_GLOBAL(0x013CE964, uint16) *= 3;

		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WWT_24;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].image = caption;
		window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
		window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WWT_SPINNER;
		window_ride_operating_widgets[WIDX_MODE_TWEAK].image = format;
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
 * rct2: 0x006B1001
 */
static void window_ride_operating_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	uint16 blockSections;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);

	// Horizontal rule between mode settings and depart settings
	gfx_fill_rect_inset(
		dpi,
		w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].left + 4,
		w->y + 103,
		w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].right - 5,
		w->y + 104,
		w->colours[1],
		0x20
	);

	// Number of block sections
	if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED) {
		blockSections = ride->num_block_brakes + ride->num_stations;
		gfx_draw_string_left(dpi, STR_BLOCK_SECTIONS, &blockSections, 0, w->x + 21, ride->mode == 36 ? w->y + 76 : w->y + 61);
	}
}

#pragma endregion

#pragma region Maintenance

/**
 * 
 * rct2: 0x006B1AE4
 */
static void window_ride_locate_mechanic(rct_window *w)
{
	rct_ride *ride;
	rct_peep *mechanic;

	ride = GET_RIDE(w->number);

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
 * rct2: 0x006B7D08
 */
static void window_ride_maintenance_draw_bar(rct_window *w, rct_drawpixelinfo *dpi, int x, int y, int value, int unk)
{
	gfx_fill_rect_inset(dpi, x, y, x + 149, y + 8, w->colours[1], 0x30);
	if (unk & (1 << 31)) {
		unk &= ~(1 << 31);
		if (RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) == 0 && (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 8))
			return;
	}

	value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
	if (value > 2) {
		gfx_fill_rect_inset(dpi, x + 2, y + 1, x + value + 1, y + 7, unk, 0);
	}
}

/**
 * 
 * rct2: 0x006B1AAD
 */
static void window_ride_maintenance_mouseup(rct_window *w, int widgetIndex)
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
 * rct2: 0x006B1D70
 */
static void window_ride_maintenance_resize(rct_window *w)
{
	window_set_resize(w, 316, 135, 316, 135);
}

/**
 * 
 * rct2: 0x006B1ACE
 */
static void window_ride_maintenance_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	rct_ride_type *ride_type;
	rct_widget *dropdownWidget;
	int i, j, num_items;
	uint8 breakdownReason;
	
	dropdownWidget = widget;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];

	switch (widgetIndex) {
	case WIDX_INSPECTION_INTERVAL_DROPDOWN:
		dropdownWidget--;
		for (i = 0; i < 7; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_EVERY_10_MINUTES + i;
		}
		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			7,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = (1 << ride->inspection_interval);
		break;

	case WIDX_FORCE_BREAKDOWN:
		num_items = 1;
		for (j = 0; j < 3; j++) {
			if (ride_type->ride_type[j] != 0xFF)
				break;
		}
		gDropdownItemsFormat[0] = 1142;
		gDropdownItemsArgs[0] = 5290;
		for (i = 0; i < 8; i++) {
			if (RideAvailableBreakdowns[ride_type->ride_type[j]] & (uint8)(1 << i)) {
				if (i == BREAKDOWN_BRAKES_FAILURE && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
					if (ride->num_vehicles != 1)
						continue;
				}
				gDropdownItemsFormat[num_items] = 1142;
				gDropdownItemsArgs[num_items] = STR_SAFETY_CUT_OUT + i;
				num_items++;
			}
		}
		if (num_items == 1) {
			window_error_open(5289, STR_NONE);
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
			breakdownReason = ride->breakdown_reason_pending;
			if (breakdownReason != BREAKDOWN_NONE && (ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING)) {
				for (i = 0; i < 8; i++) {
					if (RideAvailableBreakdowns[ride_type->ride_type[j]] & (uint8)(1 << i)) {
						if (i == BREAKDOWN_BRAKES_FAILURE && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)) {
							if (ride->num_vehicles != 1)
								continue;
						}
						if (i == breakdownReason) {
							gDropdownItemsChecked = (1 << num_items);
							break;
						}
						gDropdownItemsFormat[num_items] = 1142;
						gDropdownItemsArgs[num_items] = STR_SAFETY_CUT_OUT + i;
						num_items++;
					}
				}
			}

			if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0) {
				*gDropdownItemsDisabled = (1 << 0);
			}
		}
		break;
	}
	
}

/**
 * 
 * rct2: 0x006B1AD9
 */
static void window_ride_maintenance_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	rct_ride *ride;
	rct_ride_type *ride_type;
	rct_vehicle *vehicle;
	int i, j, num_items;

	if (dropdownIndex == -1)
		return;
	
	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];

	switch (widgetIndex) {
	case WIDX_INSPECTION_INTERVAL_DROPDOWN:
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
		game_do_command(0, (dropdownIndex << 8) | 1, 0, (5 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
		break;

	case WIDX_FORCE_BREAKDOWN:
		if (dropdownIndex == 0) {
			switch (ride->breakdown_reason_pending) {
			case BREAKDOWN_RESTRAINTS_STUCK_CLOSED:
			case BREAKDOWN_RESTRAINTS_STUCK_OPEN:
			case BREAKDOWN_DOORS_STUCK_CLOSED:
			case BREAKDOWN_DOORS_STUCK_OPEN:
				vehicle = &(g_sprite_list[ride->vehicles[ride->broken_vehicle]].vehicle);
				vehicle->var_48 &= ~0x100;
				break;
			case BREAKDOWN_VEHICLE_MALFUNCTION:
				vehicle = &(g_sprite_list[ride->vehicles[ride->broken_vehicle]].vehicle);
				vehicle->var_48 &= ~0x200;
				break;
			}
			ride->lifecycle_flags &= ~(RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN);
			window_invalidate_by_number(WC_RIDE, w->number);
			break;
		}
		if (ride->lifecycle_flags & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED)) {
			window_error_open(5291, 5287);
		}
		else if (ride->status == RIDE_STATUS_CLOSED) {
			window_error_open(5291, 5288);
		}
		else {
			num_items = 1;
			for (j = 0; j < 3; j++) {
				if (ride_type->ride_type[j] != 0xFF)
					break;
			}
			for (i = 0; i < 8; i++) {
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
 * rct2: 0x006B1D37
 */
static void window_ride_maintenance_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_4);

	ride = GET_RIDE(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAINTENANCE) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAINTENANCE;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006B17C8
 */
static void window_ride_maintenance_invalidate(rct_window *w)
{
	rct_widget *widgets;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL].image = STR_EVERY_10_MINUTES + ride->inspection_interval;

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

	if (gConfigGeneral.debugging_tools) {
		window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_FLATBTN;
	}
	else {
		window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_EMPTY;

	}
}

/**
 * 
 * rct2: 0x006B1877
 */
static void window_ride_maintenance_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_widget *widget;
	rct_peep *mechanicSprite;
	int x, y;
	uint16 reliability, downTime, lastInspection;
	rct_string_id stringId, breakdownMessage;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);

	// Locate mechanic button image
	widget = &window_ride_maintenance_widgets[WIDX_LOCATE_MECHANIC];
	x = w->x + widget->left;
	y = w->y + widget->top;
	gfx_draw_sprite(dpi, (RCT2_GLOBAL(RCT2_ADDRESS_MECHANIC_COLOUR, uint8) << 24) | 0xA0000000 | 5196, x, y, 0);

	// Inspection label
	widget = &window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL];
	x = w->x + 4;
	y = w->y + widget->top + 1;
	gfx_draw_string_left(dpi, STR_INSPECTION, NULL, 0, x, y);

	// Reliability
	widget = &window_ride_maintenance_widgets[WIDX_PAGE_BACKGROUND];
	x = w->x + widget->left + 4;
	y = w->y + widget->top + 4;

	reliability = ride->reliability >> 8;
	gfx_draw_string_left(dpi, STR_RELIABILITY_LABEL_1757, &reliability, 0, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, max(10, reliability), 14);
	y += 11;

	downTime = ride->downtime;
	gfx_draw_string_left(dpi, STR_DOWN_TIME_LABEL_1889, &downTime, 0, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, downTime, 28);
	y += 26;

	// Last inspection
	lastInspection = ride->last_inspection;

	// Use singular form for 1 minute of time or less
	if (lastInspection <= 1)
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTE;
	else if (lastInspection <= 240)
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MINUTES;
	else
		stringId = STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;

	gfx_draw_string_left(dpi, stringId, &lastInspection, 0, x, y);
	y += 12;

	// Last / current breakdown
	if (ride->breakdown_reason == BREAKDOWN_NONE)
		return;

	stringId = (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ?
		STR_CURRENT_BREAKDOWN :
		STR_LAST_BREAKDOWN;
	breakdownMessage = STR_SAFETY_CUT_OUT + ride->breakdown_reason;
	gfx_draw_string_left(dpi, stringId, &breakdownMessage, 0, x, y);
	y += 10;

	// Mechanic status
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) {
		switch (ride->mechanic_status) {
		case RIDE_MECHANIC_STATUS_CALLING:
			stringId = STR_CALLING_MECHANIC;
			break;
		case RIDE_MECHANIC_STATUS_HEADING:
			stringId = STR_MEHCANIC_IS_HEADING_FOR_THE_RIDE;
			break;
		case RIDE_MECHANIC_STATUS_FIXING:
		case 4:
			stringId = STR_MEHCANIC_IS_FIXING_THE_RIDE;
			break;
		default:
			stringId = 0;
			break;
		}

		if (stringId != 0) {
			if (stringId == STR_CALLING_MECHANIC) {
				gfx_draw_string_left_wrapped(dpi, NULL, x + 4, y, 280, stringId, 0);
			} else {
				mechanicSprite = &(g_sprite_list[ride->mechanic].peep);
				if (peep_is_mechanic(mechanicSprite)) {
					RCT2_GLOBAL(0x013CE952 + 0, uint16) = mechanicSprite->name_string_idx;
					RCT2_GLOBAL(0x013CE952 + 2, uint32) = mechanicSprite->id;
					gfx_draw_string_left_wrapped(dpi, (void*)0x013CE952, x + 4, y, 280, stringId, 0);
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
	RIDE_ENTRANCE_STYLE_SPACE
};

static uint32 window_ride_get_colour_button_image(int colour)
{
	return 0x60000000 | (colour << 19) | 5059;
}

static int window_ride_has_track_colour(rct_ride *ride, int trackColour)
{
	uint16 unk_1 = RCT2_GLOBAL(0x00993E20 + (ride->entrance_style * 8), uint16);

	switch (trackColour) {
	case 0: return ((unk_1 & 1) && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN);
	case 1: return ((unk_1 & 2) && !ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP)) || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL);
	case 2: return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS);
	default: return 0;
	}
}

static void window_ride_set_track_colour_scheme(rct_window *w, int x, int y)
{
	rct_map_element *mapElement;
	uint8 newColourScheme;
	int interactionType, z, direction;

	newColourScheme = (uint8)(*((uint16*)&w->var_494));

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
 * rct2: 0x006B04FA
 */
static void window_ride_colour_close(rct_window *w)
{
	if (!(RCT2_GLOBAL(RCT2_ADDRESS_INPUT_FLAGS, uint32) & INPUT_FLAG_TOOL_ACTIVE))
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) != w->classification)
		return;

	if (RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber) != w->number)
		return;

	tool_cancel();
}

/**
 * 
 * rct2: 0x006B02A1
 */
static void window_ride_colour_mouseup(rct_window *w, int widgetIndex)
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
		tool_set(w, WIDX_PAINT_INDIVIDUAL_AREA, 23);
		break;
	}
}

/**
 * 
 * rct2: 0x006B0AB6
 */
static void window_ride_colour_resize(rct_window *w)
{
	window_set_resize(w, 316, 207, 316, 207);
}

/**
 * 
 * rct2: 0x006B02C6
 */
static void window_ride_colour_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride;
	uint16 colourSchemeIndex;
	vehicle_colour vehicleColour;
	rct_widget *dropdownWidget;
	rct_ride_type *rideEntry;
	int i, numItems;
	rct_string_id stringId;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);
	colourSchemeIndex = *((uint16*)&w->var_494);
	dropdownWidget = widget - 1;

	switch (widgetIndex) {
	case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
		for (i = 0; i < 4; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_MAIN_COLOUR_SCHEME + i;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			4,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = 1 << colourSchemeIndex;
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
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_BRICK_WALLS + i;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			4,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = 1 << ride->track_colour_supports[colourSchemeIndex];
		break;
	case WIDX_ENTRANCE_STYLE_DROPDOWN:		
		for (i = 0; i < countof(window_ride_entrance_style_list); i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = STR_PLAIN_ENTRANCE + window_ride_entrance_style_list[i];

			if (ride->entrance_style == window_ride_entrance_style_list[i])
				gDropdownItemsChecked = 1 << i;
		}
		int checked = gDropdownItemsChecked;

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			countof(window_ride_entrance_style_list),
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = checked;
		break;
	case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:		
		for (i = 0; i < 3; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = (RideNameConvention[ride->type].vehicle_name << 16) | (STR_ALL_VEHICLES_IN_SAME_COLOURS + i);
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			rideEntry->max_cars_in_train > 1 ? 3 : 2,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = 1 << (ride->colour_scheme_type & 3);
		break;
	case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
		numItems = ride->num_vehicles;
		if ((ride->colour_scheme_type & 3) != VEHICLE_COLOUR_SCHEME_PER_TRAIN)
			numItems = ride->num_cars_per_train;

		stringId = (ride->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_TRAIN ? 1135 : 1133;
		for (i = 0; i < 32; i++) {
			gDropdownItemsFormat[i] = 1142;
			gDropdownItemsArgs[i] = ((sint64)(i + 1) << 32) | ((RideNameConvention[ride->type].vehicle_name + 2) << 16) | stringId;
		}

		window_dropdown_show_text_custom_width(
			w->x + dropdownWidget->left,
			w->y + dropdownWidget->top,
			dropdownWidget->bottom - dropdownWidget->top + 1,
			w->colours[1],
			DROPDOWN_FLAG_STAY_OPEN,
			numItems,
			widget->right - dropdownWidget->left
		);

		gDropdownItemsChecked = 1 << w->var_48C;
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
 * rct2: 0x006B0331
 */
static void window_ride_colour_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (dropdownIndex == -1)
		return;

	switch (widgetIndex) {
	case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
		*((uint16*)&w->var_494) = dropdownIndex;
		window_invalidate(w);
		break;
	case WIDX_TRACK_MAIN_COLOUR:
		game_do_command(0, (0 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, *((uint16*)&w->var_494), 0);
		break;
	case WIDX_TRACK_ADDITIONAL_COLOUR:
		game_do_command(0, (1 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, *((uint16*)&w->var_494), 0);
		break;
	case WIDX_TRACK_SUPPORT_COLOUR:
		game_do_command(0, (4 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, *((uint16*)&w->var_494), 0);
		break;
	case WIDX_MAZE_STYLE_DROPDOWN:
		game_do_command(0, (4 << 8) | 1, 0, (dropdownIndex << 8) | w->number, GAME_COMMAND_SET_RIDE_APPEARANCE, *((uint16*)&w->var_494), 0);
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
 * rct2: 0x006B0A8F
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
 * rct2: 0x006B04EC
 */
static void window_ride_colour_tooldown(rct_window *w, int widgetIndex, int x, int y)
{	
	if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
		window_ride_set_track_colour_scheme(w, x, y);
}

/**
 * 
 * rct2: 0x006B04F3
 */
static void window_ride_colour_tooldrag(rct_window *w, int widgetIndex, int x, int y)
{	
	if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
		window_ride_set_track_colour_scheme(w, x, y);
}

/**
 * 
 * rct2: 0x006AFB36
 */
static void window_ride_colour_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride_type *rideEntry;
	rct_ride *ride;
	track_colour trackColour;
	vehicle_colour vehicleColour;
	int vehicleColourSchemeType;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// Track colours
	int colourScheme = *((uint16*)&w->var_494);
	trackColour = ride_get_track_colour(ride, colourScheme);

	// Maze style
	if (ride->type == RIDE_TYPE_MAZE) {
		window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WWT_DROPDOWN;
		window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
		window_ride_colour_widgets[WIDX_MAZE_STYLE].image = STR_BRICK_WALLS + trackColour.supports;
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
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_COLORBTN;
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].image = window_ride_get_colour_button_image(trackColour.main);
	} else {
		window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_EMPTY;
	}

	// Track additional colour
	if (window_ride_has_track_colour(ride, 1)) {
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_COLORBTN;
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = window_ride_get_colour_button_image(trackColour.additional);
	} else {
		window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_EMPTY;
	}
	
	// Track supports colour
	if (window_ride_has_track_colour(ride, 2) && ride->type != RIDE_TYPE_MAZE) {
		window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WWT_COLORBTN;
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
	if (RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x20) {
		window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_SPINNER;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_DROPDOWN;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_DROPDOWN_BUTTON;

		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].image = STR_PLAIN_ENTRANCE + ride->entrance_style;
	} else {
		window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_EMPTY;
		window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_EMPTY;
	}

	// Vehicle colours
	if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_13) && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_26)) {
		vehicleColourSchemeType = ride->colour_scheme_type & 3;
		if (vehicleColourSchemeType == 0)
			w->var_48C = 0;

		vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);

		window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WWT_SCROLL;
		window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].type = WWT_COLORBTN;
		window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].image = window_ride_get_colour_button_image(vehicleColour.main);
		
		uint8 trainLayout[16];
		ride_entry_get_train_layout(ride->subtype, ride->num_cars_per_train, trainLayout);

		uint32 colourFlags = 0;
		for (int i = 0; i < ride->num_cars_per_train; i++) {
			uint8 vehicleTypeIndex = trainLayout[i];

			colourFlags |= rideEntry->vehicles[vehicleTypeIndex].var_14;
			colourFlags = ror32(colourFlags, 16);
			colourFlags |= rideEntry->vehicles[vehicleTypeIndex].var_12;
			colourFlags = ror32(colourFlags, 16);
		}

		// Additional colours
		if (colourFlags & 1) {
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_COLORBTN;
			window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].image = window_ride_get_colour_button_image(vehicleColour.additional_1);
			if (colourFlags & 0x2000000) {
				window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_COLORBTN;
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
		RCT2_GLOBAL(0x013CE952 +  6, uint16) = STR_ALL_VEHICLES_IN_SAME_COLOURS + vehicleColourSchemeType;
		RCT2_GLOBAL(0x013CE952 +  8, uint16) = RideNameConvention[ride->type].vehicle_name;
		RCT2_GLOBAL(0x013CE952 + 10, uint16) = RideNameConvention[ride->type].vehicle_name + 2;
		RCT2_GLOBAL(0x013CE952 + 12, uint16) = w->var_48C + 1;
		// Vehicle index
		if (vehicleColourSchemeType != 0) {
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_DROPDOWN;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_DROPDOWN_BUTTON;
			window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].image = vehicleColourSchemeType == 1 ? 1134 : 1132;
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

	RCT2_GLOBAL(0x013CE960, uint16) = STR_MAIN_COLOUR_SCHEME + colourScheme;

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006AFF3E
 */
static void window_ride_colour_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_drawpixelinfo *clippedDpi;
	rct_widget *widget;
	rct_ride *ride;
	rct_ride_type *rideEntry;
	int x, y, spriteIndex, terniaryColour;
	track_colour trackColour;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	// Track / shop item preview
	widget = &window_ride_colour_widgets[WIDX_TRACK_PREVIEW];
	if (widget->type != WWT_EMPTY)
		gfx_fill_rect(dpi, w->x + widget->left + 1, w->y + widget->top + 1, w->x + widget->right - 1, w->y + widget->bottom - 1, 12);
	
	trackColour = ride_get_track_colour(ride, *((uint16*)&w->var_494));

	// 
	if (rideEntry->shop_item == 0xFF) {
		x = w->x + widget->left;
		y = w->y + widget->top;

		// Track
		if (ride->type == RIDE_TYPE_MAZE) {
			spriteIndex = 21990 + trackColour.supports;
			gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
		} else {
			spriteIndex = 14222 + (ride->type * 2);
			spriteIndex |= (trackColour.additional << 24) | (trackColour.main << 19);
			spriteIndex |= 0xA0000000;
			gfx_draw_sprite(dpi, spriteIndex, x, y, 0);

			// Supports
			spriteIndex = 14222 + (ride->type * 2) + 1;
			spriteIndex |= trackColour.supports << 19;
			spriteIndex |= 0x20000000;
			gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
		}
	} else {
		x = w->x + (widget->left + widget->right) / 2 - 8;
		y = w->y + (widget->bottom + widget->top) / 2 - 6;

		uint8 shopItem = rideEntry->shop_item_secondary == 255 ? rideEntry->shop_item : rideEntry->shop_item_secondary;
		spriteIndex = 5061 + shopItem;
		spriteIndex |= ride->track_colour_main[0] << 19;
		spriteIndex |= 0x20000000;

		gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
	}

	// Entrance preview
	trackColour = ride_get_track_colour(ride, 0);
	widget = &w->widgets[WIDX_ENTRANCE_PREVIEW];
	if (widget->type != WWT_EMPTY) {
		clippedDpi = clip_drawpixelinfo(
			dpi, w->x + widget->left + 1, widget->right - widget->left, w->y + widget->top + 1, widget->bottom - widget->top
		);
		if (clippedDpi != NULL) {
			gfx_clear(clippedDpi, 0x0C0C0C0C);

			terniaryColour = 0;
			if (RCT2_GLOBAL(0x00993E1C + (ride->entrance_style * 8), uint32) & 0x40000000)
				terniaryColour = 0x40000000 | ((trackColour.main + 112) << 19);

			spriteIndex = (trackColour.additional << 24) | (trackColour.main << 19);
			spriteIndex |= 0xA0000000;
			spriteIndex += RideEntranceDefinitions[ride->entrance_style].spriteIndex;

			// Back
			gfx_draw_sprite(clippedDpi, spriteIndex, 34, 20, terniaryColour);

			// Front
			gfx_draw_sprite(clippedDpi, spriteIndex + 4, 34, 20, terniaryColour);

			// ?
			if (terniaryColour != 0)
				gfx_draw_sprite(clippedDpi, ((spriteIndex + 20) & 0x7FFFF) + terniaryColour, 34, 20, terniaryColour);

			rct2_free(clippedDpi);
		}
	}
}

/**
 * 
 * rct2: 0x006B0192
 */
static void window_ride_colour_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_widget *vehiclePreviewWidget;
	int trainCarIndex, x, y, spriteIndex;
	vehicle_colour vehicleColour;

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);
	vehiclePreviewWidget = &window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW];
	vehicleColour = ride_get_vehicle_colour(ride, w->var_48C);

	// Background colour
	gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, 12);

	// ?
	x = (vehiclePreviewWidget->right - vehiclePreviewWidget->left) / 2;
	y = vehiclePreviewWidget->bottom - vehiclePreviewWidget->top - 15;

	uint8 trainLayout[16];
	ride_entry_get_train_layout(ride->subtype, ride->num_cars_per_train, trainLayout);

	// ?
	trainCarIndex = (ride->colour_scheme_type & 3) == RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR ?
		w->var_48C : rideEntry->tab_vehicle;

	rct_ride_type_vehicle* rideVehicleEntry = &rideEntry->vehicles[trainLayout[trainCarIndex]];

	y += rideVehicleEntry->var_0A;

	// Draw the coloured spinning vehicle
	spriteIndex = rideVehicleEntry->var_12 & 0x800 ? w->frame_no / 4 : w->frame_no / 2;
	spriteIndex &= rideVehicleEntry->var_00;
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
 * rct2: 0x006B215D
 */
static void window_ride_toggle_music(rct_window *w)
{
	rct_ride *ride = GET_RIDE(w->number);

	int activateMusic = (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (activateMusic << 8) | 1, 0, (6 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 * 
 * rct2: 0x006B1ED7
 */
static void window_ride_music_mouseup(rct_window *w, int widgetIndex)
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
 * rct2: 0x006AF4A2
 */
static void window_ride_music_resize(rct_window *w)
{
	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 81, 316, 81);
}

/**
 * 
 * rct2: 0x006B1EFC
 */
static void window_ride_music_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	int i;

	if (widgetIndex != WIDX_MUSIC_DROPDOWN)
		return;

	dropdownWidget = widget - 1;
	rct_ride *ride = GET_RIDE(w->number);

	int numItems = 0;
	if (ride->type == RIDE_TYPE_MERRY_GO_ROUND) {
		window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_FAIRGROUND_ORGAN;		 
	} else {
		for (i = 0; i < countof(MusicStyleOrder); i++)
			window_ride_current_music_style_order[numItems++] = MusicStyleOrder[i];

		if (ride_music_info_list[36]->length != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_1;
		if (ride_music_info_list[37]->length != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_2;
	}

	for (i = 0; i < numItems; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_MUSIC_STYLE_START + window_ride_current_music_style_order[i];
	}

	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		DROPDOWN_FLAG_STAY_OPEN,
		numItems,
		widget->right - dropdownWidget->left
	);

	for (i = 0; i < numItems; i++) {
		if (window_ride_current_music_style_order[i] == ride->music)
			gDropdownItemsChecked = (1 << i);
	}
}

/**
 * 
 * rct2: 0x006B1F03
 */
static void window_ride_music_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	rct_ride *ride;
	uint8 musicStyle;

	if (widgetIndex != WIDX_MUSIC_DROPDOWN || dropdownIndex == -1)
		return;

	ride = GET_RIDE(w->number);
	musicStyle = window_ride_current_music_style_order[dropdownIndex];
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (musicStyle << 8) | 1, 0, (7 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 * 
 * rct2: 0x006B2198
 */
static void window_ride_music_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_6);
}

/**
 * 
 * rct2: 0x006B1DEA
 */
static void window_ride_music_invalidate(rct_window *w)
{
	rct_widget *widgets;
	int isMusicActivated;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// Set selected music
	window_ride_music_widgets[WIDX_MUSIC].image = STR_MUSIC_STYLE_START + ride->music;

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
 * rct2: 0x006B1ECC
 */
static void window_ride_music_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Measurements

/* rct2: 0x006D2804 when al == 0*/
static void cancel_scenery_selection(){
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) &= ~(1 << 2);
	RCT2_GLOBAL(0x9DEA6F, uint8) &= ~(1 << 0);
	unpause_sounds();

	rct_window* main_w = window_get_main();

	if (main_w){
		main_w->viewport->flags &= ~(VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
	}

	gfx_invalidate_screen();
	tool_cancel();
}

/* rct2: 0x006D27A3 */
static void setup_scenery_selection(rct_window* w){
	rct_ride* ride = GET_RIDE(w->number);

	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1){
		cancel_scenery_selection();
	}

	while (tool_set(w, 0, 12));

	RCT2_GLOBAL(0x00F64DE8, uint8) = (uint8)w->number;
	RCT2_GLOBAL(0x009DA193, uint8) = 0xFF;

	gTrackSavedMapElements[0] = (rct_map_element*)-1;
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_PAUSED, uint8) |= (1 << 2);
	RCT2_GLOBAL(0x009DEA6F, uint8) |= 1;

	pause_sounds();

	rct_window* w_main = window_get_main();

	if (w_main){
		w_main->viewport->flags |= (VIEWPORT_FLAG_HIDE_VERTICAL | VIEWPORT_FLAG_HIDE_BASE);
	}

	gfx_invalidate_screen();
}

/**
 * 
 * rct2: 0x006D3026
 */
static void window_ride_measurements_design_reset()
{
	track_save_reset_scenery();
}

/**
 * 
 * rct2: 0x006D303D
 */
static void window_ride_measurements_design_select_nearby_scenery()
{
	track_save_select_nearby_scenery(RCT2_GLOBAL(0x00F64DE8, uint8));
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_design_cancel()
{
	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1)
		cancel_scenery_selection();
}

/**
 * 
 * rct2: 0x006AD4CD
 */
static void window_ride_measurements_design_save(rct_window *w)
{
	if (save_track_design((uint8)w->number) == 0) return;

	window_ride_measurements_design_cancel();
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_close(rct_window *w)
{
	window_ride_measurements_design_cancel();
}

/**
 * 
 * rct2: 0x006AD478
 */
static void window_ride_measurements_mouseup(rct_window *w, int widgetIndex)
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
 * rct2: 0x006AD564
 */
static void window_ride_measurements_resize(rct_window *w)
{
	window_set_resize(w, 316, 202, 316, 202);
}

/**
 * 
 * rct2: 0x006AD4AB
 */
static void window_ride_measurements_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
		return;

	rct_ride *ride = GET_RIDE(w->number);

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
	RCT2_GLOBAL(0x009DEBA2, sint16) = 0;
	if (RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_FLAGS, uint8) & SCREEN_FLAGS_TRACK_DESIGNER)
		RCT2_GLOBAL(0x009DED34, uint32) |= 2;
}

/**
 * 
 * rct2: 0x006AD4B2
 */
static void window_ride_measurements_dropdown(rct_window *w, int widgetIndex, int dropdownIndex)
{
	if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
		return;

	if (dropdownIndex == -1)
		dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

	if (dropdownIndex == 0)
		save_track_design((uint8)w->number);
	else
		setup_scenery_selection(w);
}

/**
 * 
 * rct2: 0x006AD5DD
 */
static void window_ride_measurements_update(rct_window *w)
{
	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_7);
}

/**
 * 
 * rct2: 0x006D2AE7
 */
static void window_ride_measurements_tooldown(rct_window *w, int widgetIndex, int x, int y)
{
	rct_map_element *mapElement;
	sint16 mapX, mapY;
	int interactionType;

	get_map_coordinates_from_pos(x, y, 0xFCCF, &mapX, &mapY, &interactionType, &mapElement, NULL);
	switch (interactionType) {
	case VIEWPORT_INTERACTION_ITEM_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
	case VIEWPORT_INTERACTION_ITEM_WALL:
	case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
		track_save_toggle_map_element(interactionType, mapX, mapY, mapElement);
		break;
	}
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_toolabort(rct_window *w, int widgetIndex)
{
	window_ride_measurements_design_cancel();
}

/**
 * 
 * rct2: 0x006ACDBC
 */
static void window_ride_measurements_invalidate(rct_window *w)
{
	rct_widget *widgets;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
	window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WWT_EMPTY;
	if ((RCT2_GLOBAL(0x009DEA6F, uint8) & 1) && RCT2_GLOBAL(0x00F64DE8, uint8) == w->number) {
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
				if (ride->excitement != -1) {
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
 * rct2: 0x006ACF07
 */
static void window_ride_measurements_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_widget *widget;
	rct_ride *ride;
	rct_string_id stringId;
	int x, y, i, numTimes, numLengths;
	sint16 holes, maxSpeed, averageSpeed, drops, highestDropHeight, inversions, time;
	sint32 maxPositiveVerticalGs, maxNegativeVerticalGs, maxLateralGs, totalAirTime, length;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	if (window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type == WWT_DROPDOWN_BUTTON) {
		widget = &window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND];

		x = w->x + (widget->right - widget->left) / 2;
		y = w->y + widget->top + 40;
		gfx_draw_string_centred_wrapped(dpi, NULL, x, y, w->width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, 0);

		x = w->x + 4;
		y = w->y + window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
		gfx_fill_rect_inset(dpi, x, y, w->x + 312, y + 1, w->colours[1], 0x20);
	} else {
		ride = GET_RIDE(w->number);
		
		x = w->x + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].left + 4;
		y = w->y + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].top + 4;

		if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED) {
			// Excitement
			RCT2_GLOBAL(0x013CE952 + 0, uint32) = ride->excitement;
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_RATING_LOW + min(ride->excitement >> 8, 5);
			stringId = ride->excitement == -1 ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE : STR_EXCITEMENT_RATING;
			gfx_draw_string_left(dpi, stringId, (void*)0x013CE952, 0, x, y);
			y += 10;

			// Intensity
			RCT2_GLOBAL(0x013CE952 + 0, uint32) = ride->intensity;
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_RATING_LOW + min(ride->intensity >> 8, 5);

			stringId = STR_INTENSITY_RATING;
			if (ride->excitement == -1)
				stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
			else if (ride->intensity >= RIDE_RATING(10,00))
				stringId = STR_INTENSITY_RATING_RED;

			gfx_draw_string_left(dpi, stringId, (void*)0x013CE952, 0, x, y);
			y += 10;

			// Nausea
			RCT2_GLOBAL(0x013CE952 + 0, uint32) = ride->nausea;
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = STR_RATING_LOW + min(ride->nausea >> 8, 5);
			stringId = ride->excitement == -1 ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
			gfx_draw_string_left(dpi, stringId, (void*)0x013CE952, 0, x, y);
			y += 20;

			// Horizontal rule
			gfx_fill_rect_inset(dpi, x, y - 6, x + 303, y - 5, w->colours[1], 0x20);

			if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_NO_RAW_STATS)) {
				if (ride->type == RIDE_TYPE_MINI_GOLF) {
					// Holes
					holes = ride->holes & 0x1F;
					gfx_draw_string_left(dpi, STR_HOLES, &holes, 0, x, y);
					y += 10;
				} else {
					// Max speed
					maxSpeed = (ride->max_speed * 9) >> 18;
					gfx_draw_string_left(dpi, STR_MAX_SPEED, &maxSpeed, 0, x, y);
					y += 10;

					// Average speed
					averageSpeed = (ride->average_speed * 9) >> 18;
					gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &averageSpeed, 0, x, y);
					y += 10;

					// Ride time
					numTimes = 0;
					for (i = 0; i < ride->num_stations; i++) {
						time = ride->time[numTimes];
						if (time != 0) {
							RCT2_GLOBAL(0x013CE952 + 0 + (numTimes * 4), uint16) = 1343;
							RCT2_GLOBAL(0x013CE952 + 2 + (numTimes * 4), uint16) = time;
							numTimes++;
						}
					}
					if (numTimes == 0) {
						RCT2_GLOBAL(0x013CE952 + 0, uint16) = 1343;
						RCT2_GLOBAL(0x013CE952 + 2, uint16) = 0;
						numTimes++;
					}
					RCT2_GLOBAL(0x013CE94E + (numTimes * 4), uint16) = 1342;
					RCT2_GLOBAL(0x013CE952 + 0 + (numTimes * 4), uint16) = 0;
					RCT2_GLOBAL(0x013CE952 + 2 + (numTimes * 4), uint16) = 0;
					RCT2_GLOBAL(0x013CE952 + 4 + (numTimes * 4), uint16) = 0;
					RCT2_GLOBAL(0x013CE952 + 6 + (numTimes * 4), uint16) = 0;
					gfx_draw_string_left_clipped(dpi, STR_RIDE_TIME, (void*)0x013CE952, 0, x, y, 308);
					y += 10;
				}

				// Ride length
				numLengths = 0;
				for (i = 0; i < ride->num_stations; i++) {
					length = ride->length[numLengths];
					if (length != 0) {
						length >>= 16;
						RCT2_GLOBAL(0x013CE952 + 0 + (numLengths * 4), uint16) = 1346;
						RCT2_GLOBAL(0x013CE952 + 2 + (numLengths * 4), uint16) = (length & 0xFFFF);
						numLengths++;
					}
				}
				if (numLengths == 0) {
					RCT2_GLOBAL(0x013CE952 + 0, uint16) = 1346;
					RCT2_GLOBAL(0x013CE952 + 2, uint16) = 0;
					numLengths++;
				}
				RCT2_GLOBAL(0x013CE94E + (numLengths * 4), uint16) = 1345;
				RCT2_GLOBAL(0x013CE952 + 0 + (numLengths * 4), uint16) = 0;
				RCT2_GLOBAL(0x013CE952 + 2 + (numLengths * 4), uint16) = 0;
				RCT2_GLOBAL(0x013CE952 + 4 + (numLengths * 4), uint16) = 0;
				RCT2_GLOBAL(0x013CE952 + 6 + (numLengths * 4), uint16) = 0;
				gfx_draw_string_left_clipped(dpi, STR_RIDE_LENGTH, (void*)0x013CE952, 0, x, y, 308);
				y += 10;

				if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES)) {
					// Max. positive vertical G's
					maxPositiveVerticalGs = ride->max_positive_vertical_g;
					stringId = maxPositiveVerticalGs >= FIXED_2DP(5,00) ?
						STR_MAX_POSITIVE_VERTICAL_G_RED : STR_MAX_POSITIVE_VERTICAL_G;
					gfx_draw_string_left(dpi, stringId, &maxPositiveVerticalGs, 0, x, y);
					y += 10;

					// Max. negative vertical G's
					maxNegativeVerticalGs = ride->max_negative_vertical_g;
					stringId = maxNegativeVerticalGs <= -FIXED_2DP(2,00) ?
						STR_MAX_NEGATIVE_VERTICAL_G_RED : STR_MAX_NEGATIVE_VERTICAL_G;
					gfx_draw_string_left(dpi, stringId, &maxNegativeVerticalGs, 0, x, y);
					y += 10;

					// Max lateral G's
					maxLateralGs = ride->max_lateral_g;
					stringId = maxLateralGs >= FIXED_2DP(2,80) ?
						STR_MAX_LATERAL_G_RED : STR_MAX_LATERAL_G;
					gfx_draw_string_left(dpi, stringId, &maxLateralGs, 0, x, y);
					y += 10;

					// Total 'air' time
					totalAirTime = ride->total_air_time * 3;
					gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &totalAirTime, 0, x, y);
					y += 10;
				}

				if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DROPS)) {
					// Drops
					drops = ride->drops & 0x3F;
					gfx_draw_string_left(dpi, STR_DROPS, &drops, 0, x, y);
					y += 10;

					// Highest drop height
					highestDropHeight = (ride->highest_drop_height * 3) / 4;
					gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &highestDropHeight, 0, x, y);
					y += 10;
				}

				if (ride->type != RIDE_TYPE_MINI_GOLF) {
					// Inversions
					inversions = ride->inversions & 0x1F;
					if (inversions != 0) {
						gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, 0, x, y);
						y += 10;
					}
				}
			}
		} else {
			gfx_draw_string_left(dpi, STR_NO_TEST_RESULTS_YET, NULL, 0, x, y);
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
 * rct2: 0x006AE8A6
 */
static void window_ride_set_graph(rct_window *w, int type)
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
 * rct2: 0x006AE85D
 */
static void window_ride_graphs_mouseup(rct_window *w, int widgetIndex)
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
 * rct2: 0x006AE8DA
 */
static void window_ride_graphs_resize(rct_window *w)
{
	window_set_resize(w, 316, 180, 500, 450);
}

/**
 * 
 * rct2: 0x006AE878
 */
static void window_ride_graphs_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
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
 * rct2: 0x006AE95D
 */
static void window_ride_graphs_update(rct_window *w)
{
	rct_widget *widget;
	rct_ride_measurement *measurement;
	int x;

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
 * rct2: 0x006AEA75
 */
static void window_ride_graphs_scrollgetheight(rct_window *w, int scrollIndex, int *width, int *height)
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
 * rct2: 0x006AE953
 */
static void window_ride_graphs_15(rct_window *w, int scrollIndex, int scrollAreaType)
{
	w->list_information_type |= 0x8000;
}

/**
 * 
 * rct2: 0x006AEA05
 */
static void window_ride_graphs_tooltip(rct_window* w, int widgetIndex, rct_string_id *stringId)
{
	rct_ride *ride;
	rct_ride_measurement *measurement;
	rct_string_id message;

	if (widgetIndex == WIDX_GRAPH) {
		RCT2_GLOBAL(0x013CE952, uint16) = 3158;
		measurement = ride_get_measurement(w->number, &message);
		if (measurement != NULL && (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING)) {
			RCT2_GLOBAL(0x013CE952 + 4, uint16) = measurement->vehicle_index + 1;
			ride = GET_RIDE(w->number);
			RCT2_GLOBAL(0x013CE952 + 2, uint16) = RideNameConvention[ride->type].vehicle_name + 6;
		} else {
			*stringId = message;
		}
	} else {
		*stringId = STR_NONE;
	}
}

/**
 * 
 * rct2: 0x006AE372
 */
static void window_ride_graphs_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride *ride;
	int x, y;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

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
 * rct2: 0x006AE4BC
 */
static void window_ride_graphs_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

/**
 * 
 * rct2: 0x006AE4C7
 */
static void window_ride_graphs_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int scrollIndex)
{
	rct_ride_measurement *measurement;
	rct_widget *widget;
	int x, y, width, time, listType, colour, top, bottom, tmp;
	rct_string_id stringId;

	gfx_clear(dpi, RCT2_GLOBAL(0x0141FC9D, uint8) * 0x01010101);

	widget = &window_ride_graphs_widgets[WIDX_GRAPH];
	listType = w->list_information_type & 0xFF;
	measurement = ride_get_measurement(w->number, &stringId);
	if (measurement == NULL) {
		// No measurement message
		x = (widget->right - widget->left) / 2;
		y = (widget->bottom - widget->top) / 2 - 5;
		width = widget->right - widget->left - 2;
		gfx_draw_string_centred_wrapped(dpi, (void*)0x013CE952, x, y, width, stringId, 0);
		return;
	}

	// Vertical grid lines
	time = 0;
	for (x = 0; x < dpi->x + dpi->width; x += 80) {
		if (x + 80 >= dpi->x) {
			gfx_fill_rect(dpi, x +  0, dpi->y, x +  0, dpi->y + dpi->height - 1, RCT2_GLOBAL(0x0141FCA0, uint8));
			gfx_fill_rect(dpi, x + 16, dpi->y, x + 16, dpi->y + dpi->height - 1, RCT2_GLOBAL(0x0141FC9F, uint8));
			gfx_fill_rect(dpi, x + 32, dpi->y, x + 32, dpi->y + dpi->height - 1, RCT2_GLOBAL(0x0141FC9F, uint8));
			gfx_fill_rect(dpi, x + 48, dpi->y, x + 48, dpi->y + dpi->height - 1, RCT2_GLOBAL(0x0141FC9F, uint8));
			gfx_fill_rect(dpi, x + 64, dpi->y, x + 64, dpi->y + dpi->height - 1, RCT2_GLOBAL(0x0141FC9F, uint8));
		}
		time += 5;
	}

	// Horizontal grid lines
	y = widget->bottom - widget->top - 13;
	short yUnit = RCT2_GLOBAL(0x0098DD9A + (listType * 8), uint16);
	short ax = RCT2_GLOBAL(0x0098DD9E + (listType * 8), uint16);
	short yUnitInterval = RCT2_GLOBAL(0x0098DD9C + (listType * 8), uint16);
	short yInterval = RCT2_GLOBAL(0x0098DD98 + (listType * 8), uint16);

	// Scale modifier
	if (ax == 1420) {
		short unk = RCT2_GLOBAL(0x01359208, uint16);
		yUnit -= RCT2_GLOBAL(0x01359208, uint16);
		unk *= 2;
		yUnit -= unk;
	}

	for (y = widget->bottom - widget->top - 13; y >= 8; y -= yInterval, yUnit += yUnitInterval) {
		// Minor / major line
		colour = yUnit == 0 ?
			RCT2_GLOBAL(0x0141FCA0, uint8) :
			RCT2_GLOBAL(0x0141FC9F, uint8);

		gfx_fill_rect(dpi, dpi->x, y, dpi->x + dpi->width - 1, y, colour);

		sint16 scaled_yUnit = yUnit;
		// Scale modifier
		if (ax == 1420)
			scaled_yUnit /= 2;

		gfx_draw_string_left(dpi, ax, &scaled_yUnit, 0, w->scrolls[0].h_left + 1, y - 4);
	}

	// Time marks
	x = 0;
	time = 0;
	for (x = 0; x < dpi->x + dpi->width; x += 80) {
		if (x + 80 >= dpi->x)
			gfx_draw_string_left(dpi, 1414, &time, 0, x + 2, 1);
		time += 5;
	}

	// Plot
	x = dpi->x;
	for (width = 0; width < dpi->width; width++, x++) {
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
		}

		top = widget->bottom - widget->top - top - 13;
		bottom = widget->bottom - widget->top - bottom - 13;
		if (top > bottom) {
			tmp = top;
			top = bottom;
			bottom = tmp;
		}
		gfx_fill_rect(dpi, x, top, x, bottom, x > measurement->current_item ? 17 : 21);
	}
}

#pragma endregion

#pragma region Income

/**
 * 
 * rct2: 0x006ADEFD
 */
static void window_ride_income_toggle_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;
	uint32 newFlags, shop_item;
	money16 price;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];
	
	if (ride->type == RIDE_TYPE_TOILETS) {
		shop_item = 0x1F;
	}
	else {
		shop_item = ride_type->shop_item;
		if (shop_item == 0xFFFF)
			return;
	}
	if (shop_item == 0x3 || shop_item == 0x20 || shop_item == 0x21 || shop_item == 0x22) {
		newFlags = RCT2_GLOBAL(0x01358838, uint32);
		newFlags ^= (1 << 0x3);
		game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);

		newFlags = RCT2_GLOBAL(0x0135934C, uint32);
		newFlags ^= (1 << 0x0) | (1 << 0x1) | (1 << 0x2);
		game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
	}
	else {
		if (shop_item < 32) {
			newFlags = RCT2_GLOBAL(0x01358838, uint32);
			newFlags ^= (1 << shop_item);
			game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
		else {
			newFlags = RCT2_GLOBAL(0x0135934C, uint32);
			newFlags ^= (1 << (shop_item - 32));
			game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
	}
	price = ride->price;
	game_do_command(0, 1, 0, w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 * 
 * rct2: 0x006AE06E
 */
static void window_ride_income_toggle_secondary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;
	uint32 newFlags, shop_item;
	money16 price;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];
	
	shop_item = ride_type->shop_item_secondary;
	if (shop_item == 0xFF)
		shop_item = RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8);

	if (shop_item == 0x3 || shop_item == 0x20 || shop_item == 0x21 || shop_item == 0x22) {
		newFlags = RCT2_GLOBAL(0x01358838, uint32);
		newFlags ^= (1 << 0x3);
		game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);

		newFlags = RCT2_GLOBAL(0x0135934C, uint32);
		newFlags ^= (1 << 0x0) | (1 << 0x1) | (1 << 0x2);
		game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
	}
	else {
		if (shop_item < 32) {
			newFlags = RCT2_GLOBAL(0x01358838, uint32);
			newFlags ^= (1 << shop_item);
			game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
		else {
			newFlags = RCT2_GLOBAL(0x0135934C, uint32);
			newFlags ^= (1 << (shop_item - 32));
			game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
		}
	}
	price = ride->price_secondary;
	game_do_command(0, 1, 0, (1 << 8) | w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 * 
 * rct2: 0x006AE1E4
 */
static void window_ride_income_increase_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];
	
	if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item == 0xFF) {
			if (!gConfigCheat.unlock_all_prices)
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
 * rct2: 0x006AE237
 */
static void window_ride_income_decrease_primary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];

	if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item == 0xFF) {
			if (!gConfigCheat.unlock_all_prices)
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
 * rct2: 0x006AE269
 */
static void window_ride_income_increase_secondary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];
	
	if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item_secondary == 0xFF) {
			if (!gConfigCheat.unlock_all_prices)
				return;
		}
	}
	money16 price = ride->price_secondary;
	if (price < MONEY(20, 00))
		price++;

	game_do_command(0, 1, 0, (w->number & 0x00FF) | 0x0100, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 * 
 * rct2: 0x006AE28D
 */
static void window_ride_income_decrease_secondary_price(rct_window *w)
{
	rct_ride *ride;
	rct_ride_type *ride_type;

	ride = GET_RIDE(w->number);
	ride_type = gRideTypeList[ride->subtype];
	
	if ((RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) == 0) {
		if (ride->type != RIDE_TYPE_TOILETS && ride_type->shop_item_secondary == 0xFF) {
			if (!gConfigCheat.unlock_all_prices)
				return;
		}
	}
	money16 price = ride->price_secondary;
	if (price > MONEY(0, 00))
		price--;

	game_do_command(0, 1, 0, (w->number & 0x00FF) | 0x0100, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 * 
 * rct2: 0x006ADEA9
 */
static void window_ride_income_mouseup(rct_window *w, int widgetIndex)
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
 * rct2: 0x006AE2F8
 */
static void window_ride_income_resize(rct_window *w)
{
	window_set_resize(w, 316, 177, 316, 177);
}

/**
 * 
 * rct2: 0x006ADED4
 */
static void window_ride_income_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
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
 * rct2: 0x006AE2BF
 */
static void window_ride_income_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_9);

	ride = GET_RIDE(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_INCOME) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_INCOME;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006ADAA3
 */
static void window_ride_income_invalidate(rct_window *w)
{
	rct_widget *widgets;
	rct_ride_type *rideEntry;
	rct_string_id stringId;
	int primaryItem, secondaryItem;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	rideEntry = ride_get_entry(ride);

	// Primary item
	w->pressed_widgets &= ~(1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
	w->disabled_widgets &= ~(1 << WIDX_PRIMARY_PRICE);

	//If the park doesn't have free entry, lock the admission price, unless the cheat to unlock all prices is activated.
	if ((!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) && rideEntry->shop_item == 255 && ride->type != RIDE_TYPE_TOILETS)
		&& (!gConfigCheat.unlock_all_prices))
	{
		w->disabled_widgets |= (1 << WIDX_PRIMARY_PRICE);
	}

	window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].image = STR_RIDE_INCOME_ADMISSION_PRICE;
	window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].image = STR_ON_RIDE_PHOTO_PRICE;
	window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_EMPTY;
	
	window_ride_income_widgets[WIDX_PRIMARY_PRICE].image = 1429;
	RCT2_GLOBAL(0x013CE952 + 6, money32) = ride->price;
	if (ride->price == 0)
		window_ride_income_widgets[WIDX_PRIMARY_PRICE].image = STR_FREE;

	primaryItem = 31;
	if (ride->type == RIDE_TYPE_TOILETS || ((primaryItem = (sint8)rideEntry->shop_item) != -1)) {
		window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;
		if (primaryItem < 32) {
			if (RCT2_GLOBAL(0x01358838, uint32) & (1 << primaryItem))
				w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

			if (primaryItem != 31)
				window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].image = 1960 + primaryItem;
		}
		else {
			primaryItem -= 32;
			if (RCT2_GLOBAL(0x0135934C, uint32) & (1 << primaryItem))
				w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

			window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].image = 2100 + primaryItem;
		}

	}

	// Get secondary item
	secondaryItem = RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8);
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO)) {
		if ((secondaryItem = (sint8)rideEntry->shop_item_secondary) != -1) {
			// Set secondary item label
			stringId = 1960 + secondaryItem;
			if (stringId >= 1992)
				stringId += 108;

			window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].image = stringId;
		}
	}

	if (secondaryItem == -1) {
		// Hide secondary item widgets
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_EMPTY;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_EMPTY;
	} else {
		// Set same price throughout park checkbox
		w->pressed_widgets &= ~(1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
		if (secondaryItem < 32) {
			if (RCT2_GLOBAL(0x01358838, uint32) & (1 << secondaryItem))
				w->pressed_widgets |= (1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
		} else {
			secondaryItem -= 32;
			if (RCT2_GLOBAL(0x0135934C, uint32) & (1 << secondaryItem))
				w->pressed_widgets |= (1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
		}

		// Show widgets
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_24;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_SPINNER;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_DROPDOWN_BUTTON;
		window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;

		// Set secondary item price
		window_ride_income_widgets[WIDX_SECONDARY_PRICE].image = 1799;
		RCT2_GLOBAL(0x013CE952 + 10, money32) = ride->price_secondary;
		if (ride->price_secondary == 0)
			window_ride_income_widgets[WIDX_SECONDARY_PRICE].image = STR_FREE;
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006ADCE5
 */
static void window_ride_income_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	rct_ride_type *rideEntry;
	rct_string_id stringId;
	money32 profit, costPerHour;
	int x, y, primaryItem, secondaryItem;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);
	rideEntry = ride_get_entry(ride);

	x = w->x + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].top + 29;

	// Primary item profit / loss per item sold
	primaryItem = (sint8)rideEntry->shop_item;
	if (primaryItem != -1) {
		profit = ride->price;

		stringId = STR_PROFIT_PER_ITEM_SOLD;
		profit -= primaryItem < 32 ?
			RCT2_GLOBAL(0x00982164 + (primaryItem * 8), uint16) :
			RCT2_GLOBAL(0x00982144 + (primaryItem * 8), uint16);
		if (profit < 0) {
			profit *= -1;
			stringId = STR_LOSS_PER_ITEM_SOLD;
		}

		gfx_draw_string_left(dpi, stringId, &profit, 0, x, y);
	}
	y += 39;

	// Secondary item profit / loss per item sold
	secondaryItem = RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8);
	if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
		secondaryItem = (sint8)rideEntry->shop_item_secondary;

	if (secondaryItem != -1) {
		profit = ride->price_secondary;

		stringId = STR_PROFIT_PER_ITEM_SOLD;
		profit -= secondaryItem < 32 ?
			RCT2_GLOBAL(0x00982164 + (secondaryItem * 8), uint16) :
			RCT2_GLOBAL(0x00982144 + (secondaryItem * 8), uint16);
		if (profit < 0) {
			profit *= -1;
			stringId = STR_LOSS_PER_ITEM_SOLD;
		}

		gfx_draw_string_left(dpi, stringId, &profit, 0, x, y);
	}
	y += 15;

	// Income per hour
	if (ride->income_per_hour != MONEY32_UNDEFINED) {
		gfx_draw_string_left(dpi, STR_INCOME_PER_HOUR, &ride->income_per_hour, 0, x, y);
		y += 10;
	}

	// Running cost per hour
	costPerHour = ride->upkeep_cost * 16;
	stringId = ride->upkeep_cost == 0xFFFF ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
	gfx_draw_string_left(dpi, stringId, &costPerHour, 0, x, y);
	y += 10;

	// Profit per hour
	if (ride->profit != MONEY32_UNDEFINED) {
		gfx_draw_string_left(dpi, STR_PROFIT_PER_HOUR, &ride->profit, 0, x, y);
		y += 10;
	}
	y += 5;

	// Total profit
	gfx_draw_string_left(dpi, STR_TOTAL_PROFIT, &ride->total_profit, 0, x, y);
}

#pragma endregion

#pragma region Customer

/**
 * 
 * rct2: 0x006AD986
 */
static void window_ride_customer_mouseup(rct_window *w, int widgetIndex)
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
		window_guest_list_open_with_filter(2, w->number);
		break;
	case WIDX_SHOW_GUESTS_ON_RIDE:
		window_guest_list_open_with_filter(0, w->number);
		break;
	case WIDX_SHOW_GUESTS_QUEUING:
		window_guest_list_open_with_filter(1, w->number);
		break;
	}
}

/**
 * 
 * rct2: 0x006ADA29
 */
static void window_ride_customer_resize(rct_window *w)
{
	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 139, 316, 139);
}

/**
 * 
 * rct2: 0x006AD9DD
 */
static void window_ride_customer_update(rct_window *w)
{
	rct_ride *ride;

	w->var_492++;
	if (w->var_492 >= 24)
		w->var_492 = 0;

	window_event_invalidate_call(w);
	widget_invalidate(w, WIDX_TAB_10);

	ride = GET_RIDE(w->number);
	if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_CUSTOMER) {
		ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_CUSTOMER;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006AD5F8
 */
static void window_ride_customer_invalidate(rct_window *w)
{
	rct_widget *widgets;

	colour_scheme_update(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

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
 * rct2: 0x006AD6CD
 */
static void window_ride_customer_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
	rct_ride *ride;
	int x, y;
	uint8 shopItem;
	sint16 popularity, satisfaction, queueTime, age;
	sint32 customersPerHour;
	rct_string_id stringId;

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);
	x = w->x + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].left + 4;
	y = w->y + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].top + 4;

	// Customers per hour
	customersPerHour = ride->var_124 + ride->var_126 + ride->var_128 + ride->var_12A + ride->var_12C +
					   ride->var_12E + ride->age + ride->running_cost + ride->var_134 + ride->var_136;
	customersPerHour *= 12;
	gfx_draw_string_left(dpi, STR_CUSTOMERS_PER_HOUR, &customersPerHour, 0, x, y);
	y += 10;

	// Popularity
	popularity = ride->popularity;
	if (popularity == 255) {
		stringId = STR_POPULARITY_UNKNOWN;
	} else {
		stringId = STR_POPULARITY_PERCENT;
		popularity *= 4;
	}
	gfx_draw_string_left(dpi, stringId, &popularity, 0, x, y);
	y += 10;

	// Satisfaction
	satisfaction = ride->satisfaction;
	if (satisfaction == 255) {
		stringId = STR_SATISFACTION_UNKNOWN;
	} else {
		stringId = STR_SATISFACTION_PERCENT;
		satisfaction *= 5;
	}
	gfx_draw_string_left(dpi, stringId, &satisfaction, 0, x, y);
	y += 10;

	// Queue time
	queueTime = ride_get_max_queue_time(ride);
	stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
	y += gfx_draw_string_left_wrapped(dpi, &queueTime, x, y, 308, stringId, 0);
	y += 5;

	// Primary shop items sold
	shopItem = ride_get_entry(ride)->shop_item;
	if (shopItem != 0xFF) {
		stringId = 2016 + shopItem;
		if (stringId >= 2048)
			stringId += 96;

		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stringId;
		RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->no_primary_items_sold;
		gfx_draw_string_left(dpi, STR_ITEMS_SOLD, (void*)0x013CE952, 0, x, y);
		y += 10;
	}
	
	// Secondary shop items sold / on-ride photos sold
	shopItem = ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO ?
		RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8) :
		ride_get_entry(ride)->shop_item_secondary;
	if (shopItem != 0xFF) {
		stringId = 2016 + shopItem;
		if (stringId >= 2048)
			stringId += 96;

		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stringId;
		RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->no_secondary_items_sold;
		gfx_draw_string_left(dpi, STR_ITEMS_SOLD, (void*)0x013CE952, 0, x, y);
		y += 10;
	}

	// Total customers
	gfx_draw_string_left(dpi, STR_TOTAL_CUSTOMERS, &ride->total_customers, 0, x, y);
	y += 10;

	// Guests favourite
	if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE) {
		stringId = ride->guests_favourite == 1 ?
			STR_FAVOURITE_RIDE_OF_GUEST :
			STR_FAVOURITE_RIDE_OF_GUESTS;
		gfx_draw_string_left(dpi, stringId, &ride->guests_favourite, 0, x, y);
		y += 10;
	}
	y += 2;

	// Age
	//If the ride has a build date that is in the future, show it as built this year.
	age = max((RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date) / 8, 0);
	stringId = age == 0 ?
		STR_BUILT_THIS_YEAR :
		age == 1 ?
			STR_BUILT_LAST_YEAR :
			STR_BUILT_YEARS_AGO;
	gfx_draw_string_left(dpi, stringId, &age, 0, x, y);
}

#pragma endregion
