/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/actions/TrackPlaceAction.h>
#include <openrct2/actions/TrackRemoveAction.h>
#include <openrct2/actions/TrackSetBrakeSpeedAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/platform.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
static constexpr const int32_t WH = 394;
static constexpr const int32_t WW = 166;
static constexpr const uint16_t ARROW_PULSE_DURATION = 200;

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
    WIDX_SIMULATE,
};

validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_CONSTRUCT);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ENTRANCE);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_EXIT);
validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ROTATE);

static rct_widget window_ride_construction_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget        ({  3,  17}, {160,  57}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_DIRECTION                                                              ),
    MakeWidget        ({  3,  76}, {160,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_SLOPE                                                                  ),
    MakeWidget        ({  3, 120}, {160,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_ROLL_BANKING                                                           ),
    MakeWidget        ({  6,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL,  STR_RIDE_CONSTRUCTION_LEFT_CURVE_VERY_SMALL_TIP     ),
    MakeWidget        ({  6,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL,  STR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL_TIP          ),
    MakeWidget        ({ 28,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_LEFT_CURVE,        STR_RIDE_CONSTRUCTION_LEFT_CURVE_TIP                ),
    MakeWidget        ({ 72,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_STRAIGHT,          STR_RIDE_CONSTRUCTION_STRAIGHT_TIP                  ),
    MakeWidget        ({116,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_RIGHT_CURVE,       STR_RIDE_CONSTRUCTION_RIGHT_CURVE_TIP               ),
    MakeWidget        ({138,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL, STR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL_TIP         ),
    MakeWidget        ({138,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL, STR_RIDE_CONSTRUCTION_RIGHT_CURVE_VERY_SMALL_TIP    ),
    MakeWidget        ({ 17,  55}, {132,  14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_YELLOW_STRING,                       STR_RIDE_CONSTRUCTION_OTHER_TRACK_CONFIGURATIONS_TIP),
    MakeWidget        ({ 23,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP,  STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP          ),
    MakeWidget        ({ 47,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_DOWN,        STR_RIDE_CONSTRUCTION_SLOPE_DOWN_TIP                ),
    MakeWidget        ({ 71,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL,       STR_RIDE_CONSTRUCTION_LEVEL_TIP                     ),
    MakeWidget        ({ 95,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP,          STR_RIDE_CONSTRUCTION_SLOPE_UP_TIP                  ),
    MakeWidget        ({119,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP,    STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP            ),
    MakeWidget        ({134,  88}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CHAIN_LIFT,                          STR_RIDE_CONSTRUCTION_CHAIN_LIFT_TIP                ),
    MakeWidget        ({ 47, 132}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_LEFT_BANK,         STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP       ),
    MakeWidget        ({ 71, 132}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_NO_BANK,           STR_RIDE_CONSTRUCTION_NO_ROLL_TIP                   ),
    MakeWidget        ({ 95, 132}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_RIGHT_BANK,        STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP      ),
    MakeWidget        ({  3, 164}, {160, 170}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                              STR_RIDE_CONSTRUCTION_CONSTRUCT_SELECTED_SECTION_TIP),
    MakeWidget        ({ 60, 338}, { 46,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_DEMOLISH_CURRENT_SECTION,            STR_RIDE_CONSTRUCTION_REMOVE_HIGHLIGHTED_SECTION_TIP),
    MakeWidget        ({ 50,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE,  STR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE_TIP          ),
    MakeWidget        ({ 30, 338}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_PREVIOUS,                            STR_RIDE_CONSTRUCTION_MOVE_TO_PREVIOUS_SECTION_TIP  ),
    MakeWidget        ({112, 338}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_NEXT,                                STR_RIDE_CONSTRUCTION_MOVE_TO_NEXT_SECTION_TIP      ),
    MakeWidget        ({  3, 362}, {160,  28}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                                 ),
    MakeWidget        ({  9, 372}, { 70,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_ENTRANCE,          STR_RIDE_CONSTRUCTION_ENTRANCE_TIP                  ),
    MakeWidget        ({ 87, 372}, { 70,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_EXIT,              STR_RIDE_CONSTRUCTION_EXIT_TIP                      ),
    MakeWidget        ({ 94,  29}, { 22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE, STR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE_TIP         ),
    MakeWidget        ({ 72, 338}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_ROTATE_ARROW,                        STR_ROTATE_90_TIP                                   ),
    MakeWidget        ({ 19, 132}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK,    STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP       ),
    MakeWidget        ({123, 132}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK,    STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP   ),
    MakeWidget        ({ 96, 120}, { 67,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_SEAT_ROT                                                               ),
    MakeSpinnerWidgets({101, 138}, { 58,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary, 0,                                       STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP),
    MakeWidget        ({139, 338}, { 24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_G2_SIMULATE,                         STR_SIMULATE_RIDE_TIP                               ),
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
static void window_ride_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_ride_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_ride_construction_invalidate(rct_window *w);
static void window_ride_construction_paint(rct_window *w, rct_drawpixelinfo *dpi);
static bool track_piece_direction_is_diagonal(const uint8_t direction);

//0x993EEC
static rct_window_event_list window_ride_construction_events([](auto& events)
{
    events.close = &window_ride_construction_close;
    events.mouse_up = &window_ride_construction_mouseup;
    events.resize = &window_ride_construction_resize;
    events.mouse_down = &window_ride_construction_mousedown;
    events.dropdown = &window_ride_construction_dropdown;
    events.update = &window_ride_construction_update;
    events.tool_update = &window_ride_construction_toolupdate;
    events.tool_down = &window_ride_construction_tooldown;
    events.invalidate = &window_ride_construction_invalidate;
    events.paint = &window_ride_construction_paint;
});

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
    STR_BOOSTER,                            // 100
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
    STR_QUARTER_LOOP,                       // 255
    STR_SPINNING_CONTROL_TOGGLE_TRACK,      // 256
};
// clang-format on

#pragma endregion

static bool _trackPlaceCtrlState;
static int32_t _trackPlaceCtrlZ;
static bool _trackPlaceShiftState;
static ScreenCoordsXY _trackPlaceShiftStart;
static int32_t _trackPlaceShiftZ;
static int32_t _trackPlaceZ;
static money32 _trackPlaceCost;
static bool _autoOpeningShop;
static bool _autoRotatingShop;
static uint8_t _currentlyShowingBrakeOrBoosterSpeed;

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
    rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t edx,
    const CoordsXY& originCoords, int32_t originZ);
static void window_ride_construction_update_map_selection();
static void window_ride_construction_update_possible_ride_configurations();
static void window_ride_construction_update_widgets(rct_window* w);
static void window_ride_construction_select_map_tiles(
    Ride* ride, int32_t trackType, int32_t trackDirection, const CoordsXY& tileCoords);
static void window_ride_construction_show_special_track_dropdown(rct_window* w, rct_widget* widget);
static void ride_selected_track_set_seat_rotation(int32_t seatRotation);
static void loc_6C7502(int32_t al);
static void ride_construction_set_brakes_speed(int32_t brakesSpeed);
static void ride_construction_tooldown_entrance_exit(const ScreenCoordsXY& screenCoords);

static track_type_t _currentPossibleRideConfigurations[32];

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
    return (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE) ? ride->GetRideTypeDescriptor().AlternateType
                                                                         : ride->type;
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

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return nullptr;

    _stationConstructed = ride->num_stations != 0;
    _deferClose = false;

    if (ride->type == RIDE_TYPE_MAZE)
    {
        return context_open_window_view(WV_MAZE_CONSTRUCTION);
    }

    auto w = WindowCreate(
        ScreenCoordsXY(0, 29), 166, 394, &window_ride_construction_events, WC_RIDE_CONSTRUCTION, WF_NO_AUTO_CLOSE);

    w->widgets = window_ride_construction_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
        | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
        | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL) | (1ULL << WIDX_SPECIAL_TRACK_DROPDOWN) | (1ULL << WIDX_SLOPE_DOWN_STEEP)
        | (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP)
        | (1ULL << WIDX_CHAIN_LIFT) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT)
        | (1ULL << WIDX_CONSTRUCT) | (1ULL << WIDX_DEMOLISH) | (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_PREVIOUS_SECTION)
        | (1ULL << WIDX_NEXT_SECTION) | (1ULL << WIDX_SIMULATE) | (1ULL << WIDX_ENTRANCE) | (1ULL << WIDX_EXIT)
        | (1ULL << WIDX_RIGHT_CURVE_LARGE) | (1ULL << WIDX_ROTATE) | (1ULL << WIDX_U_TRACK) | (1ULL << WIDX_O_TRACK)
        | (1ULL << WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP) | (1ULL << WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN);

    WindowInitScrollWidgets(w);

    w->colours[0] = COLOUR_DARK_BROWN;
    w->colours[1] = COLOUR_DARK_BROWN;
    w->colours[2] = COLOUR_DARK_BROWN;

    w->number = rideIndex;

    window_push_others_right(w);
    show_gridlines();

    _currentTrackPrice = MONEY32_UNDEFINED;
    _currentBrakeSpeed2 = 8;
    _currentSeatRotationAngle = 4;

    _currentTrackCurve = ride->GetRideTypeDescriptor().StartTrackPiece | RideConstructionSpecialPieceSelected;
    _currentTrackSlopeEnd = 0;
    _currentTrackBankEnd = 0;
    _currentTrackLiftHill = 0;
    _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

    if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED))
        _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

    _previousTrackBankEnd = 0;
    _previousTrackSlopeEnd = 0;

    _currentTrackPieceDirection = 0;
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_PLACE;
    _currentTrackSelectionFlags = 0;
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
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    // In order to cancel the yellow arrow correctly the
    // selection tool should be cancelled. Don't do a tool cancel if
    // another window has already taken control of tool.
    if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
        tool_cancel();

    hide_gridlines();

    // If we demolish a ride all windows will be closed including the construction window,
    // the ride at this point is already gone.
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
    {
        return;
    }

    if (ride_try_get_origin_element(ride, nullptr))
    {
        // Auto open shops if required.
        if (ride->mode == RideMode::ShopStall && gConfigGeneral.auto_open_shops)
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

        ride->SetToDefaultInspectionInterval();
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
        case WIDX_SIMULATE:
        {
            auto ride = get_ride(_currentRideIndex);
            if (ride != nullptr)
            {
                auto status = ride->status == RIDE_STATUS_SIMULATING ? RIDE_STATUS_CLOSED : RIDE_STATUS_SIMULATING;
                ride_set_status(ride, status);
            }
            break;
        }
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

    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    int32_t rideType = ride_get_alternative_type(ride);

    uint64_t disabledWidgets = 0;

    if (_currentTrackCurve & RideConstructionSpecialPieceSelected)
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
        if (!ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
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
                && !(
                    is_track_enabled(TRACK_SLOPE_STEEP_LONG)
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
                && !(
                    is_track_enabled(TRACK_SLOPE_STEEP_LONG)
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
        if (ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
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
        if (ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
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
        if (window_ride_construction_update_state(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
        {
            disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
        }
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        disabledWidgets |= (1ULL << WIDX_PREVIOUS_SECTION);
        if (window_ride_construction_update_state(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
        {
            disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
        }
    }
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
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
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

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
                    _currentTrackCurve = TrackElemType::LeftHalfBankedHelixDownLarge | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightHalfBankedHelixDownLarge | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TrackElemType::LeftHalfBankedHelixDownSmall | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightHalfBankedHelixDownSmall | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TrackElemType::LeftQuarterBankedHelixLargeDown | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightQuarterBankedHelixLargeDown | RideConstructionSpecialPieceSelected;
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
                        _currentTrackCurve = TrackElemType::LeftQuarterHelixLargeDown | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    else if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightQuarterHelixLargeDown | RideConstructionSpecialPieceSelected;
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
            if (ride->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
            {
                if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT && _currentTrackCurve == TRACK_CURVE_NONE)
                {
                    _currentTrackCurve = TrackElemType::ReverseFreefallSlope | RideConstructionSpecialPieceSelected;
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
                    _currentTrackCurve = TrackElemType::LeftHalfBankedHelixUpLarge | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightHalfBankedHelixUpLarge | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TrackElemType::LeftHalfBankedHelixUpSmall | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightHalfBankedHelixUpSmall | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
            }
            if (is_track_enabled(TRACK_HELIX_LARGE))
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                {
                    _currentTrackCurve = TrackElemType::LeftQuarterBankedHelixLargeUp | RideConstructionSpecialPieceSelected;
                    _currentTrackPrice = MONEY32_UNDEFINED;
                    window_ride_construction_update_active_elements();
                    break;
                }
                else if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                {
                    _currentTrackCurve = TrackElemType::RightQuarterBankedHelixLargeUp | RideConstructionSpecialPieceSelected;
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
                        _currentTrackCurve = TrackElemType::LeftQuarterHelixLargeUp | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    else if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightQuarterHelixLargeUp | RideConstructionSpecialPieceSelected;
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
    track_type_t trackPiece = _currentPossibleRideConfigurations[dropdownIndex];
    switch (trackPiece)
    {
        case TrackElemType::EndStation:
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendRight:
            _currentTrackSlopeEnd = 0;
            break;
        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            _currentTrackBankEnd = TRACK_BANK_NONE;
            _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
            break;
    }
    _currentTrackCurve = trackPiece | RideConstructionSpecialPieceSelected;
    window_ride_construction_update_active_elements();
}
static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const TrackPlaceActionResult* result);
static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const TrackPlaceActionResult* result);
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

static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const TrackPlaceActionResult* result)
{
    if (result->Error != GameActions::Status::Ok)
    {
        window_ride_construction_update_active_elements();
        return;
    }
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        int32_t trackDirection = _currentTrackPieceDirection;
        auto trackPos = _currentTrackBegin;
        if (!(trackDirection & 4))
        {
            trackPos -= CoordsDirectionDelta[trackDirection];
        }

        CoordsXYE next_track;
        if (track_block_get_next_from_zero(trackPos, ride, trackDirection, &next_track, &trackPos.z, &trackDirection, false))
        {
            _currentTrackBegin.x = next_track.x;
            _currentTrackBegin.y = next_track.y;
            _currentTrackBegin.z = trackPos.z;
            _currentTrackPieceDirection = next_track.element->GetDirection();
            _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
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

static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const TrackPlaceActionResult* result)
{
    if (result->Error != GameActions::Status::Ok)
    {
        window_ride_construction_update_active_elements();
        return;
    }
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        auto trackDirection = direction_reverse(_currentTrackPieceDirection);
        auto trackPos = _currentTrackBegin;
        if (!(trackDirection & 4))
        {
            trackPos += CoordsDirectionDelta[trackDirection];
        }

        track_begin_end trackBeginEnd;
        if (track_block_get_previous_from_zero(trackPos, ride, trackDirection, &trackBeginEnd))
        {
            _currentTrackBegin.x = trackBeginEnd.begin_x;
            _currentTrackBegin.y = trackBeginEnd.begin_y;
            _currentTrackBegin.z = trackBeginEnd.begin_z;
            _currentTrackPieceDirection = trackBeginEnd.begin_direction;
            _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
            _currentTrackSelectionFlags = 0;
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
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
    int32_t trackType, trackDirection, liftHillAndAlternativeState, properties;
    CoordsXYZ trackPos{};

    _currentTrackPrice = MONEY32_UNDEFINED;
    _trackPlaceCost = MONEY32_UNDEFINED;
    ride_construction_invalidate_current_track();
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, &properties))
    {
        window_ride_construction_update_active_elements();
        return;
    }

    auto trackPlaceAction = TrackPlaceAction(
        rideIndex, trackType, { trackPos, static_cast<uint8_t>(trackDirection) }, (properties)&0xFF, (properties >> 8) & 0x0F,
        (properties >> 12) & 0x0F, liftHillAndAlternativeState, false);
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
    if (res->Error != GameActions::Status::Ok)
    {
        if (auto error = res->ErrorMessage.AsStringId())
            gGameCommandErrorText = *error;
        else
            gGameCommandErrorText = STR_NONE;
        _trackPlaceCost = MONEY32_UNDEFINED;
    }
    else
    {
        gGameCommandErrorText = STR_NONE;
        _trackPlaceCost = res->Cost;
    }

    if (res->Error != GameActions::Status::Ok)
    {
        return;
    }
    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, trackPos);

    if (network_get_mode() != NETWORK_MODE_NONE)
    {
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED;
    }

    auto tpar = dynamic_cast<TrackPlaceActionResult*>(res.get());
    if (tpar != nullptr && tpar->GroundFlags & ELEMENT_IS_UNDERGROUND)
    {
        viewport_set_visibility(1);
    }

    if ((_currentTrackCurve >= (TrackElemType::LeftHalfBankedHelixUpSmall | RideConstructionSpecialPieceSelected)
         && _currentTrackCurve <= (TrackElemType::RightHalfBankedHelixDownLarge | RideConstructionSpecialPieceSelected))
        || (_currentTrackCurve >= (TrackElemType::LeftQuarterBankedHelixLargeUp | RideConstructionSpecialPieceSelected)
            && _currentTrackCurve <= (TrackElemType::RightQuarterHelixLargeDown | RideConstructionSpecialPieceSelected))
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
    int32_t direction;
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
    direction = _currentTrackPieceDirection;
    track_type_t type = _currentTrackPieceType;
    auto newCoords = sub_6C683D({ _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
    if (newCoords == std::nullopt)
    {
        window_ride_construction_update_active_elements();
        return;
    }

    // Get the previous track element to go to after the selected track element is deleted
    inputElement.x = newCoords->x;
    inputElement.y = newCoords->y;
    inputElement.element = tileElement;
    if (track_block_get_previous({ *newCoords, tileElement }, &trackBeginEnd))
    {
        *newCoords = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        direction = trackBeginEnd.begin_direction;
        type = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
        gGotoStartPlacementMode = false;
    }
    else if (track_block_get_next(&inputElement, &outputElement, &newCoords->z, &direction))
    {
        newCoords->x = outputElement.x;
        newCoords->y = outputElement.y;
        direction = outputElement.element->GetDirection();
        type = outputElement.element->AsTrack()->GetTrackType();
        gGotoStartPlacementMode = false;
    }
    else
    {
        direction = _currentTrackPieceDirection;
        type = _currentTrackPieceType;
        newCoords = sub_6C683D({ _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);

        if (newCoords == std::nullopt)
        {
            window_ride_construction_update_active_elements();
            return;
        }

        const rct_preview_track* trackBlock = TrackBlocks[tileElement->AsTrack()->GetTrackType()];
        newCoords->z = (tileElement->GetBaseZ()) - trackBlock->z;
        gGotoStartPlacementMode = true;
    }

    auto trackRemoveAction = TrackRemoveAction(
        _currentTrackPieceType, 0,
        { _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z, _currentTrackPieceDirection });

    trackRemoveAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
        {
            window_ride_construction_update_active_elements();
        }
        else
        {
            auto ride = get_ride(w->number);
            if (ride != nullptr)
            {
                _stationConstructed = ride->num_stations != 0;
                window_ride_construction_mouseup_demolish_next_piece({ *newCoords, static_cast<Direction>(direction) }, type);
            }
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
    if (tool_set(w, WIDX_ENTRANCE, Tool::Crosshair))
    {
        auto ride = get_ride(_currentRideIndex);
        if (ride != nullptr && !ride_try_get_origin_element(ride, nullptr))
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
    if (tool_set(w, WIDX_EXIT, Tool::Crosshair))
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
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    // Close construction window if ride is not closed,
    // editing ride while open will cause many issues until properly handled
    if (ride->status != RIDE_STATUS_CLOSED && ride->status != RIDE_STATUS_SIMULATING)
    {
        window_close(w);
        return;
    }

    switch (_currentTrackCurve)
    {
        case TrackElemType::SpinningTunnel | RideConstructionSpecialPieceSelected:
        case TrackElemType::Whirlpool | RideConstructionSpecialPieceSelected:
        case TrackElemType::Rapids | RideConstructionSpecialPieceSelected:
        case TrackElemType::Waterfall | RideConstructionSpecialPieceSelected:
            widget_invalidate(w, WIDX_CONSTRUCT);
            break;
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE)
    {
        if (!WidgetIsActiveTool(w, WIDX_CONSTRUCT))
        {
            window_close(w);
            return;
        }
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
    {
        if (!WidgetIsActiveTool(w, WIDX_ENTRANCE) && !WidgetIsActiveTool(w, WIDX_EXIT))
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
static std::optional<CoordsXY> ride_get_place_position_from_screen_position(ScreenCoordsXY screenCoords)
{
    CoordsXY mapCoords;

    if (!_trackPlaceCtrlState)
    {
        if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_COPY_Z)
        {
            auto info = get_map_coordinates_from_pos(screenCoords, 0xFCCA);
            if (info.SpriteType != ViewportInteractionItem::None)
            {
                _trackPlaceCtrlZ = info.Element->GetBaseZ();
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
            _trackPlaceShiftStart = screenCoords;
            _trackPlaceShiftZ = 0;
        }
    }
    else
    {
        if (gInputPlaceObjectModifier & PLACE_OBJECT_MODIFIER_SHIFT_Z)
        {
            uint16_t maxHeight = (std::numeric_limits<decltype(TileElement::base_height)>::max() - 32) * ZoomLevel::max();

            _trackPlaceShiftZ = _trackPlaceShiftStart.y - screenCoords.y + 4;
            // Scale delta by zoom to match mouse position.
            auto* mainWnd = window_get_main();
            if (mainWnd && mainWnd->viewport)
            {
                _trackPlaceShiftZ = _trackPlaceShiftZ * mainWnd->viewport->zoom;
            }
            _trackPlaceShiftZ = floor2(_trackPlaceShiftZ, 8);

            // Clamp to maximum possible value of base_height can offer.
            _trackPlaceShiftZ = std::min<int16_t>(_trackPlaceShiftZ, maxHeight);

            screenCoords = _trackPlaceShiftStart;
        }
        else
        {
            _trackPlaceShiftState = false;
        }
    }

    if (!_trackPlaceCtrlState)
    {
        mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
        if (mapCoords.isNull())
            return std::nullopt;

        _trackPlaceZ = 0;
        if (_trackPlaceShiftState)
        {
            auto surfaceElement = map_get_surface_element_at(mapCoords);
            if (surfaceElement == nullptr)
                return std::nullopt;
            auto mapZ = floor2(surfaceElement->GetBaseZ(), 16);
            mapZ += _trackPlaceShiftZ;
            mapZ = std::max<int16_t>(mapZ, 16);
            _trackPlaceZ = mapZ;
        }
    }
    else
    {
        auto mapZ = _trackPlaceCtrlZ;
        auto mapXYCoords = screen_get_map_xy_with_z(screenCoords, mapZ);
        if (mapXYCoords)
        {
            mapCoords = *mapXYCoords;
        }
        else
        {
            return std::nullopt;
        }

        if (_trackPlaceShiftState != 0)
        {
            mapZ += _trackPlaceShiftZ;
        }
        _trackPlaceZ = std::max<int32_t>(mapZ, 16);
    }

    if (mapCoords.x == LOCATION_NULL)
        return std::nullopt;

    return mapCoords.ToTileStart();
}

/**
 *
 *  rct2: 0x006C8229
 */
static void window_ride_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CONSTRUCT:
            ride_construction_toolupdate_construct(screenCoords);
            break;
        case WIDX_ENTRANCE:
        case WIDX_EXIT:
            ride_construction_toolupdate_entrance_exit(screenCoords);
            break;
    }
}

/**
 *
 *  rct2: 0x006C8248
 */
static void window_ride_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_CONSTRUCT:
            ride_construction_tooldown_construct(screenCoords);
            break;
        case WIDX_ENTRANCE:
        case WIDX_EXIT:
            ride_construction_tooldown_entrance_exit(screenCoords);
            break;
    }
}

/**
 *
 *  rct2: 0x006C6AD5
 */
static void window_ride_construction_invalidate(rct_window* w)
{
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
    {
        return;
    }

    rct_string_id stringId = STR_RIDE_CONSTRUCTION_SPECIAL;
    if (_currentTrackCurve & RideConstructionSpecialPieceSelected)
    {
        stringId = RideConfigurationStringIds[_currentTrackCurve & ~RideConstructionSpecialPieceSelected];
        if (stringId == STR_RAPIDS && (ride->type == RIDE_TYPE_MONSTER_TRUCKS || ride->type == RIDE_TYPE_CAR_RIDE))
        {
            stringId = STR_LOG_BUMPS;
        }
    }
    auto ft = Formatter::Common();
    ft.Add<uint16_t>(stringId);

    if (_currentlyShowingBrakeOrBoosterSpeed)
    {
        uint16_t brakeSpeed2 = ((_currentBrakeSpeed2 * 9) >> 2) & 0xFFFF;
        if (_selectedTrackType == TrackElemType::Booster
            || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::Booster))
        {
            brakeSpeed2 = get_booster_speed(ride->type, brakeSpeed2);
        }
        ft.Add<uint16_t>(brakeSpeed2);
    }

    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].text = RideConstructionSeatAngleRotationStrings
        [_currentSeatRotationAngle];

    // Simulate button
    auto& simulateWidget = w->widgets[WIDX_SIMULATE];
    simulateWidget.type = WindowWidgetType::Empty;
    if (ride->SupportsStatus(RIDE_STATUS_SIMULATING))
    {
        simulateWidget.type = WindowWidgetType::FlatBtn;
        if (ride->status == RIDE_STATUS_SIMULATING)
        {
            w->pressed_widgets |= (1ULL << WIDX_SIMULATE);
        }
        else
        {
            w->pressed_widgets &= ~(1ULL << WIDX_SIMULATE);
        }
    }

    // Set window title arguments
    ft = Formatter::Common();
    ft.Increment(4);
    ride->FormatNameTo(ft);
}

/**
 *
 *  rct2: 0x006C6B86
 */
static void window_ride_construction_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_drawpixelinfo clipdpi;
    rct_widget* widget;
    int32_t width, height;

    WindowDrawWidgets(w, dpi);

    widget = &window_ride_construction_widgets[WIDX_CONSTRUCT];
    if (widget->type == WindowWidgetType::Empty)
        return;

    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndInvertedState;
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndInvertedState, nullptr, nullptr))
        return;

    // Draw track piece
    auto screenCoords = ScreenCoordsXY{ w->windowPos.x + widget->left + 1, w->windowPos.y + widget->top + 1 };
    width = widget->width() - 1;
    height = widget->height() - 1;
    if (clip_drawpixelinfo(&clipdpi, dpi, screenCoords, width, height))
    {
        window_ride_construction_draw_track_piece(
            w, &clipdpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, width, height);
    }

    // Draw cost
    screenCoords = { w->windowPos.x + widget->midX(), w->windowPos.y + widget->bottom - 23 };
    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_PLACE)
        DrawTextBasic(dpi, screenCoords, STR_BUILD_THIS, w, { TextAlignment::CENTRE });

    screenCoords.y += 11;
    if (_currentTrackPrice != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        DrawTextBasic(dpi, screenCoords, STR_COST_LABEL, static_cast<void*>(&_currentTrackPrice), { TextAlignment::CENTRE });
    }
}

static void window_ride_construction_draw_track_piece(
    rct_window* w, rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection,
    int32_t liftHillAndInvertedState, int32_t width, int32_t height)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    auto trackBlock = TrackBlocks[trackType];
    while ((trackBlock + 1)->index != 0xFF)
        trackBlock++;

    CoordsXYZ mapCoords{ trackBlock->x, trackBlock->y, trackBlock->z };
    if (trackBlock->flags & RCT_PREVIEW_TRACK_FLAG_1)
    {
        mapCoords.x = 0;
        mapCoords.y = 0;
    }

    auto rotatedMapCoords = mapCoords.Rotate(trackDirection);
    // this is actually case 0, but the other cases all jump to it
    mapCoords.x = 4112 + (rotatedMapCoords.x / 2);
    mapCoords.y = 4112 + (rotatedMapCoords.y / 2);
    mapCoords.z = 1024 + mapCoords.z;

    int16_t previewZOffset = TrackDefinitions[trackType].preview_z_offset;
    mapCoords.z -= previewZOffset;

    const ScreenCoordsXY rotatedScreenCoords = translate_3d_to_2d_with_z(get_current_rotation(), mapCoords);

    dpi->x += rotatedScreenCoords.x - width / 2;
    dpi->y += rotatedScreenCoords.y - height / 2 - 16;

    sub_6CBCE2(dpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, { 4096, 4096 }, 1024);
}

static TileElement _tempTrackTileElement;
static TileElement _tempSideTrackTileElement = { 0x80, 0x8F, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
static TileElement* _backupTileElementArrays[5];

/**
 *
 *  rct2: 0x006CBCE2
 * bh: trackDirection
 * dl: rideIndex
 * dh: trackType
 */
static void sub_6CBCE2(
    rct_drawpixelinfo* dpi, ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndInvertedState,
    const CoordsXY& originCoords, int32_t originZ)
{
    paint_session* session = PaintSessionAlloc(dpi, 0);
    trackDirection &= 3;

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return;

    int16_t preserveMapSizeUnits = gMapSizeUnits;
    int16_t preserveMapSizeMinus2 = gMapSizeMinus2;
    int16_t preserveMapSize = gMapSize;
    int16_t preserveMapSizeMaxXY = gMapSizeMaxXY;

    gMapSizeUnits = MAXIMUM_TILE_START_XY;
    gMapSizeMinus2 = ((MAXIMUM_MAP_SIZE_TECHNICAL - 1) * 32) + 286;
    gMapSize = MAXIMUM_MAP_SIZE_TECHNICAL;
    gMapSizeMaxXY = MAXIMUM_MAP_SIZE_BIG - 1;

    auto trackBlock = TrackBlocks[trackType];
    while (trackBlock->index != 255)
    {
        auto quarterTile = trackBlock->var_08.Rotate(trackDirection);
        CoordsXY offsets = { trackBlock->x, trackBlock->y };
        CoordsXY coords = originCoords + offsets.Rotate(trackDirection);

        int32_t baseZ = originZ + trackBlock->z;
        int32_t clearanceZ = trackBlock->var_07 + ride->GetRideTypeDescriptor().Heights.ClearanceHeight + baseZ
            + (4 * COORDS_Z_STEP);

        auto centreTileCoords = TileCoordsXY{ coords };
        auto eastTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_EAST];
        auto westTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_WEST];
        auto northTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_NORTH];
        auto southTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_SOUTH];

        // Replace map elements with temporary ones containing track
        _backupTileElementArrays[0] = map_get_first_element_at(centreTileCoords.ToCoordsXY());
        _backupTileElementArrays[1] = map_get_first_element_at(eastTileCoords.ToCoordsXY());
        _backupTileElementArrays[2] = map_get_first_element_at(westTileCoords.ToCoordsXY());
        _backupTileElementArrays[3] = map_get_first_element_at(northTileCoords.ToCoordsXY());
        _backupTileElementArrays[4] = map_get_first_element_at(southTileCoords.ToCoordsXY());
        map_set_tile_element(centreTileCoords, &_tempTrackTileElement);
        map_set_tile_element(eastTileCoords, &_tempSideTrackTileElement);
        map_set_tile_element(westTileCoords, &_tempSideTrackTileElement);
        map_set_tile_element(northTileCoords, &_tempSideTrackTileElement);
        map_set_tile_element(southTileCoords, &_tempSideTrackTileElement);

        // Set the temporary track element
        _tempTrackTileElement.SetType(TILE_ELEMENT_TYPE_TRACK);
        _tempTrackTileElement.SetDirection(trackDirection);
        _tempTrackTileElement.AsTrack()->SetHasChain((liftHillAndInvertedState & CONSTRUCTION_LIFT_HILL_SELECTED) != 0);
        _tempTrackTileElement.SetOccupiedQuadrants(quarterTile.GetBaseQuarterOccupied());
        _tempTrackTileElement.SetLastForTile(true);
        _tempTrackTileElement.SetBaseZ(baseZ);
        _tempTrackTileElement.SetClearanceZ(clearanceZ);
        _tempTrackTileElement.AsTrack()->SetTrackType(trackType);
        _tempTrackTileElement.AsTrack()->SetSequenceIndex(trackBlock->index);
        _tempTrackTileElement.AsTrack()->SetHasCableLift(false);
        _tempTrackTileElement.AsTrack()->SetInverted((liftHillAndInvertedState & CONSTRUCTION_INVERTED_TRACK_SELECTED) != 0);
        _tempTrackTileElement.AsTrack()->SetColourScheme(RIDE_COLOUR_SCHEME_MAIN);
        // Skipping seat rotation, should not be necessary for a temporary piece.
        _tempTrackTileElement.AsTrack()->SetRideIndex(rideIndex);

        // Draw this map tile
        sub_68B2B7(session, coords);

        // Restore map elements
        map_set_tile_element(centreTileCoords, _backupTileElementArrays[0]);
        map_set_tile_element(eastTileCoords, _backupTileElementArrays[1]);
        map_set_tile_element(westTileCoords, _backupTileElementArrays[2]);
        map_set_tile_element(northTileCoords, _backupTileElementArrays[3]);
        map_set_tile_element(southTileCoords, _backupTileElementArrays[4]);

        trackBlock++;
    }

    gMapSizeUnits = preserveMapSizeUnits;
    gMapSizeMinus2 = preserveMapSizeMinus2;
    gMapSize = preserveMapSize;
    gMapSizeMaxXY = preserveMapSizeMaxXY;

    PaintSessionArrange(session);
    PaintDrawStructs(session);
    PaintSessionFree(session);
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

    _selectedTrackType = TrackElemType::None;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_SELECTED)
    {
        if (sub_6C683D(
                { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
                &tileElement, 0)
            != std::nullopt)
        {
            _selectedTrackType = tileElement->AsTrack()->GetTrackType();
            if (TrackTypeHasSpeedSetting(tileElement->AsTrack()->GetTrackType()))
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
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    int32_t rideType = ride_get_alternative_type(ride);
    _enabledRidePieces = GetRideTypeDescriptor(rideType).GetAvailableTrackPieces();
}

/**
 *
 *  rct2: 0x006C94D8
 */
void sub_6C94D8()
{
    ride_id_t rideIndex;
    int32_t direction, type, liftHillAndAlternativeState;
    CoordsXYZ trackPos{};

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
        {
            if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
            {
                if (window_ride_construction_update_state(
                        &type, &direction, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr))
                {
                    ride_construction_remove_ghosts();
                }
                else
                {
                    _currentTrackPrice = place_provisional_track_piece(
                        rideIndex, type, direction, liftHillAndAlternativeState, trackPos);
                    window_ride_construction_update_active_elements();
                }
            }

            auto curTime = platform_get_ticks();
            if (_rideConstructionNextArrowPulse >= curTime)
                break;
            _rideConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            trackPos = _currentTrackBegin;
            direction = _currentTrackPieceDirection;
            type = _currentTrackPieceType;
            if (direction >= 4)
                direction += 4;
            if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
                direction = direction_reverse(direction);
            gMapSelectArrowPosition = trackPos;
            gMapSelectArrowDirection = direction;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
            map_invalidate_tile_full(trackPos);
            break;
        }
        case RIDE_CONSTRUCTION_STATE_SELECTED:
        {
            auto curTime = platform_get_ticks();
            if (_rideConstructionNextArrowPulse >= curTime)
                break;
            _rideConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            direction = _currentTrackPieceDirection & 3;
            type = _currentTrackPieceType;
            uint16_t flags = _currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW ? TRACK_ELEMENT_SET_HIGHLIGHT_TRUE
                                                                                      : TRACK_ELEMENT_SET_HIGHLIGHT_FALSE;
            auto newCoords = sub_6C683D({ _currentTrackBegin, static_cast<Direction>(direction) }, type, 0, nullptr, flags);
            if (newCoords == std::nullopt)
            {
                ride_construction_remove_ghosts();
                _rideConstructionState = RIDE_CONSTRUCTION_STATE_0;
            }
            break;
        }
        case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
        case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
        case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
        {
            auto curTime = platform_get_ticks();
            if (_rideConstructionNextArrowPulse >= curTime)
                break;
            _rideConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

            _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
            trackPos = CoordsXYZ{ _currentTrackBegin.x & 0xFFE0, _currentTrackBegin.y & 0xFFE0, _currentTrackBegin.z + 15 };
            gMapSelectArrowPosition = trackPos;
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
            map_invalidate_tile_full(trackPos);
            break;
        }
    }
}

/**
 *
 *  rct2: 0x006C84E2
 */
static void window_ride_construction_update_map_selection()
{
    int32_t trackType, trackDirection;
    CoordsXYZ trackPos{};

    map_invalidate_map_selection_tiles();
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_0:
            trackDirection = _currentTrackPieceDirection;
            trackType = 0;
            trackPos = _currentTrackBegin;
            break;
        case RIDE_CONSTRUCTION_STATE_SELECTED:
            trackDirection = _currentTrackPieceDirection;
            trackType = _currentTrackPieceType;
            trackPos = _currentTrackBegin;
            break;
        case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
            gMapSelectionTiles.clear();
            return;
        default:
            if (window_ride_construction_update_state(&trackType, &trackDirection, nullptr, nullptr, &trackPos, nullptr))
            {
                trackDirection = _currentTrackPieceDirection;
                trackType = 0;
                trackPos = _currentTrackBegin;
            }
            break;
    }

    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        window_ride_construction_select_map_tiles(ride, trackType, trackDirection, trackPos);
        map_invalidate_map_selection_tiles();
    }
}

/**
 *
 *  rct2: 0x006C8648
 */
static void window_ride_construction_update_possible_ride_configurations()
{
    int32_t trackType;

    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    _currentlyShowingBrakeOrBoosterSpeed = false;

    int32_t currentPossibleRideConfigurationIndex = 0;
    _numCurrentPossibleSpecialTrackPieces = 0;
    for (trackType = 0; trackType < TrackElemType::Count; trackType++)
    {
        int32_t trackTypeCategory = TrackDefinitions[trackType].type;

        if (trackTypeCategory == TRACK_NONE)
            continue;

        if (!is_track_enabled(trackTypeCategory))
        {
            continue;
        }

        int32_t slope, bank;
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_FRONT || _rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE)
        {
            slope = TrackDefinitions[trackType].vangle_start;
            bank = TrackDefinitions[trackType].bank_start;
        }
        else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
        {
            slope = TrackDefinitions[trackType].vangle_end;
            bank = TrackDefinitions[trackType].bank_end;
        }
        else
        {
            continue;
        }

        if (!ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
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
            && (trackType != TrackElemType::TowerBase
                || ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_EXTRA_TOWER_BASES)))
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
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    int32_t rideType = ride_get_alternative_type(ride);

    w->hold_down_widgets = 0;
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_IS_SHOP) || !_stationConstructed)
    {
        window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WindowWidgetType::Empty;
        window_ride_construction_widgets[WIDX_ENTRANCE].type = WindowWidgetType::Empty;
        window_ride_construction_widgets[WIDX_EXIT].type = WindowWidgetType::Empty;
    }
    else
    {
        window_ride_construction_widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WindowWidgetType::Groupbox;
        window_ride_construction_widgets[WIDX_ENTRANCE].type = WindowWidgetType::Button;
        window_ride_construction_widgets[WIDX_EXIT].type = WindowWidgetType::Button;
    }

    if (_numCurrentPossibleSpecialTrackPieces == 0)
    {
        window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Empty;
    }
    else
    {
        window_ride_construction_widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Button;
    }

    if (is_track_enabled(TRACK_STRAIGHT))
    {
        window_ride_construction_widgets[WIDX_STRAIGHT].type = WindowWidgetType::FlatBtn;
    }
    else
    {
        window_ride_construction_widgets[WIDX_STRAIGHT].type = WindowWidgetType::Empty;
    }

    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_HAS_LARGE_CURVES))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_LARGE].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_LARGE].type = WindowWidgetType::FlatBtn;
    }
    else
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_LARGE].type = WindowWidgetType::Empty;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_LARGE].type = WindowWidgetType::Empty;
    }

    window_ride_construction_widgets[WIDX_LEFT_CURVE].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 28;
    window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 49;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 116;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 137;
    window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE;
    window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE;
    if (is_track_enabled(TRACK_CURVE_VERTICAL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
    }
    if (is_track_enabled(TRACK_CURVE))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
    }
    if (is_track_enabled(TRACK_CURVE_SMALL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
    }
    if (is_track_enabled(TRACK_CURVE_VERY_SMALL))
    {
        window_ride_construction_widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
    }

    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_LEVEL].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SLOPE_UP].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP;
    window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
    if (GetRideTypeDescriptor(rideType).SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
    {
        window_ride_construction_widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
    }
    if (is_track_enabled(TRACK_SLOPE) || is_track_enabled(TRACK_SLOPE_STEEP))
    {
        window_ride_construction_widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
    }
    if (is_track_enabled(TRACK_SLOPE))
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
    }
    if (is_track_enabled(TRACK_HELIX_SMALL) && _currentTrackBankEnd != TRACK_BANK_NONE
        && _currentTrackSlopeEnd == TRACK_SLOPE_NONE)
    {
        if (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL)
        {
            // Enable helix
            window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
            if (rideType != RIDE_TYPE_SPLASH_BOATS && rideType != RIDE_TYPE_RIVER_RAFTS)
                window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
        }
    }

    if (is_track_enabled(TRACK_SLOPE_STEEP))
    {
        window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
        if (rideType != RIDE_TYPE_SPLASH_BOATS && rideType != RIDE_TYPE_RIVER_RAFTS)
            window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
    }

    int32_t x;
    if ((is_track_enabled(TRACK_LIFT_HILL) && (_currentTrackCurve & RideConstructionSpecialPieceSelected) == 0)
        || (gCheatsEnableChainLiftOnAllTrack && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK)))
    {
        window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::FlatBtn;
        x = 9;
    }
    else
    {
        window_ride_construction_widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::Empty;
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
    window_ride_construction_widgets[WIDX_BANK_LEFT].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_U_TRACK].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_O_TRACK].type = WindowWidgetType::Empty;

    bool brakesSelected = _selectedTrackType == TrackElemType::Brakes
        || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::Brakes);
    bool boosterTrackSelected = _selectedTrackType == TrackElemType::Booster
        || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::Booster);

    if (!brakesSelected && !boosterTrackSelected)
    {
        if (is_track_enabled(TRACK_FLAT_ROLL_BANKING))
        {
            window_ride_construction_widgets[WIDX_BANK_LEFT].type = WindowWidgetType::FlatBtn;
            window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::FlatBtn;
            window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::FlatBtn;
        }
        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
        {
            if (rideType == RIDE_TYPE_WATER_COASTER)
            {
                window_ride_construction_widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_RC_TRACK;
                window_ride_construction_widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_WATER_CHANNEL;
                window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_STANDARD_RC_TRACK_TIP;
                window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_WATER_CHANNEL_TIP;
                if ((_currentTrackCurve < TRACK_CURVE_LEFT_SMALL
                     || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::SBendLeft)
                     || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::SBendRight))
                    && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_BANK_NONE)
                {
                    window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                    window_ride_construction_widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
                    window_ride_construction_widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;
                }
            }
            else
            {
                window_ride_construction_widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK;
                window_ride_construction_widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK;
                window_ride_construction_widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP;
                window_ride_construction_widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP;
                window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                window_ride_construction_widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
                window_ride_construction_widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;
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

        window_ride_construction_widgets[WIDX_BANK_LEFT].type = WindowWidgetType::Spinner;
        window_ride_construction_widgets[WIDX_BANK_LEFT].left = 12;
        window_ride_construction_widgets[WIDX_BANK_LEFT].right = 96;
        window_ride_construction_widgets[WIDX_BANK_LEFT].top = 138;
        window_ride_construction_widgets[WIDX_BANK_LEFT].bottom = 149;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::Button;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].text = STR_NUMERIC_UP;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].left = 84;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].right = 95;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].top = 139;
        window_ride_construction_widgets[WIDX_BANK_STRAIGHT].bottom = 148;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::Button;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].text = STR_NUMERIC_DOWN;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].left = 72;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].right = 83;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].top = 139;
        window_ride_construction_widgets[WIDX_BANK_RIGHT].bottom = 148;
        w->hold_down_widgets |= (1 << WIDX_BANK_STRAIGHT) | (1 << WIDX_BANK_RIGHT);
    }

    window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 162;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Empty;
    if ((rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER || rideType == RIDE_TYPE_MULTI_DIMENSION_ROLLER_COASTER_ALT)
        && _selectedTrackType != TrackElemType::Brakes
        && _currentTrackCurve != (RideConstructionSpecialPieceSelected | TrackElemType::Brakes))
    {
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Groupbox;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Spinner;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Button;
        window_ride_construction_widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Button;
        window_ride_construction_widgets[WIDX_BANKING_GROUPBOX].right = 92;
        if (window_ride_construction_widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Spinner)
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

    window_ride_construction_widgets[WIDX_CONSTRUCT].type = WindowWidgetType::Empty;
    window_ride_construction_widgets[WIDX_DEMOLISH].type = WindowWidgetType::FlatBtn;
    window_ride_construction_widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
    if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
    {
        window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
        window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
    }
    else
    {
        window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::FlatBtn;
        window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::FlatBtn;
    }

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_FRONT:
            window_ride_construction_widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
            window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
            break;
        case RIDE_CONSTRUCTION_STATE_BACK:
            window_ride_construction_widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
            window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
            break;
        case RIDE_CONSTRUCTION_STATE_PLACE:
            window_ride_construction_widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
            window_ride_construction_widgets[WIDX_DEMOLISH].type = WindowWidgetType::Empty;
            window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
            window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
            window_ride_construction_widgets[WIDX_ROTATE].type = WindowWidgetType::FlatBtn;
            break;
        case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
            window_ride_construction_widgets[WIDX_DEMOLISH].type = WindowWidgetType::Empty;
            window_ride_construction_widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
            window_ride_construction_widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
            break;
        default:
            w->pressed_widgets = pressedWidgets;
            w->Invalidate();
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
        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
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
    w->Invalidate();
}

