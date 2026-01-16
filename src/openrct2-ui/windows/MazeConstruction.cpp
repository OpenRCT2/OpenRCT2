/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../ride/Construction.h"

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/MazeSetTrackAction.h>
#include <openrct2/actions/ResultWithMessage.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/tile_element/EntranceElement.h>

using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;
using OpenRCT2::GameActions::MazeBuildMode;

namespace OpenRCT2::Ui::Windows
{
#pragma region Widgets

    static constexpr StringId kWindowTitle = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
    static constexpr ScreenSize kWindowSize = { 166, 200 };

    enum : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_MAZE_MODE_GROUPBOX,
        WIDX_MAZE_BUILD_MODE = 6,
        WIDX_MAZE_MOVE_MODE,
        WIDX_MAZE_FILL_MODE,
        WIDX_MAZE_DIRECTION_GROUPBOX = 27,
        WIDX_MAZE_DIRECTION_NW,
        WIDX_MAZE_DIRECTION_NE,
        WIDX_MAZE_DIRECTION_SW,
        WIDX_MAZE_DIRECTION_SE,
        WIDX_MAZE_ENTRANCE = 32,
        WIDX_MAZE_EXIT,
    };

    VALIDATE_GLOBAL_WIDX(WC_MAZE_CONSTRUCTION, WIDX_MAZE_DIRECTION_GROUPBOX);
    VALIDATE_GLOBAL_WIDX(WC_MAZE_CONSTRUCTION, WIDX_MAZE_ENTRANCE);
    VALIDATE_GLOBAL_WIDX(WC_MAZE_CONSTRUCTION, WIDX_MAZE_EXIT);

    // clang-format off
    static constexpr auto window_maze_construction_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ 3,  17}, {160, 55}, WidgetType::groupbox, WindowColour::primary,   STR_RIDE_CONSTRUCTION_MODE                                                                    ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({35,  29}, { 32, 32}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_MAZE_CONSTRUCTION_BUILD),   STR_RIDE_CONSTRUCTION_BUILD_MODE                      ),
        makeWidget({67,  29}, { 32, 32}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_MAZE_CONSTRUCTION_MOVE),    STR_RIDE_CONSTRUCTION_MOVE_MODE                       ),
        makeWidget({99,  29}, { 32, 32}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_MAZE_CONSTRUCTION_FILL_IN), STR_RIDE_CONSTRUCTION_FILL_IN_MODE                    ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 3, 168}, {160, 28}, WidgetType::groupbox, WindowColour::primary                                                                                                  ),
        makeWidget({ 3,  80}, {160, 87}, WidgetType::groupbox, WindowColour::primary,   STR_RIDE_CONSTRUCTION_BUILD                                                                   ),
        makeWidget({83,  96}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NE), STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
        makeWidget({83, 125}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SE), STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
        makeWidget({38, 125}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_SW), STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
        makeWidget({38,  96}, { 45, 29}, WidgetType::flatBtn,  WindowColour::secondary, ImageId(SPR_CONSTRUCTION_DIRECTION_NW), STR_RIDE_CONSTRUCTION_BUILD_MAZE_IN_THIS_DIRECTION_TIP),
        makeWidget({ 9, 178}, { 70, 12}, WidgetType::button,   WindowColour::secondary, STR_RIDE_CONSTRUCTION_ENTRANCE,         STR_RIDE_CONSTRUCTION_ENTRANCE_TIP                    ),
        makeWidget({87, 178}, { 70, 12}, WidgetType::button,   WindowColour::secondary, STR_RIDE_CONSTRUCTION_EXIT,             STR_RIDE_CONSTRUCTION_EXIT_TIP                        ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  ),
        makeWidget({ 0,   0}, {  1,  1}, WidgetType::empty,    WindowColour::primary                                                                                                  )
    );
    // clang-format on

