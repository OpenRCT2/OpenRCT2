/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>

#pragma region Widgets

static constexpr const rct_string_id WINDOW_TITLE = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
static constexpr const int32_t WH = 200;
static constexpr const int32_t WW = 166;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_MAZE_MODE_GROUPBOX,
    WIDX_MAZE_BUILD_MODE = 6,
    WIDX_MAZE_MOVE_MODE,
    WIDX_MAZE_FILL_MODE,
    WIDX_MAZE_DIRECTION_GROUPBOX = 23,
    WIDX_MAZE_DIRECTION_NW,
    WIDX_MAZE_DIRECTION_NE,
    WIDX_MAZE_DIRECTION_SW,
    WIDX_MAZE_DIRECTION_SE,
    WIDX_MAZE_ENTRANCE = 29,
    WIDX_MAZE_EXIT,
};

static rct_widget window_maze_construction_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({ 3,  17}, {160, 55}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_MODE                                                            ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({35,  29}, { 32, 32}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_MAZE_CONSTRUCTION_BUILD,    STR_RIDE_CONSTRUCTION_BUILD_MODE                      ),
    MakeWidget({67,  29}, { 32, 32}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_MAZE_CONSTRUCTION_MOVE,     STR_RIDE_CONSTRUCTION_MOVE_MODE                       ),
    MakeWidget({99,  29}, { 32, 32}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_MAZE_CONSTRUCTION_FILL_IN,  STR_RIDE_CONSTRUCTION_FILL_IN_MODE                    ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 3,  80}, {160, 87}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_BUILD                                                           ),
    MakeWidget({83,  96}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NE,  STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
    MakeWidget({83, 125}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SE,  STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
    MakeWidget({38, 125}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_SW,  STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
    MakeWidget({38,  96}, { 45, 29}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, SPR_CONSTRUCTION_DIRECTION_NW,  STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
    MakeWidget({ 3, 168}, {160, 28}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                          ),
    MakeWidget({ 9, 178}, { 70, 12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_ENTRANCE, STR_RIDE_CONSTRUCTION_ENTRANCE_TIP                    ),
    MakeWidget({87, 178}, { 70, 12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_EXIT,     STR_RIDE_CONSTRUCTION_EXIT_TIP                        ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    MakeWidget({ 0,   0}, {  1,  1}, WindowWidgetType::Empty,    WindowColour::Primary                                                                                          ),
    { WIDGETS_END }
};

#pragma endregion

#pragma region Events

static void window_maze_construction_close(rct_window *w);
static void window_maze_construction_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_maze_construction_resize(rct_window *w);
static void window_maze_construction_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget *widget);
static void window_maze_construction_update(rct_window *w);
static void window_maze_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_maze_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_maze_construction_invalidate(rct_window *w);
static void window_maze_construction_paint(rct_window *w, rct_drawpixelinfo *dpi);

// 0x993F6C
static rct_window_event_list window_maze_construction_events([](auto& events)
{
    events.close = &window_maze_construction_close;
    events.mouse_up = &window_maze_construction_mouseup;
    events.resize = &window_maze_construction_resize;
    events.mouse_down = &window_maze_construction_mousedown;
    events.update = &window_maze_construction_update;
    events.tool_update = &window_maze_construction_toolupdate;
    events.tool_down = &window_maze_construction_tooldown;
    events.invalidate = &window_maze_construction_invalidate;
    events.paint = &window_maze_construction_paint;
});
// clang-format on

#pragma endregion

static void window_maze_construction_construct(int32_t direction);

/**
 *
 *  rct2: 0x006CB481
 */
rct_window* window_maze_construction_open()
{
    rct_window* w = WindowCreate(
        ScreenCoordsXY(0, 29), 166, 200, &window_maze_construction_events, WC_RIDE_CONSTRUCTION, WF_NO_AUTO_CLOSE);
    w->widgets = window_maze_construction_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_MAZE_BUILD_MODE) | (1ULL << WIDX_MAZE_MOVE_MODE)
        | (1ULL << WIDX_MAZE_FILL_MODE) | (1ULL << WIDX_MAZE_DIRECTION_NW) | (1ULL << WIDX_MAZE_DIRECTION_NE)
        | (1ULL << WIDX_MAZE_DIRECTION_SW) | (1ULL << WIDX_MAZE_DIRECTION_SE) | (1ULL << WIDX_MAZE_ENTRANCE)
        | (1ULL << WIDX_MAZE_EXIT);

    WindowInitScrollWidgets(w);

    w->number = _currentRideIndex;

    window_push_others_right(w);
    show_gridlines();
    return w;
}

/**
 *
 *  rct2: 0x006CD811
 */
static void window_maze_construction_close(rct_window* w)
{
    ride_construction_invalidate_current_track();
    viewport_set_visibility(0);

    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    // In order to cancel the yellow arrow correctly the
    // selection tool should be cancelled.
    tool_cancel();

    hide_gridlines();

    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        if (ride->overall_view.isNull())
        {
            int32_t savedPausedState = gGamePaused;
            gGamePaused = 0;
            ride_action_modify(ride, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
            gGamePaused = savedPausedState;
        }
        else
        {
            auto intent = Intent(WC_RIDE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, ride->id);
            context_open_intent(&intent);
        }
    }
}

static void window_maze_construction_entrance_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    if (tool_set(w, widgetIndex, Tool::Crosshair))
        return;

    gRideEntranceExitPlaceType = widgetIndex == WIDX_MAZE_ENTRANCE ? ENTRANCE_TYPE_RIDE_ENTRANCE : ENTRANCE_TYPE_RIDE_EXIT;
    gRideEntranceExitPlaceRideIndex = static_cast<uint8_t>(w->number);
    gRideEntranceExitPlaceStationIndex = 0;
    input_set_flag(INPUT_FLAG_6, true);

    ride_construction_invalidate_current_track();

    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
    {
        gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
    }
    _rideConstructionState = RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT;

    window_maze_construction_update_pressed_widgets();
}

/**
 *
 *  rct2: 0x006CD461
 */
static void window_maze_construction_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_MAZE_ENTRANCE:
        case WIDX_MAZE_EXIT:
            window_maze_construction_entrance_mouseup(w, widgetIndex);
            break;
        case WIDX_MAZE_DIRECTION_NW:
        case WIDX_MAZE_DIRECTION_NE:
        case WIDX_MAZE_DIRECTION_SE:
        case WIDX_MAZE_DIRECTION_SW:
            window_maze_construction_construct(((widgetIndex - WIDX_MAZE_DIRECTION_NW) - get_current_rotation()) & 3);
            break;
    }
}

/**
 *
 *  rct2: 0x006CD623
 */
static void window_maze_construction_resize(rct_window* w)
{
    uint64_t disabledWidgets = 0;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_PLACE)
    {
        disabledWidgets
            |= ((1ULL << WIDX_MAZE_BUILD_MODE) | (1ULL << WIDX_MAZE_MOVE_MODE) | (1ULL << WIDX_MAZE_FILL_MODE)
                | (1ULL << WIDX_MAZE_DIRECTION_NW) | (1ULL << WIDX_MAZE_DIRECTION_NE) | (1ULL << WIDX_MAZE_DIRECTION_SW)
                | (1ULL << WIDX_MAZE_DIRECTION_SE));
    }
    else if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
    {
        disabledWidgets = (1ULL << WIDX_MAZE_DIRECTION_NW) | (1ULL << WIDX_MAZE_DIRECTION_NE) | (1ULL << WIDX_MAZE_DIRECTION_SW)
            | (1ULL << WIDX_MAZE_DIRECTION_SE);
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

static void window_maze_construction_build_mode_mousedown(uint8_t rideConstructionState)
{
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT)
    {
        tool_cancel();
    }
    _rideConstructionState = rideConstructionState;
    window_maze_construction_update_pressed_widgets();
}

/**
 *
 *  rct2: 0x006CD48C
 */
static void window_maze_construction_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_MAZE_BUILD_MODE:
            window_maze_construction_build_mode_mousedown(RIDE_CONSTRUCTION_STATE_MAZE_BUILD);
            break;
        case WIDX_MAZE_MOVE_MODE:
            window_maze_construction_build_mode_mousedown(RIDE_CONSTRUCTION_STATE_MAZE_MOVE);
            break;
        case WIDX_MAZE_FILL_MODE:
            window_maze_construction_build_mode_mousedown(RIDE_CONSTRUCTION_STATE_MAZE_FILL);
            break;
    }
}

