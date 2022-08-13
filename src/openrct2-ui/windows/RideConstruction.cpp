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
#include <openrct2/actions/MazeSetTrackAction.h>
#include <openrct2/actions/RideDemolishAction.h>
#include <openrct2/actions/RideEntranceExitPlaceAction.h>
#include <openrct2/actions/RideSetStatusAction.h>
#include <openrct2/actions/TrackPlaceAction.h>
#include <openrct2/actions/TrackRemoveAction.h>
#include <openrct2/actions/TrackSetBrakeSpeedAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
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
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_RIDE_CONSTRUCTION_WINDOW_TITLE;
static constexpr const int32_t WH = 394;
static constexpr const int32_t WW = 166;
static constexpr const uint16_t ARROW_PULSE_DURATION = 200;

using namespace OpenRCT2::TrackMetaData;

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

// clang-format off
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
    MakeWidget        ({  6,  55}, {154,  14}, WindowWidgetType::Button,   WindowColour::Secondary, STR_YELLOW_STRING,                       STR_RIDE_CONSTRUCTION_OTHER_TRACK_CONFIGURATIONS_TIP),
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
    WIDGETS_END,
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
static StringId _trackPlaceErrorMessage;
static bool _autoRotatingShop;

static constexpr const StringId RideConstructionSeatAngleRotationStrings[] = {
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_180, STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_135,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_90,  STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_NEG_45,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_0,       STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_45,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_90,      STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_135,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_180,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_225,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_270,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_315,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_360,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_405,
    STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_450,     STR_RIDE_CONSTRUCTION_SEAT_ROTATION_ANGLE_495,
};

static void window_ride_construction_mouseup_demolish_next_piece(const CoordsXYZD& piecePos, int32_t type);

static int32_t RideGetAlternativeType(Ride* ride)
{
    return (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE) ? ride->GetRideTypeDescriptor().AlternateType
                                                                         : ride->type;
}

/* move to ride.c */
static void CloseRideWindowForConstruction(RideId rideId)
{
    rct_window* w = window_find_by_number(WC_RIDE, rideId.ToUnderlying());
    if (w != nullptr && w->page == 1)
        window_close(*w);
}

static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActions::Result* result);
static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActions::Result* result);
static void CloseConstructWindowOnCompletion(Ride* ride);

class RideConstructionWindow final : public Window
{
private:
    uint8_t _currentlyShowingBrakeOrBoosterSpeed{};
    uint64_t _currentDisabledSpecialTrackPieces{};
    track_type_t _currentPossibleRideConfigurations[64]{};
    uint16_t _numCurrentPossibleRideConfigurations{};
    uint16_t _numCurrentPossibleSpecialTrackPieces{};
    bool _autoOpeningShop{};

public:
    void OnOpen() override
    {
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        widgets = window_ride_construction_widgets;
        number = _currentRideIndex.ToUnderlying();

        InitScrollWidgets();

        colours[0] = COLOUR_DARK_BROWN;
        colours[1] = COLOUR_DARK_BROWN;
        colours[2] = COLOUR_DARK_BROWN;

        window_push_others_right(*this);
        show_gridlines();

        _currentTrackPrice = MONEY32_UNDEFINED;
        _currentBrakeSpeed2 = 8;
        _currentSeatRotationAngle = 4;

        _currentTrackCurve = currentRide->GetRideTypeDescriptor().StartTrackPiece | RideConstructionSpecialPieceSelected;
        _currentTrackSlopeEnd = 0;
        _currentTrackBankEnd = 0;
        _currentTrackLiftHill = 0;
        _currentTrackAlternative = RIDE_TYPE_NO_ALTERNATIVES;

        if (currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_START_CONSTRUCTION_INVERTED))
            _currentTrackAlternative |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;

        _previousTrackBankEnd = 0;
        _previousTrackSlopeEnd = 0;

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
        ride_construction_invalidate_current_track();
        viewport_set_visibility(0);

        map_invalidate_map_selection_tiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        // In order to cancel the yellow arrow correctly the
        // selection tool should be cancelled. Don't do a tool cancel if
        // another window has already taken control of tool.
        if (classification == gCurrentToolWidget.window_classification && number == gCurrentToolWidget.window_number)
            tool_cancel();

        hide_gridlines();

