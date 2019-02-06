/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <cmath>
#include <iterator>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/GameAction.h>
#include <openrct2/actions/RideSetAppearanceAction.hpp>
#include <openrct2/actions/RideSetColourScheme.hpp>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/StationObject.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideGroupManager.h>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ride/Station.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2;

enum
{
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

// clang-format off
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
    WIDX_RIDE_TYPE_DROPDOWN,

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
    WIDX_REFURBISH_RIDE,
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
    { WWT_FRAME,            0,  0,      315,    0,      206,    0xFFFFFFFF,                     STR_NONE                                    }, \
    { WWT_CAPTION,          0,  1,      314,    1,      14,     STR_RIDE_WINDOW_TITLE,          STR_WINDOW_TITLE_TIP                        }, \
    { WWT_CLOSEBOX,         0,  303,    313,    2,      13,     STR_CLOSE_X,                    STR_CLOSE_WINDOW_TIP                        }, \
    { WWT_RESIZE,           1,  0,      315,    43,     179,    0xFFFFFFFF,                     STR_NONE                                    }, \
    { WWT_TAB,              1,  3,      33,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_VIEW_OF_RIDE_ATTRACTION_TIP             }, \
    { WWT_TAB,              1,  34,     64,     17,     46,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_VEHICLE_DETAILS_AND_OPTIONS_TIP         }, \
    { WWT_TAB,              1,  65,     95,     17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_OPERATING_OPTIONS_TIP                   }, \
    { WWT_TAB,              1,  96,     126,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_MAINTENANCE_OPTIONS_TIP                 }, \
    { WWT_TAB,              1,  127,    157,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_COLOUR_SCHEME_OPTIONS_TIP               }, \
    { WWT_TAB,              1,  158,    188,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_SOUND_AND_MUSIC_OPTIONS_TIP             }, \
    { WWT_TAB,              1,  189,    219,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_MEASUREMENTS_AND_TEST_DATA_TIP          }, \
    { WWT_TAB,              1,  220,    250,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_GRAPHS_TIP                              }, \
    { WWT_TAB,              1,  251,    281,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_INCOME_AND_COSTS_TIP                    }, \
    { WWT_TAB,              1,  282,    312,    17,     43,     IMAGE_TYPE_REMAP | SPR_TAB,     STR_CUSTOMER_INFORMATION_TIP                }

// 0x009ADC34
static rct_widget window_ride_main_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_VIEWPORT,         1,  3,      290,    60,     166,    STR_VIEWPORT,                   STR_NONE                                    },
    { WWT_DROPDOWN,         1,  35,     256,    46,     57,     0xFFFFFFFF,                     STR_VIEW_SELECTION                          },
    { WWT_BUTTON,           1,  245,    255,    47,     56,     STR_DROPDOWN_GLYPH,             STR_VIEW_SELECTION                          },
    { WWT_LABEL_CENTRED,    1,  3,      290,    167,    177,    0xFFFFFFFF,                     STR_NONE                                    },
    { WWT_FLATBTN,          1,  291,    314,    46,     69,     0xFFFFFFFF,                     STR_OPEN_CLOSE_OR_TEST_RIDE                 },
    { WWT_FLATBTN,          1,  291,    314,    70,     93,     SPR_CONSTRUCTION,               STR_CONSTRUCTION                            },
    { WWT_FLATBTN,          1,  291,    314,    94,     117,    SPR_RENAME,                     STR_NAME_RIDE_TIP                           },
    { WWT_FLATBTN,          1,  291,    314,    118,    141,    SPR_LOCATE,                     STR_LOCATE_SUBJECT_TIP                      },
    { WWT_FLATBTN,          1,  291,    314,    142,    165,    SPR_DEMOLISH,                   STR_DEMOLISH_RIDE_TIP                       },
    { WWT_IMGBTN,           1,  296,    309,    48,     61,     SPR_G2_RCT1_CLOSE_BUTTON_0,     STR_CLOSE_RIDE_TIP                          },
    { WWT_IMGBTN,           1,  296,    309,    62,     75,     SPR_G2_RCT1_TEST_BUTTON_0,      STR_TEST_RIDE_TIP                           },
    { WWT_IMGBTN,           1,  296,    309,    76,     89,     SPR_G2_RCT1_OPEN_BUTTON_0,      STR_OPEN_RIDE_TIP                           },
    { WWT_DROPDOWN,         1,  3,      307,    180,    191,    STR_ARG_6_STRINGID,             STR_NONE                                    },
    { WWT_BUTTON,           1,  297,    307,    180,    191,    STR_DROPDOWN_GLYPH,             STR_NONE                                    },
    { WIDGETS_END },
};

// 0x009ADDA8
static rct_widget window_ride_vehicle_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_DROPDOWN,         1,  7,      308,    50,     61,     0xFFFFFFFF,                                 STR_NONE                                        },
    { WWT_BUTTON,           1,  297,    307,    51,     60,     STR_DROPDOWN_GLYPH,                         STR_NONE                                        },
    { WWT_SCROLL,           1,  7,      308,    147,    189,    0,                                          STR_NONE                                        },
      SPINNER_WIDGETS      (1,  7,      151,    196,    207,    STR_RIDE_VEHICLE_COUNT,                     STR_MAX_VEHICLES_TIP),
      SPINNER_WIDGETS      (1,  164,    308,    196,    207,    STR_1_CAR_PER_TRAIN,                        STR_MAX_CARS_PER_TRAIN_TIP),
    { WIDGETS_END },
};

// 0x009ADEFC
static rct_widget window_ride_operating_widgets[] = {
    MAIN_RIDE_WIDGETS,
      SPINNER_WIDGETS      (1,  157,    308,    61,     72,     STR_ARG_18_STRINGID,                        STR_NONE), // NB: 3 widgets
      SPINNER_WIDGETS      (1,  157,    308,    75,     86,     STR_LIFT_HILL_CHAIN_SPEED_VALUE,            STR_NONE), // NB: 3 widgets
    { WWT_CHECKBOX,         1,  7,      86,     109,    120,    STR_WAIT_FOR,                               STR_WAIT_FOR_PASSENGERS_BEFORE_DEPARTING_TIP    },
    { WWT_CHECKBOX,         1,  7,      308,    124,    135,    0xFFFFFFFF,                                 STR_NONE                                        },
    { WWT_CHECKBOX,         1,  7,      156,    139,    150,    STR_MINIMUM_WAITING_TIME,                   STR_MINIMUM_LENGTH_BEFORE_DEPARTING_TIP         },
      SPINNER_WIDGETS      (1,  157,    308,    139,    150,    STR_ARG_10_STRINGID,                        STR_NONE), // NB: 3 widgets
    { WWT_CHECKBOX,         1,  7,      156,    154,    165,    STR_MAXIMUM_WAITING_TIME,                   STR_MAXIMUM_LENGTH_BEFORE_DEPARTING_TIP         },
      SPINNER_WIDGETS      (1,  157,    308,    154,    165,    STR_ARG_14_STRINGID,                        STR_NONE), // NB: 3 widgets
    { WWT_CHECKBOX,         1,  7,      308,    169,    180,    STR_SYNCHRONISE_WITH_ADJACENT_STATIONS,     STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP      },
    { WWT_LABEL,            1, 21,      149,    61,     72,     0xFFFFFFFF,                                 STR_NONE                                        },
    { WWT_LABEL,            1, 21,      149,    75,     86,     STR_LIFT_HILL_CHAIN_SPEED,                  STR_NONE                                        },
    { WWT_DROPDOWN,         1,  7,      308,    47,     58,     0xFFFFFFFF,                                 STR_SELECT_OPERATING_MODE                       },
    { WWT_BUTTON,           1,  297,    307,    48,     57,     STR_DROPDOWN_GLYPH,                         STR_SELECT_OPERATING_MODE                       },
    { WWT_DROPDOWN,         1,  87,     308,    109,    120,    0xFFFFFFFF,                                 STR_NONE                                        },
    { WWT_BUTTON,           1,  297,    307,    110,    119,    STR_DROPDOWN_GLYPH,                         STR_NONE                                        },
    { WWT_LABEL,            1,  21,     149,    89,     100,    STR_NUMBER_OF_CIRCUITS,                     STR_NUMBER_OF_CIRCUITS_TIP                      },
      SPINNER_WIDGETS      (1,  157,    308,    89,     100,    STR_NUMBER_OF_CIRCUITS_VALUE,               STR_NONE), // NB: 3 widgets
    { WIDGETS_END },
};

// 0x009AE190
static rct_widget window_ride_maintenance_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_DROPDOWN,         1,  107,    308,    71,     82,     0,                              STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE      },
    { WWT_BUTTON,           1,  297,    307,    72,     81,     STR_DROPDOWN_GLYPH,             STR_SELECT_HOW_OFTEN_A_MECHANIC_SHOULD_CHECK_THIS_RIDE      },
    { WWT_FLATBTN,          1,  289,    312,    108,    131,    0xFFFFFFFF,                     STR_LOCATE_NEAREST_AVAILABLE_MECHANIC_TIP                   },
    { WWT_FLATBTN,          1,  265,    288,    108,    131,    SPR_CONSTRUCTION,               STR_REFURBISH_RIDE_TIP                                      },
    { WWT_FLATBTN,          1,  241,    264,    108,    131,    SPR_NO_ENTRY,                   STR_DEBUG_FORCE_BREAKDOWN_TIP                               },
    { WIDGETS_END },
};

// 0x009AE2A4
static rct_widget window_ride_colour_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_SPINNER,          1,  3,      70,     47,     93,     0xFFFFFFFF,                     STR_NONE                                                    },
    { WWT_DROPDOWN,         1,  74,     312,    49,     60,     STR_ARG_14_STRINGID,            STR_NONE                                                    },
    { WWT_BUTTON,           1,  301,    311,    50,     59,     STR_DROPDOWN_GLYPH,             STR_COLOUR_SCHEME_TO_CHANGE_TIP                             },
    { WWT_COLOURBTN,        1,  79,     90,     74,     85,     0xFFFFFFFF,                     STR_SELECT_MAIN_COLOUR_TIP                                  },
    { WWT_COLOURBTN,        1,  99,     110,    74,     85,     0xFFFFFFFF,                     STR_SELECT_ADDITIONAL_COLOUR_1_TIP                          },
    { WWT_COLOURBTN,        1,  119,    130,    74,     85,     0xFFFFFFFF,                     STR_SELECT_SUPPORT_STRUCTURE_COLOUR_TIP                     },
    { WWT_DROPDOWN,         1,  74,     312,    49,     60,     0xFFFFFFFF,                     STR_NONE                                                    },
    { WWT_BUTTON,           1,  301,    311,    50,     59,     STR_DROPDOWN_GLYPH,             STR_NONE                                                    },
    { WWT_FLATBTN,          1,  289,    312,    68,     91,     SPR_PAINTBRUSH,                 STR_PAINT_INDIVIDUAL_AREA_TIP                               },
    { WWT_SPINNER,          1,  245,    312,    101,    147,    0xFFFFFFFF,                     STR_NONE                                                    },
    { WWT_DROPDOWN,         1,  103,    241,    103,    114,    0,                              STR_NONE                                                    },
    { WWT_BUTTON,           1,  230,    240,    104,    113,    STR_DROPDOWN_GLYPH,             STR_SELECT_STYLE_OF_ENTRANCE_EXIT_STATION_TIP               },
    { WWT_SCROLL,           1,  3,      70,     157,    203,    0,                              STR_NONE                                                    },
    { WWT_DROPDOWN,         1,  74,     312,    157,    168,    STR_ARG_6_STRINGID,             STR_NONE                                                    },
    { WWT_BUTTON,           1,  301,    311,    158,    167,    STR_DROPDOWN_GLYPH,             STR_SELECT_VEHICLE_COLOUR_SCHEME_TIP                        },
    { WWT_DROPDOWN,         1,  74,     312,    173,    184,    0xFFFFFFFF,                     STR_NONE                                                    },
    { WWT_BUTTON,           1,  301,    311,    174,    183,    STR_DROPDOWN_GLYPH,             STR_SELECT_VEHICLE_TO_MODIFY_TIP                            },
    { WWT_COLOURBTN,        1,  79,     90,     190,    201,    0xFFFFFFFF,                     STR_SELECT_MAIN_COLOUR_TIP                                  },
    { WWT_COLOURBTN,        1,  99,     110,    190,    201,    0xFFFFFFFF,                     STR_SELECT_ADDITIONAL_COLOUR_1_TIP                          },
    { WWT_COLOURBTN,        1,  119,    130,    190,    201,    0xFFFFFFFF,                     STR_SELECT_ADDITIONAL_COLOUR_2_TIP                          },
    { WIDGETS_END },
};

// 0x009AE4C8
static rct_widget window_ride_music_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_CHECKBOX,         1,  7,      308,    47,     58,     STR_PLAY_MUSIC,                 STR_SELECT_MUSIC_TIP                        },
    { WWT_DROPDOWN,         1,  7,      308,    62,     73,     0,                              STR_NONE                                    },
    { WWT_BUTTON,           1,  297,    307,    63,     72,     STR_DROPDOWN_GLYPH,             STR_SELECT_MUSIC_STYLE_TIP                  },
    { WIDGETS_END },
};

// 0x009AE5DC
static rct_widget window_ride_measurements_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_FLATBTN,          1,  288,    311,    194,    217,    SPR_FLOPPY,                     STR_SAVE_TRACK_DESIGN                       },
    { WWT_BUTTON,           1,  4,      157,    128,    139,    STR_SELECT_NEARBY_SCENERY,      STR_NONE                                    },
    { WWT_BUTTON,           1,  158,    311,    128,    139,    STR_RESET_SELECTION,            STR_NONE                                    },
    { WWT_BUTTON,           1,  4,      157,    178,    189,    STR_DESIGN_SAVE,                STR_NONE                                    },
    { WWT_BUTTON,           1,  158,    311,    178,    189,    STR_DESIGN_CANCEL,              STR_NONE                                    },
    { WIDGETS_END },
};

// 0x009AE710
static rct_widget window_ride_graphs_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_SCROLL,           1,  3,      308,    46,     157,    SCROLL_HORIZONTAL,              STR_LOGGING_DATA_FROM_TIP                                   },
    { WWT_BUTTON,           1,  3,      75,     163,    176,    STR_RIDE_STATS_VELOCITY,        STR_SHOW_GRAPH_OF_VELOCITY_AGAINST_TIME_TIP                 },
    { WWT_BUTTON,           1,  76,     148,    163,    176,    STR_RIDE_STATS_ALTITUDE,        STR_SHOW_GRAPH_OF_ALTITUDE_AGAINST_TIME_TIP                 },
    { WWT_BUTTON,           1,  149,    221,    163,    176,    STR_RIDE_STATS_VERT_G,          STR_SHOW_GRAPH_OF_VERTICAL_ACCELERATION_AGAINST_TIME_TIP    },
    { WWT_BUTTON,           1,  222,    294,    163,    176,    STR_RIDE_STATS_LAT_G,           STR_SHOW_GRAPH_OF_LATERAL_ACCELERATION_AGAINST_TIME_TIP     },
    { WIDGETS_END },
};

// 0x009AE844
static rct_widget window_ride_income_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_LABEL,            1,  19,     144,    50,     61,     0xFFFFFFFF,                                 STR_NONE                                                    },
      SPINNER_WIDGETS      (1,  147,    308,    50,     61,     STR_ARG_6_CURRENCY2DP,                      STR_NONE),  // NB: 3 widgets
    { WWT_CHECKBOX,         1,  5,      310,    62,     74,     STR_SAME_PRICE_THROUGHOUT_PARK,             STR_SAME_PRICE_THROUGHOUT_PARK_TIP                          },
    { WWT_LABEL,            1,  19,     144,    94,     105,    0xFFFFFFFF,                                 STR_NONE                                                    },
      SPINNER_WIDGETS      (1,  147,    308,    94,     105,    STR_RIDE_SECONDARY_PRICE_VALUE,             STR_NONE),  // NB: 3 widgets
    { WWT_CHECKBOX,         1,  5,      310,    106,    118,    STR_SAME_PRICE_THROUGHOUT_PARK,             STR_SAME_PRICE_THROUGHOUT_PARK_TIP                          },
    { WIDGETS_END },
};

