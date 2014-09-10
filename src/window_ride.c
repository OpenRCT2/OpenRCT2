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

#include <string.h>
#include "addresses.h"
#include "game.h"
#include "map.h"
#include "ride.h"
#include "ride_data.h"
#include "staff.h"
#include "string_ids.h"
#include "sprite.h"
#include "sprites.h"
#include "viewport.h"
#include "widget.h"
#include "window.h"
#include "window_dropdown.h"

enum WINDOW_PARK_PAGE {
	WINDOW_PARK_PAGE_MAIN,
	WINDOW_PARK_PAGE_VEHICLE,
	WINDOW_PARK_PAGE_OPERATING,
	WINDOW_PARK_PAGE_MAINTENANCE,
	WINDOW_PARK_PAGE_COLOUR,
	WINDOW_PARK_PAGE_MUSIC,
	WINDOW_PARK_PAGE_MEASUREMENTS,
	WINDOW_PARK_PAGE_GRAPHS,
	WINDOW_PARK_PAGE_INCOME,
	WINDOW_PARK_PAGE_CUSTOMER,
	WINDOW_PARK_PAGE_COUNT
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

	WIDX_TRACK_PREVIEW = 14,
	WIDX_TRACK_COLOUR_SCHEME,
	WIDX_TRACK_COLOUR_SCHEME_DROPDOWN,
	WIDX_TRACK_MAIN_COLOUR,
	WIDX_TRACK_ADDITIONAL_COLOUR,
	WIDX_TRACK_SUPPORT_COLOUR,
	WIDX_COLOUR_20,
	WIDX_COLOUR_21,
	WIDX_PAINT_INDIVIDUAL_AREA,
	WIDX_ENTRANCE_PREVIEW,
	WIDX_STATION_STYLE,
	WIDX_STATION_STYLE_DROPDOWN,
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
	(rct_widget*)0x009ADDA8,
	window_ride_operating_widgets,
	window_ride_maintenance_widgets,
	window_ride_colour_widgets,
	window_ride_music_widgets,
	window_ride_measurements_widgets,
	(rct_widget*)0x009AE710,
	window_ride_income_widgets,
	window_ride_customer_widgets
};

const uint64 window_ride_page_enabled_widgets[] = {
	0x00000000007DBFF4,
	0x00000000001EFFF4,
	0x0000019E777DBFF4,
	0x000000000001FFF4,
	0x00000003F37F3FF4,
	0x000000000001FFF4,
	0x000000000007FFF4,
	0x000000000007BFF4,
	0x0000000000E73FF4,
	0x000000000001FFF4
};

#pragma endregion

#pragma region Events

static void window_ride_emptysub() { }

static void window_ride_main_mouseup();
static void window_ride_main_resize();
static void window_ride_main_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_main_dropdown();
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput();
static void window_ride_main_unknown_14();
static void window_ride_main_invalidate();
static void window_ride_main_paint();

static void window_ride_operating_mouseup();
static void window_ride_operating_resize();
static void window_ride_operating_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_operating_dropdown();
static void window_ride_operating_update(rct_window *w);
static void window_ride_operating_invalidate();
static void window_ride_operating_paint();

static void window_ride_maintenance_mouseup();
static void window_ride_maintenance_resize();
static void window_ride_maintenance_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_maintenance_dropdown();
static void window_ride_maintenance_update(rct_window *w);
static void window_ride_maintenance_invalidate();
static void window_ride_maintenance_paint();

static void window_ride_colour_close();
static void window_ride_colour_mouseup();
static void window_ride_colour_resize();
static void window_ride_colour_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_colour_dropdown();
static void window_ride_colour_update(rct_window *w);
static void window_ride_colour_toolupdate();
static void window_ride_colour_tooldown();
static void window_ride_colour_tooldrag();
static void window_ride_colour_invalidate();
static void window_ride_colour_paint();
static void window_ride_colour_scrollpaint();

static void window_ride_music_mouseup();
static void window_ride_music_resize();
static void window_ride_music_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_music_dropdown();
static void window_ride_music_update(rct_window *w);
static void window_ride_music_invalidate();
static void window_ride_music_paint();

static void window_ride_measurements_close();
static void window_ride_measurements_mouseup();
static void window_ride_measurements_resize();
static void window_ride_measurements_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_measurements_dropdown();
static void window_ride_measurements_update(rct_window *w);
static void window_ride_measurements_tooldown();
static void window_ride_measurements_toolabort();
static void window_ride_measurements_invalidate();
static void window_ride_measurements_paint();

static void window_ride_income_mouseup();
static void window_ride_income_resize();
static void window_ride_income_mousedown(int widgetIndex, rct_window *w, rct_widget *widget);
static void window_ride_income_update(rct_window *w);
static void window_ride_income_invalidate();
static void window_ride_income_paint();

static void window_ride_customer_mouseup();
static void window_ride_customer_resize();
static void window_ride_customer_update(rct_window *w);
static void window_ride_customer_invalidate();
static void window_ride_customer_paint();

// 0x0098DFD4
static void* window_ride_main_events[] = {
	window_ride_emptysub,
	window_ride_main_mouseup,
	window_ride_main_resize,
	window_ride_main_mousedown,
	window_ride_main_dropdown,
	window_ride_emptysub,
	window_ride_main_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_main_textinput,
	window_ride_main_unknown_14,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_main_invalidate,
	window_ride_main_paint,
	window_ride_emptysub
};

// 0x0098E0B4
static void* window_ride_operating_events[] = {
	window_ride_emptysub,
	window_ride_operating_mouseup,
	window_ride_operating_resize,
	window_ride_operating_mousedown,
	window_ride_operating_dropdown,
	window_ride_emptysub,
	window_ride_operating_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_operating_invalidate,
	window_ride_operating_paint,
	window_ride_emptysub
};

