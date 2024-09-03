/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2-ui/ProvisionalElements.h"

#include <limits>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/MazeSetTrackAction.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/actions/TrackPlaceAction.h>
#include <openrct2/actions/TrackRemoveAction.h>
#include <openrct2/actions/TrackSetBrakeSpeedAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/interface/Viewport.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Math.hpp>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Park.h>

constexpr int8_t kDefaultSpeedIncrement = 2;
constexpr int8_t kDefaultMinimumSpeed = 2;

using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
    static constexpr int32_t WH = 394;
    static constexpr int32_t WW = 210;

    static constexpr uint16_t ARROW_PULSE_DURATION = 200;
    // Width of the group boxes, e.g. “Banking”
    static constexpr int32_t GW = WW - 6;

#pragma region Widgets

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_DIRECTION_GROUPBOX,
        WIDX_SLOPE_GROUPBOX,
        WIDX_BANKING_GROUPBOX,
        WIDX_LEFT_CURVE_VERY_SMALL,
        WIDX_LEFT_CURVE_SMALL,
        WIDX_LEFT_CURVE,
        WIDX_LEFT_CURVE_LARGE,
        WIDX_STRAIGHT,
        WIDX_RIGHT_CURVE_LARGE,
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
        WIDX_PREVIOUS_SECTION,
        WIDX_NEXT_SECTION,
        WIDX_ENTRANCE_EXIT_GROUPBOX,
        WIDX_ENTRANCE,
        WIDX_EXIT,
        WIDX_ROTATE,
        WIDX_U_TRACK,
        WIDX_O_TRACK,
        WIDX_SEAT_ROTATION_GROUPBOX,
        WIDX_SEAT_ROTATION_ANGLE_SPINNER,
        WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP,
        WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN,
        WIDX_SIMULATE,
        WIDX_SPEED_GROUPBOX = WIDX_BANKING_GROUPBOX,
        WIDX_SPEED_SETTING_SPINNER = WIDX_BANK_LEFT,
        WIDX_SPEED_SETTING_SPINNER_UP = WIDX_BANK_STRAIGHT,
        WIDX_SPEED_SETTING_SPINNER_DOWN = WIDX_BANK_RIGHT,
    };

    validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_CONSTRUCT);
    validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ENTRANCE);
    validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_EXIT);
    validate_global_widx(WC_RIDE_CONSTRUCTION, WIDX_ROTATE);

    // clang-format off
    static Widget _rideConstructionWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget        ({  3,  17}, {     GW,  57}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_DIRECTION                                                                       ),
        MakeWidget        ({  3,  76}, {     GW,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_SLOPE                                                                           ),
        MakeWidget        ({  3, 120}, {     GW,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_ROLL_BANKING                                                                    ),
        MakeWidget        ({  6,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL),  STR_RIDE_CONSTRUCTION_LEFT_CURVE_VERY_SMALL_TIP     ),
        MakeWidget        ({ 28,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_G2_ICON_MEDIUM_CURVE_LEFT),           STR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL_TIP          ),
        MakeWidget        ({ 50,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_LEFT_CURVE),        STR_RIDE_CONSTRUCTION_LEFT_CURVE_TIP                ),
        MakeWidget        ({ 72,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE),  STR_RIDE_CONSTRUCTION_LEFT_CURVE_LARGE_TIP          ),
        MakeWidget        ({ 94,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_STRAIGHT),          STR_RIDE_CONSTRUCTION_STRAIGHT_TIP                  ),
        MakeWidget        ({116,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE), STR_RIDE_CONSTRUCTION_RIGHT_CURVE_LARGE_TIP         ),
        MakeWidget        ({138,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_RIGHT_CURVE),       STR_RIDE_CONSTRUCTION_RIGHT_CURVE_TIP               ),
        MakeWidget        ({160,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_G2_ICON_MEDIUM_CURVE_RIGHT),          STR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL_TIP         ),
        MakeWidget        ({182,  29}, {     22,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL), STR_RIDE_CONSTRUCTION_RIGHT_CURVE_VERY_SMALL_TIP    ),
        MakeWidget        ({  6,  55}, { GW - 6,  14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_YELLOW_STRING,                                STR_RIDE_CONSTRUCTION_OTHER_TRACK_CONFIGURATIONS_TIP),
        MakeWidget        ({ 45,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP),  STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP          ),
        MakeWidget        ({ 69,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN),        STR_RIDE_CONSTRUCTION_SLOPE_DOWN_TIP                ),
        MakeWidget        ({ 93,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL),       STR_RIDE_CONSTRUCTION_LEVEL_TIP                     ),
        MakeWidget        ({117,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP),          STR_RIDE_CONSTRUCTION_SLOPE_UP_TIP                  ),
        MakeWidget        ({141,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP),    STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP            ),
        MakeWidget        ({178,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_CHAIN_LIFT),                          STR_RIDE_CONSTRUCTION_CHAIN_LIFT_TIP                ),
        MakeWidget        ({ 69, 132}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_LEFT_BANK),         STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP       ),
        MakeWidget        ({ 93, 132}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_NO_BANK),           STR_RIDE_CONSTRUCTION_NO_ROLL_TIP                   ),
        MakeWidget        ({117, 132}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_RIGHT_BANK),        STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP      ),
        MakeWidget        ({  3, 164}, {     GW, 170}, WindowWidgetType::ImgBtn,   WindowColour::Secondary, 0xFFFFFFFF,                                       STR_RIDE_CONSTRUCTION_CONSTRUCT_SELECTED_SECTION_TIP),
        MakeWidget        ({ 82, 338}, {     46,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_DEMOLISH_CURRENT_SECTION),            STR_RIDE_CONSTRUCTION_REMOVE_HIGHLIGHTED_SECTION_TIP),
        MakeWidget        ({ 52, 338}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_PREVIOUS),                            STR_RIDE_CONSTRUCTION_MOVE_TO_PREVIOUS_SECTION_TIP  ),
        MakeWidget        ({134, 338}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_NEXT),                                STR_RIDE_CONSTRUCTION_MOVE_TO_NEXT_SECTION_TIP      ),
        MakeWidget        ({  3, 362}, {     GW,  28}, WindowWidgetType::Groupbox, WindowColour::Primary                                                                                                          ),
        MakeWidget        ({ 31, 372}, {     70,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_ENTRANCE,                   STR_RIDE_CONSTRUCTION_ENTRANCE_TIP                  ),
        MakeWidget        ({109, 372}, {     70,  12}, WindowWidgetType::Button,   WindowColour::Secondary, STR_RIDE_CONSTRUCTION_EXIT,                       STR_RIDE_CONSTRUCTION_EXIT_TIP                      ),
        MakeWidget        ({ 94, 338}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_ROTATE_ARROW),                        STR_ROTATE_90_TIP                                   ),
        MakeWidget        ({ 41, 132}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK),    STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP       ),
        MakeWidget        ({144, 132}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK),    STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP   ),
        MakeWidget        ({118, 120}, {     89,  41}, WindowWidgetType::Groupbox, WindowColour::Primary  , STR_RIDE_CONSTRUCTION_SEAT_ROT                                                                        ),
        MakeSpinnerWidgets({123, 138}, {     58,  12}, WindowWidgetType::Spinner,  WindowColour::Secondary, 0,                                                STR_RIDE_CONSTRUCTION_SELECT_SEAT_ROTATION_ANGLE_TIP),
        MakeWidget        ({161, 338}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_G2_SIMULATE),                         STR_SIMULATE_RIDE_TIP                               ),
        kWidgetsEnd,
    };
    // clang-format on