// 0x009AE9C8
static rct_widget window_ride_customer_widgets[] = {
    MAIN_RIDE_WIDGETS,
    { WWT_FLATBTN,          1,  289,    312,    54,     77,     SPR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION,    STR_SHOW_GUESTS_THOUGHTS_ABOUT_THIS_RIDE_ATTRACTION_TIP },
    { WWT_FLATBTN,          1,  289,    312,    78,     101,    SPR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION,                STR_SHOW_GUESTS_ON_THIS_RIDE_ATTRACTION_TIP             },
    { WWT_FLATBTN,          1,  289,    312,    102,    125,    SPR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION,       STR_SHOW_GUESTS_QUEUING_FOR_THIS_RIDE_ATTRACTION_TIP    },
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

static constexpr const uint64_t window_ride_page_enabled_widgets[] = {
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
        (1ULL << WIDX_RIDE_TYPE_DROPDOWN),
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
        (1ULL << WIDX_REFURBISH_RIDE) |
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
        (1ULL << WIDX_PRIMARY_PRICE) |
        (1ULL << WIDX_PRIMARY_PRICE_INCREASE) |
        (1ULL << WIDX_PRIMARY_PRICE_DECREASE) |
        (1ULL << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK) |
        (1ULL << WIDX_SECONDARY_PRICE) |
        (1ULL << WIDX_SECONDARY_PRICE_INCREASE) |
        (1ULL << WIDX_SECONDARY_PRICE_DECREASE) |
        (1ULL << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK),
    MAIN_RIDE_ENABLED_WIDGETS |
        (1ULL << WIDX_SHOW_GUESTS_THOUGHTS) |
        (1ULL << WIDX_SHOW_GUESTS_ON_RIDE) |
        (1ULL << WIDX_SHOW_GUESTS_QUEUING),
};

static constexpr const uint64_t window_ride_page_hold_down_widgets[] = {
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
static void window_ride_main_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_main_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_main_update(rct_window *w);
static void window_ride_main_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_ride_main_viewport_rotate(rct_window *w);
static void window_ride_main_invalidate(rct_window *w);
static void window_ride_main_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_vehicle_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_vehicle_resize(rct_window *w);
static void window_ride_vehicle_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_vehicle_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_vehicle_update(rct_window *w);
static void window_ride_vehicle_invalidate(rct_window *w);
static void window_ride_vehicle_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_vehicle_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void window_ride_operating_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_operating_resize(rct_window *w);
static void window_ride_operating_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_operating_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_operating_update(rct_window *w);
static void window_ride_operating_invalidate(rct_window *w);
static void window_ride_operating_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_maintenance_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_maintenance_resize(rct_window *w);
static void window_ride_maintenance_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_maintenance_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_maintenance_update(rct_window *w);
static void window_ride_maintenance_invalidate(rct_window *w);
static void window_ride_maintenance_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_colour_close(rct_window *w);
static void window_ride_colour_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_colour_resize(rct_window *w);
static void window_ride_colour_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_colour_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_colour_update(rct_window *w);
static void window_ride_colour_tooldown(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_colour_tooldrag(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_colour_invalidate(rct_window *w);
static void window_ride_colour_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_colour_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void window_ride_music_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_music_resize(rct_window *w);
static void window_ride_music_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_music_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_music_update(rct_window *w);
static void window_ride_music_invalidate(rct_window *w);
static void window_ride_music_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_measurements_close(rct_window *w);
static void window_ride_measurements_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_measurements_resize(rct_window *w);
static void window_ride_measurements_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_measurements_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_measurements_update(rct_window *w);
static void window_ride_measurements_tooldown(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_measurements_tooldrag(rct_window *w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_measurements_toolabort(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_measurements_invalidate(rct_window *w);
static void window_ride_measurements_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_graphs_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_graphs_resize(rct_window *w);
static void window_ride_graphs_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_graphs_update(rct_window *w);
static void window_ride_graphs_scrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_ride_graphs_15(rct_window *w, int32_t scrollIndex, int32_t scrollAreaType);
static void window_ride_graphs_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_ride_graphs_invalidate(rct_window *w);
static void window_ride_graphs_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_ride_graphs_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void window_ride_income_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_income_resize(rct_window *w);
static void window_ride_income_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_income_update(rct_window *w);
static void window_ride_income_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_ride_income_invalidate(rct_window *w);
static void window_ride_income_paint(rct_window *w, rct_drawpixelinfo *dpi);
static bool window_ride_income_can_modify_primary_price(rct_window* w);

static void window_ride_customer_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_customer_resize(rct_window *w);
static void window_ride_customer_update(rct_window *w);
static void window_ride_customer_invalidate(rct_window *w);
static void window_ride_customer_paint(rct_window *w, rct_drawpixelinfo *dpi);

static void window_ride_set_page(rct_window *w, int32_t page);

// 0x0098DFD4
static rct_window_event_list window_ride_main_events = {
    nullptr,
    window_ride_main_mouseup,
    window_ride_main_resize,
    window_ride_main_mousedown,
    window_ride_main_dropdown,
    nullptr,
    window_ride_main_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_main_textinput,
    window_ride_main_viewport_rotate,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_main_invalidate,
    window_ride_main_paint,
    nullptr
};

// 0x0098E204
static rct_window_event_list window_ride_vehicle_events = {
    nullptr,
    window_ride_vehicle_mouseup,
    window_ride_vehicle_resize,
    window_ride_vehicle_mousedown,
    window_ride_vehicle_dropdown,
    nullptr,
    window_ride_vehicle_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_vehicle_invalidate,
    window_ride_vehicle_paint,
    window_ride_vehicle_scrollpaint
};

// 0x0098E0B4
static rct_window_event_list window_ride_operating_events = {
    nullptr,
    window_ride_operating_mouseup,
    window_ride_operating_resize,
    window_ride_operating_mousedown,
    window_ride_operating_dropdown,
    nullptr,
    window_ride_operating_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_operating_invalidate,
    window_ride_operating_paint,
    nullptr
};

// 0x0098E124
static rct_window_event_list window_ride_maintenance_events = {
    nullptr,
    window_ride_maintenance_mouseup,
    window_ride_maintenance_resize,
    window_ride_maintenance_mousedown,
    window_ride_maintenance_dropdown,
    nullptr,
    window_ride_maintenance_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_maintenance_invalidate,
    window_ride_maintenance_paint,
    nullptr
};

// 0x0098E044
static rct_window_event_list window_ride_colour_events = {
    window_ride_colour_close,
    window_ride_colour_mouseup,
    window_ride_colour_resize,
    window_ride_colour_mousedown,
    window_ride_colour_dropdown,
    nullptr,
    window_ride_colour_update,
    nullptr,
    nullptr,
    nullptr,
    window_ride_colour_tooldown,
    window_ride_colour_tooldrag,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_colour_invalidate,
    window_ride_colour_paint,
    window_ride_colour_scrollpaint
};

// 0x0098E194
static rct_window_event_list window_ride_music_events = {
    nullptr,
    window_ride_music_mouseup,
    window_ride_music_resize,
    window_ride_music_mousedown,
    window_ride_music_dropdown,
    nullptr,
    window_ride_music_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_music_invalidate,
    window_ride_music_paint,
    nullptr
};

// 0x0098DE14
static rct_window_event_list window_ride_measurements_events = {
    window_ride_measurements_close,
    window_ride_measurements_mouseup,
    window_ride_measurements_resize,
    window_ride_measurements_mousedown,
    window_ride_measurements_dropdown,
    nullptr,
    window_ride_measurements_update,
    nullptr,
    nullptr,
    nullptr,
    window_ride_measurements_tooldown,
    window_ride_measurements_tooldrag,
    nullptr,
    window_ride_measurements_toolabort,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_measurements_invalidate,
    window_ride_measurements_paint,
    nullptr
};

// 0x0098DF64
static rct_window_event_list window_ride_graphs_events = {
    nullptr,
    window_ride_graphs_mouseup,
    window_ride_graphs_resize,
    window_ride_graphs_mousedown,
    nullptr,
    nullptr,
    window_ride_graphs_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_graphs_scrollgetheight,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_graphs_15,
    window_ride_graphs_tooltip,
    nullptr,
    nullptr,
    window_ride_graphs_invalidate,
    window_ride_graphs_paint,
    window_ride_graphs_scrollpaint
};

// 0x0098DEF4
static rct_window_event_list window_ride_income_events = {
    nullptr,
    window_ride_income_mouseup,
    window_ride_income_resize,
    window_ride_income_mousedown,
    nullptr,
    nullptr,
    window_ride_income_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_income_textinput,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_income_invalidate,
    window_ride_income_paint,
    nullptr
};

// 0x0098DE84
static rct_window_event_list window_ride_customer_events = {
    nullptr,
    window_ride_customer_mouseup,
    window_ride_customer_resize,
    nullptr,
    nullptr,
    nullptr,
    window_ride_customer_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_ride_customer_invalidate,
    window_ride_customer_paint,
    nullptr
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

static bool _collectTrackDesignScenery = false;
static int32_t _lastSceneryX = 0;
static int32_t _lastSceneryY = 0;

static void set_operating_setting(int32_t rideNumber, uint8_t setting, uint8_t value);

// Cached overall view for each ride
// (Re)calculated when the ride window is opened
struct ride_overall_view {
    int16_t x, y, z;
    uint8_t zoom;
};

static ride_overall_view ride_overall_views[MAX_RIDES] = {};

static constexpr const int32_t window_ride_tab_animation_divisor[] = { 0, 0, 2, 2, 4, 2, 8, 8, 2, 0 };
static constexpr const int32_t window_ride_tab_animation_frames[] = { 0, 0, 4, 16, 8, 16, 8, 8, 8, 0 };

static constexpr const rct_string_id RatingNames[] = {
    STR_RATING_LOW,
    STR_RATING_MEDIUM,
    STR_RATING_HIGH,
    STR_RATING_VERY_HIGH,
    STR_RATING_EXTREME,
    STR_RATING_ULTRA_EXTREME,
};

static constexpr const rct_string_id RideBreakdownReasonNames[] = {
    STR_RIDE_BREAKDOWN_SAFETY_CUT_OUT ,
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_CLOSED,
    STR_RIDE_BREAKDOWN_RESTRAINTS_STUCK_OPEN,
    STR_RIDE_BREAKDOWN_DOORS_STUCK_CLOSED,
    STR_RIDE_BREAKDOWN_DOORS_STUCK_OPEN,
    STR_RIDE_BREAKDOWN_VEHICLE_MALFUNCTION,
    STR_RIDE_BREAKDOWN_BRAKES_FAILURE,
    STR_RIDE_BREAKDOWN_CONTROL_FAILURE
};

static constexpr const rct_string_id ColourSchemeNames[] = {
    STR_MAIN_COLOUR_SCHEME,
    STR_ALTERNATIVE_COLOUR_SCHEME_1,
    STR_ALTERNATIVE_COLOUR_SCHEME_2,
    STR_ALTERNATIVE_COLOUR_SCHEME_3,
};

static constexpr const rct_string_id VehicleLoadNames[] = {
    STR_QUARTER_LOAD,
    STR_HALF_LOAD,
    STR_THREE_QUARTER_LOAD,
    STR_FULL_LOAD,
    STR_ANY_LOAD,
};

static constexpr const rct_string_id VehicleColourSchemeNames[] = {
    STR_ALL_VEHICLES_IN_SAME_COLOURS ,
    STR_DIFFERENT_COLOURS_PER ,
    STR_DIFFERENT_COLOURS_PER_VEHICLE ,
};

static constexpr const rct_string_id VehicleStatusNames[] = {
    STR_MOVING_TO_END_OF,           // VEHICLE_STATUS_MOVING_TO_END_OF_STATION
    STR_WAITING_FOR_PASSENGERS_AT,  // VEHICLE_STATUS_WAITING_FOR_PASSENGERS
    STR_WAITING_TO_DEPART,          // VEHICLE_STATUS_WAITING_TO_DEPART
    STR_DEPARTING,                  // VEHICLE_STATUS_DEPARTING
    STR_TRAVELLING_AT_0,            // VEHICLE_STATUS_TRAVELLING
    STR_ARRIVING_AT,                // VEHICLE_STATUS_ARRIVING
    STR_UNLOADING_PASSENGERS_AT,    // VEHICLE_STATUS_UNLOADING_PASSENGERS
    STR_TRAVELLING_AT_1,            // VEHICLE_STATUS_TRAVELLING_BOAT
    STR_CRASHING,                   // VEHICLE_STATUS_CRASHING
    STR_CRASHED_0,                  // VEHICLE_STATUS_CRASHED
    STR_TRAVELLING_AT_2,            // VEHICLE_STATUS_TRAVELLING_DODGEMS
    STR_SWINGING,                   // VEHICLE_STATUS_SWINGING
    STR_ROTATING_0,                 // VEHICLE_STATUS_ROTATING
    STR_ROTATING_1,                 // VEHICLE_STATUS_FERRIS_WHEEL_ROTATING
    STR_OPERATING_0,                // VEHICLE_STATUS_SIMULATOR_OPERATING
    STR_SHOWING_FILM,               // VEHICLE_STATUS_SHOWING_FILM
    STR_ROTATING_2,                 // VEHICLE_STATUS_SPACE_RINGS_OPERATING
    STR_OPERATING_1,                // VEHICLE_STATUS_TOP_SPIN_OPERATING
    STR_OPERATING_2,                // VEHICLE_STATUS_HAUNTED_HOUSE_OPERATING
    STR_DOING_CIRCUS_SHOW,          // VEHICLE_STATUS_DOING_CIRCUS_SHOW
    STR_OPERATING_3,                // VEHICLE_STATUS_CROOKED_HOUSE_OPERATING
    STR_WAITING_FOR_CABLE_LIFT,     // VEHICLE_STATUS_WAITING_FOR_CABLE_LIFT
    STR_TRAVELLING_AT_3,            // VEHICLE_STATUS_TRAVELLING_CABLE_LIFT
    STR_STOPPING_0,                 // VEHICLE_STATUS_STOPPING
    STR_WAITING_FOR_PASSENGERS,     // VEHICLE_STATUS_WAITING_FOR_PASSENGERS_17
    STR_WAITING_TO_START,           // VEHICLE_STATUS_WAITING_TO_START
    STR_STARTING,                   // VEHICLE_STATUS_STARTING
    STR_OPERATING,                  // VEHICLE_STATUS_OPERATING_1A
    STR_STOPPING_1,                 // VEHICLE_STATUS_STOPPING_1B
    STR_UNLOADING_PASSENGERS,       // VEHICLE_STATUS_UNLOADING_PASSENGERS_1C
    STR_STOPPED_BY_BLOCK_BRAKES,    // VEHICLE_STATUS_STOPPED_BY_BLOCK_BRAKES
};

static constexpr const rct_string_id SingleSessionVehicleStatusNames[] = {
    STR_STOPPING_0,                 // VEHICLE_STATUS_MOVING_TO_END_OF_STATION
    STR_WAITING_FOR_PASSENGERS,     // VEHICLE_STATUS_WAITING_FOR_PASSENGERS
    STR_WAITING_TO_START,           // VEHICLE_STATUS_WAITING_TO_DEPART
    STR_STARTING,                   // VEHICLE_STATUS_DEPARTING
    STR_OPERATING,                  // VEHICLE_STATUS_TRAVELLING
    STR_STOPPING_1,                 // VEHICLE_STATUS_ARRIVING
    STR_UNLOADING_PASSENGERS,       // VEHICLE_STATUS_UNLOADING_PASSENGERS
};

static constexpr const rct_string_id MusicStyleNames[] = {
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

struct window_ride_maze_design_option {
    rct_string_id text;
    uint32_t sprite;
};

static constexpr const window_ride_maze_design_option MazeOptions[] = {
    { STR_RIDE_DESIGN_MAZE_BRICK_WALLS,     SPR_RIDE_DESIGN_PREVIEW_MAZE_BRICK_WALLS },
    { STR_RIDE_DESIGN_MAZE_HEDGES,          SPR_RIDE_DESIGN_PREVIEW_MAZE_HEDGES },
    { STR_RIDE_DESIGN_MAZE_ICE_BLOCKS,      SPR_RIDE_DESIGN_PREVIEW_MAZE_ICE_BLOCKS },
    { STR_RIDE_DESIGN_MAZE_WOODEN_FENCES,   SPR_RIDE_DESIGN_PREVIEW_MAZE_WOODEN_FENCES },
};

struct window_ride_colour_preview
{
    uint32_t track;
    uint32_t supports;
};

static constexpr const window_ride_colour_preview TrackColourPreviews[] = {
    { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_TRACK,          SPR_RIDE_DESIGN_PREVIEW_SPIRAL_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_TRACK,        SPR_RIDE_DESIGN_PREVIEW_STAND_UP_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_TRACK,     SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_SWINGING_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_INVERTED_ROLLER_COASTER_TRACK,        SPR_RIDE_DESIGN_PREVIEW_INVERTED_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_TRACK,          SPR_RIDE_DESIGN_PREVIEW_JUNIOR_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_TRACK,              SPR_RIDE_DESIGN_PREVIEW_MINIATURE_RAILWAY_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MONORAIL_TRACK,                       SPR_RIDE_DESIGN_PREVIEW_MONORAIL_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_TRACK,         SPR_RIDE_DESIGN_PREVIEW_MINI_SUSPENDED_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_BOAT_HIRE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_TRACK,              SPR_RIDE_DESIGN_PREVIEW_WOODEN_WILD_MOUSE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_STEEPLECHASE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_TRACK,                       SPR_RIDE_DESIGN_PREVIEW_CAR_RIDE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_TRACK,              SPR_RIDE_DESIGN_PREVIEW_LAUNCHED_FREEFALL_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_TRACK,              SPR_RIDE_DESIGN_PREVIEW_BOBSLEIGH_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_TRACK,              SPR_RIDE_DESIGN_PREVIEW_OBSERVATION_TOWER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_LOOPING_ROLLER_COASTER_TRACK,         SPR_RIDE_DESIGN_PREVIEW_LOOPING_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_DINGHY_SLIDE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINE_TRAIN_COASTER_TRACK,             SPR_RIDE_DESIGN_PREVIEW_MINE_TRAIN_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_CHAIRLIFT_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_TRACK,       SPR_RIDE_DESIGN_PREVIEW_CORKSCREW_ROLLER_COASTER_SUPPORTS},
    { 0, 0 }, // MAZE
    { SPR_RIDE_DESIGN_PREVIEW_SPIRAL_SLIDE_TRACK,                   0},
    { SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_TRACK,                       SPR_RIDE_DESIGN_PREVIEW_GO_KARTS_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_LOG_FLUME_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_RIVER_RAPIDS_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_DODGEMS_TRACK,                        SPR_RIDE_DESIGN_PREVIEW_DODGEMS_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_PIRATE_SHIP_TRACK,                    SPR_RIDE_DESIGN_PREVIEW_PIRATE_SHIP_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_TRACK,         SPR_RIDE_DESIGN_PREVIEW_SWINGING_INVERTER_SHIP_SUPPORTS},
    { 0, 0 }, // FOOD_STALL
    { 0, 0 }, // 1D
    { 0, 0 }, // DRINK_STALL
    { 0, 0 }, // 1F
    { 0, 0 }, // SHOP
    { 0, 0 }, // MERRY_GO_ROUND
    { 0, 0 }, // 22
    { 0, 0 }, // INFORMATION_KIOSK
    { 0, 0 }, // TOILETS
    { SPR_RIDE_DESIGN_PREVIEW_FERRIS_WHEEL_TRACK,                   0},
    { 0, 0 }, // MOTION_SIMULATOR
    { 0, 0 }, // 3D_CINEMA
    { SPR_RIDE_DESIGN_PREVIEW_TOP_SPIN_TRACK,                       0},
    { 0, 0 }, // SPACE_RINGS
    { SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_TRACK,       SPR_RIDE_DESIGN_PREVIEW_REVERSE_FREEFALL_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_LIFT_TRACK,                           0},
    { SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_TRACK,   SPR_RIDE_DESIGN_PREVIEW_VERTICAL_DROP_ROLLER_COASTER_SUPPORTS},
    { 0, 0 }, // CASH_MACHINE
    { 0, 0 }, // TWIST_TRACK
    { 0, 0 }, // HAUNTED_HOUSE
    { 0, 0 }, // FIRST_AID
    { 0, 0 }, // CIRCUS_SHOW
    { SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_TRACK,                    SPR_RIDE_DESIGN_PREVIEW_GHOST_TRAIN_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_TRACK,         SPR_RIDE_DESIGN_PREVIEW_TWISTER_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_TRACK,          SPR_RIDE_DESIGN_PREVIEW_WOODEN_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_TRACK,   SPR_RIDE_DESIGN_PREVIEW_SIDE_FRICTION_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_TRACK,                     SPR_RIDE_DESIGN_PREVIEW_WILD_MOUSE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_MULTI_DIMENSION_ROLLER_COASTER_SUPPORTS},
    { 0, 0 }, // 38
    { SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_TRACK,          SPR_RIDE_DESIGN_PREVIEW_FLYING_ROLLER_COASTER_SUPPORTS},
    { 0,     0 }, // 3A
    { SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_TRACK,                  SPR_RIDE_DESIGN_PREVIEW_VIRGINIA_REEL_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_SPLASH_BOATS_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINI_HELICOPTERS_TRACK,               SPR_RIDE_DESIGN_PREVIEW_MINI_HELICOPTERS_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_TRACK,        SPR_RIDE_DESIGN_PREVIEW_LAY_DOWN_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_TRACK,             SPR_RIDE_DESIGN_PREVIEW_SUSPENDED_MONORAIL_SUPPORTS},
    { 0, 0 }, // 40
    { SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_TRACK,        SPR_RIDE_DESIGN_PREVIEW_REVERSER_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_TRACK,      SPR_RIDE_DESIGN_PREVIEW_HEARTLINE_TWISTER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_MINI_GOLF_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_GIGA_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_ROTO_DROP_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_FLYING_SAUCERS_TRACK,                 0},
    { 0, 0 }, // CROOKED_HOUSE
    { SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_TRACK,                SPR_RIDE_DESIGN_PREVIEW_MONORAIL_CYCLES_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_TRACK,       SPR_RIDE_DESIGN_PREVIEW_COMPACT_INVERTED_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_TRACK,                  SPR_RIDE_DESIGN_PREVIEW_WATER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_TRACK,   SPR_RIDE_DESIGN_PREVIEW_AIR_POWERED_VERTICAL_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_TRACK,       SPR_RIDE_DESIGN_PREVIEW_INVERTED_HAIRPIN_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_TRACK,                   SPR_RIDE_DESIGN_PREVIEW_MAGIC_CARPET_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_TRACK,                 SPR_RIDE_DESIGN_PREVIEW_SUBMARINE_RIDE_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_TRACK,                    SPR_RIDE_DESIGN_PREVIEW_RIVER_RAFTS_SUPPORTS},
    { 0, 0 }, // 50
    { 0, 0 }, // ENTERPRISE
    { 0, 0 }, // 52
    { 0, 0 }, // 53
    { 0, 0 }, // 54
    { 0, 0 }, // 55
    { SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_TRACK,       SPR_RIDE_DESIGN_PREVIEW_INVERTED_IMPULSE_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINI_ROLLER_COASTER_TRACK,            SPR_RIDE_DESIGN_PREVIEW_MINI_ROLLER_COASTER_SUPPORTS},
    { SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_TRACK,                      SPR_RIDE_DESIGN_PREVIEW_MINE_RIDE_SUPPORTS},
    { 0, 0 }, // 59
    { SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_TRACK,    SPR_RIDE_DESIGN_PREVIEW_LIM_LAUNCHED_ROLLER_COASTER_SUPPORTS},
};
// clang-format on

struct rct_window_graphs_y_axis
{
    uint8_t interval;
    int8_t unit;
    int8_t unit_interval;
    rct_string_id label;
};

/** rct2: 0x0098DD98 */
static constexpr const rct_window_graphs_y_axis window_graphs_y_axi[] = {
    { 11, 0, 10, STR_RIDE_STATS_VELOCITY_FORMAT }, // GRAPH_VELOCITY
    { 10, 0, 15, STR_RIDE_STATS_ALTITUDE_FORMAT }, // GRAPH_ALTITUDE
    { 13, -3, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_VERTICAL
    { 13, -4, 1, STR_RIDE_STATS_G_FORCE_FORMAT },  // GRAPH_LATERAL
};

static constexpr auto RIDE_G_FORCES_RED_POS_VERTICAL = FIXED_2DP(5, 00);
static constexpr auto RIDE_G_FORCES_RED_NEG_VERTICAL = -FIXED_2DP(2, 00);
static constexpr auto RIDE_G_FORCES_RED_LATERAL = FIXED_2DP(2, 80);

// Used for sorting the ride type cheat dropdown.
struct RideTypeLabel
{
    uint8_t ride_type_id;
    rct_string_id label_id;
    const char* label_string;
};

static int32_t RideDropdownDataLanguage = LANGUAGE_UNDEFINED;
static std::vector<RideTypeLabel> RideDropdownData;

// Used for sorting the vehicle type dropdown.
struct VehicleTypeLabel
{
    int32_t subtype_id;
    rct_string_id label_id;
    const char* label_string;
};

static int32_t VehicleDropdownDataLanguage = LANGUAGE_UNDEFINED;
static rct_ride_entry* VehicleDropdownRideType = nullptr;
static bool VehicleDropdownExpanded = false;
static std::vector<VehicleTypeLabel> VehicleDropdownData;

static void window_ride_draw_tab_image(rct_drawpixelinfo* dpi, rct_window* w, int32_t page, int32_t spriteIndex)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + page;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        if (w->page == page)
        {
            int32_t frame = w->frame_no / window_ride_tab_animation_divisor[w->page];
            spriteIndex += (frame % window_ride_tab_animation_frames[w->page]);
        }

        gfx_draw_sprite(dpi, spriteIndex, w->x + w->widgets[widgetIndex].left, w->y + w->widgets[widgetIndex].top, 0);
    }
}

/**
 *
 *  rct2: 0x006B2E88
 */
static void window_ride_draw_tab_main(rct_drawpixelinfo* dpi, rct_window* w)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_MAIN;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        int32_t spriteIndex = 0;
        int32_t rideType = get_ride(w->number)->type;

        switch (gRideClassifications[rideType])
        {
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
static void window_ride_draw_tab_vehicle(rct_drawpixelinfo* dpi, rct_window* w)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_VEHICLE;
    rct_widget* widget = &w->widgets[widgetIndex];

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        int32_t x = widget->left + 1;
        int32_t y = widget->top + 1;
        int32_t width = widget->right - x;
        int32_t height = widget->bottom - 3 - y;
        if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
            height += 4;

        x += w->x;
        y += w->y;

        rct_drawpixelinfo clipDPI;
        if (!clip_drawpixelinfo(&clipDPI, dpi, x, y, width, height))
        {
            return;
        }

        x = (widget->right - widget->left) / 2;
        y = (widget->bottom - widget->top) - 12;

        Ride* ride = get_ride(w->number);

        rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);
        if (rideEntry->flags & RIDE_ENTRY_FLAG_VEHICLE_TAB_SCALE_HALF)
        {
            clipDPI.zoom_level = 1;
            clipDPI.width *= 2;
            clipDPI.height *= 2;
            x *= 2;
            y *= 2;
            clipDPI.x *= 2;
            clipDPI.y *= 2;
        }

        // For any suspended rides, move image higher in the vehicle tab on the rides window
        if (ride->type == RIDE_TYPE_COMPACT_INVERTED_COASTER || ride->type == RIDE_TYPE_INVERTED_ROLLER_COASTER
            || ride->type == RIDE_TYPE_INVERTED_IMPULSE_COASTER || ride->type == RIDE_TYPE_INVERTED_HAIRPIN_COASTER
            || ride->type == RIDE_TYPE_SUSPENDED_SWINGING_COASTER || ride->type == RIDE_TYPE_CHAIRLIFT
            || ride->type == RIDE_TYPE_MINI_SUSPENDED_COASTER || ride->type == RIDE_TYPE_SUSPENDED_MONORAIL)
        {
            y /= 4;
        }

        const uint8_t vehicle = ride_entry_get_vehicle_at_position(
            ride->subtype, ride->num_cars_per_train, rideEntry->tab_vehicle);
        rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[vehicle];

        vehicle_colour vehicleColour = ride_get_vehicle_colour(ride, 0);
        int32_t spriteIndex = 32;
        if (w->page == WINDOW_RIDE_PAGE_VEHICLE)
            spriteIndex += w->frame_no;
        spriteIndex /= (rideVehicleEntry->flags & VEHICLE_ENTRY_FLAG_11) ? 4 : 2;
        spriteIndex &= rideVehicleEntry->rotation_frame_mask;
        spriteIndex *= rideVehicleEntry->base_num_frames;
        spriteIndex += rideVehicleEntry->base_image_id;
        spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
        spriteIndex |= IMAGE_TYPE_REMAP_2_PLUS;

        gfx_draw_sprite(&clipDPI, spriteIndex, x, y, vehicleColour.additional_2);
    }
}

/**
 *
 *  rct2: 0x006B2F42
 */
static void window_ride_draw_tab_customer(rct_drawpixelinfo* dpi, rct_window* w)
{
    rct_widgetindex widgetIndex = WIDX_TAB_1 + WINDOW_RIDE_PAGE_CUSTOMER;

    if (!(w->disabled_widgets & (1LL << widgetIndex)))
    {
        rct_widget* widget = &w->widgets[widgetIndex];
        int32_t spriteIndex = 0;
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
static void window_ride_draw_tab_images(rct_drawpixelinfo* dpi, rct_window* w)
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
static void window_ride_disable_tabs(rct_window* w)
{
    uint32_t disabled_tabs = 0;
    Ride* ride = get_ride(w->number & 0xFF);

    uint8_t ride_type = ride->type; // ecx

    if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_DATA_LOGGING))
        disabled_tabs |= (1 << WIDX_TAB_8); // 0x800

    if (ride_type == RIDE_TYPE_MINI_GOLF)
        disabled_tabs |= (1 << WIDX_TAB_2 | 1 << WIDX_TAB_3 | 1 << WIDX_TAB_4); // 0xE0

    if (ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_NO_VEHICLES))
        disabled_tabs |= (1 << WIDX_TAB_2); // 0x20

    if (!ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN)
        && !ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL)
        && !ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS)
        && !ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS)
        && !(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ENTRANCE_EXIT))
    {
        disabled_tabs |= (1 << WIDX_TAB_5); // 0x100
    }

    if (ride_type_has_flag(ride_type, RIDE_TYPE_FLAG_IS_SHOP))
        disabled_tabs |= (1 << WIDX_TAB_3 | 1 << WIDX_TAB_4 | 1 << WIDX_TAB_7); // 0x4C0

    if (!(RideData4[ride->type].flags & RIDE_TYPE_FLAG4_ALLOW_MUSIC))
    {
        disabled_tabs |= (1 << WIDX_TAB_6); // 0x200
    }

    if (ride_type == RIDE_TYPE_CASH_MACHINE || ride_type == RIDE_TYPE_FIRST_AID || (gParkFlags & PARK_FLAGS_NO_MONEY) != 0)
        disabled_tabs |= (1 << WIDX_TAB_9); // 0x1000

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER) != 0)
        disabled_tabs |= (1 << WIDX_TAB_4 | 1 << WIDX_TAB_6 | 1 << WIDX_TAB_9 | 1 << WIDX_TAB_10); // 0x3280

    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);

    if (rideEntry == nullptr)
    {
        disabled_tabs |= 1 << WIDX_TAB_2 | 1 << WIDX_TAB_3 | 1 << WIDX_TAB_4 | 1 << WIDX_TAB_5 | 1 << WIDX_TAB_6
            | 1 << WIDX_TAB_7 | 1 << WIDX_TAB_8 | 1 << WIDX_TAB_9 | 1 << WIDX_TAB_10;
    }
    else if ((rideEntry->flags & RIDE_ENTRY_FLAG_DISABLE_COLOUR_TAB) != 0)
    {
        disabled_tabs |= (1 << WIDX_TAB_5);
    }

    w->disabled_widgets = disabled_tabs;
}