// 0x0098E124
static void* window_ride_maintenance_events[] = {
	window_ride_emptysub,
	window_ride_maintenance_mouseup,
	window_ride_maintenance_resize,
	window_ride_maintenance_mousedown,
	window_ride_maintenance_dropdown,
	window_ride_emptysub,
	window_ride_maintenance_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_maintenance_invalidate,
	window_ride_maintenance_paint,
	window_ride_emptysub
};

// 0x0098E044
static void* window_ride_colour_events[] = {
	window_ride_colour_close,
	window_ride_colour_mouseup,
	window_ride_colour_resize,
	window_ride_colour_mousedown,
	window_ride_colour_dropdown,
	window_ride_emptysub,
	window_ride_colour_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_colour_toolupdate,
	window_ride_colour_tooldown,
	window_ride_colour_tooldrag,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_colour_invalidate,
	window_ride_colour_paint,
	window_ride_colour_scrollpaint
};

// 0x0098E194
static void* window_ride_music_events[] = {
	window_ride_emptysub,
	window_ride_music_mouseup,
	window_ride_music_resize,
	window_ride_music_mousedown,
	window_ride_music_dropdown,
	window_ride_emptysub,
	window_ride_music_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_music_invalidate,
	window_ride_music_paint,
	window_ride_emptysub
};

// 0x0098DE14
static void* window_ride_measurements_events[] = {
	window_ride_emptysub,
	window_ride_measurements_mouseup,
	window_ride_measurements_resize,
	window_ride_measurements_mousedown,
	window_ride_measurements_dropdown,
	window_ride_emptysub,
	window_ride_measurements_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_measurements_tooldown,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_measurements_toolabort,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_measurements_invalidate,
	window_ride_measurements_paint,
	window_ride_emptysub
};

// 0x0098DEF4
static void* window_ride_income_events[] = {
	window_ride_emptysub,
	window_ride_income_mouseup,
	window_ride_income_resize,
	window_ride_income_mousedown,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_income_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_income_invalidate,
	window_ride_income_paint,
	window_ride_emptysub
};

// 0x0098DE84
static void* window_ride_customer_events[] = {
	window_ride_emptysub,
	window_ride_customer_mouseup,
	window_ride_customer_resize,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_customer_update,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_emptysub,
	window_ride_customer_invalidate,
	window_ride_customer_paint,
	window_ride_emptysub
};

static uint32* window_ride_page_events[] = {
	(uint32*)window_ride_main_events,
	(uint32*)0x0098E204,
	(uint32*)window_ride_operating_events,
	(uint32*)window_ride_maintenance_events,
	(uint32*)window_ride_colour_events,
	(uint32*)window_ride_music_events,
	(uint32*)window_ride_measurements_events,
	(uint32*)0x0098DF64,
	(uint32*)window_ride_income_events,
	(uint32*)window_ride_customer_events
};

#pragma endregion

const int window_ride_tab_animation_divisor[] = { 0, 0, 2, 2, 4, 2, 8, 8, 2, 0 };
const int window_ride_tab_animation_frames[] = { 0, 0, 4, 16, 8, 16, 8, 8, 8, 0 };

// WINDOW_PARK_PAGE_MAIN,
// WINDOW_PARK_PAGE_VEHICLE,
// WINDOW_PARK_PAGE_OPERATING,
// WINDOW_PARK_PAGE_MAINTENANCE,
// WINDOW_PARK_PAGE_COLOUR,
// WINDOW_PARK_PAGE_MUSIC,
// WINDOW_PARK_PAGE_MEASUREMENTS,
// WINDOW_PARK_PAGE_GRAPHS,
// WINDOW_PARK_PAGE_INCOME,
// WINDOW_PARK_PAGE_CUSTOMER

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
	int widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_MAIN;

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		int spriteIndex;
		int rideType = GET_RIDE(w->number)->type;

		switch (gRideClassifications[rideType]) {
		case RIDE_CLASS_RIDE:
			spriteIndex = 5442;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_SHOP_OR_STALL:
			spriteIndex = 5351;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
				spriteIndex += (w->frame_no / 4) % 16;
			break;
		case RIDE_CLASS_KIOSK_OR_FACILITY:
			spriteIndex = 5367;
			if (w->page == WINDOW_PARK_PAGE_MAIN)
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
	int widgetIndex, spriteIndex, colour, x, y, width, height;
	uint8 *ebp;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;

	widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_VEHICLE;
	widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		x = widget->left + 1;
		y = widget->top + 1;
		width = widget->right - x;
		height = widget->bottom - 3 - y;
		if (w->page == WINDOW_PARK_PAGE_VEHICLE)
			height += 4;

		x += w->x;
		y += w->y;

		dpi = clip_drawpixelinfo(dpi, x, width, y, height);
		if (dpi == NULL)
			return;

		x = (widget->right - widget->left) / 2;
		y = (widget->bottom - widget->top) - 12;

		ride = GET_RIDE(w->number);
		RCT2_CALLPROC_X(0x006DE4CD, (ride->var_0C9 << 8) | ride->subtype, 0, 0, 0, 0, 0, 0);

		rideEntry = rideEntries[ride->subtype];
		if (rideEntry->var_008 & 1) {
			dpi->zoom_level = 1;
			dpi->width *= 2;
			dpi->height *= 2;
			x *= 2;
			y *= 2;
			dpi->x *= 2;
			dpi->y *= 2;
		}

		ebp = (uint8*)rideEntry + (RCT2_ADDRESS(0x00F64E38, uint8)[rideEntry->var_013] * 101);
		height += RCT2_GLOBAL(ebp + 0x24, sint8);

		RCT2_GLOBAL(0x00F43480, uint32) = ride->var_1D4;
		colour = ((ride->vehicle_colours[0] & 0xFF00) << 16) | ((ride->vehicle_colours[0] & 0xFF) << 19);
		spriteIndex = 32;
		if (w->page == WINDOW_PARK_PAGE_VEHICLE)
			spriteIndex += w->frame_no;
		spriteIndex /= 2;
		if (RCT2_GLOBAL(ebp + 0x2C, uint16) & 0x800)
			spriteIndex /= 2;
		spriteIndex &= RCT2_GLOBAL(ebp + 0x1A, uint16);
		spriteIndex *= RCT2_GLOBAL(ebp + 0x30, uint16);
		spriteIndex += RCT2_GLOBAL(ebp + 0x32, uint32);
		spriteIndex |= colour;
		spriteIndex |= 0x80000000;

		gfx_draw_sprite(dpi, spriteIndex, x, y, RCT2_GLOBAL(0x00F43480, uint32));
	}
}