        // If we demolish a currentRide all windows will be closed including the construction window,
        // the currentRide at this point is already gone.
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        if (ride_try_get_origin_element(currentRide, nullptr))
        {
            // Auto open shops if required.
            if (currentRide->mode == RideMode::ShopStall && gConfigGeneral.auto_open_shops)
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
            auto intent = Intent(WC_RIDE);
            intent.putExtra(INTENT_EXTRA_RIDE_ID, currentRide->id.ToUnderlying());
            context_open_intent(&intent);
        }
        else
        {
            int32_t previousPauseState = gGamePaused;
            gGamePaused = 0;
            auto gameAction = RideDemolishAction(currentRide->id, RIDE_MODIFY_DEMOLISH);
            GameActions::Execute(&gameAction);
            gGamePaused = previousPauseState;
        }
    }

    void OnResize() override
    {
        WindowRideConstructionUpdateEnabledTrackPieces();

        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        int32_t rideType = RideGetAlternativeType(currentRide);

        uint64_t disabledWidgets = 0;

        if (_currentTrackCurve & RideConstructionSpecialPieceSelected)
        {
            disabledWidgets |= (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_BANKING_GROUPBOX) | (1ULL << WIDX_SLOPE_DOWN_STEEP)
                | (1ULL << WIDX_SLOPE_DOWN) | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP)
                | (1ULL << WIDX_CHAIN_LIFT) | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT)
                | (1ULL << WIDX_BANK_RIGHT);
        }

        // Disable large curves if the start or end of the track is sloped.
        if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE)
        {
            disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_LARGE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
        }
        if (IsTrackEnabled(TRACK_SLOPE_CURVE) && IsTrackEnabled(TRACK_CURVE_VERY_SMALL))
        {
            // Disable small curves if the start or end of the track is sloped.
            if (_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _currentTrackSlopeEnd != TRACK_SLOPE_NONE)
            {
                disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE)
                    | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
            }
        }
        if (!IsTrackEnabled(TRACK_SLOPE_CURVE))
        {
            if (IsTrackEnabled(TRACK_CURVE_VERTICAL))
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
        if (!IsTrackEnabled(TRACK_FLAT_ROLL_BANKING))
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
        if (!IsTrackEnabled(TRACK_SLOPE) && !IsTrackEnabled(TRACK_SLOPE_STEEP_DOWN) && !IsTrackEnabled(TRACK_SLOPE_STEEP_UP))
        {
            if (!currentRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
            {
                // Disable all slopes
                disabledWidgets |= (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN)
                    | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            }
        }
        if (currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_UP_INCLINE_REQUIRES_LIFT)
            && !gCheatsEnableAllDrawableTrackPieces)
        {
            // Disable lift hill toggle and banking if current track piece is uphill
            if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_25 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_60
                || _currentTrackSlopeEnd == TRACK_SLOPE_UP_25 || _currentTrackSlopeEnd == TRACK_SLOPE_UP_60)
                disabledWidgets |= 1ULL << WIDX_CHAIN_LIFT | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_RIGHT);
            // Disable upward slope if current track piece is not flat
            if ((_previousTrackSlopeEnd != TRACK_SLOPE_NONE || _previousTrackBankEnd != TRACK_BANK_NONE)
                && !(_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED))
                disabledWidgets |= (1ULL << WIDX_SLOPE_UP);
        }
        if (_rideConstructionState == RideConstructionState::State0)
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
        if (!IsTrackEnabled(TRACK_SLOPE_ROLL_BANKING))
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
                    disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE)
                        | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
                    if (!IsTrackEnabled(TRACK_SLOPE_CURVE_STEEP))
                    {
                        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_SMALL);
                    }
                    break;
                case TRACK_SLOPE_UP_90:
                case TRACK_SLOPE_DOWN_90:
                    disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE)
                        | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
                    if (!IsTrackEnabled(TRACK_CURVE_VERTICAL))
                    {
                        disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_SMALL);
                    }
                    break;
            }
        }
        else
        {
            // Disable all curves
            disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
                | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
        }

        switch (_previousTrackSlopeEnd)
        {
            case TRACK_SLOPE_NONE:
                if (_currentTrackCurve != TRACK_CURVE_NONE
                    || (IsTrackEnabled(TRACK_SLOPE_STEEP_LONG) && TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
                {
                    disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP);
                }
                break;
            case TRACK_SLOPE_DOWN_25:
                disabledWidgets |= (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
                break;
            case TRACK_SLOPE_DOWN_60:
                disabledWidgets |= (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP);
                if (!IsTrackEnabled(TRACK_SLOPE_LONG)
                    && !(IsTrackEnabled(TRACK_SLOPE_STEEP_LONG) && !TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
                {
                    disabledWidgets |= (1ULL << WIDX_LEVEL);
                }
                break;
            case TRACK_SLOPE_UP_25:
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN);
                break;
            case TRACK_SLOPE_UP_60:
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN);
                if (!IsTrackEnabled(TRACK_SLOPE_LONG)
                    && !(IsTrackEnabled(TRACK_SLOPE_STEEP_LONG) && !TrackPieceDirectionIsDiagonal(_currentTrackPieceDirection)))
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
            if (!IsTrackEnabled(TRACK_SLOPE_LONG) && !IsTrackEnabled(TRACK_SLOPE_STEEP_LONG))
            {
                disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_UP_STEEP);
            }
        }
        if (IsTrackEnabled(TRACK_SLOPE_VERTICAL))
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
            if (IsTrackEnabled(TRACK_SLOPE_ROLL_BANKING))
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
            if (!IsTrackEnabled(TRACK_LIFT_HILL_CURVE))
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
        if (!IsTrackEnabled(TRACK_LIFT_HILL_STEEP))
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
                        || _rideConstructionState != RideConstructionState::Back || !IsTrackEnabled(TRACK_SLOPE_CURVE_BANKED))
                    {
                        disabledWidgets |= (1ULL << WIDX_LEVEL);
                    }
                }
                if (_currentTrackSlopeEnd == TRACK_SLOPE_DOWN_25)
                {
                    disabledWidgets |= (1ULL << WIDX_SLOPE_DOWN_STEEP);
                    if (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT
                        || _rideConstructionState != RideConstructionState::Front || !IsTrackEnabled(TRACK_SLOPE_CURVE_BANKED))
                    {
                        disabledWidgets |= (1ULL << WIDX_LEVEL);
                    }
                }
            }
            else if (IsTrackEnabled(TRACK_SLOPE_CURVE_BANKED))
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
            if (_currentTrackSlopeEnd != TRACK_SLOPE_NONE && !IsTrackEnabled(TRACK_LIFT_HILL_CURVE))
            {
                disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_LEFT_CURVE_LARGE)
                    | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_LARGE);
            }
            if (!IsTrackEnabled(TRACK_LIFT_HILL_STEEP))
            {
                if (widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
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
            if (currentRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
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
            if (currentRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
            {
                disabledWidgets |= (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                    | (1ULL << WIDX_LEFT_CURVE_SMALL) | (1ULL << WIDX_LEFT_CURVE);
            }
        }
        if (IsTrackEnabled(TRACK_HELIX_LARGE_UNBANKED))
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
            (IsTrackEnabled(TRACK_HELIX_SMALL)
             || (IsTrackEnabled(TRACK_HELIX_LARGE) && _currentTrackCurve != TRACK_CURVE_LEFT_SMALL
                 && _currentTrackCurve != TRACK_CURVE_RIGHT_SMALL))
            && (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT
                || _currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
            && (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd != TRACK_BANK_NONE))
        {
            if (_previousTrackSlopeEnd == _currentTrackSlopeEnd)
            {
                // Enable helix
                disabledWidgets &= ~(1ULL << WIDX_SLOPE_DOWN_STEEP);
                if (!currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_UP_INCLINE_REQUIRES_LIFT)
                    || gCheatsEnableAllDrawableTrackPieces)
                    disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP_STEEP);
            }
        }
        if (IsTrackEnabled(TRACK_SLOPE_CURVE_BANKED))
        {
            if (_rideConstructionState == RideConstructionState::Front)
            {
                if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL || _currentTrackCurve == TRACK_CURVE_RIGHT_SMALL)
                {
                    if (_currentTrackSlopeEnd == TRACK_SLOPE_NONE && _previousTrackBankEnd != TRACK_BANK_NONE
                        && (!currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_UP_INCLINE_REQUIRES_LIFT)
                            || gCheatsEnableAllDrawableTrackPieces))
                    {
                        disabledWidgets &= ~(1ULL << WIDX_SLOPE_UP);
                    }
                }
            }
            else if (_rideConstructionState == RideConstructionState::Back)
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
            disabledWidgets |= (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
                | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_RIGHT_CURVE) | (1ULL << WIDX_RIGHT_CURVE_SMALL)
                | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL);
        }
        if (_rideConstructionState == RideConstructionState::Front)
        {
            disabledWidgets |= (1ULL << WIDX_NEXT_SECTION);
            if (window_ride_construction_update_state(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr))
            {
                disabledWidgets |= (1ULL << WIDX_CONSTRUCT);
            }
        }
        else if (_rideConstructionState == RideConstructionState::Back)
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
        if (_rideConstructionState == RideConstructionState::EntranceExit
            || _rideConstructionState == RideConstructionState::Selected)
        {
            disabledWidgets |= (1ULL << WIDX_DIRECTION_GROUPBOX) | (1ULL << WIDX_SLOPE_GROUPBOX)
                | (1ULL << WIDX_BANKING_GROUPBOX) | (1ULL << WIDX_LEFT_CURVE_VERY_SMALL) | (1ULL << WIDX_LEFT_CURVE_SMALL)
                | (1ULL << WIDX_LEFT_CURVE) | (1ULL << WIDX_STRAIGHT) | (1ULL << WIDX_RIGHT_CURVE)
                | (1ULL << WIDX_RIGHT_CURVE_SMALL) | (1ULL << WIDX_RIGHT_CURVE_VERY_SMALL)
                | (1ULL << WIDX_SPECIAL_TRACK_DROPDOWN) | (1ULL << WIDX_SLOPE_DOWN_STEEP) | (1ULL << WIDX_SLOPE_DOWN)
                | (1ULL << WIDX_LEVEL) | (1ULL << WIDX_SLOPE_UP) | (1ULL << WIDX_SLOPE_UP_STEEP) | (1ULL << WIDX_CHAIN_LIFT)
                | (1ULL << WIDX_BANK_LEFT) | (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT)
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
        uint64_t currentDisabledWidgets = disabled_widgets;
        if (currentDisabledWidgets == disabledWidgets)
            return;

        for (rct_widgetindex i = 0; i < 64; i++)
        {
            if ((disabledWidgets & (1ULL << i)) != (currentDisabledWidgets & (1ULL << i)))
            {
                widget_invalidate(*this, i);
            }
        }
        disabled_widgets = disabledWidgets;
    }

    void OnUpdate() override
    {
        auto currentRide = get_ride(_currentRideIndex);
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

        switch (_currentTrackCurve)
        {
            case TrackElemType::SpinningTunnel | RideConstructionSpecialPieceSelected:
            case TrackElemType::Whirlpool | RideConstructionSpecialPieceSelected:
            case TrackElemType::Rapids | RideConstructionSpecialPieceSelected:
            case TrackElemType::Waterfall | RideConstructionSpecialPieceSelected:
                widget_invalidate(*this, WIDX_CONSTRUCT);
                break;
        }

        if (_rideConstructionState == RideConstructionState::Place)
        {
            if (!WidgetIsActiveTool(*this, WIDX_CONSTRUCT))
            {
                Close();
                return;
            }
        }

        if (_rideConstructionState == RideConstructionState::EntranceExit)
        {
            if (!WidgetIsActiveTool(*this, WIDX_ENTRANCE) && !WidgetIsActiveTool(*this, WIDX_EXIT))
            {
                _rideConstructionState = gRideEntranceExitPlacePreviousRideConstructionState;
                window_ride_construction_update_active_elements();
            }
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

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        WindowRideConstructionUpdateEnabledTrackPieces();
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_NEXT_SECTION:
                ride_select_next_section();
                break;
            case WIDX_PREVIOUS_SECTION:
                ride_select_previous_section();
                break;
            case WIDX_CONSTRUCT:
                Construct();
                // Force any footpath construction to recheck the area.
                gProvisionalFootpath.Flags |= PROVISIONAL_PATH_FLAG_2;
                break;
            case WIDX_DEMOLISH:
                MouseUpDemolish();
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
                auto currentRide = get_ride(_currentRideIndex);
                if (currentRide != nullptr)
                {
                    auto status = currentRide->status == RideStatus::Simulating ? RideStatus::Closed : RideStatus::Simulating;
                    auto gameAction = RideSetStatusAction(currentRide->id, status);
                    GameActions::Execute(&gameAction);
                }
                break;
            }
        }
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        WindowRideConstructionUpdateEnabledTrackPieces();
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
                if (IsTrackEnabled(TRACK_HELIX_SMALL))
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftHalfBankedHelixDownLarge | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightHalfBankedHelixDownLarge
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftHalfBankedHelixDownSmall | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightHalfBankedHelixDownSmall
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
                if (IsTrackEnabled(TRACK_HELIX_LARGE))
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftQuarterBankedHelixLargeDown
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightQuarterBankedHelixLargeDown
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
                if (IsTrackEnabled(TRACK_HELIX_LARGE_UNBANKED))
                {
                    if (_currentTrackBankEnd == TRACK_BANK_NONE)
                    {
                        if (_currentTrackCurve == TRACK_CURVE_LEFT)
                        {
                            _currentTrackCurve = TrackElemType::LeftQuarterHelixLargeDown
                                | RideConstructionSpecialPieceSelected;
                            _currentTrackPrice = MONEY32_UNDEFINED;
                            window_ride_construction_update_active_elements();
                            break;
                        }
                        if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                        {
                            _currentTrackCurve = TrackElemType::RightQuarterHelixLargeDown
                                | RideConstructionSpecialPieceSelected;
                            _currentTrackPrice = MONEY32_UNDEFINED;
                            window_ride_construction_update_active_elements();
                            break;
                        }
                    }
                }
                if (widgets[WIDX_SLOPE_DOWN_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP)
                {
                    UpdateLiftHillSelected(TRACK_SLOPE_DOWN_60);
                }
                else
                {
                    UpdateLiftHillSelected(TRACK_SLOPE_UP_90);
                }
                break;
            case WIDX_SLOPE_DOWN:
                ride_construction_invalidate_current_track();
                if (_rideConstructionState == RideConstructionState::Back && _currentTrackBankEnd != TRACK_BANK_NONE)
                {
                    _currentTrackBankEnd = TRACK_BANK_NONE;
                }
                UpdateLiftHillSelected(TRACK_SLOPE_DOWN_25);
                break;
            case WIDX_LEVEL:
                ride_construction_invalidate_current_track();
                if (_rideConstructionState == RideConstructionState::Front && _previousTrackSlopeEnd == 6)
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
                else if (_rideConstructionState == RideConstructionState::Back && _previousTrackSlopeEnd == 2)
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
                UpdateLiftHillSelected(TRACK_SLOPE_NONE);
                break;
            case WIDX_SLOPE_UP:
                ride_construction_invalidate_current_track();
                if (_rideConstructionState == RideConstructionState::Front && _currentTrackBankEnd != TRACK_BANK_NONE)
                {
                    _currentTrackBankEnd = TRACK_BANK_NONE;
                }
                if (currentRide->GetRideTypeDescriptor().SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
                {
                    if (_rideConstructionState == RideConstructionState::Front && _currentTrackCurve == TRACK_CURVE_NONE)
                    {
                        _currentTrackCurve = TrackElemType::ReverseFreefallSlope | RideConstructionSpecialPieceSelected;
                        window_ride_construction_update_active_elements();
                    }
                }
                else
                {
                    UpdateLiftHillSelected(TRACK_SLOPE_UP_25);
                }
                break;
            case WIDX_SLOPE_UP_STEEP:
                ride_construction_invalidate_current_track();
                if (IsTrackEnabled(TRACK_HELIX_SMALL))
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftHalfBankedHelixUpLarge | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightHalfBankedHelixUpLarge | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_LEFT_SMALL && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftHalfBankedHelixUpSmall | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT_SMALL && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightHalfBankedHelixUpSmall | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
                if (IsTrackEnabled(TRACK_HELIX_LARGE))
                {
                    if (_currentTrackCurve == TRACK_CURVE_LEFT && _currentTrackBankEnd == TRACK_BANK_LEFT)
                    {
                        _currentTrackCurve = TrackElemType::LeftQuarterBankedHelixLargeUp
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                    if (_currentTrackCurve == TRACK_CURVE_RIGHT && _currentTrackBankEnd == TRACK_BANK_RIGHT)
                    {
                        _currentTrackCurve = TrackElemType::RightQuarterBankedHelixLargeUp
                            | RideConstructionSpecialPieceSelected;
                        _currentTrackPrice = MONEY32_UNDEFINED;
                        window_ride_construction_update_active_elements();
                        break;
                    }
                }
                if (IsTrackEnabled(TRACK_HELIX_LARGE_UNBANKED))
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
                        if (_currentTrackCurve == TRACK_CURVE_RIGHT)
                        {
                            _currentTrackCurve = TrackElemType::RightQuarterHelixLargeUp | RideConstructionSpecialPieceSelected;
                            _currentTrackPrice = MONEY32_UNDEFINED;
                            window_ride_construction_update_active_elements();
                            break;
                        }
                    }
                }
                if (widgets[WIDX_SLOPE_UP_STEEP].tooltip == STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP)
                {
                    UpdateLiftHillSelected(TRACK_SLOPE_UP_60);
                }
                else
                {
                    UpdateLiftHillSelected(TRACK_SLOPE_DOWN_90);
                }
                break;
            case WIDX_CHAIN_LIFT:
                ride_construction_invalidate_current_track();
                _currentTrackLiftHill ^= CONSTRUCTION_LIFT_HILL_SELECTED;
                if ((_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED) && !gCheatsEnableChainLiftOnAllTrack)
                    _currentTrackAlternative &= ~RIDE_TYPE_ALTERNATIVE_TRACK_PIECES;
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
                        if (_rideConstructionState == RideConstructionState::Selected)
                        {
                            SetBrakeSpeed(brakesSpeed);
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
                        if (_rideConstructionState == RideConstructionState::Selected)
                        {
                            SetBrakeSpeed(brakesSpeed);
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
                ShowSpecialTrackDropdown(&widgets[widgetIndex]);
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
                if (!gCheatsEnableChainLiftOnAllTrack)
                    _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
                _currentTrackPrice = MONEY32_UNDEFINED;
                window_ride_construction_update_active_elements();
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
                        window_ride_construction_update_active_elements();
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
                        window_ride_construction_update_active_elements();
                    }
                }
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
        if (widgetIndex != WIDX_SPECIAL_TRACK_DROPDOWN)
            return;
        if (selectedIndex == -1)
            return;

        ride_construction_invalidate_current_track();
        _currentTrackPrice = MONEY32_UNDEFINED;
        track_type_t trackPiece = _currentPossibleRideConfigurations[selectedIndex];
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

    void OnToolUpdate(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
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

    void OnToolDown(rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (widgetIndex)
        {
            case WIDX_CONSTRUCT:
                ride_construction_tooldown_construct(screenCoords);
                break;
            case WIDX_ENTRANCE:
            case WIDX_EXIT:
                ToolDownEntranceExit(screenCoords);
                break;
        }
    }

    void OnPrepareDraw() override
    {
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        StringId stringId = STR_RIDE_CONSTRUCTION_SPECIAL;
        if (_currentTrackCurve & RideConstructionSpecialPieceSelected)
        {
            const auto& rtd = currentRide->GetRideTypeDescriptor();
            const auto& ted = GetTrackElementDescriptor(_currentTrackCurve & ~RideConstructionSpecialPieceSelected);
            stringId = ted.Description;
            if (stringId == STR_RAPIDS && rtd.Category != RIDE_CATEGORY_WATER)
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
                brakeSpeed2 = get_booster_speed(currentRide->type, brakeSpeed2);
            }
            ft.Add<uint16_t>(brakeSpeed2);
        }

        widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].text = RideConstructionSeatAngleRotationStrings[_currentSeatRotationAngle];

        // Simulate button
        auto& simulateWidget = widgets[WIDX_SIMULATE];
        simulateWidget.type = WindowWidgetType::Empty;
        if (currentRide->SupportsStatus(RideStatus::Simulating))
        {
            simulateWidget.type = WindowWidgetType::FlatBtn;
            if (currentRide->status == RideStatus::Simulating)
            {
                pressed_widgets |= (1ULL << WIDX_SIMULATE);
            }
            else
            {
                pressed_widgets &= ~(1ULL << WIDX_SIMULATE);
            }
        }

        // Set window title arguments
        ft = Formatter::Common();
        currentRide->FormatNameTo(ft);
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        rct_drawpixelinfo clipdpi;
        rct_widget* widget;
        int32_t widgetWidth, widgetHeight;

        DrawWidgets(dpi);

        widget = &widgets[WIDX_CONSTRUCT];
        if (widget->type == WindowWidgetType::Empty)
            return;

        RideId rideIndex;
        int32_t trackType, trackDirection, liftHillAndInvertedState;
        if (window_ride_construction_update_state(
                &trackType, &trackDirection, &rideIndex, &liftHillAndInvertedState, nullptr, nullptr))
            return;

        // Draw track piece
        auto screenCoords = ScreenCoordsXY{ windowPos.x + widget->left + 1, windowPos.y + widget->top + 1 };
        widgetWidth = widget->width() - 1;
        widgetHeight = widget->height() - 1;
        if (clip_drawpixelinfo(&clipdpi, &dpi, screenCoords, widgetWidth, widgetHeight))
        {
            DrawTrackPiece(&clipdpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, widgetWidth, widgetHeight);
        }

        // Draw cost
        screenCoords = { windowPos.x + widget->midX(), windowPos.y + widget->bottom - 23 };
        if (_rideConstructionState != RideConstructionState::Place)
            DrawTextBasic(&dpi, screenCoords, STR_BUILD_THIS, {}, { TextAlignment::CENTRE });

        screenCoords.y += 11;
        if (_currentTrackPrice != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            auto ft = Formatter();
            ft.Add<money64>(_currentTrackPrice);
            DrawTextBasic(&dpi, screenCoords, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
        }
    }

    void UpdateWidgets()
    {
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }
        int32_t rideType = RideGetAlternativeType(currentRide);

        hold_down_widgets = 0;
        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY) || !currentRide->HasStation())
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

        if (_numCurrentPossibleSpecialTrackPieces == 0)
        {
            widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Empty;
        }
        else
        {
            widgets[WIDX_SPECIAL_TRACK_DROPDOWN].type = WindowWidgetType::Button;
        }

        if (IsTrackEnabled(TRACK_STRAIGHT))
        {
            widgets[WIDX_STRAIGHT].type = WindowWidgetType::FlatBtn;
        }
        else
        {
            widgets[WIDX_STRAIGHT].type = WindowWidgetType::Empty;
        }

        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_HAS_LARGE_CURVES))
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
        widgets[WIDX_LEFT_CURVE_SMALL].left = 28;
        widgets[WIDX_LEFT_CURVE_SMALL].right = 49;
        widgets[WIDX_RIGHT_CURVE_SMALL].left = 116;
        widgets[WIDX_RIGHT_CURVE_SMALL].right = 137;
        widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE;
        widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE;
        if (IsTrackEnabled(TRACK_CURVE_VERTICAL))
        {
            widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
            widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
            widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
            widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
            widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
            widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
        }
        if (IsTrackEnabled(TRACK_CURVE))
        {
            widgets[WIDX_LEFT_CURVE].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_RIGHT_CURVE].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_LEFT_CURVE_SMALL].left = 6;
            widgets[WIDX_LEFT_CURVE_SMALL].right = 27;
            widgets[WIDX_LEFT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_LEFT_CURVE_SMALL;
            widgets[WIDX_RIGHT_CURVE_SMALL].left = 138;
            widgets[WIDX_RIGHT_CURVE_SMALL].right = 159;
            widgets[WIDX_RIGHT_CURVE_SMALL].image = SPR_RIDE_CONSTRUCTION_RIGHT_CURVE_SMALL;
        }
        if (IsTrackEnabled(TRACK_CURVE_SMALL))
        {
            widgets[WIDX_LEFT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_RIGHT_CURVE_SMALL].type = WindowWidgetType::FlatBtn;
        }
        if (IsTrackEnabled(TRACK_CURVE_VERY_SMALL))
        {
            widgets[WIDX_LEFT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type = WindowWidgetType::FlatBtn;
        }

        widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::Empty;
        widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::Empty;
        widgets[WIDX_LEVEL].type = WindowWidgetType::Empty;
        widgets[WIDX_SLOPE_UP].type = WindowWidgetType::Empty;
        widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::Empty;
        widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP;
        widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
        widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP;
        widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
        if (GetRideTypeDescriptor(rideType).SupportsTrackPiece(TRACK_REVERSE_FREEFALL))
        {
            widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
        }
        if (IsTrackEnabled(TRACK_SLOPE) || IsTrackEnabled(TRACK_SLOPE_STEEP_DOWN) || IsTrackEnabled(TRACK_SLOPE_STEEP_UP))
        {
            widgets[WIDX_LEVEL].type = WindowWidgetType::FlatBtn;
        }
        if (IsTrackEnabled(TRACK_SLOPE))
        {
            widgets[WIDX_SLOPE_DOWN].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_SLOPE_UP].type = WindowWidgetType::FlatBtn;
        }
        if (IsTrackEnabled(TRACK_HELIX_SMALL) && _currentTrackBankEnd != TRACK_BANK_NONE
            && _currentTrackSlopeEnd == TRACK_SLOPE_NONE)
        {
            if (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL)
            {
                // Enable helix
                widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
                if (rideType != RIDE_TYPE_SPLASH_BOATS && rideType != RIDE_TYPE_RIVER_RAFTS)
                    widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
            }
        }

        if (IsTrackEnabled(TRACK_SLOPE_STEEP_DOWN))
        {
            widgets[WIDX_SLOPE_DOWN_STEEP].type = WindowWidgetType::FlatBtn;
        }
        if (IsTrackEnabled(TRACK_SLOPE_STEEP_UP))
        {
            widgets[WIDX_SLOPE_UP_STEEP].type = WindowWidgetType::FlatBtn;
        }

        if (currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_UP_INCLINE_REQUIRES_LIFT)
            && (_currentTrackSlopeEnd == TRACK_SLOPE_UP_25 || _currentTrackSlopeEnd == TRACK_SLOPE_UP_60)
            && !gCheatsEnableAllDrawableTrackPieces)
        {
            _currentTrackLiftHill |= CONSTRUCTION_LIFT_HILL_SELECTED;
        }

        int32_t x;
        if ((IsTrackEnabled(TRACK_LIFT_HILL) && (_currentTrackCurve & RideConstructionSpecialPieceSelected) == 0)
            || (gCheatsEnableChainLiftOnAllTrack && currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_TRACK)))
        {
            widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::FlatBtn;
            x = 9;
        }
        else
        {
            widgets[WIDX_CHAIN_LIFT].type = WindowWidgetType::Empty;
            x = 23;
        }

        for (int32_t i = WIDX_SLOPE_DOWN_STEEP; i <= WIDX_SLOPE_UP_STEEP; i++)
        {
            widgets[i].left = x;
            widgets[i].right = x + 23;
            x += 24;
        }

        widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_UP_STEEP;
        widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_UP_TIP;
        widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_SLOPE_DOWN_STEEP;
        widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_STEEP_SLOPE_DOWN_TIP;
        if (IsTrackEnabled(TRACK_SLOPE_VERTICAL))
        {
            if (_previousTrackSlopeEnd == TRACK_SLOPE_UP_60 || _previousTrackSlopeEnd == TRACK_SLOPE_UP_90)
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
                widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_VERTICAL_RISE;
                widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_RISE_TIP;
            }
            else if (_previousTrackSlopeEnd == TRACK_SLOPE_DOWN_60 || _previousTrackSlopeEnd == TRACK_SLOPE_DOWN_90)
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
                widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_VERTICAL_DROP;
                widgets[WIDX_SLOPE_UP_STEEP].tooltip = STR_RIDE_CONSTRUCTION_VERTICAL_DROP_TIP;
            }
        }

        if (IsTrackEnabled(TRACK_HELIX_LARGE_UNBANKED) && _currentTrackSlopeEnd == TRACK_SLOPE_NONE
            && _currentTrackBankEnd == TRACK_BANK_NONE
            && (_currentTrackCurve == TRACK_CURVE_LEFT || _currentTrackCurve == TRACK_CURVE_RIGHT))
        {
            widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_DOWN;
            widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
            widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_UP;
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

        if ((IsTrackEnabled(TRACK_HELIX_LARGE) || IsTrackEnabled(TRACK_HELIX_SMALL))
            && (_currentTrackCurve >= TRACK_CURVE_LEFT && _currentTrackCurve <= TRACK_CURVE_RIGHT_SMALL)
            && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd != TRACK_BANK_NONE)
        {
            widgets[WIDX_SLOPE_DOWN_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_DOWN;
            widgets[WIDX_SLOPE_DOWN_STEEP].tooltip = STR_RIDE_CONSTRUCTION_HELIX_DOWN_TIP;
            widgets[WIDX_SLOPE_UP_STEEP].image = SPR_RIDE_CONSTRUCTION_HELIX_UP;
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

        widgets[WIDX_BANKING_GROUPBOX].image = STR_RIDE_CONSTRUCTION_ROLL_BANKING;
        widgets[WIDX_BANK_LEFT].image = SPR_RIDE_CONSTRUCTION_LEFT_BANK;
        widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_LEFT_CURVE_TIP;
        widgets[WIDX_BANK_LEFT].left = 47;
        widgets[WIDX_BANK_LEFT].right = 70;
        widgets[WIDX_BANK_LEFT].top = 132;
        widgets[WIDX_BANK_LEFT].bottom = 155;
        widgets[WIDX_BANK_STRAIGHT].image = SPR_RIDE_CONSTRUCTION_NO_BANK;
        widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_NO_ROLL_TIP;
        widgets[WIDX_BANK_STRAIGHT].left = 71;
        widgets[WIDX_BANK_STRAIGHT].right = 94;
        widgets[WIDX_BANK_STRAIGHT].top = 132;
        widgets[WIDX_BANK_STRAIGHT].bottom = 155;
        widgets[WIDX_BANK_RIGHT].image = SPR_RIDE_CONSTRUCTION_RIGHT_BANK;
        widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_ROLL_FOR_RIGHT_CURVE_TIP;
        widgets[WIDX_BANK_RIGHT].left = 95;
        widgets[WIDX_BANK_RIGHT].right = 118;
        widgets[WIDX_BANK_RIGHT].top = 132;
        widgets[WIDX_BANK_RIGHT].bottom = 155;
        widgets[WIDX_BANK_LEFT].type = WindowWidgetType::Empty;
        widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::Empty;
        widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::Empty;
        widgets[WIDX_U_TRACK].type = WindowWidgetType::Empty;
        widgets[WIDX_O_TRACK].type = WindowWidgetType::Empty;

        bool brakesSelected = _selectedTrackType == TrackElemType::Brakes
            || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::Brakes);
        bool boosterTrackSelected = _selectedTrackType == TrackElemType::Booster
            || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::Booster);

        if (!brakesSelected && !boosterTrackSelected)
        {
            if (IsTrackEnabled(TRACK_FLAT_ROLL_BANKING))
            {
                widgets[WIDX_BANK_LEFT].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::FlatBtn;
            }
            if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES))
            {
                if (rideType == RIDE_TYPE_WATER_COASTER)
                {
                    widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_RC_TRACK;
                    widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_WATER_CHANNEL;
                    widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_STANDARD_RC_TRACK_TIP;
                    widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_WATER_CHANNEL_TIP;
                    if ((_currentTrackCurve < TRACK_CURVE_LEFT_SMALL
                         || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::SBendLeft)
                         || _currentTrackCurve == (RideConstructionSpecialPieceSelected | TrackElemType::SBendRight))
                        && _currentTrackSlopeEnd == TRACK_SLOPE_NONE && _currentTrackBankEnd == TRACK_BANK_NONE)
                    {
                        widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                        widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
                        widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;
                    }
                }
                else
                {
                    widgets[WIDX_U_TRACK].image = SPR_RIDE_CONSTRUCTION_U_SHAPED_TRACK;
                    widgets[WIDX_O_TRACK].image = SPR_RIDE_CONSTRUCTION_O_SHAPED_TRACK;
                    widgets[WIDX_U_TRACK].tooltip = STR_RIDE_CONSTRUCTION_U_SHAPED_OPEN_TRACK_TIP;
                    widgets[WIDX_O_TRACK].tooltip = STR_RIDE_CONSTRUCTION_O_SHAPED_ENCLOSED_TRACK_TIP;
                    widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_TRACK_STYLE;
                    widgets[WIDX_U_TRACK].type = WindowWidgetType::FlatBtn;
                    widgets[WIDX_O_TRACK].type = WindowWidgetType::FlatBtn;
                }
            }
        }
        else
        {
            if (brakesSelected)
            {
                widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED;
                widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
                widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
                widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_LIMIT_TIP;
            }
            else
            {
                widgets[WIDX_BANKING_GROUPBOX].text = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED;
                widgets[WIDX_BANK_LEFT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
                widgets[WIDX_BANK_STRAIGHT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
                widgets[WIDX_BANK_RIGHT].tooltip = STR_RIDE_CONSTRUCTION_BOOSTER_SPEED_LIMIT_TIP;
            }

            _currentlyShowingBrakeOrBoosterSpeed = true;
            widgets[WIDX_BANK_LEFT].text = STR_RIDE_CONSTRUCTION_BRAKE_SPEED_VELOCITY;

            widgets[WIDX_BANK_LEFT].type = WindowWidgetType::Spinner;
            widgets[WIDX_BANK_LEFT].left = 12;
            widgets[WIDX_BANK_LEFT].right = 96;
            widgets[WIDX_BANK_LEFT].top = 138;
            widgets[WIDX_BANK_LEFT].bottom = 149;
            widgets[WIDX_BANK_STRAIGHT].type = WindowWidgetType::Button;
            widgets[WIDX_BANK_STRAIGHT].text = STR_NUMERIC_UP;
            widgets[WIDX_BANK_STRAIGHT].left = 84;
            widgets[WIDX_BANK_STRAIGHT].right = 95;
            widgets[WIDX_BANK_STRAIGHT].top = 139;
            widgets[WIDX_BANK_STRAIGHT].bottom = 148;
            widgets[WIDX_BANK_RIGHT].type = WindowWidgetType::Button;
            widgets[WIDX_BANK_RIGHT].text = STR_NUMERIC_DOWN;
            widgets[WIDX_BANK_RIGHT].left = 72;
            widgets[WIDX_BANK_RIGHT].right = 83;
            widgets[WIDX_BANK_RIGHT].top = 139;
            widgets[WIDX_BANK_RIGHT].bottom = 148;
            hold_down_widgets |= (1ULL << WIDX_BANK_STRAIGHT) | (1ULL << WIDX_BANK_RIGHT);
        }

        widgets[WIDX_BANKING_GROUPBOX].right = 162;
        widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Empty;
        widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Empty;
        widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Empty;
        widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Empty;

        const auto& rtd = GetRideTypeDescriptor(rideType);
        if (rtd.HasFlag(RIDE_TYPE_FLAG_HAS_SEAT_ROTATION) && _selectedTrackType != TrackElemType::Brakes
            && _currentTrackCurve != (RideConstructionSpecialPieceSelected | TrackElemType::Brakes))
        {
            widgets[WIDX_SEAT_ROTATION_GROUPBOX].type = WindowWidgetType::Groupbox;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER].type = WindowWidgetType::Spinner;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_UP].type = WindowWidgetType::Button;
            widgets[WIDX_SEAT_ROTATION_ANGLE_SPINNER_DOWN].type = WindowWidgetType::Button;
            widgets[WIDX_BANKING_GROUPBOX].right = 92;
            if (widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Spinner)
            {
                for (int32_t i = WIDX_BANK_LEFT; i <= WIDX_BANK_RIGHT; i++)
                {
                    widgets[i].left -= 36;
                    widgets[i].right -= 36;
                }
            }
        }

        uint64_t pressedWidgets = pressed_widgets
            & ((1ULL << WIDX_BACKGROUND) | (1ULL << WIDX_TITLE) | (1ULL << WIDX_CLOSE) | (1ULL << WIDX_DIRECTION_GROUPBOX)
               | (1ULL << WIDX_SLOPE_GROUPBOX) | (1ULL << WIDX_BANKING_GROUPBOX) | (1ULL << WIDX_CONSTRUCT)
               | (1ULL << WIDX_DEMOLISH) | (1ULL << WIDX_PREVIOUS_SECTION) | (1ULL << WIDX_NEXT_SECTION)
               | (1ULL << WIDX_ENTRANCE_EXIT_GROUPBOX) | (1ULL << WIDX_ENTRANCE) | (1ULL << WIDX_EXIT));

        widgets[WIDX_CONSTRUCT].type = WindowWidgetType::Empty;
        widgets[WIDX_DEMOLISH].type = WindowWidgetType::FlatBtn;
        widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
        if (GetRideTypeDescriptor(rideType).HasFlag(RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS))
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
                    pressed_widgets |= (1ULL << WIDX_O_TRACK);
                }
                else
                {
                    pressed_widgets |= (1ULL << WIDX_U_TRACK);
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
            pressedWidgets |= (1ULL << WIDX_CHAIN_LIFT);

        pressed_widgets = pressedWidgets;
        Invalidate();
    }

    void UpdatePossibleRideConfigurations()
    {
        int32_t trackType;
        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        _currentlyShowingBrakeOrBoosterSpeed = false;

        int32_t currentPossibleRideConfigurationIndex = 0;
        _numCurrentPossibleSpecialTrackPieces = 0;
        for (trackType = 0; trackType < TrackElemType::Count; trackType++)
        {
            const auto& ted = GetTrackElementDescriptor(trackType);
            int32_t trackTypeCategory = ted.Definition.type;

            if (trackTypeCategory == TRACK_NONE)
                continue;

            if (!IsTrackEnabled(trackTypeCategory))
            {
                continue;
            }

            int32_t slope, bank;
            if (_rideConstructionState == RideConstructionState::Front
                || _rideConstructionState == RideConstructionState::Place)
            {
                slope = ted.Definition.vangle_start;
                bank = ted.Definition.bank_start;
            }
            else if (_rideConstructionState == RideConstructionState::Back)
            {
                slope = ted.Definition.vangle_end;
                bank = ted.Definition.bank_end;
            }
            else
            {
                continue;
            }

            if (!currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE))
            {
                if (ted.Definition.type == TRACK_HELIX_SMALL || ted.Definition.type == TRACK_HELIX_LARGE)
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

            if ((bank == TRACK_BANK_UPSIDE_DOWN || ted.Definition.bank_end == TRACK_BANK_UPSIDE_DOWN)
                && bank != _previousTrackBankEnd)
                continue;

            _currentPossibleRideConfigurations[currentPossibleRideConfigurationIndex] = trackType;
            _currentDisabledSpecialTrackPieces |= (1ULL << currentPossibleRideConfigurationIndex);
            if (_currentTrackPieceDirection < 4 && slope == _previousTrackSlopeEnd && bank == _previousTrackBankEnd
                && (trackType != TrackElemType::TowerBase
                    || currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_ALLOW_EXTRA_TOWER_BASES)))
            {
                _currentDisabledSpecialTrackPieces &= ~(1ULL << currentPossibleRideConfigurationIndex);
                _numCurrentPossibleSpecialTrackPieces++;
            }
            currentPossibleRideConfigurationIndex++;
        }
        _numCurrentPossibleRideConfigurations = currentPossibleRideConfigurationIndex;
    }

    void UpdateMapSelection()
    {
        int32_t trackType, trackDirection;
        CoordsXYZ trackPos{};

        map_invalidate_map_selection_tiles();
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
                if (window_ride_construction_update_state(&trackType, &trackDirection, nullptr, nullptr, &trackPos, nullptr))
                {
                    trackDirection = _currentTrackPieceDirection;
                    trackType = 0;
                    trackPos = _currentTrackBegin;
                }
                break;
        }

        if (get_ride(_currentRideIndex))
        {
            SelectMapTiles(trackType, trackDirection, trackPos);
            map_invalidate_map_selection_tiles();
        }
    }

    void SelectMapTiles(int32_t trackType, int32_t trackDirection, const CoordsXY& tileCoords)
    {
        // If the scenery tool is active, we do not display our tiles as it
        // will conflict with larger scenery objects selecting tiles
        if (scenery_tool_is_active())
        {
            return;
        }

        const rct_preview_track* trackBlock;

        const auto& ted = GetTrackElementDescriptor(trackType);
        trackBlock = ted.Block;
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
    bool TrackPieceDirectionIsDiagonal(const uint8_t direction)
    {
        return direction >= 4;
    }

    void Construct()
    {
        RideId rideIndex;
        int32_t trackType, trackDirection, liftHillAndAlternativeState, properties;
        CoordsXYZ trackPos{};

        _currentTrackPrice = MONEY32_UNDEFINED;
        _trackPlaceCost = MONEY32_UNDEFINED;
        _trackPlaceErrorMessage = STR_NONE;
        ride_construction_invalidate_current_track();
        if (window_ride_construction_update_state(
                &trackType, &trackDirection, &rideIndex, &liftHillAndAlternativeState, &trackPos, &properties))
        {
            window_ride_construction_update_active_elements();
            return;
        }

        auto currentRide = get_ride(_currentRideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        auto trackPlaceAction = TrackPlaceAction(
            rideIndex, trackType, currentRide->type, { trackPos, static_cast<uint8_t>(trackDirection) }, (properties)&0xFF,
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
            _trackPlaceCost = MONEY32_UNDEFINED;
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

        if (network_get_mode() != NETWORK_MODE_NONE)
        {
            _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK_PLACE_ACTION_QUEUED;
        }

        const auto resultData = res.GetData<TrackPlaceActionResult>();
        if (resultData.GroundFlags & ELEMENT_IS_UNDERGROUND)
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

    void MouseUpDemolish()
    {
        int32_t direction;
        TileElement* tileElement;
        CoordsXYE inputElement, outputElement;
        track_begin_end trackBeginEnd;

        _currentTrackPrice = MONEY32_UNDEFINED;
        ride_construction_invalidate_current_track();

        // Select the track element that is to be deleted
        _rideConstructionState2 = RideConstructionState::Selected;
        if (_rideConstructionState == RideConstructionState::Front)
        {
            if (!ride_select_backwards_from_front())
            {
                window_ride_construction_update_active_elements();
                return;
            }
            _rideConstructionState2 = RideConstructionState::Front;
        }
        else if (_rideConstructionState == RideConstructionState::Back)
        {
            if (!ride_select_forwards_from_back())
            {
                window_ride_construction_update_active_elements();
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
            newCoords = GetTrackElementOriginAndApplyChanges(
                { _currentTrackBegin, static_cast<Direction>(direction & 3) }, type, 0, &tileElement, 0);

            if (!newCoords.has_value())
            {
                window_ride_construction_update_active_elements();
                return;
            }

            const auto& ted = GetTrackElementDescriptor(tileElement->AsTrack()->GetTrackType());
            const rct_preview_track* trackBlock = ted.Block;
            newCoords->z = (tileElement->GetBaseZ()) - trackBlock->z;
            gGotoStartPlacementMode = true;

            // When flat rides are deleted, the window should be reset so the currentRide can be placed again.
            auto currentRide = get_ride(_currentRideIndex);
            const auto& rtd = currentRide->GetRideTypeDescriptor();
            if (rtd.HasFlag(RIDE_TYPE_FLAG_FLAT_RIDE) && !rtd.HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
            {
                ride_initialise_construction_window(currentRide);
            }
        }

        auto trackRemoveAction = TrackRemoveAction(
            _currentTrackPieceType, 0, { _currentTrackBegin.x, _currentTrackBegin.y, _currentTrackBegin.z, currentDirection });

        trackRemoveAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error != GameActions::Status::Ok)
            {
                window_ride_construction_update_active_elements();
            }
            else
            {
                auto currentRide = get_ride(_currentRideIndex);
                if (currentRide != nullptr)
                {
                    window_ride_construction_mouseup_demolish_next_piece(
                        { *newCoords, static_cast<Direction>(direction) }, type);
                }
            }
        });

        GameActions::Execute(&trackRemoveAction);
    }

    void Rotate()
    {
        _autoRotatingShop = false;
        _currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
        ride_construction_invalidate_current_track();
        _currentTrackPrice = MONEY32_UNDEFINED;
        window_ride_construction_update_active_elements();
    }

    void EntranceClick()
    {
        if (tool_set(*this, WIDX_ENTRANCE, Tool::Crosshair))
        {
            auto currentRide = get_ride(_currentRideIndex);
            if (currentRide != nullptr && !ride_try_get_origin_element(currentRide, nullptr))
            {
                ride_initialise_construction_window(currentRide);
            }
        }
        else
        {
            gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_ENTRANCE;
            gRideEntranceExitPlaceRideIndex = _currentRideIndex;
            gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
            input_set_flag(INPUT_FLAG_6, true);
            ride_construction_invalidate_current_track();
            if (_rideConstructionState != RideConstructionState::EntranceExit)
            {
                gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
                _rideConstructionState = RideConstructionState::EntranceExit;
            }
            window_ride_construction_update_active_elements();
        }
    }

    void ExitClick()
    {
        if (tool_set(*this, WIDX_EXIT, Tool::Crosshair))
        {
            auto currentRide = get_ride(_currentRideIndex);
            if (!ride_try_get_origin_element(currentRide, nullptr))
            {
                ride_initialise_construction_window(currentRide);
            }
        }
        else
        {
            gRideEntranceExitPlaceType = ENTRANCE_TYPE_RIDE_EXIT;
            gRideEntranceExitPlaceRideIndex = _currentRideIndex;
            gRideEntranceExitPlaceStationIndex = StationIndex::FromUnderlying(0);
            input_set_flag(INPUT_FLAG_6, true);
            ride_construction_invalidate_current_track();
            if (_rideConstructionState != RideConstructionState::EntranceExit)
            {
                gRideEntranceExitPlacePreviousRideConstructionState = _rideConstructionState;
                _rideConstructionState = RideConstructionState::EntranceExit;
            }
            window_ride_construction_update_active_elements();
        }
    }

    void UpdateLiftHillSelected(int32_t slope)
    {
        _currentTrackSlopeEnd = slope;
        _currentTrackPrice = MONEY32_UNDEFINED;
        if (_rideConstructionState == RideConstructionState::Front && !gCheatsEnableChainLiftOnAllTrack)
        {
            switch (slope)
            {
                case TRACK_SLOPE_NONE:
                case TRACK_SLOPE_UP_25:
                case TRACK_SLOPE_UP_60:
                    break;
                default:
                    _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
                    break;
            }
        }
        window_ride_construction_update_active_elements();
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
                window_ride_construction_update_active_elements();
            });
            GameActions::Execute(&trackSetBrakeSpeed);
            return;
        }
        window_ride_construction_update_active_elements();
    }

    void ShowSpecialTrackDropdown(rct_widget* widget)
    {
        int32_t defaultIndex = -1;
        for (int32_t i = 0; i < _numCurrentPossibleRideConfigurations; i++)
        {
            track_type_t trackPiece = _currentPossibleRideConfigurations[i];

            const auto& ted = GetTrackElementDescriptor(trackPiece);
            StringId trackPieceStringId = ted.Description;
            if (trackPieceStringId == STR_RAPIDS)
            {
                auto currentRide = get_ride(_currentRideIndex);
                if (currentRide != nullptr)
                {
                    const auto& rtd = currentRide->GetRideTypeDescriptor();
                    if (rtd.Category != RIDE_CATEGORY_WATER)
                        trackPieceStringId = STR_LOG_BUMPS;
                }
            }
            gDropdownItems[i].Format = trackPieceStringId;
            if ((trackPiece | RideConstructionSpecialPieceSelected) == _currentTrackCurve)
            {
                defaultIndex = i;
            }
        }

        WindowDropdownShowTextCustomWidth(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 0,
            _numCurrentPossibleRideConfigurations, widget->width());

        for (int32_t i = 0; i < 64; i++)
        {
            if (_currentDisabledSpecialTrackPieces & (1ULL << i))
            {
                Dropdown::SetDisabled(i, true);
            }
        }
        gDropdownDefaultIndex = defaultIndex;
    }

    void RideSelectedTrackSetSeatRotation(int32_t seatRotation)
    {
        GetTrackElementOriginAndApplyChanges(
            { _currentTrackBegin, static_cast<Direction>(_currentTrackPieceDirection & 3) }, _currentTrackPieceType,
            seatRotation, nullptr, TRACK_ELEMENT_SET_SEAT_ROTATION);
        window_ride_construction_update_active_elements();
    }

    void ToolDownEntranceExit(const ScreenCoordsXY& screenCoords)
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

            auto currentRide = get_ride(gRideEntranceExitPlaceRideIndex);
            if (currentRide != nullptr && ride_are_all_possible_entrances_and_exits_built(currentRide).Successful)
            {
                tool_cancel();
                if (currentRide->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_HAS_NO_TRACK))
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

    void DrawTrackPiece(
        rct_drawpixelinfo* dpi, RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndInvertedState,
        int32_t widgetWidth, int32_t widgetHeight)
    {
        auto currentRide = get_ride(rideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        const auto& ted = GetTrackElementDescriptor(trackType);
        const auto* trackBlock = ted.Block;
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

        int16_t previewZOffset = ted.Definition.preview_z_offset;
        mapCoords.z -= previewZOffset;

        const ScreenCoordsXY rotatedScreenCoords = translate_3d_to_2d_with_z(get_current_rotation(), mapCoords);

        dpi->x += rotatedScreenCoords.x - widgetWidth / 2;
        dpi->y += rotatedScreenCoords.y - widgetHeight / 2 - 16;

        DrawTrackPieceHelper(dpi, rideIndex, trackType, trackDirection, liftHillAndInvertedState, { 4096, 4096 }, 1024);
    }

    void DrawTrackPieceHelper(
        rct_drawpixelinfo* dpi, RideId rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndInvertedState,
        const CoordsXY& originCoords, int32_t originZ)
    {
        TileElement tempSideTrackTileElement{ 0x80, 0x8F, 128, 128, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        TileElement tempTrackTileElement{};
        TileElement* backupTileElementArrays[5]{};
        paint_session* session = PaintSessionAlloc(dpi, 0);
        trackDirection &= 3;

        auto currentRide = get_ride(rideIndex);
        if (currentRide == nullptr)
        {
            return;
        }

        auto preserveMapSize = gMapSize;

        gMapSize = { MAXIMUM_MAP_SIZE_TECHNICAL, MAXIMUM_MAP_SIZE_TECHNICAL };

        // Setup non changing parts of the temporary track tile element
        tempTrackTileElement.SetType(TileElementType::Track);
        tempTrackTileElement.SetDirection(trackDirection);
        tempTrackTileElement.AsTrack()->SetHasChain((liftHillAndInvertedState & CONSTRUCTION_LIFT_HILL_SELECTED) != 0);
        tempTrackTileElement.SetLastForTile(true);
        tempTrackTileElement.AsTrack()->SetTrackType(trackType);
        tempTrackTileElement.AsTrack()->SetRideType(currentRide->type);
        tempTrackTileElement.AsTrack()->SetHasCableLift(false);
        tempTrackTileElement.AsTrack()->SetInverted((liftHillAndInvertedState & CONSTRUCTION_INVERTED_TRACK_SELECTED) != 0);
        tempTrackTileElement.AsTrack()->SetColourScheme(RIDE_COLOUR_SCHEME_MAIN);
        // Skipping seat rotation, should not be necessary for a temporary piece.
        tempTrackTileElement.AsTrack()->SetRideIndex(rideIndex);

        const auto& ted = GetTrackElementDescriptor(trackType);
        const auto* trackBlock = ted.Block;
        while (trackBlock->index != 255)
        {
            auto quarterTile = trackBlock->var_08.Rotate(trackDirection);
            CoordsXY offsets = { trackBlock->x, trackBlock->y };
            CoordsXY coords = originCoords + offsets.Rotate(trackDirection);

            int32_t baseZ = originZ + trackBlock->z;
            int32_t clearanceZ = trackBlock->var_07 + currentRide->GetRideTypeDescriptor().Heights.ClearanceHeight + baseZ
                + (4 * COORDS_Z_STEP);

            auto centreTileCoords = TileCoordsXY{ coords };
            auto eastTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_EAST];
            auto westTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_WEST];
            auto northTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_NORTH];
            auto southTileCoords = centreTileCoords + TileDirectionDelta[TILE_ELEMENT_DIRECTION_SOUTH];

            // Replace map elements with temporary ones containing track
            backupTileElementArrays[0] = map_get_first_element_at(centreTileCoords);
            backupTileElementArrays[1] = map_get_first_element_at(eastTileCoords);
            backupTileElementArrays[2] = map_get_first_element_at(westTileCoords);
            backupTileElementArrays[3] = map_get_first_element_at(northTileCoords);
            backupTileElementArrays[4] = map_get_first_element_at(southTileCoords);
            map_set_tile_element(centreTileCoords, &tempTrackTileElement);
            map_set_tile_element(eastTileCoords, &tempSideTrackTileElement);
            map_set_tile_element(westTileCoords, &tempSideTrackTileElement);
            map_set_tile_element(northTileCoords, &tempSideTrackTileElement);
            map_set_tile_element(southTileCoords, &tempSideTrackTileElement);

            // Set the temporary track element
            tempTrackTileElement.SetOccupiedQuadrants(quarterTile.GetBaseQuarterOccupied());
            tempTrackTileElement.SetBaseZ(baseZ);
            tempTrackTileElement.SetClearanceZ(clearanceZ);
            tempTrackTileElement.AsTrack()->SetSequenceIndex(trackBlock->index);

            // Draw this map tile
            tile_element_paint_setup(*session, coords, true);

            // Restore map elements
            map_set_tile_element(centreTileCoords, backupTileElementArrays[0]);
            map_set_tile_element(eastTileCoords, backupTileElementArrays[1]);
            map_set_tile_element(westTileCoords, backupTileElementArrays[2]);
            map_set_tile_element(northTileCoords, backupTileElementArrays[3]);
            map_set_tile_element(southTileCoords, backupTileElementArrays[4]);

            trackBlock++;
        }

        gMapSize = preserveMapSize;

        PaintSessionArrange(*session);
        PaintDrawStructs(*session);
        PaintSessionFree(session);
    }
};