static void window_ride_update_overall_view(ride_id_t ride_index)
{
    // Calculate x, y, z bounds of the entire ride using its track elements
    tile_element_iterator it;

    tile_element_iterator_begin(&it);

    int32_t minx = std::numeric_limits<int32_t>::max(), miny = std::numeric_limits<int32_t>::max(),
            minz = std::numeric_limits<int32_t>::max();
    int32_t maxx = std::numeric_limits<int32_t>::min(), maxy = std::numeric_limits<int32_t>::min(),
            maxz = std::numeric_limits<int32_t>::min();

    while (tile_element_iterator_next(&it))
    {
        if (it.element->GetType() != TILE_ELEMENT_TYPE_TRACK)
            continue;

        if (it.element->AsTrack()->GetRideIndex() != ride_index)
            continue;

        int32_t x = it.x * 32;
        int32_t y = it.y * 32;
        int32_t z1 = it.element->base_height * 8;
        int32_t z2 = it.element->clearance_height * 8;

        minx = std::min(minx, x);
        miny = std::min(miny, y);
        minz = std::min(minz, z1);

        maxx = std::max(maxx, x);
        maxy = std::max(maxy, y);
        maxz = std::max(maxz, z2);
    }

    ride_overall_view* view = &ride_overall_views[ride_index];
    view->x = (minx + maxx) / 2 + 16;
    view->y = (miny + maxy) / 2 + 16;
    view->z = (minz + maxz) / 2 - 8;

    // Calculate size to determine from how far away to view the ride
    int32_t dx = maxx - minx;
    int32_t dy = maxy - miny;
    int32_t dz = maxz - minz;

    int32_t size = (int32_t)std::sqrt(dx * dx + dy * dy + dz * dz);

    if (size >= 80)
    {
        // Each farther zoom level shows twice as many tiles (log)
        // Appropriate zoom is lowered by one to fill the entire view with the ride
        view->zoom = std::clamp<int32_t>(std::ceil(std::log(size / 80)) - 1, 0, 3);
    }
    else
    {
        // Small rides or stalls are zoomed in all the way.
        view->zoom = 0;
    }
}

/**
 *
 *  rct2: 0x006AEAB4
 */
static rct_window* window_ride_open(ride_id_t rideIndex)
{
    rct_window* w;

    w = window_create_auto_pos(316, 207, window_ride_page_events[0], WC_RIDE, WF_10 | WF_RESIZABLE);
    w->widgets = window_ride_page_widgets[WINDOW_RIDE_PAGE_MAIN];
    w->enabled_widgets = window_ride_page_enabled_widgets[WINDOW_RIDE_PAGE_MAIN];
    w->hold_down_widgets = window_ride_page_hold_down_widgets[WINDOW_RIDE_PAGE_MAIN];
    w->number = rideIndex;

    w->page = WINDOW_RIDE_PAGE_MAIN;
    w->vehicleIndex = 0;
    w->frame_no = 0;
    w->list_information_type = 0;
    w->var_492 = 0;
    w->ride_colour = 0;
    window_ride_disable_tabs(w);
    w->min_width = 316;
    w->min_height = 180;
    w->max_width = 500;
    w->max_height = 450;

    window_ride_update_overall_view(rideIndex);

    return w;
}

/**
 *
 *  rct2: 0x006ACC28
 */
rct_window* window_ride_main_open(ride_id_t rideIndex)
{
    rct_window* w;

    w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
    if (w == nullptr)
    {
        w = window_ride_open(rideIndex);
        w->ride.var_482 = -1;
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
    {
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
        {
            tool_cancel();
        }
    }

    if (w->page != WINDOW_RIDE_PAGE_MAIN)
    {
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
static rct_window* window_ride_open_station(ride_id_t rideIndex, int32_t stationIndex)
{
    int32_t i;
    Ride* ride;
    rct_window* w;

    ride = get_ride(rideIndex);

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
        return window_ride_main_open(rideIndex);

    w = window_bring_to_front_by_number(WC_RIDE, rideIndex);
    if (w == nullptr)
    {
        w = window_ride_open(rideIndex);
        w->ride.var_482 = -1;
    }

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == w->classification
        && gCurrentToolWidget.window_number == w->number)
    {
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
    for (i = stationIndex; i >= 0; i--)
    {
        if (ride->stations[i].Start.xy == RCT_XY8_UNDEFINED)
        {
            stationIndex--;
        }
    }

    w->ride.view = 1 + ride->num_vehicles + stationIndex;
    window_ride_init_viewport(w);

    return w;
}

rct_window* window_ride_open_track(TileElement* tileElement)
{
    assert(
        tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE || tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK
        || tileElement->GetType() == TILE_ELEMENT_TYPE_PATH);

    if (tileElement->GetType() == TILE_ELEMENT_TYPE_ENTRANCE)
    {
        ride_id_t rideIndex = tileElement->AsEntrance()->GetRideIndex();
        // Open ride window in station view
        return window_ride_open_station(rideIndex, tileElement->AsEntrance()->GetStationIndex());
    }
    else if (
        tileElement->GetType() == TILE_ELEMENT_TYPE_TRACK
        && TrackSequenceProperties[tileElement->AsTrack()->GetTrackType()][0] & TRACK_SEQUENCE_FLAG_ORIGIN)
    {
        ride_id_t rideIndex = tileElement->AsTrack()->GetRideIndex();
        // Open ride window in station view
        return window_ride_open_station(rideIndex, tileElement->AsTrack()->GetStationIndex());
    }
    else
    {
        ride_id_t rideIndex = tile_element_get_ride_index(tileElement);
        // Open ride window in overview mode.
        return window_ride_main_open(rideIndex);
    }
}

/**
 *
 *  rct2: 0x006ACAC2
 */
rct_window* window_ride_open_vehicle(rct_vehicle* vehicle)
{
    rct_vehicle* headVehicle = vehicle_get_head(vehicle);
    uint16_t headVehicleSpriteIndex = headVehicle->sprite_index;
    ride_id_t rideIndex = headVehicle->ride;
    Ride* ride = get_ride(rideIndex);

    // Get view index
    int32_t view = 1;
    for (int32_t i = 0; i < MAX_VEHICLES_PER_RIDE; i++)
    {
        if (ride->vehicles[i] == headVehicleSpriteIndex)
            break;

        view++;
    }

    rct_window* w = window_find_by_number(WC_RIDE, rideIndex);
    if (w != nullptr)
    {
        window_invalidate(w);

        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && gCurrentToolWidget.window_classification == w->classification
            && gCurrentToolWidget.window_number == w->number)
        {
            tool_cancel();
        }

        int32_t openedPeepWindow = 0;
        if (w->ride.view == view)
        {
            int32_t numPeepsLeft = vehicle->num_peeps;
            for (int32_t i = 0; i < 32 && numPeepsLeft > 0; i++)
            {
                uint16_t peepSpriteIndex = vehicle->peep[i];
                if (peepSpriteIndex == SPRITE_INDEX_NULL)
                    continue;

                numPeepsLeft--;
                rct_window* w2 = window_find_by_number(WC_PEEP, peepSpriteIndex);
                if (w2 == nullptr)
                {
                    rct_peep* peep = &(get_sprite(peepSpriteIndex)->peep);
                    auto intent = Intent(WC_PEEP);
                    intent.putExtra(INTENT_EXTRA_PEEP, peep);
                    context_open_intent(&intent);
                    openedPeepWindow = 1;

                    break;
                }
            }
        }

        w = openedPeepWindow ? window_find_by_number(WC_RIDE, rideIndex) : window_bring_to_front_by_number(WC_RIDE, rideIndex);
    }

    if (w == nullptr)
    {
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
static void window_ride_set_page(rct_window* w, int32_t page)
{
    int32_t listen;

    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();

    // Set listen only to viewport
    listen = 0;
    if (page == WINDOW_RIDE_PAGE_MAIN && w->page == WINDOW_RIDE_PAGE_MAIN && w->viewport != nullptr
        && !(w->viewport->flags & VIEWPORT_FLAG_SOUND_ON))
        listen++;

    w->page = page;
    w->frame_no = 0;
    w->var_492 = 0;

    // There doesn't seem to be any need for this call, and it can sometimes modify the reported number of cars per train, so
    // I've removed it if (page == WINDOW_RIDE_PAGE_VEHICLE) { ride_update_max_vehicles(ride);
    //}

    if (w->viewport != nullptr)
    {
        w->viewport->width = 0;
        w->viewport = nullptr;
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

    if (listen != 0 && w->viewport != nullptr)
        w->viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
}

static void window_ride_set_pressed_tab(rct_window* w)
{
    int32_t i;
    for (i = 0; i < WINDOW_RIDE_PAGE_COUNT; i++)
        w->pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
    w->pressed_widgets |= 1LL << (WIDX_TAB_1 + w->page);
}

static void window_ride_anchor_border_widgets(rct_window* w)
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
static void window_ride_init_viewport(rct_window* w)
{
    if (w->page != WINDOW_RIDE_PAGE_MAIN)
        return;

    Ride* ride = get_ride(w->number);
    int32_t eax = w->viewport_focus_coordinates.var_480 - 1;

    union
    {
        sprite_focus sprite;
        coordinate_focus coordinate;
    } focus;

    focus.coordinate.x = 0;
    focus.coordinate.y = 0;
    focus.coordinate.z = 0;
    focus.sprite.sprite_id = SPRITE_INDEX_NULL;
    focus.coordinate.zoom = 0;
    focus.coordinate.rotation = get_current_rotation();
    focus.coordinate.width = 0;
    focus.coordinate.height = 0;

    if (eax >= 0 && eax < ride->num_vehicles && ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK)
    {
        focus.sprite.sprite_id = ride->vehicles[eax];

        rct_ride_entry* ride_entry = get_ride_entry_by_ride(ride);
        if (ride_entry && ride_entry->tab_vehicle != 0)
        {
            rct_vehicle* vehicle = GET_VEHICLE(focus.sprite.sprite_id);
            if (vehicle->next_vehicle_on_train != SPRITE_INDEX_NULL)
            {
                focus.sprite.sprite_id = vehicle->next_vehicle_on_train;
            }
        }
        if (focus.sprite.sprite_id != SPRITE_INDEX_NULL)
        {
            focus.sprite.type |= VIEWPORT_FOCUS_TYPE_SPRITE;
        }
    }
    else if (eax >= ride->num_vehicles && eax < (ride->num_vehicles + ride->num_stations))
    {
        int32_t stationIndex = -1;
        int32_t count = eax - ride->num_vehicles;
        do
        {
            stationIndex++;
            if (ride->stations[stationIndex].Start.xy != RCT_XY8_UNDEFINED)
            {
                count--;
            }
        } while (count >= 0);

        LocationXY8 location = ride->stations[stationIndex].Start;

        focus.coordinate.x = location.x * 32;
        focus.coordinate.y = location.y * 32;
        focus.coordinate.z = ride->stations[stationIndex].Height << 3;
        focus.sprite.type |= VIEWPORT_FOCUS_TYPE_COORDINATE;
    }
    else
    {
        if (eax > 0)
        {
            w->viewport_focus_coordinates.var_480 = 0;
        }

        ride_overall_view* view = &ride_overall_views[w->number];

        focus.coordinate.x = view->x;
        focus.coordinate.y = view->y;
        focus.coordinate.z = view->z;
        focus.coordinate.zoom = view->zoom;

        focus.sprite.type |= VIEWPORT_FOCUS_TYPE_COORDINATE;
    }
    focus.coordinate.var_480 = w->viewport_focus_coordinates.var_480;

    uint16_t viewport_flags = 0;

    if (w->viewport != nullptr)
    {
        if (focus.coordinate.x == w->viewport_focus_coordinates.x
            && (focus.coordinate.y & VIEWPORT_FOCUS_Y_MASK) == w->viewport_focus_coordinates.y
            && focus.coordinate.z == w->viewport_focus_coordinates.z
            && focus.coordinate.rotation == w->viewport_focus_coordinates.rotation
            && focus.coordinate.zoom == w->viewport_focus_coordinates.zoom && focus.coordinate.width == w->width
            && focus.coordinate.height == w->height)
        {
            return;
        }
        viewport_flags = w->viewport->flags;
        w->viewport->width = 0;
        w->viewport = nullptr;
    }
    else if (gConfigGeneral.always_show_gridlines)
    {
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

    // rct2: 0x006aec9c only used here so brought it into the function
    if (!w->viewport && ride->overall_view.xy != RCT_XY8_UNDEFINED)
    {
        rct_widget* view_widget = &w->widgets[WIDX_VIEWPORT];

        int32_t x = view_widget->left + 1 + w->x;
        int32_t y = view_widget->top + 1 + w->y;
        int32_t width = view_widget->right - view_widget->left - 1;
        int32_t height = view_widget->bottom - view_widget->top - 1;
        viewport_create(
            w, x, y, width, height, focus.coordinate.zoom, focus.coordinate.x, focus.coordinate.y & VIEWPORT_FOCUS_Y_MASK,
            focus.coordinate.z, focus.sprite.type & VIEWPORT_FOCUS_TYPE_MASK, focus.sprite.sprite_id);

        w->flags |= WF_NO_SCROLLING;
        window_invalidate(w);
    }
    if (w->viewport)
    {
        w->viewport->flags = viewport_flags;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006AF315
 */
static void window_ride_rename(rct_window* w)
{
    Ride* ride;

    ride = get_ride(w->number);
    set_format_arg(16, uint32_t, ride->name_arguments);
    window_text_input_open(
        w, WIDX_RENAME, STR_RIDE_ATTRACTION_NAME, STR_ENTER_NEW_NAME_FOR_THIS_RIDE_ATTRACTION, ride->name, ride->name_arguments,
        32);
}

/**
 *
 *  rct2: 0x006AF17E
 */
static void window_ride_main_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    ride_id_t rideIndex;
    int32_t status;

    switch (widgetIndex)
    {
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
            rideIndex = (uint8_t)w->number;
            ride_construct(rideIndex);
            if (window_find_by_number(WC_RIDE_CONSTRUCTION, rideIndex) != nullptr)
            {
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
            context_open_detail_window(WD_DEMOLISH_RIDE, w->number);
            break;
        case WIDX_CLOSE_LIGHT:
        case WIDX_TEST_LIGHT:
        case WIDX_OPEN_LIGHT:
            switch (widgetIndex)
            {
                default:
                case WIDX_CLOSE_LIGHT:
                    status = RIDE_STATUS_CLOSED;
                    break;
                case WIDX_TEST_LIGHT:
                    status = RIDE_STATUS_TESTING;
                    break;
                case WIDX_OPEN_LIGHT:
                    status = RIDE_STATUS_OPEN;
                    break;
            }

            ride_set_status(w->number, status);
            break;
    }
}

/**
 *
 *  rct2: 0x006AF4A2
 */
static void window_ride_main_resize(rct_window* w)
{
    const int32_t offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
    w->flags |= WF_RESIZABLE;
    int32_t minHeight = 180 + offset;
    if (theme_get_flags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
        minHeight = 200 + offset + RCT1_LIGHT_OFFSET
            - (ride_type_has_flag(get_ride(w->number)->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? 14 : 0);
    window_set_resize(w, 316, minHeight, 500, 450);
    window_ride_init_viewport(w);
}

/**
 *
 *  rct2: 0x006AF825
 */
static void window_ride_show_view_dropdown(rct_window* w, rct_widget* widget)
{
    rct_widget* dropdownWidget = widget - 1;
    Ride* ride = get_ride(w->number);

    int32_t numItems = 1;
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES))
    {
        numItems += ride->num_stations;
        numItems += ride->num_vehicles;
    }

    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], 0, 0, numItems, widget->right - dropdownWidget->left);

    // First item
    gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[0] = STR_OVERALL_VIEW;
    int32_t currentItem = 1;

    // Vehicles
    int32_t name = RideComponentNames[RideNameConvention[ride->type].vehicle].number;
    for (int32_t i = 1; i <= ride->num_vehicles; i++)
    {
        gDropdownItemsFormat[currentItem] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[currentItem] = name | (currentItem << 16);
        currentItem++;
    }

    // Stations
    name = RideComponentNames[RideNameConvention[ride->type].station].number;
    for (int32_t i = 1; i <= ride->num_stations; i++)
    {
        gDropdownItemsFormat[currentItem] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[currentItem] = name | (i << 16);
        currentItem++;
    }

    // Set highlighted item
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
    {
        for (int32_t i = 0; i < ride->num_vehicles; i++)
        {
            // The +1 is to skip 'Overall view'
            dropdown_set_disabled(i + 1, true);
            ;
        }
    }

    // Set checked item
    dropdown_set_checked(w->ride.view, true);
}

/**
 *
 *  rct2: 0x006AF64C
 */
static void window_ride_show_open_dropdown(rct_window* w, rct_widget* widget)
{
    Ride* ride;
    int32_t numItems, highlightedIndex = 0, checkedIndex;

    ride = get_ride(w->number);

    numItems = 0;
    gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[numItems] = STR_CLOSE_RIDE;
    numItems++;

    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE))
    {
        gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[numItems] = STR_TEST_RIDE;
        numItems++;
    }

    gDropdownItemsFormat[numItems] = STR_DROPDOWN_MENU_LABEL;
    gDropdownItemsArgs[numItems] = STR_OPEN_RIDE;
    numItems++;

    window_dropdown_show_text(
        w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1, w->colours[1], 0, numItems);

    checkedIndex = ride->status;
    switch (ride->status)
    {
        case RIDE_STATUS_CLOSED:
            highlightedIndex = 0;
            if ((ride->lifecycle_flags & RIDE_LIFECYCLE_CRASHED)
                || (ride->lifecycle_flags & RIDE_LIFECYCLE_HAS_STALLED_VEHICLE))
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

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE))
    {
        if (checkedIndex != 0)
            checkedIndex--;
        if (highlightedIndex != 0)
            highlightedIndex--;
    }

    dropdown_set_checked(checkedIndex, true);
    gDropdownDefaultIndex = highlightedIndex;
}

static void populate_ride_type_dropdown()
{
    auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
    if (RideDropdownDataLanguage == ls.GetCurrentLanguage())
        return;

    RideDropdownData.clear();

    for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
    {
        RideTypeLabel label = { i, RideNaming[i].name, ls.GetString(RideNaming[i].name) };
        RideDropdownData.push_back(label);
    }

    std::sort(RideDropdownData.begin(), RideDropdownData.end(), [](auto& a, auto& b) {
        return std::strcmp(a.label_string, b.label_string) < 0;
    });

    RideDropdownDataLanguage = ls.GetCurrentLanguage();
}

static void window_ride_show_ride_type_dropdown(rct_window* w, rct_widget* widget)
{
    Ride* ride = get_ride(w->number);
    populate_ride_type_dropdown();

    for (size_t i = 0; i < RideDropdownData.size(); i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = RideDropdownData[i].label_id;
    }

    rct_widget* dropdownWidget = widget - 1;
    window_dropdown_show_text(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], DROPDOWN_FLAG_STAY_OPEN, RIDE_TYPE_COUNT);

    // Find the current ride type in the ordered list.
    uint8_t pos = 0;
    for (uint8_t i = 0; i < RIDE_TYPE_COUNT; i++)
    {
        if (RideDropdownData[i].ride_type_id == ride->type)
        {
            pos = i;
            break;
        }
    }

    gDropdownHighlightedIndex = pos;
    gDropdownDefaultIndex = pos;
    dropdown_set_checked(pos, true);
}

static void populate_vehicle_type_dropdown(Ride* ride)
{
    rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);

    bool selectionShouldBeExpanded;
    int32_t rideTypeIterator, rideTypeIteratorMax;
    if (gCheatsShowVehiclesFromOtherTrackTypes
        && !(
            ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) || ride->type == RIDE_TYPE_MAZE
            || ride->type == RIDE_TYPE_MINI_GOLF))
    {
        selectionShouldBeExpanded = true;
        rideTypeIterator = 0;
        rideTypeIteratorMax = RIDE_TYPE_COUNT - 1;
    }
    else
    {
        selectionShouldBeExpanded = false;
        rideTypeIterator = ride->type;
        rideTypeIteratorMax = ride->type;
    }

    // Don't repopulate the list if we just did.
    auto& ls = OpenRCT2::GetContext()->GetLocalisationService();
    if (VehicleDropdownExpanded == selectionShouldBeExpanded && VehicleDropdownRideType == rideEntry
        && VehicleDropdownDataLanguage == ls.GetCurrentLanguage())
        return;

    VehicleDropdownData.clear();

    for (; rideTypeIterator <= rideTypeIteratorMax; rideTypeIterator++)
    {
        if (selectionShouldBeExpanded && ride_type_has_flag(rideTypeIterator, RIDE_TYPE_FLAG_FLAT_RIDE))
            continue;
        if (selectionShouldBeExpanded && (rideTypeIterator == RIDE_TYPE_MAZE || rideTypeIterator == RIDE_TYPE_MINI_GOLF))
            continue;

        uint8_t* rideEntryIndexPtr = get_ride_entry_indices_for_ride_type(rideTypeIterator);

        for (uint8_t* currentRideEntryIndex = rideEntryIndexPtr; *currentRideEntryIndex != RIDE_ENTRY_INDEX_NULL;
             currentRideEntryIndex++)
        {
            int32_t rideEntryIndex = *currentRideEntryIndex;
            rct_ride_entry* currentRideEntry = get_ride_entry(rideEntryIndex);

            // Skip if vehicle type has not been invented yet
            if (!ride_entry_is_invented(rideEntryIndex) && !gCheatsIgnoreResearchStatus)
                continue;

            // Skip if vehicle does not belong to the same ride group
            if (RideGroupManager::RideTypeHasRideGroups(ride->type) && !selectionShouldBeExpanded)
            {
                const RideGroup* rideGroup = RideGroupManager::GetRideGroup(ride->type, rideEntry);
                const RideGroup* currentRideGroup = RideGroupManager::GetRideGroup(ride->type, currentRideEntry);

                if (!rideGroup->Equals(currentRideGroup))
                    continue;
            }

            VehicleTypeLabel label = { rideEntryIndex, currentRideEntry->naming.name,
                                       ls.GetString(currentRideEntry->naming.name) };
            VehicleDropdownData.push_back(label);
        }
    }

    std::sort(VehicleDropdownData.begin(), VehicleDropdownData.end(), [](auto& a, auto& b) {
        return std::strcmp(a.label_string, b.label_string) < 0;
    });

    VehicleDropdownExpanded = selectionShouldBeExpanded;
    VehicleDropdownRideType = rideEntry;
    VehicleDropdownDataLanguage = ls.GetCurrentLanguage();
}

static void window_ride_show_vehicle_type_dropdown(rct_window* w, rct_widget* widget)
{
    Ride* ride = get_ride(w->number);
    populate_vehicle_type_dropdown(ride);

    size_t numItems = std::min<size_t>(VehicleDropdownData.size(), DROPDOWN_ITEMS_MAX_SIZE);

    for (size_t i = 0; i < numItems; i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = VehicleDropdownData[i].label_id;
    }

    rct_widget* dropdownWidget = widget - 1;
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numItems, widget->right - dropdownWidget->left);

    // Find the current vehicle type in the ordered list.
    uint8_t pos = 0;
    for (uint8_t i = 0; i < VehicleDropdownData.size(); i++)
    {
        if (VehicleDropdownData[i].subtype_id == ride->subtype)
        {
            pos = i;
            break;
        }
    }

    gDropdownHighlightedIndex = pos;
    gDropdownDefaultIndex = pos;
    dropdown_set_checked(pos, true);
}

