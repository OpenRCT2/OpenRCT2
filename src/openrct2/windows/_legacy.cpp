/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../audio/audio.h"
#include "../Cheats.h"
#include "../Context.h"
#include "../core/Util.hpp"
#include "../Game.h"
#include "../Input.h"
#include "../interface/Viewport.h"
#include "../network/network.h"
#include "../paint/VirtualFloor.h"
#include "../peep/Staff.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../world/Banner.h"
#include "../world/Scenery.h"
#include "../world/Sprite.h"
#include "Intent.h"

#include <tuple>

bool gDisableErrorWindowSound = false;

void game_command_callback_pickup_guest(
    int32_t eax,
    int32_t ebx,
    int32_t ecx,
    [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi,
    [[maybe_unused]] int32_t edi,
    [[maybe_unused]] int32_t ebp)
{
    switch (ecx)
    {
    case 0:
    {
        int32_t peepnum = eax;
        rct_window * w = window_find_by_number(WC_PEEP, peepnum);
        if (w)
        {
            tool_set(w, WC_PEEP__WIDX_PICKUP, TOOL_PICKER);
        }
    }
        break;
    case 2:
        if (ebx == 0)
        {
            tool_cancel();
            gPickupPeepImage = UINT32_MAX;
        }
        break;
    }
}

void game_command_callback_hire_new_staff_member(
    [[maybe_unused]] int32_t eax,
    [[maybe_unused]] int32_t ebx,
    [[maybe_unused]] int32_t ecx,
    [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi,
    int32_t edi,
    [[maybe_unused]] int32_t ebp)
{
    int32_t sprite_index = edi;
    if (sprite_index == SPRITE_INDEX_NULL)
    {
        rct_window * window = window_find_by_class(WC_STAFF_LIST);
        window_invalidate(window);
    }
    else
    {
        rct_peep * peep = &get_sprite(sprite_index)->peep;
        auto intent = Intent(WC_PEEP);
        intent.putExtra(INTENT_EXTRA_PEEP, peep);
        context_open_intent(&intent);
    }
}

void game_command_callback_pickup_staff(
    int32_t eax,
    int32_t ebx,
    int32_t ecx,
    [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi,
    [[maybe_unused]] int32_t edi,
    [[maybe_unused]] int32_t ebp)
{
    switch (ecx)
    {
    case 0:
    {
        int32_t peepnum = eax;
        rct_window * w = window_find_by_number(WC_PEEP, peepnum);
        if (w)
        {
            tool_set(w, WC_STAFF__WIDX_PICKUP, TOOL_PICKER);
        }
    }
        break;
    case 2:
        if (ebx == 0)
        {
            tool_cancel();
            gPickupPeepImage = UINT32_MAX;
        }
        break;
    }
}

uint64_t _enabledRidePieces;
uint8_t _rideConstructionState2;

// This variable is updated separately from ride->num_stations because the latter
// is unreliable if currently in station construction mode
bool _stationConstructed;
bool _deferClose;

void game_command_callback_place_ride_entrance_or_exit(
    [[maybe_unused]] int32_t eax,
    [[maybe_unused]] int32_t ebx,
    [[maybe_unused]] int32_t ecx,
    [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi,
    [[maybe_unused]] int32_t edi,
    [[maybe_unused]] int32_t ebp)
{
    audio_play_sound_at_location(
        SOUND_PLACE_ITEM,
        gCommandPosition.x,
        gCommandPosition.y,
        gCommandPosition.z
    );

    Ride *ride = get_ride(gRideEntranceExitPlaceRideIndex);
    if (ride_are_all_possible_entrances_and_exits_built(ride)) {
        tool_cancel();
        if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK)) {
            window_close_by_class(WC_RIDE_CONSTRUCTION);
        }
    } else {
        gRideEntranceExitPlaceType ^= 1;
        gCurrentToolWidget.widget_index = (gRideEntranceExitPlaceType == ENTRANCE_TYPE_RIDE_ENTRANCE) ?
                                          WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE : WC_RIDE_CONSTRUCTION__WIDX_EXIT;
    }
}

/**
 *
 *  rct2: 0x006CA162
 */
money32 place_provisional_track_piece(int32_t rideIndex, int32_t trackType, int32_t trackDirection, int32_t liftHillAndAlternativeState, int32_t x, int32_t y, int32_t z)
{
    Ride *ride;
    money32 result;

    ride_construction_remove_ghosts();
    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE)
    {
        int32_t flags = GAME_COMMAND_FLAG_APPLY | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_5 | GAME_COMMAND_FLAG_GHOST; // 105
        result = maze_set_track(x, y, z, flags, true, 0, rideIndex, GC_SET_MAZE_TRACK_BUILD);
        if (result == MONEY32_UNDEFINED)
            return result;

        _unkF440C5.x = x;
        _unkF440C5.y = y;
        _unkF440C5.z = z;
        _unkF440C5.direction = trackDirection;
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        viewport_set_visibility((gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) ? 1 : 3);
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        if (!scenery_tool_is_active())
        {
            // Set new virtual floor height.
            virtual_floor_set_height(z);
        }

        return result;
    }
    else
    {
        result = game_do_command(x, 105 | (trackDirection << 8), y, rideIndex | (trackType << 8) | (liftHillAndAlternativeState << 16), GAME_COMMAND_PLACE_TRACK, z, 0);
        if (result == MONEY32_UNDEFINED)
            return result;

        int16_t z_begin, z_end;
        const rct_track_coordinates * coords = get_track_coord_from_ride(ride, trackType);
        if (!ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_HAS_NO_TRACK))
        {
            z_begin = coords->z_begin;
            z_end = coords->z_end;
        }
        else
        {
            z_end = z_begin = coords->z_begin;
        }

        _unkF440C5.x = x;
        _unkF440C5.y = y;
        z += z_begin;

        _unkF440C5.z = z;
        _unkF440C5.direction = trackDirection;
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        viewport_set_visibility((gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) ? 1 : 3);
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        // Invalidate previous track piece (we may not be changing height!)
        virtual_floor_invalidate();

        if (!scenery_tool_is_active())
        {
            // Set height to where the next track piece would begin
            virtual_floor_set_height(z - z_begin + z_end);
        }

        return result;
    }
}