/**
 * 
 * rct2: 0x006B2F42
 */
static void window_ride_draw_tab_customer(rct_drawpixelinfo *dpi, rct_window *w)
{
	int spriteIndex;
	int widgetIndex = WIDX_TAB_1 + WINDOW_PARK_PAGE_CUSTOMER;
	rct_widget *widget = &w->widgets[widgetIndex];

	if (!(w->disabled_widgets & (1LL << widgetIndex))) {
		spriteIndex = 0;
		if (w->page == WINDOW_PARK_PAGE_CUSTOMER)
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
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_OPERATING, 5201);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MAINTENANCE, 5205);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_INCOME, 5253);
	window_ride_draw_tab_main(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MEASUREMENTS, 5229);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_COLOUR, 5221);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_GRAPHS, 5237);
	window_ride_draw_tab_customer(dpi, w);
	window_ride_draw_tab_image(dpi, w, WINDOW_PARK_PAGE_MUSIC, 5335);
}

/**
 * 
 * rct2: 0x006AEAB4
 */
rct_window *window_ride_open(int rideIndex)
{
	rct_window *w;

	w = window_create_auto_pos(316, 180, window_ride_page_events[0], WC_RIDE, 0x400);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = window_ride_page_enabled_widgets[0];
	w->number = rideIndex;

	w->page = 0;
	w->var_48C = 0;
	w->frame_no = 0;
	w->list_information_type = 0;
	w->var_492 = 0;
	w->var_494 = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	w->min_width = 316;
	w->min_height = 180;
	w->max_width = 500;
	w->max_height = 450;
	w->flags |= WF_RESIZABLE;
	w->colours[0] = 1;
	w->colours[1] = 26;
	w->colours[2] = 11;

	rct_ride *ride = &g_ride_list[rideIndex];
	uint8 *edx = (uint8*)0x009E32F8;
	if (ride->type != RIDE_TYPE_NULL) {
		int rideType = ride->type;
		do {
			edx++;
			if (*(edx - 1) != 0xFF)
				continue;
		} while (rideType-- != 0);
	}

	int eax, ebx = 0, ecx;
	while (*edx != 0xFF) {
		eax = *edx++;
		ecx = eax >> 5;
		eax &= 0x1F;
		if (!(RCT2_ADDRESS(0x001357424, uint32)[ecx] & (1 << eax)))
			continue;
		ebx++;
	}
	RCT2_GLOBAL((int)w + 496, uint16) = ebx;
	return w;
}

/**
 * 
 * rct2: 0x006ACC28
 */
void window_ride_main_open(int rideIndex)
{
	rct_window *w;

	w = window_bring_to_front_by_id(WC_RIDE, rideIndex);
	if (w == NULL) {
		w = window_ride_open(rideIndex);
		w->ride.var_482 = -1;
	}

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)) {
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) &&
			w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber)
		) {
			tool_cancel();
		}
	}

	w->page = 0;
	w->width = 316;
	w->height = 180;
	window_invalidate(w);
	w->widgets = window_ride_page_widgets[0];
	w->enabled_widgets = 0x007DBFF4;
	w->var_020 = 0;
	w->event_handlers = window_ride_page_events[0];
	w->pressed_widgets = 0;
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	window_init_scroll_widgets(w);
	w->ride.view = 0;
	RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 *
 *  rct2: 0x006AF1D2
 */
