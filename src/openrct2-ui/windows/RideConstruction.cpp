/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.hpp>
#include <openrct2/actions/TrackPlaceAction.hpp>
#include <openrct2/actions/TrackRemoveAction.hpp>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideGroupManager.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

#pragma region Widgets

// clang-format off
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

validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_CONSTRUCT);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ENTRANCE);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_EXIT);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ROTATE);

static rct_widget window_ride_construction_widgets[] = {
    { WWT_FRAME,            0,  0,      165,    0,      393,    0xFFFFFFFF,                                 STR_NONE                                                },
    { WWT_CAPTION,          0,  1,      164,    1,      14,     STR_RIDE_CONSTRUCTION_WINDOW_TITLE,         STR_WINDOW_TITLE_TIP                                    },
    { WWT_CLOSEBOX,         0,  153,    163,    2,      13,     STR_CLOSE_X,                                STR_CLOSE_WINDOW_TIP                                    },
    { WWT_GROUPBOX,         0,  3,      162,    17,     73,     STR_RIDE_CONSTRUCTION_DIRECTION,            STR_NONE                                                },
    { WWT_GROUPBOX,         0,  3,      162,    76,     116,    STR_RIDE_CONSTRUCTION_SLOPE,                STR_NONE                                                },
    { WWT_GROUPBOX,         0,  3,      162,    120,    160,    STR_RIDE_CONSTRUCTION_ROLL_BANKING,         STR_NONE                                                },
    { WWT_FLATBTN,          1,  6,      27,     29,     52,     SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL,     STR_RIDE_CONSTRUCTION_LEFT_CURVE_VERY_SMALL_TIP         },
    { WWT_FLATBTN,          1,  6,      27,     29,     52,     SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL,     STR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL_TIP              },
    { WWT_FLATBTN,          1,  28,     49,     29,     52,     SPR_RIDE_CONSTRUCTION_LEFT_CURVE,           STR_RIDE_CONSTRUCTION_LEFT_CURVE_TIP                    },
    { WWT_FLATBTN,          1,  72,     93,     29,     52,     SPR_RIDE_CONSTRUCTION_STRAIGHT,             STR_RIDE_CONSTRUCTION_STRAIGHT_TIP                      },
    { WWT_FLATBTN,          1,  116,    137,    29,     52,     SPR_RIDE_CONSTRUCTION_RIGHT_CURVE,          STR_RIDE_CONSTRUCTION_RIGHT_CURVE_TIP                   },
    { WWT_FLATBTN,          1,  138,    159,    29,     52,     SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL,    STR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL_TIP             },
    { WWT_FLATBTN,          1,  138,    159,    29,     52,     SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL,    STR_RIDE_CONSTRUCTION_RIGHT_CURVE_VERY_SMALL_TIP        },
    { WWT_BUTTON,           1,  17,     148,    55,     68,     STR_YELLOW_STRING,                          STR_RIDE_CONSTRUCTION_OTHER_TRACK_CONFIGURATIONS_TIP    },
    { WWT_FLATBTN,          1,  23,     46,     88,     111,    SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP,     STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP              },
    { WWT_FLATBTN,          1,  47,     70,     88,     111,    SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,           STR_RIDE_CONSTRUCTION_SLOPE_DOWN_TIP                    },
    { WWT_FLATBTN,          1,  71,     94,     88,     111,    SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL,          STR_RIDE_CONSTRUCTION_LEVEL_TIP                         },
    { WWT_FLATBTN,          1,  95,     118,    88,     111,    SPR_RIDE_CONSTRUCTION_SLOPE_UP,             STR_RIDE_CONSTRUCTION_SLOPE_UP_TIP                      },
    { WWT_FLATBTN,          1,  119,    142,    88,     111,    SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP,       STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP                },
    { WWT_FLATBTN,          1,  134,    157,    88,     111,    SPR_CHAIN_LIFT,                             STR_RIDE_CONSTRUCTION_CHAIN_LIFT_TIP                    },
    { WWT_FLATBTN,          1,  47,     70,     132,    155,    SPR_RIDE_CONSTRUCTION_LEFT_BANK,            STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP           },
    { WWT_FLATBTN,          1,  71,     94,     132,    155,    SPR_RIDE_CONSTRUCTION_NO_BANK,              STR_RIDE_CONSTRUCTION_NO_ROLL_TIP                       },
    { WWT_FLATBTN,          1,  95,     118,    132,    155,    SPR_RIDE_CONSTRUCTION_RIGHT_BANK,           STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP          },
    { WWT_IMGBTN,           1,  3,      162,    164,    333,    0xFFFFFFFF,                                 STR_RIDE_CONSTRUCTION_CONSTRUCT_SELECTED_SECTION_TIP    },
    { WWT_FLATBTN,          1,  60,     105,    338,    361,    SPR_DEMOLISH_CURRENT_SECTION,               STR_RIDE_CONSTRUCTION_REMOVE_HIGHLIGHTED_SECTION_TIP    },
    { WWT_FLATBTN,          1,  50,     71,     29,     52,     SPR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE,     STR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE_TIP              },
    { WWT_FLATBTN,          1,  20,     43,     338,    361,    SPR_PREVIOUS,                               STR_RIDE_CONSTRUCTION_MOVE_TO_PREVIOUS_SECTION_TIP      },
    { WWT_FLATBTN,          1,  122,    145,    338,    361,    SPR_NEXT,                                   STR_RIDE_CONSTRUCTION_MOVE_TO_NEXT_SECTION_TIP          },
    { WWT_GROUPBOX,         0,  3,      162,    362,    389,    0xFFFFFFFF,                                 STR_NONE                                                },
    { WWT_BUTTON,           1,  9,      78,     372,    383,    STR_RIDE_CONSTRUCTION_ENTRANCE,             STR_RIDE_CONSTRUCTION_ENTRANCE_TIP                      },
    { WWT_BUTTON,           1,  87,     156,    372,    383,    STR_RIDE_CONSTRUCTION_EXIT,                 STR_RIDE_CONSTRUCTION_EXIT_TIP                          },
    { WWT_FLATBTN,          1,  94,     115,    29,     52,     SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE,    STR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE_TIP             },
    { WWT_FLATBTN,          1,  72,     95,     338,    361,    SPR_ROTATE_ARROW,                           STR_ROTATE_90_TIP                                       },
    { WWT_FLATBTN,          1,  19,     42,     132,    155,    SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK,       STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP           },
    { WWT_FLATBTN,          1,  123,    146,    132,    155,    SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK,       STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP       },
    { WWT_GROUPBOX,         0,  96,     162,    120,    160,    STR_RIDE_CONSTRUCTION_SEAT_ROT,             STR_NONE                                                },
      SPINNER_WIDGETS      (1,  101,    158,    138,    149,    0,                                          STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP),
    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_ride_construction_close(rct_window *w);
static void window_ride_construction_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_ride_construction_resize(rct_window *w);
static void window_ride_construction_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_ride_construction_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_ride_construction_update(rct_window *w);
static void window_ride_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y);
static void window_ride_construction_invalidate(rct_window *w);
static void window_ride_construction_paint(rct_window *w, rct_drawpixelinfo *dpi);
static bool track_piece_direction_is_diagonal(const uint8_t direction);

//0x993EEC
static rct_window_event_list window_ride_construction_events = {
    window_ride_construction_close,
    window_ride_construction_mouseup,
    window_ride_construction_resize,
    window_ride_construction_mousedown,
    window_ride_construction_dropdown,
    nullptr,
    window_ride_construction_update,
    nullptr,
    nullptr,
    window_ride_construction_toolupdate,
    window_ride_construction_tooldown,
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
    window_ride_construction_invalidate,
    window_ride_construction_paint,
    nullptr
};

#pragma endregion

#pragma region RideConfigurationStringIds

// rct2: 0x00999492
static constexpr const rct_string_id RideConfigurationStringIds[] = {
    0,                                      // 0
    STR_STATION_PLATFORM,                   // 1
    0,                                      // 2
    0,                                      // 3
    0,                                      // 4
    0,                                      // 5
    0,                                      // 6
    0,                                      // 7
    0,                                      // 8
    0,                                      // 9
    0,                                      // 10
    0,                                      // 11
    0,                                      // 12
    0,                                      // 13
    0,                                      // 14
    0,                                      // 15
    0,                                      // 16
    0,                                      // 17
    0,                                      // 18
    0,                                      // 19
    0,                                      // 20
    0,                                      // 21
    0,                                      // 22
    0,                                      // 23
    0,                                      // 24
    0,                                      // 25
    0,                                      // 26
    0,                                      // 27
    0,                                      // 28
    0,                                      // 29
    0,                                      // 30
    0,                                      // 31
    0,                                      // 32
    0,                                      // 33
    0,                                      // 34
    0,                                      // 35
    0,                                      // 36
    0,                                      // 37
    STR_S_BEND_LEFT,                        // 38
    STR_S_BEND_RIGHT,                       // 39
    STR_VERTICAL_LOOP_LEFT,                 // 40
    STR_VERTICAL_LOOP_RIGHT,                // 41
    0,                                      // 42
    0,                                      // 43
    0,                                      // 44
    0,                                      // 45
    0,                                      // 46
    0,                                      // 47
    0,                                      // 48
    0,                                      // 49
    0,                                      // 50
    0,                                      // 51
    STR_IN_LINE_TWIST_LEFT,                 // 52
    STR_IN_LINE_TWIST_RIGHT,                // 53
    STR_IN_LINE_TWIST_LEFT,                 // 54
    STR_IN_LINE_TWIST_RIGHT,                // 55
    STR_HALF_LOOP,                          // 56
    STR_HALF_LOOP,                          // 57
    STR_HALF_CORKSCREW_LEFT,                // 58
    STR_HALF_CORKSCREW_RIGHT,               // 59
    STR_HALF_CORKSCREW_LEFT,                // 60
    STR_HALF_CORKSCREW_RIGHT,               // 61
    0,                                      // 62
    0,                                      // 63
    0,                                      // 64
    0,                                      // 65
    STR_ENTRY_EXIT_PLATFORM,                // 66
    STR_VERTICAL_TOWER,                     // 67
    0,                                      // 68
    0,                                      // 69
    0,                                      // 70
    0,                                      // 71
    0,                                      // 72
    0,                                      // 73
    0,                                      // 74
    0,                                      // 75
    0,                                      // 76
    0,                                      // 77
    0,                                      // 78
    0,                                      // 79
    0,                                      // 80
    0,                                      // 81
    0,                                      // 82
    STR_S_BEND_LEFT,                        // 83
    STR_S_BEND_RIGHT,                       // 84
    0,                                      // 85
    0,                                      // 86
    STR_HELIX_UP_SMALL,                     // 87
    STR_HELIX_UP_SMALL,                     // 88
    STR_HELIX_DOWN_SMALL,                   // 89
    STR_HELIX_DOWN_SMALL,                   // 90
    STR_HELIX_UP_LARGE,                     // 91
    STR_HELIX_UP_LARGE,                     // 92
    STR_HELIX_DOWN_LARGE,                   // 93
    STR_HELIX_DOWN_LARGE,                   // 94
    0,                                      // 95
    0,                                      // 96
    0,                                      // 97
    0,                                      // 98
    STR_BRAKES,                             // 99
    STR_SPINNING_CONTROL_TOGGLE_TRACK,      // 100
    0,                                      // 101
    STR_HELIX_UP_LARGE,                     // 102
    STR_HELIX_UP_LARGE,                     // 103
    STR_HELIX_DOWN_LARGE,                   // 104
    STR_HELIX_DOWN_LARGE,                   // 105
    STR_HELIX_UP_LEFT,                      // 106
    STR_HELIX_UP_RIGHT,                     // 107
    STR_HELIX_DOWN_LEFT,                    // 108
    STR_HELIX_DOWN_RIGHT,                   // 109
    STR_BASE_SIZE_2_X_2,                    // 110
    STR_BASE_SIZE_4_X_4,                    // 111
    STR_WATERFALLS,                         // 112
    STR_RAPIDS,                             // 113
    STR_ON_RIDE_PHOTO_SECTION,              // 114
    STR_BASE_SIZE_2_X_4,                    // 115
    STR_BASE_SIZE_5_X_1,                    // 116
    STR_WATER_SPLASH,                       // 117
    0,                                      // 118
    0,                                      // 119
    STR_WHIRLPOOL,                          // 120
    0,                                      // 121
    0,                                      // 122
    STR_CABLE_LIFT_HILL,                    // 123
    STR_SLOPE_UP_TO_VERTICAL,               // 124
    STR_VERTICAL_TRACK,                     // 125
    0,                                      // 126
    0,                                      // 127
    0,                                      // 128
    0,                                      // 129
    0,                                      // 130
    0,                                      // 131
    STR_HOLDING_BRAKE_FOR_DROP,             // 132
    0,                                      // 133
    0,                                      // 134
    0,                                      // 135
    0,                                      // 136
    0,                                      // 137
    0,                                      // 138
    0,                                      // 139
    0,                                      // 140
    0,                                      // 141
    0,                                      // 142
    0,                                      // 143
    0,                                      // 144
    0,                                      // 145
    0,                                      // 146
    0,                                      // 147
    0,                                      // 148
    0,                                      // 149
    0,                                      // 150
    0,                                      // 151
    0,                                      // 152
    0,                                      // 153
    0,                                      // 154
    0,                                      // 155
    0,                                      // 156
    0,                                      // 157
    0,                                      // 158
    0,                                      // 159
    0,                                      // 160
    0,                                      // 161
    0,                                      // 162
    0,                                      // 163
    0,                                      // 164
    0,                                      // 165
    0,                                      // 166
    0,                                      // 167
    0,                                      // 168
    0,                                      // 169
    0,                                      // 170
    0,                                      // 171
    STR_REVERSER_TURNTABLE,                 // 172
    STR_SPINNING_TUNNEL,                    // 173
    STR_BARREL_ROLL_LEFT,                   // 174
    STR_BARREL_ROLL_RIGHT,                  // 175
    STR_BARREL_ROLL_LEFT,                   // 176
    STR_BARREL_ROLL_RIGHT,                  // 177
    0,                                      // 178
    0,                                      // 179
    0,                                      // 180
    0,                                      // 181
    STR_LAUNCHED_LIFT_HILL,                 // 182
    STR_LARGE_HALF_LOOP_LEFT,               // 183
    STR_LARGE_HALF_LOOP_RIGHT,              // 184
    STR_LARGE_HALF_LOOP_LEFT,               // 185
    STR_LARGE_HALF_LOOP_RIGHT,              // 186
    STR_IN_LINE_TWIST_LEFT,                 // 187
    STR_IN_LINE_TWIST_RIGHT,                // 188
    STR_IN_LINE_TWIST_LEFT,                 // 189
    STR_IN_LINE_TWIST_RIGHT,                // 190
    STR_HALF_LOOP,                          // 191
    STR_HALF_LOOP,                          // 192
    STR_HALF_CORKSCREW_LEFT,                // 193
    STR_HALF_CORKSCREW_RIGHT,               // 194
    STR_HALF_CORKSCREW_LEFT,                // 195
    STR_HALF_CORKSCREW_RIGHT,               // 196
    STR_UPPER_TRANSFER,                     // 197
    STR_LOWER_TRANSFER,                     // 198
    STR_HEARTLINE_ROLL_LEFT,                // 199
    STR_HEARTLINE_ROLL_RIGHT,               // 200
    STR_GOLF_HOLE_A,                        // 201
    STR_GOLF_HOLE_B,                        // 202
    STR_GOLF_HOLE_C,                        // 203
    STR_GOLF_HOLE_D,                        // 204
    STR_GOLF_HOLE_E,                        // 205
    STR_QUARTER_LOOP,                       // 206
    STR_QUARTER_LOOP,                       // 207
    STR_QUARTER_LOOP,                       // 208
    STR_CURVED_LIFT_HILL_LEFT,              // 209
    STR_CURVED_LIFT_HILL_RIGHT,             // 210
    STR_REVERSER_LEFT,                      // 211
    STR_REVERSER_RIGHT,                     // 212
    STR_TOP_SECTION,                        // 213
    STR_VERTICAL_TRACK,                     // 214
    STR_SLOPE_TO_LEVEL,                     // 215
    STR_BLOCK_BRAKES,                       // 216
    0,                                      // 217
    0,                                      // 218
    0,                                      // 219
    0,                                      // 220
    0,                                      // 221
    0,                                      // 222
    0,                                      // 223
    0,                                      // 224
    0,                                      // 225
    0,                                      // 226
    0,                                      // 227
    0,                                      // 228
    0,                                      // 229
    0,                                      // 230
    0,                                      // 231
    0,                                      // 232
    0,                                      // 233
    0,                                      // 234
    0,                                      // 235
    0,                                      // 236
    0,                                      // 237
    0,                                      // 238
    0,                                      // 239
    0,                                      // 240
    0,                                      // 241
    0,                                      // 242
    0,                                      // 243
    0,                                      // 244
    0,                                      // 245
    0,                                      // 246
    0,                                      // 247
    0,                                      // 248
    0,                                      // 249
    0,                                      // 250
    0,                                      // 251
    0,                                      // 252
    STR_QUARTER_LOOP,                       // 253
    STR_QUARTER_LOOP,                       // 254
    STR_QUARTER_LOOP                        // 255
};
// clang-format on