static std::tuple<bool, uint8_t> window_ride_construction_update_state_get_track_element() {
    auto intent = Intent(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES);
    context_broadcast_intent(&intent);

    uint8_t startSlope = _previousTrackSlopeEnd;
    uint8_t endSlope = _currentTrackSlopeEnd;
    uint8_t startBank = _previousTrackBankEnd;
    uint8_t endBank = _currentTrackBankEnd;

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
        startSlope = _currentTrackSlopeEnd;
        endSlope = _previousTrackSlopeEnd;
        startBank = _currentTrackBankEnd;
        endBank = _previousTrackBankEnd;
    }

    uint16_t curve = _currentTrackCurve;
    if (curve == 0xFFFF) {
        return std::make_tuple(false, 0);
    }

    bool startsDiagonal = (_currentTrackPieceDirection & (1 << 2)) != 0;
    if (curve == TRACK_CURVE_LEFT_LARGE || curve == TRACK_CURVE_RIGHT_LARGE) {
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
            startsDiagonal = !startsDiagonal;
        }
    }

    if (curve <= 8)
    {
        for (uint32_t i = 0; i < Util::CountOf(gTrackDescriptors); i++)
        {
            const track_descriptor * trackDescriptor = &gTrackDescriptors[i];

            if (trackDescriptor->track_curve != curve) continue;
            if (trackDescriptor->starts_diagonal != startsDiagonal) continue;
            if (trackDescriptor->slope_start != startSlope) continue;
            if (trackDescriptor->slope_end != endSlope) continue;
            if (trackDescriptor->bank_start != startBank) continue;
            if (trackDescriptor->bank_end != endBank) continue;

            return std::make_tuple(true, trackDescriptor->track_element);
        }

        return std::make_tuple(false, 0);
    }

    switch (curve & 0xFF) {
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_RIGHT:
        if (startSlope != TRACK_SLOPE_NONE || endSlope != TRACK_SLOPE_NONE) {
            return std::make_tuple(false, 0);
        }

        if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE) {
            return std::make_tuple(false, 0);
        }

        return std::make_tuple(true, curve & 0xFF);

    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE) {
            return std::make_tuple(false, 0);
        }

        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
            if (endSlope != TRACK_SLOPE_DOWN_25) {
                return std::make_tuple(false, 0);
            }
        } else {
            if (startSlope != TRACK_SLOPE_UP_25) {
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
 * @param[out] _liftHillAndAlternativeState (liftHillAndAlternativeState)
 * @param[out] _x (ax)
 * @param[out] _y (cx)
 * @param[out] _z (di)
 * @param[out] _properties (edirs16)
 * @return (CF)
 */
bool window_ride_construction_update_state(int32_t *_trackType, int32_t *_trackDirection, int32_t *_rideIndex, int32_t *_liftHillAndAlternativeState, int32_t *_x, int32_t *_y, int32_t *_z, int32_t *_properties) {
    uint8_t trackType, trackDirection, rideIndex;
    uint16_t z, x, y, liftHillAndAlternativeState, properties;

    auto updated_element = window_ride_construction_update_state_get_track_element();
    if (!std::get<0>(updated_element)) {
        return true;
    }

    trackType = std::get<1>(updated_element);
    liftHillAndAlternativeState = 0;
    rideIndex = _currentRideIndex;
    if (_currentTrackLiftHill & CONSTRUCTION_LIFT_HILL_SELECTED) {
        liftHillAndAlternativeState |= CONSTRUCTION_LIFT_HILL_SELECTED;
    }

    if (_currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_TYPE) {
        liftHillAndAlternativeState |= RIDE_TYPE_ALTERNATIVE_TRACK_TYPE;
    }

    Ride *ride = get_ride(rideIndex);

    if (_enabledRidePieces & (1ULL << TRACK_SLOPE_STEEP_LONG)) {
        switch (trackType) {
        case TRACK_ELEM_FLAT_TO_60_DEG_UP:
            trackType = TRACK_ELEM_FLAT_TO_60_DEG_UP_LONG_BASE;
            break;

        case TRACK_ELEM_60_DEG_UP_TO_FLAT:
            trackType = TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE;
            break;

        case TRACK_ELEM_FLAT_TO_60_DEG_DOWN:
            trackType = TRACK_ELEM_60_DEG_UP_TO_FLAT_LONG_BASE_122;
            break;

        case TRACK_ELEM_60_DEG_DOWN_TO_FLAT:
            trackType = TRACK_ELEM_FLAT_TO_60_DEG_DOWN_LONG_BASE;
            break;

        case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_UP:
        case TRACK_ELEM_DIAG_60_DEG_UP_TO_FLAT:
        case TRACK_ELEM_DIAG_FLAT_TO_60_DEG_DOWN:
        case TRACK_ELEM_DIAG_60_DEG_DOWN_TO_FLAT:
            return true;
        }
    }

    if (ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_TRACK_ELEMENTS_HAVE_TWO_VARIETIES) && _currentTrackAlternative & RIDE_TYPE_ALTERNATIVE_TRACK_PIECES) {
        if (ride->type != RIDE_TYPE_WATER_COASTER || trackType == TRACK_ELEM_FLAT || trackType == TRACK_ELEM_LEFT_QUARTER_TURN_5_TILES || trackType == TRACK_ELEM_RIGHT_QUARTER_TURN_5_TILES) {
            int16_t alternativeType = AlternativeTrackTypes[trackType];
            if (alternativeType > -1) {
                trackType = (uint8_t) alternativeType;
            }
            liftHillAndAlternativeState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }

    const rct_track_coordinates *trackCoordinates = get_track_coord_from_ride(ride, trackType);

    x = _currentTrackBeginX;
    y = _currentTrackBeginY;
    z = _currentTrackBeginZ;
    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
        z -= trackCoordinates->z_end;
        trackDirection = _currentTrackPieceDirection ^ 0x02;
        trackDirection -= trackCoordinates->rotation_end;
        trackDirection += trackCoordinates->rotation_begin;
        trackDirection &= 0x03;

        if (trackCoordinates->rotation_begin & (1 << 2)) {
            trackDirection |= 0x04;
        }

        switch (trackDirection & 0x03) {
        case 0:
            x -= trackCoordinates->x;
            y -= trackCoordinates->y;
            break;

        case 1:
            x -= trackCoordinates->y;
            y += trackCoordinates->x;
            break;

        case 2:
            x += trackCoordinates->x;
            y += trackCoordinates->y;
            break;

        case 3:
            x += trackCoordinates->y;
            y -= trackCoordinates->x;
            break;
        }
    } else {
        z -= trackCoordinates->z_begin;
        trackDirection = _currentTrackPieceDirection;
    }


    bool turnOffLiftHill = false;
    if (!(_enabledRidePieces & (1ULL << TRACK_LIFT_HILL_CURVE))) {
        if (TrackFlags[trackType] & TRACK_ELEM_FLAG_CURVE_ALLOWS_LIFT) {
            turnOffLiftHill = true;
        }
    }

    if (!(TrackFlags[trackType] & TRACK_ELEM_FLAG_ALLOW_LIFT_HILL)) {
        turnOffLiftHill = true;
    }

    if (turnOffLiftHill && !gCheatsEnableChainLiftOnAllTrack) {
        liftHillAndAlternativeState &= ~CONSTRUCTION_LIFT_HILL_SELECTED;
        _currentTrackLiftHill &= ~CONSTRUCTION_LIFT_HILL_SELECTED;

        if (trackType == TRACK_ELEM_LEFT_CURVED_LIFT_HILL || trackType == TRACK_ELEM_RIGHT_CURVED_LIFT_HILL) {
            liftHillAndAlternativeState |= CONSTRUCTION_LIFT_HILL_SELECTED;
        }
    }


    if (track_element_has_speed_setting(trackType)) {
        properties = _currentBrakeSpeed2;
    } else {
        properties = _currentSeatRotationAngle << 12;
    }


    if (_trackType != NULL) *_trackType = trackType;
    if (_trackDirection != NULL) *_trackDirection = trackDirection;
    if (_rideIndex != NULL) *_rideIndex = rideIndex;
    if (_liftHillAndAlternativeState != NULL) *_liftHillAndAlternativeState = liftHillAndAlternativeState;
    if (_x != NULL) *_x = x;
    if (_y != NULL) *_y = y;
    if (_z != NULL) *_z = z;
    if (_properties != NULL) *_properties = properties;

    return false;
}

void window_ride_construction_do_entrance_exit_check()
{
    rct_window *w = window_find_by_class(WC_RIDE_CONSTRUCTION);
    Ride *ride = get_ride(_currentRideIndex);

    if (w == NULL || ride == NULL) {
        return;
    }

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_0) {
        w = window_find_by_class(WC_RIDE_CONSTRUCTION);
        if (w != NULL) {
            if (!ride_are_all_possible_entrances_and_exits_built(ride)) {
                window_event_mouse_up_call(w, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
            } else {
                _deferClose = true;
            }
        }
    }
}

void window_ride_construction_do_station_check()
{
    Ride *ride = get_ride(_currentRideIndex);
    if (ride != NULL) {
        _stationConstructed = ride->num_stations != 0;
    }
}

void window_ride_construction_mouseup_demolish_next_piece(int32_t x, int32_t y, int32_t z, int32_t direction, int32_t type)
{
    if (gGotoStartPlacementMode) {
        z &= 0xFFF0;
        _currentTrackBeginZ = z;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        _currentTrackPieceDirection = direction & 3;
        int32_t slope = _currentTrackCurve;
        int32_t slopeEnd = _previousTrackSlopeEnd;
        int32_t b2 = _currentTrackSlopeEnd;
        int32_t bankEnd = _previousTrackBankEnd;
        int32_t bankStart = _currentTrackBankEnd;
        int32_t b5 = _currentTrackAlternative;
        int32_t b4 = _currentTrackLiftHill;
        ride_construction_set_default_next_piece();
        window_ride_construction_update_active_elements();
        if (!ride_try_get_origin_element(_currentRideIndex, NULL)) {
            ride_initialise_construction_window(_currentRideIndex);
            _currentTrackPieceDirection = direction & 3;
            if (!(slope & 0x100)) {
                _currentTrackCurve = slope;
                _previousTrackSlopeEnd = slopeEnd;
                _currentTrackSlopeEnd = b2;
                _previousTrackBankEnd = bankEnd;
                _currentTrackBankEnd = bankStart;
                _currentTrackAlternative = b5;
                _currentTrackLiftHill = b4;
                window_ride_construction_update_active_elements();
            }
        }
    }
    else {
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_SELECTED ||
            _rideConstructionState2 == RIDE_CONSTRUCTION_STATE_FRONT
            ) {
            if (type == TRACK_ELEM_MIDDLE_STATION || type == TRACK_ELEM_BEGIN_STATION) {
                type = TRACK_ELEM_END_STATION;
            }
        }
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_BACK) {
            if (type == TRACK_ELEM_MIDDLE_STATION) {
                type = TRACK_ELEM_BEGIN_STATION;
            }
        }
        if (network_get_mode() == NETWORK_MODE_CLIENT)
        {
            // rideConstructionState needs to be set again to the proper value, this only affects the client
            _rideConstructionState = RIDE_CONSTRUCTION_STATE_SELECTED;
        }
        _currentTrackBeginX = x;
        _currentTrackBeginY = y;
        _currentTrackBeginZ = z;
        _currentTrackPieceDirection = direction;
        _currentTrackPieceType = type;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_FRONT) {
            ride_select_next_section();
        }
        else if (_rideConstructionState2 == RIDE_CONSTRUCTION_STATE_BACK) {
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

void game_command_callback_place_banner(
    [[maybe_unused]] int32_t eax,
    int32_t ebx,
    [[maybe_unused]] int32_t ecx,
    [[maybe_unused]] int32_t edx,
    [[maybe_unused]] int32_t esi,
    int32_t edi,
    [[maybe_unused]] int32_t ebp)
{
    if (ebx != MONEY32_UNDEFINED) {
        int32_t bannerId = edi;

        audio_play_sound_at_location(SOUND_PLACE_ITEM, gCommandPosition.x, gCommandPosition.y, gCommandPosition.z);
        context_open_detail_window(WD_BANNER, bannerId);
    }
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