static void window_ride_set_page(rct_window *w, int page)
{
	int listen;

	if (RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3))
		if (w->classification == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWCLASS, rct_windowclass) && w->number == RCT2_GLOBAL(RCT2_ADDRESS_TOOL_WINDOWNUMBER, rct_windownumber))
			tool_cancel();

	// Set listen only to viewport
	listen = 0;
	if (page == WINDOW_PARK_PAGE_MAIN && w->page == WINDOW_PARK_PAGE_MAIN && w->viewport != NULL && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
		listen++;

	w->page = page;
	w->frame_no = 0;
	w->var_492 = 0;
	if (w->viewport != NULL) {
		w->viewport->width = 0;
		w->viewport = NULL;
	}

	w->enabled_widgets = window_ride_page_enabled_widgets[page];
	w->var_020 = RCT2_ADDRESS(0x0098DD68, uint32)[page];
	w->event_handlers = window_ride_page_events[page];
	w->widgets = window_ride_page_widgets[page];
	RCT2_CALLPROC_X(0x006AEB9F, 0, 0, 0, 0, (int)w, 0, 0);
	window_invalidate(w);

	RCT2_CALLPROC_X(w->event_handlers[WE_RESIZE], 0, 0, 0, 0, (int)w, 0, 0);
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);

	window_init_scroll_widgets(w);
	window_invalidate(w);

	if (listen != 0 && w->viewport != NULL)
		w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void window_ride_set_pressed_tab(rct_window *w)
{
	int i;
	for (i = 0; i < WINDOW_PARK_PAGE_COUNT; i++)
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
	RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006B4971
 */
static void window_ride_construct(rct_window *w)
{
	int rideIndex = w->number;

	window_close_by_id(WC_RIDE_CONSTRUCTION | 0x80, rideIndex);
	w = window_find_by_id(WC_RIDE, rideIndex);
	if (w == NULL)
		return;

	rct_map_element *trackMapElement;
	int trackX, trackY;

	trackMapElement = sub_6CAF80(rideIndex, &trackX, &trackY);
	if (trackMapElement == (rct_map_element*)-1) {
		RCT2_CALLPROC_X(0x006CC3FB, 0, 0, 0, rideIndex, 0, 0, 0);
	} else {
		trackMapElement = ride_find_track_gap(trackMapElement, &trackX, &trackY);

		w = window_get_main();
		if (w != NULL && ride_try_construct(trackMapElement))
			window_scroll_to_location(w, trackX, trackY, trackMapElement->base_height * 8);
	}
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
	window_show_textinput(w, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, ride->name);
}

/**
 * 
 * rct2: 0x006AF3B3
 */
static void window_ride_locate(rct_window *w)
{
	rct_window *mainWindow;
	int xy, x, y, z;

	if (w->viewport->width == 0)
		return;

	xy = w->ride.var_482;
	z = w->ride.var_486;
	if (xy == -1)
		return;

	if (xy & 0x80000000) {
		rct_sprite *sprite = &g_sprite_list[xy];
		x = sprite->unknown.x;
		y = sprite->unknown.y;
		z = sprite->unknown.z;
	} else {
		x = (xy & ~0xC0000000) & 0xFFFF;
		y = (xy & ~0xC0000000) >> 16;
		z = z >> 16;
	}

	mainWindow = window_get_main();
	if (mainWindow != NULL)
		window_scroll_to_location(mainWindow, x, y, z);
}

/**
 * 
 * rct2: 0x006B486A
 */
static void window_ride_demolish(rct_window *w)
{
	rct_window *demolishWindow;
	int x, y, screenWidth, screenHeight;

	demolishWindow = window_bring_to_front_by_id(WC_DEMOLISH_RIDE_PROMPT, w->number);
	if (demolishWindow != NULL)
		return;

	screenWidth = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_WIDTH, sint16);
	screenHeight = RCT2_GLOBAL(RCT2_ADDRESS_SCREEN_HEIGHT, sint16);
	x = screenWidth / 2 - 100;
	y = max(28, screenHeight / 2 - 50);

	demolishWindow = window_create(x, y, 200, 100, (uint32*)0x0098E2E4, WC_DEMOLISH_RIDE_PROMPT, 0);
	demolishWindow->widgets = (rct_widget*)0x009AEBA0;
	demolishWindow->enabled_widgets = 4 | 8 | 16;
	window_init_scroll_widgets(demolishWindow);
	demolishWindow->flags |= WF_TRANSPARENT;
	demolishWindow->number = w->number;
	demolishWindow->colours[0] = 154;
}

/**
 * 
 * rct2: 0x006AF17E
 */
static void window_ride_main_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
		window_ride_locate(w);
		break;
	case WIDX_DEMOLISH:
		window_ride_demolish(w);
		break;
	}
}

/**
 * 
 * rct2: 0x006AF4A2
 */
static void window_ride_main_resize()
{
	rct_window *w;

	window_get_register(w);

	w->flags |= WF_RESIZABLE;
	window_set_resize(w, 316, 180, 500, 450);
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
	if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x2000)) {
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

	if (!(RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800)) {
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
		if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800)
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

	if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800) {
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
static void window_ride_main_dropdown()
{
	rct_ride *ride;
	rct_window *w;
	int status;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

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
		RCT2_CALLPROC_X(0x006AF994, 0, 0, 0, 0, (int)w, 0, 0);
		window_invalidate(w);
		break;
	case WIDX_OPEN:
		if (dropdownIndex == -1)
			dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

		ride = GET_RIDE(w->number);
		if ((RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x800) && dropdownIndex != 0)
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

		RCT2_GLOBAL(0x013CE952 + 6, uint16) = ride->overall_view;
		RCT2_GLOBAL(0x013CE952 + 8, uint32) = ride->name_arguments;
		game_do_command(0, 1, 0, w->number | (status << 8), GAME_COMMAND_SET_RIDE_OPEN, 0, 0);
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
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_1);

	// Update status
	ride = GET_RIDE(w->number);
	if (!(ride->var_14D & 4)) {
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

	ride->var_14D &= ~4;
	widget_invalidate(WC_RIDE, w->number, WIDX_STATUS);
}

/**
 * 
 * rct2: 0x006AF2F9
 */
static void window_ride_main_textinput()
{
	uint8 result;
	short widgetIndex;
	rct_window *w;
	char *text;

	window_textinput_get_registers(w, widgetIndex, result, text);

	if (widgetIndex != WIDX_RENAME || !result)
		return;

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_RENAME_RIDE_ATTRACTION;
	game_do_command(1, (w->number << 8) | 1, 0, *((int*)(text +  0)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text +  8)), *((int*)(text +  4)));
	game_do_command(2, (w->number << 8) | 1, 0, *((int*)(text + 12)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text + 20)), *((int*)(text + 16)));
	game_do_command(0, (w->number << 8) | 1, 0, *((int*)(text + 24)), GAME_COMMAND_SET_RIDE_NAME, *((int*)(text + 32)), *((int*)(text + 28)));
}

/**
 * 
 * rct2: 0x006AF55A
 */
static void window_ride_main_unknown_14()
{
	rct_window *w;

	window_get_register(w);

	window_ride_init_viewport(w);
}

/**
 * 
 * rct2: 0x006AECF6
 */