/**
 *
 *  rct2: 0x006CB481
 */
rct_window* WindowRideConstructionOpen()
{
    RideId rideIndex = _currentRideIndex;
    CloseRideWindowForConstruction(rideIndex);

    auto currentRide = get_ride(rideIndex);
    if (currentRide == nullptr)
    {
        return nullptr;
    }

    const auto& rtd = currentRide->GetRideTypeDescriptor();
    switch (rtd.ConstructionWindowContext)
    {
        case RideConstructionWindowContext::Maze:
            return context_open_window_view(WV_MAZE_CONSTRUCTION);
        case RideConstructionWindowContext::Default:
            return WindowCreate<RideConstructionWindow>(
                WC_RIDE_CONSTRUCTION, ScreenCoordsXY(0, 29), 166, 394, WF_NO_AUTO_CLOSE);
    }
    return WindowCreate<RideConstructionWindow>(WC_RIDE_CONSTRUCTION, ScreenCoordsXY(0, 29), 166, 394, WF_NO_AUTO_CLOSE);
}

static void CloseConstructWindowOnCompletion(Ride* ride)
{
    if (_rideConstructionState == RideConstructionState::State0)
    {
        auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr)
        {
            if (ride_are_all_possible_entrances_and_exits_built(ride).Successful)
            {
                window_close(*w);
            }
            else
            {
                window_event_mouse_up_call(w, WIDX_ENTRANCE);
            }
        }
    }
}