#pragma endregion

static bool _trackPlaceCtrlState;
static int32_t _trackPlaceCtrlZ;
static bool _trackPlaceShiftState;
static int32_t _trackPlaceShiftStartScreenX;
static int32_t _trackPlaceShiftStartScreenY;
static int32_t _trackPlaceShiftZ;
static int32_t _trackPlaceZ;
static money32 _trackPlaceCost;
static bool _autoOpeningShop;
static bool _autoRotatingShop;
static uint8_t _currentlyShowingBrakeOrBoosterSpeed;
static bool _boosterTrackSelected;

static uint32_t _currentDisabledSpecialTrackPieces;

static void window_ride_construction_construct(rct_window* w);
static void window_ride_construction_mouseup_demolish(rct_window* w);
static void window_ride_construction_rotate(rct_window* w);
static void window_ride_construction_entrance_click(rct_window* w);
static void window_ride_construction_exit_click(rct_window* w);

static void window_ride_construction_draw_track_piece(
    rct_window* w, rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t unknown,
    int32_t width, int32_t height);
static void sub_6CBCE2(
    rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t edx, int32_t originX,
    int32_t originY, int32_t originZ);
static void window_ride_construction_update_map_selection();
static void window_ride_construction_update_possible_ride_configurations();
static void window_ride_construction_update_widgets(rct_window* w);
static void window_ride_construction_select_map_tiles(
    Ride* ride, int32_t trackType, int32_t trackDirection, int32_t x, int32_t y);
static void window_ride_construction_show_special_track_dropdown(rct_window* w, rct_widget* widget);
static void ride_selected_track_set_seat_rotation(int32_t seatRotation);
static void loc_6C7502(int32_t al);
static void ride_construction_set_brakes_speed(int32_t brakesSpeed);
static void ride_construction_tooldown_entrance_exit(int32_t screenX, int32_t screenY);

static uint8_t _currentPossibleRideConfigurations[32];

static constexpr const rct_string_id RideConstructionSeatAngleRotationStrings[] = {
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_180, STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_135,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_90,  STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_45,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_0,       STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_45,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_90,      STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_135,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_180,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_225,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_270,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_315,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_360,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_405,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_450,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_495,
};

static bool is_track_enabled(int32_t trackFlagIndex)
{
    return (_enabledRidePieces & (1ULL << trackFlagIndex)) != 0;
}

static int32_t ride_get_alternative_type(Ride* ride)
{
    return (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE) ? RideData4[ride->type].alternate_type : ride->type;
}

/* move to ride.c */
static void close_ride_window_for_construction(rct_windownumber number)
{
    rct_window* w = window_find_by_number(WC_RIDE, number);
    if (w != nullptr && w->page == 1)
        window_close(w);
}

/**
 *
 *  rct2: 0x006CB481
 */
rct_window* window_ride_construction_open()
{
    ride_id_t rideIndex = _currentRideIndex;
    close_ride_window_for_construction(rideIndex);

    rct_window* w;
    Ride* ride = get_ride(rideIndex);

    _stationConstructed = ride->num_stations != 0;
    _deferClose = false;

    if (ride->type == RIDE_TYPE_MAZE)
    {
        return context_open_window_view(WV_MAZE_CONSTRUCTION);
    }

    w = window_create(0, 29, 166, 394, &window_ride_construction_events, WC_RIDE_CONSTRUCTION, WF_NO_AUTO_CLOSE);

    w->widgets = window_ride_construction_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
        | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
        | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL) | (1ULL << WIDX_SPECIAL_TRACK_DROPDOWN) | (1ULL << WIDX_SLOPE_DOWN_STEEP)
        | (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP)
        | (1ULL << WIDX_CHAIN_LIFT) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT)
        | (1ULL << WIDX_CONSTRUCT) | (1ULL << WIDX_DEMOLISH) | (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_PREVIOUS_SECTION)
        | (1ULL << WIDX_NEXT_SECTION) | (1ULL << WIDX_ENTRANCE) | (1ULL << WIDX_EXIT) | (1ULL << WIDX_RIGHT_CURVE_LARGE)
        | (1ULL << WIDX_ROTATE) | (1ULL << WIDX_U_TRACK) | (1ULL << WIDX_O_TRACK)
        | (1ULL << WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP) | (1ULL << WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN);

    window_init_scroll_widgets(w);

    w->colours[0] = COLOUR_DARK_BROWN;
    w->colours[1] = COLOUR_DARK_BROWN;
    w->colours[2] = COLOUR_DARK_BROWN;

    w->number = rideIndex;

    window_push_others_right(w);
    show_gridlines();

    _currentTrackPrice = MONEY32_UNDEFINED;
    _currentBrakeSpeed2 = 8;
    _currentSeatRotationAngle = 4;

    _currentTrackCurve = RideConstructionDefaultTrackType[ride->type] | 0x100;
    _currentTrackSlopeEnd = 0;
    _currentTrackBankEnd = 0;
    _currentTrackLiftHill = 0;
    _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

    if (RideData4[ride->type].flags & RIDE_TYPE_FLAG4_START_CONSTRUCTION_INVERTED)
        _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

    _previousTrackBankEnd = 0;
    _previousTrackSlopeEnd = 0;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
    _currentTrackSelectionFlags = 0;
    _rideConstructionArrowPulseTime = 0;
    _autoOpeningShop = false;
    _autoRotatingShop = true;
    _trackPlaceCtrlState = false;
    _trackPlaceShiftState = false;
    return w;
}

/**
 *
 *  rct2: 0x006C845D
 */
static void window_ride_construction_close(rct_window* w)
{
    ride_construction_invalidate_current_track();
    viewport_set_visibility(0);

    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;

    // In order to cancel the yellow arrow correctly the
    // selection tool should be cancelled. Don't do a tool cancel if
    // another window has already taken control of tool.
    if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
        tool_cancel();

    hide_gridlines();

    // If we demolish a ride all windows will be closed including the construction window,
    // the ride at this point is already gone.
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr || ride->type == RIDE_TYPE_NULL)
    {
        return;
    }

    if (ride_try_get_origin_element(ride, nullptr))
    {
        // Auto open shops if required.
        if (ride->mode == RIDE_MODE_SHOP_STALL && gConfigGeneral.auto_open_shops)
        {
            // HACK: Until we find a good a way to defer the game command for opening the shop, stop this
            //       from getting stuck in an infinite loop as opening the ride will try to close this window
            if (!_autoOpeningShop)
            {
                _autoOpeningShop = true;
                ride_set_status(ride, RIDE_STATUS_OPEN);
                _autoOpeningShop = false;
            }
        }

        ride_set_to_default_inspection_interval(ride);
        auto intent = Intent(WC_RIDE);
        intent.putExtra(INTENT_EXTRA_RIDE_ID, ride->id);
        context_open_intent(&intent);
    }
    else
    {
        int32_t previousPauseState = gGamePaused;
        gGamePaused = 0;
        ride_action_modify(ride, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY);
        gGamePaused = previousPauseState;
    }
}

/**
 *
 *  rct2: 0x006C6E14
 */