static void window_ride_main_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	int i;

	window_get_register(w);

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
	if (ride->lifecycle_flags & RIDE_LIFECYCLE_19)
		w->disabled_widgets |= (1 << 19);

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;
	window_ride_main_widgets[WIDX_OPEN].image = SPR_CLOSED + ride->status;

	window_ride_anchor_border_widgets(w);

	// Anchor main page specific widgets
	window_ride_main_widgets[WIDX_VIEWPORT].right = w->width - 26;
	window_ride_main_widgets[WIDX_VIEWPORT].bottom = w->height - 14;
	window_ride_main_widgets[WIDX_STATUS].right = w->width - 26;
	window_ride_main_widgets[WIDX_STATUS].top = w->height - 13;
	window_ride_main_widgets[WIDX_STATUS].bottom = w->height - 3;
	for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++) {
		window_ride_main_widgets[i].left = w->width - 25;
		window_ride_main_widgets[i].right = w->width - 2;
	}
	window_ride_main_widgets[WIDX_VIEW].right = w->width - 60;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = w->width - 61;
	window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = w->width - 71;

	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
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
		stringId = STR_COST;

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
			if (!(RCT2_ADDRESS(0x01357644, uint32)[ride->type] & 0x40) && vehicle->velocity == 0) {
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

	if ((RCT2_GLOBAL(0x0097D4F2 + (ride->type * 8), uint16) & 0x100) && stringId > 1104)
		stringId += 23;

	RCT2_GLOBAL((int)arguments + 4, uint16) = RideNameConvention[ride->type].station_name;
	RCT2_GLOBAL((int)arguments + 6, uint16) = vehicle->var_4B + 1;
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
static void window_ride_main_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	rct_widget *widget;
	rct_string_id stringId;

	window_paint_get_registers(w, dpi);

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
		w->y + widget->top - 1,
		widget->right - widget->left
	);
}

#pragma endregion

#pragma region Operating

static void set_operating_setting(int rideNumber, uint8 setting, uint8 value)
{
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (value << 8) | 1, 0, (setting << 8) | rideNumber, GAME_COMMAND_11, 0, 0);
}

static void window_ride_mode_tweak_set(rct_window *w, uint8 value)
{
	rct_ride *ride = GET_RIDE(w->number);

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1362;
	if (ride->mode == RIDE_MODE_STATION_TO_STATION)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1361;
	if (ride->mode == RIDE_MODE_RACE)
		RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = 1738;
	if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x2000)
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

	game_do_command(0, (value << 8) | 1, 0, (4 << 8) | w->number, GAME_COMMAND_11, 0, 0);
}

/**
 * 
 * rct2: 0x006B11D5
 */
static void window_ride_mode_tweak_increase(rct_window *w)
{
	rct_ride *ride = GET_RIDE(w->number);
	uint8 value = ride->var_0D0;
	if (value < RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8) + 5, uint8))
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
	uint8 value = ride->var_0D0;
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
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_widget *dropdownWidget;
	rct_ride *ride;
	const uint8 *availableModes, *mode;
	int i, numAvailableModes;

	dropdownWidget = widget - 1;
	ride = GET_RIDE(w->number);
	rideEntry = rideEntries[ride->subtype];

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
	if (rideEntry->var_008 & 0x8000)
		numAvailableModes--;

	// ?
	if (rideEntry->var_008 & 0x20000) {
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
		0,
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
		0,
		5,
		widget->right - dropdownWidget->left
	);

	gDropdownItemsChecked = (1 << (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK));
}

/**
 * 
 * rct2: 0x006B10A7
 */