/**
 *
 *  rct2: 0x006AF1BD
 */
static void window_ride_main_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_VIEW_DROPDOWN:
            window_ride_show_view_dropdown(w, widget);
            break;
        case WIDX_OPEN:
            window_ride_show_open_dropdown(w, widget);
            break;
        case WIDX_RIDE_TYPE_DROPDOWN:
            window_ride_show_ride_type_dropdown(w, widget);
            break;
    }
}

/**
 *
 *  rct2: 0x006AF300
 */
static void window_ride_main_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    Ride* ride;
    int32_t status = 0;

    switch (widgetIndex)
    {
        case WIDX_VIEW_DROPDOWN:
            if (dropdownIndex == -1)
            {
                dropdownIndex = w->ride.view;
                ride = get_ride(w->number);
                dropdownIndex++;
                if (dropdownIndex != 0 && dropdownIndex <= ride->num_vehicles
                    && !(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
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

            switch (dropdownIndex)
            {
                case 0:
                    status = RIDE_STATUS_CLOSED;
                    break;
                case 1:
                    status = RIDE_STATUS_TESTING;
                    break;
                case 2:
                    status = RIDE_STATUS_OPEN;
                    break;
            }
            ride_set_status(w->number, status);
            break;
        case WIDX_RIDE_TYPE_DROPDOWN:
            if (dropdownIndex != -1 && dropdownIndex < RIDE_TYPE_COUNT)
            {
                uint8_t rideLabelId = std::clamp(dropdownIndex, 0, RIDE_TYPE_COUNT - 1);
                uint8_t rideType = RideDropdownData[rideLabelId].ride_type_id;
                if (rideType < RIDE_TYPE_COUNT)
                {
                    set_operating_setting(w->number, RIDE_SETTING_RIDE_TYPE, rideType);
                }
                window_invalidate_all();
            }
    }
}

/**
 *
 *  rct2: 0x006AF40F
 */
static void window_ride_main_update(rct_window* w)
{
    // Update tab animation
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);

    // Update status
    Ride* ride = get_ride(w->number);
    if (!(ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAIN))
    {
        if (w->ride.view == 0)
            return;

        if (w->ride.view <= ride->num_vehicles)
        {
            int32_t vehicleIndex = w->ride.view - 1;
            uint16_t vehicleSpriteIndex = ride->vehicles[vehicleIndex];
            if (vehicleSpriteIndex == SPRITE_INDEX_NULL)
                return;

            rct_vehicle* vehicle = &(get_sprite(vehicleSpriteIndex)->vehicle);
            if (vehicle->status != 4 && vehicle->status != 22 && vehicle->status != 10 && vehicle->status != 7)
            {
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
static void window_ride_main_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_RENAME || text == nullptr)
        return;

    ride_set_name(w->number, text, 0);
}

/**
 *
 *  rct2: 0x006AF55A
 */
static void window_ride_main_viewport_rotate(rct_window* w)
{
    window_ride_init_viewport(w);
}

/**
 *
 *  rct2: 0x006AECF6
 */
static void window_ride_main_invalidate(rct_window* w)
{
    rct_widget* widgets;
    int32_t i, height;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    w->disabled_widgets &= ~((1 << WIDX_DEMOLISH) | (1 << WIDX_CONSTRUCTION));
    if (ride->lifecycle_flags & (RIDE_LIFECYCLE_INDESTRUCTIBLE | RIDE_LIFECYCLE_INDESTRUCTIBLE_TRACK))
        w->disabled_widgets |= (1 << WIDX_DEMOLISH);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);
    set_format_arg(6, uint16_t, RideNaming[ride->type].name);
    uint32_t spriteIds[] = {
        SPR_CLOSED,
        SPR_OPEN,
        SPR_TESTING,
    };
    window_ride_main_widgets[WIDX_OPEN].image = spriteIds[ride->status];

    window_ride_main_widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + (ride->status == RIDE_STATUS_CLOSED) * 2
        + widget_is_pressed(w, WIDX_CLOSE_LIGHT);
    window_ride_main_widgets[WIDX_TEST_LIGHT].image = SPR_G2_RCT1_TEST_BUTTON_0 + (ride->status == RIDE_STATUS_TESTING) * 2
        + widget_is_pressed(w, WIDX_TEST_LIGHT);
    window_ride_main_widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + (ride->status == RIDE_STATUS_OPEN) * 2
        + widget_is_pressed(w, WIDX_OPEN_LIGHT);

    window_ride_anchor_border_widgets(w);

    const int32_t offset = gCheatsAllowArbitraryRideTypeChanges ? 15 : 0;
    // Anchor main page specific widgets
    window_ride_main_widgets[WIDX_VIEWPORT].right = w->width - 26;
    window_ride_main_widgets[WIDX_VIEWPORT].bottom = w->height - (14 + offset);
    window_ride_main_widgets[WIDX_STATUS].right = w->width - 26;
    window_ride_main_widgets[WIDX_STATUS].top = w->height - (13 + offset);
    window_ride_main_widgets[WIDX_STATUS].bottom = w->height - (3 + offset);
    window_ride_main_widgets[WIDX_VIEW].right = w->width - 60;
    window_ride_main_widgets[WIDX_VIEW_DROPDOWN].right = w->width - 61;
    window_ride_main_widgets[WIDX_VIEW_DROPDOWN].left = w->width - 71;
    window_ride_main_widgets[WIDX_RIDE_TYPE].right = w->width - 26;
    window_ride_main_widgets[WIDX_RIDE_TYPE].top = w->height - 17;
    window_ride_main_widgets[WIDX_RIDE_TYPE].bottom = w->height - 4;
    window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].left = w->width - 37;
    window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].right = w->width - 27;
    window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].top = w->height - 16;
    window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].bottom = w->height - 5;

    if (!gCheatsAllowArbitraryRideTypeChanges)
    {
        window_ride_main_widgets[WIDX_RIDE_TYPE].type = WWT_EMPTY;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WWT_EMPTY;
    }
    else
    {
        window_ride_main_widgets[WIDX_RIDE_TYPE].type = WWT_DROPDOWN;
        window_ride_main_widgets[WIDX_RIDE_TYPE_DROPDOWN].type = WWT_BUTTON;
    }

    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

    if (theme_get_flags() & UITHEME_FLAG_USE_LIGHTS_RIDE)
    {
        window_ride_main_widgets[WIDX_OPEN].type = WWT_EMPTY;
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WWT_IMGBTN;
        window_ride_main_widgets[WIDX_TEST_LIGHT].type
            = (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_TEST_MODE) ? WWT_EMPTY : WWT_IMGBTN);
        window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WWT_IMGBTN;

        height = 62;
        if (window_ride_main_widgets[WIDX_TEST_LIGHT].type != WWT_EMPTY)
        {
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
    else
    {
        window_ride_main_widgets[WIDX_OPEN].type = WWT_FLATBTN;
        window_ride_main_widgets[WIDX_CLOSE_LIGHT].type = WWT_EMPTY;
        window_ride_main_widgets[WIDX_TEST_LIGHT].type = WWT_EMPTY;
        window_ride_main_widgets[WIDX_OPEN_LIGHT].type = WWT_EMPTY;
        height = 46;
    }
    for (i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
    {
        window_ride_main_widgets[i].left = w->width - 20;
        window_ride_main_widgets[i].right = w->width - 7;
    }
    for (i = WIDX_OPEN; i <= WIDX_DEMOLISH; i++, height += 24)
    {
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
static rct_string_id window_ride_get_status_overall_view(rct_window* w, void* arguments)
{
    int32_t argument;
    rct_string_id formatSecondary, stringId;

    ride_get_status(w->number, &formatSecondary, &argument);
    std::memcpy(arguments, &formatSecondary, sizeof(formatSecondary));
    std::memcpy((void*)((uintptr_t)arguments + 2), &argument, sizeof(argument));
    stringId = STR_RED_OUTLINED_STRING;
    if (formatSecondary != STR_BROKEN_DOWN && formatSecondary != STR_CRASHED)
        stringId = STR_BLACK_STRING;

    return stringId;
}

/**
 *
 *  rct2: 0x006AEFEF
 */
static rct_string_id window_ride_get_status_vehicle(rct_window* w, void* arguments)
{
    Ride* ride;
    rct_vehicle* vehicle;
    int32_t vehicleIndex;
    uint16_t vehicleSpriteIndex;
    rct_string_id stringId;

    ride = get_ride(w->number);

    vehicleIndex = w->ride.view - 1;
    vehicleSpriteIndex = ride->vehicles[vehicleIndex];
    if (vehicleSpriteIndex == SPRITE_INDEX_NULL)
        return 0;

    vehicle = &(get_sprite(vehicleSpriteIndex)->vehicle);
    if (vehicle->status != VEHICLE_STATUS_CRASHING && vehicle->status != VEHICLE_STATUS_CRASHED)
    {
        int32_t trackType = vehicle->track_type >> 2;
        if (trackType == TRACK_ELEM_BLOCK_BRAKES || trackType == TRACK_ELEM_CABLE_LIFT_HILL
            || trackType == TRACK_ELEM_25_DEG_UP_TO_FLAT || trackType == TRACK_ELEM_60_DEG_UP_TO_FLAT
            || trackType == TRACK_ELEM_DIAG_25_DEG_UP_TO_FLAT || trackType == TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT)
        {
            if (track_piece_is_available_for_ride_type(ride->type, TRACK_BLOCK_BRAKES) && vehicle->velocity == 0)
            {
                *(rct_string_id*)(uintptr_t)arguments = STR_STOPPED_BY_BLOCK_BRAKES;
                return STR_BLACK_STRING;
            }
        }
    }

    stringId = VehicleStatusNames[vehicle->status];

    // Get speed in mph
    *((uint16_t*)((uintptr_t)arguments + 2)) = (abs(vehicle->velocity) * 9) >> 18;

    if (ride->type == RIDE_TYPE_MINI_GOLF)
        return 0;

    if ((RideData4[ride->type].flags & RIDE_TYPE_FLAG4_SINGLE_SESSION)
        && vehicle->status <= VEHICLE_STATUS_UNLOADING_PASSENGERS)
    {
        stringId = SingleSessionVehicleStatusNames[vehicle->status];
    }

    const ride_component_name stationName = RideComponentNames[RideNameConvention[ride->type].station];
    *(rct_string_id*)((uintptr_t)arguments + 4) = (ride->num_stations > 1) ? stationName.number : stationName.singular;
    *((uint16_t*)((uintptr_t)arguments + 6)) = vehicle->current_station + 1;
    *(rct_string_id*)((uintptr_t)arguments + 0) = stringId;
    return stringId != STR_CRASHING && stringId != STR_CRASHED_0 ? STR_BLACK_STRING : STR_RED_OUTLINED_STRING;
}

/**
 *
 *  rct2: 0x006AEF65
 */
static rct_string_id window_ride_get_status_station(rct_window* w, void* arguments)
{
    Ride* ride = get_ride(w->number);
    int32_t count = w->ride.view - ride->num_vehicles - 1;
    int32_t stationIndex = -1;
    rct_string_id stringId = 0;

    do
    {
        stationIndex++;
        if (ride->stations[stationIndex].Start.xy != RCT_XY8_UNDEFINED)
            count--;
    } while (count >= 0);

    // Entrance / exit
    if (ride->status == RIDE_STATUS_CLOSED)
    {
        if (ride_get_entrance_location((uint8_t)w->number, (uint8_t)stationIndex).isNull())
            stringId = STR_NO_ENTRANCE;
        else if (ride_get_exit_location((uint8_t)w->number, (uint8_t)stationIndex).isNull())
            stringId = STR_NO_EXIT;
    }
    else
    {
        if (ride_get_entrance_location((uint8_t)w->number, (uint8_t)stationIndex).isNull())
            stringId = STR_EXIT_ONLY;
    }

    // Queue length
    if (stringId == 0)
    {
        int32_t queueLength = ride->stations[stationIndex].QueueLength;
        set_format_arg_body(static_cast<uint8_t*>(arguments), 2, (uintptr_t)queueLength, sizeof(uint16_t));
        stringId = STR_QUEUE_EMPTY;
        if (queueLength == 1)
            stringId = STR_QUEUE_ONE_PERSON;
        else if (queueLength > 1)
            stringId = STR_QUEUE_PEOPLE;
    }

    set_format_arg_body(static_cast<uint8_t*>(arguments), 0, (uintptr_t)stringId, sizeof(rct_string_id));
    return STR_BLACK_STRING;
}

/**
 *
 *  rct2: 0x006AEE73
 */
static rct_string_id window_ride_get_status(rct_window* w, void* arguments)
{
    Ride* ride = get_ride(w->number);

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
static void window_ride_main_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Ride* ride;
    rct_widget* widget;
    rct_string_id stringId;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    // Viewport and ear icon
    if (w->viewport != nullptr)
    {
        window_draw_viewport(dpi, w);
        if (w->viewport->flags & VIEWPORT_FLAG_SOUND_ON)
            gfx_draw_sprite(dpi, SPR_HEARING_VIEWPORT, w->x + 2, w->y + 2, 0);
    }

    // View dropdown
    ride = get_ride(w->number);
    stringId = STR_OVERALL_VIEW;
    if (w->ride.view != 0)
    {
        stringId = RideComponentNames[RideNameConvention[ride->type].vehicle].number;
        if (w->ride.view > ride->num_vehicles)
        {
            set_format_arg(2, uint16_t, w->ride.view - ride->num_vehicles);
            stringId = RideComponentNames[RideNameConvention[ride->type].station].number;
        }
        else
        {
            set_format_arg(2, uint16_t, w->ride.view);
        }
    }
    set_format_arg(0, uint16_t, stringId);

    widget = &window_ride_main_widgets[WIDX_VIEW];
    gfx_draw_string_centred(
        dpi, STR_WINDOW_COLOUR_2_STRINGID, w->x + (widget->left + widget->right - 11) / 2, w->y + widget->top, COLOUR_BLACK,
        gCommonFormatArgs);

    // Status
    widget = &window_ride_main_widgets[WIDX_STATUS];
    rct_string_id ride_status = window_ride_get_status(w, gCommonFormatArgs);
    gfx_draw_string_centred_clipped(
        dpi, ride_status, gCommonFormatArgs, COLOUR_BLACK, w->x + (widget->left + widget->right) / 2, w->y + widget->top,
        widget->right - widget->left);
}

#pragma endregion

#pragma region Vehicle

/**
 *
 *  rct2: 0x006B272D
 */
static void window_ride_vehicle_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_ride_vehicle_resize(rct_window* w)
{
    window_set_resize(w, 316, 214, 316, 214);
}

/**
 *
 *  rct2: 0x006B2748
 */
static void window_ride_vehicle_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    Ride* ride = get_ride(w->number);

    switch (widgetIndex)
    {
        case WIDX_VEHICLE_TYPE_DROPDOWN:
            window_ride_show_vehicle_type_dropdown(w, &w->widgets[widgetIndex]);
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
            rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);
            if (ride->num_cars_per_train > rideEntry->zero_cars + 1)
                ride_set_num_cars_per_vehicle(w->number, ride->num_cars_per_train - 1);
            break;
    }
}

/**
 *
 *  rct2: 0x006B2767
 */
static void window_ride_vehicle_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_VEHICLE_TYPE_DROPDOWN:
            if (dropdownIndex >= 0 && static_cast<std::size_t>(dropdownIndex) < VehicleDropdownData.size())
            {
                int32_t newRideType = VehicleDropdownData[dropdownIndex].subtype_id;
                ride_set_ride_entry(w->number, newRideType);
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006B2AA1
 */
static void window_ride_vehicle_update(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_2);
}

/**
 *
 *  rct2: 0x006B222C
 */
static void window_ride_vehicle_invalidate(rct_window* w)
{
    rct_widget* widgets;
    Ride* ride;
    rct_ride_entry* rideEntry;
    rct_string_id stringId;
    int32_t carsPerTrain;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    // Widget setup
    carsPerTrain = ride->num_cars_per_train - rideEntry->zero_cars;

    // Vehicle type
    window_ride_vehicle_widgets[WIDX_VEHICLE_TYPE].text = rideEntry->naming.name;

    // Trains
    if (rideEntry->cars_per_flat_ride > 1 || gCheatsDisableTrainLengthLimit)
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_SPINNER;
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WWT_BUTTON;
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WWT_BUTTON;
    }
    else
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS].type = WWT_EMPTY;
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_INCREASE].type = WWT_EMPTY;
        window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_DECREASE].type = WWT_EMPTY;
    }

    // Cars per train
    if (rideEntry->zero_cars + 1 < rideEntry->max_cars_in_train || gCheatsDisableTrainLengthLimit)
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_SPINNER;
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WWT_BUTTON;
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WWT_BUTTON;
    }
    else
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].type = WWT_EMPTY;
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_INCREASE].type = WWT_EMPTY;
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN_DECREASE].type = WWT_EMPTY;
    }

    set_format_arg(6, uint16_t, carsPerTrain);
    RIDE_COMPONENT_TYPE vehicleType = RideNameConvention[ride->type].vehicle;
    stringId = RideComponentNames[vehicleType].count;
    if (ride->num_vehicles > 1)
    {
        stringId = RideComponentNames[vehicleType].count_plural;
    }
    set_format_arg(8, rct_string_id, stringId);
    set_format_arg(10, uint16_t, ride->num_vehicles);

    stringId = RideComponentNames[vehicleType].count;
    if (ride->max_trains > 1)
    {
        stringId = RideComponentNames[vehicleType].count_plural;
    }
    set_format_arg(12, rct_string_id, stringId);
    set_format_arg(14, uint16_t, ride->max_trains);

    set_format_arg(16, uint16_t, std::max(1, ride->min_max_cars_per_train & 0xF) - rideEntry->zero_cars);

    stringId = RideComponentNames[RIDE_COMPONENT_TYPE_CAR].singular;
    if ((ride->min_max_cars_per_train & 0xF) - rideEntry->zero_cars > 1)
    {
        stringId = RideComponentNames[RIDE_COMPONENT_TYPE_CAR].plural;
    }

    set_format_arg(18, rct_string_id, stringId);

    window_ride_anchor_border_widgets(w);
    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

    if (ride->num_cars_per_train > (rideEntry->zero_cars + 1))
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_X_CARS_PER_TRAIN;
    }
    else
    {
        window_ride_vehicle_widgets[WIDX_VEHICLE_CARS_PER_TRAIN].text = STR_1_CAR_PER_TRAIN;
    }
}

/**
 *
 *  rct2: 0x006B23DC
 */
static void window_ride_vehicle_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Ride* ride;
    rct_ride_entry* rideEntry;
    int32_t x, y;
    int16_t factor;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);

    x = w->x + 8;
    y = w->y + 64;

    // Description
    y += gfx_draw_string_left_wrapped(dpi, &rideEntry->naming.description, x, y, 300, STR_BLACK_STRING, COLOUR_BLACK);
    y += 2;

    // Capacity
    gfx_draw_string_left(dpi, STR_CAPACITY, &rideEntry->capacity, COLOUR_BLACK, x, y);

    // Excitement Factor
    factor = rideEntry->excitement_multiplier;
    if (factor > 0)
    {
        y += LIST_ROW_HEIGHT;
        gfx_draw_string_left(dpi, STR_EXCITEMENT_FACTOR, &factor, COLOUR_BLACK, x, y);
    }

    // Intensity Factor
    factor = rideEntry->intensity_multiplier;
    if (factor > 0)
    {
        int32_t lineHeight = font_get_line_height(FONT_SPRITE_BASE_MEDIUM);
        if (lineHeight != 10)
            x += 150;
        else
            y += LIST_ROW_HEIGHT;

        gfx_draw_string_left(dpi, STR_INTENSITY_FACTOR, &factor, COLOUR_BLACK, x, y);

        if (lineHeight != 10)
            x -= 150;
    }

    // Nausea Factor
    factor = rideEntry->nausea_multiplier;
    if (factor > 0)
    {
        y += LIST_ROW_HEIGHT;
        gfx_draw_string_left(dpi, STR_NAUSEA_FACTOR, &factor, COLOUR_BLACK, x, y);
    }
}

struct rct_vehicle_paintinfo
{
    int16_t x;
    int16_t y;
    int32_t sprite_index;
    int32_t tertiary_colour;
};

static rct_vehicle_paintinfo _sprites_to_draw[144];

/**
 *
 *  rct2: 0x006B2502
 */
static void window_ride_vehicle_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    Ride* ride = get_ride(w->number);
    rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);

    // Background
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width, dpi->y + dpi->height, PALETTE_INDEX_12);

    rct_widget* widget = &window_ride_vehicle_widgets[WIDX_VEHICLE_TRAINS_PREVIEW];
    int32_t startX = std::max(2, ((widget->right - widget->left) - ((ride->num_vehicles - 1) * 36)) / 2 - 25);
    int32_t startY = widget->bottom - widget->top - 4;

    rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(
        ride->subtype, ride->num_cars_per_train, 0)];
    startY += rideVehicleEntry->tab_height;

    // For each train
    for (int32_t i = 0; i < ride->num_vehicles; i++)
    {
        rct_vehicle_paintinfo* nextSpriteToDraw = _sprites_to_draw;
        int32_t x = startX;
        int32_t y = startY;

        // For each car in train
        for (int32_t j = 0; j < ride->num_cars_per_train; j++)
        {
            rideVehicleEntry = &rideEntry
                                    ->vehicles[ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, j)];
            x += rideVehicleEntry->spacing / 17432;
            y -= (rideVehicleEntry->spacing / 2) / 17432;

            // Get colour of vehicle
            int32_t vehicleColourIndex = 0;
            switch (ride->colour_scheme_type & 3)
            {
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

            int32_t spriteIndex = 16;
            if (rideVehicleEntry->flags & VEHICLE_ENTRY_FLAG_11)
                spriteIndex /= 2;

            spriteIndex &= rideVehicleEntry->rotation_frame_mask;
            spriteIndex *= rideVehicleEntry->base_num_frames;
            spriteIndex += rideVehicleEntry->base_image_id;
            spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
            spriteIndex |= IMAGE_TYPE_REMAP_2_PLUS;

            nextSpriteToDraw->x = x;
            nextSpriteToDraw->y = y;
            nextSpriteToDraw->sprite_index = spriteIndex;
            nextSpriteToDraw->tertiary_colour = vehicleColour.additional_2;
            nextSpriteToDraw++;

            x += rideVehicleEntry->spacing / 17432;
            y -= (rideVehicleEntry->spacing / 2) / 17432;
        }

        if (ride->type == RIDE_TYPE_REVERSER_ROLLER_COASTER)
        {
            rct_vehicle_paintinfo tmp = *(nextSpriteToDraw - 1);
            *(nextSpriteToDraw - 1) = *(nextSpriteToDraw - 2);
            *(nextSpriteToDraw - 2) = tmp;
        }

        rct_vehicle_paintinfo* current = nextSpriteToDraw;
        while (--current >= _sprites_to_draw)
            gfx_draw_sprite(dpi, current->sprite_index, current->x, current->y, current->tertiary_colour);

        startX += 36;
    }
}

#pragma endregion

#pragma region Operating