static void window_ride_construction_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    window_ride_construction_update_enabled_track_pieces();
    switch (widgetIndex)
    {
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
            // Force any footpath construction to recheck the area.
            gFootpathProvisionalFlags |= PROVISIONAL_PATH_FLAG_2;
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
 *  rct2: 0x006C7934
 */
static void window_ride_construction_resize(rct_window* w)
{
    window_ride_construction_update_enabled_track_pieces();
    w->enabled_widgets &= ~(1 << WIDX_CONSTRUCT);
    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_PLACE)
    {
        w->enabled_widgets |= (1 << WIDX_CONSTRUCT);
    }

    Ride* ride = get_ride(_currentRideIndex);
    int32_t rideType = ride_get_alternative_type(ride);

    uint64_t disabledWidgets = 0;

    if (_currentTrackCurve & 0x100)
    {
        disabledWidgets |= (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_BANKING_GROUPBOX) | (1ULL << WIDX_SLOPE_DOWN_STEEP)
            | (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP)
            | (1ULL << WIDX_CHAIN_LIFT) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT);
    }

    // Disable large curves if the start or end of the track is sloped.
    if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE)
    {
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
    }
    if (is_track_enabled(TRACK_SLOPE_CURVE) && is_track_enabled(TRACK_CURVE_VERY_SMALL))
    {
        // Disable small curves if the start or end of the track is sloped.
        if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE)
                | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
        }
    }
    if (!is_track_enabled(TRACK_SLOPE_CURVE))
    {
        if (is_track_enabled(TRACK_CURVE_VERTICAL))
        {
            // Disable all curves only on vertical track
            if (_previousTrackSlopeEnd != TRACK_SLOPE_UP_90 || _currentTrackSlopeEnd != TRACK_SLOPE_UP_90)
            {
                if (_previousTrackSlopeEnd != TRACK_SLOPE_DOWN_90 || _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_90)
                {
                    disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
                        | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                        | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
                }
            }
        }
        else
        {
            // Disable all curves on sloped track
            if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
                    | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                    | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
            }
        }
    }
    if (!is_track_enabled(TRACK_FLAT_ROLL_BANKING))
    {
        // Disable banking
        disabledWidgets |= (1ULL << WIDX_BANKING_GROUPBOX) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT)
            | (1ULL << WIDX_BANK_RIGHT);
    }
    // Disable banking if the start track is steep and the end of the track becomes flat.
    if ((_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_60)
        && _currentTrackSlopeEnd == TRACK_SLOPE_NONE)
    {
        disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
    }
    if (!is_track_enabled(TRACK_SLOPE) && !is_track_enabled(TRACK_SLOPE_STEEP))
    {
        if (rideType != RIDE_TYPE_REVERSE_FREEFALL_COASTER && rideType != RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
        {
            // Disable all slopes
            disabledWidgets |= (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN)
                | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
        }
    }
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0)
    {
        disabledWidgets |= (1ULL << WIDX_CONSTRUCT) | (1ULL << WIDX_DEMOLISH) | (1ULL << WIDX_PREVIOUS_SECTION)
            | (1ULL << WIDX_NEXT_SECTION);
    }
    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_LEFT_VERY_SMALL:
        case TRACK_CURVE_LEFT_SMALL:
        case TRACK_CURVE_LEFT:
        case TRACK_CURVE_LEFT_LARGE:
            disabledWidgets |= (1ULL << WIDX_BANK_RIGHT);
            if (_previousTrackBankEnd == TRACK_BANK_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_BANK_LEFT);
            }
            else
            {
                disabledWidgets |= (1ULL << WIDX_BANK_STRAIGHT);
            }
            break;
        case TRACK_CURVE_RIGHT_LARGE:
        case TRACK_CURVE_RIGHT:
        case TRACK_CURVE_RIGHT_SMALL:
        case TRACK_CURVE_RIGHT_VERY_SMALL:
            disabledWidgets |= (1ULL << WIDX_BANK_LEFT);
            if (_previousTrackBankEnd == TRACK_BANK_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_BANK_RIGHT);
            }
            else
            {
                disabledWidgets |= (1ULL << WIDX_BANK_STRAIGHT);
            }
            break;
    }
    if (!is_track_enabled(TRACK_SLOPE_ROLL_BANKING))
    {
        if (_currentTrackBankEnd != TRACK_BANK_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_SLOPE_UP);
        }
    }
    if (_previousTrackSlopeEnd == _currentTrackSlopeEnd)
    {
        switch (_currentTrackSlopeEnd)
        {
            case TRACK_SLOPE_UP_60:
            case TRACK_SLOPE_DOWN_60:
                disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE)
                    | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
                if (!is_track_enabled(TRACK_SLOPE_CURVE_STEEP))
                {
                    disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_SMALL);
                }
                break;
            case TRACK_SLOPE_UP_90:
            case TRACK_SLOPE_DOWN_90:
                disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE)
                    | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
                if (!is_track_enabled(TRACK_CURVE_VERTICAL))
                {
                    disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_SMALL);
                }
                break;
        }
    }
    else
    {
        // Disable all curves
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE)
            | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
    }

    switch (_previousTrackSlopeEnd)
    {
        case TRACK_SLOPE_NONE:
            if (_currentTrackCurve != TRACK_CURVE_NONE
                || (is_track_enabled(TRACK_SLOPE_STEEP_LONG) && track_piece_direction_is_diagonal(_currentTrackPieceDirection)))
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            disabledWidgets |= (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            break;
        case TRACK_SLOPE_DOWN_60:
            disabledWidgets |= (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            if (!is_track_enabled(TRACK_SLOPE_LONG)
                && !(is_track_enabled(TRACK_SLOPE_STEEP_LONG)
                     && !track_piece_direction_is_diagonal(_currentTrackPieceDirection)))
            {
                disabledWidgets |= (1ULL << WIDX_LEVEL);
            }
            break;
        case TRACK_SLOPE_UP_25:
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN);
            break;
        case TRACK_SLOPE_UP_60:
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN);
            if (!is_track_enabled(TRACK_SLOPE_LONG)
                && !(is_track_enabled(TRACK_SLOPE_STEEP_LONG)
                     && !track_piece_direction_is_diagonal(_currentTrackPieceDirection)))
            {
                disabledWidgets |= (1ULL << WIDX_LEVEL);
            }
            break;
        case TRACK_SLOPE_DOWN_90:
        case TRACK_SLOPE_UP_90:
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP);
            break;
    }
    if (_previousTrackSlopeEnd == TRACK_SLOPE_NONE)
    {
        if (!is_track_enabled(TRACK_SLOPE_LONG) && !is_track_enabled(TRACK_SLOPE_STEEP_LONG))
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP);
        }
    }
    if (is_track_enabled(TRACK_SLOPE_VERTICAL))
    {
        if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 && _currentTrackPieceDirection < 4)
        {
            disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
        }
        if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_90)
        {
            disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
        }
        if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 && _currentTrackPieceDirection < 4)
        {
            disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
        }
    }
    if (_previousTrackBankEnd == TRACK_BANK_LEFT)
    {
        disabledWidgets |= (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_LARGE)
            | (1ULL << WIDX_BANK_RIGHT);
    }
    if (_previousTrackBankEnd == TRACK_BANK_RIGHT)
    {
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_LEFT_CURVE_LARGE)
            | (1ULL << WIDX_BANK_LEFT);
    }
    if (_currentTrackBankEnd != _previousTrackBankEnd)
    {
        disabledWidgets |= (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_LARGE)
            | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_LEFT_CURVE_LARGE);
    }
    if (_currentTrackSlopeEnd != TRACK_SLOPE_NONE)
    {
        if (is_track_enabled(TRACK_SLOPE_ROLL_BANKING))
        {
            if (_previousTrackSlopeEnd == TRACK_SLOPE_NONE)
            {
                if (_currentTrackSlopeEnd != TRACK_SLOPE_UP_25 && _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_25)
                {
                    disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
                }
            }
            else
            {
                if (_currentTrackSlopeEnd != _previousTrackSlopeEnd)
                {
                    disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
                }
                else
                {
                    if (_currentTrackSlopeEnd != TRACK_SLOPE_UP_25 && _currentTrackSlopeEnd != TRACK_SLOPE_DOWN_25)
                    {
                        disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
                    }
                }
            }
        }
        else
        {
            disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
        }
    }
    if (_currentTrackBankEnd != TRACK_BANK_NONE || _previousTrackBankEnd != TRACK_BANK_NONE)
    {
        disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP) | (1ULL << WIDX_CHAIN_LIFT);
    }
    if (_currentTrackCurve != TRACK_CURVE_NONE)
    {
        if (!is_track_enabled(TRACK_LIFT_HILL_CURVE))
        {
            disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
        }
        if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
        }
        if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_60)
        {
            disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
        }
        if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_60)
        {
            disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
        }
    }
    if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_90 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90)
    {
        disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
    }
    if (!is_track_enabled(TRACK_LIFT_HILL_STEEP))
    {
        if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 || _currentTrackSlopeEnd == TRACK_SLOPE_UP_60)
        {
            disabledWidgets |= (1ULL << WIDX_CHAIN_LIFT);
        }
    }
    if (_previousTrackBankEnd == TRACK_BANK_UPSIDE_DOWN)
    {
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_LEFT_CURVE_LARGE)
            | (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE)
            | (1ULL << WIDX_RIGHT_CURVE_LARGE);
    }
    if (_currentTrackCurve != TRACK_CURVE_NONE)
    {
        if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_SLOPE_UP);
        }
        if (_currentTrackSlopeEnd == _previousTrackSlopeEnd)
        {
            if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_25)
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
                if (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT
                    || _rideConstructionState != RIDE_CONSTRUCTION_STATE_BACK || !is_track_enabled(TRACK_SLOPE_CURVE_BANKED))
                {
                    disabledWidgets |= (1ULL << WIDX_LEVEL);
                }
            }
            if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_25)
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP);
                if (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT
                    || _rideConstructionState != RIDE_CONSTRUCTION_STATE_FRONT || !is_track_enabled(TRACK_SLOPE_CURVE_BANKED))
                {
                    disabledWidgets |= (1ULL << WIDX_LEVEL);
                }
            }
        }
        else if (is_track_enabled(TRACK_SLOPE_CURVE_BANKED))
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            if (_currentTrackBankEnd == TRACK_BANK_LEFT)
            {
                disabledWidgets |= (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT);
                disabledWidgets &= ~(1ULL << WIDX_BANK_LEFT);
            }
            if (_currentTrackBankEnd == TRACK_BANK_RIGHT)
            {
                disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT);
                disabledWidgets &= ~(1ULL << WIDX_BANK_RIGHT);
            }
            if (_currentTrackBankEnd == TRACK_BANK_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
                disabledWidgets &= ~(1ULL << WIDX_BANK_STRAIGHT);
            }
            if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_SLOPE_UP);
                disabledWidgets &= ~(1ULL << WIDX_LEVEL);
            }
            if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_25)
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL);
                disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP);
            }
            if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_25)
            {
                disabledWidgets |= (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP);
                disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN);
            }
            if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL)
            {
                disabledWidgets &= ~(1ULL << WIDX_LEFT_CURVE_SMALL);
            }
            if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
            {
                disabledWidgets &= ~(1ULL << WIDX_RIGHT_CURVE_SMALL);
            }
        }
    }
    if (_currentTrackCurve != TRACK_CURVE_NONE && _currentTrackSlopeEnd == TRACK_SLOPE_UP_60)
    {
        disabledWidgets |= (1ULL << WIDX_SLOPE_UP);
    }
    if (_currentTrackCurve != TRACK_CURVE_NONE && _currentTrackSlopeEnd == TRACK_SLOPE_DOWN_60)
    {
        disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN);
    }
    if ((_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED) && !gCheatsEnableChainLiftOnAllTrack)
    {
        if (_currentTrackSlopeEnd != TRACK_SLOPE_NONE && !is_track_enabled(TRACK_LIFT_HILL_CURVE))
        {
            disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_LEFT_CURVE_LARGE)
                | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
        }
        if (!is_track_enabled(TRACK_LIFT_HILL_STEEP))
        {
            if (w->widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
            }
        }
    }
    if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 && _currentTrackCurve != TRACK_CURVE_NONE)
    {
        disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_LEVEL);
    }
    if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 && _currentTrackCurve != TRACK_CURVE_NONE)
    {
        disabledWidgets |= (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP_STEEP);
    }
    if (_currentTrackSlopeEnd == TRACK_SLOPE_UP_90 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90)
    {
        if (_currentTrackCurve != TRACK_CURVE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_UP_STEEP);
        }
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
        if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
        {
            disabledWidgets |= (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE);
        }
    }
    else if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_90 || _previousTrackSlopeEnd == TRACK_SLOPE_DOWN_90)
    {
        if (_currentTrackCurve != TRACK_CURVE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP);
        }
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
        if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
        {
            disabledWidgets |= (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE);
        }
    }
    if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED))
    {
        if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_SLOPE_NONE)
        {
            if (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT)
            {
                if (_currentTrackSlopeEnd == _previousTrackSlopeEnd)
                {
                    disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
                    disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
                }
            }
        }
    }
    else if (
        (is_track_enabled(TRACK_HELIX_SMALL)
         || (is_track_enabled(TRACK_HELIX_LARGE) && _currentTrackCurve != TRACK_CURVE_LEFT_SMALL
             && _currentTrackCurve != TRACK_CURVE_RIGHT_SMALL))
        && (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT
            || _currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
        && (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd != TRACK_BANK_NONE))
    {
        if (_previousTrackSlopeEnd == _currentTrackSlopeEnd)
        {
            // Enable helix
            disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
            disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
        }
    }
    if (is_track_enabled(TRACK_SLOPE_CURVE_BANKED))
    {
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
        {
            if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
            {
                if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _previousTrackBankEnd != TRACK_BANK_NONE)
                {
                    disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP);
                }
            }
        }
        else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
        {
            if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
            {
                if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _previousTrackBankEnd != TRACK_BANK_NONE)
                {
                    disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN);
                }
            }
        }
    }
    if (_currentTrackPieceDirection >= 4)
    {
        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE)
            | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
    }
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
        disabledWidgets |= (1ULL << WIDX_NEXT_SECTION);
        if (window_ride_construction_update_state(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
        {
            disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
        }
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        disabledWidgets |= (1ULL << WIDX_PREVIOUS_SECTION);
        if (window_ride_construction_update_state(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
        {
            disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
        }
    }
    if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
    {
        disabledWidgets &= ~(1ULL << WIDX_BANKING_GROUPBOX);
    }
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT
        || _rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
    {
        disabledWidgets |= (1ULL << WIDX_DIRECTION_GROUPBOX) | (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_BANKING_GROUPBOX)
            | (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE)
            | (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
            | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL) | (1ULL << WIDX_SPECIAL_TRACK_DROPDOWN) | (1ULL << WIDX_SLOPE_DOWN_STEEP)
            | (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP)
            | (1ULL << WIDX_CHAIN_LIFT) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT)
            | (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
    }
    if (_currentlyShowingBrakeOrBoosterSpeed)
    {
        disabledWidgets &= ~(1ULL << WIDX_BANKING_GROUPBOX);
        disabledWidgets &= ~(1ULL << WIDX_BANK_LEFT);
        disabledWidgets &= ~(1ULL << WIDX_BANK_STRAIGHT);
        disabledWidgets &= ~(1ULL << WIDX_BANK_RIGHT);
    }

    // If chain lift cheat is enabled then show the chain lift widget no matter what
    if (gCheatsEnableChainLiftOnAllTrack)
    {
        disabledWidgets &= ~(1ULL << WIDX_CHAIN_LIFT);
    }

    // Set and invalidate the changed widgets
    uint64_t currentDisabledWidgets = w->disabled_widgets;
    if (currentDisabledWidgets == disabledWidgets)
        return;

    for (rct_widgetindex i = 0; i < 64; i++)
    {
        if ((disabledWidgets & (1ULL << i)) != (currentDisabledWidgets & (1ULL << i)))
        {
            widget_invalidate(w, i);
        }
    }
    w->disabled_widgets = disabledWidgets;
}

/**
 *
 *  rct2: 0x006C6E6A
 */
static void window_ride_construction_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    Ride* ride = get_ride(_currentRideIndex);

    window_ride_construction_update_enabled_track_pieces();
    switch (widgetIndex)
    {
        case WIDX_LEFT_CURVE:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_LEFT;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_RIGHT_CURVE:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_RIGHT;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_LEFT_CURVE_SMALL:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_LEFT_SMALL;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_RIGHT_CURVE_SMALL:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_RIGHT_SMALL;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_LEFT_CURVE_VERY_SMALL:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_LEFT_VERY_SMALL;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_RIGHT_CURVE_VERY_SMALL:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_RIGHT_VERY_SMALL;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_LEFT_CURVE_LARGE:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_LEFT_LARGE;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_RIGHT_CURVE_LARGE:
            ride_construction_invalidate_current_track();
            _currentTrackCurve = TRACK_CURVE_RIGHT_LARGE;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_STRAIGHT:
            ride_construction_invalidate_current_track();
            if (_currentTrackCurve != TRACK_CURVE_NONE)
                _currentTrackBankEnd = TRACK_BANK_NONE;
            _currentTrackCurve = TRACK_CURVE_NONE;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_SLOPE_DOWN_STEEP:
            ride_construction_invalidate_current_track();
            if (is_track_enabled(TRACK_HELIX_SMALL))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_LARGE | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_HALF_BANKED_HELIX_DOWN_SMALL | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_SMALL | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_DOWN | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_DOWN | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED))
            {
                if (_currentTrackBankEnd == TRACK_BANK_NONE)
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT)
                    {
                        _currentTrackCurve = TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_DOWN | 0x100;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    else if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                    {
                        _currentTrackCurve = TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN | 0x100;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
            }
            if (w->widgets[WIDX_SLOPE_DOWN_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP)
            {
                loc_6C7502(8);
            }
            else
            {
                loc_6C7502(10);
            }
            break;
        case WIDX_SLOPE_DOWN:
            ride_construction_invalidate_current_track();
            if (_rideConstructionState == 2 && _currentTrackBankEnd != TRACK_BANK_NONE)
            {
                _currentTrackBankEnd = TRACK_BANK_NONE;
            }
            loc_6C7502(6);
            break;
        case WIDX_LEVEL:
            ride_construction_invalidate_current_track();
            if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT && _previousTrackSlopeEnd == 6)
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL)
                {
                    _currentTrackBankEnd = TRACK_BANK_LEFT;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
                {
                    _currentTrackBankEnd = TRACK_BANK_RIGHT;
                }
            }
            else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK && _previousTrackSlopeEnd == 2)
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL)
                {
                    _currentTrackBankEnd = TRACK_BANK_LEFT;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
                {
                    _currentTrackBankEnd = TRACK_BANK_RIGHT;
                }
            }
            loc_6C7502(0);
            break;
        case WIDX_SLOPE_UP:
            ride_construction_invalidate_current_track();
            if (_rideConstructionState == 1 && _currentTrackBankEnd != TRACK_BANK_NONE)
            {
                _currentTrackBankEnd = TRACK_BANK_NONE;
            }
            if (ride->type == RIDE_TYPE_REVERSE_FREEFALL_COASTER || ride->type == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
            {
                if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT && _currentTrackCurve == TRACK_CURVE_NONE)
                {
                    _currentTrackCurve = TRACK_ELEM_REVERSE_FREEFALL_SLOPE | 0x100;
                    window_ride_construction_update_active_elements();
                }
            }
            else
            {
                loc_6C7502(2);
            }
            break;
        case WIDX_SLOPE_UP_STEEP:
            ride_construction_invalidate_current_track();
            if (is_track_enabled(TRACK_HELIX_SMALL))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_LARGE | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_LARGE | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_UP_SMALL | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TRACK_ELEM_RIGHT_QUARTER_BANKED_HELIX_LARGE_UP | 0x100;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED))
            {
                if (_currentTrackBankEnd == TRACK_BANK_NONE)
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT)
                    {
                        _currentTrackCurve = TRACK_ELEM_LEFT_QUARTER_HELIX_LARGE_UP | 0x100;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    else if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                    {
                        _currentTrackCurve = TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_UP | 0x100;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
            }
            if (w->widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
            {
                loc_6C7502(4);
            }
            else
            {
                loc_6C7502(18);
            }
            break;
        case WIDX_CHAIN_LIFT:
            ride_construction_invalidate_current_track();
            _currentTrackLiftHill ^= CONSTRUCTION_LIFT_HILL_SELECTED;
            if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
            {
                _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
            }
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_BANK_LEFT:
            ride_construction_invalidate_current_track();
            if (!_currentlyShowingBrakeOrBoosterSpeed)
            {
                _currentTrackBankEnd = TRACK_BANK_LEFT;
                _currentTrackPrice = MONEY32_UNDEFINED;
                window_ride_construction_update_active_elements();
            }
            break;
        case WIDX_BANK_STRAIGHT:
            ride_construction_invalidate_current_track();
            if (!_currentlyShowingBrakeOrBoosterSpeed)
            {
                _currentTrackBankEnd = TRACK_BANK_NONE;
                _currentTrackPrice = MONEY32_UNDEFINED;
                window_ride_construction_update_active_elements();
            }
            else
            {
                uint8_t* brakesSpeedPtr = &_currentBrakeSpeed2;
                uint8_t maxBrakesSpeed = 30;
                uint8_t brakesSpeed = *brakesSpeedPtr + 2;
                if (brakesSpeed <= maxBrakesSpeed)
                {
                    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
                    {
                        ride_construction_set_brakes_speed(brakesSpeed);
                    }
                    else
                    {
                        *brakesSpeedPtr = brakesSpeed;
                        window_ride_construction_update_active_elements();
                    }
                }
            }
            break;
        case WIDX_BANK_RIGHT:
            ride_construction_invalidate_current_track();
            if (!_currentlyShowingBrakeOrBoosterSpeed)
            {
                _currentTrackBankEnd = TRACK_BANK_RIGHT;
                _currentTrackPrice = MONEY32_UNDEFINED;
                window_ride_construction_update_active_elements();
            }
            else
            {
                uint8_t* brakesSpeedPtr = &_currentBrakeSpeed2;
                uint8_t brakesSpeed = *brakesSpeedPtr - 2;
                if (brakesSpeed >= 2)
                {
                    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
                    {
                        ride_construction_set_brakes_speed(brakesSpeed);
                    }
                    else
                    {
                        *brakesSpeedPtr = brakesSpeed;
                        window_ride_construction_update_active_elements();
                    }
                }
            }
            break;
        case WIDX_SPECIAL_TRACK_DROPDOWN:
            window_ride_construction_show_special_track_dropdown(w, widget);
            break;
        case WIDX_U_TRACK:
            ride_construction_invalidate_current_track();
            _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_O_TRACK:
            ride_construction_invalidate_current_track();
            _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
            _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
            _currentTrackPrice = MONEY32_UNDEFINED;
            window_ride_construction_update_active_elements();
            break;
        case WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP:
            if (_currentSeatRotationAngle < 15)
            {
                if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
                {
                    ride_selected_track_set_seat_rotation(_currentSeatRotationAngle + 1);
                }
                else
                {
                    _currentSeatRotationAngle++;
                    window_ride_construction_update_active_elements();
                }
            }
            break;
        case WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN:
            if (_currentSeatRotationAngle > 0)
            {
                if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
                {
                    ride_selected_track_set_seat_rotation(_currentSeatRotationAngle - 1);
                }
                else
                {
                    _currentSeatRotationAngle--;
                    window_ride_construction_update_active_elements();
                }
            }
            break;
    }
}