static void window_ride_construction_do_entrance_exit_check()
{
    auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    auto ride = get_ride(_currentRideIndex);
    if (w == nullptr || ride == nullptr)
    {
        return;
    }

    if (_rideConstructionState == RideConstructionState::State0)
    {
        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr)
        {
            if (!ride_are_all_possible_entrances_and_exits_built(ride).Successful)
            {
                window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
            }
        }
    }
}

static void RideConstructPlacedForwardGameActionCallback(const GameAction* ga, const GameActions::Result* result)
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
            _rideConstructionState = RideConstructionState::Selected;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            ride_select_next_section();
        }
        else
        {
            _rideConstructionState = RideConstructionState::State0;
        }

        window_ride_construction_do_entrance_exit_check();
        window_ride_construction_update_active_elements();
    }

    window_close_by_class(WC_ERROR);
    CloseConstructWindowOnCompletion(ride);
}

static void RideConstructPlacedBackwardGameActionCallback(const GameAction* ga, const GameActions::Result* result)
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
            _rideConstructionState = RideConstructionState::Selected;
            _rideConstructionNextArrowPulse = 0;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            ride_select_previous_section();
        }
        else
        {
            _rideConstructionState = RideConstructionState::State0;
        }

        window_ride_construction_update_active_elements();
    }

    window_close_by_class(WC_ERROR);
    CloseConstructWindowOnCompletion(ride);
}

