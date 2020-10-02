/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../Cheats.h"
#include "../Context.h"
#include "../Game.h"
#include "../Input.h"
#include "../actions/TrackPlaceAction.hpp"
#include "../audio/audio.h"
#include "../interface/Viewport.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../peep/Staff.h"
#include "../ride/RideData.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/Banner.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "Intent.h"

#include <iterator>
#include <tuple>

bool gDisableErrorWindowSound = false;

uint64_t _enabledRidePieces;
uint8_t _rideConstructionState2;

// This variable is updated separately from ride->num_stations because the latter
// is unreliable if currently in station construction mode
bool _stationConstructed;
bool _deferClose;

/**
 *
 *  rct2: 0x006CA162
 */
money32 place_provisional_track_piece(
    ride_id_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndAlternativeState,
    const CoordsXYZ& trackPos)
{
    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return MONEY32_UNDEFINED;

    money32 result;
    ride_construction_remove_ghosts();
    if (ride->type == RIDE_TYPE_MAZE)
    {
        int32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND
            | GAME_COMMAND_FLAG_GHOST; // 105
        result = maze_set_track(trackPos.x, trackPos.y, trackPos.z, flags, true, 0, rideIndex, GC_SET_MAZE_TRACK_BUILD);
        if (result == MONEY32_UNDEFINED)
            return result;

        _unkF440C5 = { trackPos, static_cast<Direction>(trackDirection) };
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        viewport_set_visibility(3);
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        if (!scenery_tool_is_active())
        {
            // Set new virtual floor height.
            virtual_floor_set_height(trackPos.z);
        }

        return result;
    }
    else
    {
        auto trackPlaceAction = TrackPlaceAction(
            rideIndex, trackType, { trackPos, static_cast<uint8_t>(trackDirection) }, 0, 0, 0, liftHillAndAlternativeState,
            false);
        trackPlaceAction.SetFlags(GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
        // This command must not be sent over the network
        auto res = GameActions::Execute(&trackPlaceAction);
        auto tpar = dynamic_cast<TrackPlaceActionResult*>(res.get());
        result = ((tpar == nullptr) || (res->Error == GA_ERROR::OK)) ? res->Cost : MONEY32_UNDEFINED;
        if (result == MONEY32_UNDEFINED)
            return result;

        int16_t z_begin, z_end;
        const rct_track_coordinates* coords = get_track_coord_from_ride(ride, trackType);
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            z_begin = coords->z_begin;
            z_end = coords->z_end;
        }
        else
        {
            z_end = z_begin = coords->z_begin;
        }

        _unkF440C5 = { trackPos.x, trackPos.y, trackPos.z + z_begin, static_cast<Direction>(trackDirection) };
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        if (tpar != nullptr)
        {
            viewport_set_visibility((tpar->GroundFlags & ELEMENT_IS_UNDERGROUND) ? 1 : 3);
        }
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        if (!scenery_tool_is_active())
        {
            // Set height to where the next track piece would begin
            virtual_floor_set_height(trackPos.z - z_begin + z_end);
        }

        return result;
    }
}