/**
 *
 *  rct2: 0x006C78CD
 */
static void window_ride_construction_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex != WIDX_SPECIAL_TRACK_DROPDOWN)
        return;
    if (dropdownIndex == -1)
        return;

    ride_construction_invalidate_current_track();
    _currentTrackPrice = MONEY32_UNDEFINED;
    int32_t trackPiece = _currentPossibleRideConfigurations[dropdownIndex];
    switch (trackPiece)
    {
        case TRACK_ELEM_END_STATION:
        case TRACK_ELEM_S_BEND_LEFT:
        case TRACK_ELEM_S_BEND_RIGHT:
            _currentTrackSlopeEnd = 0;
            break;
        case TRACK_ELEM_LEFT_VERTICAL_LOOP:
        case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
            _currentTrackBankEnd = TRACK_BANK_NONE;
            _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
            break;
    }
    _currentTrackCurve = trackPiece | 0x100;
    window_ride_construction_update_active_elements();
}
static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActionResult* result);
static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActionResult* result);
static void CloseConstructWindowOnCompletion(Ride* ride);

static void CloseConstructWindowOnCompletion(Ride* ride)
{
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0)
    {
        auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr)
        {
            if (ride_are_all_possible_entrances_and_exits_built(ride))
            {
                window_close(w);
            }
            else
            {
                window_event_mouse_up_call(w, WIDX_ENTRANCE);
            }
        }
    }
}

static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActionResult* result)
{
    if (result->Error != GA_ERROR::OK)
    {
        window_ride_construction_update_active_elements();
        return;
    }
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        int32_t trackDirection = _currentTrackPieceDirection;
        int32_t x = _currentTrackBegin.x;
        int32_t y = _currentTrackBegin.y;
        int32_t z = _currentTrackBegin.z;
        if (!(trackDirection & 4))
        {
            x -= CoordsDirectionDelta[trackDirection].x;
            y -= CoordsDirectionDelta[trackDirection].y;
        }

        CoordsXYE next_track;
        if (track_block_get_next_from_zero(x, y, z, ride, trackDirection, &next_track, &z, &trackDirection, false))
        {
            _currentTrackBegin.x = next_track.x;
            _currentTrackBegin.y = next_track.y;
            _currentTrackBegin.z = z;
            _currentTrackPieceDirection = next_track.element->GetDirection();
            _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
            ride_select_next_section();
        }
        else
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
        }

        window_ride_construction_do_station_check();
        window_ride_construction_do_entrance_exit_check();
        window_ride_construction_update_active_elements();
    }

    window_close_by_class(WC_ERROR);
    CloseConstructWindowOnCompletion(ride);
}

static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActionResult* result)
{
    if (result->Error != GA_ERROR::OK)
    {
        window_ride_construction_update_active_elements();
        return;
    }
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        auto trackDirection = direction_reverse(_currentTrackPieceDirection);
        auto x = _currentTrackBegin.x;
        auto y = _currentTrackBegin.y;
        auto z = _currentTrackBegin.z;
        if (!(trackDirection & 4))
        {
            x += CoordsDirectionDelta[trackDirection].x;
            y += CoordsDirectionDelta[trackDirection].y;
        }

        track_begin_end trackBeginEnd;
        if (track_block_get_previous_from_zero(x, y, z, ride, trackDirection, &trackBeginEnd))
        {
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
            ride_select_previous_section();
        }
        else
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
        }

        window_ride_construction_do_station_check();
        window_ride_construction_update_active_elements();
    }

    window_close_by_class(WC_ERROR);
    CloseConstructWindowOnCompletion(ride);
}
/**
 *
 *  rct2: 0x006C9F72
 */
static void window_ride_construction_construct(rct_window* w)
{
    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState, x, y, z, properties;

    _currentTrackPrice = MONEY32_UNDEFINED;
    _trackPlaceCost = MONEY32_UNDEFINED;
    ride_construction_invalidate_current_track();
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, &properties))
    {
        window_ride_construction_update_active_elements();
        return;
    }

    auto trackPlaceAction = TrackPlaceAction(
        rideIndex, trackType, { x, y, z, static_cast<uint8_t>(trackDirection) }, (properties)&0xFF, (properties >> 8) & 0x0F,
        (properties >> 12) & 0x0F, liftHillAndAlternativeState);

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        trackPlaceAction.SetCallback(RideConstructPlacedBackwardGameActionCallback);
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
        trackPlaceAction.SetCallback(RideConstructPlacedForwardGameActionCallback);
    }
    auto res = GameActions::Execute(&trackPlaceAction);
    // Used by some functions
    _trackPlaceCost = res->Error == GA_ERROR::OK ? res->Cost : MONEY32_UNDEFINED;

    if (res->Error != GA_ERROR::OK)
    {
        return;
    }
    audio_play_sound_at_location(SOUND_PLACE_ITEM, x, y, z);

    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED;
    }

    if (gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND)
    {
        viewport_set_visibility(1);
    }

    if ((_currentTrackCurve >= (TRACK_ELEM_LEFT_HALF_BANKED_HELIX_UP_SMALL | 0x100)
         && _currentTrackCurve <= (TRACK_ELEM_RIGHT_HALF_BANKED_HELIX_DOWN_LARGE | 0x100))
        || (_currentTrackCurve >= (TRACK_ELEM_LEFT_QUARTER_BANKED_HELIX_LARGE_UP | 0x100)
            && _currentTrackCurve <= (TRACK_ELEM_RIGHT_QUARTER_HELIX_LARGE_DOWN | 0x100))
        || (_currentTrackSlopeEnd != TRACK_SLOPE_NONE))
    {
        viewport_set_visibility(2);
    }
}

/**
 *
 *  rct2: 0x006C9BA5
 */
static void window_ride_construction_mouseup_demolish(rct_window* w)
{
    int32_t x, y, z, direction, type;
    TileElement* tileElement;
    CoordsXYE inputElement, outputElement;
    track_begin_end trackBeginEnd;
    // bool gotoStartPlacementMode;

    _currentTrackPrice = MONEY32_UNDEFINED;
    ride_construction_invalidate_current_track();

    // Select the track element that is to be deleted
    _rideConstructionState2 = RIDE_CONSTRUCTION_STATE_SELECTED;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
        if (!ride_select_backwards_from_front())
        {
            window_ride_construction_update_active_elements();
            return;
        }
        _rideConstructionState2 = RIDE_CONSTRUCTION_STATE_FRONT;
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        if (!ride_select_forwards_from_back())
        {
            window_ride_construction_update_active_elements();
            return;
        }
        _rideConstructionState2 = RIDE_CONSTRUCTION_STATE_BACK;
    }

    // Invalidate the selected track element or make sure it's at origin???
    x = _currentTrackBegin.x;
    y = _currentTrackBegin.y;
    z = _currentTrackBegin.z;
    direction = _currentTrackPieceDirection;
    type = _currentTrackPieceType;
    if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &tileElement, 0))
    {
        window_ride_construction_update_active_elements();
        return;
    }

    // Get the previous track element to go to after the selected track element is deleted
    inputElement.x = x;
    inputElement.y = y;
    inputElement.element = tileElement;
    if (track_block_get_previous(x, y, tileElement, &trackBeginEnd))
    {
        x = trackBeginEnd.begin_x;
        y = trackBeginEnd.begin_y;
        z = trackBeginEnd.begin_z;
        direction = trackBeginEnd.begin_direction;
        type = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        gGotoStartPlacementMode = false;
    }
    else if (track_block_get_next(&inputElement, &outputElement, &z, &direction))
    {
        x = outputElement.x;
        y = outputElement.y;
        direction = outputElement.element->GetDirection();
        type = outputElement.element->AsTrack()->GetTrackType();
        gGotoStartPlacementMode = false;
    }
    else
    {
        x = _currentTrackBegin.x;
        y = _currentTrackBegin.y;
        z = _currentTrackBegin.z;
        direction = _currentTrackPieceDirection;
        type = _currentTrackPieceType;

        if (sub_6C683D(&x, &y, &z, direction & 3, type, 0, &tileElement, 0))
        {
            window_ride_construction_update_active_elements();
            return;
        }

        const rct_preview_track* trackBlock = get_track_def_from_ride_index(
            _currentRideIndex, tileElement->AsTrack()->GetTrackType());
        z = (tileElement->base_height * 8) - trackBlock->z;
        gGotoStartPlacementMode = true;
    }

    auto trackRemoveAction = TrackRemoveAction(
        _currentTrackPieceType, 0,
        { _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z, _currentTrackPieceDirection });

    trackRemoveAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
        _stationConstructed = get_ride(w->number)->num_stations != 0;
        window_ride_construction_mouseup_demolish_next_piece(x, y, z, direction, type);
        if (result->Error != GA_ERROR::OK)
        {
            window_ride_construction_update_active_elements();
        }
    });

    GameActions::Execute(&trackRemoveAction);
}

/**
 *
 *  rct2: 0x006C78AA
 */
static void window_ride_construction_rotate(rct_window* w)
{
    _autoRotatingShop = false;
    _currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
    ride_construction_invalidate_current_track();
    _currentTrackPrice = MONEY32_UNDEFINED;
    window_ride_construction_update_active_elements();
}

/**
 *
 *  rct2: 0x006C7802
 */
static void window_ride_construction_entrance_click(rct_window* w)
{
    if (tool_set(w, WIDX_ENTRANCE, TOOL_CROSSHAIR))
    {
        auto ride = get_ride(_currentRideIndex);
        if (!ride_try_get_origin_element(ride, nullptr))
        {
            ride_initialise_construction_window(ride);
        }
    }
    else
    {
        gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_ENTRANCE;
        gRideEntranceExitPlaceRideIndex = w->number & 0xFF;
        gRideEntranceExitPlaceStationIndex = 0;
        input_set_flag(INPUT_FLAG_6, true);
        ride_construction_invalidate_current_track();
        if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
        }
        window_ride_construction_update_active_elements();
    }
}

/**
 *
 *  rct2: 0x006C7866
 */