static void set_operating_setting(int32_t rideNumber, uint8_t setting, uint8_t value)
{
    gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
    game_do_command(0, (value << 8) | 1, 0, (setting << 8) | rideNumber, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

static void window_ride_mode_tweak_set(rct_window* w, uint8_t value)
{
    Ride* ride = get_ride(w->number);

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
    if (ride->mode == RIDE_MODE_ROTATION || ride->mode == RIDE_MODE_FORWARD_ROTATION
        || ride->mode == RIDE_MODE_BACKWARD_ROTATION)
        gGameCommandErrorTitle = STR_CANT_CHANGE_NUMBER_OF_ROTATIONS;

    set_operating_setting(w->number, RIDE_SETTING_OPERATION_OPTION, value);
}

/**
 *
 *  rct2: 0x006B11D5
 */
static void window_ride_mode_tweak_increase(rct_window* w)
{
    Ride* ride = get_ride(w->number);

    uint8_t maxValue = RideProperties[ride->type].max_value;
    uint8_t minValue = gCheatsFastLiftHill ? 0 : RideProperties[ride->type].min_value;

    if (gCheatsFastLiftHill)
    {
        maxValue = 255;
    }

    uint8_t increment = ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

    window_ride_mode_tweak_set(w, std::clamp<int16_t>(ride->operation_option + increment, minValue, maxValue));
}

/**
 *
 *  rct2: 0x006B120A
 */
static void window_ride_mode_tweak_decrease(rct_window* w)
{
    Ride* ride = get_ride(w->number);

    uint8_t maxValue = RideProperties[ride->type].max_value;
    uint8_t minValue = gCheatsFastLiftHill ? 0 : RideProperties[ride->type].min_value;
    if (gCheatsFastLiftHill)
    {
        maxValue = 255;
    }

    uint8_t decrement = ride->mode == RIDE_MODE_BUMPERCAR ? 10 : 1;

    window_ride_mode_tweak_set(w, std::clamp<int16_t>(ride->operation_option - decrement, minValue, maxValue));
}

/**
 *
 *  rct2: 0x006B1631
 */
static void window_ride_mode_dropdown(rct_window* w, rct_widget* widget)
{
    rct_widget* dropdownWidget;
    Ride* ride;
    const uint8_t *availableModes, *mode;
    int32_t i, numAvailableModes;

    dropdownWidget = widget - 1;
    ride = get_ride(w->number);

    // Seek to available modes for this ride
    availableModes = ride_seek_available_modes(ride);

    // Count number of available modes
    mode = availableModes;
    numAvailableModes = -1;
    do
    {
        numAvailableModes++;
    } while (*(mode++) != 255);

    // Create dropdown list
    for (i = 0; i < numAvailableModes; i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = RideModeNames[availableModes[i]];
    }
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numAvailableModes, widget->right - dropdownWidget->left);

    // Set checked item
    for (i = 0; i < numAvailableModes; i++)
    {
        if (ride->mode == availableModes[i])
        {
            dropdown_set_checked(i, true);
        }
    }
}

/**
 *
 *  rct2: 0x006B15C0
 */
static void window_ride_load_dropdown(rct_window* w, rct_widget* widget)
{
    rct_widget* dropdownWidget;
    int32_t i;

    dropdownWidget = widget - 1;
    Ride* ride = get_ride(w->number);

    for (i = 0; i < 5; i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = VehicleLoadNames[i];
    }
    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 5, widget->right - dropdownWidget->left);

    dropdown_set_checked(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK, true);
}

/**
 *
 *  rct2: 0x006B10A7
 */
static void window_ride_operating_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    Ride* ride;

    ride = get_ride(w->number);

    switch (widgetIndex)
    {
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
            set_operating_setting(
                w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_LEAVE_WHEN_ANOTHER_ARRIVES);
            break;
        case WIDX_MINIMUM_LENGTH_CHECKBOX:
            set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MINIMUM_LENGTH);
            break;
        case WIDX_MAXIMUM_LENGTH_CHECKBOX:
            set_operating_setting(w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_WAIT_FOR_MAXIMUM_LENGTH);
            break;
        case WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX:
            set_operating_setting(
                w->number, RIDE_SETTING_DEPARTURE, ride->depart_flags ^ RIDE_DEPART_SYNCHRONISE_WITH_ADJACENT_STATIONS);
            break;
    }
}

/**
 *
 *  rct2: 0x006B1715
 */
static void window_ride_operating_resize(rct_window* w)
{
    window_set_resize(w, 316, 186, 316, 186);
}

/**
 *
 *  rct2: 0x006B10F4
 */
static void window_ride_operating_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    Ride* ride = get_ride(w->number);
    uint8_t upper_bound, lower_bound;

    switch (widgetIndex)
    {
        case WIDX_MODE_TWEAK_INCREASE:
            window_ride_mode_tweak_increase(w);
            break;
        case WIDX_MODE_TWEAK_DECREASE:
            window_ride_mode_tweak_decrease(w);
            break;
        case WIDX_LIFT_HILL_SPEED_INCREASE:
            upper_bound = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
            lower_bound = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
            set_operating_setting(
                w->number, RIDE_SETTING_LIFT_HILL_SPEED,
                std::clamp<int16_t>(ride->lift_hill_speed + 1, lower_bound, upper_bound));
            break;
        case WIDX_LIFT_HILL_SPEED_DECREASE:
            upper_bound = gCheatsFastLiftHill ? 255 : RideLiftData[ride->type].maximum_speed;
            lower_bound = gCheatsFastLiftHill ? 0 : RideLiftData[ride->type].minimum_speed;
            set_operating_setting(
                w->number, RIDE_SETTING_LIFT_HILL_SPEED,
                std::clamp<int16_t>(ride->lift_hill_speed - 1, lower_bound, upper_bound));
            break;
        case WIDX_MINIMUM_LENGTH_INCREASE:
            upper_bound = 250;
            lower_bound = 0;
            set_operating_setting(
                w->number, RIDE_SETTING_MIN_WAITING_TIME,
                std::clamp<int16_t>(ride->min_waiting_time + 1, lower_bound, upper_bound));
            break;
        case WIDX_MINIMUM_LENGTH_DECREASE:
            upper_bound = 250;
            lower_bound = 0;
            set_operating_setting(
                w->number, RIDE_SETTING_MIN_WAITING_TIME,
                std::clamp<int16_t>(ride->min_waiting_time - 1, lower_bound, upper_bound));
            break;
        case WIDX_MAXIMUM_LENGTH_INCREASE:
            upper_bound = 250;
            lower_bound = 0;
            set_operating_setting(
                w->number, RIDE_SETTING_MAX_WAITING_TIME,
                std::clamp<int16_t>(ride->max_waiting_time + 1, lower_bound, upper_bound));
            break;
        case WIDX_MAXIMUM_LENGTH_DECREASE:
            upper_bound = 250;
            lower_bound = 0;
            set_operating_setting(
                w->number, RIDE_SETTING_MAX_WAITING_TIME,
                std::clamp<int16_t>(ride->max_waiting_time - 1, lower_bound, upper_bound));
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
            set_operating_setting(
                w->number, RIDE_SETTING_NUM_CIRCUITS, std::clamp<int16_t>(ride->num_circuits + 1, lower_bound, upper_bound));
            break;
        case WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE:
            upper_bound = gCheatsFastLiftHill ? 255 : 20;
            lower_bound = 1;
            set_operating_setting(
                w->number, RIDE_SETTING_NUM_CIRCUITS, std::clamp<int16_t>(ride->num_circuits - 1, lower_bound, upper_bound));
            break;
    }
}

/**
 *
 *  rct2: 0x006B1165
 */
static void window_ride_operating_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    Ride* ride;
    const uint8_t* availableModes;

    if (dropdownIndex == -1)
        return;

    ride = get_ride(w->number);

    switch (widgetIndex)
    {
        case WIDX_MODE_DROPDOWN:
            // Seek to available modes for this ride
            availableModes = ride_seek_available_modes(ride);

            set_operating_setting(w->number, RIDE_SETTING_MODE, availableModes[dropdownIndex]);
            break;
        case WIDX_LOAD_DROPDOWN:
            set_operating_setting(
                w->number, RIDE_SETTING_DEPARTURE, (ride->depart_flags & ~RIDE_DEPART_WAIT_FOR_LOAD_MASK) | dropdownIndex);
            break;
    }
}

/**
 *
 *  rct2: 0x006B178E
 */
static void window_ride_operating_update(rct_window* w)
{
    Ride* ride;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_3);

    ride = get_ride(w->number);
    if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_OPERATING)
    {
        ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_OPERATING;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006B0B30
 */
static void window_ride_operating_invalidate(rct_window* w)
{
    rct_widget* widgets;
    Ride* ride;
    rct_string_id format, caption, tooltip;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    ride = get_ride(w->number);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    // Widget setup
    w->pressed_widgets &= ~(
        (1ULL << WIDX_LOAD_CHECKBOX) | (1ULL << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX)
        | (1ULL << WIDX_MINIMUM_LENGTH_CHECKBOX) | (1ULL << WIDX_MAXIMUM_LENGTH_CHECKBOX)
        | (1ULL << WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX));

    // Lift hill speed
    if (track_piece_is_available_for_ride_type(ride->type, TRACK_LIFT_HILL))
    {
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WWT_LABEL;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WWT_SPINNER;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WWT_BUTTON;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WWT_BUTTON;
        set_format_arg(20, uint16_t, ride->lift_hill_speed);
    }
    else
    {
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_LABEL].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_INCREASE].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_LIFT_HILL_SPEED_DECREASE].type = WWT_EMPTY;
    }

    // Number of circuits
    if (ride_can_have_multiple_circuits(ride))
    {
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WWT_LABEL;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WWT_SPINNER;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WWT_BUTTON;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WWT_BUTTON;
        set_format_arg(22, uint16_t, ride->num_circuits);
    }
    else
    {
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_LABEL].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_INCREASE].type = WWT_EMPTY;
        window_ride_operating_widgets[WIDX_OPERATE_NUMBER_OF_CIRCUITS_DECREASE].type = WWT_EMPTY;
    }

    // Leave if another vehicle arrives at station
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LEAVE_WHEN_ANOTHER_VEHICLE_ARRIVES_AT_STATION)
        && ride->num_vehicles > 1 && ride->mode != RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
        && ride->mode != RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WWT_CHECKBOX;
        window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].tooltip
            = STR_LEAVE_IF_ANOTHER_VEHICLE_ARRIVES_TIP;
        window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].text = RideNameConvention[ride->type].vehicle
                == RIDE_COMPONENT_TYPE_BOAT
            ? STR_LEAVE_IF_ANOTHER_BOAT_ARRIVES
            : STR_LEAVE_IF_ANOTHER_TRAIN_ARRIVES;
    }
    else
    {
        window_ride_operating_widgets[WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX].type = WWT_EMPTY;
    }

    // Synchronise with adjacent stations
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_CAN_SYNCHRONISE_ADJACENT_STATIONS))
    {
        window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WWT_CHECKBOX;
        window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].image
            = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS;
        window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].tooltip
            = STR_SYNCHRONISE_WITH_ADJACENT_STATIONS_TIP;
    }
    else
    {
        window_ride_operating_widgets[WIDX_SYNCHRONISE_WITH_ADJACENT_STATIONS_CHECKBOX].type = WWT_EMPTY;
    }

    // Mode
    window_ride_operating_widgets[WIDX_MODE].text = RideModeNames[ride->mode];

    // Waiting
    window_ride_operating_widgets[WIDX_LOAD].text = VehicleLoadNames[(ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD_MASK)];
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS))
    {
        window_ride_operating_widgets[WIDX_LOAD_CHECKBOX].type = WWT_CHECKBOX;
        window_ride_operating_widgets[WIDX_LOAD].type = WWT_DROPDOWN;
        window_ride_operating_widgets[WIDX_LOAD_DROPDOWN].type = WWT_BUTTON;

        window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_CHECKBOX].type = WWT_CHECKBOX;
        window_ride_operating_widgets[WIDX_MINIMUM_LENGTH].type = WWT_SPINNER;
        window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_INCREASE].type = WWT_BUTTON;
        window_ride_operating_widgets[WIDX_MINIMUM_LENGTH_DECREASE].type = WWT_BUTTON;

        window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_CHECKBOX].type = WWT_CHECKBOX;
        window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH].type = WWT_SPINNER;
        window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_INCREASE].type = WWT_BUTTON;
        window_ride_operating_widgets[WIDX_MAXIMUM_LENGTH_DECREASE].type = WWT_BUTTON;

        set_format_arg(10, rct_string_id, STR_FORMAT_SECONDS);
        set_format_arg(12, uint16_t, ride->min_waiting_time);
        set_format_arg(14, rct_string_id, STR_FORMAT_SECONDS);
        set_format_arg(16, uint16_t, ride->max_waiting_time);

        if (ride->depart_flags & RIDE_DEPART_WAIT_FOR_LOAD)
            w->pressed_widgets |= (1 << WIDX_LOAD_CHECKBOX);
    }
    else
    {
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
    set_format_arg(18, uint16_t, ride->operation_option);
    switch (ride->mode)
    {
        case RIDE_MODE_POWERED_LAUNCH_PASSTROUGH:
        case RIDE_MODE_POWERED_LAUNCH:
        case RIDE_MODE_UPWARD_LAUNCH:
        case RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED:
            set_format_arg(18, uint16_t, (ride->launch_speed * 9) / 4);
            format = STR_RIDE_MODE_SPEED_VALUE;
            caption = STR_LAUNCH_SPEED;
            tooltip = STR_LAUNCH_SPEED_TIP;
            break;
        case RIDE_MODE_STATION_TO_STATION:
            set_format_arg(18, uint16_t, (ride->speed * 9) / 4);
            format = STR_RIDE_MODE_SPEED_VALUE;
            caption = STR_SPEED;
            tooltip = STR_SPEED_TIP;
            break;
        case RIDE_MODE_RACE:
            set_format_arg(18, uint16_t, ride->num_laps);
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

    if (format != 0)
    {
        if (ride->type == RIDE_TYPE_TWIST)
        {
            uint16_t arg;
            std::memcpy(&arg, gCommonFormatArgs + 18, sizeof(uint16_t));
            set_format_arg(18, uint16_t, arg * 3);
        }

        window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].type = WWT_LABEL;
        window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].text = caption;
        window_ride_operating_widgets[WIDX_MODE_TWEAK_LABEL].tooltip = tooltip;
        window_ride_operating_widgets[WIDX_MODE_TWEAK].type = WWT_SPINNER;
        window_ride_operating_widgets[WIDX_MODE_TWEAK].text = format;
        window_ride_operating_widgets[WIDX_MODE_TWEAK_INCREASE].type = WWT_BUTTON;
        window_ride_operating_widgets[WIDX_MODE_TWEAK_DECREASE].type = WWT_BUTTON;
        w->pressed_widgets &= ~(1 << WIDX_LEAVE_WHEN_ANOTHER_ARRIVES_CHECKBOX);
    }
    else
    {
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
static void window_ride_operating_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Ride* ride;
    uint16_t blockSections;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    ride = get_ride(w->number);

    // Horizontal rule between mode settings and depart settings
    gfx_fill_rect_inset(
        dpi, w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].left + 4, w->y + 103,
        w->x + window_ride_operating_widgets[WIDX_PAGE_BACKGROUND].right - 5, w->y + 104, w->colours[1],
        INSET_RECT_FLAG_BORDER_INSET);

    // Number of block sections
    if (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED)
    {
        blockSections = ride->num_block_brakes + ride->num_stations;
        gfx_draw_string_left(
            dpi, STR_BLOCK_SECTIONS, &blockSections, COLOUR_BLACK, w->x + 21,
            ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED ? w->y + 89 : w->y + 61);
    }
}

#pragma endregion

#pragma region Maintenance

/**
 *
 *  rct2: 0x006B1AE4
 */
static void window_ride_locate_mechanic(rct_window* w)
{
    Ride* ride;
    rct_peep* mechanic;

    ride = get_ride(w->number);

    // First check if there is a mechanic assigned
    mechanic = ride_get_assigned_mechanic(ride);

    // Otherwise find the closest mechanic
    if (mechanic == nullptr)
        mechanic = ride_find_closest_mechanic(ride, 1);

    if (mechanic == nullptr)
        context_show_error(STR_UNABLE_TO_LOCATE_MECHANIC, STR_NONE);
    else
    {
        auto intent = Intent(WC_PEEP);
        intent.putExtra(INTENT_EXTRA_PEEP, mechanic);
        context_open_intent(&intent);
    }
}

/**
 *
 *  rct2: 0x006B7D08
 */
static void window_ride_maintenance_draw_bar(
    rct_window* w, rct_drawpixelinfo* dpi, int32_t x, int32_t y, int32_t value, int32_t colour)
{
    gfx_fill_rect_inset(dpi, x, y, x + 149, y + 8, w->colours[1], INSET_RECT_F_30);
    if (colour & BAR_BLINK)
    {
        colour &= ~BAR_BLINK;
        if (game_is_not_paused() && (gCurrentRealTimeTicks & 8))
            return;
    }

    value = ((186 * ((value * 2) & 0xFF)) >> 8) & 0xFF;
    if (value > 2)
    {
        gfx_fill_rect_inset(dpi, x + 2, y + 1, x + value + 1, y + 7, colour, 0);
    }
}

/**
 *
 *  rct2: 0x006B1AAD
 */
static void window_ride_maintenance_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
        case WIDX_REFURBISH_RIDE:
            context_open_detail_window(WD_REFURBISH_RIDE, w->number);
            break;
    }
}

/**
 *
 *  rct2: 0x006B1D70
 */
static void window_ride_maintenance_resize(rct_window* w)
{
    window_set_resize(w, 316, 135, 316, 135);
}

/**
 *
 *  rct2: 0x006B1ACE
 */
static void window_ride_maintenance_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    Ride* ride = get_ride(w->number);
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);
    if (rideEntry == nullptr)
    {
        return;
    }

    rct_widget* dropdownWidget = widget;
    int32_t j, num_items;

    switch (widgetIndex)
    {
        case WIDX_INSPECTION_INTERVAL_DROPDOWN:
            dropdownWidget--;
            for (int32_t i = 0; i < 7; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = RideInspectionIntervalNames[i];
            }
            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 7, widget->right - dropdownWidget->left);

            dropdown_set_checked(ride->inspection_interval, true);
            break;

        case WIDX_FORCE_BREAKDOWN:
            num_items = 1;
            for (j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
            {
                if (rideEntry->ride_type[j] != 0xFF)
                    break;
            }
            gDropdownItemsFormat[0] = STR_DROPDOWN_MENU_LABEL;
            gDropdownItemsArgs[0] = STR_DEBUG_FIX_RIDE;
            for (int32_t i = 0; i < 8; i++)
            {
                assert(j < (int32_t)std::size(rideEntry->ride_type));
                if (RideAvailableBreakdowns[rideEntry->ride_type[j]] & (uint8_t)(1 << i))
                {
                    if (i == BREAKDOWN_BRAKES_FAILURE
                        && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
                            || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED))
                    {
                        if (ride->num_vehicles != 1)
                            continue;
                    }
                    gDropdownItemsFormat[num_items] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsArgs[num_items] = RideBreakdownReasonNames[i];
                    num_items++;
                }
            }
            if (num_items == 1)
            {
                context_show_error(STR_DEBUG_NO_BREAKDOWNS_AVAILABLE, STR_NONE);
            }
            else
            {
                window_dropdown_show_text(
                    w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                    w->colours[1], DROPDOWN_FLAG_STAY_OPEN, num_items);

                num_items = 1;
                int32_t breakdownReason = ride->breakdown_reason_pending;
                if (breakdownReason != BREAKDOWN_NONE && (ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING))
                {
                    for (int32_t i = 0; i < 8; i++)
                    {
                        if (RideAvailableBreakdowns[rideEntry->ride_type[j]] & (uint8_t)(1 << i))
                        {
                            if (i == BREAKDOWN_BRAKES_FAILURE
                                && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
                                    || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED))
                            {
                                if (ride->num_vehicles != 1)
                                    continue;
                            }
                            if (i == breakdownReason)
                            {
                                dropdown_set_checked(num_items, true);
                                break;
                            }
                            gDropdownItemsFormat[num_items] = STR_DROPDOWN_MENU_LABEL;
                            gDropdownItemsArgs[num_items] = RideBreakdownReasonNames[i];
                            num_items++;
                        }
                    }
                }

                if ((ride->lifecycle_flags & RIDE_LIFECYCLE_BREAKDOWN_PENDING) == 0)
                {
                    dropdown_set_disabled(0, true);
                }
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006B1AD9
 */