/**
 *
 *  rct2: 0x006CC538
 */
static std::optional<CoordsXY> RideGetPlacePositionFromScreenPosition(ScreenCoordsXY screenCoords)
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
            uint16_t maxHeight = ZoomLevel::max().ApplyTo(std::numeric_limits<decltype(TileElement::base_height)>::max() - 32);

            _trackPlaceShiftZ = _trackPlaceShiftStart.y - screenCoords.y + 4;
            // Scale delta by zoom to match mouse position.
            auto* mainWnd = window_get_main();
            if (mainWnd != nullptr && mainWnd->viewport != nullptr)
            {
                _trackPlaceShiftZ = mainWnd->viewport->zoom.ApplyTo(_trackPlaceShiftZ);
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
        if (mapCoords.IsNull())
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

    if (mapCoords.x == LOCATION_NULL)
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

    if (auto currentRide = get_ride(_currentRideIndex); !currentRide || currentRide->type == RIDE_TYPE_MAZE)
    {
        return;
    }

    auto window = static_cast<RideConstructionWindow*>(window_find_by_class(WC_RIDE_CONSTRUCTION));
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
                _currentBrakeSpeed2 = tileElement->AsTrack()->GetBrakeBoosterSpeed();
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
    auto ride = get_ride(_currentRideIndex);
    if (ride == nullptr)
        return;

    auto rideEntry = ride->GetRideEntry();
    if (rideEntry == nullptr)
        return;

    int32_t rideType = RideGetAlternativeType(ride);
    UpdateEnabledRidePieces(rideType);
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
        ride_construction_invalidate_current_track();
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
                direction = direction_reverse(direction);
            gMapSelectArrowPosition = trackPos;
            gMapSelectArrowDirection = direction;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            if (_currentTrackSelectionFlags & TRACK_SELECTION_FLAG_ARROW)
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_ARROW;
            map_invalidate_tile_full(trackPos);
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
                ride_construction_remove_ghosts();
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
            map_invalidate_tile_full(trackPos);
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
void ride_construction_toolupdate_construct(const ScreenCoordsXY& screenCoords)
{
    int32_t z;
    const rct_preview_track* trackBlock;

    map_invalidate_map_selection_tiles();
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
    gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
    auto mapCoords = RideGetPlacePositionFromScreenPosition(screenCoords);
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

    RideId rideIndex;
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
    if (!ride)
    {
        return;
    }

    if (ride->type != RIDE_TYPE_MAZE)
    {
        auto window = static_cast<RideConstructionWindow*>(window_find_by_class(WC_RIDE_CONSTRUCTION));
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
    const auto& ted = GetTrackElementDescriptor(trackType);
    trackBlock = ted.Block;
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

    if (_autoRotatingShop && _rideConstructionState == RideConstructionState::Place
        && ride->GetRideTypeDescriptor().HasFlag(RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY))
    {
        TileElement* pathsByDir[NumOrthogonalDirections];

        bool keepOrientation = false;
        for (int8_t i = 0; i < NumOrthogonalDirections; i++)
        {
            const auto testLoc = CoordsXYZ{ *mapCoords + CoordsDirectionDelta[i], z };
            if (!map_is_location_owned(testLoc))
            {
                pathsByDir[i] = nullptr;
                continue;
            }

            pathsByDir[i] = map_get_footpath_element(testLoc);

            if (pathsByDir[i] != nullptr && (pathsByDir[i])->AsPath()->IsSloped()
                && (pathsByDir[i])->AsPath()->GetSlopeDirection() != i)
            {
                pathsByDir[i] = nullptr;
            }

            // Sloped path on the level below
            if (pathsByDir[i] == nullptr)
            {
                pathsByDir[i] = map_get_footpath_element({ *mapCoords + CoordsDirectionDelta[i], z - PATH_HEIGHT_STEP });

                if (pathsByDir[i] != nullptr
                    && (!(pathsByDir[i])->AsPath()->IsSloped()
                        || (pathsByDir[i])->AsPath()->GetSlopeDirection() != direction_reverse(i)))
                {
                    pathsByDir[i] = nullptr;
                }
            }

            if (pathsByDir[i] != nullptr && (pathsByDir[i])->AsPath()->IsQueue())
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
            for (int8_t i = 0; i < NumOrthogonalDirections; i++)
            {
                if (pathsByDir[i] != nullptr)
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

    rct_window* w;

    map_invalidate_map_selection_tiles();
    ride_construction_invalidate_current_track();

    CoordsXYZ mapCoords{};
    int32_t trackType, z, highestZ;

    if (window_ride_construction_update_state(&trackType, nullptr, nullptr, nullptr, nullptr, nullptr))
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
    auto ridePlacePosition = RideGetPlacePositionFromScreenPosition(screenCoords);
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
        const auto& ted = GetTrackElementDescriptor(_currentTrackPieceType);
        const rct_preview_track* trackBlock = ted.Block;
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
            _rideConstructionState = RideConstructionState::MazeBuild;
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
                _trackPlaceCost = MONEY32_UNDEFINED;
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
                    w = window_find_by_class(WC_RIDE_CONSTRUCTION);
                    if (w != nullptr)
                    {
                        tool_set(*w, WIDX_CONSTRUCT, Tool::Crosshair);
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
        _rideConstructionState = RideConstructionState::Front;
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
            StringId errorText = _trackPlaceErrorMessage;
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

void window_ride_construction_keyboard_shortcut_turn_left()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_LEFT_SMALL:
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_LEFT:
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_VERY_SMALL)
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackCurve)
    {
        case TRACK_CURVE_RIGHT_SMALL:
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_VERY_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_VERY_SMALL);
            }
            break;
        case TRACK_CURVE_RIGHT:
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
            if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE_SMALL)
                && w->widgets[WIDX_LEFT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_SMALL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEFT_CURVE) && w->widgets[WIDX_LEFT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_LEFT_CURVE_LARGE)
                && w->widgets[WIDX_LEFT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEFT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_STRAIGHT);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_LARGE)
                && w->widgets[WIDX_RIGHT_CURVE_LARGE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_LARGE);
            }
            else if (!WidgetIsDisabled(*w, WIDX_RIGHT_CURVE) && w->widgets[WIDX_RIGHT_CURVE].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_SMALL)
                && w->widgets[WIDX_RIGHT_CURVE_SMALL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_RIGHT_CURVE_SMALL);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_RIGHT_CURVE_VERY_SMALL)
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    if (!WidgetIsDisabled(*w, WIDX_STRAIGHT) && w->widgets[WIDX_STRAIGHT].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_STRAIGHT);
    }

    if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_LEVEL);
    }

    if (!WidgetIsDisabled(*w, WIDX_CHAIN_LIFT) && w->widgets[WIDX_CHAIN_LIFT].type != WindowWidgetType::Empty
        && _currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
    {
        window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
    }

    if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
    {
        window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
    }
}