static void window_ride_construction_exit_click(rct_window* w)
{
    if (tool_set(w, WIDX_EXIT, TOOL_CROSSHAIR))
    {
        auto ride = get_ride(_currentRideIndex);
        if (!ride_try_get_origin_element(ride, nullptr))
        {
            ride_initialise_construction_window(ride);
        }
    }
    else
    {
        gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_EXIT;
        gRideEntranceExitPlaceRideIndex = w->number & 0xFF;
        gRideEntranceExitPlaceStationIndex = 0;
        input_set_flag(INPUT_FLAG_6, true);
        ride_construction_invalidate_current_track();
        if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;
        }
        window_ride_construction_update_active_elements();
    }
}

/**
 *
 *  rct2: 0x006C8374
 */
static void window_ride_construction_update(rct_window* w)
{
    Ride* ride = get_ride(_currentRideIndex);

    // Close construction window if ride is not closed,
    // editing ride while open will cause many issues until properly handled
    if (ride->status != RIDE_STATUS_CLOSED)
    {
        window_close(w);
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_ELEM_SPINNING_TUNNEL | 0x100:
        case TRACK_ELEM_WHIRLPOOL | 0x100:
        case TRACK_ELEM_RAPIDS | 0x100:
        case TRACK_ELEM_WATERFALL | 0x100:
            widget_invalidate(w, WIDX_CONSTRUCT);
            break;
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE)
    {
        if (!widget_is_active_tool(w, WIDX_CONSTRUCT))
        {
            window_close(w);
            return;
        }
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
    {
        if (!widget_is_active_tool(w, WIDX_ENTRANCE) && !widget_is_active_tool(w, WIDX_EXIT))
        {
            _rideConstructionState = gRideEntranceExitPlacePreviousRideConstructionState;
            window_ride_construction_update_active_elements();
        }
    }

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_FRONT:
        case RIDE_CONSTRUCTION_STATE_BACK:
        case RIDE_CONSTRUCTION_STATE_SELECTED:
            if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE)) && gCurrentToolWidget.window_classification == WC_RIDE_CONSTRUCTION)
            {
                tool_cancel();
            }
            break;
    }

    sub_6C94D8();
}

/**
 *
 *  rct2: 0x006CC538
 */
static bool ride_get_place_position_from_screen_position(int32_t screenX, int32_t screenY, int32_t* outX, int32_t* outY)
{
    int16_t mapX, mapY, mapZ;
    int32_t interactionType, direction;
    TileElement* tileElement;
    rct_viewport* viewport;

    if (!_trackPlaceCtrlState)
    {
        if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_COPY_Z)
        {
            get_map_coordinates_from_pos(screenX, screenY, 0xFCCA, &mapX, &mapY, &interactionType, &tileElement, &viewport);
            if (interactionType != 0)
            {
                _trackPlaceCtrlZ = tileElement->base_height * 8;
                _trackPlaceCtrlState = true;
            }
        }
    }
    else
    {
        if (!(gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_COPY_Z))
        {
            _trackPlaceCtrlState = false;
        }
    }

    if (!_trackPlaceShiftState)
    {
        if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
        {
            _trackPlaceShiftState = true;
            _trackPlaceShiftStartScreenX = screenX;
            _trackPlaceShiftStartScreenY = screenY;
            _trackPlaceShiftZ = 0;
        }
    }
    else
    {
        if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
        {
            constexpr uint16_t maxHeight = (std::numeric_limits<decltype(TileElement::base_height)>::max() - 32)
                << MAX_ZOOM_LEVEL;

            _trackPlaceShiftZ = _trackPlaceShiftStartScreenY - screenY + 4;
            // Scale delta by zoom to match mouse position.
            auto* mainWnd = window_get_main();
            if (mainWnd && mainWnd->viewport)
            {
                _trackPlaceShiftZ <<= mainWnd->viewport->zoom;
            }
            _trackPlaceShiftZ = floor2(_trackPlaceShiftZ, 8);

            // Clamp to maximum possible value of base_height can offer.
            _trackPlaceShiftZ = std::min<int16_t>(_trackPlaceShiftZ, maxHeight);

            screenX = _trackPlaceShiftStartScreenX;
            screenY = _trackPlaceShiftStartScreenY;
        }
        else
        {
            _trackPlaceShiftState = false;
        }
    }

    if (!_trackPlaceCtrlState)
    {
        sub_68A15E(screenX, screenY, &mapX, &mapY, &direction, &tileElement);
        if (mapX == LOCATION_NULL)
            return false;

        _trackPlaceZ = 0;
        if (_trackPlaceShiftState)
        {
            tileElement = map_get_surface_element_at(mapX >> 5, mapY >> 5);
            mapZ = floor2(tileElement->base_height * 8, 16);
            mapZ += _trackPlaceShiftZ;
            mapZ = std::max<int16_t>(mapZ, 16);
            _trackPlaceZ = mapZ;
        }
    }
    else
    {
        mapZ = _trackPlaceCtrlZ;
        screen_get_map_xy_with_z(screenX, screenY, mapZ, &mapX, &mapY);
        if (_trackPlaceShiftState != 0)
        {
            mapZ += _trackPlaceShiftZ;
        }
        _trackPlaceZ = std::max<int32_t>(mapZ, 16);
    }

    if (mapX == LOCATION_NULL)
        return false;

    *outX = floor2(mapX, 32);
    *outY = floor2(mapY, 32);
    return true;
}

/**
 *
 *  rct2: 0x006C8229
 */
static void window_ride_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    switch (widgetIndex)
    {
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
static void window_ride_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, int32_t x, int32_t y)
{
    switch (widgetIndex)
    {
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
 *  rct2: 0x006C6AD5
 */
static void window_ride_construction_invalidate(rct_window* w)
{
    Ride* ride;
    rct_string_id stringId;

    ride = get_ride(_currentRideIndex);

    stringId = STR_RIDE_CONSTRUCTION_SPECIAL;
    if (_currentTrackCurve & 0x100)
    {
        stringId = RideConfigurationStringIds[_currentTrackCurve & 0xFF];
        if (stringId == STR_RAPIDS && ride->type == RIDE_TYPE_CAR_RIDE)
            stringId = STR_LOG_BUMPS;
        if (stringId == STR_SPINNING_CONTROL_TOGGLE_TRACK && ride->type != RIDE_TYPE_STEEL_WILD_MOUSE)
        {
            stringId = STR_BOOSTER;
        }
    }
    set_format_arg(0, uint16_t, stringId);

    if (_currentlyShowingBrakeOrBoosterSpeed)
    {
        uint16_t brakeSpeed2 = ((_currentBrakeSpeed2 * 9) >> 2) & 0xFFFF;
        if (_boosterTrackSelected)
        {
            brakeSpeed2 = get_booster_speed(ride->type, brakeSpeed2);
        }
        set_format_arg(2, uint16_t, brakeSpeed2);
    }

    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].text = RideConstructionSeatAngleRotationStrings
        [_currentSeatRotationAngle];

    // Set window title arguments
    set_format_arg(4, rct_string_id, ride->name);
    set_format_arg(6, uint32_t, ride->name_arguments);
}

/**
 *
 *  rct2: 0x006C6B86
 */
static void window_ride_construction_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_drawpixelinfo clipdpi;
    rct_widget* widget;
    int32_t x, y, width, height;

    window_draw_widgets(w, dpi);

    widget = &window_ride_construction_widgets[WIDX_CONSTRUCT];
    if (widget->type == WWT_EMPTY)
        return;

    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState;
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, nullptr, nullptr, nullptr, nullptr))
        return;

    // Draw track piece
    x = w->x + widget->left + 1;
    y = w->y + widget->top + 1;
    width = widget->right - widget->left - 1;
    height = widget->bottom - widget->top - 1;
    if (clip_drawpixelinfo(&clipdpi, dpi, x, y, width, height))
    {
        window_ride_construction_draw_track_piece(
            w, &clipdpi, rideIndex, trackType, trackDirection, liftHillAndAlternativeState, width, height);
    }

    // Draw cost
    x = w->x + (widget->left + widget->right) / 2;
    y = w->y + widget->bottom - 23;
    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_PLACE)
        gfx_draw_string_centred(dpi, STR_BUILD_THIS, x, y, COLOUR_BLACK, w);

    y += 11;
    if (_currentTrackPrice != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        gfx_draw_string_centred(dpi, STR_COST_LABEL, x, y, COLOUR_BLACK, (void*)&_currentTrackPrice);
    }
}

static void window_ride_construction_draw_track_piece(
    rct_window* w, rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t unknown,
    int32_t width, int32_t height)
{
    const rct_preview_track* trackBlock;
    Ride* ride;

    ride = get_ride(rideIndex);

    trackBlock = get_track_def_from_ride(ride, trackType);
    while ((trackBlock + 1)->index != 0xFF)
        trackBlock++;

    int16_t x = trackBlock->x;
    int16_t z = trackBlock->z;
    int16_t y = trackBlock->y;
    if (trackBlock->var_09 & 2)
    {
        x = 0;
        y = 0;
    }

    int16_t tmp;
    switch (trackDirection & 3)
    {
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
    // this is actually case 0, but the other cases all jump to it
    x = 4112 + (x / 2);
    y = 4112 + (y / 2);
    z = 1024 + z;

    int16_t previewZOffset = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE)
        ? FlatRideTrackDefinitions[trackType].preview_z_offset
        : TrackDefinitions[trackType].preview_z_offset;
    z -= previewZOffset;

    const LocationXY16 rotatedCoords = ride_get_rotated_coords(x, y, z);
    x = rotatedCoords.x;
    y = rotatedCoords.y;

    dpi->x += x - width / 2;
    dpi->y += y - height / 2 - 16;
    uint32_t d = unknown << 16;
    d |= rideIndex;
    d |= trackType << 8;

    sub_6CBCE2(dpi, rideIndex, trackType, trackDirection, d, 4096, 4096, 1024);
}

static TileElement _tempTrackTileElement;
static TileElement _tempSideTrackTileElement = { 0x80, 0x8F, 128, 128, 0, 0, 0, 0 };
static TileElement* _backupTileElementArrays[5];

/**
 *
 *  rct2: 0x006CBCE2
 * bh: trackDirection
 * dl: rideIndex
 * dh: trackType
 */
static void sub_6CBCE2(
    rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t edx, int32_t originX,
    int32_t originY, int32_t originZ)
{
    Ride* ride;
    const rct_preview_track* trackBlock;
    int32_t offsetX, offsetY;

    paint_session* session = paint_session_alloc(dpi, 0);
    trackDirection &= 3;

    ride = get_ride(rideIndex);

    int16_t preserveMapSizeUnits = gMapSizeUnits;
    int16_t preserveMapSizeMinus2 = gMapSizeMinus2;
    int16_t preserveMapSize = gMapSize;
    int16_t preserveMapSizeMaxXY = gMapSizeMaxXY;

    gMapSizeUnits = 255 * 32;
    gMapSizeMinus2 = (255 * 32) + 286;
    gMapSize = 256;
    gMapSizeMaxXY = (256 * 32) - 1;

    trackBlock = get_track_def_from_ride(ride, trackType);
    while (trackBlock->index != 255)
    {
        auto quarterTile = trackBlock->var_08.Rotate(trackDirection);
        switch (trackDirection)
        {
            default:
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
        int32_t x = originX + offsetX;
        int32_t y = originY + offsetY;
        int32_t baseZ = (originZ + trackBlock->z) >> 3;
        int32_t clearanceZ = ((trackBlock->var_07 + RideData5[ride->type].clearance_height) >> 3) + baseZ + 4;

        int32_t tileX = x >> 5;
        int32_t tileY = y >> 5;

        // Replace map elements with temporary ones containing track
        _backupTileElementArrays[0] = map_get_first_element_at(tileX + 0, tileY + 0);
        _backupTileElementArrays[1] = map_get_first_element_at(tileX + 1, tileY + 0);
        _backupTileElementArrays[2] = map_get_first_element_at(tileX - 1, tileY + 0);
        _backupTileElementArrays[3] = map_get_first_element_at(tileX + 0, tileY + 1);
        _backupTileElementArrays[4] = map_get_first_element_at(tileX + 0, tileY - 1);
        map_set_tile_elements(tileX + 0, tileY + 0, &_tempTrackTileElement);
        map_set_tile_elements(tileX + 1, tileY + 0, &_tempSideTrackTileElement);
        map_set_tile_elements(tileX - 1, tileY + 0, &_tempSideTrackTileElement);
        map_set_tile_elements(tileX + 0, tileY + 1, &_tempSideTrackTileElement);
        map_set_tile_elements(tileX + 0, tileY - 1, &_tempSideTrackTileElement);

        // Set the temporary track element
        _tempTrackTileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
        _tempTrackTileElement.SetDirection(trackDirection);
        _tempTrackTileElement.AsTrack()->SetHasChain((edx & 0x10000) ? true : false);
        _tempTrackTileElement.flags = quarterTile.GetBaseQuarterOccupied() | TILE_ELEMENT_FLAG_LAST_TILE;
        _tempTrackTileElement.base_height = baseZ;
        _tempTrackTileElement.clearance_height = clearanceZ;
        _tempTrackTileElement.AsTrack()->SetTrackType(trackType);
        _tempTrackTileElement.AsTrack()->SetSequenceIndex(trackBlock->index);
        _tempTrackTileElement.AsTrack()->SetHasCableLift(false);
        _tempTrackTileElement.AsTrack()->SetInverted((edx & 0x20000) ? true : false);
        _tempTrackTileElement.AsTrack()->SetColourScheme(RIDE_COLOUR_SCHEME_MAIN);
        // Skipping seat rotation, should not be necessary for a temporary piece.
        _tempTrackTileElement.AsTrack()->SetRideIndex(rideIndex);

        // Draw this map tile
        sub_68B2B7(session, x, y);

        // Restore map elements
        map_set_tile_elements(tileX + 0, tileY + 0, _backupTileElementArrays[0]);
        map_set_tile_elements(tileX + 1, tileY + 0, _backupTileElementArrays[1]);
        map_set_tile_elements(tileX - 1, tileY + 0, _backupTileElementArrays[2]);
        map_set_tile_elements(tileX + 0, tileY + 1, _backupTileElementArrays[3]);
        map_set_tile_elements(tileX + 0, tileY - 1, _backupTileElementArrays[4]);

        trackBlock++;
    }

    gMapSizeUnits = preserveMapSizeUnits;
    gMapSizeMinus2 = preserveMapSizeMinus2;
    gMapSize = preserveMapSize;
    gMapSizeMaxXY = preserveMapSizeMaxXY;

    paint_session_arrange(session);
    paint_draw_structs(session);
    paint_session_free(session);
}

/**
 *
 *  rct2: 0x006C84CE
 */
void window_ride_construction_update_active_elements_impl()
{
    rct_window* w;
    TileElement* tileElement;

    window_ride_construction_update_enabled_track_pieces();
    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr)
        return;

    window_ride_construction_update_map_selection();

    _selectedTrackType = 255;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
    {
        int32_t x = _currentTrackBegin.x;
        int32_t y = _currentTrackBegin.y;
        int32_t z = _currentTrackBegin.z;
        if (!sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, 0, &tileElement, 0))
        {
            _selectedTrackType = tileElement->AsTrack()->GetTrackType();
            if (track_element_has_speed_setting(tileElement->AsTrack()->GetTrackType()))
                _currentBrakeSpeed2 = tileElement->AsTrack()->GetBrakeBoosterSpeed();
            _currentSeatRotationAngle = tileElement->AsTrack()->GetSeatRotation();
        }
    }

    window_ride_construction_update_possible_ride_configurations();
    window_ride_construction_update_widgets(w);
}