static std::tuple<bool, uint8_t> window_ride_construction_update_state_get_track_element()
{
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES);
    context_broadcast_intent(&intent);

    uint8_t startSlope = _previousTrackSlopeEnd;
    uint8_t endSlope = _currentTrackSlopeEnd;
    uint8_t startBank = _previousTrackBankEnd;
    uint8_t endBank = _currentTrackBankEnd;

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        startSlope = _currentTrackSlopeEnd;
        endSlope = _previousTrackSlopeEnd;
        startBank = _currentTrackBankEnd;
        endBank = _previousTrackBankEnd;
    }

    uint16_t curve = _currentTrackCurve;
    if (curve == 0xFFFF)
    {
        return std::make_tuple(false, 0);
    }

    bool startsDiagonal = (_currentTrackPieceDirection & (1 << 2)) != 0;
    if (curve == TRACK_CURVE_LEFT_LARGE || curve == TRACK_CURVE_RIGHT_LARGE)
    {
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
        {
            startsDiagonal = !startsDiagonal;
        }
    }

    if (curve <= 8)
    {
        for (uint32_t i = 0; i < std::size(gTrackDescriptors); i++)
        {
            const track_descriptor* trackDescriptor = &gTrackDescriptors[i];

            if (trackDescriptor->track_curve != curve)
                continue;
            if (trackDescriptor->starts_diagonal != startsDiagonal)
                continue;
            if (trackDescriptor->slope_start != startSlope)
                continue;
            if (trackDescriptor->slope_end != endSlope)
                continue;
            if (trackDescriptor->bank_start != startBank)
                continue;
            if (trackDescriptor->bank_end != endBank)
                continue;

            return std::make_tuple(true, trackDescriptor->track_element);
        }

        return std::make_tuple(false, 0);
    }

    switch (curve & 0xFF)
    {
        case TrackElemType::EndStation:
        case TrackElemType::SBendLeft:
        case TrackElemType::SBendRight:
            if (startSlope != TRACK_SLOPE_NONE || endSlope != TRACK_SLOPE_NONE)
            {
                return std::make_tuple(false, 0);
            }

            if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE)
            {
                return std::make_tuple(false, 0);
            }

            return std::make_tuple(true, curve & 0xFF);

        case TrackElemType::LeftVerticalLoop:
        case TrackElemType::RightVerticalLoop:
            if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE)
            {
                return std::make_tuple(false, 0);
            }

            if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
            {
                if (endSlope != TRACK_SLOPE_DOWN_25)
                {
                    return std::make_tuple(false, 0);
                }
            }
            else
            {
                if (startSlope != TRACK_SLOPE_UP_25)
                {
                    return std::make_tuple(false, 0);
                }
            }

            return std::make_tuple(true, curve & 0xFF);

        default:
            return std::make_tuple(true, curve & 0xFF);
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
bool window_ride_construction_update_state(
    int32_t* _trackType, int32_t* _trackDirection, ride_id_t* _rideIndex, int32_t* _liftHillAndInvertedState,
    CoordsXYZ* _trackPos, int32_t* _properties)
{
    ride_id_t rideIndex;
    uint8_t trackType, trackDirection;
    uint16_t x, y, liftHillAndInvertedState, properties;

    auto updated_element = window_ride_construction_update_state_get_track_element();
    if (!std::get<0>(updated_element))
    {
        return true;
    }

    trackType = std::get<1>(updated_element);
    liftHillAndInvertedState = 0;
    rideIndex = _currentRideIndex;
    if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED)
    {
        liftHillAndInvertedState |= CONSTRUCTION_LIFT_HILL_SELECTED;
    }

    if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE)
    {
        liftHillAndInvertedState |= CONSTRUCTION_INVERTED_TRACK_SELECTED;
    }

    auto ride = get_ride(rideIndex);
    if (ride == nullptr)
        return true;

    if (_enabledRidePieces & (1ULL << TRACK_SLOPE_STEEP_LONG))
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

            case TrackElemType::DiagFlatToUp60:
            case TrackElemType::DiagUp60ToFlat:
            case TrackElemType::DiagFlatToDown60:
            case TrackElemType::DiagDown60ToFlat:
                return true;
        }
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES)
        && _currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_PIECES)
    {
        auto availablePieces = RideTypeDescriptors[ride->type].CoveredTrackPieces;
        auto alternativeType = AlternativeTrackTypes[trackType];
        if (alternativeType != -1 && availablePieces & (1ULL << trackType))
        {
            trackType = static_cast<uint8_t>(alternativeType);
            liftHillAndInvertedState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }

    const rct_track_coordinates* trackCoordinates = get_track_coord_from_ride(ride, trackType);

    x = _currentTrackBegin.x;
    y = _currentTrackBegin.y;
    auto z = _currentTrackBegin.z;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK)
    {
        z -= trackCoordinates->z_end;
        trackDirection = _currentTrackPieceDirection ^ 0x02;
        trackDirection -= trackCoordinates->rotation_end;
        trackDirection += trackCoordinates->rotation_begin;
        trackDirection &= 0x03;

        if (trackCoordinates->rotation_begin & (1 << 2))
        {
            trackDirection |= 0x04;
        }

        CoordsXY offsets = { trackCoordinates->x, trackCoordinates->y };
        CoordsXY coords = { x, y };
        coords += offsets.Rotate(direction_reverse(trackDirection));
        x = static_cast<uint16_t>(coords.x);
        y = static_cast<uint16_t>(coords.y);
    }
    else
    {
        z -= trackCoordinates->z_begin;
        trackDirection = _currentTrackPieceDirection;
    }

    bool turnOffLiftHill = false;
    if (!(_enabledRidePieces & (1ULL << TRACK_LIFT_HILL_CURVE)))
    {
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT)
        {
            turnOffLiftHill = true;
        }
    }

    if (!(TrackFlags[trackType] & TRACK_ELEM_FLAG_ALLOW_LIFT_HILL))
    {
        turnOffLiftHill = true;
    }

    if (turnOffLiftHill && !gCheatsEnableChainLiftOnAllTrack)
    {
        liftHillAndInvertedState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;

        if (trackType == TrackElemType::LeftCurvedLiftHill || trackType == TrackElemType::RightCurvedLiftHill)
        {
            liftHillAndInvertedState |= CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }

    if (TrackTypeHasSpeedSetting(trackType))
    {
        properties = _currentBrakeSpeed2;
    }
    else
    {
        properties = _currentSeatRotationAngle << 12;
    }

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

void window_ride_construction_do_entrance_exit_check()
{
    auto w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    auto ride = get_ride(_currentRideIndex);
    if (w == nullptr || ride == nullptr)
    {
        return;
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0)
    {
        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != nullptr)
        {
            if (!ride_are_all_possible_entrances_and_exits_built(ride))
            {
                window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
            }
            else
            {
                _deferClose = true;
            }
        }
    }
}