static void window_ride_maintenance_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    rct_vehicle* vehicle;
    Ride* ride = get_ride(w->number);
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);

    switch (widgetIndex)
    {
        case WIDX_INSPECTION_INTERVAL_DROPDOWN:
            gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
            game_do_command(0, (dropdownIndex << 8) | 1, 0, (5 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
            break;

        case WIDX_FORCE_BREAKDOWN:
            if (dropdownIndex == 0)
            {
                switch (ride->breakdown_reason_pending)
                {
                    case BREAKDOWN_SAFETY_CUT_OUT:
                        if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_TRACK))
                            break;
                        for (int32_t i = 0; i < ride->num_vehicles; ++i)
                        {
                            uint16_t spriteId = ride->vehicles[i];
                            while (spriteId != SPRITE_INDEX_NULL)
                            {
                                vehicle = GET_VEHICLE(spriteId);
                                vehicle->update_flags &= ~(
                                    VEHICLE_UPDATE_FLAG_BROKEN_CAR | VEHICLE_UPDATE_FLAG_ZERO_VELOCITY
                                    | VEHICLE_UPDATE_FLAG_BROKEN_TRAIN);
                                spriteId = vehicle->next_vehicle_on_train;
                            }
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
            if (ride->lifecycle_flags
                & (RIDE_LIFECYCLE_BREAKDOWN_PENDING | RIDE_LIFECYCLE_BROKEN_DOWN | RIDE_LIFECYCLE_CRASHED))
            {
                context_show_error(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_ALREADY_BROKEN);
            }
            else if (ride->status == RIDE_STATUS_CLOSED)
            {
                context_show_error(STR_DEBUG_CANT_FORCE_BREAKDOWN, STR_DEBUG_RIDE_IS_CLOSED);
            }
            else
            {
                int32_t j;
                for (j = 0; j < MAX_RIDE_TYPES_PER_RIDE_ENTRY; j++)
                {
                    if (rideEntry->ride_type[j] != RIDE_TYPE_NULL)
                        break;
                }
                int32_t i;
                int32_t num_items = 1;
                for (i = 0; i < BREAKDOWN_COUNT; i++)
                {
                    assert(j < (int32_t)std::size(rideEntry->ride_type));
                    if (RideAvailableBreakdowns[rideEntry->ride_type[j]] & (uint8_t)(1 << i))
                    {
                        if (i == BREAKDOWN_BRAKES_FAILURE
                            && (ride->mode == RIDE_MODE_CONTINUOUS_CIRCUIT_BLOCK_SECTIONED
                                || ride->mode == RIDE_MODE_POWERED_LAUNCH_BLOCK_SECTIONED))
                        {
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
static void window_ride_maintenance_update(rct_window* w)
{
    Ride* ride;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_4);

    ride = get_ride(w->number);
    if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_MAINTENANCE)
    {
        ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_MAINTENANCE;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006B17C8
 */
static void window_ride_maintenance_invalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL].text = RideInspectionIntervalNames[ride->inspection_interval];

    window_ride_anchor_border_widgets(w);
    window_align_tabs(w, WIDX_TAB_1, WIDX_TAB_10);

    if (gConfigGeneral.debugging_tools && network_get_mode() == NETWORK_MODE_NONE)
    {
        window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_FLATBTN;
    }
    else
    {
        window_ride_maintenance_widgets[WIDX_FORCE_BREAKDOWN].type = WWT_EMPTY;
    }

    if (RideAvailableBreakdowns[ride->type] == 0 || !(ride->lifecycle_flags & RIDE_LIFECYCLE_EVER_BEEN_OPENED))
    {
        w->disabled_widgets |= (1 << WIDX_REFURBISH_RIDE);
        window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_CANT_REFURBISH_NOT_NEEDED;
    }
    else
    {
        w->disabled_widgets &= ~(1 << WIDX_REFURBISH_RIDE);
        window_ride_maintenance_widgets[WIDX_REFURBISH_RIDE].tooltip = STR_REFURBISH_RIDE_TIP;
    }
}

/**
 *
 *  rct2: 0x006B1877
 */
static void window_ride_maintenance_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    Ride* ride = get_ride(w->number);

    // Locate mechanic button image
    rct_widget* widget = &window_ride_maintenance_widgets[WIDX_LOCATE_MECHANIC];
    int32_t x = w->x + widget->left;
    int32_t y = w->y + widget->top;
    gfx_draw_sprite(dpi, (gStaffMechanicColour << 24) | IMAGE_TYPE_REMAP | IMAGE_TYPE_REMAP_2_PLUS | SPR_MECHANIC, x, y, 0);

    // Inspection label
    widget = &window_ride_maintenance_widgets[WIDX_INSPECTION_INTERVAL];
    x = w->x + 4;
    y = w->y + widget->top + 1;
    gfx_draw_string_left(dpi, STR_INSPECTION, nullptr, COLOUR_BLACK, x, y);

    // Reliability
    widget = &window_ride_maintenance_widgets[WIDX_PAGE_BACKGROUND];
    x = w->x + widget->left + 4;
    y = w->y + widget->top + 4;

    uint16_t reliability = ride->reliability_percentage;
    gfx_draw_string_left(dpi, STR_RELIABILITY_LABEL_1757, &reliability, COLOUR_BLACK, x, y);
    window_ride_maintenance_draw_bar(w, dpi, x + 103, y, std::max<int32_t>(10, reliability), COLOUR_BRIGHT_GREEN);
    y += 11;

    uint16_t downTime = ride->downtime;
    gfx_draw_string_left(dpi, STR_DOWN_TIME_LABEL_1889, &downTime, COLOUR_BLACK, x, y);
    window_ride_maintenance_draw_bar(w, dpi, x + 103, y, downTime, COLOUR_BRIGHT_RED);
    y += 26;

    // Last inspection
    uint16_t lastInspection = ride->last_inspection;

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

    stringId = (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN) ? STR_CURRENT_BREAKDOWN : STR_LAST_BREAKDOWN;
    rct_string_id breakdownMessage = RideBreakdownReasonNames[ride->breakdown_reason];
    gfx_draw_string_left(dpi, stringId, &breakdownMessage, COLOUR_BLACK, x, y);
    y += 10;

    // Mechanic status
    if (ride->lifecycle_flags & RIDE_LIFECYCLE_BROKEN_DOWN)
    {
        rct_peep* peep;
        uint16_t spriteIndex;

        switch (ride->mechanic_status)
        {
            case RIDE_MECHANIC_STATUS_CALLING:
                stringId = STR_NO_MECHANICS_ARE_HIRED_MESSAGE;

                FOR_ALL_STAFF (spriteIndex, peep)
                {
                    if (peep->staff_type == STAFF_TYPE_MECHANIC)
                    {
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
                stringId = STR_EMPTY;
                break;
        }

        if (stringId != STR_EMPTY)
        {
            if (stringId == STR_CALLING_MECHANIC || stringId == STR_NO_MECHANICS_ARE_HIRED_MESSAGE)
            {
                gfx_draw_string_left_wrapped(dpi, nullptr, x + 4, y, 280, stringId, COLOUR_BLACK);
            }
            else
            {
                rct_peep* mechanicSprite = &(get_sprite(ride->mechanic)->peep);
                if (mechanicSprite->IsMechanic())
                {
                    set_format_arg(0, rct_string_id, mechanicSprite->name_string_idx);
                    set_format_arg(2, uint32_t, mechanicSprite->id);
                    gfx_draw_string_left_wrapped(dpi, gCommonFormatArgs, x + 4, y, 280, stringId, COLOUR_BLACK);
                }
            }
        }
    }
}

#pragma endregion

#pragma region Colour

static uint32_t window_ride_get_colour_button_image(int32_t colour)
{
    return IMAGE_TYPE_TRANSPARENT | SPRITE_ID_PALETTE_COLOUR_1(colour) | SPR_PALETTE_BTN;
}

static int32_t window_ride_has_track_colour(Ride* ride, int32_t trackColour)
{
    // Get station flags (shops don't have them)
    auto stationObjFlags = 0;
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
    {
        auto stationObj = ride_get_station_object(ride);
        if (stationObj != nullptr)
        {
            stationObjFlags = stationObj->Flags;
        }
    }

    switch (trackColour)
    {
        case 0:
            return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_PRIMARY_COLOUR)
                || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN);
        case 1:
            return (stationObjFlags & STATION_OBJECT_FLAGS::HAS_SECONDARY_COLOUR)
                || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL);
        case 2:
            return ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS);
        default:
            return 0;
    }
}

static void window_ride_set_track_colour_scheme(rct_window* w, int32_t x, int32_t y)
{
    TileElement* tileElement;
    uint8_t newColourScheme;
    int32_t interactionType, z, direction;

    newColourScheme = (uint8_t)w->ride_colour;

    LocationXY16 mapCoord = {};
    get_map_coordinates_from_pos(
        x, y, VIEWPORT_INTERACTION_MASK_RIDE, &mapCoord.x, &mapCoord.y, &interactionType, &tileElement, nullptr);
    x = mapCoord.x;
    y = mapCoord.y;

    if (interactionType != VIEWPORT_INTERACTION_ITEM_RIDE)
        return;
    if (tileElement->GetType() != TILE_ELEMENT_TYPE_TRACK)
        return;
    if (tileElement->AsTrack()->GetRideIndex() != w->number)
        return;
    if (tileElement->AsTrack()->GetColourScheme() == newColourScheme)
        return;

    z = tileElement->base_height * 8;
    direction = tileElement->GetDirection();
    auto gameAction = RideSetColourSchemeAction(x, y, z, direction, tileElement->AsTrack()->GetTrackType(), newColourScheme);
    GameActions::Execute(&gameAction);
}

/**
 *
 *  rct2: 0x006B04FA
 */
static void window_ride_colour_close(rct_window* w)
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
static void window_ride_colour_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_ride_colour_resize(rct_window* w)
{
    window_set_resize(w, 316, 207, 316, 207);
}

/**
 *
 *  rct2: 0x006B02C6
 */
static void window_ride_colour_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    Ride* ride;
    uint16_t colourSchemeIndex;
    vehicle_colour vehicleColour;
    rct_widget* dropdownWidget;
    rct_ride_entry* rideEntry;
    int32_t i, numItems;
    rct_string_id stringId;

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);
    colourSchemeIndex = w->ride_colour;
    dropdownWidget = widget - 1;

    switch (widgetIndex)
    {
        case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
            for (i = 0; i < NUM_COLOUR_SCHEMES; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = ColourSchemeNames[i];
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 4, widget->right - dropdownWidget->left);

            dropdown_set_checked(colourSchemeIndex, true);
            break;
        case WIDX_TRACK_MAIN_COLOUR:
            window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour[colourSchemeIndex].main);
            break;
        case WIDX_TRACK_ADDITIONAL_COLOUR:
            window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour[colourSchemeIndex].additional);
            break;
        case WIDX_TRACK_SUPPORT_COLOUR:
            window_dropdown_show_colour(w, widget, w->colours[1], ride->track_colour[colourSchemeIndex].supports);
            break;
        case WIDX_MAZE_STYLE_DROPDOWN:
            for (i = 0; i < 4; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = MazeOptions[i].text;
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, 4, widget->right - dropdownWidget->left);

            dropdown_set_checked(ride->track_colour[colourSchemeIndex].supports, true);
            break;
        case WIDX_ENTRANCE_STYLE_DROPDOWN:
        {
            auto ddIndex = 0;
            auto& objManager = GetContext()->GetObjectManager();
            for (i = 0; i < MAX_STATION_OBJECTS; i++)
            {
                auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(OBJECT_TYPE_STATION, i));
                if (stationObj != nullptr)
                {
                    gDropdownItemsFormat[ddIndex] = STR_DROPDOWN_MENU_LABEL;
                    gDropdownItemsArgs[ddIndex] = stationObj->NameStringId;
                    if (ride->entrance_style == i)
                    {
                        gDropdownItemsFormat[ddIndex] = STR_DROPDOWN_MENU_LABEL_SELECTED;
                    }
                    ddIndex++;
                }
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, ddIndex, widget->right - dropdownWidget->left);
            break;
        }
        case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
            for (i = 0; i < 3; i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = (RideComponentNames[RideNameConvention[ride->type].vehicle].singular << 16)
                    | VehicleColourSchemeNames[i];
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, rideEntry->max_cars_in_train > 1 ? 3 : 2,
                widget->right - dropdownWidget->left);

            dropdown_set_checked(ride->colour_scheme_type & 3, true);
            break;
        case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
            numItems = ride->num_vehicles;
            if ((ride->colour_scheme_type & 3) != VEHICLE_COLOUR_SCHEME_PER_TRAIN)
                numItems = ride->num_cars_per_train;

            stringId = (ride->colour_scheme_type & 3) == VEHICLE_COLOUR_SCHEME_PER_TRAIN ? STR_RIDE_COLOUR_TRAIN_OPTION
                                                                                         : STR_RIDE_COLOUR_VEHICLE_OPTION;
            for (i = 0; i < std::min(numItems, (int32_t)DROPDOWN_ITEMS_MAX_SIZE); i++)
            {
                gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
                gDropdownItemsArgs[i] = ((int64_t)(i + 1) << 32)
                    | ((RideComponentNames[RideNameConvention[ride->type].vehicle].capitalised) << 16) | stringId;
            }

            window_dropdown_show_text_custom_width(
                w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
                w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numItems, widget->right - dropdownWidget->left);

            dropdown_set_checked(w->vehicleIndex, true);
            break;
        case WIDX_VEHICLE_MAIN_COLOUR:
            vehicleColour = ride_get_vehicle_colour(ride, w->vehicleIndex);
            window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.main);
            break;
        case WIDX_VEHICLE_ADDITIONAL_COLOUR_1:
            vehicleColour = ride_get_vehicle_colour(ride, w->vehicleIndex);
            window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.additional_1);
            break;
        case WIDX_VEHICLE_ADDITIONAL_COLOUR_2:
            vehicleColour = ride_get_vehicle_colour(ride, w->vehicleIndex);
            window_dropdown_show_colour(w, widget, w->colours[1], vehicleColour.additional_2);
            break;
    }
}

/**
 *
 *  rct2: 0x006B0331
 */
static void window_ride_colour_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    switch (widgetIndex)
    {
        case WIDX_TRACK_COLOUR_SCHEME_DROPDOWN:
            w->ride_colour = (uint16_t)dropdownIndex;
            window_invalidate(w);
            break;
        case WIDX_TRACK_MAIN_COLOUR:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 0, dropdownIndex, w->ride_colour);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_TRACK_ADDITIONAL_COLOUR:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 1, dropdownIndex, w->ride_colour);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_TRACK_SUPPORT_COLOUR:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 4, dropdownIndex, w->ride_colour);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_MAZE_STYLE_DROPDOWN:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 4, dropdownIndex, w->ride_colour);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_ENTRANCE_STYLE_DROPDOWN:
        {
            auto ddIndex = 0;
            auto& objManager = GetContext()->GetObjectManager();
            for (auto i = 0; i < MAX_STATION_OBJECTS; i++)
            {
                auto stationObj = static_cast<StationObject*>(objManager.GetLoadedObject(OBJECT_TYPE_STATION, i));
                if (stationObj != nullptr)
                {
                    if (ddIndex == dropdownIndex)
                    {
                        auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 6, ddIndex, 0);
                        GameActions::Execute(&rideSetAppearanceAction);
                        break;
                    }
                    ddIndex++;
                }
            }
            break;
        }
        case WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 5, dropdownIndex, 0);
            GameActions::Execute(&rideSetAppearanceAction);
            w->vehicleIndex = 0;
        }
        break;
        case WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN:
            w->vehicleIndex = dropdownIndex;
            window_invalidate(w);
            break;
        case WIDX_VEHICLE_MAIN_COLOUR:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 2, dropdownIndex, w->vehicleIndex);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_VEHICLE_ADDITIONAL_COLOUR_1:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 3, dropdownIndex, w->vehicleIndex);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
        case WIDX_VEHICLE_ADDITIONAL_COLOUR_2:
        {
            auto rideSetAppearanceAction = RideSetAppearanceAction(w->number, 7, dropdownIndex, w->vehicleIndex);
            GameActions::Execute(&rideSetAppearanceAction);
        }
        break;
    }
}

/**
 *
 *  rct2: 0x006B0A8F
 */
static void window_ride_colour_update(rct_window* w)
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
static void window_ride_colour_tooldown(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
        window_ride_set_track_colour_scheme(w, x, y);
}

/**
 *
 *  rct2: 0x006B04F3
 */
static void window_ride_colour_tooldrag(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (widgetIndex == WIDX_PAINT_INDIVIDUAL_AREA)
        window_ride_set_track_colour_scheme(w, x, y);
}

/**
 *
 *  rct2: 0x006AFB36
 */
static void window_ride_colour_invalidate(rct_window* w)
{
    rct_widget* widgets;
    rct_ride_entry* rideEntry;
    Ride* ride;
    TrackColour trackColour;
    vehicle_colour vehicleColour;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    // Track colours
    int32_t colourScheme = w->ride_colour;
    trackColour = ride_get_track_colour(ride, colourScheme);

    // Maze style
    if (ride->type == RIDE_TYPE_MAZE)
    {
        window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WWT_DROPDOWN;
        window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WWT_BUTTON;
        window_ride_colour_widgets[WIDX_MAZE_STYLE].text = MazeOptions[trackColour.supports].text;
    }
    else
    {
        window_ride_colour_widgets[WIDX_MAZE_STYLE].type = WWT_EMPTY;
        window_ride_colour_widgets[WIDX_MAZE_STYLE_DROPDOWN].type = WWT_EMPTY;
    }

    // Track, multiple colour schemes
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_SUPPORTS_MULTIPLE_TRACK_COLOUR))
    {
        window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WWT_DROPDOWN;
        window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WWT_BUTTON;
        window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WWT_FLATBTN;
    }
    else
    {
        window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME].type = WWT_EMPTY;
        window_ride_colour_widgets[WIDX_TRACK_COLOUR_SCHEME_DROPDOWN].type = WWT_EMPTY;
        window_ride_colour_widgets[WIDX_PAINT_INDIVIDUAL_AREA].type = WWT_EMPTY;
    }

    // Track main colour
    if (window_ride_has_track_colour(ride, 0))
    {
        window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_COLOURBTN;
        window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].image = window_ride_get_colour_button_image(trackColour.main);
    }
    else
    {
        window_ride_colour_widgets[WIDX_TRACK_MAIN_COLOUR].type = WWT_EMPTY;
    }

    // Track additional colour
    if (window_ride_has_track_colour(ride, 1))
    {
        window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_COLOURBTN;
        window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].image = window_ride_get_colour_button_image(
            trackColour.additional);
    }
    else
    {
        window_ride_colour_widgets[WIDX_TRACK_ADDITIONAL_COLOUR].type = WWT_EMPTY;
    }

    // Track supports colour
    if (window_ride_has_track_colour(ride, 2) && ride->type != RIDE_TYPE_MAZE)
    {
        window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WWT_COLOURBTN;
        window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].image = window_ride_get_colour_button_image(trackColour.supports);
    }
    else
    {
        window_ride_colour_widgets[WIDX_TRACK_SUPPORT_COLOUR].type = WWT_EMPTY;
    }

    // Track preview
    if (ride_type_has_flag(
            ride->type,
            RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_MAIN | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_ADDITIONAL
                | RIDE_TYPE_FLAG_HAS_TRACK_COLOUR_SUPPORTS))
        window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WWT_SPINNER;
    else
        window_ride_colour_widgets[WIDX_TRACK_PREVIEW].type = WWT_EMPTY;

    // Entrance style
    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_HAS_ENTRANCE_EXIT)
    {
        window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_SPINNER;
        window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_DROPDOWN;
        window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_BUTTON;

        auto stringId = STR_NONE;
        auto stationObj = ride_get_station_object(ride);
        if (stationObj != nullptr)
        {
            stringId = stationObj->NameStringId;
        }
        window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].text = stringId;
    }
    else
    {
        window_ride_colour_widgets[WIDX_ENTRANCE_PREVIEW].type = WWT_EMPTY;
        window_ride_colour_widgets[WIDX_ENTRANCE_STYLE].type = WWT_EMPTY;
        window_ride_colour_widgets[WIDX_ENTRANCE_STYLE_DROPDOWN].type = WWT_EMPTY;
    }

    // Vehicle colours
    if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_NO_VEHICLES)
        && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS))
    {
        int32_t vehicleColourSchemeType = ride->colour_scheme_type & 3;
        if (vehicleColourSchemeType == 0)
            w->vehicleIndex = 0;

        vehicleColour = ride_get_vehicle_colour(ride, w->vehicleIndex);

        window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW].type = WWT_SCROLL;
        window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].type = WWT_COLOURBTN;
        window_ride_colour_widgets[WIDX_VEHICLE_MAIN_COLOUR].image = window_ride_get_colour_button_image(vehicleColour.main);

        bool allowChangingAdditionalColour1 = false;
        bool allowChangingAdditionalColour2 = false;

        for (int32_t i = 0; i < ride->num_cars_per_train; i++)
        {
            uint8_t vehicleTypeIndex = ride_entry_get_vehicle_at_position(ride->subtype, ride->num_cars_per_train, i);

            if (rideEntry->vehicles[vehicleTypeIndex].flags & VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_1)
            {
                allowChangingAdditionalColour1 = true;
            }
            if (rideEntry->vehicles[vehicleTypeIndex].flags & VEHICLE_ENTRY_FLAG_ENABLE_ADDITIONAL_COLOUR_2)
            {
                allowChangingAdditionalColour2 = true;
            }
        }

        // Additional colours
        if (allowChangingAdditionalColour1)
        {
            window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_COLOURBTN;
            window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].image = window_ride_get_colour_button_image(
                vehicleColour.additional_1);
            if (allowChangingAdditionalColour2)
            {
                window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_COLOURBTN;
                window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].image = window_ride_get_colour_button_image(
                    vehicleColour.additional_2);
            }
            else
            {
                window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_EMPTY;
            }
        }
        else
        {
            window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_1].type = WWT_EMPTY;
            window_ride_colour_widgets[WIDX_VEHICLE_ADDITIONAL_COLOUR_2].type = WWT_EMPTY;
        }

        // Vehicle colour scheme type
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_16) && (ride->num_cars_per_train | ride->num_vehicles) > 1)
        {
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WWT_DROPDOWN;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WWT_BUTTON;
        }
        else
        {
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME].type = WWT_EMPTY;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_SCHEME_DROPDOWN].type = WWT_EMPTY;
        }
        set_format_arg(6, rct_string_id, VehicleColourSchemeNames[vehicleColourSchemeType]);
        set_format_arg(8, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].singular);
        set_format_arg(10, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].capitalised);
        set_format_arg(12, uint16_t, w->vehicleIndex + 1);
        // Vehicle index
        if (vehicleColourSchemeType != 0)
        {
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_DROPDOWN;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_BUTTON;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].text = vehicleColourSchemeType == 1
                ? STR_RIDE_COLOUR_TRAIN_VALUE
                : STR_RIDE_COLOUR_VEHICLE_VALUE;
        }
        else
        {
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX].type = WWT_EMPTY;
            window_ride_colour_widgets[WIDX_VEHICLE_COLOUR_INDEX_DROPDOWN].type = WWT_EMPTY;
        }
    }
    else
    {
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
static void window_ride_colour_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    // TODO: This should use lists and identified sprites
    rct_drawpixelinfo clippedDpi;
    rct_widget* widget;
    Ride* ride;
    rct_ride_entry* rideEntry;

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    // Track / shop item preview
    widget = &window_ride_colour_widgets[WIDX_TRACK_PREVIEW];
    if (widget->type != WWT_EMPTY)
        gfx_fill_rect(
            dpi, w->x + widget->left + 1, w->y + widget->top + 1, w->x + widget->right - 1, w->y + widget->bottom - 1,
            PALETTE_INDEX_12);

    auto trackColour = ride_get_track_colour(ride, w->ride_colour);

    //
    if (rideEntry->shop_item == SHOP_ITEM_NONE)
    {
        int32_t x = w->x + widget->left;
        int32_t y = w->y + widget->top;

        // Track
        if (ride->type == RIDE_TYPE_MAZE)
        {
            int32_t spriteIndex = MazeOptions[trackColour.supports].sprite;
            gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
        }
        else
        {
            int32_t spriteIndex = TrackColourPreviews[ride->type].track;
            if (spriteIndex != 0)
            {
                spriteIndex |= SPRITE_ID_PALETTE_COLOUR_2(trackColour.main, trackColour.additional);
                gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
            }

            // Supports
            spriteIndex = TrackColourPreviews[ride->type].supports;
            if (spriteIndex != 0)
            {
                spriteIndex |= SPRITE_ID_PALETTE_COLOUR_1(trackColour.supports);
                gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
            }
        }
    }
    else
    {
        int32_t x = w->x + (widget->left + widget->right) / 2 - 8;
        int32_t y = w->y + (widget->bottom + widget->top) / 2 - 6;

        uint8_t shopItem = rideEntry->shop_item_secondary == SHOP_ITEM_NONE ? rideEntry->shop_item
                                                                            : rideEntry->shop_item_secondary;
        int32_t spriteIndex = ShopItemImage[shopItem];
        spriteIndex |= SPRITE_ID_PALETTE_COLOUR_1(ride->track_colour[0].main);

        gfx_draw_sprite(dpi, spriteIndex, x, y, 0);
    }

    // Entrance preview
    trackColour = ride_get_track_colour(ride, 0);
    widget = &w->widgets[WIDX_ENTRANCE_PREVIEW];
    if (widget->type != WWT_EMPTY)
    {
        if (clip_drawpixelinfo(
                &clippedDpi, dpi, w->x + widget->left + 1, w->y + widget->top + 1, widget->right - widget->left,
                widget->bottom - widget->top))
        {
            gfx_clear(&clippedDpi, PALETTE_INDEX_12);

            auto stationObj = ride_get_station_object(ride);
            if (stationObj != nullptr && stationObj->BaseImageId != 0)
            {
                int32_t terniaryColour = 0;
                if (stationObj->Flags & STATION_OBJECT_FLAGS::IS_TRANSPARENT)
                {
                    terniaryColour = IMAGE_TYPE_TRANSPARENT | (GlassPaletteIds[trackColour.main] << 19);
                }

                int32_t spriteIndex = SPRITE_ID_PALETTE_COLOUR_2(trackColour.main, trackColour.additional);
                spriteIndex += stationObj->BaseImageId;

                // Back
                gfx_draw_sprite(&clippedDpi, spriteIndex, 34, 20, terniaryColour);

                // Front
                gfx_draw_sprite(&clippedDpi, spriteIndex + 4, 34, 20, terniaryColour);

                // Glass
                if (terniaryColour != 0)
                    gfx_draw_sprite(&clippedDpi, ((spriteIndex + 20) & 0x7FFFF) + terniaryColour, 34, 20, terniaryColour);
            }
        }

        gfx_draw_string_left_clipped(dpi, STR_STATION_STYLE, gCommonFormatArgs, COLOUR_BLACK, w->x + 3, w->y + 103, 97);
    }
}

/**
 *
 *  rct2: 0x006B0192
 */