/**
 *
 *  rct2: 0x006C6A77
 */
void window_ride_construction_update_enabled_track_pieces()
{
    Ride* ride = get_ride(_currentRideIndex);
    rct_ride_entry* rideEntry = get_ride_entry_by_ride(ride);
    int32_t rideType = (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE) ? RideData4[ride->type].alternate_type
                                                                                     : ride->type;

    if (rideEntry == nullptr)
        return;

    if (gCheatsEnableAllDrawableTrackPieces)
    {
        _enabledRidePieces = RideTypePossibleTrackConfigurations[rideType];
        return;
    }

    if (RideGroupManager::RideTypeHasRideGroups(rideType))
    {
        const RideGroup* rideGroup = RideGroupManager::GetRideGroup(rideType, rideEntry);
        _enabledRidePieces = rideGroup->AvailableTrackPieces;
    }
    else
    {
        _enabledRidePieces = RideTypePossibleTrackConfigurations[rideType];
    }
}

/**
 *
 *  rct2: 0x006C94D8
 */
void sub_6C94D8()
{
    ride_id_t rideIndex;
    int32_t x, y, z, direction, type, liftHillAndAlternativeState;

    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED)
    {
        return;
    }

    // Recheck if area is fine for new track.
    // Set by footpath placement
    if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_RECHECK)
    {
        ride_construction_invalidate_current_track();
        _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_RECHECK;
    }

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_FRONT:
        case RIDE_CONSTRUCTION_STATE_BACK:
            if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
            {
                if (window_ride_construction_update_state(
                        &type, &direction, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, nullptr))
                {
                    ride_construction_remove_ghosts();
                }
                else
                {
                    _currentTrackPrice = place_provisional_track_piece(
                        rideIndex, type, direction, liftHillAndAlternativeState, x, y, z);
                    window_ride_construction_update_active_elements();
                }
            }
            _rideConstructionArrowPulseTime--;
            if (_rideConstructionArrowPulseTime >= 0)
                break;

            _rideConstructionArrowPulseTime = 5;
            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            x = _currentTrackBegin.x;
            y = _currentTrackBegin.y;
            z = _currentTrackBegin.z;
            direction = _currentTrackPieceDirection;
            type = _currentTrackPieceType;
            gMapSelectArrowPosition.x = x;
            gMapSelectArrowPosition.y = y;
            gMapSelectArrowPosition.z = z;
            if (direction >= 4)
                direction += 4;
            if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
                direction = direction_reverse(direction);
            gMapSelectArrowDirection = direction;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
            map_invalidate_tile_full(x, y);
            break;
        case RIDE_CONSTRUCTION_STATE_SELECTED:
            _rideConstructionArrowPulseTime--;
            if (_rideConstructionArrowPulseTime >= 0)
                break;

            _rideConstructionArrowPulseTime = 5;
            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            x = _currentTrackBegin.x;
            y = _currentTrackBegin.y;
            z = _currentTrackBegin.z;
            direction = _currentTrackPieceDirection & 3;
            type = _currentTrackPieceType;
            if (sub_6C683D(
                    &x, &y, &z, direction, type, 0, nullptr, _currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW ? 2 : 1))
            {
                ride_construction_remove_ghosts();
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
            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            x = _currentTrackBegin.x & 0xFFE0;
            y = _currentTrackBegin.y & 0xFFE0;
            z = _currentTrackBegin.z + 15;
            gMapSelectArrowPosition.x = x;
            gMapSelectArrowPosition.y = y;
            gMapSelectArrowPosition.z = z;
            gMapSelectArrowDirection = 4;
            if (((_currentTrackBegin.x & 0x1F) | (_currentTrackBegin.y & 0x1F)) != 0)
            {
                gMapSelectArrowDirection = 6;
                if (((_currentTrackBegin.x & 0x1F) & (_currentTrackBegin.y & 0x1F)) == 0)
                {
                    gMapSelectArrowDirection = 5;
                    if ((_currentTrackBegin.y & 0x1F) == 0)
                        gMapSelectArrowDirection = 7;
                }
            }
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
            map_invalidate_tile_full(x, y);
            break;
    }
}

/**
 *
 *  rct2: 0x006C84E2
 */
static void window_ride_construction_update_map_selection()
{
    Ride* ride;
    int32_t trackType, trackDirection, x, y;

    map_invalidate_map_selection_tiles();
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_0:
            trackDirection = _currentTrackPieceDirection;
            trackType = 0;
            x = _currentTrackBegin.x;
            y = _currentTrackBegin.y;
            break;
        case RIDE_CONSTRUCTION_STATE_SELECTED:
            trackDirection = _currentTrackPieceDirection;
            trackType = _currentTrackPieceType;
            x = _currentTrackBegin.x;
            y = _currentTrackBegin.y;
            break;
        default:
            if (window_ride_construction_update_state(&trackType, &trackDirection, nullptr, nullptr, &x, &y, nullptr, nullptr))
            {
                trackDirection = _currentTrackPieceDirection;
                trackType = 0;
                x = _currentTrackBegin.x;
                y = _currentTrackBegin.y;
            }
            break;
    }

    ride = get_ride(_currentRideIndex);
    window_ride_construction_select_map_tiles(ride, trackType, trackDirection, x, y);
    map_invalidate_map_selection_tiles();
}

/**
 *
 *  rct2: 0x006C8648
 */
static void window_ride_construction_update_possible_ride_configurations()
{
    Ride* ride;
    int32_t trackType;
    int32_t edi;

    ride = get_ride(_currentRideIndex);

    _currentlyShowingBrakeOrBoosterSpeed = false;
    if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
        edi = RideData4[ride->type].alternate_type;
    else
        edi = ride->type;

    int32_t currentPossibleRideConfigurationIndex = 0;
    _numCurrentPossibleSpecialTrackPieces = 0;
    for (trackType = 0; trackType < 256; trackType++)
    {
        int32_t edx = ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ? FlatRideTrackDefinitions[trackType].type
                                                                               : TrackDefinitions[trackType].type;

        if (edx == 0)
            continue;

        if (edx & 0x80)
        {
            edx &= 0x7F;
            if (edx != edi)
                continue;
        }
        else if (!is_track_enabled(edx))
        {
            continue;
        }

        int32_t slope, bank;
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT || _rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE)
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                slope = FlatRideTrackDefinitions[trackType].vangle_start;
                bank = FlatRideTrackDefinitions[trackType].bank_start;
            }
            else
            {
                slope = TrackDefinitions[trackType].vangle_start;
                bank = TrackDefinitions[trackType].bank_start;
            }
        }
        else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
        {
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                slope = FlatRideTrackDefinitions[trackType].vangle_end;
                bank = FlatRideTrackDefinitions[trackType].bank_end;
            }
            else
            {
                slope = TrackDefinitions[trackType].vangle_end;
                bank = TrackDefinitions[trackType].bank_end;
            }
        }
        else
        {
            continue;
        }

        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE))
        {
            if (TrackDefinitions[trackType].type == TRACK_HELIX_SMALL || TrackDefinitions[trackType].type == TRACK_HELIX_LARGE)
            {
                if (bank != _previousTrackBankEnd)
                {
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
        _currentDisabledSpecialTrackPieces |= (1 << currentPossibleRideConfigurationIndex);
        if (_currentTrackPieceDirection < 4 && slope == _previousTrackSlopeEnd && bank == _previousTrackBankEnd
            && (trackType != TRACK_ELEM_TOWER_BASE || ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_29)))
        {
            _currentDisabledSpecialTrackPieces &= ~(1 << currentPossibleRideConfigurationIndex);
            _numCurrentPossibleSpecialTrackPieces++;
        }
        currentPossibleRideConfigurationIndex++;
    }
    _numCurrentPossibleRideConfigurations = currentPossibleRideConfigurationIndex;
}

/**
 *
 *  rct2: 0x006C87F5
 */