void window_ride_construction_do_station_check()
{
    auto ride = get_ride(_currentRideIndex);
    if (ride != nullptr)
    {
        _stationConstructed = ride->num_stations != 0;
    }
}

void window_ride_construction_mouseup_demolish_next_piece(const CoordsXYZD& piecePos, int32_t type)
{
    if (gGotoStartPlacementMode)
    {
        _currentTrackBegin.z = floor2(piecePos.z, COORDS_Z_STEP);
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _currentTrackPieceDirection = piecePos.direction & 3;
        int32_t savedCurrentTrackCurve = _currentTrackCurve;
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
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_SELECTED
            || _rideConstructionState2 == RIDE_CONSTRUCTION_STATE_FRONT)
        {
            if (type == TrackElemType::MiddleStation || type == TrackElemType::BeginStation)
            {
                type = TrackElemType::EndStation;
            }
        }
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_BACK)
        {
            if (type == TrackElemType::MiddleStation)
            {
                type = TrackElemType::BeginStation;
            }
        }
        if (network_get_mode() == NETWORK_MODE_CLIENT)
        {
            // rideConstructionState needs to be set again to the proper value, this only affects the client
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        }
        _currentTrackBegin = piecePos;
        _currentTrackPieceDirection = piecePos.direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_FRONT)
        {
            ride_select_next_section();
        }
        else if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_BACK)
        {
            ride_select_previous_section();
        }
        window_ride_construction_update_active_elements();
    }
}

/**
 *
 *  rct2: 0x006C84CE
 */
void window_ride_construction_update_active_elements()
{
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS);
    context_broadcast_intent(&intent);
}

/**
 *
 *  rct2: 0x0066DB3D
 */
bool scenery_tool_is_active()
{
    int32_t toolWindowClassification = gCurrentToolWidget.window_classification;
    rct_widgetindex toolWidgetIndex = gCurrentToolWidget.widget_index;
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (toolWindowClassification == WC_TOP_TOOLBAR && toolWidgetIndex == WC_TOP_TOOLBAR__WIDX_SCENERY)
            return true;

    return false;
}

void init_scenery()
{
    auto intent = Intent(INTENT_ACTION_INIT_SCENERY);
    context_broadcast_intent(&intent);
}

void scenery_set_default_placement_configuration()
{
    auto intent = Intent(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
    context_broadcast_intent(&intent);
}
