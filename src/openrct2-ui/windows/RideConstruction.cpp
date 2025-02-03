/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2-ui/windows/Windows.h>
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
#include <openrct2/core/Numerics.hpp>
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
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/ConstructionClearance.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/TrackElement.h>

constexpr int8_t kDefaultSpeedIncrement = 2;
constexpr int8_t kDefaultMinimumSpeed = 2;

constexpr uint8_t kVerticalDropButtonStart = 6;

using namespace OpenRCT2::Numerics;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Ui::Windows
{
    bool gDisableErrorWindowSound = false;

    static RideConstructionState _rideConstructionState2;

    static bool WindowRideConstructionUpdateState(
        OpenRCT2::TrackElemType* trackType, int32_t* trackDirection, RideId* rideIndex,
        SelectedLiftAndInverted* _liftHillAndAlternativeState, CoordsXYZ* trackPos, int32_t* properties);
    money64 PlaceProvisionalTrackPiece(
        RideId rideIndex, OpenRCT2::TrackElemType trackType, int32_t trackDirection,
        SelectedLiftAndInverted liftHillAndAlternativeState, const CoordsXYZ& trackPos);
    static std::pair<bool, OpenRCT2::TrackElemType> WindowRideConstructionUpdateStateGetTrackElement();

    static constexpr StringId kWindowTitle = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
    static constexpr int32_t WH = 394;
    static constexpr int32_t WW = 210;

    static constexpr uint16_t kArrowPulseDuration = 200;
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
        WIDX_SLOPE_DOWN_VERTICAL,
        WIDX_SLOPE_DOWN_STEEP,
        WIDX_SLOPE_DOWN,
        WIDX_LEVEL,
        WIDX_SLOPE_UP,
        WIDX_SLOPE_UP_STEEP,
        WIDX_SLOPE_UP_VERTICAL,
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
    static constexpr Widget kRideConstructionWidgets[] = {
        WINDOW_SHIM(kWindowTitle, WW, WH),
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
        MakeWidget        ({  6,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_VERTICAL_DROP),     STR_RIDE_CONSTRUCTION_VERTICAL_DROP_TIP          ),
        MakeWidget        ({ 30,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP),  STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP          ),
        MakeWidget        ({ 54,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_DOWN),        STR_RIDE_CONSTRUCTION_SLOPE_DOWN_TIP                ),
        MakeWidget        ({ 78,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_LEVEL),       STR_RIDE_CONSTRUCTION_LEVEL_TIP                     ),
        MakeWidget        ({102,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP),          STR_RIDE_CONSTRUCTION_SLOPE_UP_TIP                  ),
        MakeWidget        ({126,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP),    STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP            ),
        MakeWidget        ({150,  88}, {     24,  24}, WindowWidgetType::FlatBtn,  WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_VERTICAL_RISE),     STR_RIDE_CONSTRUCTION_VERTICAL_RISE_TIP            ),
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

    static constexpr StringId kSeatAngleRotationStrings[] = {
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_180, STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_135,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_90,  STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_45,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_0,       STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_45,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_90,      STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_135,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_180,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_225,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_270,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_315,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_360,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_405,
        STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_450,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_495,
    };

    static void WindowRideConstructionMouseUpDemolishNextPiece(const CoordsXYZD& piecePos, OpenRCT2::TrackElemType type);
    static void WindowRideConstructionUpdateActiveElements();

    /* move to ride.c */
    static void CloseRideWindowForConstruction(RideId rideId)
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByNumber(WindowClass::Ride, rideId.ToUnderlying());
        if (w != nullptr && w->page == 1)
            windowMgr->Close(*w);
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

            SetWidgets(kRideConstructionWidgets);
            number = _currentRideIndex.ToUnderlying();

            InitScrollWidgets();

            WindowPushOthersRight(*this);
            ShowGridlines();

            _currentTrackPrice = kMoney64Undefined;
            _currentBrakeSpeed = 8;
            _currentColourScheme = RideColourScheme::main;
            _currentSeatRotationAngle = 4;

            _currentlySelectedTrack = currentRide->GetRideTypeDescriptor().StartTrackPiece;
            _currentTrackPitchEnd = TrackPitch::None;
            _currentTrackRollEnd = TrackRoll::None;
            _currentTrackHasLiftHill = false;
            _currentTrackAlternative.clearAll();

            if (currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::startConstructionInverted))
                _currentTrackAlternative.set(AlternativeTrackFlag::inverted);

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
                && !GetGameState().Cheats.enableAllDrawableTrackPieces)
            {
                // Disable lift hill toggle and banking if current track piece is uphill
                if (_previousTrackPitchEnd == TrackPitch::Up25 || _previousTrackPitchEnd == TrackPitch::Up60
                    || _currentTrackPitchEnd == TrackPitch::Up25 || _currentTrackPitchEnd == TrackPitch::Up60)
                    disabledWidgets |= 1uLL << WIDX_CHAIN_LIFT | (1uLL << WIDX_BANK_LEFT) | (1uLL << WIDX_BANK_RIGHT);
                // Disable upward slope if current track piece is not flat
                if ((_previousTrackPitchEnd != TrackPitch::None || _previousTrackRollEnd != TrackRoll::None)
                    && !(_currentTrackHasLiftHill))
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
                        || (IsTrackEnabled(TrackGroup::slopeSteepLong) && !IsTrackEnabled(TrackGroup::diagSlopeSteepLong)
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
                    if (!IsTrackEnabled(TrackGroup::flatToSteepSlope)
                        && !(
                            IsTrackEnabled(TrackGroup::slopeSteepLong)
                            && !(
                                TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)
                                && !IsTrackEnabled(TrackGroup::diagSlopeSteepLong))))
                    {
                        disabledWidgets |= (1uLL << WIDX_LEVEL);
                    }
                    break;
                case TrackPitch::Up25:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN);
                    break;
                case TrackPitch::Up60:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN);
                    if (!IsTrackEnabled(TrackGroup::flatToSteepSlope)
                        && !(
                            IsTrackEnabled(TrackGroup::slopeSteepLong)
                            && !(
                                TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)
                                && !IsTrackEnabled(TrackGroup::diagSlopeSteepLong))))
                    {
                        disabledWidgets |= (1uLL << WIDX_LEVEL);
                    }
                    break;
                case TrackPitch::Down90:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL) | (1uLL << WIDX_SLOPE_UP)
                        | (1uLL << WIDX_SLOPE_UP_STEEP);
                    break;
                case TrackPitch::Up90:
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_DOWN) | (1uLL << WIDX_LEVEL)
                        | (1uLL << WIDX_SLOPE_UP);
                    break;
            }
            if (_previousTrackPitchEnd == TrackPitch::None)
            {
                if (!IsTrackEnabled(TrackGroup::flatToSteepSlope) && !IsTrackEnabled(TrackGroup::slopeSteepLong)
                    && !IsTrackEnabled(TrackGroup::diagSlopeSteepLong))
                {
                    disabledWidgets |= (1uLL << WIDX_SLOPE_DOWN_STEEP) | (1uLL << WIDX_SLOPE_UP_STEEP);
                }
            }
            if (IsTrackEnabled(TrackGroup::slopeVertical))
            {
                if (_previousTrackPitchEnd == TrackPitch::Up60 && _currentTrackPieceDirection < 4)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_VERTICAL);
                }
                if (_previousTrackPitchEnd == TrackPitch::Up90)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_UP_VERTICAL);
                }
                if (_previousTrackPitchEnd == TrackPitch::Down60 && _currentTrackPieceDirection < 4)
                {
                    disabledWidgets &= ~(1uLL << WIDX_SLOPE_DOWN_VERTICAL);
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
            if ((_currentTrackHasLiftHill) && !GetGameState().Cheats.enableChainLiftOnAllTrack)
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
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE)
                    | (1uLL << WIDX_SLOPE_DOWN_STEEP);
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
                disabledWidgets |= (1uLL << WIDX_LEFT_CURVE_LARGE) | (1uLL << WIDX_RIGHT_CURVE_LARGE)
                    | (1uLL << WIDX_SLOPE_UP_STEEP);
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
                                || GetGameState().Cheats.enableAllDrawableTrackPieces))
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
            if (GetGameState().Cheats.enableChainLiftOnAllTrack)
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
                    InvalidateWidget(i);
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
                        InvalidateWidget(WIDX_CONSTRUCT);
                        break;
                    default:
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
                    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
                        VirtualFloorSetHeight(_currentTrackBegin.z);
                    break;
                case WIDX_PREVIOUS_SECTION:
                    VirtualFloorInvalidate();
                    RideSelectPreviousSection();
                    if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
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
                case WIDX_SLOPE_DOWN_VERTICAL:
                    RideConstructionInvalidateCurrentTrack();
                    UpdateLiftHillSelected(TrackPitch::Down90);
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

                    UpdateLiftHillSelected(TrackPitch::Down60);
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

                    UpdateLiftHillSelected(TrackPitch::Up60);
                    break;
                case WIDX_SLOPE_UP_VERTICAL:
                    RideConstructionInvalidateCurrentTrack();
                    UpdateLiftHillSelected(TrackPitch::Up90);
                    break;
                case WIDX_CHAIN_LIFT:
                    RideConstructionInvalidateCurrentTrack();
                    _currentTrackHasLiftHill = !_currentTrackHasLiftHill;
                    if ((_currentTrackHasLiftHill) && !GetGameState().Cheats.enableChainLiftOnAllTrack)
                        _currentTrackAlternative.unset(AlternativeTrackFlag::alternativePieces);
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
                        if (GetGameState().Cheats.unlockOperatingLimits)
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
                    _currentTrackAlternative.unset(AlternativeTrackFlag::alternativePieces);
                    _currentTrackPrice = kMoney64Undefined;
                    WindowRideConstructionUpdateActiveElements();
                    break;
                case WIDX_O_TRACK:
                    RideConstructionInvalidateCurrentTrack();
                    _currentTrackAlternative.set(AlternativeTrackFlag::alternativePieces);
                    if (!GetGameState().Cheats.enableChainLiftOnAllTrack)
                        _currentTrackHasLiftHill = false;
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
            OpenRCT2::TrackElemType trackPiece = _specialElementDropdownState.Elements[selectedIndex].TrackType;
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
                    _currentTrackHasLiftHill = false;
                    break;
                case TrackElemType::BlockBrakes:
                case TrackElemType::DiagBlockBrakes:
                    _currentBrakeSpeed = kRCT2DefaultBlockBrakeSpeed;
                default:
                    break;
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
                    brakeSpeed2 = GetUnifiedBoosterSpeed(currentRide->type, brakeSpeed2);
                }
                ft.Add<uint16_t>(brakeSpeed2);
            }
            else
            {
                ft.Increment(2);
            }

            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].text = kSeatAngleRotationStrings[_currentSeatRotationAngle];

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

        static void OnDrawUpdateCoveredPieces(const TrackDrawerDescriptor& trackDrawerDescriptor, std::span<Widget> widgets)
        {
            widgets[WIDX_U_TRACK].type = WindowWidgetType::Empty;
            widgets[WIDX_O_TRACK].type = WindowWidgetType::Empty;

            if (!trackDrawerDescriptor.HasCoveredPieces())
                return;

            auto trackElementResult = WindowRideConstructionUpdateStateGetTrackElement();
            if (!std::get<0>(trackElementResult))
                return;

            auto selectedTrackType = std::get<1>(trackElementResult);
            const auto& ted = GetTrackElementDescriptor(selectedTrackType);
            auto availableGroups = trackDrawerDescriptor.Covered.enabledTrackGroups;

            if (!availableGroups.get(EnumValue(ted.definition.group)))
                return;

            widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
            widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;

            widgets[WIDX_U_TRACK].image = ImageId(trackDrawerDescriptor.Regular.icon);
            widgets[WIDX_O_TRACK].image = ImageId(trackDrawerDescriptor.Covered.icon);
            widgets[WIDX_U_TRACK].tooltip = trackDrawerDescriptor.Regular.tooltip;
            widgets[WIDX_O_TRACK].tooltip = trackDrawerDescriptor.Covered.tooltip;
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
            OpenRCT2::TrackElemType trackType;
            int32_t trackDirection;
            SelectedLiftAndInverted liftHillAndInvertedState{};
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
                && !gameState.Cheats.enableAllDrawableTrackPieces)
            {
                _currentTrackHasLiftHill = true;
            }

            if ((IsTrackEnabled(TrackGroup::liftHill) && !_currentlySelectedTrack.isTrackType)
                || (gameState.Cheats.enableChainLiftOnAllTrack
                    && currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack)))
            {
                widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::FlatBtn;
            }
            else
            {
                widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::Empty;
            }

            int32_t x = kVerticalDropButtonStart;
            for (int32_t i = WIDX_SLOPE_DOWN_VERTICAL; i <= WIDX_SLOPE_UP_VERTICAL; i++)
            {
                widgets[i].moveToX(x);
                x += 24;
            }

            widgets[WIDX_SLOPE_DOWN_VERTICAL].type = WindowWidgetType::Empty;
            widgets[WIDX_SLOPE_UP_VERTICAL].type = WindowWidgetType::Empty;

            if (IsTrackEnabled(TrackGroup::slopeVertical) && !TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection))
            {
                if (_previousTrackPitchEnd == TrackPitch::Up60 || _previousTrackPitchEnd == TrackPitch::Up90)
                {
                    widgets[WIDX_SLOPE_UP_VERTICAL].type = WindowWidgetType::FlatBtn;
                }
                else if (_previousTrackPitchEnd == TrackPitch::Down60 || _previousTrackPitchEnd == TrackPitch::Down90)
                {
                    widgets[WIDX_SLOPE_DOWN_VERTICAL].type = WindowWidgetType::FlatBtn;
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
                OnDrawUpdateCoveredPieces(trackDrawerDescriptor, widgets);
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
                case TrackPitch::Down90:
                    widgetIndex = WIDX_SLOPE_DOWN_VERTICAL;
                    break;
                case TrackPitch::Down60:
                    widgetIndex = WIDX_SLOPE_DOWN_STEEP;
                    break;
                case TrackPitch::Down25:
                    widgetIndex = WIDX_SLOPE_DOWN;
                    break;
                case TrackPitch::Up25:
                    widgetIndex = WIDX_SLOPE_UP;
                    break;
                case TrackPitch::Up60:
                    widgetIndex = WIDX_SLOPE_UP_STEEP;
                    break;
                case TrackPitch::Up90:
                    widgetIndex = WIDX_SLOPE_UP_VERTICAL;
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
                    if (_currentTrackAlternative.has(AlternativeTrackFlag::alternativePieces))
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

            if (_currentTrackHasLiftHill)
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
            OpenRCT2::TrackElemType trackType;
            int32_t trackDirection;
            CoordsXYZ trackPos{};

            MapInvalidateMapSelectionTiles();
            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags |= MAP_SELECT_FLAG_GREEN;

            switch (_rideConstructionState)
            {
                case RideConstructionState::State0:
                    trackDirection = _currentTrackPieceDirection;
                    trackType = TrackElemType::Flat;
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
                        trackType = OpenRCT2::TrackElemType::Flat;
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

        void SelectMapTiles(OpenRCT2::TrackElemType trackType, int32_t trackDirection, const CoordsXY& tileCoords)
        {
            // If the scenery tool is active, we do not display our tiles as it
            // will conflict with larger scenery objects selecting tiles
            if (isToolActive(WindowClass::Scenery))
            {
                return;
            }

            const auto& ted = GetTrackElementDescriptor(trackType);
            trackDirection &= 3;
            gMapSelectionTiles.clear();
            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                CoordsXY offsets = { ted.sequences[i].clearance.x, ted.sequences[i].clearance.y };
                CoordsXY currentTileCoords = tileCoords + offsets.Rotate(trackDirection);

                gMapSelectionTiles.push_back(currentTileCoords);
            }
        }

    private:
        void Construct()
        {
            RideId rideIndex;
            OpenRCT2::TrackElemType trackType;
            int32_t trackDirection, properties;
            SelectedLiftAndInverted liftHillAndAlternativeState{};
            CoordsXYZ trackPos{};

            _currentTrackPrice = kMoney64Undefined;
            _trackPlaceCost = kMoney64Undefined;
            _trackPlaceErrorMessage = kStringIdNone;
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
                _trackPlaceErrorMessage = kStringIdNone;
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
            OpenRCT2::TrackElemType type = _currentTrackPieceType;
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
                newCoords->z = (tileElement->GetBaseZ()) - ted.sequences[0].clearance.z;
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
            if (_rideConstructionState == RideConstructionState::Front && !GetGameState().Cheats.enableChainLiftOnAllTrack)
            {
                switch (slope)
                {
                    case TrackPitch::None:
                    case TrackPitch::Up25:
                    case TrackPitch::Up60:
                        break;
                    default:
                        _currentTrackHasLiftHill = false;
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
            auto& elements = _specialElementDropdownState.Elements;

            int32_t defaultIndex = -1;
            int32_t i = 0;
            for (auto& element : elements)
            {
                TrackElemType trackPiece = element.TrackType;

                // Separate elements logically
                if (trackPiece == TrackElemType::None)
                {
                    gDropdownItems[i++].Format = kStringIdEmpty;
                    continue;
                }

                const auto& ted = GetTrackElementDescriptor(trackPiece);
                StringId trackPieceStringId = ted.description;

                // TODO: this should probably be done elsewhere
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
                    defaultIndex = i;

                i++;
            }

            // Tune dropdown to the elements it contains
            auto ddWidth = widget->width();
            auto targetColumnSize = _specialElementDropdownState.PreferredNumRows;
            if (targetColumnSize < _specialElementDropdownState.Elements.size())
                ddWidth -= 30;

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 0,
                elements.size(), ddWidth, targetColumnSize);

            for (size_t j = 0; j < elements.size(); j++)
            {
                Dropdown::SetDisabled(static_cast<int32_t>(j), _specialElementDropdownState.Elements[j].Disabled);
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

                auto* windowMgr = GetWindowManager();

                auto currentRide = GetRide(gRideEntranceExitPlaceRideIndex);
                if (currentRide != nullptr && RideAreAllPossibleEntrancesAndExitsBuilt(*currentRide).Successful)
                {
                    ToolCancel();
                    if (!currentRide->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
                    {
                        windowMgr->CloseByClass(WindowClass::RideConstruction);
                    }
                }
                else
                {
                    gRideEntranceExitPlaceType = gRideEntranceExitPlaceType ^ 1;
                    windowMgr->InvalidateByClass(WindowClass::RideConstruction);

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
            DrawPixelInfo& dpi, RideId rideIndex, OpenRCT2::TrackElemType trackType, int32_t trackDirection,
            SelectedLiftAndInverted liftHillAndInvertedState, int32_t widgetWidth, int32_t widgetHeight)
        {
            auto currentRide = GetRide(rideIndex);
            if (currentRide == nullptr)
            {
                return;
            }

            const auto& ted = GetTrackElementDescriptor(trackType);
            const auto trackBlock = ted.sequences[ted.numSequences - 1].clearance;
            CoordsXYZ mapCoords{ trackBlock.x, trackBlock.y, trackBlock.z };
            if (trackBlock.flags & RCT_PREVIEW_TRACK_FLAG_1)
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
            DrawPixelInfo& dpi, RideId rideIndex, OpenRCT2::TrackElemType trackType, int32_t trackDirection,
            SelectedLiftAndInverted liftHillAndInvertedState, const CoordsXY& originCoords, int32_t originZ)
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
            tempTrackTileElement.AsTrack()->SetHasChain(liftHillAndInvertedState.has(LiftHillAndInverted::liftHill));
            tempTrackTileElement.SetLastForTile(true);
            tempTrackTileElement.AsTrack()->SetTrackType(trackType);
            tempTrackTileElement.AsTrack()->SetRideType(currentRide->type);
            tempTrackTileElement.AsTrack()->SetHasCableLift(false);
            tempTrackTileElement.AsTrack()->SetInverted(liftHillAndInvertedState.has(LiftHillAndInverted::inverted));
            tempTrackTileElement.AsTrack()->SetColourScheme(_currentColourScheme);
            // Skipping seat rotation, should not be necessary for a temporary piece.
            tempTrackTileElement.AsTrack()->SetRideIndex(rideIndex);

            const auto& ted = GetTrackElementDescriptor(trackType);
            const auto* rideEntry = currentRide->GetRideEntry();
            auto clearanceHeight = (rideEntry != nullptr) ? rideEntry->Clearance
                                                          : currentRide->GetRideTypeDescriptor().Heights.ClearanceHeight;

            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                const auto& trackBlock = ted.sequences[i].clearance;

                auto quarterTile = trackBlock.quarterTile.Rotate(trackDirection);
                CoordsXY offsets = { trackBlock.x, trackBlock.y };
                CoordsXY coords = originCoords + offsets.Rotate(trackDirection);

                int32_t baseZ = originZ + trackBlock.z;
                int32_t clearanceZ = trackBlock.clearanceZ + clearanceHeight + baseZ + (4 * kCoordsZStep);

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
                tempTrackTileElement.AsTrack()->SetSequenceIndex(i);

                // Draw this map tile
                TileElementPaintSetup(*session, coords, true);

                // Restore map elements
                MapSetTileElement(centreTileCoords, backupTileElementArrays[0]);
                MapSetTileElement(eastTileCoords, backupTileElementArrays[1]);
                MapSetTileElement(westTileCoords, backupTileElementArrays[2]);
                MapSetTileElement(northTileCoords, backupTileElementArrays[3]);
                MapSetTileElement(southTileCoords, backupTileElementArrays[4]);
            }

            gameState.MapSize = preserveMapSize;

            PaintSessionArrange(*session);
            PaintDrawStructs(*session);
            PaintSessionFree(session);
        }
    };

    static void WindowRideConstructionUpdateDisabledPieces(ride_type_t rideType)
    {
        RideTrackGroups disabledGroups{};
        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (rtd.HasFlag(RtdFlag::hasTrack))
        {
            // Set all pieces as “disabled”. When looping over the ride entries,
            // pieces will be re-enabled as soon as a single entry supports it.
            disabledGroups.flip();

            auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
            auto& rideEntries = objManager.GetAllRideEntries(rideType);
            // If there are no vehicles for this ride type, enable all the track pieces.
            if (rideEntries.size() == 0)
            {
                disabledGroups.reset();
            }

            for (auto rideEntryIndex : rideEntries)
            {
                const auto* currentRideEntry = GetRideEntryByIndex(rideEntryIndex);
                if (currentRideEntry == nullptr)
                    continue;

                // Non-default vehicle visuals do not use this system, so we have to assume it supports all the track pieces.
                auto& firstCar = currentRideEntry->Cars[0];
                if (firstCar.PaintStyle != VEHICLE_VISUAL_DEFAULT || (firstCar.flags & CAR_ENTRY_FLAG_CHAIRLIFT)
                    || (firstCar.flags & CAR_ENTRY_FLAG_SLIDE_SWING))
                {
                    disabledGroups.reset();
                    break;
                }

                // Any pieces that this ride entry supports must be taken out of the array.
                auto supportedPieces = RideEntryGetSupportedTrackPieces(*currentRideEntry);
                disabledGroups &= supportedPieces.flip();
            }
        }

        UpdateDisabledRideGroups(disabledGroups);
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

        auto* windowMgr = GetWindowManager();

        const auto& rtd = currentRide->GetRideTypeDescriptor();
        switch (rtd.ConstructionWindowContext)
        {
            case RideConstructionWindowContext::Maze:
                return ContextOpenWindowView(WV_MAZE_CONSTRUCTION);
            case RideConstructionWindowContext::Default:
                return windowMgr->Create<RideConstructionWindow>(
                    WindowClass::RideConstruction, ScreenCoordsXY(0, 29), WW, WH, WF_NO_AUTO_CLOSE);
        }
        return windowMgr->Create<RideConstructionWindow>(
            WindowClass::RideConstruction, ScreenCoordsXY(0, 29), WW, WH, WF_NO_AUTO_CLOSE);
    }

    static void CloseConstructWindowOnCompletion(const Ride& ride)
    {
        if (_rideConstructionState == RideConstructionState::State0)
        {
            auto* windowMgr = GetWindowManager();
            auto w = windowMgr->FindByClass(WindowClass::RideConstruction);
            if (w != nullptr)
            {
                if (RideAreAllPossibleEntrancesAndExitsBuilt(ride).Successful)
                {
                    windowMgr->Close(*w);
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
        auto* windowMgr = GetWindowManager();
        auto w = windowMgr->FindByClass(WindowClass::RideConstruction);
        auto ride = GetRide(_currentRideIndex);
        if (w == nullptr || ride == nullptr)
        {
            return;
        }

        if (_rideConstructionState == RideConstructionState::State0)
        {
            w = windowMgr->FindByClass(WindowClass::RideConstruction);
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

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Error);
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

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::Error);
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
                if (info.interactionType != ViewportInteractionItem::None)
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
                _trackPlaceShiftZ = floor2(_trackPlaceShiftZ, 8);

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
                auto mapZ = floor2(surfaceElement->GetBaseZ(), 16);
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
            !currentRide || currentRide->GetRideTypeDescriptor().specialType == RtdSpecialType::maze)
        {
            return;
        }
        auto* windowMgr = GetWindowManager();
        auto window = static_cast<RideConstructionWindow*>(windowMgr->FindByClass(WindowClass::RideConstruction));
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
                _currentColourScheme = static_cast<RideColourScheme>(tileElement->AsTrack()->GetColourScheme());
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
        UpdateEnabledRideGroups(trackDrawerDescriptor);
    }

    /**
     *
     *  rct2: 0x006C94D8
     */
    void UpdateGhostTrackAndArrow()
    {
        RideId rideIndex;
        int32_t direction;
        OpenRCT2::TrackElemType type;
        SelectedLiftAndInverted liftHillAndAlternativeState{};
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

                        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
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
                _rideConstructionNextArrowPulse = curTime + kArrowPulseDuration;

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
                _rideConstructionNextArrowPulse = curTime + kArrowPulseDuration;

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
                _rideConstructionNextArrowPulse = curTime + kArrowPulseDuration;

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
        OpenRCT2::TrackElemType trackType;
        int32_t trackDirection;
        SelectedLiftAndInverted liftHillAndAlternativeState{};
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
        if (rtd.specialType != RtdSpecialType::maze)
        {
            auto* windowMgr = GetWindowManager();
            auto window = static_cast<RideConstructionWindow*>(windowMgr->FindByClass(WindowClass::RideConstruction));
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
        int32_t bx = 0;
        for (uint8_t i = 0; i < ted.numSequences; i++)
        {
            bx = std::min<int32_t>(bx, ted.sequences[i].clearance.z);
        }
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

        if (rtd.specialType == RtdSpecialType::maze)
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
        OpenRCT2::TrackElemType trackType;
        int32_t z, highestZ;

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
            int32_t bx = 0;
            for (uint8_t i = 0; i < ted.numSequences; i++)
            {
                bx = std::min<int32_t>(bx, ted.sequences[i].clearance.z);
            }
            z -= bx;

            // FIX not sure exactly why it starts trial and error place from a lower Z, but it causes issues with disable
            // clearance
            if (!GetGameState().Cheats.disableClearanceChecks && z > kMinimumLandZ)
            {
                z -= kLandHeightStep;
            }
        }
        else
        {
            z = _trackPlaceZ;
        }

        // search for z value to build at, up to max ride height
        int numAttempts = (z <= MAX_TRACK_HEIGHT ? ((MAX_TRACK_HEIGHT - z) / kCoordsZStep + 1) : 2);

        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
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

                auto* windowMgr = GetWindowManager();
                w = windowMgr->FindByClass(WindowClass::RideConstruction);
                if (w == nullptr)
                    break;

                gDisableErrorWindowSound = true;

                auto gameAction = MazeSetTrackAction(
                    CoordsXYZD{ _currentTrackBegin, 0 }, true, _currentRideIndex, GC_SET_MAZE_TRACK_BUILD);
                auto mazeSetTrackResult = GameActions::Execute(&gameAction);
                if (mazeSetTrackResult.Error == GameActions::Status::Ok)
                {
                    _trackPlaceCost = mazeSetTrackResult.Cost;
                    _trackPlaceErrorMessage = kStringIdNone;
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
                        Audio::Play(OpenRCT2::Audio::SoundId::Error, 0, state->position.x);
                        w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
                    windowMgr->CloseByClass(WindowClass::Error);
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

            auto* windowMgr = GetWindowManager();
            w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
                    auto savedCurrentTrackAlternative = _currentTrackAlternative;
                    auto savedCurrentTrackLiftHill = _currentTrackHasLiftHill;

                    RideInitialiseConstructionWindow(*ride);

                    _currentTrackPieceDirection = saveTrackDirection;
                    _currentlySelectedTrack = saveCurrentTrackCurve;
                    _previousTrackPitchEnd = savePreviousTrackPitchEnd;
                    _currentTrackPitchEnd = saveCurrentTrackPitchEnd;
                    _previousTrackRollEnd = savePreviousTrackRollEnd;
                    _currentTrackRollEnd = saveCurrentTrackRollEnd;
                    _currentTrackAlternative = savedCurrentTrackAlternative;
                    _currentTrackHasLiftHill = savedCurrentTrackLiftHill;

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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
            && _currentTrackHasLiftHill)
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackPitchEnd)
        {
            case TrackPitch::Down60:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_VERTICAL)
                    && w->widgets[WIDX_SLOPE_DOWN_VERTICAL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_DOWN_VERTICAL);
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
                else if (IsTrackEnabled(TrackGroup::slopeVertical))
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
                else if (IsTrackEnabled(TrackGroup::slopeVertical))
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
                    && w->widgets[WIDX_SLOPE_UP_VERTICAL].type != WindowWidgetType::Empty)
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
        {
            return;
        }

        switch (_currentTrackPitchEnd)
        {
            case TrackPitch::Up60:
                if (IsTrackEnabled(TrackGroup::slopeVertical) && !WidgetIsDisabled(*w, WIDX_SLOPE_UP_VERTICAL)
                    && w->widgets[WIDX_SLOPE_UP_VERTICAL].type != WindowWidgetType::Empty)
                {
                    w->OnMouseDown(WIDX_SLOPE_UP_VERTICAL);
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
                else if (IsTrackEnabled(TrackGroup::slopeVertical))
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
                else if (IsTrackEnabled(TrackGroup::slopeVertical))
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_CHAIN_LIFT)
            || w->widgets[WIDX_CHAIN_LIFT].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_CHAIN_LIFT);
    }

    void WindowRideConstructionKeyboardShortcutBankLeft()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
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
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_PREVIOUS_SECTION)
            || w->widgets[WIDX_PREVIOUS_SECTION].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_PREVIOUS_SECTION);
    }

    void WindowRideConstructionKeyboardShortcutNextTrack()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_NEXT_SECTION)
            || w->widgets[WIDX_NEXT_SECTION].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_NEXT_SECTION);
    }

    void WindowRideConstructionKeyboardShortcutBuildCurrent()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_CONSTRUCT);
    }

    void WindowRideConstructionKeyboardShortcutDemolishCurrent()
    {
        auto* windowMgr = GetWindowManager();
        WindowBase* w = windowMgr->FindByClass(WindowClass::RideConstruction);
        if (w == nullptr || WidgetIsDisabled(*w, WIDX_DEMOLISH) || w->widgets[WIDX_DEMOLISH].type == WindowWidgetType::Empty)
        {
            return;
        }

        w->OnMouseDown(WIDX_DEMOLISH);
    }

    static void WindowRideConstructionMouseUpDemolishNextPiece(const CoordsXYZD& piecePos, OpenRCT2::TrackElemType type)
    {
        if (_gotoStartPlacementMode)
        {
            _currentTrackBegin.z = floor2(piecePos.z, kCoordsZStep);
            _rideConstructionState = RideConstructionState::Front;
            _currentTrackSelectionFlags = 0;
            _currentTrackPieceDirection = piecePos.direction & 3;
            auto savedSelectedTrack = _currentlySelectedTrack;
            auto savedPreviousTrackPitchEnd = _previousTrackPitchEnd;
            auto savedCurrentTrackPitchEnd = _currentTrackPitchEnd;
            auto savedPreviousTrackRollEnd = _previousTrackRollEnd;
            auto savedCurrentTrackRollEnd = _currentTrackRollEnd;
            auto savedCurrentTrackAlternative = _currentTrackAlternative;
            auto savedCurrentTrackLiftHill = _currentTrackHasLiftHill;
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
                    _currentTrackHasLiftHill = savedCurrentTrackLiftHill;
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

    /**
     *
     *  rct2: 0x006CA162
     */
    money64 PlaceProvisionalTrackPiece(
        RideId rideIndex, OpenRCT2::TrackElemType trackType, int32_t trackDirection,
        SelectedLiftAndInverted liftHillAndAlternativeState, const CoordsXYZ& trackPos)
    {
        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
            return kMoney64Undefined;

        RideConstructionRemoveGhosts();
        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
        {
            int32_t flags = GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST;
            auto gameAction = MazeSetTrackAction(CoordsXYZD{ trackPos, 0 }, true, rideIndex, GC_SET_MAZE_TRACK_BUILD);
            gameAction.SetFlags(flags);
            auto result = GameActions::Execute(&gameAction);

            if (result.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            _unkF440C5 = { trackPos, static_cast<Direction>(trackDirection) };
            _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
            ViewportSetVisibility(ViewportVisibility::UndergroundViewOff);
            if (_currentTrackPitchEnd != TrackPitch::None)
                ViewportSetVisibility(ViewportVisibility::TrackHeights);

            return result.Cost;
        }

        auto trackPlaceAction = TrackPlaceAction(
            rideIndex, trackType, ride->type, { trackPos, static_cast<uint8_t>(trackDirection) }, 0, 0, 0,
            liftHillAndAlternativeState, false);
        trackPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        // This command must not be sent over the network
        auto res = GameActions::Execute(&trackPlaceAction);
        if (res.Error != GameActions::Status::Ok)
            return kMoney64Undefined;

        int16_t zBegin{}, zEnd{};
        const auto& ted = GetTrackElementDescriptor(trackType);
        const TrackCoordinates& coords = ted.coordinates;
        if (ride->GetRideTypeDescriptor().HasFlag(RtdFlag::hasTrack))
        {
            zBegin = coords.zBegin;
            zEnd = coords.zEnd;
        }
        else
        {
            zEnd = zBegin = coords.zBegin;
        }

        _unkF440C5 = { trackPos.x, trackPos.y, trackPos.z + zBegin, static_cast<Direction>(trackDirection) };
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;

        const auto resultData = res.GetData<TrackPlaceActionResult>();
        const auto visiblity = (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND) ? ViewportVisibility::UndergroundViewOn
                                                                                 : ViewportVisibility::UndergroundViewOff;
        ViewportSetVisibility(visiblity);
        if (_currentTrackPitchEnd != TrackPitch::None)
            ViewportSetVisibility(ViewportVisibility::TrackHeights);

        // Invalidate previous track piece (we may not be changing height!)
        VirtualFloorInvalidate();

        if (!(gMapSelectFlags & MAP_SELECT_FLAG_ENABLE))
        {
            // Set height to where the next track piece would begin
            VirtualFloorSetHeight(trackPos.z - zBegin + zEnd);
        }

        return res.Cost;
    }

    static std::pair<bool, OpenRCT2::TrackElemType> WindowRideConstructionUpdateStateGetTrackElement()
    {
        auto startSlope = _previousTrackPitchEnd;
        auto endSlope = _currentTrackPitchEnd;
        auto startBank = _previousTrackRollEnd;
        auto endBank = _currentTrackRollEnd;

        if (_rideConstructionState == RideConstructionState::Back)
        {
            startSlope = _currentTrackPitchEnd;
            endSlope = _previousTrackPitchEnd;
            startBank = _currentTrackRollEnd;
            endBank = _previousTrackRollEnd;
        }

        auto selectedTrack = _currentlySelectedTrack;
        if (selectedTrack == TrackElemType::None)
        {
            return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
        }

        bool startsDiagonal = (_currentTrackPieceDirection & (1 << 2)) != 0;
        if (selectedTrack == TrackCurve::LeftLarge || selectedTrack == TrackCurve::RightLarge)
        {
            if (_rideConstructionState == RideConstructionState::Back)
            {
                startsDiagonal = !startsDiagonal;
            }
        }

        if (!selectedTrack.isTrackType)
        {
            auto trackPiece = GetTrackTypeFromCurve(
                selectedTrack.curve, startsDiagonal, startSlope, endSlope, startBank, endBank);
            if (trackPiece != TrackElemType::None)
                return std::make_pair(true, trackPiece);
            else
                return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
        }

        auto asTrackType = selectedTrack.trackType;
        switch (asTrackType)
        {
            case TrackElemType::EndStation:
            case TrackElemType::SBendLeft:
            case TrackElemType::SBendRight:
                if (startSlope != TrackPitch::None || endSlope != TrackPitch::None)
                {
                    return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
                }

                if (startBank != TrackRoll::None || endBank != TrackRoll::None)
                {
                    return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
                }

                return std::make_pair(true, asTrackType);

            case TrackElemType::LeftVerticalLoop:
            case TrackElemType::RightVerticalLoop:
                if (startBank != TrackRoll::None || endBank != TrackRoll::None)
                {
                    return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
                }

                if (_rideConstructionState == RideConstructionState::Back)
                {
                    if (endSlope != TrackPitch::Down25)
                    {
                        return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
                    }
                }
                else
                {
                    if (startSlope != TrackPitch::Up25)
                    {
                        return std::make_pair(false, OpenRCT2::TrackElemType::Flat);
                    }
                }

                return std::make_pair(true, asTrackType);

            default:
                return std::make_pair(true, asTrackType);
        }
    }

    /**
     * rct2: 0x006CA2DF
     *
     * @param[out] _trackType (dh)
     * @param[out] _trackDirection (bh)
     * @param[out] _rideIndex (dl)
     * @param[out] _liftHillAndInvertedState (liftHillAndInvertedState)
     * @param[out] _x (ax)
     * @param[out] _y (cx)
     * @param[out] _z (di)
     * @param[out] _properties (edirs16)
     * @return (CF)
     */
    static bool WindowRideConstructionUpdateState(
        OpenRCT2::TrackElemType* _trackType, int32_t* _trackDirection, RideId* _rideIndex,
        SelectedLiftAndInverted* _liftHillAndInvertedState, CoordsXYZ* _trackPos, int32_t* _properties)
    {
        RideId rideIndex;
        uint8_t trackDirection;
        uint16_t x, y, properties;
        SelectedLiftAndInverted liftHillAndInvertedState{};

        auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES);
        ContextBroadcastIntent(&intent);
        auto updated_element = WindowRideConstructionUpdateStateGetTrackElement();
        if (!std::get<0>(updated_element))
        {
            return true;
        }

        OpenRCT2::TrackElemType trackType = std::get<1>(updated_element);
        rideIndex = _currentRideIndex;
        if (_currentTrackHasLiftHill)
        {
            liftHillAndInvertedState.set(LiftHillAndInverted::liftHill);
        }

        if (_currentTrackAlternative.has(AlternativeTrackFlag::inverted))
        {
            liftHillAndInvertedState.set(LiftHillAndInverted::inverted);
        }

        auto ride = GetRide(rideIndex);
        if (ride == nullptr)
            return true;

        if (IsTrackEnabled(TrackGroup::slopeSteepLong))
        {
            switch (trackType)
            {
                case TrackElemType::FlatToUp60:
                    trackType = TrackElemType::FlatToUp60LongBase;
                    break;

                case TrackElemType::Up60ToFlat:
                    trackType = TrackElemType::Up60ToFlatLongBase;
                    break;

                case TrackElemType::FlatToDown60:
                    trackType = TrackElemType::FlatToDown60LongBase;
                    break;

                case TrackElemType::Down60ToFlat:
                    trackType = TrackElemType::Down60ToFlatLongBase;
                    break;

                default:
                    break;
            }
        }

        if (IsTrackEnabled(TrackGroup::diagSlopeSteepLong))
        {
            switch (trackType)
            {
                case TrackElemType::DiagFlatToUp60:
                    trackType = TrackElemType::DiagFlatToUp60LongBase;
                    break;

                case TrackElemType::DiagUp60ToFlat:
                    trackType = TrackElemType::DiagUp60ToFlatLongBase;
                    break;

                case TrackElemType::DiagFlatToDown60:
                    trackType = TrackElemType::DiagFlatToDown60LongBase;
                    break;

                case TrackElemType::DiagDown60ToFlat:
                    trackType = TrackElemType::DiagDown60ToFlatLongBase;
                    break;

                default:
                    break;
            }
        }

        const auto& rtd = ride->GetRideTypeDescriptor();
        const auto trackDrawerDescriptor = getCurrentTrackDrawerDescriptor(rtd);
        if (trackDrawerDescriptor.HasCoveredPieces() && _currentTrackAlternative.has(AlternativeTrackFlag::alternativePieces))
        {
            auto availableGroups = trackDrawerDescriptor.Covered.enabledTrackGroups;
            const auto& ted = GetTrackElementDescriptor(trackType);

            auto coveredVariant = ted.alternativeType;
            // this method limits the track element types that can be used
            if (coveredVariant != TrackElemType::None && (availableGroups.get(EnumValue(ted.definition.group))))
            {
                trackType = coveredVariant;
                if (!GetGameState().Cheats.enableChainLiftOnAllTrack)
                    liftHillAndInvertedState.unset(LiftHillAndInverted::liftHill);
            }
        }

        const auto& ted = GetTrackElementDescriptor(trackType);
        const TrackCoordinates& trackCoordinates = ted.coordinates;

        x = _currentTrackBegin.x;
        y = _currentTrackBegin.y;
        auto z = _currentTrackBegin.z;
        if (_rideConstructionState == RideConstructionState::Back)
        {
            z -= trackCoordinates.zEnd;
            trackDirection = _currentTrackPieceDirection ^ 0x02;
            trackDirection -= trackCoordinates.rotationEnd;
            trackDirection += trackCoordinates.rotationBegin;
            trackDirection &= 0x03;

            if (trackCoordinates.rotationBegin & (1 << 2))
            {
                trackDirection |= 0x04;
            }

            CoordsXY offsets = { trackCoordinates.x, trackCoordinates.y };
            CoordsXY coords = { x, y };
            coords += offsets.Rotate(DirectionReverse(trackDirection));
            x = static_cast<uint16_t>(coords.x);
            y = static_cast<uint16_t>(coords.y);
        }
        else
        {
            z -= trackCoordinates.zBegin;
            trackDirection = _currentTrackPieceDirection;
        }

        bool turnOffLiftHill = false;
        if (!IsTrackEnabled(TrackGroup::liftHillCurve))
        {
            if (ted.flags & TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT)
            {
                turnOffLiftHill = true;
            }
        }

        if (!(ted.flags & TRACK_ELEM_FLAG_ALLOW_LIFT_HILL))
        {
            turnOffLiftHill = true;
        }

        if (turnOffLiftHill && !GetGameState().Cheats.enableChainLiftOnAllTrack)
        {
            liftHillAndInvertedState.unset(LiftHillAndInverted::liftHill);
            _currentTrackHasLiftHill = false;

            if (trackType == TrackElemType::LeftCurvedLiftHill || trackType == TrackElemType::RightCurvedLiftHill)
            {
                liftHillAndInvertedState.set(LiftHillAndInverted::liftHill);
            }
        }

        if (TrackTypeHasSpeedSetting(trackType))
        {
            properties = _currentBrakeSpeed;
        }
        else
        {
            properties = _currentSeatRotationAngle << 12;
        }

        properties |= static_cast<uint8_t>(_currentColourScheme) << 8;

        if (_trackType != nullptr)
            *_trackType = trackType;
        if (_trackDirection != nullptr)
            *_trackDirection = trackDirection;
        if (_rideIndex != nullptr)
            *_rideIndex = rideIndex;
        if (_liftHillAndInvertedState != nullptr)
            *_liftHillAndInvertedState = liftHillAndInvertedState;
        if (_trackPos != nullptr)
            *_trackPos = { x, y, z };
        if (_properties != nullptr)
            *_properties = properties;

        return false;
    }

    void RideRestoreProvisionalTrackPiece()
    {
        if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK)
        {
            RideId rideIndex;
            int32_t direction;
            OpenRCT2::TrackElemType type;
            SelectedLiftAndInverted liftHillAndAlternativeState{};
            CoordsXYZ trackPos;
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
            }
        }
    }

    void RideRemoveProvisionalTrackPiece()
    {
        auto rideIndex = _currentRideIndex;
        auto ride = GetRide(rideIndex);
        if (ride == nullptr || !(_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_TRACK))
        {
            return;
        }

        int32_t x = _unkF440C5.x;
        int32_t y = _unkF440C5.y;
        int32_t z = _unkF440C5.z;

        const auto& rtd = ride->GetRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
        {
            const int32_t flags = GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST;
            const CoordsXYZD quadrants[kNumOrthogonalDirections] = {
                { x, y, z, 0 },
                { x, y + 16, z, 1 },
                { x + 16, y + 16, z, 2 },
                { x + 16, y, z, 3 },
            };
            for (const auto& quadrant : quadrants)
            {
                auto gameAction = MazeSetTrackAction(quadrant, false, rideIndex, GC_SET_MAZE_TRACK_FILL);
                gameAction.SetFlags(flags);
                auto res = GameActions::Execute(&gameAction);
            }
        }
        else
        {
            int32_t direction = _unkF440C5.direction;
            if (!(direction & 4))
            {
                x -= CoordsDirectionDelta[direction].x;
                y -= CoordsDirectionDelta[direction].y;
            }
            CoordsXYE next_track;
            if (TrackBlockGetNextFromZero({ x, y, z }, *ride, direction, &next_track, &z, &direction, true))
            {
                auto trackType = next_track.element->AsTrack()->GetTrackType();
                int32_t trackSequence = next_track.element->AsTrack()->GetSequenceIndex();
                auto trackRemoveAction = TrackRemoveAction{
                    trackType, trackSequence, { next_track.x, next_track.y, z, static_cast<Direction>(direction) }
                };
                trackRemoveAction.SetFlags(
                    GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
                GameActions::Execute(&trackRemoveAction);
            }
        }
    }
} // namespace OpenRCT2::Ui::Windows