static void window_ride_construction_update_widgets(rct_window* w)
{
    ride_id_t rideIndex = _currentRideIndex;
    Ride* ride = get_ride(rideIndex);
    int32_t rideType = ride_get_alternative_type(ride);

    w->hold_down_widgets = 0;
    if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_IS_SHOP) || !_stationConstructed)
    {
        window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WWT_EMPTY;
        window_ride_construction_widgets[WIDX_ENTRANCE].type = WWT_EMPTY;
        window_ride_construction_widgets[WIDX_EXIT].type = WWT_EMPTY;
    }
    else
    {
        window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WWT_GROUPBOX;
        window_ride_construction_widgets[WIDX_ENTRANCE].type = WWT_BUTTON;
        window_ride_construction_widgets[WIDX_EXIT].type = WWT_BUTTON;
    }

    if (_numCurrentPossibleSpecialTrackPieces == 0)
    {
        window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WWT_EMPTY;
    }
    else
    {
        window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WWT_BUTTON;
    }

    if (is_track_enabled(TRACK_STRAIGHT))
    {
        window_ride_construction_widgets[WIDX_STRAIGHT].type = WWT_FLATBTN;
    }
    else
    {
        window_ride_construction_widgets[WIDX_STRAIGHT].type = WWT_EMPTY;
    }

    if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_HAS_LARGE_CURVES))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_LARGE].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_LARGE].type = WWT_FLATBTN;
    }
    else
    {
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
    window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE;
    if (is_track_enabled(TRACK_CURVE_VERTICAL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
    }
    if (is_track_enabled(TRACK_CURVE))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
    }
    if (is_track_enabled(TRACK_CURVE_SMALL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WWT_FLATBTN;
    }
    if (is_track_enabled(TRACK_CURVE_VERY_SMALL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WWT_FLATBTN;
    }

    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_LEVEL].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
    if (rideType == RIDE_TYPE_REVERSE_FREEFALL_COASTER || rideType == RIDE_TYPE_AIR_POWERED_VERTICAL_COASTER)
    {
        window_ride_construction_widgets[WIDX_LEVEL].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_FLATBTN;
    }
    if (is_track_enabled(TRACK_SLOPE) || is_track_enabled(TRACK_SLOPE_STEEP))
    {
        window_ride_construction_widgets[WIDX_LEVEL].type = WWT_FLATBTN;
    }
    if (is_track_enabled(TRACK_SLOPE))
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_SLOPE_UP].type = WWT_FLATBTN;
    }
    if (is_track_enabled(TRACK_HELIX_SMALL) && _currentTrackBankEnd != TRACK_BANK_NONE
        && _currentTrackSlopeEnd == TRACK_SLOPE_NONE)
    {
        if (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL)
        {
            // Enable helix
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_FLATBTN;
            if (rideType != RIDE_TYPE_SPLASH_BOATS)
                window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_FLATBTN;
        }
    }

    if (is_track_enabled(TRACK_SLOPE_STEEP))
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WWT_FLATBTN;
        if (rideType != RIDE_TYPE_SPLASH_BOATS)
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WWT_FLATBTN;
    }

    int32_t x;
    if ((is_track_enabled(TRACK_LIFT_HILL) && _currentTrackCurve < 256)
        || (gCheatsEnableChainLiftOnAllTrack && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_TRACK)))
    {
        window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WWT_FLATBTN;
        x = 9;
    }
    else
    {
        window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WWT_EMPTY;
        x = 23;
    }

    for (int32_t i = WIDX_SLOPE_DOWN_STEEP; i <= WIDX_SLOPE_UP_STEEP; i++)
    {
        window_ride_construction_widgets[i].left = x;
        window_ride_construction_widgets[i].right = x + 23;
        x += 24;
    }

    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
    if (is_track_enabled(TRACK_SLOPE_VERTICAL))
    {
        if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90)
        {
            int32_t originalSlopeUpSteepLeft = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left;
            int32_t originalSlopeUpSteepRight = window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right;
            for (int32_t i = WIDX_SLOPE_UP_STEEP; i > WIDX_SLOPE_DOWN_STEEP; i--)
            {
                window_ride_construction_widgets[i].left = window_ride_construction_widgets[i - 1].left;
                window_ride_construction_widgets[i].right = window_ride_construction_widgets[i - 1].right;
            }
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left = originalSlopeUpSteepLeft;
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right = originalSlopeUpSteepRight;
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_VERTICAL_RISE;
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_RISE_TIP;
        }
        else if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 || _previousTrackSlopeEnd == TRACK_SLOPE_DOWN_90)
        {
            int32_t originalSlopeDownSteepLeft = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
            int32_t originalSlopeDownSteepRight = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].right;
            for (int32_t i = WIDX_SLOPE_DOWN_STEEP; i < WIDX_SLOPE_UP_STEEP; i++)
            {
                window_ride_construction_widgets[i].left = window_ride_construction_widgets[i + 1].left;
                window_ride_construction_widgets[i].right = window_ride_construction_widgets[i + 1].right;
            }
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].left = originalSlopeDownSteepLeft;
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].right = originalSlopeDownSteepRight;
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_VERTICAL_DROP;
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_DROP_TIP;
        }
    }

    if (is_track_enabled(TRACK_HELIX_LARGE_UNBANKED) && _currentTrackSlopeEnd == TRACK_SLOPE_NONE
        && _currentTrackBankEnd == TRACK_BANK_NONE
        && (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT))
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_DOWN;
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
        window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_UP;
        window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

        int32_t tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
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

    if ((is_track_enabled(TRACK_HELIX_LARGE) || is_track_enabled(TRACK_HELIX_SMALL))
        && (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL)
        && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd != TRACK_BANK_NONE)
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_DOWN;
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
        window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_UP;
        window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

        int32_t tmp = window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].left;
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
    window_ride_construction_widgets[WIDX_BANK_LEFT].image = SPR_RIDE_CONSTRUCTION_LEFT_BANK;
    window_ride_construction_widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP;
    window_ride_construction_widgets[WIDX_BANK_LEFT].left = 47;
    window_ride_construction_widgets[WIDX_BANK_LEFT].right = 70;
    window_ride_construction_widgets[WIDX_BANK_LEFT].top = 132;
    window_ride_construction_widgets[WIDX_BANK_LEFT].bottom = 155;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].image = SPR_RIDE_CONSTRUCTION_NO_BANK;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_NO_ROLL_TIP;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].left = 71;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].right = 94;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].top = 132;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].bottom = 155;
    window_ride_construction_widgets[WIDX_BANK_RIGHT].image = SPR_RIDE_CONSTRUCTION_RIGHT_BANK;
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

    bool brakesSelected = _selectedTrackType == TRACK_ELEM_BRAKES || _currentTrackCurve == (0x100 | TRACK_ELEM_BRAKES);
    _boosterTrackSelected = track_element_is_booster(ride->type, _selectedTrackType)
        || (ride->type != RIDE_TYPE_STEEL_WILD_MOUSE && _currentTrackCurve == (0x100 | TRACK_ELEM_BOOSTER));

    if (!brakesSelected && !_boosterTrackSelected)
    {
        if (is_track_enabled(TRACK_FLAT_ROLL_BANKING))
        {
            window_ride_construction_widgets[WIDX_BANK_LEFT].type = WWT_FLATBTN;
            window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WWT_FLATBTN;
            window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WWT_FLATBTN;
        }
        if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
        {
            if (rideType == RIDE_TYPE_WATER_COASTER)
            {
                window_ride_construction_widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_RC_TRACK;
                window_ride_construction_widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_WATER_CHANNEL;
                window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_STANDARD_RC_TRACK_TIP;
                window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_WATER_CHANNEL_TIP;
                if ((_currentTrackCurve < TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == (0x100 | TRACK_ELEM_S_BEND_LEFT)
                     || _currentTrackCurve == (0x100 | TRACK_ELEM_S_BEND_RIGHT))
                    && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_BANK_NONE)
                {
                    window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                    window_ride_construction_widgets[WIDX_U_TRACK].type = WWT_FLATBTN;
                    window_ride_construction_widgets[WIDX_O_TRACK].type = WWT_FLATBTN;
                }
            }
            else
            {
                window_ride_construction_widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK;
                window_ride_construction_widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK;
                window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP;
                window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP;
                window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                window_ride_construction_widgets[WIDX_U_TRACK].type = WWT_FLATBTN;
                window_ride_construction_widgets[WIDX_O_TRACK].type = WWT_FLATBTN;
            }
        }
    }
    else
    {
        if (brakesSelected)
        {
            window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED;
            window_ride_construction_widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
            window_ride_construction_widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
            window_ride_construction_widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
        }
        else
        {
            window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED;
            window_ride_construction_widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
            window_ride_construction_widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
            window_ride_construction_widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
        }

        _currentlyShowingBrakeOrBoosterSpeed = true;
        window_ride_construction_widgets[WIDX_BANK_LEFT].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_VELOCITY;

        window_ride_construction_widgets[WIDX_BANK_LEFT].type = WWT_SPINNER;
        window_ride_construction_widgets[WIDX_BANK_LEFT].left = 12;
        window_ride_construction_widgets[WIDX_BANK_LEFT].right = 96;
        window_ride_construction_widgets[WIDX_BANK_LEFT].top = 138;
        window_ride_construction_widgets[WIDX_BANK_LEFT].bottom = 149;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WWT_BUTTON;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].text = STR_NUMERIC_UP;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].left = 84;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].right = 95;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].top = 139;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].bottom = 148;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WWT_BUTTON;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].text = STR_NUMERIC_DOWN;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].left = 72;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].right = 83;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].top = 139;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].bottom = 148;
        w->hold_down_widgets |= (1 << WIDX_BANK_STRAIGHT) | (1 << WIDX_BANK_RIGHT);
    }

    window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 162;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WWT_EMPTY;
    if ((rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER || rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT)
        && _selectedTrackType != TRACK_ELEM_BRAKES && _currentTrackCurve != (0x100 | TRACK_ELEM_BRAKES))
    {
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WWT_GROUPBOX;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WWT_SPINNER;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WWT_BUTTON;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WWT_BUTTON;
        window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 92;
        if (window_ride_construction_widgets[WIDX_BANK_LEFT].type != WWT_SPINNER)
        {
            for (int32_t i = WIDX_BANK_LEFT; i <= WIDX_BANK_RIGHT; i++)
            {
                window_ride_construction_widgets[i].left -= 36;
                window_ride_construction_widgets[i].right -= 36;
            }
        }
    }

    uint64_t pressedWidgets = w->pressed_widgets
        & ((1 << WIDX_BACKGROUND) | (1 << WIDX_TITLE) | (1 << WIDX_CLOSE) | (1 << WIDX_DIRECTION_GROUPBOX)
           | (1 << WIDX_SLOPE_GROUPBOX) | (1 << WIDX_BANKING_GROUPBOX) | (1 << WIDX_CONSTRUCT) | (1 << WIDX_DEMOLISH)
           | (1 << WIDX_PREVIOUS_SECTION) | (1 << WIDX_NEXT_SECTION) | (1 << WIDX_ENTRANCE_EXIT_GROUPBOX) | (1 << WIDX_ENTRANCE)
           | (1 << WIDX_EXIT));

    window_ride_construction_widgets[WIDX_CONSTRUCT].type = WWT_EMPTY;
    window_ride_construction_widgets[WIDX_DEMOLISH].type = WWT_FLATBTN;
    window_ride_construction_widgets[WIDX_ROTATE].type = WWT_EMPTY;
    if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
    {
        window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_EMPTY;
        window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_EMPTY;
    }
    else
    {
        window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WWT_FLATBTN;
        window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WWT_FLATBTN;
    }

    switch (_rideConstructionState)
    {
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

    rct_widgetindex widgetIndex;
    switch (_currentTrackCurve)
    {
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

    switch (_currentTrackSlopeEnd)
    {
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

    if (!_currentlyShowingBrakeOrBoosterSpeed)
    {
        if (ride_type_has_flag(rideType, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
        {
            if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_PIECES)
            {
                w->pressed_widgets |= (1ULL << WIDX_O_TRACK);
            }
            else
            {
                w->pressed_widgets |= (1ULL << WIDX_U_TRACK);
            }
        }
        switch (_currentTrackBankEnd)
        {
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

    if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
        pressedWidgets |= (1 << WIDX_CHAIN_LIFT);

    w->pressed_widgets = pressedWidgets;
    window_invalidate(w);
}

static void window_ride_construction_select_map_tiles(
    Ride* ride, int32_t trackType, int32_t trackDirection, int32_t x, int32_t y)
{
    // If the scenery tool is active, we do not display our tiles as it
    // will conflict with larger scenery objects selecting tiles
    if (scenery_tool_is_active())
    {
        return;
    }

    const rct_preview_track* trackBlock;
    int32_t offsetX, offsetY;

    trackBlock = get_track_def_from_ride(ride, trackType);
    trackDirection &= 3;
    int32_t selectionTileIndex = 0;
    while (trackBlock->index != 255)
    {
        switch (trackDirection)
        {
            default:
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
 *  rct2: 0x006C776D
 */
static void window_ride_construction_show_special_track_dropdown(rct_window* w, rct_widget* widget)
{
    int32_t defaultIndex = -1;
    for (int32_t i = 0; i < _numCurrentPossibleRideConfigurations; i++)
    {
        uint8_t trackPiece = _currentPossibleRideConfigurations[i];
        rct_string_id trackPieceStringId = RideConfigurationStringIds[trackPiece];
        if (trackPieceStringId == STR_RAPIDS)
        {
            Ride* ride = get_ride(_currentRideIndex);
            if (ride->type == RIDE_TYPE_CAR_RIDE)
                trackPieceStringId = STR_LOG_BUMPS;
        }
        if (trackPieceStringId == STR_SPINNING_CONTROL_TOGGLE_TRACK)
        {
            Ride* ride = get_ride(_currentRideIndex);
            if (ride->type != RIDE_TYPE_STEEL_WILD_MOUSE)
                trackPieceStringId = STR_BOOSTER;
        }
        gDropdownItemsFormat[i] = trackPieceStringId;
        if ((trackPiece | 0x100) == _currentTrackCurve)
        {
            defaultIndex = i;
        }
    }

    window_dropdown_show_text_custom_width(
        w->x + widget->left, w->y + widget->top, widget->bottom - widget->top + 1, w->colours[1], 0, 0,
        _numCurrentPossibleRideConfigurations, widget->right - widget->left);

    for (int32_t i = 0; i < 32; i++)
    {
        if (_currentDisabledSpecialTrackPieces & (1 << i))
        {
            dropdown_set_disabled(i, true);
        }
    }
    gDropdownDefaultIndex = defaultIndex;
}

/**
 *
 *  rct2: 0x006C7630
 */
static void ride_selected_track_set_seat_rotation(int32_t seatRotation)
{
    int32_t x, y, z;
    x = _currentTrackBegin.x;
    y = _currentTrackBegin.y;
    z = _currentTrackBegin.z;
    sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, seatRotation, nullptr, (1 << 5));
    window_ride_construction_update_active_elements();
}

/**
 *
 *  rct2: 0x006C7502
 */
static void loc_6C7502(int32_t al)
{
    _currentTrackSlopeEnd = al;
    _currentTrackPrice = MONEY32_UNDEFINED;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT)
    {
        if (al != 2 && al != 4 && al != 0)
        {
            _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }
    window_ride_construction_update_active_elements();
}

/**
 *
 *  rct2: 0x006C76E9
 */
static void ride_construction_set_brakes_speed(int32_t brakesSpeed)
{
    TileElement* tileElement;
    int32_t x, y, z;

    x = _currentTrackBegin.x;
    y = _currentTrackBegin.y;
    z = _currentTrackBegin.z;
    if (!sub_6C683D(&x, &y, &z, _currentTrackPieceDirection & 3, _currentTrackPieceType, 0, &tileElement, 0))
    {
        game_do_command(
            _currentTrackBegin.x, GAME_COMMAND_FLAG_APPLY | ((brakesSpeed) << 8), _currentTrackBegin.y,
            tileElement->AsTrack()->GetTrackType(), GAME_COMMAND_SET_BRAKES_SPEED, _currentTrackBegin.z, 0);
    }
    window_ride_construction_update_active_elements();
}

/**
 *
 *  rct2: 0x006CC6A8
 */
void ride_construction_toolupdate_construct(int32_t screenX, int32_t screenY)
{
    int32_t x, y, z;
    Ride* ride;
    const rct_preview_track* trackBlock;

    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    if (!ride_get_place_position_from_screen_position(screenX, screenY, &x, &y))
    {
        ride_construction_invalidate_current_track();
        map_invalidate_map_selection_tiles();
        return;
    }

    z = _trackPlaceZ;
    if (z == 0)
        z = map_get_highest_z(x >> 5, y >> 5);

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
    gMapSelectArrowDirection = _currentTrackPieceDirection;
    gMapSelectArrowPosition.x = x;
    gMapSelectArrowPosition.y = y;
    gMapSelectArrowPosition.z = z;
    gMapSelectionTiles[0].x = x;
    gMapSelectionTiles[0].y = y;
    gMapSelectionTiles[1].x = -1;
    gMapSelectionTiles[1].y = -1;

    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState;
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, nullptr, nullptr, nullptr, nullptr))
    {
        ride_construction_invalidate_current_track();
        map_invalidate_map_selection_tiles();
        return;
    }
    _currentTrackPieceType = trackType;
    ride = get_ride(_currentRideIndex);

    // Re-using this other code, very slight difference from original
    //   - Original code checks for MSB mask instead of 255 on trackPart->var_00
    //   - Original code checks this first as its already set origin tile, probably just a micro optimisation
    window_ride_construction_select_map_tiles(ride, trackType, trackDirection, x, y);

    gMapSelectArrowPosition.z = z;
    if (_trackPlaceZ == 0)
    {
        // Raise z above all slopes and water
        if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
        {
            int32_t highestZ = 0;
            LocationXY16* selectedTile = gMapSelectionTiles;
            while (selectedTile->x != -1)
            {
                if (selectedTile->x < (256 * 32) && selectedTile->y < (256 * 32))
                {
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
    int32_t bx = 0;
    do
    {
        bx = std::min<int32_t>(bx, trackBlock->z);
        trackBlock++;
    } while (trackBlock->index != 255);
    z -= bx;

    gMapSelectArrowPosition.z = z;
    bx = 41;
    _currentTrackBegin.x = x;
    _currentTrackBegin.y = y;
    _currentTrackBegin.z = z;
    if ((_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK) && x == _previousTrackPiece.x && y == _previousTrackPiece.y
        && z == _previousTrackPiece.z)
    {
        map_invalidate_map_selection_tiles();
        return;
    }

    _previousTrackPiece.x = x;
    _previousTrackPiece.y = y;
    _previousTrackPiece.z = z;
    if (ride->type == RIDE_TYPE_MAZE)
    {
        for (;;)
        {
            window_ride_construction_update_state(
                &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, nullptr);
            _currentTrackPrice = place_provisional_track_piece(
                rideIndex, trackType, trackDirection, liftHillAndAlternativeState, x, y, z);
            if (_currentTrackPrice != MONEY32_UNDEFINED)
                break;

            bx--;
            if (bx == 0)
                break;

            _currentTrackBegin.z -= 8;
            if (_currentTrackBegin.z & LOCATION_NULL)
                break;

            if (bx >= 0)
                _currentTrackBegin.z += 16;
        }

        auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
        context_broadcast_intent(&intent);
        map_invalidate_map_selection_tiles();
        return;
    }

    for (;;)
    {
        window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, nullptr);
        _currentTrackPrice = place_provisional_track_piece(
            rideIndex, trackType, trackDirection, liftHillAndAlternativeState, x, y, z);
        if (_currentTrackPrice != MONEY32_UNDEFINED)
            break;

        bx--;
        if (bx == 0)
            break;

        _currentTrackBegin.z -= 8;
        if (_currentTrackBegin.z & LOCATION_NULL)
            break;

        if (bx >= 0)
            _currentTrackBegin.z += 16;
    }

    if (_autoRotatingShop && _rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE
        && ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_IS_SHOP))
    {
        TileElement* pathsByDir[4];
        constexpr sLocationXY8 DirOffsets[4] = { { -1, 0 }, { 0, 1 }, { 1, 0 }, { 0, -1 } };

        bool keepOrientation = false;
        for (int8_t i = 0; i < 4; i++)
        {
            pathsByDir[i] = map_get_footpath_element((x >> 5) + DirOffsets[i].x, (y >> 5) + DirOffsets[i].y, z >> 3);

            if (pathsByDir[i] && (pathsByDir[i])->AsPath()->IsSloped() && (pathsByDir[i])->AsPath()->GetSlopeDirection() != i)
            {
                pathsByDir[i] = nullptr;
            }

            // Sloped path on the level below
            if (!pathsByDir[i])
            {
                pathsByDir[i] = map_get_footpath_element((x >> 5) + DirOffsets[i].x, (y >> 5) + DirOffsets[i].y, (z >> 3) - 2);

                if (pathsByDir[i]
                    && (!(pathsByDir[i])->AsPath()->IsSloped()
                        || (pathsByDir[i])->AsPath()->GetSlopeDirection() != direction_reverse(i)))
                {
                    pathsByDir[i] = nullptr;
                }
            }

            if (pathsByDir[i] && (pathsByDir[i])->AsPath()->IsQueue())
            {
                pathsByDir[i] = nullptr;
            }

            if (pathsByDir[i] && i == _currentTrackPieceDirection)
            {
                keepOrientation = true;
                break;
            }
        }

        if (!keepOrientation)
        {
            for (int8_t i = 0; i < 4; i++)
            {
                if (pathsByDir[i])
                {
                    _currentTrackPieceDirection = i;

                    window_ride_construction_update_state(
                        &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, nullptr);
                    place_provisional_track_piece(rideIndex, trackType, trackDirection, liftHillAndAlternativeState, x, y, z);
                    gMapSelectArrowDirection = _currentTrackPieceDirection;
                    break;
                }
            }
        }
    }

    window_ride_construction_update_active_elements();
    map_invalidate_map_selection_tiles();
}

/**
 *
 *  rct2: 0x006CD354
 */
void ride_construction_toolupdate_entrance_exit(int32_t screenX, int32_t screenY)
{
    int32_t x, y, direction;
    uint8_t stationNum;

    map_invalidate_selection_rect();
    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    ride_get_entrance_or_exit_position_from_screen_position(screenX, screenY, &x, &y, &direction);
    if (gRideEntranceExitPlaceDirection == 255)
    {
        ride_construction_invalidate_current_track();
        return;
    }
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA.x = x;
    gMapSelectPositionA.y = y;
    gMapSelectPositionB.x = x;
    gMapSelectPositionB.y = y;
    gMapSelectArrowDirection = direction_reverse(direction);
    gMapSelectArrowPosition.x = x;
    gMapSelectArrowPosition.y = y;
    gMapSelectArrowPosition.z = _unkF44188.z * 8;
    map_invalidate_selection_rect();

    direction = direction_reverse(gRideEntranceExitPlaceDirection);
    stationNum = gRideEntranceExitPlaceStationIndex;
    if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT) || x != gRideEntranceExitGhostPosition.x
        || y != gRideEntranceExitGhostPosition.y || direction != gRideEntranceExitGhostPosition.direction
        || stationNum != gRideEntranceExitGhostStationIndex)
    {
        auto ride = get_ride(_currentRideIndex);
        if (ride != nullptr)
        {
            _currentTrackPrice = ride_entrance_exit_place_ghost(ride, x, y, direction, gRideEntranceExitPlaceType, stationNum);
        }
        window_ride_construction_update_active_elements();
    }
}

/**
 *
 *  rct2: 0x006CCA73
 */
void ride_construction_tooldown_construct(int32_t screenX, int32_t screenY)
{
    const CursorState* state = context_get_cursor_state();
    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState, x, y, z, properties, highestZ;
    rct_window* w;

    map_invalidate_map_selection_tiles();
    ride_construction_invalidate_current_track();

    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &x, &y, &z, &properties))
        return;

    _currentTrackPieceType = trackType;

    // Raise z above all slopes and water
    highestZ = 0;
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        LocationXY16* selectedTile = gMapSelectionTiles;
        while (selectedTile->x != -1)
        {
            if (selectedTile->x >= (256 * 32) || selectedTile->y >= (256 * 32))
                continue;

            z = map_get_highest_z(selectedTile->x >> 5, selectedTile->y >> 5);
            if (z > highestZ)
                highestZ = z;

            selectedTile++;
        }
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    if (!ride_get_place_position_from_screen_position(screenX, screenY, &x, &y))
        return;

    z = _trackPlaceZ;
    if (z == 0)
        z = map_get_highest_z(x >> 5, y >> 5);

    tool_cancel();

    Ride* ride = get_ride(_currentRideIndex);
    if (_trackPlaceZ == 0)
    {
        const rct_preview_track* trackBlock = get_track_def_from_ride(ride, _currentTrackPieceType);
        int32_t bx = 0;
        do
        {
            bx = std::min<int32_t>(bx, trackBlock->z);
            trackBlock++;
        } while (trackBlock->index != 255);
        z -= bx;

        // FIX not sure exactly why it starts trial and error place from a lower Z, but it causes issues with disable clearance
        if (!gCheatsDisableClearanceChecks && z > 16)
        {
            z -= 16;
        }
    }
    else
    {
        z = _trackPlaceZ;
    }

    if (ride->type == RIDE_TYPE_MAZE)
    {
        for (int32_t zAttempts = 41; zAttempts >= 0; zAttempts--)
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
            _currentTrackBegin.x = x;
            _currentTrackBegin.y = y;
            _currentTrackBegin.z = z;
            _currentTrackSelectionFlags = 0;
            _rideConstructionArrowPulseTime = 0;
            auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
            context_broadcast_intent(&intent);
            w = window_find_by_class(WC_RIDE_CONSTRUCTION);
            if (w == nullptr)
                break;

            gDisableErrorWindowSound = true;

            _trackPlaceCost = maze_set_track(
                _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z, GAME_COMMAND_FLAG_APPLY, true, 0,
                _currentRideIndex, GC_SET_MAZE_TRACK_BUILD);

            gDisableErrorWindowSound = false;

            if (_trackPlaceCost == MONEY32_UNDEFINED)
            {
                _rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
                rct_string_id errorText = gGameCommandErrorText;
                z -= 8;
                if (errorText == STR_NOT_ENOUGH_CASH_REQUIRES || errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER
                    || errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER || errorText == STR_RIDE_CANT_BUILD_THIS_UNDERWATER
                    || errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND || errorText == STR_TOO_HIGH_FOR_SUPPORTS
                    || zAttempts == 0 || z < 0)
                {
                    audio_play_sound(SOUND_ERROR, 0, state->x);
                    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
                    if (w != nullptr)
                    {
                        tool_set(w, WIDX_CONSTRUCT, TOOL_CROSSHAIR);
                        input_set_flag(INPUT_FLAG_6, true);
                        _trackPlaceCtrlState = false;
                        _trackPlaceShiftState = false;
                    }
                    auto intent2 = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
                    context_broadcast_intent(&intent2);
                    break;
                }
                else if (zAttempts >= 0)
                {
                    z += 16;
                }
            }
            else
            {
                window_close_by_class(WC_ERROR);
                audio_play_sound_at_location(
                    SOUND_PLACE_ITEM, _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z);
                break;
            }
        }
        return;
    }

    for (int32_t zAttempts = 41; zAttempts >= 0; zAttempts--)
    {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
        _currentTrackBegin.x = x;
        _currentTrackBegin.y = y;
        _currentTrackBegin.z = z;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        window_ride_construction_update_active_elements();
        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w == nullptr)
            break;

        gDisableErrorWindowSound = true;
        window_event_mouse_up_call(w, WIDX_CONSTRUCT);
        gDisableErrorWindowSound = false;

        if (_trackPlaceCost == MONEY32_UNDEFINED)
        {
            rct_string_id errorText = gGameCommandErrorText;
            z -= 8;
            if (errorText == STR_NOT_ENOUGH_CASH_REQUIRES || errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER
                || errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER || errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND
                || errorText == STR_TOO_HIGH_FOR_SUPPORTS || zAttempts == 0 || z < 0)
            {
                int32_t saveTrackDirection = _currentTrackPieceDirection;
                int32_t saveCurrentTrackCurve = _currentTrackCurve;
                int32_t savePreviousTrackSlopeEnd = _previousTrackSlopeEnd;
                int32_t saveCurrentTrackSlopeEnd = _currentTrackSlopeEnd;
                int32_t savePreviousTrackBankEnd = _previousTrackBankEnd;
                int32_t saveCurrentTrackBankEnd = _currentTrackBankEnd;
                int32_t saveCurrentTrackAlternative = _currentTrackAlternative;
                int32_t saveCurrentTrackLiftHill = _currentTrackLiftHill;

                ride_initialise_construction_window(ride);

                _currentTrackPieceDirection = saveTrackDirection;
                _currentTrackCurve = saveCurrentTrackCurve;
                _previousTrackSlopeEnd = savePreviousTrackSlopeEnd;
                _currentTrackSlopeEnd = saveCurrentTrackSlopeEnd;
                _previousTrackBankEnd = savePreviousTrackBankEnd;
                _currentTrackBankEnd = saveCurrentTrackBankEnd;
                _currentTrackAlternative = saveCurrentTrackAlternative;
                _currentTrackLiftHill = saveCurrentTrackLiftHill;

                audio_play_sound(SOUND_ERROR, 0, state->x);
                break;
            }
            else if (zAttempts >= 0)
            {
                z += 16;
            }
        }
        else
        {
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006CCA73
 */
static void ride_construction_tooldown_entrance_exit(int32_t screenX, int32_t screenY)
{
    ride_construction_invalidate_current_track();
    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    int32_t mapX, mapY, direction;
    ride_get_entrance_or_exit_position_from_screen_position(screenX, screenY, &mapX, &mapY, &direction);
    if (gRideEntranceExitPlaceDirection == 255)
        return;

    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        { _unkF44188.x, _unkF44188.y }, direction_reverse(gRideEntranceExitPlaceDirection), gRideEntranceExitPlaceRideIndex,
        gRideEntranceExitPlaceStationIndex,
        gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

    rideEntranceExitPlaceAction.SetCallback([=](const GameAction* ga, const GameActionResult* result) {
        if (result->Error != GA_ERROR::OK)
            return;

        audio_play_sound_at_location(SOUND_PLACE_ITEM, result->Position.x, result->Position.y, result->Position.z);

        Ride* ride = get_ride(gRideEntranceExitPlaceRideIndex);
        if (ride_are_all_possible_entrances_and_exits_built(ride))
        {
            tool_cancel();
            if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
            {
            window_close_by_class(WC_RIDE_CONSTRUCTION);
            }
        }
        else
        {
            gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
            window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
            gCurrentToolWidget.widget_index = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
                ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
                : WC_RIDE_CONSTRUCTION__WIDX_EXIT;
        }
    });
    auto res = GameActions::Execute(&rideEntranceExitPlaceAction);
}

void window_ride_construction_keyboard_shortcut_turn_left()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_LEFT_SMALL:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_LEFT:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_LARGE:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_NONE:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_LARGE:
            if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_SMALL:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_VERY_SMALL:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_LEFT_CURVE_VERY_SMALL) && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_turn_right()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_RIGHT_SMALL:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_RIGHT:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_LARGE:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_NONE:
            if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_LARGE:
            if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_SMALL:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_VERY_SMALL:
            if (!widget_is_disabled(w, WIDX_LEFT_CURVE_SMALL) && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_LEFT_CURVE_LARGE) && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_LARGE) && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (!widget_is_disabled(w, WIDX_RIGHT_CURVE_SMALL) && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !widget_is_disabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_use_track_default()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    if (!widget_is_disabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WIDX_STRAIGHT);
    }

    if (!widget_is_disabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WIDX_LEVEL);
    }

    if (!widget_is_disabled(w, WIDX_CHAIN_LIFT) && w->widgets[WIDX_CHAIN_LIFT].type != WWT_EMPTY
        && _currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
    {
        window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
    }

    if (!widget_is_disabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WWT_EMPTY)
    {
        window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
    }
}

void window_ride_construction_keyboard_shortcut_slope_down()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_DOWN_25:
            if (!widget_is_disabled(w, WIDX_SLOPE_DOWN_STEEP) && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!widget_is_disabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_DOWN_STEEP) && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_UP_25:
            if (!widget_is_disabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_DOWN_STEEP) && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_UP_60:
            if (!widget_is_disabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!widget_is_disabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_DOWN_STEEP) && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_slope_up()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_UP_25:
            if (!widget_is_disabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!widget_is_disabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            if (!widget_is_disabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_60:
            if (!widget_is_disabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!widget_is_disabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!widget_is_disabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_chain_lift_toggle()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_CHAIN_LIFT) || w->widgets[WIDX_CHAIN_LIFT].type == WWT_EMPTY)
    {
        return;
    }

    window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
}

void window_ride_construction_keyboard_shortcut_bank_left()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_BANK_STRAIGHT) || w->widgets[WIDX_BANK_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!widget_is_disabled(w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_LEFT);
            }
            break;
        case TRACK_BANK_RIGHT:
            if (!widget_is_disabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_LEFT);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_bank_right()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_BANK_STRAIGHT) || w->widgets[WIDX_BANK_STRAIGHT].type == WWT_EMPTY)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!widget_is_disabled(w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_RIGHT);
            }
            break;
        case TRACK_BANK_LEFT:
            if (!widget_is_disabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!widget_is_disabled(w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WWT_EMPTY)
            {
                window_event_mouse_down_call(w, WIDX_BANK_RIGHT);
            }
            else
            {
                return;
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_previous_track()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_PREVIOUS_SECTION) || w->widgets[WIDX_PREVIOUS_SECTION].type == WWT_EMPTY)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_PREVIOUS_SECTION);
}

void window_ride_construction_keyboard_shortcut_next_track()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_NEXT_SECTION) || w->widgets[WIDX_NEXT_SECTION].type == WWT_EMPTY)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_NEXT_SECTION);
}

void window_ride_construction_keyboard_shortcut_build_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WWT_EMPTY)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_CONSTRUCT);
}

void window_ride_construction_keyboard_shortcut_demolish_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || widget_is_disabled(w, WIDX_DEMOLISH) || w->widgets[WIDX_DEMOLISH].type == WWT_EMPTY)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_DEMOLISH);
}

static bool track_piece_direction_is_diagonal(const uint8_t direction)
{
    return direction >= 4;
}