static void window_ride_colour_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    Ride* ride;
    rct_ride_entry* rideEntry;
    rct_widget* vehiclePreviewWidget;
    int32_t trainCarIndex, x, y, spriteIndex;
    vehicle_colour vehicleColour;

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);
    vehiclePreviewWidget = &window_ride_colour_widgets[WIDX_VEHICLE_PREVIEW];
    vehicleColour = ride_get_vehicle_colour(ride, w->vehicleIndex);

    // Background colour
    gfx_fill_rect(dpi, dpi->x, dpi->y, dpi->x + dpi->width - 1, dpi->y + dpi->height - 1, PALETTE_INDEX_12);

    // ?
    x = (vehiclePreviewWidget->right - vehiclePreviewWidget->left) / 2;
    y = vehiclePreviewWidget->bottom - vehiclePreviewWidget->top - 15;

    // ?
    trainCarIndex = (ride->colour_scheme_type & 3) == RIDE_COLOUR_SCHEME_DIFFERENT_PER_CAR ? w->vehicleIndex
                                                                                           : rideEntry->tab_vehicle;

    rct_ride_entry_vehicle* rideVehicleEntry = &rideEntry->vehicles[ride_entry_get_vehicle_at_position(
        ride->subtype, ride->num_cars_per_train, trainCarIndex)];

    y += rideVehicleEntry->tab_height;

    // Draw the coloured spinning vehicle
    spriteIndex = (rideVehicleEntry->flags & VEHICLE_ENTRY_FLAG_11) ? w->frame_no / 4 : w->frame_no / 2;
    spriteIndex &= rideVehicleEntry->rotation_frame_mask;
    spriteIndex *= rideVehicleEntry->base_num_frames;
    spriteIndex += rideVehicleEntry->base_image_id;
    spriteIndex |= (vehicleColour.additional_1 << 24) | (vehicleColour.main << 19);
    spriteIndex |= IMAGE_TYPE_REMAP_2_PLUS;
    gfx_draw_sprite(dpi, spriteIndex, x, y, vehicleColour.additional_2);
}

#pragma endregion

#pragma region Music

static constexpr const uint8_t MusicStyleOrder[] = {
    MUSIC_STYLE_GENTLE,       MUSIC_STYLE_SUMMER,        MUSIC_STYLE_WATER,     MUSIC_STYLE_RAGTIME,      MUSIC_STYLE_TECHNO,
    MUSIC_STYLE_MECHANICAL,   MUSIC_STYLE_MODERN,        MUSIC_STYLE_WILD_WEST, MUSIC_STYLE_PIRATES,      MUSIC_STYLE_ROCK,
    MUSIC_STYLE_ROCK_STYLE_2, MUSIC_STYLE_ROCK_STYLE_3,  MUSIC_STYLE_FANTASY,   MUSIC_STYLE_HORROR,       MUSIC_STYLE_TOYLAND,
    MUSIC_STYLE_CANDY_STYLE,  MUSIC_STYLE_ROMAN_FANFARE, MUSIC_STYLE_ORIENTAL,  MUSIC_STYLE_MARTIAN,      MUSIC_STYLE_SPACE,
    MUSIC_STYLE_JUNGLE_DRUMS, MUSIC_STYLE_JURASSIC,      MUSIC_STYLE_EGYPTIAN,  MUSIC_STYLE_DODGEMS_BEAT, MUSIC_STYLE_SNOW,
    MUSIC_STYLE_ICE,          MUSIC_STYLE_MEDIEVAL,      MUSIC_STYLE_URBAN,     MUSIC_STYLE_ORGAN
};

static uint8_t window_ride_current_music_style_order[42];

/**
 *
 *  rct2: 0x006B215D
 */
static void window_ride_toggle_music(rct_window* w)
{
    Ride* ride = get_ride(w->number);

    int32_t activateMusic = (ride->lifecycle_flags & RIDE_LIFECYCLE_MUSIC) ? 0 : 1;

    gGameCommandErrorTitle = STR_CANT_CHANGE_OPERATING_MODE;
    game_do_command(0, (activateMusic << 8) | 1, 0, (6 << 8) | w->number, GAME_COMMAND_SET_RIDE_SETTING, 0, 0);
}

/**
 *
 *  rct2: 0x006B1ED7
 */
static void window_ride_music_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_ride_music_resize(rct_window* w)
{
    w->flags |= WF_RESIZABLE;
    window_set_resize(w, 316, 81, 316, 81);
}

/**
 *
 *  rct2: 0x006B1EFC
 */
static void window_ride_music_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    rct_widget* dropdownWidget;
    int32_t i;

    if (widgetIndex != WIDX_MUSIC_DROPDOWN)
        return;

    dropdownWidget = widget - 1;
    Ride* ride = get_ride(w->number);

    int32_t numItems = 0;
    if (ride->type == RIDE_TYPE_MERRY_GO_ROUND)
    {
        window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_FAIRGROUND_ORGAN;
    }
    else
    {
        for (size_t n = 0; n < std::size(MusicStyleOrder); n++)
            window_ride_current_music_style_order[numItems++] = MusicStyleOrder[n];

        if (gRideMusicInfoList[36].length != 0)
            window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_1;
        if (gRideMusicInfoList[37].length != 0)
            window_ride_current_music_style_order[numItems++] = MUSIC_STYLE_CUSTOM_MUSIC_2;
    }

    for (i = 0; i < numItems; i++)
    {
        gDropdownItemsFormat[i] = STR_DROPDOWN_MENU_LABEL;
        gDropdownItemsArgs[i] = MusicStyleNames[window_ride_current_music_style_order[i]];
    }

    window_dropdown_show_text_custom_width(
        w->x + dropdownWidget->left, w->y + dropdownWidget->top, dropdownWidget->bottom - dropdownWidget->top + 1,
        w->colours[1], 0, DROPDOWN_FLAG_STAY_OPEN, numItems, widget->right - dropdownWidget->left);

    for (i = 0; i < numItems; i++)
    {
        if (window_ride_current_music_style_order[i] == ride->music)
        {
            dropdown_set_checked(i, true);
        }
    }
}

/**
 *
 *  rct2: 0x006B1F03
 */
static void window_ride_music_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    uint8_t musicStyle;

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
static void window_ride_music_update(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_6);
}

/**
 *
 *  rct2: 0x006B1DEA
 */
static void window_ride_music_invalidate(rct_window* w)
{
    rct_widget* widgets;
    int32_t isMusicActivated;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    // Set selected music
    window_ride_music_widgets[WIDX_MUSIC].text = MusicStyleNames[ride->music];

    // Set music activated
    isMusicActivated = ride->lifecycle_flags & (RIDE_LIFECYCLE_MUSIC);
    if (isMusicActivated)
    {
        w->pressed_widgets |= (1 << WIDX_PLAY_MUSIC);
        w->disabled_widgets &= ~(1 << WIDX_MUSIC);
        w->disabled_widgets &= ~(1 << WIDX_MUSIC_DROPDOWN);
    }
    else
    {
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
static void window_ride_music_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);
}

#pragma endregion

#pragma region Measurements

static rct_string_id get_rating_name(ride_rating rating)
{
    int32_t index = std::clamp<int32_t>(rating >> 8, 0, (int32_t)std::size(RatingNames) - 1);
    return RatingNames[index];
}

/**
 *
 *  rct2: 0x006D2804
  when al == 0*/
static void cancel_scenery_selection()
{
    gGamePaused &= ~GAME_PAUSED_SAVING_TRACK;
    gTrackDesignSaveMode = false;
    audio_unpause_sounds();

    rct_window* main_w = window_get_main();

    if (main_w)
    {
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
    if (gTrackDesignSaveMode)
    {
        cancel_scenery_selection();
    }

    while (tool_set(w, WIDX_BACKGROUND, TOOL_CROSSHAIR))
        ;

    gTrackDesignSaveRideIndex = (uint8_t)w->number;

    track_design_save_init();
    gGamePaused |= GAME_PAUSED_SAVING_TRACK;
    gTrackDesignSaveMode = true;

    audio_stop_all_music_and_sounds();

    rct_window* w_main = window_get_main();

    if (w_main)
    {
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
    if (gTrackDesignSaveMode)
    {
        cancel_scenery_selection();
    }
}

/**
 *
 *  rct2: 0x006AD4CD
 */
static void window_ride_measurements_design_save(rct_window* w)
{
    track_design_save((uint8_t)w->number);
}

/**
 *
 *  rct2: 0x006AD4DA
 */
static void window_ride_measurements_close(rct_window* w)
{
    window_ride_measurements_design_cancel();
}

/**
 *
 *  rct2: 0x006AD478
 */
static void window_ride_measurements_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_ride_measurements_resize(rct_window* w)
{
    window_set_resize(w, 316, 234, 316, 234);
}

/**
 *
 *  rct2: 0x006AD4AB
 */
static void window_ride_measurements_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
        return;

    Ride* ride = get_ride(w->number);

    gDropdownItemsFormat[0] = STR_SAVE_TRACK_DESIGN_ITEM;
    gDropdownItemsFormat[1] = STR_SAVE_TRACK_DESIGN_WITH_SCENERY_ITEM;

    window_dropdown_show_text(
        w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1, w->colours[1], DROPDOWN_FLAG_STAY_OPEN, 2);
    gDropdownDefaultIndex = 0;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
    {
        dropdown_set_disabled(1, true);
    }
    else if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK))
    {
        // Disable saving without scenery if we're a flat ride
        dropdown_set_disabled(0, true);
        gDropdownDefaultIndex = 1;
    }
}

/**
 *
 *  rct2: 0x006AD4B2
 */
static void window_ride_measurements_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_SAVE_TRACK_DESIGN)
        return;

    if (dropdownIndex == -1)
        dropdownIndex = gDropdownHighlightedIndex;

    if (dropdownIndex == 0)
        track_design_save((uint8_t)w->number);
    else
        setup_scenery_selection(w);
}

/**
 *
 *  rct2: 0x006AD5DD
 */
static void window_ride_measurements_update(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_7);
}

/**
 *
 *  rct2: 0x006D2AE7
 */
static void window_ride_measurements_tooldown(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    TileElement* tileElement;
    int16_t mapX, mapY;
    int32_t interactionType;

    _lastSceneryX = x;
    _lastSceneryY = y;
    _collectTrackDesignScenery = true; // Default to true in case user does not select anything valid

    get_map_coordinates_from_pos(x, y, 0xFCCF, &mapX, &mapY, &interactionType, &tileElement, nullptr);
    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_WALL:
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            _collectTrackDesignScenery = !track_design_save_contains_tile_element(tileElement);
            track_design_save_select_tile_element(interactionType, mapX, mapY, tileElement, _collectTrackDesignScenery);
            break;
    }
}

static void window_ride_measurements_tooldrag(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    if (x == _lastSceneryX && y == _lastSceneryY)
        return;
    _lastSceneryX = x;
    _lastSceneryY = y;

    TileElement* tileElement;
    int16_t mapX, mapY;
    int32_t interactionType;

    get_map_coordinates_from_pos(x, y, 0xFCCF, &mapX, &mapY, &interactionType, &tileElement, nullptr);
    switch (interactionType)
    {
        case VIEWPORT_INTERACTION_ITEM_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_LARGE_SCENERY:
        case VIEWPORT_INTERACTION_ITEM_WALL:
        case VIEWPORT_INTERACTION_ITEM_FOOTPATH:
            track_design_save_select_tile_element(interactionType, mapX, mapY, tileElement, _collectTrackDesignScenery);
            break;
    }
}

/**
 *
 *  rct2: 0x006AD4DA
 */
static void window_ride_measurements_toolabort(rct_window* w, rct_widgetindex widgetIndex)
{
    window_ride_measurements_design_cancel();
}

/**
 *
 *  rct2: 0x006ACDBC
 */
static void window_ride_measurements_invalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN_NOT_POSSIBLE;
    window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WWT_EMPTY;
    if (gTrackDesignSaveMode && gTrackDesignSaveRideIndex == w->number)
    {
        window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WWT_BUTTON;
        window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WWT_BUTTON;
        window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WWT_BUTTON;
        window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WWT_BUTTON;
    }
    else
    {
        window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].type = WWT_EMPTY;
        window_ride_measurements_widgets[WIDX_RESET_SELECTION].type = WWT_EMPTY;
        window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type = WWT_EMPTY;
        window_ride_measurements_widgets[WIDX_CANCEL_DESIGN].type = WWT_EMPTY;

        window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].type = WWT_FLATBTN;
        w->disabled_widgets |= (1 << WIDX_SAVE_TRACK_DESIGN);
        if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
        {
            if (ride->excitement != RIDE_RATING_UNDEFINED)
            {
                w->disabled_widgets &= ~(1 << WIDX_SAVE_TRACK_DESIGN);
                window_ride_measurements_widgets[WIDX_SAVE_TRACK_DESIGN].tooltip = STR_SAVE_TRACK_DESIGN;
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
static void window_ride_measurements_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int16_t holes, maxSpeed, averageSpeed, drops, highestDropHeight, inversions, time;
    int32_t maxPositiveVerticalGs, maxNegativeVerticalGs, maxLateralGs, totalAirTime, length;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    if (window_ride_measurements_widgets[WIDX_SAVE_DESIGN].type == WWT_BUTTON)
    {
        rct_widget* widget = &window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND];

        int32_t x = w->x + (widget->right - widget->left) / 2;
        int32_t y = w->y + widget->top + 40;
        gfx_draw_string_centred_wrapped(dpi, nullptr, x, y, w->width - 8, STR_CLICK_ITEMS_OF_SCENERY_TO_SELECT, COLOUR_BLACK);

        x = w->x + 4;
        y = w->y + window_ride_measurements_widgets[WIDX_SELECT_NEARBY_SCENERY].bottom + 17;
        gfx_fill_rect_inset(dpi, x, y, w->x + 312, y + 1, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);
    }
    else
    {
        Ride* ride = get_ride(w->number);
        int32_t x = w->x + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].left + 4;
        int32_t y = w->y + window_ride_measurements_widgets[WIDX_PAGE_BACKGROUND].top + 4;

        if (ride->lifecycle_flags & RIDE_LIFECYCLE_TESTED)
        {
            // Excitement
            rct_string_id ratingName = get_rating_name(ride->excitement);
            set_format_arg(0, uint32_t, ride->excitement);
            set_format_arg(4, rct_string_id, ratingName);
            rct_string_id stringId = ride->excitement == RIDE_RATING_UNDEFINED ? STR_EXCITEMENT_RATING_NOT_YET_AVAILABLE
                                                                               : STR_EXCITEMENT_RATING;
            gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Intensity
            ratingName = get_rating_name(ride->intensity);
            set_format_arg(0, uint32_t, ride->intensity);
            set_format_arg(4, rct_string_id, ratingName);

            stringId = STR_INTENSITY_RATING;
            if (ride->excitement == RIDE_RATING_UNDEFINED)
                stringId = STR_INTENSITY_RATING_NOT_YET_AVAILABLE;
            else if (ride->intensity >= RIDE_RATING(10, 00))
                stringId = STR_INTENSITY_RATING_RED;

            gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Nausea
            ratingName = get_rating_name(ride->nausea);
            set_format_arg(0, uint32_t, ride->nausea);
            set_format_arg(4, rct_string_id, ratingName);
            stringId = ride->excitement == RIDE_RATING_UNDEFINED ? STR_NAUSEA_RATING_NOT_YET_AVAILABLE : STR_NAUSEA_RATING;
            gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y);
            y += 2 * LIST_ROW_HEIGHT;

            // Horizontal rule
            gfx_fill_rect_inset(dpi, x, y - 6, x + 303, y - 5, w->colours[1], INSET_RECT_FLAG_BORDER_INSET);

            if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_NO_RAW_STATS))
            {
                if (ride->type == RIDE_TYPE_MINI_GOLF)
                {
                    // Holes
                    holes = ride->holes & 0x1F;
                    gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;
                }
                else
                {
                    // Max speed
                    maxSpeed = (ride->max_speed * 9) >> 18;
                    gfx_draw_string_left(dpi, STR_MAX_SPEED, &maxSpeed, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Average speed
                    averageSpeed = (ride->average_speed * 9) >> 18;
                    gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &averageSpeed, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Ride time
                    int32_t numTimes = 0;
                    for (int32_t i = 0; i < ride->num_stations; i++)
                    {
                        time = ride->stations[numTimes].SegmentTime;
                        if (time != 0)
                        {
                            set_format_arg(0 + (numTimes * 4), uint16_t, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR);
                            set_format_arg(2 + (numTimes * 4), uint16_t, time);
                            numTimes++;
                        }
                    }
                    if (numTimes == 0)
                    {
                        set_format_arg(0, rct_string_id, STR_RIDE_TIME_ENTRY);
                        set_format_arg(2, uint16_t, 0);
                        numTimes++;
                    }
                    else
                    {
                        // sadly, STR_RIDE_TIME_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                        // therefore we set the last entry to use the no-separator format now, post-format
                        set_format_arg(0 + ((numTimes - 1) * 4), uint16_t, STR_RIDE_TIME_ENTRY);
                    }
                    set_format_arg(0 + (numTimes * 4), uint16_t, 0);
                    set_format_arg(2 + (numTimes * 4), uint16_t, 0);
                    set_format_arg(4 + (numTimes * 4), uint16_t, 0);
                    set_format_arg(6 + (numTimes * 4), uint16_t, 0);
                    gfx_draw_string_left_clipped(dpi, STR_RIDE_TIME, gCommonFormatArgs, COLOUR_BLACK, x, y, 308);
                    y += LIST_ROW_HEIGHT;
                }

                // Ride length
                int32_t numLengths = 0;
                for (int32_t i = 0; i < ride->num_stations; i++)
                {
                    length = ride->stations[numLengths].SegmentLength;
                    if (length != 0)
                    {
                        length >>= 16;
                        set_format_arg(0 + (numLengths * 4), uint16_t, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR);
                        set_format_arg(2 + (numLengths * 4), uint16_t, (length & 0xFFFF));
                        numLengths++;
                    }
                }
                if (numLengths == 0)
                {
                    set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
                    set_format_arg(2, uint16_t, 0);
                    numLengths++;
                }
                else
                {
                    // sadly, STR_RIDE_LENGTH_ENTRY_WITH_SEPARATOR are defined with the separator AFTER an entry
                    // therefore we set the last entry to use the no-separator format now, post-format
                    set_format_arg(0 + ((numLengths - 1) * 4), rct_string_id, STR_RIDE_LENGTH_ENTRY);
                }
                set_format_arg(0 + (numLengths * 4), uint16_t, 0);
                set_format_arg(2 + (numLengths * 4), uint16_t, 0);
                set_format_arg(4 + (numLengths * 4), uint16_t, 0);
                set_format_arg(6 + (numLengths * 4), uint16_t, 0);
                gfx_draw_string_left_clipped(dpi, STR_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 308);
                y += LIST_ROW_HEIGHT;

                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
                {
                    // Max. positive vertical G's
                    maxPositiveVerticalGs = ride->max_positive_vertical_g;
                    stringId = maxPositiveVerticalGs >= RIDE_G_FORCES_RED_POS_VERTICAL ? STR_MAX_POSITIVE_VERTICAL_G_RED
                                                                                       : STR_MAX_POSITIVE_VERTICAL_G;
                    gfx_draw_string_left(dpi, stringId, &maxPositiveVerticalGs, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Max. negative vertical G's
                    maxNegativeVerticalGs = ride->max_negative_vertical_g;
                    stringId = maxNegativeVerticalGs <= RIDE_G_FORCES_RED_NEG_VERTICAL ? STR_MAX_NEGATIVE_VERTICAL_G_RED
                                                                                       : STR_MAX_NEGATIVE_VERTICAL_G;
                    gfx_draw_string_left(dpi, stringId, &maxNegativeVerticalGs, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Max lateral G's
                    maxLateralGs = ride->max_lateral_g;
                    stringId = maxLateralGs >= RIDE_G_FORCES_RED_LATERAL ? STR_MAX_LATERAL_G_RED : STR_MAX_LATERAL_G;
                    gfx_draw_string_left(dpi, stringId, &maxLateralGs, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Total 'air' time
                    totalAirTime = ride->total_air_time * 3;
                    gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &totalAirTime, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;
                }

                if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_DROPS))
                {
                    // Drops
                    drops = ride->drops & 0x3F;
                    gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;

                    // Highest drop height
                    highestDropHeight = (ride->highest_drop_height * 3) / 4;
                    gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &highestDropHeight, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;
                }

                if (ride->type != RIDE_TYPE_MINI_GOLF)
                {
                    // Inversions
                    inversions = ride->inversions & 0x1F;
                    if (inversions != 0)
                    {
                        gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
                        y += LIST_ROW_HEIGHT;
                    }
                }
            }
        }
        else
        {
            gfx_draw_string_left(dpi, STR_NO_TEST_RESULTS_YET, nullptr, COLOUR_BLACK, x, y);
        }
    }
}

#pragma endregion

#pragma region Graphs

enum
{
    GRAPH_VELOCITY,
    GRAPH_ALTITUDE,
    GRAPH_VERTICAL,
    GRAPH_LATERAL
};

/**
 *
 *  rct2: 0x006AE8A6
 */
static void window_ride_set_graph(rct_window* w, int32_t type)
{
    if ((w->list_information_type & 0xFF) == type)
    {
        w->list_information_type ^= 0x8000;
    }
    else
    {
        w->list_information_type &= 0xFF00;
        w->list_information_type |= type;
    }
    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006AE85D
 */
static void window_ride_graphs_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
static void window_ride_graphs_resize(rct_window* w)
{
    window_set_resize(w, 316, 180, 500, 450);
}

/**
 *
 *  rct2: 0x006AE878
 */
static void window_ride_graphs_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
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
static void window_ride_graphs_update(rct_window* w)
{
    rct_widget* widget;
    rct_ride_measurement* measurement;
    int32_t x;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_8);
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_GRAPH);

    widget = &window_ride_graphs_widgets[WIDX_GRAPH];
    x = w->scrolls[0].h_left;
    if (!(w->list_information_type & 0x8000))
    {
        measurement = ride_get_measurement(w->number, nullptr);
        x = measurement == nullptr ? 0 : measurement->current_item - (((widget->right - widget->left) / 4) * 3);
    }

    w->scrolls[0].h_left = std::clamp(x, 0, w->scrolls[0].h_right - ((widget->right - widget->left) - 2));
    widget_scroll_update_thumbs(w, WIDX_GRAPH);
}

/**
 *
 *  rct2: 0x006AEA75
 */
static void window_ride_graphs_scrollgetheight(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    rct_ride_measurement* measurement;

    window_event_invalidate_call(w);

    // Set minimum size
    *width = window_ride_graphs_widgets[WIDX_GRAPH].right - window_ride_graphs_widgets[WIDX_GRAPH].left - 2;

    // Get measurement size
    measurement = ride_get_measurement(w->number, nullptr);
    if (measurement != nullptr)
        *width = std::max<int32_t>(*width, measurement->num_items);
}

/**
 *
 *  rct2: 0x006AE953
 */
static void window_ride_graphs_15(rct_window* w, int32_t scrollIndex, int32_t scrollAreaType)
{
    w->list_information_type |= 0x8000;
}

/**
 *
 *  rct2: 0x006AEA05
 */
static void window_ride_graphs_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id* stringId)
{
    if (widgetIndex == WIDX_GRAPH)
    {
        rct_string_id message;
        rct_ride_measurement* measurement = ride_get_measurement(w->number, &message);
        if (measurement != nullptr && (measurement->flags & RIDE_MEASUREMENT_FLAG_RUNNING))
        {
            set_format_arg(4, uint16_t, measurement->vehicle_index + 1);
            Ride* ride = get_ride(w->number);
            set_format_arg(2, rct_string_id, RideComponentNames[RideNameConvention[ride->type].vehicle].count);
        }
        else
        {
            *stringId = message;
        }
    }
    else
    {
        *stringId = STR_NONE;
    }
}

/**
 *
 *  rct2: 0x006AE372
 */
static void window_ride_graphs_invalidate(rct_window* w)
{
    rct_widget* widgets;
    Ride* ride;
    int32_t x, y;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    ride = get_ride(w->number);

    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    // Set pressed graph button type
    w->pressed_widgets &= ~(1 << WIDX_GRAPH_VELOCITY);
    w->pressed_widgets &= ~(1 << WIDX_GRAPH_ALTITUDE);
    w->pressed_widgets &= ~(1 << WIDX_GRAPH_VERTICAL);
    w->pressed_widgets &= ~(1 << WIDX_GRAPH_LATERAL);
    w->pressed_widgets |= (1LL << (WIDX_GRAPH_VELOCITY + (w->list_information_type & 0xFF)));

    // Hide graph buttons that are not applicable
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
    {
        window_ride_graphs_widgets[WIDX_GRAPH_VERTICAL].type = WWT_BUTTON;
        window_ride_graphs_widgets[WIDX_GRAPH_LATERAL].type = WWT_BUTTON;
    }
    else
    {
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
static void window_ride_graphs_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);
}