void window_ride_construction_keyboard_shortcut_slope_down()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_DOWN_60:
            if (IsTrackEnabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                && w->widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
                && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                IsTrackEnabled(TRACK_SLOPE_VERTICAL)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
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
            if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
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
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (
                IsTrackEnabled(TRACK_SLOPE_VERTICAL)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
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
            if (IsTrackEnabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_STRAIGHT) || w->widgets[WIDX_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackSlopeEnd)
    {
        case TRACK_SLOPE_UP_60:
            if (IsTrackEnabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_RISE
                && w->widgets[WIDX_SLOPE_DOWN_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN_STEEP);
            }
            break;
        case TRACK_SLOPE_UP_25:
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            break;
        case TRACK_SLOPE_NONE:
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                IsTrackEnabled(TRACK_SLOPE_VERTICAL)
                && w->widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_25:
            if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_60:
            if (!WidgetIsDisabled(*w, WIDX_SLOPE_DOWN) && w->widgets[WIDX_SLOPE_DOWN].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_DOWN);
            }
            else if (!WidgetIsDisabled(*w, WIDX_LEVEL) && w->widgets[WIDX_LEVEL].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_LEVEL);
            }
            else if (!WidgetIsDisabled(*w, WIDX_SLOPE_UP) && w->widgets[WIDX_SLOPE_UP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP);
            }
            else if (
                IsTrackEnabled(TRACK_SLOPE_VERTICAL)
                && w->widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP)
            {
                return;
            }
            else if (
                !WidgetIsDisabled(*w, WIDX_SLOPE_UP_STEEP) && w->widgets[WIDX_SLOPE_UP_STEEP].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_SLOPE_UP_STEEP);
            }
            else
            {
                return;
            }
            break;
        case TRACK_SLOPE_DOWN_90:
            if (IsTrackEnabled(TRACK_SLOPE_VERTICAL) && !WidgetIsDisabled(*w, WIDX_SLOPE_DOWN_STEEP)
                && w->widgets[WIDX_SLOPE_UP_STEEP].image == SPR_RIDE_CONSTRUCTION_VERTICAL_DROP
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_CHAIN_LIFT) || w->widgets[WIDX_CHAIN_LIFT].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_down_call(w, WIDX_CHAIN_LIFT);
}