/**
 *
 *  rct2: 0x006CD767
 */
static void window_maze_construction_update(rct_window* w)
{
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr || ride->status != RIDE_STATUS_CLOSED)
    {
        window_close(w);
        return;
    }

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_PLACE:
            if (!WidgetIsActiveTool(w, WIDX_MAZE_DIRECTION_GROUPBOX))
            {
                window_close(w);
                return;
            }
            break;
        case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
            if (!WidgetIsActiveTool(w, WIDX_MAZE_ENTRANCE) && !WidgetIsActiveTool(w, WIDX_MAZE_EXIT))
            {
                _rideConstructionState = gRideEntranceExitPlacePreviousRideConstructionState;
                window_maze_construction_update_pressed_widgets();
            }
            break;
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
 *  rct2: 0x006CD63E
 */
static void window_maze_construction_toolupdate(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_MAZE_DIRECTION_GROUPBOX:
            ride_construction_toolupdate_construct(screenCoords);
            break;
        case WIDX_MAZE_ENTRANCE:
        case WIDX_MAZE_EXIT:
            ride_construction_toolupdate_entrance_exit(screenCoords);
            break;
    }
}

/**
 *
 *  rct2: 0x006C825F
 */
static void window_maze_construction_entrance_tooldown(const ScreenCoordsXY& screenCoords, rct_window* w)
{
    ride_construction_invalidate_current_track();

    map_invalidate_selection_rect();

    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

    CoordsXYZD entranceOrExitCoords = ride_get_entrance_or_exit_position_from_screen_position(screenCoords);
    if (entranceOrExitCoords.isNull())
        return;

    if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
        return;

    ride_id_t rideIndex = gRideEntranceExitPlaceRideIndex;

    auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
        entranceOrExitCoords, direction_reverse(entranceOrExitCoords.direction), rideIndex, gRideEntranceExitPlaceStationIndex,
        gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

    rideEntranceExitPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
        if (result->Error != GameActions::Status::Ok)
            return;

        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

        auto ride = get_ride(rideIndex);
        if (ride != nullptr && ride_are_all_possible_entrances_and_exits_built(ride))
        {
            tool_cancel();
            if (ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
                window_close_by_class(WC_RIDE_CONSTRUCTION);
        }
        else
        {
            gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
            window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
            gCurrentToolWidget.widget_index = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE) ? WIDX_MAZE_ENTRANCE
                                                                                                          : WIDX_MAZE_EXIT;

            window_maze_construction_update_pressed_widgets();
        }
    });
    auto res = GameActions::Execute(&rideEntranceExitPlaceAction);
}