/**
 *
 *  rct2: 0x006AE4C7
 */
static void window_ride_graphs_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    gfx_clear(dpi, ColourMapA[COLOUR_SATURATED_GREEN].darker);

    rct_widget* widget = &window_ride_graphs_widgets[WIDX_GRAPH];
    rct_string_id stringId;
    rct_ride_measurement* measurement = ride_get_measurement(w->number, &stringId);
    if (measurement == nullptr)
    {
        // No measurement message
        int32_t x = (widget->right - widget->left) / 2;
        int32_t y = (widget->bottom - widget->top) / 2 - 5;
        int32_t width = widget->right - widget->left - 2;
        gfx_draw_string_centred_wrapped(dpi, gCommonFormatArgs, x, y, width, stringId, COLOUR_BLACK);
        return;
    }

    // Vertical grid lines
    const uint8_t lightColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_light;
    const uint8_t darkColour = ColourMapA[COLOUR_SATURATED_GREEN].mid_dark;

    int32_t time = 0;
    for (int32_t x = 0; x < dpi->x + dpi->width; x += 80)
    {
        if (x + 80 >= dpi->x)
        {
            gfx_fill_rect(dpi, x + 0, dpi->y, x + 0, dpi->y + dpi->height - 1, lightColour);
            gfx_fill_rect(dpi, x + 16, dpi->y, x + 16, dpi->y + dpi->height - 1, darkColour);
            gfx_fill_rect(dpi, x + 32, dpi->y, x + 32, dpi->y + dpi->height - 1, darkColour);
            gfx_fill_rect(dpi, x + 48, dpi->y, x + 48, dpi->y + dpi->height - 1, darkColour);
            gfx_fill_rect(dpi, x + 64, dpi->y, x + 64, dpi->y + dpi->height - 1, darkColour);
        }
        time += 5;
    }

    // Horizontal grid lines
    int32_t listType = w->list_information_type & 0xFF;
    int16_t yUnit = window_graphs_y_axi[listType].unit;
    rct_string_id stringID = window_graphs_y_axi[listType].label;
    int16_t yUnitInterval = window_graphs_y_axi[listType].unit_interval;
    int16_t yInterval = window_graphs_y_axi[listType].interval;

    // Scale modifier
    if (listType == GRAPH_ALTITUDE)
    {
        yUnit -= gMapBaseZ * 3;
    }

    for (int32_t y = widget->bottom - widget->top - 13; y >= 8; y -= yInterval, yUnit += yUnitInterval)
    {
        // Minor / major line
        int32_t colour = yUnit == 0 ? lightColour : darkColour;
        gfx_fill_rect(dpi, dpi->x, y, dpi->x + dpi->width - 1, y, colour);

        int16_t scaled_yUnit = yUnit;
        // Scale modifier
        if (listType == GRAPH_ALTITUDE)
            scaled_yUnit /= 2;

        gfx_draw_string_left(dpi, stringID, &scaled_yUnit, COLOUR_BLACK, w->scrolls[0].h_left + 1, y - 4);
    }

    // Time marks
    time = 0;
    for (int32_t x = 0; x < dpi->x + dpi->width; x += 80)
    {
        if (x + 80 >= dpi->x)
            gfx_draw_string_left(dpi, STR_RIDE_STATS_TIME, &time, COLOUR_BLACK, x + 2, 1);
        time += 5;
    }

    // Plot
    int32_t x = dpi->x;
    int32_t top, bottom;
    // Uses the force limits (used to draw extreme G's in red on measurement tab) to determine if line should be drawn red.
    int32_t intensityThresholdPositive = 0;
    int32_t intensityThresholdNegative = 0;
    for (int32_t width = 0; width < dpi->width; width++, x++)
    {
        if (x < 0 || x >= measurement->num_items - 1)
            continue;

        switch (listType)
        {
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
                intensityThresholdPositive = (RIDE_G_FORCES_RED_POS_VERTICAL / 8) + 39;
                intensityThresholdNegative = (RIDE_G_FORCES_RED_NEG_VERTICAL / 8) + 39;
                break;
            case GRAPH_LATERAL:
                top = measurement->lateral[x] + 52;
                bottom = measurement->lateral[x + 1] + 52;
                intensityThresholdPositive = (RIDE_G_FORCES_RED_LATERAL / 8) + 52;
                intensityThresholdNegative = -(RIDE_G_FORCES_RED_LATERAL / 8) + 52;
                break;
            default:
                log_error("Wrong graph type %d", listType);
                top = bottom = 0;
                break;
        }

        // Adjust line to match graph widget position.
        top = widget->bottom - widget->top - top - 13;
        bottom = widget->bottom - widget->top - bottom - 13;
        if (top > bottom)
        {
            std::swap(top, bottom);
        }

        // Adjust threshold line position as well
        if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
        {
            intensityThresholdPositive = widget->bottom - widget->top - intensityThresholdPositive - 13;
            intensityThresholdNegative = widget->bottom - widget->top - intensityThresholdNegative - 13;
        }

        const bool previousMeasurement = x > measurement->current_item;

        // Draw the current line in gray.
        gfx_fill_rect(dpi, x, top, x, bottom, previousMeasurement ? PALETTE_INDEX_17 : PALETTE_INDEX_21);

        // Draw red over extreme values (if supported by graph type).
        if (listType == GRAPH_VERTICAL || listType == GRAPH_LATERAL)
        {
            const auto redLineColour = previousMeasurement ? PALETTE_INDEX_171 : PALETTE_INDEX_173;

            // Line exceeds negative threshold (at bottom of graph).
            if (bottom >= intensityThresholdNegative)
            {
                const auto redLineTop = std::max(top, intensityThresholdNegative);
                const auto redLineBottom = std::max(bottom, intensityThresholdNegative);
                gfx_fill_rect(dpi, x, redLineTop, x, redLineBottom, redLineColour);
            }

            // Line exceeds positive threshold (at top of graph).
            if (top <= intensityThresholdPositive)
            {
                const auto redLineTop = std::min(top, intensityThresholdPositive);
                const auto redLineBottom = std::min(bottom, intensityThresholdPositive);
                gfx_fill_rect(dpi, x, redLineTop, x, redLineBottom, redLineColour);
            }
        }
    }
}

#pragma endregion

#pragma region Income

static utf8 _moneyInputText[MONEY_STRING_MAXLENGTH];

static void update_same_price_throughout_flags(uint32_t shop_item)
{
    uint32_t newFlags;

    if (shop_item_is_photo(shop_item))
    {
        newFlags = gSamePriceThroughoutParkA;
        newFlags ^= (1 << SHOP_ITEM_PHOTO);
        game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);

        newFlags = gSamePriceThroughoutParkB;
        newFlags ^= (1 << (SHOP_ITEM_PHOTO2 - 32)) | (1 << (SHOP_ITEM_PHOTO3 - 32)) | (1 << (SHOP_ITEM_PHOTO4 - 32));
        game_do_command(0, 1, 0, (0x3 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
    }
    else
    {
        if (shop_item < 32)
        {
            newFlags = gSamePriceThroughoutParkA;
            newFlags ^= (1u << shop_item);
            game_do_command(0, 1, 0, (0x2 << 8), GAME_COMMAND_SET_PARK_OPEN, newFlags, shop_item);
        }
        else
        {
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
static void window_ride_income_toggle_primary_price(rct_window* w)
{
    Ride* ride;
    rct_ride_entry* rideEntry;
    uint32_t shop_item;
    money16 price;

    ride = get_ride(w->number);
    rideEntry = get_ride_entry(ride->subtype);

    if (ride->type == RIDE_TYPE_TOILETS)
    {
        shop_item = SHOP_ITEM_ADMISSION;
    }
    else
    {
        shop_item = rideEntry->shop_item;
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
static void window_ride_income_toggle_secondary_price(rct_window* w)
{
    Ride* ride;
    rct_ride_entry* rideEntry;
    uint32_t shop_item;
    money16 price;

    ride = get_ride(w->number);
    rideEntry = get_ride_entry(ride->subtype);

    shop_item = rideEntry->shop_item_secondary;
    if (shop_item == SHOP_ITEM_NONE)
        shop_item = RidePhotoItems[ride->type];

    update_same_price_throughout_flags(shop_item);

    price = ride->price_secondary;
    game_do_command(0, 1, 0, (1 << 8) | w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

static void window_ride_income_set_primary_price(rct_window* w, money16 price)
{
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, w->number, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

/**
 *
 *  rct2: 0x006AE1E4
 */
static void window_ride_income_increase_primary_price(rct_window* w)
{
    if (!window_ride_income_can_modify_primary_price(w))
        return;

    Ride* ride = get_ride(w->number);
    money16 price = ride->price;
    if (price < MONEY(20, 00))
        price++;

    window_ride_income_set_primary_price(w, price);
}

/**
 *
 *  rct2: 0x006AE237
 */
static void window_ride_income_decrease_primary_price(rct_window* w)
{
    if (!window_ride_income_can_modify_primary_price(w))
        return;

    Ride* ride = get_ride(w->number);
    money16 price = ride->price;
    if (price > MONEY(0, 00))
        price--;

    window_ride_income_set_primary_price(w, price);
}

static money16 window_ride_income_get_secondary_price(rct_window* w)
{
    Ride* ride = get_ride(w->number);
    money16 price = ride->price_secondary;
    return price;
}

static void window_ride_income_set_secondary_price(rct_window* w, money16 price)
{
    game_do_command(0, GAME_COMMAND_FLAG_APPLY, 0, (w->number & 0x00FF) | 0x0100, GAME_COMMAND_SET_RIDE_PRICE, price, 0);
}

static bool window_ride_income_can_modify_primary_price(rct_window* w)
{
    Ride* ride = get_ride(w->number);
    rct_ride_entry* rideEntry = get_ride_entry(ride->subtype);

    return park_ride_prices_unlocked() || ride->type == RIDE_TYPE_TOILETS || rideEntry->shop_item != SHOP_ITEM_NONE;
}

/**
 *
 *  rct2: 0x006AE269
 */
static void window_ride_income_increase_secondary_price(rct_window* w)
{
    money16 price = window_ride_income_get_secondary_price(w);

    if (price < MONEY(20, 00))
        price++;

    window_ride_income_set_secondary_price(w, price);
}

/**
 *
 *  rct2: 0x006AE28D
 */
static void window_ride_income_decrease_secondary_price(rct_window* w)
{
    money16 price = window_ride_income_get_secondary_price(w);

    if (price > MONEY(0, 00))
        price--;

    window_ride_income_set_secondary_price(w, price);
}

/**
 *
 *  rct2: 0x006ADEA9
 */
static void window_ride_income_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
        case WIDX_PRIMARY_PRICE:
        {
            if (!window_ride_income_can_modify_primary_price(w))
                return;

            Ride* ride = get_ride(w->number);

            money_to_string((money32)ride->price, _moneyInputText, MONEY_STRING_MAXLENGTH, true);
            window_text_input_raw_open(
                w, WIDX_PRIMARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, _moneyInputText, MONEY_STRING_MAXLENGTH);
            break;
        }
        case WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK:
            window_ride_income_toggle_primary_price(w);
            break;
        case WIDX_SECONDARY_PRICE:
        {
            money32 price32 = (money32)window_ride_income_get_secondary_price(w);

            money_to_string(price32, _moneyInputText, MONEY_STRING_MAXLENGTH, true);
            window_text_input_raw_open(
                w, WIDX_SECONDARY_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, _moneyInputText, MONEY_STRING_MAXLENGTH);
        }
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
static void window_ride_income_resize(rct_window* w)
{
    window_set_resize(w, 316, 194, 316, 194);
}

/**
 *
 *  rct2: 0x006ADED4
 */
static void window_ride_income_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
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
static void window_ride_income_update(rct_window* w)
{
    Ride* ride;

    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_9);

    ride = get_ride(w->number);
    if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_INCOME)
    {
        ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_INCOME;
        window_invalidate(w);
    }
}

static void window_ride_income_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if ((widgetIndex != WIDX_PRIMARY_PRICE && widgetIndex != WIDX_SECONDARY_PRICE) || text == nullptr)
        return;

    money32 price = string_to_money(text);
    if (price == MONEY32_UNDEFINED)
    {
        return;
    }

    price = std::clamp(price, MONEY(0, 00), MONEY(20, 00));
    money16 price16 = (money16)price;

    if (widgetIndex == WIDX_PRIMARY_PRICE)
    {
        window_ride_income_set_primary_price(w, price16);
    }
    else
    {
        window_ride_income_set_secondary_price(w, price16);
    }
}

/**
 *
 *  rct2: 0x006ADAA3
 */
static void window_ride_income_invalidate(rct_window* w)
{
    rct_widget* widgets;
    rct_ride_entry* rideEntry;
    int32_t primaryItem, secondaryItem;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    rideEntry = get_ride_entry_by_ride(ride);

    // Primary item
    w->pressed_widgets &= ~(1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);
    w->disabled_widgets &= ~(1 << WIDX_PRIMARY_PRICE);

    window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].tooltip = STR_NONE;
    window_ride_income_widgets[WIDX_PRIMARY_PRICE].tooltip = STR_NONE;

    // If ride prices are locked, do not allow setting the price, unless we're dealing with a shop or toilet.
    if (!park_ride_prices_unlocked() && rideEntry->shop_item == SHOP_ITEM_NONE && ride->type != RIDE_TYPE_TOILETS)
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
    if (ride->type == RIDE_TYPE_TOILETS || ((primaryItem = rideEntry->shop_item) != SHOP_ITEM_NONE))
    {
        window_ride_income_widgets[WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_CHECKBOX;

        if (shop_item_has_common_price(primaryItem))
            w->pressed_widgets |= (1 << WIDX_PRIMARY_PRICE_SAME_THROUGHOUT_PARK);

        window_ride_income_widgets[WIDX_PRIMARY_PRICE_LABEL].text = ShopItemStringIds[primaryItem].price_label;
    }

    // Get secondary item
    secondaryItem = RidePhotoItems[ride->type];
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
    {
        if ((secondaryItem = rideEntry->shop_item_secondary) != SHOP_ITEM_NONE)
        {
            window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].text = ShopItemStringIds[secondaryItem].price_label;
        }
    }

    if (secondaryItem == SHOP_ITEM_NONE)
    {
        // Hide secondary item widgets
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_EMPTY;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_EMPTY;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_EMPTY;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_EMPTY;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK].type = WWT_EMPTY;
    }
    else
    {
        // Set same price throughout park checkbox
        w->pressed_widgets &= ~(1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);
        if (shop_item_has_common_price(secondaryItem))
            w->pressed_widgets |= (1 << WIDX_SECONDARY_PRICE_SAME_THROUGHOUT_PARK);

        // Show widgets
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_LABEL].type = WWT_LABEL;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE].type = WWT_SPINNER;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_INCREASE].type = WWT_BUTTON;
        window_ride_income_widgets[WIDX_SECONDARY_PRICE_DECREASE].type = WWT_BUTTON;
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
static void window_ride_income_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Ride* ride;
    rct_ride_entry* rideEntry;
    rct_string_id stringId;
    money32 profit, costPerHour;
    int32_t x, y, primaryItem, secondaryItem;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    ride = get_ride(w->number);
    rideEntry = get_ride_entry_by_ride(ride);

    x = w->x + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].left + 4;
    y = w->y + window_ride_income_widgets[WIDX_PAGE_BACKGROUND].top + 33;

    // Primary item profit / loss per item sold
    primaryItem = rideEntry->shop_item;
    if (primaryItem != SHOP_ITEM_NONE)
    {
        profit = ride->price;

        stringId = STR_PROFIT_PER_ITEM_SOLD;
        profit -= get_shop_item_cost(primaryItem);
        if (profit < 0)
        {
            profit *= -1;
            stringId = STR_LOSS_PER_ITEM_SOLD;
        }

        gfx_draw_string_left(dpi, stringId, &profit, COLOUR_BLACK, x, y);
    }
    y += 44;

    // Secondary item profit / loss per item sold
    secondaryItem = RidePhotoItems[ride->type];
    if (!(ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO))
        secondaryItem = rideEntry->shop_item_secondary;

    if (secondaryItem != SHOP_ITEM_NONE)
    {
        profit = ride->price_secondary;

        stringId = STR_PROFIT_PER_ITEM_SOLD;
        profit -= get_shop_item_cost(secondaryItem);
        if (profit < 0)
        {
            profit *= -1;
            stringId = STR_LOSS_PER_ITEM_SOLD;
        }

        gfx_draw_string_left(dpi, stringId, &profit, COLOUR_BLACK, x, y);
    }
    y += 18;

    // Income per hour
    if (ride->income_per_hour != MONEY32_UNDEFINED)
    {
        gfx_draw_string_left(dpi, STR_INCOME_PER_HOUR, &ride->income_per_hour, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    // Running cost per hour
    costPerHour = ride->upkeep_cost * 16;
    stringId = ride->upkeep_cost == MONEY16_UNDEFINED ? STR_RUNNING_COST_UNKNOWN : STR_RUNNING_COST_PER_HOUR;
    gfx_draw_string_left(dpi, stringId, &costPerHour, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    // Profit per hour
    if (ride->profit != MONEY32_UNDEFINED)
    {
        gfx_draw_string_left(dpi, STR_PROFIT_PER_HOUR, &ride->profit, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
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
static void window_ride_customer_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
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
        {
            auto intent = Intent(WC_GUEST_LIST);
            intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, GLFT_GUESTS_THINKING_ABOUT_RIDE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, w->number);
            context_open_intent(&intent);
            break;
        }
        case WIDX_SHOW_GUESTS_ON_RIDE:
        {
            auto intent = Intent(WC_GUEST_LIST);
            intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, GLFT_GUESTS_ON_RIDE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, w->number);
            context_open_intent(&intent);
            break;
        }
        case WIDX_SHOW_GUESTS_QUEUING:
        {
            auto intent = Intent(WC_GUEST_LIST);
            intent.putExtra(INTENT_EXTRA_GUEST_LIST_FILTER, GLFT_GUESTS_IN_QUEUE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, w->number);
            context_open_intent(&intent);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006ADA29
 */
static void window_ride_customer_resize(rct_window* w)
{
    w->flags |= WF_RESIZABLE;
    window_set_resize(w, 316, 163, 316, 163);
}

/**
 *
 *  rct2: 0x006AD9DD
 */
static void window_ride_customer_update(rct_window* w)
{
    Ride* ride;

    w->var_492++;
    if (w->var_492 >= 24)
        w->var_492 = 0;

    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_10);

    ride = get_ride(w->number);
    if (ride->window_invalidate_flags & RIDE_INVALIDATE_RIDE_CUSTOMER)
    {
        ride->window_invalidate_flags &= ~RIDE_INVALIDATE_RIDE_CUSTOMER;
        window_invalidate(w);
    }
}

/**
 *
 *  rct2: 0x006AD5F8
 */
static void window_ride_customer_invalidate(rct_window* w)
{
    rct_widget* widgets;

    widgets = window_ride_page_widgets[w->page];
    if (w->widgets != widgets)
    {
        w->widgets = widgets;
        window_init_scroll_widgets(w);
    }

    window_ride_set_pressed_tab(w);

    Ride* ride = get_ride(w->number);
    set_format_arg(0, rct_string_id, ride->name);
    set_format_arg(2, uint32_t, ride->name_arguments);

    window_ride_customer_widgets[WIDX_SHOW_GUESTS_THOUGHTS].type = WWT_FLATBTN;
    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
    {
        window_ride_customer_widgets[WIDX_SHOW_GUESTS_ON_RIDE].type = WWT_EMPTY;
        window_ride_customer_widgets[WIDX_SHOW_GUESTS_QUEUING].type = WWT_EMPTY;
    }
    else
    {
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
static void window_ride_customer_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    Ride* ride;
    int32_t x, y;
    uint8_t shopItem;
    int16_t popularity, satisfaction, queueTime, age;
    int32_t customersPerHour;
    rct_string_id stringId;

    window_draw_widgets(w, dpi);
    window_ride_draw_tab_images(dpi, w);

    ride = get_ride(w->number);
    x = w->x + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].left + 4;
    y = w->y + window_ride_customer_widgets[WIDX_PAGE_BACKGROUND].top + 4;

    // Customers currently on ride
    if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE)
    {
        int16_t customersOnRide = ride->num_riders;
        gfx_draw_string_left(dpi, STR_CUSTOMERS_ON_RIDE, &customersOnRide, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    // Customers per hour
    customersPerHour = ride_customers_per_hour(ride);
    gfx_draw_string_left(dpi, STR_CUSTOMERS_PER_HOUR, &customersPerHour, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    // Popularity
    popularity = ride->popularity;
    if (popularity == 255)
    {
        stringId = STR_POPULARITY_UNKNOWN;
    }
    else
    {
        stringId = STR_POPULARITY_PERCENT;
        popularity *= 4;
    }
    gfx_draw_string_left(dpi, stringId, &popularity, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    // Satisfaction
    satisfaction = ride->satisfaction;
    if (satisfaction == 255)
    {
        stringId = STR_SATISFACTION_UNKNOWN;
    }
    else
    {
        stringId = STR_SATISFACTION_PERCENT;
        satisfaction *= 5;
    }
    gfx_draw_string_left(dpi, stringId, &satisfaction, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    // Queue time
    if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE)
    {
        queueTime = ride_get_max_queue_time(ride);
        stringId = queueTime == 1 ? STR_QUEUE_TIME_MINUTE : STR_QUEUE_TIME_MINUTES;
        y += gfx_draw_string_left_wrapped(dpi, &queueTime, x, y, 308, stringId, COLOUR_BLACK);
        y += 5;
    }

    // Primary shop items sold
    shopItem = get_ride_entry_by_ride(ride)->shop_item;
    if (shopItem != SHOP_ITEM_NONE)
    {
        set_format_arg(0, rct_string_id, ShopItemStringIds[shopItem].plural);
        set_format_arg(2, uint32_t, ride->no_primary_items_sold);
        gfx_draw_string_left(dpi, STR_ITEMS_SOLD, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    // Secondary shop items sold / on-ride photos sold
    shopItem = (ride->lifecycle_flags & RIDE_LIFECYCLE_ON_RIDE_PHOTO) ? RidePhotoItems[ride->type]
                                                                      : get_ride_entry_by_ride(ride)->shop_item_secondary;
    if (shopItem != SHOP_ITEM_NONE)
    {
        set_format_arg(0, rct_string_id, ShopItemStringIds[shopItem].plural);
        set_format_arg(2, uint32_t, ride->no_secondary_items_sold);
        gfx_draw_string_left(dpi, STR_ITEMS_SOLD, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    // Total customers
    gfx_draw_string_left(dpi, STR_TOTAL_CUSTOMERS, &ride->total_customers, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    // Guests favourite
    if (gRideClassifications[ride->type] == RIDE_CLASS_RIDE)
    {
        stringId = ride->guests_favourite == 1 ? STR_FAVOURITE_RIDE_OF_GUEST : STR_FAVOURITE_RIDE_OF_GUESTS;
        gfx_draw_string_left(dpi, stringId, &ride->guests_favourite, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }
    y += 2;

    // Age
    // If the ride has a build date that is in the future, show it as built this year.
    age = std::max((gDateMonthsElapsed - ride->build_date) / 8, 0);
    stringId = age == 0 ? STR_BUILT_THIS_YEAR : age == 1 ? STR_BUILT_LAST_YEAR : STR_BUILT_YEARS_AGO;
    gfx_draw_string_left(dpi, stringId, &age, COLOUR_BLACK, x, y);
}

#pragma endregion