#pragma endregion

    static bool _trackPlaceCtrlState;
    static int32_t _trackPlaceCtrlZ;
    static bool _trackPlaceShiftState;
    static ScreenCoordsXY _trackPlaceShiftStart;
    static int32_t _trackPlaceShiftZ;
    static int32_t _trackPlaceZ;
    static money64 _trackPlaceCost;
    static StringId _trackPlaceErrorMessage;
    static bool _autoRotatingShop;
    static bool _gotoStartPlacementMode = false;

    static constexpr StringId RideConstructionSeatAngleRotationStrings[] = {
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_180, STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_135,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_90,  STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_45,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_0,       STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_45,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_90,      STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_135,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_180,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_225,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_270,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_315,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_360,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_405,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_450,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_495,
    };

    static void WindowRideConstructionMouseUpDemolishNextPiece(const CoordsXYZD& piecePos, int32_t type);
    static void WindowRideConstructionUpdateActiveElements();

    /* move to ride.c */
    static void CloseRideWindowForConstruction(RideId rideId)
    {
        WindowBase* w = WindowFindByNumber(WindowClass::Ride, rideId.ToUnderlying());
        if (w != nullptr && w->page == 1)
            WindowClose(*w);
    }

    static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActions::Result* result);
    static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActions::Result* result);
    static void CloseConstructWindowOnCompletion(const Ride& ride);

    class RideConstructionWindow final : public Window
    {
    private:
        uint8_t _currentlyShowingBrakeOrBoosterSpeed{};
        SpecialElementsDropdownState _specialElementDropdownState;
        bool _autoOpeningShop{};

    public:
        void OnOpen() override
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            widgets = _rideConstructionWidgets;
            number = _currentRideIndex.ToUnderlying();

            InitScrollWidgets();

            WindowPushOthersRight(*this);
            ShowGridlines();

            _currentTrackPrice = kMoney64Undefined;
            _currentBrakeSpeed = 8;
            _currentSeatRotationAngle = 4;

            _currentlySelectedTrack = currentRide->GetRideTypeDescriptor().StartTrackPiece;
            _currentTrackPitchEnd = TrackPitch::None;
            _currentTrackRollEnd = TrackRoll::None;
            _currentTrackLiftHill = 0;
            _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

            if (currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::startConstructionInverted))
                _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

            _previousTrackRollEnd = TrackRoll::None;
            _previousTrackPitchEnd = TrackPitch::None;

            _currentTrackPieceDirection = 0;
            _rideConstructionState = RideConstructionState::Place;
            _currentTrackSelectionFlags = 0;
            _autoOpeningShop = false;
            _autoRotatingShop = true;
            _trackPlaceCtrlState = false;
            _trackPlaceShiftState = false;
        }

        void OnClose() override
        {
            RideConstructionInvalidateCurrentTrack();
            ViewportSetVisibility(ViewportVisibility::Default);

            MapInvalidateMapSelectionTiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            // In order to cancel the yellow arrow correctly the
            // selection tool should be cancelled. Don't do a tool cancel if
            // another window has already taken control of tool.
            if (isToolActive(classification, number))
                ToolCancel();

            HideGridlines();

            // If we demolish a currentRide all windows will be closed including the construction window,
            // the currentRide at this point is already gone.
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            if (RideTryGetOriginElement(*currentRide, nullptr))
            {
                // Auto open shops if required.
                if (currentRide->mode == RideMode::ShopStall && Config::Get().general.AutoOpenShops)
                {
                    // HACK: Until we find a good a way to defer the game command for opening the shop, stop this
                    //       from getting stuck in an infinite loop as opening the currentRide will try to close this window
                    if (!_autoOpeningShop)
                    {
                        _autoOpeningShop = true;
                        auto gameAction = RideSetStatusAction(currentRide->id, RideStatus::Open);
                        GameActions::Execute(&gameAction);
                        _autoOpeningShop = false;
                    }
                }

                currentRide->SetToDefaultInspectionInterval();
                auto intent = Intent(WindowClass::Ride);
                intent.PutExtra(INTENT_EXTRA_RIDE_ID, currentRide->id.ToUnderlying());
                ContextOpenIntent(&intent);
            }
            else
            {
                auto gameAction = RideDemolishAction(currentRide->id, RIDE_MODIFY_DEMOLISH);
                gameAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
                GameActions::Execute(&gameAction);
            }
        }

        void OnResize() override
        {
            ResizeFrame();
            WindowRideConstructionUpdateEnabledTrackPieces();

            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }
            const auto& rtd = currentRide->GetRideTypeDescriptor();
            const auto currentTrackDrawerDescriptor = getCurrentTrackDrawerDescriptor(rtd);

            uint64_t disabledWidgets = 0;

            if (_currentlySelectedTrack.isTrackType)
            {
                disabledWidgets |= (1uLL << WIDX_SLOPE_GROUPBOX) | (1uLL << WIDX_BANKING_GROUPBOX)
                    | (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL)
                    | (1uLL << WIDX_SLOPE_UP) | (1uLL << WIDX_SLOPE_UP_STEEP) | (1uLL << WIDX_CHAIN_LIFT)
                    | (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_STRAIGHT) | (1uLL << WIDX_BANK_RIGHT);
            }

            // Disable large curves if the start or end of the track is sloped and large sloped curves are not available
            if ((_previousTrackPitchEnd != TrackPitch::None || _currentTrackPitchEnd != TrackPitch::None))
            {
                if (!IsTrackEnabled(TrackGroup::slopeCurveLarge)
                    || !(_previousTrackPitchEnd == TrackPitch::Up25 || _previousTrackPitchEnd == TrackPitch::Down25)
                    || !(_currentTrackPitchEnd == TrackPitch::Up25 || _currentTrackPitchEnd == TrackPitch::Down25))
                {
                    disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE);
                }
            }
            if (IsTrackEnabled(TrackGroup::slopeCurve) && IsTrackEnabled(TrackGroup::curveVerySmall))
            {
                // Disable small curves if the start or end of the track is sloped.
                if (_previousTrackPitchEnd != TrackPitch::None || _currentTrackPitchEnd != TrackPitch::None)
                {
                    disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
                }
            }
            if (!IsTrackEnabled(TrackGroup::slopeCurve))
            {
                if (IsTrackEnabled(TrackGroup::curveVertical))
                {
                    // Disable all curves only on vertical track
                    if (_previousTrackPitchEnd != TrackPitch::Up90 || _currentTrackPitchEnd != TrackPitch::Up90)
                    {
                        if (_previousTrackPitchEnd != TrackPitch::Down90 || _currentTrackPitchEnd != TrackPitch::Down90)
                        {
                            disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE_SMALL)
                                | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                                | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
                        }
                    }
                }
                else
                {
                    // Disable all curves on sloped track
                    if (_previousTrackPitchEnd != TrackPitch::None || _currentTrackPitchEnd != TrackPitch::None)
                    {
                        disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE_SMALL)
                            | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                            | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
                    }
                }
            }
            if (!IsTrackEnabled(TrackGroup::flatRollBanking))
            {
                // Disable banking
                disabledWidgets |= (1uLL << WIDX_BANKING_GROUPBOX) | (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_STRAIGHT)
                    | (1uLL << WIDX_BANK_RIGHT);
            }
            // Disable banking if the start track is steep and the end of the track becomes flat.
            if ((_previousTrackPitchEnd == TrackPitch::Down60 || _previousTrackPitchEnd == TrackPitch::Up60)
                && _currentTrackPitchEnd == TrackPitch::None)
            {
                disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
            }
            if (!IsTrackEnabled(TrackGroup::slope) && !IsTrackEnabled(TrackGroup::slopeSteepDown)
                && !IsTrackEnabled(TrackGroup::slopeSteepUp))
            {
                if (!currentRide->GetRideTypeDescriptor().SupportsTrackGroup(TrackGroup::reverseFreefall))
                {
                    // Disable all slopes
                    disabledWidgets |= (1uLL << WIDX_SLOPE_GROUPBOX) | (1uLL << WIDX_SLOPE_DOWN_STEEP)
                        | (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP)
                        | (1uLL << WIDX_SLOPE_UP_STEEP);
                }
            }
            // If ride type does not have access to diagonal sloped turns, disallow simultaneous use of banked and sloped
            // diagonals
            if (!IsTrackEnabled(TrackGroup::slopeCurveLarge) && TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection))
            {
                if (_currentTrackPitchEnd != TrackPitch::None)
                {
                    disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                }
                else if (_currentTrackRollEnd != TrackRoll::None)
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_SLOPE_UP);
                }
            }
            if (currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::upInclineRequiresLift)
                && !GetGameState().Cheats.EnableAllDrawableTrackPieces)
            {
                // Disable lift hill toggle and banking if current track piece is uphill
                if (_previousTrackPitchEnd == TrackPitch::Up25 || _previousTrackPitchEnd == TrackPitch::Up60
                    || _currentTrackPitchEnd == TrackPitch::Up25 || _currentTrackPitchEnd == TrackPitch::Up60)
                    disabledWidgets |= 1uLL << WIDX_CHAIN_LIFT | (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                // Disable upward slope if current track piece is not flat
                if ((_previousTrackPitchEnd != TrackPitch::None || _previousTrackRollEnd != TrackRoll::None)
                    && !(_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED))
                    disabledWidgets |= (1uLL << WIDX_SLOPE_UP);
            }
            if (_rideConstructionState == RideConstructionState::State0)
            {
                disabledWidgets |= (1uLL << WIDX_CONSTRUCT) | (1uLL << WIDX_DEMOLISH) | (1uLL << WIDX_PREVIOUS_SECTION)
                    | (1uLL << WIDX_NEXT_SECTION);
            }
            if (!_currentlySelectedTrack.isTrackType)
            {
                switch (_currentlySelectedTrack.curve)
                {
                    case TrackCurve::LeftVerySmall:
                    case TrackCurve::LeftSmall:
                    case TrackCurve::Left:
                    case TrackCurve::LeftLarge:
                        disabledWidgets |= (1uLL << WIDX_BANK_RIGHT);
                        if (_previousTrackRollEnd == TrackRoll::None)
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_LEFT);
                        }
                        else
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_STRAIGHT);
                        }
                        break;
                    case TrackCurve::RightLarge:
                    case TrackCurve::Right:
                    case TrackCurve::RightSmall:
                    case TrackCurve::RightVerySmall:
                        disabledWidgets |= (1uLL << WIDX_BANK_LEFT);
                        if (_previousTrackRollEnd == TrackRoll::None)
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_RIGHT);
                        }
                        else
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_STRAIGHT);
                        }
                        break;
                    default:
                        break;
                }
            }
            if (!IsTrackEnabled(TrackGroup::slopeRollBanking))
            {
                if (_currentTrackRollEnd != TrackRoll::None)
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_SLOPE_UP);
                }
            }
            if (_previousTrackPitchEnd == _currentTrackPitchEnd)
            {
                switch (_currentTrackPitchEnd)
                {
                    case TrackPitch::Up60:
                    case TrackPitch::Down60:
                        disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE)
                            | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
                        if (!IsTrackEnabled(TrackGroup::slopeCurveSteep))
                        {
                            disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE_SMALL);
                        }
                        break;
                    case TrackPitch::Up90:
                    case TrackPitch::Down90:
                        disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE)
                            | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
                        if (!IsTrackEnabled(TrackGroup::curveVertical))
                        {
                            disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE_SMALL);
                        }
                        break;
                    default:
                        break;
                }
            }
            else
            {
                // Disable all curves
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE_SMALL)
                    | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                    | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
            }

            switch (_previousTrackPitchEnd)
            {
                case TrackPitch::None:
                    if (_currentlySelectedTrack != TrackCurve::None
                        || (IsTrackEnabled(TrackGroup::slopeSteepLong)
                            && TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                    }
                    break;
                case TrackPitch::Down25:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_UP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                    break;
                case TrackPitch::Down60:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_UP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                    if (!IsTrackEnabled(TrackGroup::slopeLong)
                        && !(
                            IsTrackEnabled(TrackGroup::slopeSteepLong)
                            && !TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
                    {
                        disabledWidgets |= (1uLL << WIDX_LEVEL);
                    }
                    break;
                case TrackPitch::Up25:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN);
                    break;
                case TrackPitch::Up60:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN);
                    if (!IsTrackEnabled(TrackGroup::slopeLong)
                        && !(
                            IsTrackEnabled(TrackGroup::slopeSteepLong)
                            && !TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
                    {
                        disabledWidgets |= (1uLL << WIDX_LEVEL);
                    }
                    break;
                case TrackPitch::Down90:
                case TrackPitch::Up90:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP);
                    break;
            }
            if (_previousTrackPitchEnd == TrackPitch::None)
            {
                if (!IsTrackEnabled(TrackGroup::slopeLong) && !IsTrackEnabled(TrackGroup::slopeSteepLong))
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                }
            }
            if (IsTrackEnabled(TrackGroup::slopeVertical))
            {
                if (_previousTrackPitchEnd == TrackPitch::Up60 && _currentTrackPieceDirection < 4)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_STEEP);
                }
                if (_previousTrackPitchEnd == TrackPitch::Up90)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_STEEP);
                }
                if (_previousTrackPitchEnd == TrackPitch::Down60 && _currentTrackPieceDirection < 4)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_STEEP);
                }
            }
            if (_previousTrackRollEnd == TrackRoll::Left)
            {
                disabledWidgets |= (1uLL << WIDX_RIGHT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE)
                    | (1uLL << WIDX_RIGHT_CURVE_LARGE) | (1uLL << WIDX_BANK_RIGHT);
            }
            if (_previousTrackRollEnd == TrackRoll::Right)
            {
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_LEFT_CURVE_LARGE)
                    | (1uLL << WIDX_BANK_LEFT);
            }
            if (_currentTrackRollEnd != _previousTrackRollEnd)
            {
                disabledWidgets |= (1uLL << WIDX_RIGHT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE)
                    | (1uLL << WIDX_RIGHT_CURVE_LARGE) | (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE)
                    | (1uLL << WIDX_LEFT_CURVE_LARGE);
            }
            if (_currentTrackPitchEnd != TrackPitch::None)
            {
                if (IsTrackEnabled(TrackGroup::slopeRollBanking))
                {
                    if (_previousTrackPitchEnd == TrackPitch::None)
                    {
                        if (_currentTrackPitchEnd != TrackPitch::Up25 && _currentTrackPitchEnd != TrackPitch::Down25)
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                        }
                    }
                    else
                    {
                        if (_currentTrackPitchEnd != _previousTrackPitchEnd)
                        {
                            disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                        }
                        else
                        {
                            if (_currentTrackPitchEnd != TrackPitch::Up25 && _currentTrackPitchEnd != TrackPitch::Down25)
                            {
                                disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                            }
                        }
                    }
                }
                else
                {
                    disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                }
            }
            if (_currentTrackRollEnd != TrackRoll::None || _previousTrackRollEnd != TrackRoll::None)
            {
                disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_UP_STEEP) | (1uLL << WIDX_CHAIN_LIFT);
            }
            if (_currentlySelectedTrack != TrackCurve::None)
            {
                if (!IsTrackEnabled(TrackGroup::liftHillCurve))
                {
                    disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
                }
                if (_currentTrackPitchEnd == TrackPitch::None)
                {
                    disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
                }
                if (_currentTrackPitchEnd == TrackPitch::Up60)
                {
                    disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
                }
                if (_currentTrackPitchEnd == TrackPitch::Down60)
                {
                    disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
                }
            }
            if (_currentTrackPitchEnd == TrackPitch::Up90 || _previousTrackPitchEnd == TrackPitch::Up90)
            {
                disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
            }
            if (!IsTrackEnabled(TrackGroup::liftHillSteep))
            {
                if (_previousTrackPitchEnd == TrackPitch::Up60 || _currentTrackPitchEnd == TrackPitch::Up60)
                {
                    disabledWidgets |= (1uLL << WIDX_CHAIN_LIFT);
                }
            }
            if (_previousTrackRollEnd == TrackRoll::UpsideDown)
            {
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_LEFT_CURVE_LARGE)
                    | (1uLL << WIDX_STRAIGHT) | (1uLL << WIDX_RIGHT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE)
                    | (1uLL << WIDX_RIGHT_CURVE_LARGE);
            }
            if (_currentlySelectedTrack != TrackCurve::None)
            {
                if (_currentTrackPitchEnd == TrackPitch::None)
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_SLOPE_UP);
                }
                if (_currentTrackPitchEnd == _previousTrackPitchEnd)
                {
                    if (_currentTrackPitchEnd == TrackPitch::Up25)
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_UP_STEEP);
                        if (_currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right
                            || _rideConstructionState != RideConstructionState::Back
                            || !IsTrackEnabled(TrackGroup::slopeCurveBanked))
                        {
                            disabledWidgets |= (1uLL << WIDX_LEVEL);
                        }
                    }
                    if (_currentTrackPitchEnd == TrackPitch::Down25)
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP);
                        if (_currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right
                            || _rideConstructionState != RideConstructionState::Front
                            || !IsTrackEnabled(TrackGroup::slopeCurveBanked))
                        {
                            disabledWidgets |= (1uLL << WIDX_LEVEL);
                        }
                    }
                }
                else if (IsTrackEnabled(TrackGroup::slopeCurveBanked))
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                    if (_currentTrackRollEnd == TrackRoll::Left)
                    {
                        disabledWidgets |= (1uLL << WIDX_BANK_STRAIGHT) | (1uLL << WIDX_BANK_RIGHT);
                        disabledWidgets &= ~(1uLL << WIDX_BANK_LEFT);
                    }
                    if (_currentTrackRollEnd == TrackRoll::Right)
                    {
                        disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_STRAIGHT);
                        disabledWidgets &= ~(1uLL << WIDX_BANK_RIGHT);
                    }
                    if (_currentTrackRollEnd == TrackRoll::None)
                    {
                        disabledWidgets |= (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                        disabledWidgets &= ~(1uLL << WIDX_BANK_STRAIGHT);
                    }
                    if (_currentTrackPitchEnd == TrackPitch::None)
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_SLOPE_UP);
                        disabledWidgets &= ~(1uLL << WIDX_LEVEL);
                    }
                    if (_currentTrackPitchEnd == TrackPitch::Up25)
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL);
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP);
                    }
                    if (_currentTrackPitchEnd == TrackPitch::Down25)
                    {
                        disabledWidgets |= (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP);
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN);
                    }
                    if (_currentlySelectedTrack == TrackCurve::LeftSmall)
                    {
                        disabledWidgets &= ~(1uLL << WIDX_LEFT_CURVE_SMALL);
                    }
                    if (_currentlySelectedTrack == TrackCurve::RightSmall)
                    {
                        disabledWidgets &= ~(1uLL << WIDX_RIGHT_CURVE_SMALL);
                    }
                }
            }
            if (_currentlySelectedTrack != TrackCurve::None && _currentTrackPitchEnd == TrackPitch::Up60)
            {
                disabledWidgets |= (1uLL << WIDX_SLOPE_UP);
            }
            if (_currentlySelectedTrack != TrackCurve::None && _currentTrackPitchEnd == TrackPitch::Down60)
            {
                disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN);
            }
            if ((_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED) && !GetGameState().Cheats.EnableChainLiftOnAllTrack)
            {
                if (_currentTrackPitchEnd != TrackPitch::None && !IsTrackEnabled(TrackGroup::liftHillCurve))
                {
                    disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE)
                        | (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE)
                        | (1uLL << WIDX_RIGHT_CURVE_LARGE);
                }
                if (!IsTrackEnabled(TrackGroup::liftHillSteep))
                {
                    if (widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
                    {
                        disabledWidgets |= (1uLL << WIDX_SLOPE_UP_STEEP);
                    }
                }
            }
            if (_previousTrackPitchEnd == TrackPitch::Up60 && _currentlySelectedTrack != TrackCurve::None)
            {
                disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_LEVEL);
            }
            if (_previousTrackPitchEnd == TrackPitch::Down60 && _currentlySelectedTrack != TrackCurve::None)
            {
                disabledWidgets |= (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP_STEEP);
            }
            if (_currentTrackPitchEnd == TrackPitch::Up90 || _previousTrackPitchEnd == TrackPitch::Up90)
            {
                if (_currentlySelectedTrack != TrackCurve::None)
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_UP_STEEP);
                }
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE);
                if (currentRide->GetRideTypeDescriptor().SupportsTrackGroup(TrackGroup::reverseFreefall))
                {
                    disabledWidgets |= (1uLL << WIDX_STRAIGHT) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                        | (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE);
                }
            }
            else if (_currentTrackPitchEnd == TrackPitch::Down90 || _previousTrackPitchEnd == TrackPitch::Down90)
            {
                if (_currentlySelectedTrack != TrackCurve::None)
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP);
                }
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE);
                if (currentRide->GetRideTypeDescriptor().SupportsTrackGroup(TrackGroup::reverseFreefall))
                {
                    disabledWidgets |= (1uLL << WIDX_STRAIGHT) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                        | (1uLL << WIDX_LEFT_CURVE_SMALL) | (1uLL << WIDX_LEFT_CURVE);
                }
            }
            // If the previous track is flat and the next track is flat, attempt to show buttons for helixes
            if (_currentTrackPitchEnd == TrackPitch::None && _currentTrackPitchEnd == _previousTrackPitchEnd)
            {
                // If the bank is none, attempt to show unbanked quarter helixes
                if (_currentTrackRollEnd == TrackRoll::None
                    && (_currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right))
                {
                    if (IsTrackEnabled(TrackGroup::helixDownUnbankedQuarter))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_STEEP);
                    if (IsTrackEnabled(TrackGroup::helixUpUnbankedQuarter))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_STEEP);
                }
                // If the track is banked left or right and curvature is standard size (2.5 tile radius), attempt to show
                // buttons for half or quarter helixes
                else if (
                    (_currentTrackRollEnd == TrackRoll::Left || _currentTrackRollEnd == TrackRoll::Right)
                    && (_currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right))
                {
                    if (IsTrackEnabled(TrackGroup::helixDownBankedHalf) || IsTrackEnabled(TrackGroup::helixDownBankedQuarter))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_STEEP);
                    if (IsTrackEnabled(TrackGroup::helixUpBankedHalf) || IsTrackEnabled(TrackGroup::helixUpBankedQuarter))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_STEEP);
                }
                // If the track is banked left or right and curvature is small size (1.5 tile radius), attempt to show buttons
                // for half helixes
                else if (
                    (_currentTrackRollEnd == TrackRoll::Left || _currentTrackRollEnd == TrackRoll::Right)
                    && (_currentlySelectedTrack == TrackCurve::LeftSmall || _currentlySelectedTrack == TrackCurve::RightSmall))
                {
                    if (IsTrackEnabled(TrackGroup::helixDownBankedHalf))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_STEEP);
                    if (IsTrackEnabled(TrackGroup::helixUpBankedHalf))
                        disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_STEEP);
                }
            }
            if (IsTrackEnabled(TrackGroup::slopeCurveBanked))
            {
                if (_rideConstructionState == RideConstructionState::Front)
                {
                    if (_currentlySelectedTrack == TrackCurve::LeftSmall || _currentlySelectedTrack == TrackCurve::RightSmall)
                    {
                        if (_currentTrackPitchEnd == TrackPitch::None && _previousTrackRollEnd != TrackRoll::None
                            && (!currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::upInclineRequiresLift)
                                || GetGameState().Cheats.EnableAllDrawableTrackPieces))
                        {
                            disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP);
                        }
                    }
                }
                else if (_rideConstructionState == RideConstructionState::Back)
                {
                    if (_currentlySelectedTrack == TrackCurve::LeftSmall || _currentlySelectedTrack == TrackCurve::RightSmall)
                    {
                        if (_currentTrackPitchEnd == TrackPitch::None && _previousTrackRollEnd != TrackRoll::None)
                        {
                            disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN);
                        }
                    }
                }
            }
            if (_currentTrackPieceDirection >= 4)
            {
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE_SMALL)
                    | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_RIGHT_CURVE) | (1uLL << WIDX_RIGHT_CURVE_SMALL)
                    | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            if (_rideConstructionState == RideConstructionState::Front)
            {
                disabledWidgets |= (1uLL << WIDX_NEXT_SECTION);
                if (WindowRideConstructionUpdateState(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
                {
                    disabledWidgets |= (1uLL << WIDX_CONSTRUCT);
                }
            }
            else if (_rideConstructionState == RideConstructionState::Back)
            {
                disabledWidgets |= (1uLL << WIDX_PREVIOUS_SECTION);
                if (WindowRideConstructionUpdateState(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
                {
                    disabledWidgets |= (1uLL << WIDX_CONSTRUCT);
                }
            }
            if (currentTrackDrawerDescriptor.HasCoveredPieces())
            {
                disabledWidgets &= ~(1uLL << WIDX_BANKING_GROUPBOX);
            }
            if (_rideConstructionState == RideConstructionState::EntranceExit
                || _rideConstructionState == RideConstructionState::Selected)
            {
                disabledWidgets |= (1uLL << WIDX_DIRECTION_GROUPBOX) | (1uLL << WIDX_SLOPE_GROUPBOX)
                    | (1uLL << WIDX_BANKING_GROUPBOX) | (1uLL << WIDX_LEFT_CURVE_VERY_SMALL) | (1uLL << WIDX_LEFT_CURVE_SMALL)
                    | (1uLL << WIDX_LEFT_CURVE) | (1uLL << WIDX_STRAIGHT) | (1uLL << WIDX_RIGHT_CURVE)
                    | (1uLL << WIDX_RIGHT_CURVE_SMALL) | (1uLL << WIDX_RIGHT_CURVE_VERY_SMALL)
                    | (1uLL << WIDX_SPECIAL_TRACK_DROPDOWN) | (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN)
                    | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP) | (1uLL << WIDX_SLOPE_UP_STEEP) | (1uLL << WIDX_CHAIN_LIFT)
                    | (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_STRAIGHT) | (1uLL << WIDX_BANK_RIGHT)
                    | (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE);
            }
            if (_currentlyShowingBrakeOrBoosterSpeed)
            {
                disabledWidgets &= ~(1uLL << WIDX_BANKING_GROUPBOX);
                disabledWidgets &= ~(1uLL << WIDX_BANK_LEFT);
                disabledWidgets &= ~(1uLL << WIDX_BANK_STRAIGHT);
                disabledWidgets &= ~(1uLL << WIDX_BANK_RIGHT);
            }

            // If chain lift cheat is enabled then show the chain lift widget no matter what
            if (GetGameState().Cheats.EnableChainLiftOnAllTrack)
            {
                disabledWidgets &= ~(1uLL << WIDX_CHAIN_LIFT);
            }

            // Set and invalidate the changed widgets
            uint64_t currentDisabledWidgets = disabled_widgets;
            if (currentDisabledWidgets == disabledWidgets)
                return;

            for (WidgetIndex i = 0; i < 64; i++)
            {
                if ((disabledWidgets & (1uLL << i)) != (currentDisabledWidgets & (1uLL << i)))
                {
                    WidgetInvalidate(*this, i);
                }
            }
            disabled_widgets = disabledWidgets;
        }

        void OnUpdate() override
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            // Close construction window if currentRide is not closed,
            // editing currentRide while open will cause many issues until properly handled
            if (currentRide->status != RideStatus::Closed && currentRide->status != RideStatus::Simulating)
            {
                Close();
                return;
            }

            if (_currentlySelectedTrack.isTrackType)
            {
                switch (_currentlySelectedTrack.trackType)
                {
                    case TrackElemType::SpinningTunnel:
                    case TrackElemType::Whirlpool:
                    case TrackElemType::Rapids:
                    case TrackElemType::Waterfall:
                        WidgetInvalidate(*this, WIDX_CONSTRUCT);
                        break;
                }
            }

            if (_rideConstructionState == RideConstructionState::Place)
            {
                if (!isToolActive(*this, WIDX_CONSTRUCT))
                {
                    Close();
                    return;
                }
            }

            if (_rideConstructionState == RideConstructionState::EntranceExit)
            {
                if (!isToolActive(*this, WIDX_ENTRANCE) && !isToolActive(*this, WIDX_EXIT))
                {
                    _rideConstructionState = gRideEntranceExitPlacePreviousRideConstructionState;
                    WindowRideConstructionUpdateActiveElements();
                }
            }

            switch (_rideConstructionState)
            {
                case RideConstructionState::Front:
                case RideConstructionState::Back:
                case RideConstructionState::Selected:
                    if (isToolActive(WindowClass::RideConstruction))
                    {
                        ToolCancel();
                    }
                    break;
                default:
                    break;
            }

            UpdateGhostTrackAndArrow();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            WindowRideConstructionUpdateEnabledTrackPieces();
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_ROTATE:
                    Rotate();
                    break;
                case WIDX_ENTRANCE:
                    EntranceClick();
                    break;
                case WIDX_EXIT:
                    ExitClick();
                    break;
                case WIDX_SIMULATE:
                {
                    auto currentRide = GetRide(_currentRideIndex);
                    if (currentRide != nullptr)
                    {
                        auto status = currentRide->status == RideStatus::Simulating ? RideStatus::Closed
                                                                                    : RideStatus::Simulating;
                        auto gameAction = RideSetStatusAction(currentRide->id, status);
                        GameActions::Execute(&gameAction);
                    }
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            WindowRideConstructionUpdateEnabledTrackPieces();
            switch (widgetIndex)
            {
                case WIDX_CONSTRUCT:
                {
                    Construct();
                    // Force any footpath construction to recheck the area.
                    FootpathRecheckProvisional();
                    break;
                }
                case WIDX_DEMOLISH:
                    MouseUpDemolish();
                    break;
                case WIDX_NEXT_SECTION:
                    VirtualFloorInvalidate();
                    RideSelectNextSection();
                    if (!isToolActive(WindowClass::Scenery))
                        VirtualFloorSetHeight(_currentTrackBegin.z);
                    break;
                case WIDX_PREVIOUS_SECTION:
                    VirtualFloorInvalidate();
                    RideSelectPreviousSection();
                    if (!isToolActive(WindowClass::Scenery))
                        VirtualFloorSetHeight(_currentTrackBegin.z);
                    break;
                case WIDX_LEFT_CURVE:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::Left;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_RIGHT_CURVE:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::Right;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_LEFT_CURVE_SMALL:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::LeftSmall;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_RIGHT_CURVE_SMALL:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::RightSmall;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_LEFT_CURVE_VERY_SMALL:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::LeftVerySmall;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_RIGHT_CURVE_VERY_SMALL:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::RightVerySmall;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_LEFT_CURVE_LARGE:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::LeftLarge;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_RIGHT_CURVE_LARGE:
                    RideConstructionInvalidateCurrentTrack();
                    _currentlySelectedTrack = TrackCurve::RightLarge;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_STRAIGHT:
                    RideConstructionInvalidateCurrentTrack();
                    if (_currentlySelectedTrack != TrackCurve::None)
                        _currentTrackRollEnd = TrackRoll::None;
                    _currentlySelectedTrack = TrackCurve::None;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_SLOPE_DOWN_STEEP:
                    RideConstructionInvalidateCurrentTrack();
                    if (IsTrackEnabled(TrackGroup::helixDownBankedHalf) || IsTrackEnabled(TrackGroup::helixUpBankedHalf))
                    {
                        if (_currentlySelectedTrack == TrackCurve::Left && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftHalfBankedHelixDownLarge;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::Right && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightHalfBankedHelixDownLarge;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::LeftSmall && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftHalfBankedHelixDownSmall;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::RightSmall && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightHalfBankedHelixDownSmall;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                    }
                    if (IsTrackEnabled(TrackGroup::helixDownBankedQuarter) || IsTrackEnabled(TrackGroup::helixUpBankedQuarter))
                    {
                        if (_currentlySelectedTrack == TrackCurve::Left && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftQuarterBankedHelixLargeDown;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::Right && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightQuarterBankedHelixLargeDown;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                    }
                    if (IsTrackEnabled(TrackGroup::helixDownUnbankedQuarter)
                        || IsTrackEnabled(TrackGroup::helixUpUnbankedQuarter))
                    {
                        if (_currentTrackRollEnd == TrackRoll::None)
                        {
                            if (_currentlySelectedTrack == TrackCurve::Left)
                            {
                                _currentlySelectedTrack = TrackElemType::LeftQuarterHelixLargeDown;
                                _currentTrackPrice = kMoney64Undefined;
                                WindowRideConstructionUpdateActiveElements();
                                break;
                            }
                            if (_currentlySelectedTrack == TrackCurve::Right)
                            {
                                _currentlySelectedTrack = TrackElemType::RightQuarterHelixLargeDown;
                                _currentTrackPrice = kMoney64Undefined;
                                WindowRideConstructionUpdateActiveElements();
                                break;
                            }
                        }
                    }
                    if (widgets[WIDX_SLOPE_DOWN_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP)
                    {
                        UpdateLiftHillSelected(TrackPitch::Down60);
                    }
                    else
                    {
                        UpdateLiftHillSelected(TrackPitch::Up90);
                    }
                    break;
                case WIDX_SLOPE_DOWN:
                    RideConstructionInvalidateCurrentTrack();
                    if (_rideConstructionState == RideConstructionState::Back && _currentTrackRollEnd != TrackRoll::None)
                    {
                        _currentTrackRollEnd = TrackRoll::None;
                    }
                    UpdateLiftHillSelected(TrackPitch::Down25);
                    break;
                case WIDX_LEVEL:
                    RideConstructionInvalidateCurrentTrack();
                    if (_rideConstructionState == RideConstructionState::Front && _previousTrackPitchEnd == TrackPitch::Down25)
                    {
                        if (_currentlySelectedTrack == TrackCurve::LeftSmall)
                        {
                            _currentTrackRollEnd = TrackRoll::Left;
                        }
                        else if (_currentlySelectedTrack == TrackCurve::RightSmall)
                        {
                            _currentTrackRollEnd = TrackRoll::Right;
                        }
                    }
                    else if (
                        _rideConstructionState == RideConstructionState::Back && _previousTrackPitchEnd == TrackPitch::Up25)
                    {
                        if (_currentlySelectedTrack == TrackCurve::LeftSmall)
                        {
                            _currentTrackRollEnd = TrackRoll::Left;
                        }
                        else if (_currentlySelectedTrack == TrackCurve::RightSmall)
                        {
                            _currentTrackRollEnd = TrackRoll::Right;
                        }
                    }
                    UpdateLiftHillSelected(TrackPitch::None);
                    break;
                case WIDX_SLOPE_UP:
                    RideConstructionInvalidateCurrentTrack();
                    if (_rideConstructionState == RideConstructionState::Front && _currentTrackRollEnd != TrackRoll::None)
                    {
                        _currentTrackRollEnd = TrackRoll::None;
                    }
                    if (currentRide->GetRideTypeDescriptor().SupportsTrackGroup(TrackGroup::reverseFreefall))
                    {
                        if (_rideConstructionState == RideConstructionState::Front
                            && _currentlySelectedTrack == TrackCurve::None)
                        {
                            _currentlySelectedTrack = TrackElemType::ReverseFreefallSlope;
                            WindowRideConstructionUpdateActiveElements();
                        }
                    }
                    else
                    {
                        UpdateLiftHillSelected(TrackPitch::Up25);
                    }
                    break;
                case WIDX_SLOPE_UP_STEEP:
                    RideConstructionInvalidateCurrentTrack();
                    if (IsTrackEnabled(TrackGroup::helixDownBankedHalf) || IsTrackEnabled(TrackGroup::helixUpBankedHalf))
                    {
                        if (_currentlySelectedTrack == TrackCurve::Left && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftHalfBankedHelixUpLarge;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::Right && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightHalfBankedHelixUpLarge;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::LeftSmall && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftHalfBankedHelixUpSmall;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::RightSmall && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightHalfBankedHelixUpSmall;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                    }
                    if (IsTrackEnabled(TrackGroup::helixDownBankedQuarter) || IsTrackEnabled(TrackGroup::helixUpBankedQuarter))
                    {
                        if (_currentlySelectedTrack == TrackCurve::Left && _currentTrackRollEnd == TrackRoll::Left)
                        {
                            _currentlySelectedTrack = TrackElemType::LeftQuarterBankedHelixLargeUp;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                        if (_currentlySelectedTrack == TrackCurve::Right && _currentTrackRollEnd == TrackRoll::Right)
                        {
                            _currentlySelectedTrack = TrackElemType::RightQuarterBankedHelixLargeUp;
                            _currentTrackPrice = kMoney64Undefined;
                            WindowRideConstructionUpdateActiveElements();
                            break;
                        }
                    }
                    if (IsTrackEnabled(TrackGroup::helixDownUnbankedQuarter)
                        || IsTrackEnabled(TrackGroup::helixUpUnbankedQuarter))
                    {
                        if (_currentTrackRollEnd == TrackRoll::None)
                        {
                            if (_currentlySelectedTrack == TrackCurve::Left)
                            {
                                _currentlySelectedTrack = TrackElemType::LeftQuarterHelixLargeUp;
                                _currentTrackPrice = kMoney64Undefined;
                                WindowRideConstructionUpdateActiveElements();
                                break;
                            }
                            if (_currentlySelectedTrack == TrackCurve::Right)
                            {
                                _currentlySelectedTrack = TrackElemType::RightQuarterHelixLargeUp;
                                _currentTrackPrice = kMoney64Undefined;
                                WindowRideConstructionUpdateActiveElements();
                                break;
                            }
                        }
                    }
                    if (widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
                    {
                        UpdateLiftHillSelected(TrackPitch::Up60);
                    }
                    else
                    {
                        UpdateLiftHillSelected(TrackPitch::Down90);
                    }
                    break;
                case WIDX_CHAIN_LIFT:
                    RideConstructionInvalidateCurrentTrack();
                    _currentTrackLiftHill ^= CONSTRUCTION_LIFT_HILL_SELECTED;
                    if ((_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
                        && !GetGameState().Cheats.EnableChainLiftOnAllTrack)
                        _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_BANK_LEFT:
                    RideConstructionInvalidateCurrentTrack();
                    if (!_currentlyShowingBrakeOrBoosterSpeed)
                    {
                        _currentTrackRollEnd = TrackRoll::Left;
                        _currentTrackPrice = kMoney64Undefined;
                        WindowRideConstructionUpdateActiveElements();
                    }
                    break;
                case WIDX_BANK_STRAIGHT:
                    RideConstructionInvalidateCurrentTrack();
                    if (!_currentlyShowingBrakeOrBoosterSpeed)
                    {
                        _currentTrackRollEnd = TrackRoll::None;
                        _currentTrackPrice = kMoney64Undefined;
                        WindowRideConstructionUpdateActiveElements();
                    }
                    else
                    {
                        auto trackSpeedMaximum = kMaximumTrackSpeed;
                        auto trackSpeedIncrement = kDefaultSpeedIncrement;
                        uint8_t brakesSpeed = std::min<int16_t>(trackSpeedMaximum, _currentBrakeSpeed + trackSpeedIncrement);
                        if (brakesSpeed != _currentBrakeSpeed)
                        {
                            if (_rideConstructionState == RideConstructionState::Selected)
                            {
                                SetBrakeSpeed(brakesSpeed);
                            }
                            else
                            {
                                _currentBrakeSpeed = brakesSpeed;
                                WindowRideConstructionUpdateActiveElements();
                            }
                        }
                    }
                    break;
                case WIDX_BANK_RIGHT:
                    RideConstructionInvalidateCurrentTrack();
                    if (!_currentlyShowingBrakeOrBoosterSpeed)
                    {
                        _currentTrackRollEnd = TrackRoll::Right;
                        _currentTrackPrice = kMoney64Undefined;
                        WindowRideConstructionUpdateActiveElements();
                    }
                    else
                    {
                        auto trackSpeedIncrement = kDefaultSpeedIncrement;
                        auto trackSpeedMinimum = kDefaultMinimumSpeed;
                        if (GetGameState().Cheats.UnlockOperatingLimits)
                        {
                            trackSpeedMinimum = 0;
                        }
                        uint8_t brakesSpeed = std::max<int16_t>(trackSpeedMinimum, _currentBrakeSpeed - trackSpeedIncrement);
                        if (brakesSpeed != _currentBrakeSpeed)
                        {
                            if (_rideConstructionState == RideConstructionState::Selected)
                            {
                                SetBrakeSpeed(brakesSpeed);
                            }
                            else
                            {
                                _currentBrakeSpeed = brakesSpeed;
                                WindowRideConstructionUpdateActiveElements();
                            }
                        }
                    }
                    break;
                case WIDX_SPECIAL_TRACK_DROPDOWN:
                    ShowSpecialTrackDropdown(&widgets[widgetIndex]);
                    break;
                case WIDX_U_TRACK:
                    RideConstructionInvalidateCurrentTrack();
                    _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_O_TRACK:
                    RideConstructionInvalidateCurrentTrack();
                    _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
                    if (!GetGameState().Cheats.EnableChainLiftOnAllTrack)
                        _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP:
                    if (_currentSeatRotationAngle < 15)
                    {
                        if (_rideConstructionState == RideConstructionState::Selected)
                        {
                            RideSelectedTrackSetSeatRotation(_currentSeatRotationAngle + 1);
                        }
                        else
                        {
                            _currentSeatRotationAngle++;
                            WindowRideConstructionUpdateActiveElements();
                        }
                    }
                    break;
                case WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN:
                    if (_currentSeatRotationAngle > 0)
                    {
                        if (_rideConstructionState == RideConstructionState::Selected)
                        {
                            RideSelectedTrackSetSeatRotation(_currentSeatRotationAngle - 1);
                        }
                        else
                        {
                            _currentSeatRotationAngle--;
                            WindowRideConstructionUpdateActiveElements();
                        }
                    }
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (widgetIndex != WIDX_SPECIAL_TRACK_DROPDOWN)
                return;
            if (selectedIndex == -1)
                return;

            RideConstructionInvalidateCurrentTrack();
            _currentTrackPrice = kMoney64Undefined;
            track_type_t trackPiece = _specialElementDropdownState.Elements[selectedIndex].TrackType;
            switch (trackPiece)
            {
                case TrackElemType::EndStation:
                case TrackElemType::SBendLeft:
                case TrackElemType::SBendRight:
                    _currentTrackPitchEnd = TrackPitch::None;
                    break;
                case TrackElemType::LeftVerticalLoop:
                case TrackElemType::RightVerticalLoop:
                    _currentTrackRollEnd = TrackRoll::None;
                    _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
                    break;
                case TrackElemType::BlockBrakes:
                case TrackElemType::DiagBlockBrakes:
                    _currentBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;
            }
            _currentlySelectedTrack = trackPiece;
            WindowRideConstructionUpdateActiveElements();
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_CONSTRUCT:
                    RideConstructionToolupdateConstruct(screenCoords);
                    break;
                case WIDX_ENTRANCE:
                case WIDX_EXIT:
                    RideConstructionToolupdateEntranceExit(screenCoords);
                    break;
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_CONSTRUCT:
                    RideConstructionTooldownConstruct(screenCoords);
                    break;
                case WIDX_ENTRANCE:
                case WIDX_EXIT:
                    ToolDownEntranceExit(screenCoords);
                    break;
            }
        }

        void OnPrepareDraw() override
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            StringId stringId = STR_RIDE_CONSTRUCTION_SPECIAL;
            if (_currentlySelectedTrack.isTrackType)
            {
                const auto& rtd = currentRide->GetRideTypeDescriptor();
                const auto& ted = GetTrackElementDescriptor(_currentlySelectedTrack.trackType);
                stringId = ted.description;
                if (stringId == STR_RAPIDS && rtd.Category != RIDE_CATEGORY_WATER)
                {
                    stringId = STR_LOG_BUMPS;
                }
            }
            auto ft = Formatter::Common();
            ft.Add<uint16_t>(stringId);

            if (_currentlyShowingBrakeOrBoosterSpeed)
            {
                uint16_t brakeSpeed2 = ((_currentBrakeSpeed * 9) >> 2) & 0xFFFF;
                if (TrackTypeIsBooster(_selectedTrackType) || TrackTypeIsBooster(_currentlySelectedTrack.trackType))
                {
                    brakeSpeed2 = GetBoosterSpeed(currentRide->type, brakeSpeed2);
                }
                ft.Add<uint16_t>(brakeSpeed2);
            }
            else
            {
                ft.Increment(2);
            }

            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].text = RideConstructionSeatAngleRotationStrings
                [_currentSeatRotationAngle];

            // Simulate button
            auto& simulateWidget = widgets[WIDX_SIMULATE];
            simulateWidget.type = WindowWidgetType::Empty;
            if (currentRide->SupportsStatus(RideStatus::Simulating))
            {
                simulateWidget.type = WindowWidgetType::FlatBtn;
                if (currentRide->status == RideStatus::Simulating)
                {
                    pressed_widgets |= (1uLL << WIDX_SIMULATE);
                }
                else
                {
                    pressed_widgets &= ~(1uLL << WIDX_SIMULATE);
                }
            }

            // Set window title arguments
            currentRide->FormatNameTo(ft);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawPixelInfo clipdpi;
            Widget* widget;
            int32_t widgetWidth, widgetHeight;

            DrawWidgets(dpi);

            widget = &widgets[WIDX_CONSTRUCT];
            if (widget->type == WindowWidgetType::Empty)
                return;

            RideId rideIndex;
            int32_t trackType, trackDirection, liftHillAndInvertedState;
            if (WindowRideConstructionUpdateState(
                    &trackType, &trackDirection, &rideIndex, &liftHillAndInvertedState, nullptr, nullptr))
                return;

            // Draw track piece
            auto screenCoords = ScreenCoordsXY{ windowPos.x + widget->left + 1, windowPos.y + widget->top + 1 };
            widgetWidth = widget->width() - 1;
            widgetHeight = widget->height() - 1;
            if (ClipDrawPixelInfo(clipdpi, dpi, screenCoords, widgetWidth, widgetHeight))
            {
                DrawTrackPiece(
                    clipdpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, widgetWidth, widgetHeight);
            }

            // Draw cost
            screenCoords = { windowPos.x + widget->midX(), windowPos.y + widget->bottom - 23 };
            if (_rideConstructionState != RideConstructionState::Place)
                DrawTextBasic(dpi, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::CENTRE });

            screenCoords.y += 11;
            if (_currentTrackPrice != kMoney64Undefined && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(_currentTrackPrice);
                DrawTextBasic(dpi, screenCoords, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
            }
        }

        void UpdateWidgets()
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            const auto& rtd = GetRideTypeDescriptor(currentRide->type);
            auto trackDrawerDescriptor = getCurrentTrackDrawerDescriptor(rtd);

            hold_down_widgets = (1u << WIDX_CONSTRUCT) | (1u << WIDX_DEMOLISH) | (1u << WIDX_NEXT_SECTION)
                | (1u << WIDX_PREVIOUS_SECTION);
            if (rtd.HasFlag(RtdFlag::isShopOrFacility) || !currentRide->HasStation())
            {
                widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WindowWidgetType::Empty;
                widgets[WIDX_ENTRANCE].type = WindowWidgetType::Empty;
                widgets[WIDX_EXIT].type = WindowWidgetType::Empty;
            }
            else
            {
                widgets[WIDX_ENTRANCE_EXIT_GROUPBOX].type = WindowWidgetType::Groupbox;
                widgets[WIDX_ENTRANCE].type = WindowWidgetType::Button;
                widgets[WIDX_EXIT].type = WindowWidgetType::Button;
            }

            if (_specialElementDropdownState.HasActiveElements)
            {
                widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Button;
            }
            else
            {
                widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Empty;
            }

            if (IsTrackEnabled(TrackGroup::straight))
            {
                widgets[WIDX_STRAIGHT].type = WindowWidgetType::FlatBtn;
            }
            else
            {
                widgets[WIDX_STRAIGHT].type = WindowWidgetType::Empty;
            }

            if (IsTrackEnabled(TrackGroup::curveLarge))
            {
                widgets[WIDX_LEFT_CURVE_LARGE].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_RIGHT_CURVE_LARGE].type = WindowWidgetType::FlatBtn;
            }
            else
            {
                widgets[WIDX_LEFT_CURVE_LARGE].type = WindowWidgetType::Empty;
                widgets[WIDX_RIGHT_CURVE_LARGE].type = WindowWidgetType::Empty;
            }

            widgets[WIDX_LEFT_CURVE].type = WindowWidgetType::Empty;
            widgets[WIDX_RIGHT_CURVE].type = WindowWidgetType::Empty;
            widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::Empty;
            widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::Empty;
            widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WindowWidgetType::Empty;
            widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WindowWidgetType::Empty;
            if (IsTrackEnabled(TrackGroup::curveVertical))
            {
                widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::curve))
            {
                widgets[WIDX_LEFT_CURVE].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_RIGHT_CURVE].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::curveSmall))
            {
                widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::curveVerySmall))
            {
                widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
            }

            widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::Empty;
            widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::Empty;
            widgets[WIDX_LEVEL].type = WindowWidgetType::Empty;
            widgets[WIDX_SLOPE_UP].type = WindowWidgetType::Empty;
            widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::Empty;
            widgets[WIDX_SLOPE_DOWN_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP);
            widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
            widgets[WIDX_SLOPE_UP_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP);
            widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
            if (trackDrawerDescriptor.Regular.SupportsTrackGroup(TrackGroup::reverseFreefall))
            {
                widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::slope) || IsTrackEnabled(TrackGroup::slopeSteepDown)
                || IsTrackEnabled(TrackGroup::slopeSteepUp))
            {
                widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::slope))
            {
                widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
            }
            if ((IsTrackEnabled(TrackGroup::helixDownBankedHalf) || IsTrackEnabled(TrackGroup::helixUpBankedHalf))
                && _currentTrackRollEnd != TrackRoll::None && _currentTrackPitchEnd == TrackPitch::None)
            {
                const bool hasHelixEquivalent = _currentlySelectedTrack == TrackCurve::Left
                    || _currentlySelectedTrack == TrackCurve::Right || _currentlySelectedTrack == TrackCurve::LeftSmall
                    || _currentlySelectedTrack == TrackCurve::RightSmall;
                if (hasHelixEquivalent)
                {
                    // Enable helix
                    widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
                    if (IsTrackEnabled(TrackGroup::helixUpBankedHalf))
                        widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
                }
            }

            if (IsTrackEnabled(TrackGroup::slopeSteepDown))
            {
                widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
            }
            if (IsTrackEnabled(TrackGroup::slopeSteepUp))
            {
                widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
            }

            const auto& gameState = GetGameState();
            if (currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::upInclineRequiresLift)
                && (_currentTrackPitchEnd == TrackPitch::Up25 || _currentTrackPitchEnd == TrackPitch::Up60)
                && !gameState.Cheats.EnableAllDrawableTrackPieces)
            {
                _currentTrackLiftHill |= CONSTRUCTION_LIFT_HILL_SELECTED;
            }

            if ((IsTrackEnabled(TrackGroup::liftHill) && !_currentlySelectedTrack.isTrackType)
                || (gameState.Cheats.EnableChainLiftOnAllTrack
                    && currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack)))
            {
                widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::FlatBtn;
            }
            else
            {
                widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::Empty;
            }

            int32_t x = 45;
            for (int32_t i = WIDX_SLOPE_DOWN_STEEP; i <= WIDX_SLOPE_UP_STEEP; i++)
            {
                widgets[i].left = x;
                widgets[i].right = x + 23;
                x += 24;
            }

            widgets[WIDX_SLOPE_UP_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP);
            widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
            widgets[WIDX_SLOPE_DOWN_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP);
            widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
            if (IsTrackEnabled(TrackGroup::slopeVertical))
            {
                if (_previousTrackPitchEnd == TrackPitch::Up60 || _previousTrackPitchEnd == TrackPitch::Up90)
                {
                    int32_t originalSlopeUpSteepLeft = widgets[WIDX_SLOPE_UP_STEEP].left;
                    int32_t originalSlopeUpSteepRight = widgets[WIDX_SLOPE_UP_STEEP].right;
                    for (int32_t i = WIDX_SLOPE_UP_STEEP; i > WIDX_SLOPE_DOWN_STEEP; i--)
                    {
                        widgets[i].left = widgets[i - 1].left;
                        widgets[i].right = widgets[i - 1].right;
                    }
                    widgets[WIDX_SLOPE_DOWN_STEEP].left = originalSlopeUpSteepLeft;
                    widgets[WIDX_SLOPE_DOWN_STEEP].right = originalSlopeUpSteepRight;
                    widgets[WIDX_SLOPE_DOWN_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_VERTICAL_RISE);
                    widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_RISE_TIP;
                }
                else if (_previousTrackPitchEnd == TrackPitch::Down60 || _previousTrackPitchEnd == TrackPitch::Down90)
                {
                    int32_t originalSlopeDownSteepLeft = widgets[WIDX_SLOPE_DOWN_STEEP].left;
                    int32_t originalSlopeDownSteepRight = widgets[WIDX_SLOPE_DOWN_STEEP].right;
                    for (int32_t i = WIDX_SLOPE_DOWN_STEEP; i < WIDX_SLOPE_UP_STEEP; i++)
                    {
                        widgets[i].left = widgets[i + 1].left;
                        widgets[i].right = widgets[i + 1].right;
                    }
                    widgets[WIDX_SLOPE_UP_STEEP].left = originalSlopeDownSteepLeft;
                    widgets[WIDX_SLOPE_UP_STEEP].right = originalSlopeDownSteepRight;
                    widgets[WIDX_SLOPE_UP_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_VERTICAL_DROP);
                    widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_DROP_TIP;
                }
            }

            if ((IsTrackEnabled(TrackGroup::helixDownUnbankedQuarter) || IsTrackEnabled(TrackGroup::helixUpUnbankedQuarter))
                && _currentTrackPitchEnd == TrackPitch::None && _currentTrackRollEnd == TrackRoll::None
                && (_currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right))
            {
                widgets[WIDX_SLOPE_DOWN_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_HELIX_DOWN);
                widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
                widgets[WIDX_SLOPE_UP_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_HELIX_UP);
                widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

                int32_t tmp = widgets[WIDX_SLOPE_DOWN_STEEP].left;
                widgets[WIDX_SLOPE_DOWN_STEEP].left = widgets[WIDX_SLOPE_DOWN].left;
                widgets[WIDX_SLOPE_DOWN].left = tmp;

                tmp = widgets[WIDX_SLOPE_DOWN_STEEP].right;
                widgets[WIDX_SLOPE_DOWN_STEEP].right = widgets[WIDX_SLOPE_DOWN].right;
                widgets[WIDX_SLOPE_DOWN].right = tmp;

                tmp = widgets[WIDX_SLOPE_UP_STEEP].left;
                widgets[WIDX_SLOPE_UP_STEEP].left = widgets[WIDX_SLOPE_UP].left;
                widgets[WIDX_SLOPE_UP].left = tmp;

                tmp = widgets[WIDX_SLOPE_UP_STEEP].right;
                widgets[WIDX_SLOPE_UP_STEEP].right = widgets[WIDX_SLOPE_UP].right;
                widgets[WIDX_SLOPE_UP].right = tmp;
            }

            if (IsTrackEnabled(TrackGroup::helixDownBankedQuarter) || IsTrackEnabled(TrackGroup::helixUpBankedQuarter)
                || IsTrackEnabled(TrackGroup::helixDownBankedHalf) || IsTrackEnabled(TrackGroup::helixUpBankedHalf))
            {
                const bool hasHelixEquivalent = _currentlySelectedTrack == TrackCurve::Left
                    || _currentlySelectedTrack == TrackCurve::Right || _currentlySelectedTrack == TrackCurve::LeftSmall
                    || _currentlySelectedTrack == TrackCurve::RightSmall;

                if (hasHelixEquivalent && _currentTrackPitchEnd == TrackPitch::None && _currentTrackRollEnd != TrackRoll::None)
                {
                    widgets[WIDX_SLOPE_DOWN_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_HELIX_DOWN);
                    widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
                    widgets[WIDX_SLOPE_UP_STEEP].image = ImageId(SPR_RIDE_CONSTRUCTION_HELIX_UP);
                    widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_UP_TIP;

                    int32_t tmp = widgets[WIDX_SLOPE_DOWN_STEEP].left;
                    widgets[WIDX_SLOPE_DOWN_STEEP].left = widgets[WIDX_SLOPE_DOWN].left;
                    widgets[WIDX_SLOPE_DOWN].left = tmp;

                    tmp = widgets[WIDX_SLOPE_DOWN_STEEP].right;
                    widgets[WIDX_SLOPE_DOWN_STEEP].right = widgets[WIDX_SLOPE_DOWN].right;
                    widgets[WIDX_SLOPE_DOWN].right = tmp;

                    tmp = widgets[WIDX_SLOPE_UP_STEEP].left;
                    widgets[WIDX_SLOPE_UP_STEEP].left = widgets[WIDX_SLOPE_UP].left;
                    widgets[WIDX_SLOPE_UP].left = tmp;

                    tmp = widgets[WIDX_SLOPE_UP_STEEP].right;
                    widgets[WIDX_SLOPE_UP_STEEP].right = widgets[WIDX_SLOPE_UP].right;
                    widgets[WIDX_SLOPE_UP].right = tmp;
                }
            }

            widgets[WIDX_BANKING_GROUPBOX].image = ImageId(STR_RIDE_CONSTRUCTION_ROLL_BANKING);
            widgets[WIDX_BANK_LEFT].image = ImageId(SPR_RIDE_CONSTRUCTION_LEFT_BANK);
            widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP;
            widgets[WIDX_BANK_LEFT].left = 69;
            widgets[WIDX_BANK_LEFT].right = 92;
            widgets[WIDX_BANK_LEFT].top = 132;
            widgets[WIDX_BANK_LEFT].bottom = 155;
            widgets[WIDX_BANK_STRAIGHT].image = ImageId(SPR_RIDE_CONSTRUCTION_NO_BANK);
            widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_NO_ROLL_TIP;
            widgets[WIDX_BANK_STRAIGHT].left = 93;
            widgets[WIDX_BANK_STRAIGHT].right = 116;
            widgets[WIDX_BANK_STRAIGHT].top = 132;
            widgets[WIDX_BANK_STRAIGHT].bottom = 155;
            widgets[WIDX_BANK_RIGHT].image = ImageId(SPR_RIDE_CONSTRUCTION_RIGHT_BANK);
            widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP;
            widgets[WIDX_BANK_RIGHT].left = 117;
            widgets[WIDX_BANK_RIGHT].right = 140;
            widgets[WIDX_BANK_RIGHT].top = 132;
            widgets[WIDX_BANK_RIGHT].bottom = 155;
            widgets[WIDX_BANK_LEFT].type = WindowWidgetType::Empty;
            widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::Empty;
            widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::Empty;
            widgets[WIDX_U_TRACK].type = WindowWidgetType::Empty;
            widgets[WIDX_O_TRACK].type = WindowWidgetType::Empty;

            bool trackHasSpeedSetting = TrackTypeHasSpeedSetting(_selectedTrackType)
                || TrackTypeHasSpeedSetting(_currentlySelectedTrack.trackType);
            bool boosterTrackSelected = TrackTypeIsBooster(_selectedTrackType)
                || TrackTypeIsBooster(_currentlySelectedTrack.trackType);

            // Only necessary because TD6 writes speed and seat rotation to the same bits. Remove for new track design format.
            bool trackHasSpeedAndSeatRotation = _selectedTrackType == TrackElemType::BlockBrakes
                || _currentlySelectedTrack == TrackElemType::BlockBrakes || _selectedTrackType > TrackElemType::HighestAlias
                || _currentlySelectedTrack.trackType > TrackElemType::HighestAlias;

            bool rideHasSeatRotation = rtd.HasFlag(RtdFlag::hasSeatRotation);

            if (!trackHasSpeedSetting)
            {
                if (IsTrackEnabled(TrackGroup::flatRollBanking))
                {
                    widgets[WIDX_BANK_LEFT].type = WindowWidgetType::FlatBtn;
                    widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::FlatBtn;
                    widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::FlatBtn;
                }
                if (trackDrawerDescriptor.HasCoveredPieces())
                {
                    widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                    widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
                    widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;

                    widgets[WIDX_U_TRACK].image = ImageId(trackDrawerDescriptor.Regular.icon);
                    widgets[WIDX_O_TRACK].image = ImageId(trackDrawerDescriptor.Covered.icon);
                    widgets[WIDX_U_TRACK].tooltip = trackDrawerDescriptor.Regular.tooltip;
                    widgets[WIDX_O_TRACK].tooltip = trackDrawerDescriptor.Covered.tooltip;

                    // TODO: Read these from the TrackDrawerEntry
                    if (currentRide->type == RIDE_TYPE_WATER_COASTER)
                    {
                        const bool hasCoveredCurve = _currentlySelectedTrack == TrackCurve::None
                            || _currentlySelectedTrack == TrackCurve::Left || _currentlySelectedTrack == TrackCurve::Right;
                        const bool isSBend = _currentlySelectedTrack == TrackElemType::SBendLeft
                            || _currentlySelectedTrack == TrackElemType::SBendRight;

                        const bool hasCoveredVersion = (hasCoveredCurve || isSBend) && _currentTrackPitchEnd == TrackPitch::None
                            && _currentTrackRollEnd == TrackRoll::None;
                        if (!hasCoveredVersion)
                        {
                            widgets[WIDX_U_TRACK].type = WindowWidgetType::Empty;
                            widgets[WIDX_O_TRACK].type = WindowWidgetType::Empty;
                        }
                    }
                }
            }
            else
            {
                if (!boosterTrackSelected)
                {
                    widgets[WIDX_SPEED_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED;
                    widgets[WIDX_SPEED_SETTING_SPINNER].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
                    widgets[WIDX_SPEED_SETTING_SPINNER_UP].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
                    widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
                }
                else
                {
                    widgets[WIDX_SPEED_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED;
                    widgets[WIDX_SPEED_SETTING_SPINNER].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
                    widgets[WIDX_SPEED_SETTING_SPINNER_UP].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
                    widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
                }

                _currentlyShowingBrakeOrBoosterSpeed = true;
                widgets[WIDX_SPEED_SETTING_SPINNER].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_VELOCITY;

                widgets[WIDX_SPEED_SETTING_SPINNER].type = WindowWidgetType::Spinner;
                widgets[WIDX_SPEED_SETTING_SPINNER_UP].type = WindowWidgetType::Button;
                widgets[WIDX_SPEED_SETTING_SPINNER_UP].text = STR_NUMERIC_UP;
                widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].type = WindowWidgetType::Button;
                widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].text = STR_NUMERIC_DOWN;

                ResizeSpinner(WIDX_SPEED_SETTING_SPINNER, { 12, 138 }, { 85, kSpinnerHeight });

                hold_down_widgets |= (1uLL << WIDX_SPEED_SETTING_SPINNER_UP) | (1uLL << WIDX_SPEED_SETTING_SPINNER_DOWN);
            }

            static constexpr int16_t bankingGroupboxRightNoSeatRotation = GW;
            static constexpr int16_t bankingGroupboxRightWithSeatRotation = 114;

            widgets[WIDX_BANKING_GROUPBOX].right = bankingGroupboxRightNoSeatRotation;
            widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Empty;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Empty;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Empty;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Empty;

            // Simplify this condition to "rideHasSeatRotation" for new track design format
            if ((rideHasSeatRotation && !trackHasSpeedSetting)
                || (rideHasSeatRotation && trackHasSpeedSetting && trackHasSpeedAndSeatRotation))
            {
                widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Groupbox;
                widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Spinner;
                widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Button;
                widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Button;
                widgets[WIDX_BANKING_GROUPBOX].right = bankingGroupboxRightWithSeatRotation;

                // squishes the track speed spinner slightly to make room for the seat rotation widgets
                if (trackHasSpeedSetting)
                {
                    widgets[WIDX_SPEED_SETTING_SPINNER].left -= 4;
                    widgets[WIDX_SPEED_SETTING_SPINNER].right -= 8;
                    widgets[WIDX_SPEED_SETTING_SPINNER_UP].right -= 8;
                    widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].right -= 8;
                    widgets[WIDX_SPEED_SETTING_SPINNER_UP].left -= 8;
                    widgets[WIDX_SPEED_SETTING_SPINNER_DOWN].left -= 8;
                }
                // moves banking buttons to the left to make room for the seat rotation widgets
                else if (IsTrackEnabled(TrackGroup::flatRollBanking))
                {
                    for (int32_t i = WIDX_BANK_LEFT; i <= WIDX_BANK_RIGHT; i++)
                    {
                        widgets[i].left -= 36;
                        widgets[i].right -= 36;
                    }
                }
            }

            uint64_t pressedWidgets = pressed_widgets
                & ((1uLL << WIDX_BACKGROUND) | (1uLL << WIDX_TITLE) | (1uLL << WIDX_CLOSE) | (1uLL << WIDX_DIRECTION_GROUPBOX)
                   | (1uLL << WIDX_SLOPE_GROUPBOX) | (1uLL << WIDX_BANKING_GROUPBOX) | (1uLL << WIDX_CONSTRUCT)
                   | (1uLL << WIDX_DEMOLISH) | (1uLL << WIDX_PREVIOUS_SECTION) | (1uLL << WIDX_NEXT_SECTION)
                   | (1uLL << WIDX_ENTRANCE_EXIT_GROUPBOX) | (1uLL << WIDX_ENTRANCE) | (1uLL << WIDX_EXIT));

            widgets[WIDX_CONSTRUCT].type = WindowWidgetType::Empty;
            widgets[WIDX_DEMOLISH].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
            if (rtd.HasFlag(RtdFlag::cannotHaveGaps))
            {
                widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
                widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
            }
            else
            {
                widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::FlatBtn;
            }

            switch (_rideConstructionState)
            {
                case RideConstructionState::Front:
                    widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
                    widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
                    break;
                case RideConstructionState::Back:
                    widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
                    widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
                    break;
                case RideConstructionState::Place:
                    widgets[WIDX_CONSTRUCT].type = WindowWidgetType::ImgBtn;
                    widgets[WIDX_DEMOLISH].type = WindowWidgetType::Empty;
                    widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
                    widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
                    widgets[WIDX_ROTATE].type = WindowWidgetType::FlatBtn;
                    break;
                case RideConstructionState::EntranceExit:
                    widgets[WIDX_DEMOLISH].type = WindowWidgetType::Empty;
                    widgets[WIDX_NEXT_SECTION].type = WindowWidgetType::Empty;
                    widgets[WIDX_PREVIOUS_SECTION].type = WindowWidgetType::Empty;
                    break;
                default:
                    pressed_widgets = pressedWidgets;
                    Invalidate();
                    return;
            }

            WidgetIndex widgetIndex = WIDX_SPECIAL_TRACK_DROPDOWN;
            if (!_currentlySelectedTrack.isTrackType)
            {
                switch (_currentlySelectedTrack.curve)
                {
                    case TrackCurve::None:
                        widgetIndex = WIDX_STRAIGHT;
                        break;
                    case TrackCurve::Left:
                        widgetIndex = WIDX_LEFT_CURVE;
                        break;
                    case TrackCurve::Right:
                        widgetIndex = WIDX_RIGHT_CURVE;
                        break;
                    case TrackCurve::LeftSmall:
                        widgetIndex = WIDX_LEFT_CURVE_SMALL;
                        break;
                    case TrackCurve::RightSmall:
                        widgetIndex = WIDX_RIGHT_CURVE_SMALL;
                        break;
                    case TrackCurve::LeftVerySmall:
                        widgetIndex = WIDX_LEFT_CURVE_VERY_SMALL;
                        break;
                    case TrackCurve::RightVerySmall:
                        widgetIndex = WIDX_RIGHT_CURVE_VERY_SMALL;
                        break;
                    case TrackCurve::LeftLarge:
                        widgetIndex = WIDX_LEFT_CURVE_LARGE;
                        break;
                    case TrackCurve::RightLarge:
                        widgetIndex = WIDX_RIGHT_CURVE_LARGE;
                        break;
                }
            }

            pressedWidgets |= (1uLL << widgetIndex);

            switch (_currentTrackPitchEnd)
            {
                case TrackPitch::Down60:
                case TrackPitch::Up90:
                    widgetIndex = WIDX_SLOPE_DOWN_STEEP;
                    break;
                case TrackPitch::Down25:
                    widgetIndex = WIDX_SLOPE_DOWN;
                    break;
                case TrackPitch::Up25:
                    widgetIndex = WIDX_SLOPE_UP;
                    break;
                case TrackPitch::Up60:
                case TrackPitch::Down90:
                    widgetIndex = WIDX_SLOPE_UP_STEEP;
                    break;
                default:
                    widgetIndex = WIDX_LEVEL;
                    break;
            }
            pressedWidgets |= (1uLL << widgetIndex);

            if (!_currentlyShowingBrakeOrBoosterSpeed)
            {
                if (trackDrawerDescriptor.HasCoveredPieces())
                {
                    if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_PIECES)
                    {
                        pressedWidgets |= (1uLL << WIDX_O_TRACK);
                    }
                    else
                    {
                        pressedWidgets |= (1uLL << WIDX_U_TRACK);
                    }
                }
                switch (_currentTrackRollEnd)
                {
                    case TrackRoll::Left:
                        widgetIndex = WIDX_BANK_LEFT;
                        break;
                    case TrackRoll::None:
                        widgetIndex = WIDX_BANK_STRAIGHT;
                        break;
                    default:
                        widgetIndex = WIDX_BANK_RIGHT;
                        break;
                }
                pressedWidgets |= (1uLL << widgetIndex);
            }

            if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
                pressedWidgets |= (1uLL << WIDX_CHAIN_LIFT);

            pressed_widgets = pressedWidgets;
            Invalidate();
        }

        void UpdatePossibleRideConfigurations()
        {
            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }
            _specialElementDropdownState = BuildSpecialElementsList(
                *currentRide, _currentTrackPieceDirection, _previousTrackPitchEnd, _previousTrackRollEnd,
                _rideConstructionState);
            _currentlyShowingBrakeOrBoosterSpeed = false;
        }

        void UpdateMapSelection()
        {
            int32_t trackType, trackDirection;
            CoordsXYZ trackPos{};

            MapInvalidateMapSelectionTiles();
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

            switch (_rideConstructionState)
            {
                case RideConstructionState::State0:
                    trackDirection = _currentTrackPieceDirection;
                    trackType = 0;
                    trackPos = _currentTrackBegin;
                    break;
                case RideConstructionState::Selected:
                    trackDirection = _currentTrackPieceDirection;
                    trackType = _currentTrackPieceType;
                    trackPos = _currentTrackBegin;
                    break;
                case RideConstructionState::EntranceExit:
                    gMapSelectionTiles.clear();
                    return;
                default:
                    if (WindowRideConstructionUpdateState(&trackType, &trackDirection, nullptr, nullptr, &trackPos, nullptr))
                    {
                        trackDirection = _currentTrackPieceDirection;
                        trackType = 0;
                        trackPos = _currentTrackBegin;
                    }
                    break;
            }

            if (GetRide(_currentRideIndex))
            {
                SelectMapTiles(trackType, trackDirection, trackPos);
                MapInvalidateMapSelectionTiles();
            }
        }

        void SelectMapTiles(int32_t trackType, int32_t trackDirection, const CoordsXY& tileCoords)
        {
            // If the scenery tool is active, we do not display our tiles as it
            // will conflict with larger scenery objects selecting tiles
            if (isToolActive(WindowClass::Scenery))
            {
                return;
            }

            const PreviewTrack* trackBlock;

            const auto& ted = GetTrackElementDescriptor(trackType);
            trackBlock = ted.block;
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

    private:
        void Construct()
        {
            RideId rideIndex;
            int32_t trackType, trackDirection, liftHillAndAlternativeState, properties;
            CoordsXYZ trackPos{};

            _currentTrackPrice = kMoney64Undefined;
            _trackPlaceCost = kMoney64Undefined;
            _trackPlaceErrorMessage = STR_NONE;
            RideConstructionInvalidateCurrentTrack();
            if (WindowRideConstructionUpdateState(
                    &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, &properties))
            {
                WindowRideConstructionUpdateActiveElements();
                return;
            }

            auto currentRide = GetRide(_currentRideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            auto trackPlaceAction = TrackPlaceAction(
                rideIndex, trackType, currentRide->type, { trackPos, static_cast<uint8_t>(trackDirection) }, properties & 0xFF,
                (properties >> 8) & 0x0F, (properties >> 12) & 0x0F, liftHillAndAlternativeState, false);
            if (_rideConstructionState == RideConstructionState::Back)
            {
                trackPlaceAction.SetCallback(RideConstructPlacedBackwardGameActionCallback);
            }
            else if (_rideConstructionState == RideConstructionState::Front)
            {
                trackPlaceAction.SetCallback(RideConstructPlacedForwardGameActionCallback);
            }
            auto res = GameActions::Execute(&trackPlaceAction);
            // Used by some functions
            if (res.Error != GameActions::Status::Ok)
            {
                _trackPlaceCost = kMoney64Undefined;
                _trackPlaceErrorMessage = std::get<StringId>(res.ErrorMessage);
            }
            else
            {
                _trackPlaceCost = res.Cost;
                _trackPlaceErrorMessage = STR_NONE;
            }

            if (res.Error != GameActions::Status::Ok)
            {
                return;
            }
            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, trackPos);

            if (NetworkGetMode() != NETWORK_MODE_NONE)
            {
                _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED;
            }

            const auto resultData = res.GetData<TrackPlaceActionResult>();
            if (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                ViewportSetVisibility(ViewportVisibility::UndergroundViewOn);
            }

            const bool helixSelected = (_currentlySelectedTrack.isTrackType)
                && TrackTypeIsHelix(_currentlySelectedTrack.trackType);
            if (helixSelected || (_currentTrackPitchEnd != TrackPitch::None))
            {
                ViewportSetVisibility(ViewportVisibility::TrackHeights);
            }
        }

        void MouseUpDemolish()
        {
            int32_t direction;
            TileElement* tileElement;
            CoordsXYE inputElement, outputElement;
            TrackBeginEnd trackBeginEnd;

            _currentTrackPrice = kMoney64Undefined;
            RideConstructionInvalidateCurrentTrack();

            // Select the track element that is to be deleted
            _rideConstructionState2 = RideConstructionState::Selected;
            if (_rideConstructionState == RideConstructionState::Front)
            {
                if (!RideSelectBackwardsFromFront())
                {
                    WindowRideConstructionUpdateActiveElements();
                    return;
                }
                _rideConstructionState2 = RideConstructionState::Front;
            }
            else if (_rideConstructionState == RideConstructionState::Back)
            {
                if (!RideSelectForwardsFromBack())
                {
                    WindowRideConstructionUpdateActiveElements();
                    return;
                }
                _rideConstructionState2 = RideConstructionState::Back;
            }

            // Invalidate the selected track element or make sure it's at origin???
            direction = _currentTrackPieceDirection;
            // The direction is reset by ride_initialise_construction_window(), but we need it to remove flat rides properly.
            Direction currentDirection = _currentTrackPieceDirection;
            track_type_t type = _currentTrackPieceType;
            auto newCoords = GetTrackElementOriginAndApplyChanges(
                { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);
            if (!newCoords.has_value())
            {
                WindowRideConstructionUpdateActiveElements();
                return;
            }

            // Get the previous track element to go to after the selected track element is deleted
            inputElement.x = newCoords->x;
            inputElement.y = newCoords->y;
            inputElement.element = tileElement;
            if (TrackBlockGetPrevious({ *newCoords, tileElement }, &trackBeginEnd))
            {
                *newCoords = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
                direction = trackBeginEnd.begin_direction;
                type = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
                _gotoStartPlacementMode = false;
            }
            else if (TrackBlockGetNext(&inputElement, &outputElement, &newCoords->z, &direction))
            {
                newCoords->x = outputElement.x;
                newCoords->y = outputElement.y;
                direction = outputElement.element->GetDirection();
                type = outputElement.element->AsTrack()->GetTrackType();
                _gotoStartPlacementMode = false;
            }
            else
            {
                direction = _currentTrackPieceDirection;
                type = _currentTrackPieceType;
                newCoords = GetTrackElementOriginAndApplyChanges(
                    { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);

                if (!newCoords.has_value())
                {
                    WindowRideConstructionUpdateActiveElements();
                    return;
                }

                const auto& ted = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
                const PreviewTrack* trackBlock = ted.block;
                newCoords->z = (tileElement->GetBaseZ()) - trackBlock->z;
                _gotoStartPlacementMode = true;
            }

            auto trackRemoveAction = TrackRemoveAction(
                _currentTrackPieceType, 0,
                { _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z, currentDirection });

            trackRemoveAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                {
                    WindowRideConstructionUpdateActiveElements();
                }
                else
                {
                    auto currentRide = GetRide(_currentRideIndex);
                    if (currentRide != nullptr)
                    {
                        // When flat rides are deleted, the window should be reset so the currentRide can be placed again.
                        const auto& rtd = currentRide->GetRideTypeDescriptor();
                        if (rtd.HasFlag(RtdFlag::isFlatRide) && !rtd.HasFlag(RtdFlag::isShopOrFacility))
                        {
                            RideInitialiseConstructionWindow(*currentRide);
                        }

                        WindowRideConstructionMouseUpDemolishNextPiece({ *newCoords, static_cast<Direction>(direction) }, type);
                    }
                }
            });

            GameActions::Execute(&trackRemoveAction);
        }

        void Rotate()
        {
            _autoRotatingShop = false;
            _currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
            RideConstructionInvalidateCurrentTrack();
            _currentTrackPrice = kMoney64Undefined;
            WindowRideConstructionUpdateActiveElements();
        }

        void EntranceClick()
        {
            if (ToolSet(*this, WIDX_ENTRANCE, Tool::Crosshair))
            {
                auto currentRide = GetRide(_currentRideIndex);
                if (currentRide != nullptr && !RideTryGetOriginElement(*currentRide, nullptr))
                {
                    RideInitialiseConstructionWindow(*currentRide);
                }
            }
            else
            {
                gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_ENTRANCE;
                gRideEntranceExitPlaceRideIndex = _currentRideIndex;
                gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
                InputSetFlag(INPUT_FLAG_6, true);
                RideConstructionInvalidateCurrentTrack();
                if (_rideConstructionState != RideConstructionState::EntranceExit)
                {
                    gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
                    _rideConstructionState = RideConstructionState::EntranceExit;
                }
                WindowRideConstructionUpdateActiveElements();
            }
        }

        void ExitClick()
        {
            if (ToolSet(*this, WIDX_EXIT, Tool::Crosshair))
            {
                auto currentRide = GetRide(_currentRideIndex);
                if (!RideTryGetOriginElement(*currentRide, nullptr))
                {
                    RideInitialiseConstructionWindow(*currentRide);
                }
            }
            else
            {
                gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_EXIT;
                gRideEntranceExitPlaceRideIndex = _currentRideIndex;
                gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
                InputSetFlag(INPUT_FLAG_6, true);
                RideConstructionInvalidateCurrentTrack();
                if (_rideConstructionState != RideConstructionState::EntranceExit)
                {
                    gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
                    _rideConstructionState = RideConstructionState::EntranceExit;
                }
                WindowRideConstructionUpdateActiveElements();
            }
        }

        void UpdateLiftHillSelected(TrackPitch slope)
        {
            _currentTrackPitchEnd = slope;
            _currentTrackPrice = kMoney64Undefined;
            if (_rideConstructionState == RideConstructionState::Front && !GetGameState().Cheats.EnableChainLiftOnAllTrack)
            {
                switch (slope)
                {
                    case TrackPitch::None:
                    case TrackPitch::Up25:
                    case TrackPitch::Up60:
                        break;
                    default:
                        _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
                        break;
                }
            }
            WindowRideConstructionUpdateActiveElements();
        }

        void SetBrakeSpeed(int32_t brakesSpeed)
        {
            TileElement* tileElement;

            if (GetTrackElementOriginAndApplyChanges(
                    { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
                    &tileElement, 0)
                != std::nullopt)
            {
                auto trackSetBrakeSpeed = TrackSetBrakeSpeedAction(
                    _currentTrackBegin, tileElement->AsTrack()->GetTrackType(), brakesSpeed);
                trackSetBrakeSpeed.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                    WindowRideConstructionUpdateActiveElements();
                });
                GameActions::Execute(&trackSetBrakeSpeed);
                return;
            }
            WindowRideConstructionUpdateActiveElements();
        }

        void ShowSpecialTrackDropdown(Widget* widget)
        {
            int32_t defaultIndex = -1;
            for (size_t i = 0; i < _specialElementDropdownState.Elements.size(); i++)
            {
                track_type_t trackPiece = _specialElementDropdownState.Elements[i].TrackType;

                const auto& ted = GetTrackElementDescriptor(trackPiece);
                StringId trackPieceStringId = ted.description;
                if (trackPieceStringId == STR_RAPIDS)
                {
                    auto currentRide = GetRide(_currentRideIndex);
                    if (currentRide != nullptr)
                    {
                        const auto& rtd = currentRide->GetRideTypeDescriptor();
                        if (rtd.Category != RIDE_CATEGORY_WATER)
                            trackPieceStringId = STR_LOG_BUMPS;
                    }
                }
                gDropdownItems[i].Format = trackPieceStringId;
                if (_currentlySelectedTrack == trackPiece)
                {
                    defaultIndex = static_cast<int32_t>(i);
                }
            }

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 0,
                _specialElementDropdownState.Elements.size(), widget->width());

            for (size_t i = 0; i < _specialElementDropdownState.Elements.size(); i++)
            {
                Dropdown::SetDisabled(static_cast<int32_t>(i), _specialElementDropdownState.Elements[i].Disabled);
            }
            gDropdownDefaultIndex = defaultIndex;
        }

        void RideSelectedTrackSetSeatRotation(int32_t seatRotation)
        {
            GetTrackElementOriginAndApplyChanges(
                { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType,
                seatRotation, nullptr, TRACK_ELEMENT_SET_SEAT_ROTATION);
            WindowRideConstructionUpdateActiveElements();
        }

        void ToolDownEntranceExit(const ScreenCoordsXY& screenCoords)
        {
            RideConstructionInvalidateCurrentTrack();
            MapInvalidateSelectionRect();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            CoordsXYZD entranceOrExitCoords = RideGetEntranceOrExitPositionFromScreenPosition(screenCoords);
            if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
                return;

            auto rideEntranceExitPlaceAction = RideEntranceExitPlaceAction(
                entranceOrExitCoords, DirectionReverse(gRideEntranceExitPlaceDirection), gRideEntranceExitPlaceRideIndex,
                gRideEntranceExitPlaceStationIndex, gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_EXIT);

            rideEntranceExitPlaceAction.SetCallback([=, this](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                    return;

                OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);

                auto currentRide = GetRide(gRideEntranceExitPlaceRideIndex);
                if (currentRide != nullptr && RideAreAllPossibleEntrancesAndExitsBuilt(*currentRide).Successful)
                {
                    ToolCancel();
                    if (!currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                    {
                        WindowCloseByClass(WindowClass::RideConstruction);
                    }
                }
                else
                {
                    gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
                    WindowInvalidateByClass(WindowClass::RideConstruction);

                    auto newToolWidgetIndex = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE)
                        ? WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE
                        : WC_RIDE_CONSTRUCTION__WIDX_EXIT;

                    ToolCancel();
                    ToolSet(*this, newToolWidgetIndex, Tool::Crosshair);
                }
            });
            auto res = GameActions::Execute(&rideEntranceExitPlaceAction);
        }

        void DrawTrackPiece(
            DrawPixelInfo& dpi, RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndInvertedState,
            int32_t widgetWidth, int32_t widgetHeight)
        {
            auto currentRide = GetRide(rideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            const auto& ted = GetTrackElementDescriptor(trackType);
            const auto* trackBlock = ted.block;
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

            auto previewZOffset = ted.definition.previewZOffset;
            mapCoords.z -= previewZOffset;

            const ScreenCoordsXY rotatedScreenCoords = Translate3DTo2DWithZ(GetCurrentRotation(), mapCoords);

            dpi.x += rotatedScreenCoords.x - widgetWidth / 2;
            dpi.y += rotatedScreenCoords.y - widgetHeight / 2 - 16;

            DrawTrackPieceHelper(dpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, { 4096, 4096 }, 1024);
        }

        void DrawTrackPieceHelper(
            DrawPixelInfo& dpi, RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndInvertedState,
            const CoordsXY& originCoords, int32_t originZ)
        {
            TileElement tempSideTrackTileElement{ 0x80, 0x8F, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            TileElement tempTrackTileElement{};
            TileElement* backupTileElementArrays[5]{};
            PaintSession* session = PaintSessionAlloc(dpi, 0, GetCurrentRotation());
            trackDirection &= 3;

            auto currentRide = GetRide(rideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            auto& gameState = GetGameState();
            auto preserveMapSize = gameState.MapSize;

            gameState.MapSize = { kMaximumMapSizeTechnical, kMaximumMapSizeTechnical };

            // Setup non changing parts of the temporary track tile element
            tempTrackTileElement.SetType(TileElementType::Track);
            tempTrackTileElement.SetDirection(trackDirection);
            tempTrackTileElement.AsTrack()->SetHasChain((liftHillAndInvertedState & CONSTRUCTION_LIFT_HILL_SELECTED) != 0);
            tempTrackTileElement.SetLastForTile(true);
            tempTrackTileElement.AsTrack()->SetTrackType(trackType);
            tempTrackTileElement.AsTrack()->SetRideType(currentRide->type);
            tempTrackTileElement.AsTrack()->SetHasCableLift(false);
            tempTrackTileElement.AsTrack()->SetInverted((liftHillAndInvertedState & CONSTRUCTION_INVERTED_TRACK_SELECTED) != 0);
            tempTrackTileElement.AsTrack()->SetColourScheme(RideColourScheme::main);
            // Skipping seat rotation, should not be necessary for a temporary piece.
            tempTrackTileElement.AsTrack()->SetRideIndex(rideIndex);

            const auto& ted = GetTrackElementDescriptor(trackType);
            const auto* trackBlock = ted.block;
            const auto* rideEntry = currentRide->GetRideEntry();
            auto clearanceHeight = (rideEntry != nullptr) ? rideEntry->Clearance
                                                          : currentRide->GetRideTypeDescriptor().Heights.ClearanceHeight;

            while (trackBlock->index != 255)
            {
                auto quarterTile = trackBlock->quarterTile.Rotate(trackDirection);
                CoordsXY offsets = { trackBlock->x, trackBlock->y };
                CoordsXY coords = originCoords + offsets.Rotate(trackDirection);

                int32_t baseZ = originZ + trackBlock->z;
                int32_t clearanceZ = trackBlock->clearanceZ + clearanceHeight + baseZ + (4 * kCoordsZStep);

                auto centreTileCoords = TileCoordsXY{ coords };
                auto eastTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_EAST];
                auto westTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_WEST];
                auto northTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_NORTH];
                auto southTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_SOUTH];

                // Replace map elements with temporary ones containing track
                backupTileElementArrays[0] = MapGetFirstElementAt(centreTileCoords);
                backupTileElementArrays[1] = MapGetFirstElementAt(eastTileCoords);
                backupTileElementArrays[2] = MapGetFirstElementAt(westTileCoords);
                backupTileElementArrays[3] = MapGetFirstElementAt(northTileCoords);
                backupTileElementArrays[4] = MapGetFirstElementAt(southTileCoords);
                MapSetTileElement(centreTileCoords, &tempTrackTileElement);
                MapSetTileElement(eastTileCoords, &tempSideTrackTileElement);
                MapSetTileElement(westTileCoords, &tempSideTrackTileElement);
                MapSetTileElement(northTileCoords, &tempSideTrackTileElement);
                MapSetTileElement(southTileCoords, &tempSideTrackTileElement);

                // Set the temporary track element
                tempTrackTileElement.SetOccupiedQuadrants(quarterTile.GetBaseQuarterOccupied());
                tempTrackTileElement.SetBaseZ(baseZ);
                tempTrackTileElement.SetClearanceZ(clearanceZ);
                tempTrackTileElement.AsTrack()->SetSequenceIndex(trackBlock->index);

                // Draw this map tile
                TileElementPaintSetup(*session, coords, true);

                // Restore map elements
                MapSetTileElement(centreTileCoords, backupTileElementArrays[0]);
                MapSetTileElement(eastTileCoords, backupTileElementArrays[1]);
                MapSetTileElement(westTileCoords, backupTileElementArrays[2]);
                MapSetTileElement(northTileCoords, backupTileElementArrays[3]);
                MapSetTileElement(southTileCoords, backupTileElementArrays[4]);

                trackBlock++;
            }

            gameState.MapSize = preserveMapSize;

            PaintSessionArrange(*session);
            PaintDrawStructs(*session);
            PaintSessionFree(session);
        }
    };

    static void WindowRideConstructionUpdateDisabledPieces(ObjectEntryIndex rideType)
    {
        RideTrackGroup disabledPieces{};
        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (rtd.HasFlag(RtdFlag::hasTrack))
        {
            // Set all pieces as “disabled”. When looping over the ride entries,
            // pieces will be re-enabled as soon as a single entry supports it.
            disabledPieces.flip();

            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto& rideEntries = objManager.GetAllRideEntries(rideType);
            // If there are no vehicles for this ride type, enable all the track pieces.
            if (rideEntries.size() == 0)
            {
                disabledPieces.reset();
            }

            for (auto rideEntryIndex : rideEntries)
            {
                const auto* currentRideEntry = GetRideEntryByIndex(rideEntryIndex);
                if (currentRideEntry == nullptr)
                    continue;

                // Non-default vehicle visuals do not use this system, so we have to assume it supports all the track pieces.
                if (currentRideEntry->Cars[0].PaintStyle != VEHICLE_VISUAL_DEFAULT || rideType == RIDE_TYPE_CHAIRLIFT
                    || (currentRideEntry->Cars[0].flags & CAR_ENTRY_FLAG_SLIDE_SWING))
                {
                    disabledPieces.reset();
                    break;
                }

                // Any pieces that this ride entry supports must be taken out of the array.
                auto supportedPieces = RideEntryGetSupportedTrackPieces(*currentRideEntry);
                disabledPieces &= supportedPieces.flip();
            }
        }

        UpdateDisabledRidePieces(disabledPieces);
    }

    /**
     *
     *  rct2: 0x006CB481
     */
    WindowBase* RideConstructionOpen()
    {
        RideId rideIndex = _currentRideIndex;
        CloseRideWindowForConstruction(rideIndex);

        auto currentRide = GetRide(rideIndex);
        if (currentRide == nullptr)
        {
            return nullptr;
        }

        WindowRideConstructionUpdateDisabledPieces(currentRide->type);

        const auto& rtd = currentRide->GetRideTypeDescriptor();
        switch (rtd.ConstructionWindowContext)
        {
            case RideConstructionWindowContext::Maze:
                return ContextOpenWindowView(WV_MAZE_CONSTRUCTION);
            case RideConstructionWindowContext::Default:
                return WindowCreate<RideConstructionWindow>(
                    WindowClass::RideConstruction, ScreenCoordsXY(0, 29), WW, WH, WF_NO_AUTO_CLOSE);
        }
        return WindowCreate<RideConstructionWindow>(
            WindowClass::RideConstruction, ScreenCoordsXY(0, 29), WW, WH, WF_NO_AUTO_CLOSE);
    }

    static void CloseConstructWindowOnCompletion(const Ride& ride)
    {
        if (_rideConstructionState == RideConstructionState::State0)
        {
            auto w = WindowFindByClass(WindowClass::RideConstruction);
            if (w != nullptr)
            {
                if (RideAreAllPossibleEntrancesAndExitsBuilt(ride).Successful)
                {
                    WindowClose(*w);
                }
                else
                {
                    w->OnMouseUp(WIDX_ENTRANCE);
                }
            }
        }
    }

    static void WindowRideConstructionDoEntranceExitCheck()
    {
        auto w = WindowFindByClass(WindowClass::RideConstruction);
        auto ride = GetRide(_currentRideIndex);
        if (w == nullptr || ride == nullptr)
        {
            return;
        }

        if (_rideConstructionState == RideConstructionState::State0)
        {
            w = WindowFindByClass(WindowClass::RideConstruction);
            if (w != nullptr)
            {
                if (!RideAreAllPossibleEntrancesAndExitsBuilt(*ride).Successful)
                {
                    w->OnMouseUp(WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
                }
            }
        }
    }

    static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActions::Result* result)
    {
        if (result->Error != GameActions::Status::Ok)
        {
            WindowRideConstructionUpdateActiveElements();
            return;
        }
        auto ride = GetRide(_currentRideIndex);
        if (ride != nullptr)
        {
            int32_t trackDirection = _currentTrackPieceDirection;
            auto trackPos = _currentTrackBegin;
            if (!(trackDirection & 4))
            {
                trackPos -= CoordsDirectionDelta[trackDirection];
            }

            CoordsXYE next_track;
            if (TrackBlockGetNextFromZero(trackPos, *ride, trackDirection, &next_track, &trackPos.z, &trackDirection, false))
            {
                _currentTrackBegin.x = next_track.x;
                _currentTrackBegin.y = next_track.y;
                _currentTrackBegin.z = trackPos.z;
                _currentTrackPieceDirection = next_track.element->GetDirection();
                _currentTrackPieceType = next_track.element->AsTrack()->GetTrackType();
                _currentTrackSelectionFlags = 0;
                _rideConstructionState = RideConstructionState::Selected;
                _rideConstructionNextArrowPulse = 0;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
                RideSelectNextSection();
            }
            else
            {
                _rideConstructionState = RideConstructionState::State0;
            }

            WindowRideConstructionDoEntranceExitCheck();
            WindowRideConstructionUpdateActiveElements();
        }

        WindowCloseByClass(WindowClass::Error);
        if (ride != nullptr)
            CloseConstructWindowOnCompletion(*ride);
    }

    static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActions::Result* result)
    {
        if (result->Error != GameActions::Status::Ok)
        {
            WindowRideConstructionUpdateActiveElements();
            return;
        }
        auto ride = GetRide(_currentRideIndex);
        if (ride != nullptr)
        {
            auto trackDirection = DirectionReverse(_currentTrackPieceDirection);
            auto trackPos = _currentTrackBegin;
            if (!(trackDirection & 4))
            {
                trackPos += CoordsDirectionDelta[trackDirection];
            }

            TrackBeginEnd trackBeginEnd;
            if (TrackBlockGetPreviousFromZero(trackPos, *ride, trackDirection, &trackBeginEnd))
            {
                _currentTrackBegin.x = trackBeginEnd.begin_x;
                _currentTrackBegin.y = trackBeginEnd.begin_y;
                _currentTrackBegin.z = trackBeginEnd.begin_z;
                _currentTrackPieceDirection = trackBeginEnd.begin_direction;
                _currentTrackPieceType = trackBeginEnd.begin_element->AsTrack()->GetTrackType();
                _currentTrackSelectionFlags = 0;
                _rideConstructionState = RideConstructionState::Selected;
                _rideConstructionNextArrowPulse = 0;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
                RideSelectPreviousSection();
            }
            else
            {
                _rideConstructionState = RideConstructionState::State0;
            }

            WindowRideConstructionUpdateActiveElements();
        }

        WindowCloseByClass(WindowClass::Error);
        if (ride != nullptr)
            CloseConstructWindowOnCompletion(*ride);
    }

    /**
     *
     *  rct2: 0x006CC538
     */
    static std::optional<CoordsXY> RideGetPlacePositionFromScreenPosition(ScreenCoordsXY screenCoords)
    {
        CoordsXY mapCoords;
        auto& im = GetInputManager();

        if (!_trackPlaceCtrlState)
        {
            if (im.IsModifierKeyPressed(ModifierKey::ctrl))
            {
                auto interactionFlags = EnumsToFlags(
                    ViewportInteractionItem::Terrain, ViewportInteractionItem::Ride, ViewportInteractionItem::Footpath,
                    ViewportInteractionItem::PathAddition, ViewportInteractionItem::LargeScenery,
                    ViewportInteractionItem::Label, ViewportInteractionItem::Banner);

                auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
                if (info.SpriteType != ViewportInteractionItem::None)
                {
                    _trackPlaceCtrlZ = info.Element->GetBaseZ();
                    _trackPlaceCtrlState = true;
                }
            }
        }
        else
        {
            if (!(im.IsModifierKeyPressed(ModifierKey::ctrl)))
            {
                _trackPlaceCtrlState = false;
            }
        }

        if (!_trackPlaceShiftState)
        {
            if (im.IsModifierKeyPressed(ModifierKey::shift))
            {
                _trackPlaceShiftState = true;
                _trackPlaceShiftStart = screenCoords;
                _trackPlaceShiftZ = 0;
            }
        }
        else
        {
            if (im.IsModifierKeyPressed(ModifierKey::shift))
            {
                uint16_t maxHeight = ZoomLevel::max().ApplyTo(
                    std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

                _trackPlaceShiftZ = _trackPlaceShiftStart.y - screenCoords.y + 4;
                // Scale delta by zoom to match mouse position.
                auto* mainWnd = WindowGetMain();
                if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                {
                    _trackPlaceShiftZ = mainWnd->viewport->zoom.ApplyTo(_trackPlaceShiftZ);
                }
                _trackPlaceShiftZ = Floor2(_trackPlaceShiftZ, 8);

                // Clamp to maximum possible value of BaseHeight can offer.
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
            if (mapCoords.IsNull())
                return std::nullopt;

            _trackPlaceZ = 0;
            if (_trackPlaceShiftState)
            {
                auto surfaceElement = MapGetSurfaceElementAt(mapCoords);
                if (surfaceElement == nullptr)
                    return std::nullopt;
                auto mapZ = Floor2(surfaceElement->GetBaseZ(), 16);
                mapZ += _trackPlaceShiftZ;
                mapZ = std::max<int16_t>(mapZ, 16);
                _trackPlaceZ = mapZ;
            }
        }
        else
        {
            auto mapZ = _trackPlaceCtrlZ;
            auto mapXYCoords = ScreenGetMapXYWithZ(screenCoords, mapZ);
            if (mapXYCoords.has_value())
            {
                mapCoords = mapXYCoords.value();
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

        if (mapCoords.x == kLocationNull)
            return std::nullopt;

        return mapCoords.ToTileStart();
    }

    /**
     *
     *  rct2: 0x006C84CE
     */
    void WindowRideConstructionUpdateActiveElementsImpl()
    {
        WindowRideConstructionUpdateEnabledTrackPieces();
        if (auto currentRide = GetRide(_currentRideIndex);
            !currentRide || currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::isMaze))
        {
            return;
        }

        auto window = static_cast<RideConstructionWindow*>(WindowFindByClass(WindowClass::RideConstruction));
        if (!window)
        {
            return;
        }

        window->UpdateMapSelection();

        _selectedTrackType = TrackElemType::None;
        if (_rideConstructionState == RideConstructionState::Selected)
        {
            TileElement* tileElement;
            if (GetTrackElementOriginAndApplyChanges(
                    { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType, 0,
                    &tileElement, 0)
                != std::nullopt)
            {
                _selectedTrackType = tileElement->AsTrack()->GetTrackType();
                if (TrackTypeHasSpeedSetting(tileElement->AsTrack()->GetTrackType()))
                    _currentBrakeSpeed = tileElement->AsTrack()->GetBrakeBoosterSpeed();
                _currentSeatRotationAngle = tileElement->AsTrack()->GetSeatRotation();
            }
        }

        window->UpdatePossibleRideConfigurations();
        window->UpdateWidgets();
    }

    /**
     *
     *  rct2: 0x006C6A77
     */
    void WindowRideConstructionUpdateEnabledTrackPieces()
    {
        auto ride = GetRide(_currentRideIndex);
        if (ride == nullptr)
            return;

        auto rideEntry = ride->GetRideEntry();
        if (rideEntry == nullptr)
            return;

        auto trackDrawerDescriptor = getCurrentTrackDrawerDescriptor(ride->GetRideTypeDescriptor());
        UpdateEnabledRidePieces(trackDrawerDescriptor);
    }

    /**
     *
     *  rct2: 0x006C94D8
     */
    void UpdateGhostTrackAndArrow()
    {
        RideId rideIndex;
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
            RideConstructionInvalidateCurrentTrack();
            _currentTrackSelectionFlags &= ~TRACK_SELECTION_FLAG_RECHECK;
        }

        switch (_rideConstructionState)
        {
            case RideConstructionState::Front:
            case RideConstructionState::Back:
            {
                // place ghost piece
                if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
                {
                    if (WindowRideConstructionUpdateState(
                            &type, &direction, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr))
                    {
                        RideConstructionRemoveGhosts();
                    }
                    else
                    {
                        _currentTrackPrice = PlaceProvisionalTrackPiece(
                            rideIndex, type, direction, liftHillAndAlternativeState, trackPos);
                        WindowRideConstructionUpdateActiveElements();

                        // Invalidate previous track piece (we may not be changing height!)
                        VirtualFloorInvalidate();

                        if (!isToolActive(WindowClass::Scenery))
                        {
                            // Set height to where the next track piece would begin
                            VirtualFloorSetHeight(_currentTrackBegin.z);
                        }
                    }
                }
                // update flashing arrow
                auto curTime = Platform::GetTicks();
                if (_rideConstructionNextArrowPulse >= curTime)
                    break;
                _rideConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

                _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
                trackPos = _currentTrackBegin;
                direction = _currentTrackPieceDirection;
                type = _currentTrackPieceType;
                // diagonal pieces trigger this
                if (direction >= 4)
                    direction += 4;
                if (_rideConstructionState == RideConstructionState::Back)
                    direction = DirectionReverse(direction);
                gMapSelectArrowPosition = trackPos;
                gMapSelectArrowDirection = direction;
                gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
                if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
                MapInvalidateTileFull(trackPos);
                break;
            }
            case RideConstructionState::Selected:
            {
                auto curTime = Platform::GetTicks();
                if (_rideConstructionNextArrowPulse >= curTime)
                    break;
                _rideConstructionNextArrowPulse = curTime + ARROW_PULSE_DURATION;

                _currentTrackSelectionFlags ^= TRACK_SELECTION_FLAG_ARROW;
                direction = _currentTrackPieceDirection & 3;
                type = _currentTrackPieceType;
                uint16_t flags = _currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW ? TRACK_ELEMENT_SET_HIGHLIGHT_TRUE
                                                                                          : TRACK_ELEMENT_SET_HIGHLIGHT_FALSE;
                auto newCoords = GetTrackElementOriginAndApplyChanges(
                    { _currentTrackBegin, static_cast<Direction>(direction) }, type, 0, nullptr, flags);
                if (!newCoords.has_value())
                {
                    RideConstructionRemoveGhosts();
                    _rideConstructionState = RideConstructionState::State0;
                }
                break;
            }
            case RideConstructionState::MazeBuild:
            case RideConstructionState::MazeMove:
            case RideConstructionState::MazeFill:
            {
                auto curTime = Platform::GetTicks();
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
                MapInvalidateTileFull(trackPos);
                break;
            }
            default:
                break;
        }
    }

    /**
     *
     *  rct2: 0x006CC6A8
     */
    void RideConstructionToolupdateConstruct(const ScreenCoordsXY& screenCoords)
    {
        int32_t z;
        const PreviewTrack* trackBlock;

        MapInvalidateMapSelectionTiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        auto mapCoords = RideGetPlacePositionFromScreenPosition(screenCoords);
        if (!mapCoords)
        {
            RideConstructionInvalidateCurrentTrack();
            MapInvalidateMapSelectionTiles();
            return;
        }

        z = _trackPlaceZ;
        if (z == 0)
            z = MapGetHighestZ(*mapCoords);

        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_GREEN;
        gMapSelectArrowPosition = CoordsXYZ{ *mapCoords, z };
        gMapSelectArrowDirection = _currentTrackPieceDirection;
        gMapSelectionTiles.clear();
        gMapSelectionTiles.push_back(*mapCoords);

        RideId rideIndex;
        int32_t trackType, trackDirection, liftHillAndAlternativeState;
        if (WindowRideConstructionUpdateState(
                &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, nullptr, nullptr))
        {
            RideConstructionInvalidateCurrentTrack();
            MapInvalidateMapSelectionTiles();
            return;
        }
        _currentTrackPieceType = trackType;
        auto ride = GetRide(_currentRideIndex);
        if (!ride)
        {
            return;
        }

        const auto& rtd = ride->GetRideTypeDescriptor();
        if (!rtd.HasFlag(RtdFlag::isMaze))
        {
            auto window = static_cast<RideConstructionWindow*>(WindowFindByClass(WindowClass::RideConstruction));
            if (!window)
            {
                return;
            }
            // Re-using this other code, very slight difference from original
            //   - Original code checks for MSB mask instead of 255 on trackPart->var_00
            //   - Original code checks this first as its already set origin tile, probably just a micro optimisation
            window->SelectMapTiles(trackType, trackDirection, *mapCoords);
        }

        gMapSelectArrowPosition.z = z;
        if (_trackPlaceZ == 0)
        {
            // Raise z above all slopes and water
            if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
            {
                int32_t highestZ = 0;
                for (const auto& selectedTile : gMapSelectionTiles)
                {
                    if (MapIsLocationValid(selectedTile))
                    {
                        z = MapGetHighestZ(selectedTile);
                        if (z > highestZ)
                            highestZ = z;
                    }
                }
            }
            // Loc6CC8BF:
            // z = MapGetHighestZ(x >> 5, y >> 5);
        }
        // Loc6CC91B:
        const auto& ted = GetTrackElementDescriptor(trackType);
        trackBlock = ted.block;
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
            MapInvalidateMapSelectionTiles();
            return;
        }

        _previousTrackPiece = _currentTrackBegin;
        // search for appropriate z value for ghost, up to max ride height
        int numAttempts = (z <= MAX_TRACK_HEIGHT ? ((MAX_TRACK_HEIGHT - z) / kCoordsZStep + 1) : 2);

        if (rtd.HasFlag(RtdFlag::isMaze))
        {
            for (int zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
            {
                CoordsXYZ trackPos{};
                WindowRideConstructionUpdateState(
                    &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
                _currentTrackPrice = PlaceProvisionalTrackPiece(
                    rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
                if (_currentTrackPrice != kMoney64Undefined)
                    break;

                _currentTrackBegin.z -= 8;
                if (_currentTrackBegin.z < 0)
                    break;

                _currentTrackBegin.z += 16;
            }

            auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
            ContextBroadcastIntent(&intent);
            MapInvalidateMapSelectionTiles();
            return;
        }

        for (int zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
        {
            CoordsXYZ trackPos{};
            WindowRideConstructionUpdateState(
                &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
            _currentTrackPrice = PlaceProvisionalTrackPiece(
                rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
            mapCoords = trackPos;
            z = trackPos.z;
            if (_currentTrackPrice != kMoney64Undefined)
                break;

            _currentTrackBegin.z -= 8;
            if (_currentTrackBegin.z < 0)
                break;

            _currentTrackBegin.z += 16;
        }

        if (_autoRotatingShop && _rideConstructionState == RideConstructionState::Place
            && ride->GetRideTypeDescriptor().HasFlag(RtdFlag::isShopOrFacility))
        {
            PathElement* pathsByDir[kNumOrthogonalDirections];

            bool keepOrientation = false;
            for (int8_t i = 0; i < kNumOrthogonalDirections; i++)
            {
                const auto testLoc = CoordsXYZ{ *mapCoords + CoordsDirectionDelta[i], z };
                if (!MapIsLocationOwned(testLoc))
                {
                    pathsByDir[i] = nullptr;
                    continue;
                }

                pathsByDir[i] = MapGetFootpathElement(testLoc);

                if (pathsByDir[i] != nullptr && pathsByDir[i]->IsSloped() && pathsByDir[i]->GetSlopeDirection() != i)
                {
                    pathsByDir[i] = nullptr;
                }

                // Sloped path on the level below
                if (pathsByDir[i] == nullptr)
                {
                    pathsByDir[i] = MapGetFootpathElement({ *mapCoords + CoordsDirectionDelta[i], z - kPathHeightStep });

                    if (pathsByDir[i] != nullptr
                        && (!pathsByDir[i]->IsSloped() || pathsByDir[i]->GetSlopeDirection() != DirectionReverse(i)))
                    {
                        pathsByDir[i] = nullptr;
                    }
                }

                if (pathsByDir[i] != nullptr && pathsByDir[i]->IsQueue())
                {
                    pathsByDir[i] = nullptr;
                }

                if (pathsByDir[i] != nullptr && i == _currentTrackPieceDirection)
                {
                    keepOrientation = true;
                    break;
                }
            }

            if (!keepOrientation)
            {
                for (int8_t i = 0; i < kNumOrthogonalDirections; i++)
                {
                    if (pathsByDir[i] != nullptr)
                    {
                        _currentTrackPieceDirection = i;

                        CoordsXYZ trackPos{};
                        WindowRideConstructionUpdateState(
                            &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, nullptr);
                        PlaceProvisionalTrackPiece(rideIndex, trackType, trackDirection, liftHillAndAlternativeState, trackPos);
                        gMapSelectArrowDirection = _currentTrackPieceDirection;
                        break;
                    }
                }
            }
        }

        WindowRideConstructionUpdateActiveElements();
        MapInvalidateMapSelectionTiles();
    }

    /**
     *
     *  rct2: 0x006CD354
     */
    void RideConstructionToolupdateEntranceExit(const ScreenCoordsXY& screenCoords)
    {
        MapInvalidateSelectionRect();
        MapInvalidateMapSelectionTiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        CoordsXYZD entranceOrExitCoords = RideGetEntranceOrExitPositionFromScreenPosition(screenCoords);
        if (gRideEntranceExitPlaceDirection == INVALID_DIRECTION)
        {
            RideConstructionInvalidateCurrentTrack();
            return;
        }
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
        gMapSelectType = MAP_SELECT_TYPE_FULL;
        gMapSelectPositionA = entranceOrExitCoords;
        gMapSelectPositionB = entranceOrExitCoords;
        gMapSelectArrowPosition = entranceOrExitCoords;
        gMapSelectArrowDirection = DirectionReverse(entranceOrExitCoords.direction);
        MapInvalidateSelectionRect();

        entranceOrExitCoords.direction = DirectionReverse(gRideEntranceExitPlaceDirection);
        StationIndex stationNum = gRideEntranceExitPlaceStationIndex;
        if (!(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ENTRANCE_OR_EXIT)
            || entranceOrExitCoords != gRideEntranceExitGhostPosition || stationNum != gRideEntranceExitGhostStationIndex)
        {
            auto ride = GetRide(_currentRideIndex);
            if (ride != nullptr)
            {
                _currentTrackPrice = RideEntranceExitPlaceGhost(
                    *ride, entranceOrExitCoords, entranceOrExitCoords.direction, gRideEntranceExitPlaceType, stationNum);
            }
            WindowRideConstructionUpdateActiveElements();
        }
    }

    /**
     *
     *  rct2: 0x006CCA73
     */
    void RideConstructionTooldownConstruct(const ScreenCoordsXY& screenCoords)
    {
        const CursorState* state = ContextGetCursorState();

        WindowBase* w;

        MapInvalidateMapSelectionTiles();
        RideConstructionInvalidateCurrentTrack();

        CoordsXYZ mapCoords{};
        int32_t trackType, z, highestZ;

        if (WindowRideConstructionUpdateState(&trackType, nullptr, nullptr, nullptr, nullptr, nullptr))
            return;

        z = mapCoords.z;
        _currentTrackPieceType = trackType;

        // Raise z above all slopes and water
        highestZ = 0;
        if (gMapSelectFlags & MAP_SELECT_FLAG_ENABLE_CONSTRUCT)
        {
            for (const auto& selectedTile : gMapSelectionTiles)
            {
                if (!MapIsLocationValid(selectedTile))
                    continue;

                z = MapGetHighestZ(selectedTile);
                if (z > highestZ)
                    highestZ = z;
            }
        }

        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
        auto ridePlacePosition = RideGetPlacePositionFromScreenPosition(screenCoords);
        if (!ridePlacePosition)
            return;

        mapCoords = { *ridePlacePosition, z };

        z = _trackPlaceZ;
        if (z == 0)
            z = MapGetHighestZ(mapCoords);

        ToolCancel();

        auto ride = GetRide(_currentRideIndex);
        if (ride == nullptr)
            return;

        if (_trackPlaceZ == 0)
        {
            const auto& ted = GetTrackElementDescriptor(_currentTrackPieceType);
            const PreviewTrack* trackBlock = ted.block;
            int32_t bx = 0;
            do
            {
                bx = std::min<int32_t>(bx, trackBlock->z);
                trackBlock++;
            } while (trackBlock->index != 255);
            z -= bx;

            // FIX not sure exactly why it starts trial and error place from a lower Z, but it causes issues with disable
            // clearance
            if (!GetGameState().Cheats.DisableClearanceChecks && z > kMinimumLandZ)
            {
                z -= LAND_HEIGHT_STEP;
            }
        }
        else
        {
            z = _trackPlaceZ;
        }

        // search for z value to build at, up to max ride height
        int numAttempts = (z <= MAX_TRACK_HEIGHT ? ((MAX_TRACK_HEIGHT - z) / kCoordsZStep + 1) : 2);

        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.HasFlag(RtdFlag::isMaze))
        {
            for (int32_t zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
            {
                _rideConstructionState = RideConstructionState::MazeBuild;
                _currentTrackBegin.x = mapCoords.x;
                _currentTrackBegin.y = mapCoords.y;
                _currentTrackBegin.z = z;
                _currentTrackSelectionFlags = 0;
                auto intent = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
                ContextBroadcastIntent(&intent);
                w = WindowFindByClass(WindowClass::RideConstruction);
                if (w == nullptr)
                    break;

                gDisableErrorWindowSound = true;

                auto gameAction = MazeSetTrackAction(
                    CoordsXYZD{ _currentTrackBegin, 0 }, true, _currentRideIndex, GC_SET_MAZE_TRACK_BUILD);
                auto mazeSetTrackResult = GameActions::Execute(&gameAction);
                if (mazeSetTrackResult.Error == GameActions::Status::Ok)
                {
                    _trackPlaceCost = mazeSetTrackResult.Cost;
                    _trackPlaceErrorMessage = STR_NONE;
                }
                else
                {
                    _trackPlaceCost = kMoney64Undefined;
                    _trackPlaceErrorMessage = std::get<StringId>(mazeSetTrackResult.ErrorMessage);
                }

                gDisableErrorWindowSound = false;

                if (mazeSetTrackResult.Error != GameActions::Status::Ok)
                {
                    _rideConstructionState = RideConstructionState::Place;
                    StringId errorText = std::get<StringId>(mazeSetTrackResult.ErrorMessage);
                    z -= 8;
                    if (errorText == STR_NOT_ENOUGH_CASH_REQUIRES || errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER
                        || errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER || errorText == STR_RIDE_CANT_BUILD_THIS_UNDERWATER
                        || errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND || errorText == STR_TOO_HIGH_FOR_SUPPORTS
                        || zAttempts == (numAttempts - 1) || z < 0)
                    {
                        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, state->position.x);
                        w = WindowFindByClass(WindowClass::RideConstruction);
                        if (w != nullptr)
                        {
                            ToolSet(*w, WIDX_CONSTRUCT, Tool::Crosshair);
                            InputSetFlag(INPUT_FLAG_6, true);
                            _trackPlaceCtrlState = false;
                            _trackPlaceShiftState = false;
                        }
                        auto intent2 = Intent(INTENT_ACTION_UPDATE_MAZE_CONSTRUCTION);
                        ContextBroadcastIntent(&intent2);
                        break;
                    }
                    z += 16;
                }
                else
                {
                    WindowCloseByClass(WindowClass::Error);
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, _currentTrackBegin);
                    break;
                }
            }
            return;
        }

        for (int32_t zAttempts = 0; zAttempts < numAttempts; ++zAttempts)
        {
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackBegin.x = mapCoords.x;
            _currentTrackBegin.y = mapCoords.y;
            _currentTrackBegin.z = z;
            _currentTrackSelectionFlags = 0;
            WindowRideConstructionUpdateActiveElements();
            w = WindowFindByClass(WindowClass::RideConstruction);
            if (w == nullptr)
                break;

            gDisableErrorWindowSound = true;
            w->OnMouseDown(WIDX_CONSTRUCT);
            gDisableErrorWindowSound = false;

            if (_trackPlaceCost == kMoney64Undefined)
            {
                StringId errorText = _trackPlaceErrorMessage;
                z -= 8;
                if (errorText == STR_NOT_ENOUGH_CASH_REQUIRES || errorText == STR_CAN_ONLY_BUILD_THIS_UNDERWATER
                    || errorText == STR_CAN_ONLY_BUILD_THIS_ON_WATER || errorText == STR_CAN_ONLY_BUILD_THIS_ABOVE_GROUND
                    || errorText == STR_TOO_HIGH_FOR_SUPPORTS || errorText == STR_TOO_HIGH
                    || errorText == STR_LOCAL_AUTHORITY_WONT_ALLOW_CONSTRUCTION_ABOVE_TREE_HEIGHT
                    || zAttempts == (numAttempts - 1) || z < 0)
                {
                    int32_t saveTrackDirection = _currentTrackPieceDirection;
                    auto saveCurrentTrackCurve = _currentlySelectedTrack;
                    auto savePreviousTrackPitchEnd = _previousTrackPitchEnd;
                    auto saveCurrentTrackPitchEnd = _currentTrackPitchEnd;
                    auto savePreviousTrackRollEnd = _previousTrackRollEnd;
                    auto saveCurrentTrackRollEnd = _currentTrackRollEnd;
                    int32_t saveCurrentTrackAlternative = _currentTrackAlternative;
                    int32_t saveCurrentTrackLiftHill = _currentTrackLiftHill;

                    RideInitialiseConstructionWindow(*ride);

                    _currentTrackPieceDirection = saveTrackDirection;
                    _currentlySelectedTrack = saveCurrentTrackCurve;
                    _previousTrackPitchEnd = savePreviousTrackPitchEnd;
                    _currentTrackPitchEnd = saveCurrentTrackPitchEnd;
                    _previousTrackRollEnd = savePreviousTrackRollEnd;
                    _currentTrackRollEnd = saveCurrentTrackRollEnd;
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

    void WindowRideConstructionKeyboardShortcutTurnLeft()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        if (_currentlySelectedTrack.isTrackType)
            return;

        switch (_currentlySelectedTrack.curve)
        {
            case TrackCurve::LeftSmall:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                break;
            case TrackCurve::Left:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::LeftLarge:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::None:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::RightLarge:
                if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::Right:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::RightSmall:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::RightVerySmall:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::LeftVerySmall:
                break;
        }
    }

    void WindowRideConstructionKeyboardShortcutTurnRight()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        if (_currentlySelectedTrack.isTrackType)
            return;

        switch (_currentlySelectedTrack.curve)
        {
            case TrackCurve::RightSmall:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                break;
            case TrackCurve::Right:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::RightLarge:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::None:
                if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::LeftLarge:
                if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::Left:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::LeftSmall:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::LeftVerySmall:
                if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                    && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_SMALL);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                    && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEFT_CURVE_LARGE);
                }
                else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_STRAIGHT);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                    && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_LARGE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_SMALL);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                    && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_RIGHT_CURVE_VERY_SMALL);
                }
                else
                {
                    return;
                }
                break;
            case TrackCurve::RightVerySmall:
                break;
        }
    }

    void WindowRideConstructionKeyboardShortcutUseTrackDefault()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
        {
            w->OnMouseDown(WIDX_STRAIGHT);
        }

        if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
        {
            w->OnMouseDown(WIDX_LEVEL);
        }

        if (!WidgetIsDisabled(*w, WIDX_CHAIN_LIFT) && w->widgets[WIDX_CHAIN_LIFT].type != WindowWidgetType::Empty
            && _currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
        {
            w->OnMouseDown(WIDX_CHAIN_LIFT);
        }

        if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
        {
            w->OnMouseDown(WIDX_BANK_STRAIGHT);
        }
    }

    void WindowRideConstructionKeyboardShortcutSlopeDown()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackPitchEnd)
        {
            case TrackPitch::Down60:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
                    && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                break;
            case TrackPitch::Down25:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                break;
            case TrackPitch::None:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN);
                }
                else if (
                    IsTrackEnabled(TrackGroup::slopeVertical)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
                {
                    return;
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Up25:
                if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEVEL);
                }
                else if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Up60:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEVEL);
                }
                else if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN);
                }
                else if (
                    IsTrackEnabled(TrackGroup::slopeVertical)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
                {
                    return;
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Up90:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                break;
            default:
                return;
        }
    }

    void WindowRideConstructionKeyboardShortcutSlopeUp()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackPitchEnd)
        {
            case TrackPitch::Up60:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                break;
            case TrackPitch::Up25:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                break;
            case TrackPitch::None:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP);
                }
                else if (
                    IsTrackEnabled(TrackGroup::slopeVertical)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
                {
                    return;
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Down25:
                if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEVEL);
                }
                else if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP);
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Down60:
                if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN);
                }
                else if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_LEVEL);
                }
                else if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP);
                }
                else if (
                    IsTrackEnabled(TrackGroup::slopeVertical)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
                {
                    return;
                }
                else if (
                    !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_STEEP);
                }
                else
                {
                    return;
                }
                break;
            case TrackPitch::Down90:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                    && w->widgets[WIDX_SLOPE_UP_STEEP].image.GetIndex() == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
                    && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_STEEP);
                }
                break;
            default:
                return;
        }
    }

    void WindowRideConstructionKeyboardShortcutChainLiftToggle()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_CHAIN_LIFT)
            || w->widgets[WIDX_CHAIN_LIFT].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_CHAIN_LIFT);
    }

    void WindowRideConstructionKeyboardShortcutBankLeft()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT)
            || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackRollEnd)
        {
            case TrackRoll::None:
                if (!WidgetIsDisabled(*w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_LEFT);
                }
                break;
            case TrackRoll::Right:
                if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_STRAIGHT);
                }
                else if (!WidgetIsDisabled(*w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_LEFT);
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

    void WindowRideConstructionKeyboardShortcutBankRight()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT)
            || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackRollEnd)
        {
            case TrackRoll::None:
                if (!WidgetIsDisabled(*w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_RIGHT);
                }
                break;
            case TrackRoll::Left:
                if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_STRAIGHT);
                }
                else if (!WidgetIsDisabled(*w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_BANK_RIGHT);
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

    void WindowRideConstructionKeyboardShortcutPreviousTrack()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_PREVIOUS_SECTION)
            || w->widgets[WIDX_PREVIOUS_SECTION].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_PREVIOUS_SECTION);
    }

    void WindowRideConstructionKeyboardShortcutNextTrack()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_NEXT_SECTION)
            || w->widgets[WIDX_NEXT_SECTION].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_NEXT_SECTION);
    }

    void WindowRideConstructionKeyboardShortcutBuildCurrent()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_CONSTRUCT);
    }

    void WindowRideConstructionKeyboardShortcutDemolishCurrent()
    {
        WindowBase* w = WindowFindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_DEMOLISH) || w->widgets[WIDX_DEMOLISH].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_DEMOLISH);
    }

    static void WindowRideConstructionMouseUpDemolishNextPiece(const CoordsXYZD& piecePos, int32_t type)
    {
        if (_gotoStartPlacementMode)
        {
            _currentTrackBegin.z = Floor2(piecePos.z, kCoordsZStep);
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackSelectionFlags = 0;
            _currentTrackPieceDirection = piecePos.direction & 3;
            auto savedSelectedTrack = _currentlySelectedTrack;
            auto savedPreviousTrackPitchEnd = _previousTrackPitchEnd;
            auto savedCurrentTrackPitchEnd = _currentTrackPitchEnd;
            auto savedPreviousTrackRollEnd = _previousTrackRollEnd;
            auto savedCurrentTrackRollEnd = _currentTrackRollEnd;
            int32_t savedCurrentTrackAlternative = _currentTrackAlternative;
            int32_t savedCurrentTrackLiftHill = _currentTrackLiftHill;
            RideConstructionSetDefaultNextPiece();
            WindowRideConstructionUpdateActiveElements();
            auto ride = GetRide(_currentRideIndex);
            if (!RideTryGetOriginElement(*ride, nullptr))
            {
                RideInitialiseConstructionWindow(*ride);
                _currentTrackPieceDirection = piecePos.direction & 3;
                if (!savedSelectedTrack.isTrackType)
                {
                    _currentlySelectedTrack = savedSelectedTrack;
                    _previousTrackPitchEnd = savedPreviousTrackPitchEnd;
                    _currentTrackPitchEnd = savedCurrentTrackPitchEnd;
                    _previousTrackRollEnd = savedPreviousTrackRollEnd;
                    _currentTrackRollEnd = savedCurrentTrackRollEnd;
                    _currentTrackAlternative = savedCurrentTrackAlternative;
                    _currentTrackLiftHill = savedCurrentTrackLiftHill;
                    WindowRideConstructionUpdateActiveElements();
                }
            }
        }
        else
        {
            if (_rideConstructionState2 == RideConstructionState::Selected
                || _rideConstructionState2 == RideConstructionState::Front)
            {
                if (type == TrackElemType::MiddleStation || type == TrackElemType::BeginStation)
                {
                    type = TrackElemType::EndStation;
                }
            }
            if (_rideConstructionState2 == RideConstructionState::Back)
            {
                if (type == TrackElemType::MiddleStation)
                {
                    type = TrackElemType::BeginStation;
                }
            }
            if (NetworkGetMode() == NETWORK_MODE_CLIENT)
            {
                // rideConstructionState needs to be set again to the proper value, this only affects the client
                _rideConstructionState = RideConstructionState::Selected;
            }
            _currentTrackBegin = piecePos;
            _currentTrackPieceDirection = piecePos.direction;
            _currentTrackPieceType = type;
            _currentTrackSelectionFlags = 0;
            if (_rideConstructionState2 == RideConstructionState::Front)
            {
                RideSelectNextSection();
            }
            else if (_rideConstructionState2 == RideConstructionState::Back)
            {
                RideSelectPreviousSection();
            }
            WindowRideConstructionUpdateActiveElements();
        }
    }

    static void WindowRideConstructionUpdateActiveElements()
    {
        auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS);
        ContextBroadcastIntent(&intent);
    }
} // namespace OpenRCT2::Ui::Windows