/**
 *
 *  rct2: 0x006CD65D
 */
static void window_maze_construction_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    switch (widgetIndex)
    {
        case WIDX_MAZE_DIRECTION_GROUPBOX:
            ride_construction_tooldown_construct(screenCoords);
            break;
        case WIDX_MAZE_ENTRANCE:
        case WIDX_MAZE_EXIT:
            window_maze_construction_entrance_tooldown(screenCoords, w);
            break;
    }
}

/**
 *
 *  rct2: 0x006CD435
 */
static void window_maze_construction_invalidate(rct_window* w)
{
    auto ride = get_ride(_currentRideIndex);
    auto ft = Formatter::Common();
    if (ride != nullptr)
    {
        ft.Increment(4);
        ride->FormatNameTo(ft);
    }
    else
    {
        ft.Increment(4);
        ft.Add<rct_string_id>(STR_NONE);
    }
}

/**
 *
 *  rct2: 0x006CD45B
 */
static void window_maze_construction_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);
}

/**
 *
 *  rct2: 0x006CD887
 */
void window_maze_construction_update_pressed_widgets()
{
    rct_window* w;

    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr)
        return;

    uint64_t pressedWidgets = w->pressed_widgets;

    // Unpress all the mode buttons
    pressedWidgets &= ~EnumToFlag(WIDX_MAZE_BUILD_MODE);
    pressedWidgets &= ~EnumToFlag(WIDX_MAZE_MOVE_MODE);
    pressedWidgets &= ~EnumToFlag(WIDX_MAZE_FILL_MODE);
    pressedWidgets &= ~EnumToFlag(WIDX_MAZE_ENTRANCE);
    pressedWidgets &= ~EnumToFlag(WIDX_MAZE_EXIT);

    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_ENTRANCE_EXIT:
            if (gCurrentToolWidget.widget_index == WIDX_MAZE_ENTRANCE)
            {
                pressedWidgets |= EnumToFlag(WIDX_MAZE_ENTRANCE);
            }
            else
            {
                pressedWidgets |= EnumToFlag(WIDX_MAZE_EXIT);
            }
            break;
        case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_BUILD_MODE);
            break;
        case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_MOVE_MODE);
            break;
        case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_FILL_MODE);
            break;
    }

    w->pressed_widgets = pressedWidgets;
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006CD4AB
 */
static void window_maze_construction_construct(int32_t direction)
{
    int32_t x, y, z, flags, mode;

    _currentTrackSelectionFlags = 0;
    _rideConstructionNextArrowPulse = 0;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    ride_construction_invalidate_current_track();

    x = _currentTrackBegin.x + (CoordsDirectionDelta[direction].x / 2);
    y = _currentTrackBegin.y + (CoordsDirectionDelta[direction].y / 2);
    z = _currentTrackBegin.z;
    switch (_rideConstructionState)
    {
        case RIDE_CONSTRUCTION_STATE_MAZE_BUILD:
            mode = GC_SET_MAZE_TRACK_BUILD;
            flags = GAME_COMMAND_FLAG_APPLY;
            break;
        case RIDE_CONSTRUCTION_STATE_MAZE_MOVE:
            mode = GC_SET_MAZE_TRACK_MOVE;
            flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
            break;
        default:
        case RIDE_CONSTRUCTION_STATE_MAZE_FILL:
            mode = GC_SET_MAZE_TRACK_FILL;
            flags = GAME_COMMAND_FLAG_APPLY;
            break;
    }

    money32 cost = maze_set_track(x, y, z, flags, false, direction, _currentRideIndex, mode);
    if (cost == MONEY32_UNDEFINED)
    {
        return;
    }

    _currentTrackBegin.x = x;
    _currentTrackBegin.y = y;
    if (_rideConstructionState != RIDE_CONSTRUCTION_STATE_MAZE_MOVE)
    {
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, { x, y, z });
    }
}
