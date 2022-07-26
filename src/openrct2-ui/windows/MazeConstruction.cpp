/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/actions/MazeSetTrackAction.h"

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideConstruction.h>
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
    WIDGETS_END,
};

#pragma endregion
// clang-format on

class MazeConstructionWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_maze_construction_widgets;
        WindowInitScrollWidgets(this);
        rideId = _currentRideIndex;
        show_gridlines();
    }

    void OnClose() override
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

        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide != nullptr)
        {
            if (currentRide->overall_view.IsNull())
            {
                int32_t savedPausedState = gGamePaused;
                gGamePaused = 0;
                ride_action_modify(
                    currentRide, RIDE_MODIFY_DEMOLISH, GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
                gGamePaused = savedPausedState;
            }
            else
            {
                auto intent = Intent(WC_RIDE);
                intent.putExtra(INTENT_EXTRA_RIDE_ID, currentRide->id.ToUnderlying());
                context_open_intent(&intent);
            }
        }
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_MAZE_ENTRANCE:
            case WIDX_MAZE_EXIT:
                WindowMazeConstructionEntranceMouseup(widgetIndex);
                break;
            case WIDX_MAZE_DIRECTION_NW:
            case WIDX_MAZE_DIRECTION_NE:
            case WIDX_MAZE_DIRECTION_SE:
            case WIDX_MAZE_DIRECTION_SW:
                WindowMazeConstructionConstruct(((widgetIndex - WIDX_MAZE_DIRECTION_NW) - get_current_rotation()) & 3);
                break;
        }
    }

    void OnResize() override
    {
        uint64_t disabledWidgets = 0;
        if (_rideConstructionState == RideConstructionState::Place)
        {
            disabledWidgets
                |= ((1ULL << WIDX_MAZE_BUILD_MODE) | (1ULL << WIDX_MAZE_MOVE_MODE) | (1ULL << WIDX_MAZE_FILL_MODE)
                    | (1ULL << WIDX_MAZE_DIRECTION_NW) | (1ULL << WIDX_MAZE_DIRECTION_NE) | (1ULL << WIDX_MAZE_DIRECTION_SW)
                    | (1ULL << WIDX_MAZE_DIRECTION_SE));
        }
        else if (_rideConstructionState == RideConstructionState::EntranceExit)
        {
            disabledWidgets = (1ULL << WIDX_MAZE_DIRECTION_NW) | (1ULL << WIDX_MAZE_DIRECTION_NE)
                | (1ULL << WIDX_MAZE_DIRECTION_SW) | (1ULL << WIDX_MAZE_DIRECTION_SE);
        }

        // Set and invalidate the changed widgets
        uint64_t currentDisabledWidgets = disabled_widgets;
        if (currentDisabledWidgets == disabledWidgets)
            return;

        for (rct_widgetindex i = 0; i < 64; i++)
        {
            if ((disabledWidgets & (1ULL << i)) != (currentDisabledWidgets & (1ULL << i)))
            {
                widget_invalidate(this, i);
            }
        }
        disabled_widgets = disabledWidgets;
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_MAZE_BUILD_MODE:
                WindowMazeConstructionBuildModeMousedown(RideConstructionState::MazeBuild);
                break;
            case WIDX_MAZE_MOVE_MODE:
                WindowMazeConstructionBuildModeMousedown(RideConstructionState::MazeMove);
                break;
            case WIDX_MAZE_FILL_MODE:
                WindowMazeConstructionBuildModeMousedown(RideConstructionState::MazeFill);
                break;
        }
    }

    void OnUpdate() override
    {
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr || currentRide->status != RideStatus::Closed)
        {
            Close();
            return;
        }

        switch (_rideConstructionState)
        {
            case RideConstructionState::Place:
                if (!WidgetIsActiveTool(this, WIDX_MAZE_DIRECTION_GROUPBOX))
                {
                    Close();
                    return;
                }
                break;
            case RideConstructionState::EntranceExit:
                if (!WidgetIsActiveTool(this, WIDX_MAZE_ENTRANCE) && !WidgetIsActiveTool(this, WIDX_MAZE_EXIT))
                {
                    _rideConstructionState = gRideEntranceExitPlacePreviousRideConstructionState;
                    WindowMazeConstructionUpdatePressedWidgets();
                }
                break;
            default:
                break;
        }

        switch (_rideConstructionState)
        {
            case RideConstructionState::Front:
            case RideConstructionState::Back:
            case RideConstructionState::Selected:
                if ((input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
                    && gCurrentToolWidget.window_classification == WC_RIDE_CONSTRUCTION)
                {
                    tool_cancel();
                }
                break;
            default:
                break;
        }
        UpdateGhostTrackAndArrow();
    }

    void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
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

    void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (widgetIndex)
        {
            case WIDX_MAZE_DIRECTION_GROUPBOX:
                ride_construction_tooldown_construct(screenCoords);
                break;
            case WIDX_MAZE_ENTRANCE:
            case WIDX_MAZE_EXIT:
                WindowMazeConstructionEntranceTooldown(screenCoords);
                break;
        }
    }

    void OnPrepareDraw() override
    {
        auto currentRide = get_ride(_currentRideIndex);
        auto ft = Formatter::Common();
        if (currentRide != nullptr)
        {
            ft.Increment(4);
            currentRide->FormatNameTo(ft);
        }
        else
        {
            ft.Increment(4);
            ft.Add<rct_string_id>(STR_NONE);
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
    }

private:
    void WindowMazeConstructionEntranceMouseup(rct_widgetindex widgetIndex)
    {
        if (tool_set(*this, widgetIndex, Tool::Crosshair))
            return;

        gRideEntranceExitPlaceType = widgetIndex == WIDX_MAZE_ENTRANCE ? ENTRANCE_TYPE_RIDE_ENTRANCE : ENTRANCE_TYPE_RIDE_EXIT;
        gRideEntranceExitPlaceRideIndex = rideId;
        gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
        input_set_flag(INPUT_FLAG_6, true);

        ride_construction_invalidate_current_track();

        if (_rideConstructionState != RideConstructionState::EntranceExit)
        {
            gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
        }
        _rideConstructionState = RideConstructionState::EntranceExit;

        WindowMazeConstructionUpdatePressedWidgets();
    }

    void WindowMazeConstructionBuildModeMousedown(RideConstructionState rideConstructionState)
    {
        if (_rideConstructionState == RideConstructionState::EntranceExit)
        {
            tool_cancel();
        }
        _rideConstructionState = rideConstructionState;
        WindowMazeConstructionUpdatePressedWidgets();
    }

    void WindowMazeConstructionEntranceTooldown(const ScreenCoordsXY& screenCoords)
    {
        ride_construction_invalidate_current_track();

        map_invalidate_selection_rect();

        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        CoordsXYZD entranceOrExitCoords = ride_get_entrance_or_exit_position_from_screen_position(screenCoords);
        if (entranceOrExitCoords.IsNull())
            return;

        if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
            return;

        RideId rideIndex = gRideEntranceExitPlaceRideIndex;

        auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
            entranceOrExitCoords, direction_reverse(entranceOrExitCoords.direction), rideIndex,
            gRideEntranceExitPlaceStationIndex, gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

        rideEntranceExitPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error != GameActions::Status::Ok)
                return;

            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

            auto currentRide = get_ride(rideIndex);
            if (currentRide != nullptr && ride_are_all_possible_entrances_and_exits_built(currentRide).Successful)
            {
                tool_cancel();
                if (currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
                    window_close_by_class(WC_RIDE_CONSTRUCTION);
            }
            else
            {
                gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
                window_invalidate_by_class(WC_RIDE_CONSTRUCTION);
                gCurrentToolWidget.widget_index = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
                    ? WIDX_MAZE_ENTRANCE
                    : WIDX_MAZE_EXIT;

                WindowMazeConstructionUpdatePressedWidgets();
            }
        });
        auto res = GameActions::Execute(&rideEntranceExitPlaceAction);
    }

    void WindowMazeConstructionConstruct(int32_t direction)
    {
        int32_t x, y, z, actionFlags, mode;

        _currentTrackSelectionFlags = 0;
        _rideConstructionNextArrowPulse = 0;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        ride_construction_invalidate_current_track();

        x = _currentTrackBegin.x + (CoordsDirectionDelta[direction].x / 2);
        y = _currentTrackBegin.y + (CoordsDirectionDelta[direction].y / 2);
        z = _currentTrackBegin.z;
        switch (_rideConstructionState)
        {
            case RideConstructionState::MazeBuild:
                mode = GC_SET_MAZE_TRACK_BUILD;
                actionFlags = GAME_COMMAND_FLAG_APPLY;
                break;
            case RideConstructionState::MazeMove:
                mode = GC_SET_MAZE_TRACK_MOVE;
                actionFlags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED;
                break;
            default:
            case RideConstructionState::MazeFill:
                mode = GC_SET_MAZE_TRACK_FILL;
                actionFlags = GAME_COMMAND_FLAG_APPLY;
                break;
        }

        const auto loc = CoordsXYZD{ x, y, z, static_cast<uint8_t>(direction) };
        const auto action = MazeSetTrackAction(loc, false, _currentRideIndex, mode, actionFlags);
        const auto res = GameActions::Execute(&action);
        if (res.Error != GameActions::Status::Ok)
        {
            return;
        }

        _currentTrackBegin.x = x;
        _currentTrackBegin.y = y;
        if (_rideConstructionState != RideConstructionState::MazeMove)
        {
            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, { x, y, z });
        }
    }
};

rct_window* WindowMazeConstructionOpen()
{
    return WindowFocusOrCreate<MazeConstructionWindow>(WC_RIDE_CONSTRUCTION, ScreenCoordsXY(0, 29), WW, WH, WF_NO_AUTO_CLOSE);
}

void WindowMazeConstructionUpdatePressedWidgets()
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
        case RideConstructionState::EntranceExit:
            if (gCurrentToolWidget.widget_index == WIDX_MAZE_ENTRANCE)
            {
                pressedWidgets |= EnumToFlag(WIDX_MAZE_ENTRANCE);
            }
            else
            {
                pressedWidgets |= EnumToFlag(WIDX_MAZE_EXIT);
            }
            break;
        case RideConstructionState::MazeBuild:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_BUILD_MODE);
            break;
        case RideConstructionState::MazeMove:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_MOVE_MODE);
            break;
        case RideConstructionState::MazeFill:
            pressedWidgets |= EnumToFlag(WIDX_MAZE_FILL_MODE);
            break;
        default:
            break;
    }

    w->pressed_widgets = pressedWidgets;
    w->Invalidate();
}