void window_ride_construction_keyboard_shortcut_bank_left()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT)
        || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!WidgetIsDisabled(*w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_LEFT);
            }
            break;
        case TRACK_BANK_RIGHT:
            if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!WidgetIsDisabled(*w, WIDX_BANK_LEFT) && w->widgets[WIDX_BANK_LEFT].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT)
        || w->widgets[WIDX_BANK_STRAIGHT].type == WindowWidgetType::Empty)
    {
        return;
    }

    switch (_currentTrackBankEnd)
    {
        case TRACK_BANK_NONE:
            if (!WidgetIsDisabled(*w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_RIGHT);
            }
            break;
        case TRACK_BANK_LEFT:
            if (!WidgetIsDisabled(*w, WIDX_BANK_STRAIGHT) && w->widgets[WIDX_BANK_STRAIGHT].type != WindowWidgetType::Empty)
            {
                window_event_mouse_down_call(w, WIDX_BANK_STRAIGHT);
            }
            else if (!WidgetIsDisabled(*w, WIDX_BANK_RIGHT) && w->widgets[WIDX_BANK_RIGHT].type != WindowWidgetType::Empty)
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
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_PREVIOUS_SECTION)
        || w->widgets[WIDX_PREVIOUS_SECTION].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_PREVIOUS_SECTION);
}