static void window_ride_construction_select_map_tiles(
    Ride* ride, int32_t trackType, int32_t trackDirection, const CoordsXY& tileCoords)
{
    // If the scenery tool is active, we do not display our tiles as it
    // will conflict with larger scenery objects selecting tiles
    if (scenery_tool_is_active())
    {
        return;
    }

    const rct_preview_track* trackBlock;

    trackBlock = TrackBlocks[trackType];
    trackDirection &= 3;
    gMapSelectionTiles.clear();
    while (trackBlock->index != 255)
    {
        CoordsXY offsets = { trackBlock->x, trackBlock->y };
        CoordsXY currentTileCoords = tileCoords + offsets.Rotate(trackDirection);

        gMapSelectionTiles.push_back(currentTileCoords);
        trackBlock++;
    }
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
        track_type_t trackPiece = _currentPossibleRideConfigurations[i];
        rct_string_id trackPieceStringId = RideConfigurationStringIds[trackPiece];
        if (trackPieceStringId == STR_RAPIDS)
        {
            auto ride = get_ride(_currentRideIndex);
            if (ride != nullptr && (ride->type == RIDE_TYPE_MONSTER_TRUCKS || ride->type == RIDE_TYPE_CAR_RIDE))
                trackPieceStringId = STR_LOG_BUMPS;
        }
        gDropdownItemsFormat[i] = trackPieceStringId;
        if ((trackPiece | RideConstructionSpecialPieceSelected) == _currentTrackCurve)
        {
            defaultIndex = i;
        }
    }

    WindowDropdownShowTextCustomWidth(
        { w->windowPos.x + widget->left, w->windowPos.y + widget->top }, widget->height() + 1, w->colours[1], 0, 0,
        _numCurrentPossibleRideConfigurations, widget->width());

    for (int32_t i = 0; i < 32; i++)
    {
        if (_currentDisabledSpecialTrackPieces & (1 << i))
        {
            Dropdown::SetDisabled(i, true);
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
    sub_6C683D(
        { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, seatRotation,
        nullptr, TRACK_ELEMENT_SET_SEAT_ROTATION);
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

    if (sub_6C683D(
            { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
            &tileElement, 0)
        != std::nullopt)
    {
        auto trackSetBrakeSpeed = TrackSetBrakeSpeedAction(
            _currentTrackBegin, tileElement->AsTrack()->GetTrackType(), brakesSpeed);
        trackSetBrakeSpeed.SetCallback(
            [](const GameAction* ga, const GameActions::Result* result) { window_ride_construction_update_active_elements(); });
        GameActions::Execute(&trackSetBrakeSpeed);
        return;
    }
    window_ride_construction_update_active_elements();
}

/**
 *
 *  rct2: 0x006CC6A8
 */
void ride_construction_toolupdate_construct(const ScreenCoordsXY& screenCoords)
{
    int32_t z;
    const rct_preview_track* trackBlock;

    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    auto mapCoords = ride_get_place_position_from_screen_position(screenCoords);
    if (!mapCoords)
    {
        ride_construction_invalidate_current_track();
        map_invalidate_map_selection_tiles();
        return;
    }

    z = _trackPlaceZ;
    if (z == 0)
        z = map_get_highest_z(*mapCoords);

    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
    gMapSelectArrowPosition = CoordsXYZ{ *mapCoords, z };
    gMapSelectArrowDirection = _currentTrackPieceDirection;
    gMapSelectionTiles.clear();
    gMapSelectionTiles.push_back(*mapCoords);

    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState;
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, nullptr, nullptr))
    {
        ride_construction_invalidate_current_track();
        map_invalidate_map_selection_tiles();
        return;
    }
    _currentTrackPieceType = trackType;
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    // Re-using this other code, very slight difference from original
    //   - Original code checks for MSB mask instead of 255 on trackPart->var_00
    //   - Original code checks this first as its already set origin tile, probably just a micro optimisation
    window_ride_construction_select_map_tiles(ride, trackType, trackDirection, *mapCoords);

    gMapSelectArrowPosition.z = z;
    if (_trackPlaceZ == 0)
    {
        // Raise z above all slopes and water
        if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
        {
            int32_t highestZ = 0;
            for (const auto& selectedTile : gMapSelectionTiles)
            {
                if (map_is_location_valid(selectedTile))
                {
                    z = map_get_highest_z(selectedTile);
                    if (z > highestZ)
                        highestZ = z;
                }
            }
        }
        // loc_6CC8BF:
        // z = map_get_highest_z(x >> 5, y >> 5);
    }
    // loc_6CC91B:
    trackBlock = TrackBlocks[trackType];
    int32_t bx = 0;
    do
    {
        bx = std::min<int32_t>(bx, trackBlock->z);
        trackBlock++;
    } while (trackBlock->index != 255);
    z -= bx;

    gMapSelectArrowPosition.z = z;
    _currentTrackBegin.x = mapCoords->x;
    _currentTrackBegin.y = mapCoords->y;
    _currentTrackBegin.z = z;
    if ((_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK) && _currentTrackBegin == _previousTrackPiece)
    {
        map_invalidate_map_selection_tiles();
        return;
    }

    _previousTrackPiece = _currentTrackBegin;
    // search for appropriate z value for ghost, up to max ride height
    int numAttempts = (z <= MAX_TRACK_HEIGHT ? ((MAX_TRACK_HEIGHT - z) / COORDS_Z_STEP + 1) : 2);

    if (ride->type == RIDE_TYPE_MAZE)
    {
        for (int zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
        {
            CoordsXYZ trackPos{};
            window_ride_construction_update_state(
                &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
            _currentTrackPrice = place_provisional_track_piece(
                rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
            z = trackPos.z;
            if (_currentTrackPrice != MONEY32_UNDEFINED)
                break;

            _currentTrackBegin.z -= 8;
            if (_currentTrackBegin.z < 0)
                break;

            _currentTrackBegin.z += 16;
        }

        auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
        context_broadcast_intent(&intent);
        map_invalidate_map_selection_tiles();
        return;
    }

    for (int zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
    {
        CoordsXYZ trackPos{};
        window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
        _currentTrackPrice = place_provisional_track_piece(
            rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
        mapCoords = trackPos;
        z = trackPos.z;
        if (_currentTrackPrice != MONEY32_UNDEFINED)
            break;

        _currentTrackBegin.z -= 8;
        if (_currentTrackBegin.z < 0)
            break;

        _currentTrackBegin.z += 16;
    }

    if (_autoRotatingShop && _rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE
        && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP))
    {
        TileElement* pathsByDir[NumOrthogonalDirections];

        bool keepOrientation = false;
        for (int8_t i = 0; i < NumOrthogonalDirections; i++)
        {
            pathsByDir[i] = map_get_footpath_element({ *mapCoords + CoordsDirectionDelta[i], z });

            if (pathsByDir[i] && (pathsByDir[i])->AsPath()->IsSloped() && (pathsByDir[i])->AsPath()->GetSlopeDirection() != i)
            {
                pathsByDir[i] = nullptr;
            }

            // Sloped path on the level below
            if (!pathsByDir[i])
            {
                pathsByDir[i] = map_get_footpath_element({ *mapCoords + CoordsDirectionDelta[i], z - PATH_HEIGHT_STEP });

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

                    CoordsXYZ trackPos{};
                    window_ride_construction_update_state(
                        &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
                    place_provisional_track_piece(rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
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
void ride_construction_toolupdate_entrance_exit(const ScreenCoordsXY& screenCoords)
{
    map_invalidate_selection_rect();
    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    CoordsXYZD entranceOrExitCoords = ride_get_entrance_or_exit_position_from_screen_position(screenCoords);
    if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
    {
        ride_construction_invalidate_current_track();
        return;
    }
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
    gMapSelectType = MAP_SELECT_TYPE_FULL;
    gMapSelectPositionA = entranceOrExitCoords;
    gMapSelectPositionB = entranceOrExitCoords;
    gMapSelectArrowPosition = entranceOrExitCoords;
    gMapSelectArrowDirection = direction_reverse(entranceOrExitCoords.direction);
    map_invalidate_selection_rect();

    entranceOrExitCoords.direction = direction_reverse(gRideEntranceExitPlaceDirection);
    StationIndex stationNum = gRideEntranceExitPlaceStationIndex;
    if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
        || entranceOrExitCoords != gRideEntranceExitGhostPosition || stationNum != gRideEntranceExitGhostStationIndex)
    {
        auto ride = get_ride(_currentRideIndex);
        if (ride != nullptr)
        {
            _currentTrackPrice = ride_entrance_exit_place_ghost(
                ride, entranceOrExitCoords, entranceOrExitCoords.direction, gRideEntranceExitPlaceType, stationNum);
        }
        window_ride_construction_update_active_elements();
    }
}

/**
 *
 *  rct2: 0x006CCA73
 */
void ride_construction_tooldown_construct(const ScreenCoordsXY& screenCoords)
{
    const CursorState* state = context_get_cursor_state();
    ride_id_t rideIndex;
    int32_t trackType, trackDirection, liftHillAndAlternativeState, z, properties, highestZ;
    rct_window* w;

    map_invalidate_map_selection_tiles();
    ride_construction_invalidate_current_track();

    CoordsXYZ mapCoords{};
    if (window_ride_construction_update_state(
            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &mapCoords, &properties))
        return;

    z = mapCoords.z;
    _currentTrackPieceType = trackType;

    // Raise z above all slopes and water
    highestZ = 0;
    if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
    {
        for (const auto& selectedTile : gMapSelectionTiles)
        {
            if (!map_is_location_valid(selectedTile))
                continue;

            z = map_get_highest_z(selectedTile);
            if (z > highestZ)
                highestZ = z;
        }
    }

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    auto ridePlacePosition = ride_get_place_position_from_screen_position(screenCoords);
    if (!ridePlacePosition)
        return;

    mapCoords = { *ridePlacePosition, z };

    z = _trackPlaceZ;
    if (z == 0)
        z = map_get_highest_z(mapCoords);

    tool_cancel();

    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    if (_trackPlaceZ == 0)
    {
        const rct_preview_track* trackBlock = TrackBlocks[_currentTrackPieceType];
        int32_t bx = 0;
        do
        {
            bx = std::min<int32_t>(bx, trackBlock->z);
            trackBlock++;
        } while (trackBlock->index != 255);
        z -= bx;

        // FIX not sure exactly why it starts trial and error place from a lower Z, but it causes issues with disable clearance
        if (!gCheatsDisableClearanceChecks && z > MINIMUM_LAND_HEIGHT_BIG)
        {
            z -= LAND_HEIGHT_STEP;
        }
    }
    else
    {
        z = _trackPlaceZ;
    }

    // search for z value to build at, up to max ride height
    int numAttempts = (z <= MAX_TRACK_HEIGHT ? ((MAX_TRACK_HEIGHT - z) / COORDS_Z_STEP + 1) : 2);

    if (ride->type == RIDE_TYPE_MAZE)
    {
        for (int32_t zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
        {
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_MAZE_BUILD;
            _currentTrackBegin.x = mapCoords.x;
            _currentTrackBegin.y = mapCoords.y;
            _currentTrackBegin.z = z;
            _currentTrackSelectionFlags = 0;
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
                    || zAttempts == (numAttempts - 1) || z < 0)
                {
                    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, state->position.x);
                    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
                    if (w != nullptr)
                    {
                        tool_set(w, WIDX_CONSTRUCT, Tool::Crosshair);
                        input_set_flag(INPUT_FLAG_6, true);
                        _trackPlaceCtrlState = false;
                        _trackPlaceShiftState = false;
                    }
                    auto intent2 = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
                    context_broadcast_intent(&intent2);
                    break;
                }
                z += 16;
            }
            else
            {
                window_close_by_class(WC_ERROR);
                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, _currentTrackBegin);
                break;
            }
        }
        return;
    }

    for (int32_t zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
    {
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
        _currentTrackBegin.x = mapCoords.x;
        _currentTrackBegin.y = mapCoords.y;
        _currentTrackBegin.z = z;
        _currentTrackSelectionFlags = 0;
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
                || errorText == STR_TOO_HIGH_FOR_SUPPORTS || errorText == STR_TOO_HIGH
                || errorText == STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT || zAttempts == (numAttempts - 1)
                || z < 0)
            {
                int32_t saveTrackDirection = _currentTrackPieceDirection;
                auto saveCurrentTrackCurve = _currentTrackCurve;
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

                OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, state->position.x);
                break;
            }

            z += 16;
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
static void ride_construction_tooldown_entrance_exit(const ScreenCoordsXY& screenCoords)
{
    ride_construction_invalidate_current_track();
    map_invalidate_selection_rect();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    CoordsXYZD entranceOrExitCoords = ride_get_entrance_or_exit_position_from_screen_position(screenCoords);
    if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
        return;

    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        entranceOrExitCoords, direction_reverse(gRideEntranceExitPlaceDirection), gRideEntranceExitPlaceRideIndex,
        gRideEntranceExitPlaceStationIndex, gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

    rideEntranceExitPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;

        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

        auto ride = get_ride(gRideEntranceExitPlaceRideIndex);
        if (ride != nullptr && ride_are_all_possible_entrances_and_exits_built(ride))
        {
            tool_cancel();
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
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
    if (w == nullptr || WidgetIsDisabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_LEFT_SMALL:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_LEFT:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_LARGE:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_NONE:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_LARGE:
            if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_SMALL:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_VERY_SMALL:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_RIGHT_SMALL:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_RIGHT:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_RIGHT_LARGE:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_NONE:
            if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_LARGE:
            if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_SMALL:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            else
            {
                return;
            }
            break;
        case TRACK_CURVE_LEFT_VERY_SMALL:
            if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    if (!WidgetIsDisabled(w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_STRAIGHT);
    }

    if (!WidgetIsDisabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_LEVEL);
    }

    if (!WidgetIsDisabled(w, WIDX_CHAIN_LIFT) && w->widgets[WIDX_CHAIN_LIFT].type != WindowWidgetType::Empty
        && _currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
    {
        window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
    }

    if (!WidgetIsDisabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
    }
}

void window_ride_construction_keyboard_shortcut_slope_down()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_DOWN_60:
            if (is_track_enabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP)
                && window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
                && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                is_track_enabled(TRACK_SLOPE_VERTICAL)
                && window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_UP_25:
            if (!WidgetIsDisabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_UP_60:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                is_track_enabled(TRACK_SLOPE_VERTICAL)
                && window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_UP_90:
            if (is_track_enabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP)
                && window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_slope_up()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_UP_60:
            if (is_track_enabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && window_ride_construction_widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        case TRACK_SLOPE_UP_25:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                is_track_enabled(TRACK_SLOPE_VERTICAL)
                && window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            if (!WidgetIsDisabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_60:
            if (!WidgetIsDisabled(w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!WidgetIsDisabled(w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                is_track_enabled(TRACK_SLOPE_VERTICAL)
                && window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_90:
            if (is_track_enabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(w, WIDX_SLOPE_DOWN_STEEP)
                && window_ride_construction_widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        default:
            return;
    }
}

void window_ride_construction_keyboard_shortcut_chain_lift_toggle()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_CHAIN_LIFT) || w->widgets[WIDX_CHAIN_LIFT].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
}

void window_ride_construction_keyboard_shortcut_bank_left()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_BANK_STRAIGHT)
        || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!WidgetIsDisabled(w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_LEFT);
            }
            break;
        case TRACK_BANK_RIGHT:
            if (!WidgetIsDisabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!WidgetIsDisabled(w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(w, WIDX_BANK_STRAIGHT)
        || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!WidgetIsDisabled(w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_RIGHT);
            }
            break;
        case TRACK_BANK_LEFT:
            if (!WidgetIsDisabled(w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!WidgetIsDisabled(w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(w, WIDX_PREVIOUS_SECTION)
        || w->widgets[WIDX_PREVIOUS_SECTION].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_PREVIOUS_SECTION);
}

void window_ride_construction_keyboard_shortcut_next_track()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_NEXT_SECTION) || w->widgets[WIDX_NEXT_SECTION].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_NEXT_SECTION);
}

void window_ride_construction_keyboard_shortcut_build_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_CONSTRUCT);
}

void window_ride_construction_keyboard_shortcut_demolish_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(w, WIDX_DEMOLISH) || w->widgets[WIDX_DEMOLISH].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_DEMOLISH);
}

static bool track_piece_direction_is_diagonal(const uint8_t direction)
{
    return direction >= 4;
}