#pragma endregion

    class MazeConstructionWindow final : public Window
    {
    public:
        void onOpen() override
        {
            setWidgets(window_maze_construction_widgets);
            WindowInitScrollWidgets(*this);
            rideId = _currentRideIndex;
            ShowGridlines();
        }

        void onClose() override
        {
            RideConstructionInvalidateCurrentTrack();
            ViewportSetVisibility(ViewportVisibility::standard);

            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

            // In order to cancel the yellow arrow correctly the
            // selection tool should be cancelled.
            ToolCancel();

            HideGridlines();

            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide != nullptr)
            {
                if (currentRide->overallView.IsNull())
                {
                    auto gameAction = GameActions::RideDemolishAction(currentRide->id, GameActions::RideModifyType::demolish);
                    gameAction.SetFlags({ CommandFlag::allowDuringPaused });
                    GameActions::Execute(&gameAction, getGameState());
                }
                else
                {
                    auto intent = Intent(WindowClass::ride);
                    intent.PutExtra(INTENT_EXTRA_RIDE_ID, currentRide->id.ToUnderlying());
                    ContextOpenIntent(&intent);
                }
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_MAZE_ENTRANCE:
                case WIDX_MAZE_EXIT:
                    WindowMazeConstructionEntranceMouseup(widgetIndex);
                    break;
                case WIDX_MAZE_DIRECTION_NW:
                case WIDX_MAZE_DIRECTION_NE:
                case WIDX_MAZE_DIRECTION_SE:
                case WIDX_MAZE_DIRECTION_SW:
                    WindowMazeConstructionConstruct(((widgetIndex - WIDX_MAZE_DIRECTION_NW) - GetCurrentRotation()) & 3);
                    break;
            }
        }

        void onResize() override
        {
            uint64_t newDisabledWidgets = 0;
            if (_rideConstructionState == RideConstructionState::Place)
            {
                newDisabledWidgets
                    |= ((1uLL << WIDX_MAZE_BUILD_MODE) | (1uLL << WIDX_MAZE_MOVE_MODE) | (1uLL << WIDX_MAZE_FILL_MODE)
                        | (1uLL << WIDX_MAZE_DIRECTION_NW) | (1uLL << WIDX_MAZE_DIRECTION_NE) | (1uLL << WIDX_MAZE_DIRECTION_SW)
                        | (1uLL << WIDX_MAZE_DIRECTION_SE));
            }
            else if (_rideConstructionState == RideConstructionState::EntranceExit)
            {
                newDisabledWidgets = (1uLL << WIDX_MAZE_DIRECTION_NW) | (1uLL << WIDX_MAZE_DIRECTION_NE)
                    | (1uLL << WIDX_MAZE_DIRECTION_SW) | (1uLL << WIDX_MAZE_DIRECTION_SE);
            }

            // Set and invalidate the changed widgets
            uint64_t currentDisabledWidgets = disabledWidgets;
            if (currentDisabledWidgets == newDisabledWidgets)
                return;

            for (WidgetIndex i = 0; i < 64; i++)
            {
                if ((newDisabledWidgets & (1uLL << i)) != (currentDisabledWidgets & (1uLL << i)))
                {
                    invalidateWidget(i);
                }
            }
            disabledWidgets = newDisabledWidgets;
        }

        void onMouseDown(WidgetIndex widgetIndex) override
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

        void onUpdate() override
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr || currentRide->status != RideStatus::closed)
            {
                close();
                return;
            }

            switch (_rideConstructionState)
            {
                case RideConstructionState::Place:
                    if (!isToolActive(*this, WIDX_MAZE_DIRECTION_GROUPBOX))
                    {
                        close();
                        return;
                    }
                    break;
                case RideConstructionState::EntranceExit:
                    if (!isToolActive(*this, WIDX_MAZE_ENTRANCE) && !isToolActive(*this, WIDX_MAZE_EXIT))
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
                    if (isToolActive(WindowClass::rideConstruction))
                    {
                        ToolCancel();
                    }
                    break;
                default:
                    break;
            }
            UpdateGhostTrackAndArrow();
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_MAZE_DIRECTION_GROUPBOX:
                    RideConstructionToolupdateConstruct(screenCoords);
                    break;
                case WIDX_MAZE_ENTRANCE:
                case WIDX_MAZE_EXIT:
                    RideConstructionToolupdateEntranceExit(screenCoords);
                    break;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_MAZE_DIRECTION_GROUPBOX:
                    RideConstructionTooldownConstruct(screenCoords);
                    break;
                case WIDX_MAZE_ENTRANCE:
                case WIDX_MAZE_EXIT:
                    WindowMazeConstructionEntranceTooldown(screenCoords);
                    break;
            }
        }

        void onPrepareDraw() override
        {
            auto currentRide = GetRide(_currentRideIndex);
            auto ft = Formatter::Common();
            if (currentRide != nullptr)
            {
                ft.Increment(4);
                currentRide->formatNameTo(ft);
            }
            else
            {
                ft.Increment(4);
                ft.Add<StringId>(kStringIdNone);
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
        }

    private:
        void WindowMazeConstructionEntranceMouseup(WidgetIndex widgetIndex)
        {
            if (ToolSet(*this, widgetIndex, Tool::crosshair))
                return;

            gRideEntranceExitPlaceType = widgetIndex == WIDX_MAZE_ENTRANCE ? ENTRANCE_TYPE_RIDE_ENTRANCE
                                                                           : ENTRANCE_TYPE_RIDE_EXIT;
            gRideEntranceExitPlaceRideIndex = rideId;
            gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);

            RideConstructionInvalidateCurrentTrack();

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
                ToolCancel();
            }
            _rideConstructionState = rideConstructionState;
            WindowMazeConstructionUpdatePressedWidgets();
        }

        void WindowMazeConstructionEntranceTooldown(const ScreenCoordsXY& screenCoords)
        {
            RideConstructionInvalidateCurrentTrack();

            gMapSelectFlags.unset(MapSelectFlag::enable);
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

            CoordsXYZD entranceOrExitCoords = RideGetEntranceOrExitPositionFromScreenPosition(screenCoords);
            if (entranceOrExitCoords.IsNull())
                return;

            if (gRideEntranceExitPlaceDirection == kInvalidDirection)
                return;

            RideId rideIndex = gRideEntranceExitPlaceRideIndex;

            auto rideEntranceExitPlaceAction = GameActions::RideEntranceExitPlaceAction(
                entranceOrExitCoords, DirectionReverse(entranceOrExitCoords.direction), rideIndex,
                gRideEntranceExitPlaceStationIndex, gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

            rideEntranceExitPlaceAction.SetCallback([=, this](
                                                        const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                    return;

                Audio::Play3D(Audio::SoundId::placeItem, result->position);

                auto* windowMgr = GetWindowManager();

                auto currentRide = GetRide(rideIndex);
                if (currentRide != nullptr && RideAreAllPossibleEntrancesAndExitsBuilt(*currentRide).Successful)
                {
                    ToolCancel();
                    if (!currentRide->getRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                    {
                        windowMgr->CloseByClass(WindowClass::rideConstruction);
                    }
                }
                else
                {
                    gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
                    windowMgr->InvalidateByClass(WindowClass::rideConstruction);

                    auto newToolWidgetIndex = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE) ? WIDX_MAZE_ENTRANCE
                                                                                                          : WIDX_MAZE_EXIT;

                    ToolCancel();
                    ToolSet(*this, newToolWidgetIndex, Tool::crosshair);

                    WindowMazeConstructionUpdatePressedWidgets();
                }
            });
            auto res = GameActions::Execute(&rideEntranceExitPlaceAction, getGameState());
        }

        void WindowMazeConstructionConstruct(int32_t direction)
        {
            int32_t x, y, z;
            MazeBuildMode mode;
            CommandFlags actionFlags = {};

            _currentTrackSelectionFlags.clearAll();
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);
            RideConstructionInvalidateCurrentTrack();

            x = _currentTrackBegin.x + (CoordsDirectionDelta[direction].x / 2);
            y = _currentTrackBegin.y + (CoordsDirectionDelta[direction].y / 2);
            z = _currentTrackBegin.z;
            switch (_rideConstructionState)
            {
                case RideConstructionState::MazeBuild:
                    mode = MazeBuildMode::build;
                    break;
                case RideConstructionState::MazeMove:
                    mode = MazeBuildMode::move;
                    actionFlags = { CommandFlag::allowDuringPaused };
                    break;
                default:
                case RideConstructionState::MazeFill:
                    mode = MazeBuildMode::fill;
                    break;
            }

            const auto loc = CoordsXYZD{ x, y, z, static_cast<uint8_t>(direction) };
            auto action = GameActions::MazeSetTrackAction(loc, false, _currentRideIndex, mode);
            action.SetFlags(actionFlags);
            const auto res = GameActions::Execute(&action, getGameState());
            if (res.error != GameActions::Status::ok)
            {
                return;
            }

            _currentTrackBegin.x = x;
            _currentTrackBegin.y = y;
            if (_rideConstructionState != RideConstructionState::MazeMove)
            {
                Audio::Play3D(Audio::SoundId::placeItem, { x, y, z });
            }
        }
    };

    WindowBase* MazeConstructionOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<MazeConstructionWindow>(
            WindowClass::rideConstruction, ScreenCoordsXY(0, 29), kWindowSize, WindowFlag::noAutoClose);
    }

    void WindowMazeConstructionUpdatePressedWidgets()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::rideConstruction);
        if (w == nullptr)
            return;

        uint64_t newPressedWidgets = w->pressedWidgets;

        // Unpress all the mode buttons
        newPressedWidgets &= ~EnumToFlag(WIDX_MAZE_BUILD_MODE);
        newPressedWidgets &= ~EnumToFlag(WIDX_MAZE_MOVE_MODE);
        newPressedWidgets &= ~EnumToFlag(WIDX_MAZE_FILL_MODE);
        newPressedWidgets &= ~EnumToFlag(WIDX_MAZE_ENTRANCE);
        newPressedWidgets &= ~EnumToFlag(WIDX_MAZE_EXIT);

        switch (_rideConstructionState)
        {
            case RideConstructionState::EntranceExit:
                if (isToolActive(WindowClass::rideConstruction, WIDX_MAZE_ENTRANCE))
                {
                    newPressedWidgets |= EnumToFlag(WIDX_MAZE_ENTRANCE);
                }
                else
                {
                    newPressedWidgets |= EnumToFlag(WIDX_MAZE_EXIT);
                }
                break;
            case RideConstructionState::MazeBuild:
                newPressedWidgets |= EnumToFlag(WIDX_MAZE_BUILD_MODE);
                break;
            case RideConstructionState::MazeMove:
                newPressedWidgets |= EnumToFlag(WIDX_MAZE_MOVE_MODE);
                break;
            case RideConstructionState::MazeFill:
                newPressedWidgets |= EnumToFlag(WIDX_MAZE_FILL_MODE);
                break;
            default:
                break;
        }

        w->pressedWidgets = newPressedWidgets;
        w->invalidate();
    }
} // namespace OpenRCT2::Ui::Windows