void window_ride_construction_keyboard_shortcut_next_track()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_NEXT_SECTION)
        || w->widgets[WIDX_NEXT_SECTION].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_NEXT_SECTION);
}

void window_ride_construction_keyboard_shortcut_build_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_CONSTRUCT) || w->widgets[WIDX_CONSTRUCT].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_CONSTRUCT);
}

void window_ride_construction_keyboard_shortcut_demolish_current()
{
    rct_window* w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    if (w == nullptr || WidgetIsDisabled(*w, WIDX_DEMOLISH) || w->widgets[WIDX_DEMOLISH].type == WindowWidgetType::Empty)
    {
        return;
    }

    window_event_mouse_up_call(w, WIDX_DEMOLISH);
}

static void window_ride_construction_mouseup_demolish_next_piece(const CoordsXYZD& piecePos, int32_t type)
{
    if (gGotoStartPlacementMode)
    {
        _currentTrackBegin.z = floor2(piecePos.z, COORDS_Z_STEP);
        _rideConstructionState = RideConstructionState::Front;
        _currentTrackSelectionFlags = 0;
        _currentTrackPieceDirection = piecePos.direction & 3;
        auto savedCurrentTrackCurve = _currentTrackCurve;
        int32_t savedPreviousTrackSlopeEnd = _previousTrackSlopeEnd;
        int32_t savedCurrentTrackSlopeEnd = _currentTrackSlopeEnd;
        int32_t savedPreviousTrackBankEnd = _previousTrackBankEnd;
        int32_t savedCurrentTrackBankEnd = _currentTrackBankEnd;
        int32_t savedCurrentTrackAlternative = _currentTrackAlternative;
        int32_t savedCurrentTrackLiftHill = _currentTrackLiftHill;
        ride_construction_set_default_next_piece();
        window_ride_construction_update_active_elements();
        auto ride = get_ride(_currentRideIndex);
        if (!ride_try_get_origin_element(ride, nullptr))
        {
            ride_initialise_construction_window(ride);
            _currentTrackPieceDirection = piecePos.direction & 3;
            if (!(savedCurrentTrackCurve & RideConstructionSpecialPieceSelected))
            {
                _currentTrackCurve = savedCurrentTrackCurve;
                _previousTrackSlopeEnd = savedPreviousTrackSlopeEnd;
                _currentTrackSlopeEnd = savedCurrentTrackSlopeEnd;
                _previousTrackBankEnd = savedPreviousTrackBankEnd;
                _currentTrackBankEnd = savedCurrentTrackBankEnd;
                _currentTrackAlternative = savedCurrentTrackAlternative;
                _currentTrackLiftHill = savedCurrentTrackLiftHill;
                window_ride_construction_update_active_elements();
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
        if (network_get_mode() == NETWORK_MODE_CLIENT)
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
            ride_select_next_section();
        }
        else if (_rideConstructionState2 == RideConstructionState::Back)
        {
            ride_select_previous_section();
        }
        window_ride_construction_update_active_elements();
    }
}