static void window_ride_operating_mouseup()
{
	short widgetIndex;
	rct_window *w;
	rct_ride *ride;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_operating_resize()
{
	rct_window *w;

	window_get_register(w);

	window_set_resize(w, 316, 186, 316, 186);
}

/**
 * 
 * rct2: 0x006B10F4
 */
static void window_ride_operating_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_ride *ride = GET_RIDE(w->number);

	switch (widgetIndex) {
	case WIDX_MODE_TWEAK_INCREASE:
		window_ride_mode_tweak_increase(w);
		break;
	case WIDX_MODE_TWEAK_DECREASE:
		window_ride_mode_tweak_decrease(w);
		break;
	case WIDX_LIFT_HILL_SPEED_INCREASE:
		set_operating_setting(w->number, 8, min(ride->lift_hill_speed + 1, RCT2_GLOBAL(0x0097D7CA + (ride->type * 4), uint8)));
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
		set_operating_setting(w->number, 9, min(ride->num_circuits + 1, 7));
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
static void window_ride_operating_dropdown()
{
	rct_window *w;
	short widgetIndex, dropdownIndex;
	rct_ride *ride;
	const uint8 *availableModes;
	int i;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

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
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_3);

	ride = GET_RIDE(w->number);
	if (ride->var_14D & 10) {
		ride->var_14D &= ~10;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006B0B30
 */
static void window_ride_operating_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	rct_ride *ride;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_string_id format, caption, tooltip;

	window_get_register(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);
	rideEntry = rideEntries[ride->subtype];

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// Widget setup
	w->pressed_widgets &= ~0x44700000;

	// Lift hill speed
	if ((rideEntry->var_1B6 & RCT2_ADDRESS(0x01357444, uint32)[ride->type]) & 8) {
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
		(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x10) &&
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x20) {
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
	if (RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x4000) {
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
	RCT2_GLOBAL(0x013CE964, uint16) = ride->var_0D0;
	switch (ride->mode) {
	case RIDE_MODE_POWERED_LAUNCH:
	case RIDE_MODE_POWERED_LAUNCH_35:
	case RIDE_MODE_UPWARD_LAUNCH:
	case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
		RCT2_GLOBAL(0x013CE964, uint16) = (ride->var_0D0 * 9) / 4;
		format = 1331;
		caption = STR_LAUNCH_SPEED;
		tooltip = STR_LAUNCH_SPEED_TIP;
	case RIDE_MODE_STATION_TO_STATION:
		RCT2_GLOBAL(0x013CE964, uint16) = (ride->var_0D0 * 9) / 4;
		caption = STR_SPEED;
		tooltip = STR_SPEED_TIP;
		break;
	case RIDE_MODE_RACE:
		RCT2_GLOBAL(0x013CE964, uint16) = ride->var_0D0;
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
		if (!(RCT2_GLOBAL(RCT2_ADDRESS_RIDE_FLAGS + (ride->type * 8), uint32) & 0x2000))
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
static void window_ride_operating_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	uint16 blockSections;

	window_paint_get_registers(w, dpi);

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
	rct_peep *peep;
	rct_ride *ride = GET_RIDE(w->number);

	peep = ride_get_assigned_mechanic(ride);
	if (peep != NULL) {
		window_staff_peep_open(peep);
	} else {
		// Presumebly looks for the closest mechanic
		RCT2_CALLPROC_X(0x006B1B3E, 0, w->number * 0x260, 0, 0, (int)w, 0, 0);
	}
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
		if (RCT2_GLOBAL(0x009DEA6E, uint8) == 0 && (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_TICKS, uint32) & 8))
			return;
	}

	value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
	if (value > 2) {
		gfx_fill_rect_inset(dpi, x + 2, y + 1, x + value + 1, y + 8, unk, 0);
	}
}

/**
 * 
 * rct2: 0x006B1AAD
 */
static void window_ride_maintenance_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_maintenance_resize()
{
	rct_window *w;

	window_get_register(w);

	window_set_resize(w, 316, 135, 316, 135);
}

/**
 * 
 * rct2: 0x006B1ACE
 */
static void window_ride_maintenance_mousedown(int widgetIndex, rct_window *w, rct_widget *widget)
{
	rct_widget *dropdownWidget;
	int i;

	if (widgetIndex != WIDX_INSPECTION_INTERVAL_DROPDOWN)
		return;

	dropdownWidget = widget - 1;
	rct_ride *ride = GET_RIDE(w->number);

	for (i = 0; i < 7; i++) {
		gDropdownItemsFormat[i] = 1142;
		gDropdownItemsArgs[i] = STR_EVERY_10_MINUTES + i;
	}
	window_dropdown_show_text_custom_width(
		w->x + dropdownWidget->left,
		w->y + dropdownWidget->top,
		dropdownWidget->bottom - dropdownWidget->top + 1,
		w->colours[1],
		0,
		7,
		widget->right - dropdownWidget->left
	);

	gDropdownItemsChecked = (1 << ride->inspection_interval);
}

/**
 * 
 * rct2: 0x006B1AD9
 */
static void window_ride_maintenance_dropdown()
{
	rct_window *w;
	rct_ride *ride;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_INSPECTION_INTERVAL_DROPDOWN || dropdownIndex == -1)
		return;

	ride = GET_RIDE(w->number);

	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (dropdownIndex << 8) | 1, 0, (5 << 8) | w->number, GAME_COMMAND_11, 0, 0);
}

/**
 * 
 * rct2: 0x006B1D37
 */
static void window_ride_maintenance_update(rct_window *w)
{
	rct_ride *ride;

	w->frame_no++;
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_4);

	ride = GET_RIDE(w->number);
	if (ride->var_14D & 20) {
		ride->var_14D &= ~20;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006B17C8
 */
static void window_ride_maintenance_invalidate()
{
	rct_window *w;
	rct_widget *widgets;

	window_get_register(w);

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
}

/**
 * 
 * rct2: 0x006B1877
 */
static void window_ride_maintenance_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	rct_widget *widget;
	rct_peep *mechanicSprite;
	int x, y;
	uint16 reliability, downTime, lastInspection;
	rct_string_id stringId, breakdownMessage;

	window_paint_get_registers(w, dpi);

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

	reliability = ride->var_196 >> 8;
	gfx_draw_string_left(dpi, STR_RELIABILITY_LABEL_1757, &reliability, 0, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, max(10, reliability), 14);
	y += 11;

	// Down time
	downTime = ride->var_199;
	gfx_draw_string_left(dpi, STR_DOWN_TIME_LABEL_1889, &downTime, 0, x, y);
	window_ride_maintenance_draw_bar(w, dpi, x + 103, y, downTime, 28);
	y += 26;

	// Last inspection
	lastInspection = ride->last_inspection;
	stringId = lastInspection <= 240 ?
		STR_TIME_SINCE_LAST_INSPECTION_MINUTES :
		STR_TIME_SINCE_LAST_INSPECTION_MORE_THAN_4_HOURS;
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

/**
 * 
 * rct2: 0x006B04FA
 */
static void window_ride_colour_close()
{
	rct_window *w;

	window_get_register(w);

	if (!(RCT2_GLOBAL(0x009DE518, uint32) & (1 << 3)))
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
static void window_ride_colour_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_colour_resize()
{
	rct_window *w;

	window_get_register(w);

	window_set_resize(w, 316, 207, 316, 207);
}

/**
 * 
 * rct2: 0x006B02C6
 */
static void window_ride_colour_mousedown(int widgetIndex, rct_window *w, rct_widget *widget) { }

/**
 * 
 * rct2: 0x006B0331
 */
static void window_ride_colour_dropdown() { }

/**
 * 
 * rct2: 0x006B0A8F
 */
static void window_ride_colour_update(rct_window *w)
{
	w->frame_no++;
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_5);
	widget_invalidate(WC_RIDE, w->number, WIDX_VEHICLE_PREVIEW);
}

/**
 * 
 * rct2: 0x006B04E5
 */
static void window_ride_colour_toolupdate() { }

/**
 * 
 * rct2: 0x006B04EC
 */
static void window_ride_colour_tooldown() { }

/**
 * 
 * rct2: 0x006B04F3
 */
static void window_ride_colour_tooldrag() { }

/**
 * 
 * rct2: 0x006AFB36
 */
static void window_ride_colour_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_ride *ride;

	window_get_register(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	ride = GET_RIDE(w->number);
	rideEntry = rideEntries[ride->subtype];

	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	// TODO widget setup

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006AFF3E
 */
static void window_ride_colour_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

/**
 * 
 * rct2: 0x006B0192
 */
static void window_ride_colour_scrollpaint() { }

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
	game_do_command(0, (activateMusic << 8) | 1, 0, (6 << 8) | w->number, GAME_COMMAND_11, 0, 0);
}

/**
 * 
 * rct2: 0x006B1ED7
 */
static void window_ride_music_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_music_resize()
{
	rct_window *w;

	window_get_register(w);

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

		if (RCT2_GLOBAL(0x009AF164, uint32) != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_1;
		if (RCT2_GLOBAL(0x009AF16E, uint32) != 0)
			window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_2;
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

	for (i = 0; i < numItems; i++) {
		gDropdownItemsFormat[i] = 1142;
		if (window_ride_current_music_style_order[i] == ride->music)
			gDropdownItemsChecked = (1 << i);
		gDropdownItemsArgs[i] = STR_MUSIC_STYLE_START + window_ride_current_music_style_order[i];
	}
}

/**
 * 
 * rct2: 0x006B1F03
 */
static void window_ride_music_dropdown()
{
	rct_window *w;
	rct_ride *ride;
	uint8 musicStyle;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_MUSIC_DROPDOWN || dropdownIndex == -1)
		return;

	ride = GET_RIDE(w->number);
	musicStyle = window_ride_current_music_style_order[dropdownIndex];
	RCT2_GLOBAL(RCT2_ADDRESS_GAME_COMMAND_ERROR_TITLE, uint16) = STR_CANT_CHANGE_OPERATING_MODE;
	game_do_command(0, (musicStyle << 8) | 1, 0, (7 << 8) | w->number, GAME_COMMAND_11, 0, 0);
}

/**
 * 
 * rct2: 0x006B2198
 */
static void window_ride_music_update(rct_window *w)
{
	w->frame_no++;
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_6);
}

/**
 * 
 * rct2: 0x006B1DEA
 */
static void window_ride_music_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	int isMusicActivated;

	window_get_register(w);

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
static void window_ride_music_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Measurements

/**
 * 
 * rct2: 0x006D3026
 */
static void window_ride_measurements_design_reset()
{
	RCT2_CALLPROC_EBPSAFE(0x006D3026);
}

/**
 * 
 * rct2: 0x006D303D
 */
static void window_ride_measurements_design_select_nearby_scenery()
{
	RCT2_CALLPROC_EBPSAFE(0x006D303D);
}

/**
 * 
 * rct2: 0x006AD4CD
 */
static void window_ride_measurements_design_save(rct_window *w)
{
	RCT2_CALLPROC_X(0x006D2804, 1, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_design_cancel()
{
	if (RCT2_GLOBAL(0x009DEA6F, uint8) & 1)
		RCT2_CALLPROC_X(0x006D2804, 0, 0, 0, 0, 0, 0, 0);
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_close()
{
	window_ride_measurements_design_cancel();
}

/**
 * 
 * rct2: 0x006AD478
 */
static void window_ride_measurements_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_measurements_resize()
{
	rct_window *w;

	window_get_register(w);

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
static void window_ride_measurements_dropdown()
{
	rct_window *w;
	short widgetIndex, dropdownIndex;

	window_dropdown_get_registers(w, widgetIndex, dropdownIndex);

	if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
		return;

	if (dropdownIndex == -1)
		dropdownIndex = RCT2_GLOBAL(0x009DEBA2, sint16);

	if (dropdownIndex == 0)
		RCT2_CALLPROC_X(0x006D264D, 0, 0, 0, 0, (int)w, 0, 0);
	else
		RCT2_CALLPROC_X(0x006D27A3, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AD5DD
 */
static void window_ride_measurements_update(rct_window *w)
{
	w->frame_no++;
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_7);
}

/**
 * 
 * rct2: 0x006AD4EB
 */
static void window_ride_measurements_tooldown()
{
	rct_window *w;

	window_get_register(w);

	RCT2_CALLPROC_X(0x006D2AE7, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AD4DA
 */
static void window_ride_measurements_toolabort(rct_window *w)
{
	window_ride_measurements_design_cancel();
}

/**
 * 
 * rct2: 0x006ACDBC
 */
static void window_ride_measurements_invalidate()
{
	rct_window *w;
	rct_widget *widgets;

	window_get_register(w);

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
		if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_19)) {
			if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x10000000) {
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
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006ACF07
 */
static void window_ride_measurements_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_widget *widget;
	rct_ride *ride;
	rct_string_id stringId;
	int x, y, i, numTimes, numLengths;
	sint16 holes, maxSpeed, averageSpeed, drops, highestDropHeight, inversions, time;
	sint32 maxPositiveVerticalGs, maxNegativeVerticalGs, maxLateralGs, totalAirTime, length;

	window_paint_get_registers(w, dpi);

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
		
		if (ride->lifecycle_flags & RIDE_LIFECYCLE_19)
			gfx_draw_sprite(dpi, 23225, w->x + w->width - 53, w->y + w->height - 73, 0);

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
					holes = ride->inversions & 0x1F;
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

				if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x80) {
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

				if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) & 0x400) {
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

#pragma region Income

/**
 * 
 * rct2: 0x006ADEFD
 */
static void window_ride_income_toggle_primary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006ADEFD, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AE06E
 */
static void window_ride_income_toggle_secondary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AE06E, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AE1E4
 */
static void window_ride_income_increase_primary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AE1E4, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AE237
 */
static void window_ride_income_decrease_primary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AE237, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AE269
 */
static void window_ride_income_increase_secondary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AE269, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006AE28D
 */
static void window_ride_income_decrease_secondary_price(rct_window *w)
{
	RCT2_CALLPROC_X(0x006AE28D, 0, 0, 0, 0, (int)w, 0, 0);
}

/**
 * 
 * rct2: 0x006ADEA9
 */
static void window_ride_income_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
static void window_ride_income_resize()
{
	rct_window *w;

	window_get_register(w);

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
	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_9);

	ride = GET_RIDE(w->number);
	if (ride->var_14D & 2) {
		ride->var_14D &= ~2;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006ADAA3
 */
static void window_ride_income_invalidate()
{
	rct_window *w;
	rct_widget *widgets;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_string_id stringId;
	int primaryItem, secondaryItem;

	window_get_register(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	rideEntry = rideEntries[ride->subtype];

	// Primary item
	w->pressed_widgets &= ~(1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
	w->disabled_widgets &= ~(1 << WIDX_PRIMARY_PRICE);
	if (
		!(RCT2_GLOBAL(RCT2_ADDRESS_PARK_FLAGS, uint32) & PARK_FLAGS_PARK_FREE_ENTRY) &&
		rideEntry->shop_item == 255 &&
		ride->type != RIDE_TYPE_BATHROOM
	) {
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
	if (ride->type != RIDE_TYPE_BATHROOM) {
		if ((primaryItem = (sint8)rideEntry->shop_item) != -1) {
			window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;
			if (primaryItem < 32) {
				if (RCT2_GLOBAL(0x01358838, uint32) & (1 << primaryItem))
					w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

				if (primaryItem != 31)
					window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].image = 1960 + primaryItem;
			} else {
				primaryItem -= 32;
				if (RCT2_GLOBAL(0x0135934C, uint32) & (1 << primaryItem))
					w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

				window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].image = 2100 + primaryItem;
			}
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
			if (RCT2_GLOBAL(0x0135884C, uint32) & (1 << secondaryItem))
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
static void window_ride_income_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	rct_ride_type *rideEntry, **rideEntries = (rct_ride_type**)0x009ACFA4;
	rct_string_id stringId;
	money32 profit, costPerHour;
	int x, y, primaryItem, secondaryItem;

	window_paint_get_registers(w, dpi);

	window_draw_widgets(w, dpi);
	window_ride_draw_tab_images(dpi, w);

	ride = GET_RIDE(w->number);
	rideEntry = rideEntries[ride->subtype];

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
		profit -= primaryItem < 32 ?
			RCT2_GLOBAL(0x00982164 + (primaryItem * 8), uint16) :
			RCT2_GLOBAL(0x00982144 + (primaryItem * 8), uint16);
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
static void window_ride_customer_mouseup()
{
	short widgetIndex;
	rct_window *w;

	window_widget_get_registers(w, widgetIndex);

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
		RCT2_CALLPROC_X(0x006993BA, 2, w->number, 0, 0, 0, 0, 0);
		break;
	case WIDX_SHOW_GUESTS_ON_RIDE:
		RCT2_CALLPROC_X(0x006993BA, 0, w->number, 0, 0, 0, 0, 0);
		break;
	case WIDX_SHOW_GUESTS_QUEUING:
		RCT2_CALLPROC_X(0x006993BA, 1, w->number, 0, 0, 0, 0, 0);
		break;
	}
}

/**
 * 
 * rct2: 0x006ADA29
 */
static void window_ride_customer_resize()
{
	rct_window *w;

	window_get_register(w);

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

	RCT2_CALLPROC_X(w->event_handlers[WE_INVALIDATE], 0, 0, 0, 0, (int)w, 0, 0);
	widget_invalidate(WC_RIDE, w->number, WIDX_TAB_10);

	ride = GET_RIDE(w->number);
	if (ride->var_14D & 1) {
		ride->var_14D &= ~1;
		window_invalidate(w);
	}
}

/**
 * 
 * rct2: 0x006AD5F8
 */
static void window_ride_customer_invalidate()
{
	rct_window *w;
	rct_widget *widgets;

	window_get_register(w);

	widgets = window_ride_page_widgets[w->page];
	if (w->widgets != widgets) {
		w->widgets = widgets;
		window_init_scroll_widgets(w);
	}

	window_ride_set_pressed_tab(w);

	rct_ride *ride = GET_RIDE(w->number);
	RCT2_GLOBAL(0x013CE952 + 0, uint16) = ride->name;
	RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->name_arguments;

	if (RCT2_GLOBAL(0x0097CF40 + (ride->type * 8), uint32) * 0x20000) {
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WWT_FLATBTN;
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WWT_FLATBTN;
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WWT_FLATBTN;
	} else {
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WWT_EMPTY;
		window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WWT_EMPTY;
	}

	window_ride_anchor_border_widgets(w);
	window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);
}

/**
 * 
 * rct2: 0x006AD6CD
 */
static void window_ride_customer_paint()
{
	rct_window *w;
	rct_drawpixelinfo *dpi;
	rct_ride *ride;
	int x, y;
	uint8 shopItem;
	sint16 popularity, satisfaction, queueTime, age;
	sint32 customersPerHour;
	rct_string_id stringId;
	rct_ride_type **rideEntries = (rct_ride_type**)0x009ACFA4;

	window_paint_get_registers(w, dpi);

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
	popularity = ride->var_158 & 0xFF;
	if (popularity == 255) {
		stringId = STR_POPULARITY_UNKNOWN;
	} else {
		stringId = STR_POPULARITY_PERCENT;
		popularity *= 4;
	}
	gfx_draw_string_left(dpi, stringId, &popularity, 0, x, y);
	y += 10;

	// Satisfaction
	satisfaction = ride->var_14A & 0xFF;
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
	shopItem = rideEntries[ride->subtype]->shop_item;
	if (shopItem != 0xFF) {
		stringId = 2016 + shopItem;
		if (stringId >= 2048)
			stringId += 96;

		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stringId;
		RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->var_1A4;
		gfx_draw_string_left(dpi, STR_ITEMS_SOLD, (void*)0x013CE952, 0, x, y);
		y += 10;
	}
	
	// Secondary shop items sold / on-ride photos sold
	shopItem = ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO ?
		RCT2_GLOBAL(0x0097D7CB + (ride->type * 4), uint8) :
		rideEntries[ride->subtype]->shop_item_secondary;
	if (shopItem != 0xFF) {
		stringId = 2016 + shopItem;
		if (stringId >= 2048)
			stringId += 96;

		RCT2_GLOBAL(0x013CE952 + 0, uint16) = stringId;
		RCT2_GLOBAL(0x013CE952 + 2, uint32) = ride->var_1A4;
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
	age = (RCT2_GLOBAL(RCT2_ADDRESS_CURRENT_MONTH_YEAR, uint16) - ride->build_date) / 8;
	stringId = age == 0 ?
		STR_BUILT_THIS_YEAR :
		age == 1 ?
			STR_BUILT_LAST_YEAR :
			STR_BUILT_YEARS_AGO;
	gfx_draw_string_left(dpi, stringId, &age, 0, x, y);
}

#pragma endregion