#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "_legacy.h"
#include "../interface/window.h"
#include "../peep/Staff.h"
#include "Intent.h"
#include "../Context.h"
#include "../audio/audio.h"
#include "../Input.h"
#include "../Game.h"
#include "../interface/viewport.h"
#include "../ride/Track.h"
#include "../ride/TrackData.h"
#include "../cheats.h"
#include "../network/network.h"
#include "../world/scenery.h"

#pragma warning(disable : 4295) // 'identifier': array is too small to include a terminating null character

bool gDisableErrorWindowSound = false;

/** rct2: 0x0098DA74 */
const rct_object_entry RequiredSelectedObjects[7] = {
        // Objects that are always required
        { 0x00000087, { "SCGTREES" }, 0 },      // Scenery: Trees
        { 0x00000087, { "SCGSHRUB" }, 0 },      // Scenery: Shrubs and Ornaments
        { 0x00000087, { "SCGGARDN" }, 0 },      // Scenery: Gardens
        { 0x00000087, { "SCGFENCE" }, 0 },      // Scenery: Fences and Walls
        { 0x00000087, { "SCGWALLS" }, 0 },      // Scenery: Walls and Roofs
        { 0x00000087, { "SCGPATHX" }, 0 },      // Scenery: Signs and Items for Footpaths
        { 0x00000085, { "TARMAC  " }, 0 },      // Footpath: Tarmac
};

const rct_object_entry DefaultSelectedObjects[26] = {
        // An initial default selection
        { 0x00000080, { "TWIST1  " }, 0 },      // Ride: Twist
        { 0x00000080, { "PTCT1   " }, 0 },      // Ride: Wooden Roller Coaster (Wooden Roller Coaster Trains)
        { 0x00000080, { "ZLDB    " }, 0 },      // Ride: Junior Roller Coaster (Ladybird Trains)
        { 0x00000080, { "LFB1    " }, 0 },      // Ride: Log Flume
        { 0x00000080, { "VCR     " }, 0 },      // Ride: Vintage Cars
        { 0x00000080, { "MGR1    " }, 0 },      // Ride: Merry-Go-Round
        { 0x00000080, { "TLT1    " }, 0 },      // Ride: Restroom
        { 0x00000080, { "ATM1    " }, 0 },      // Ride: Cash Machine
        { 0x00000080, { "FAID1   " }, 0 },      // Ride: First Aid Room
        { 0x00000080, { "INFOK   " }, 0 },      // Ride: Information Kiosk
        { 0x00000080, { "DRNKS   " }, 0 },      // Ride: Drinks Stall
        { 0x00000080, { "CNDYF   " }, 0 },      // Ride: Candyfloss Stall
        { 0x00000080, { "BURGB   " }, 0 },      // Ride: Burger Bar
        { 0x00000080, { "BALLN   " }, 0 },      // Ride: Balloon Stall
        { 0x00000080, { "ARRT1   " }, 0 },      // Ride: Corkscrew Roller Coaster
        { 0x00000080, { "RBOAT   " }, 0 },      // Ride: Rowing Boats
        { 0x00000088, { "PKENT1  " }, 0 },      // Park Entrance: Traditional Park Entrance
        { 0x00000089, { "WTRCYAN " }, 0 },      // Water: Natural Water
        { 0x00000085, { "TARMACB " }, 0 },      // Footpath: Brown Tarmac Footpath
        { 0x00000085, { "PATHSPCE" }, 0 },      // Footpath: Space Style Footpath
        { 0x00000085, { "PATHDIRT" }, 0 },      // Footpath: Dirt Footpath
        { 0x00000085, { "PATHCRZY" }, 0 },      // Footpath: Crazy Paving Footpath
        { 0x00000085, { "PATHASH " }, 0 },      // Footpath: Ash Footpath

        // The following are for all random map generation features to work out the box
        { 0x00000087, { "SCGJUNGL" }, 0 },      // Jungle Theming
        { 0x00000087, { "SCGSNOW " }, 0 },      // Snow and Ice Theming
        { 0x00000087, { "SCGWATER" }, 0 }       // Water Feature Theming
};

void game_command_callback_pickup_guest(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    switch (ecx)
    {
    case 0:
    {
        sint32 peepnum = eax;
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

void game_command_callback_hire_new_staff_member(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    sint32 sprite_index = edi;
    if (sprite_index == SPRITE_INDEX_NULL)
    {
        rct_window * window = window_find_by_class(WC_STAFF_LIST);
        window_invalidate(window);
    }
    else
    {
        rct_peep * peep = &get_sprite(sprite_index)->peep;
        Intent * intent = intent_create(WC_PEEP);
        intent_set_pointer(intent, INTENT_EXTRA_PEEP, peep);
        context_open_intent(intent);
        intent_release(intent);
    }
}

void game_command_callback_pickup_staff(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    switch (ecx)
    {
    case 0:
    {
        sint32 peepnum = eax;
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

uint64 _enabledRidePieces;
uint8 _rideConstructionState2;

// This variable is updated separately from ride->num_stations because the latter
// is unreliable if currently in station construction mode
bool _stationConstructed;
bool _deferClose;

void game_command_callback_place_ride_entrance_or_exit(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
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
money32 place_provisional_track_piece(sint32 rideIndex, sint32 trackType, sint32 trackDirection, sint32 liftHillAndAlternativeState, sint32 x, sint32 y, sint32 z)
{
    Ride *ride;
    money32 result;

    ride_construction_remove_ghosts();
    ride = get_ride(rideIndex);
    if (ride->type == RIDE_TYPE_MAZE) {
        result = game_do_command(x, 105 | (4 << 8), y, rideIndex | (trackType << 8) | (liftHillAndAlternativeState << 16), GAME_COMMAND_SET_MAZE_TRACK, z, 0);
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

        return result;
    } else {
        result = game_do_command(x, 105 | (trackDirection << 8), y, rideIndex | (trackType << 8) | (liftHillAndAlternativeState << 16), GAME_COMMAND_PLACE_TRACK, z, 0);
        if (result == MONEY32_UNDEFINED)
            return result;

        _unkF440C5.x = x;
        _unkF440C5.y = y;
        z += ride_type_has_flag(ride->type, RIDE_TYPE_FLAG_FLAT_RIDE) ?
             FlatTrackCoordinates[trackType].z_begin:
             TrackCoordinates[trackType].z_begin;

        _unkF440C5.z = z;
        _unkF440C5.direction = trackDirection;
        _currentTrackSelectionFlags |= TRACK_SELECTION_FLAG_TRACK;
        viewport_set_visibility((gTrackGroundFlags & TRACK_ELEMENT_LOCATION_IS_UNDERGROUND) ? 1 : 3);
        if (_currentTrackSlopeEnd != 0)
            viewport_set_visibility(2);

        return result;
    }
}

static bool sub_6CA2DF_get_track_element(uint8 *trackElement) {
    Intent * intent = intent_create(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_PIECES);
    context_broadcast_intent(intent);
    intent_release(intent);

    uint8 startSlope = _previousTrackSlopeEnd;
    uint8 endSlope = _currentTrackSlopeEnd;
    uint8 startBank = _previousTrackBankEnd;
    uint8 endBank = _currentTrackBankEnd;

    if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
        startSlope = _currentTrackSlopeEnd;
        endSlope = _previousTrackSlopeEnd;
        startBank = _currentTrackBankEnd;
        endBank = _previousTrackBankEnd;
    }

    uint16 curve = _currentTrackCurve;
    if (curve == 0xFFFF) {
        return false;
    }

    bool startsDiagonal = (_currentTrackPieceDirection & (1 << 2)) != 0;
    if (curve == TRACK_CURVE_LEFT_LARGE || curve == TRACK_CURVE_RIGHT_LARGE) {
        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
            startsDiagonal = !startsDiagonal;
        }
    }

    if (curve <= 8) {
        for (int i = 0; i < countof(gTrackDescriptors); i++)
        {
            const track_descriptor * trackDescriptor = &gTrackDescriptors[i];

            if (trackDescriptor->track_curve != curve) continue;
            if (trackDescriptor->starts_diagonal != startsDiagonal) continue;
            if (trackDescriptor->slope_start != startSlope) continue;
            if (trackDescriptor->slope_end != endSlope) continue;
            if (trackDescriptor->bank_start != startBank) continue;
            if (trackDescriptor->bank_end != endBank) continue;

            *trackElement = trackDescriptor->track_element;
            return true;
        }

        return false;
    }

    *trackElement = curve & 0xFF;
    switch (*trackElement) {
    case TRACK_ELEM_END_STATION:
    case TRACK_ELEM_S_BEND_LEFT:
    case TRACK_ELEM_S_BEND_RIGHT:
        if (startSlope != TRACK_SLOPE_NONE || endSlope != TRACK_SLOPE_NONE) {
            return false;
        }

        if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE) {
            return false;
        }

        return true;

    case TRACK_ELEM_LEFT_VERTICAL_LOOP:
    case TRACK_ELEM_RIGHT_VERTICAL_LOOP:
        if (startBank != TRACK_BANK_NONE || endBank != TRACK_BANK_NONE) {
            return false;
        }

        if (_rideConstructionState == RIDE_CONSTRUCTION_STATE_BACK) {
            if (endSlope != TRACK_SLOPE_DOWN_25) {
                return false;
            }
        } else {
            if (startSlope != TRACK_SLOPE_UP_25) {
                return false;
            }
        }

        return true;

    default:
        return true;
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
bool sub_6CA2DF(sint32 *_trackType, sint32 *_trackDirection, sint32 *_rideIndex, sint32 *_liftHillAndAlternativeState, sint32 *_x, sint32 *_y, sint32 *_z, sint32 *_properties) {
    uint8 trackType, trackDirection, rideIndex;
    uint16 z, x, y, liftHillAndAlternativeState, properties;

    if (!sub_6CA2DF_get_track_element(&trackType)) {
        return true;
    }

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
            sint16 alternativeType = AlternativeTrackTypes[trackType];
            if (alternativeType > -1) {
                trackType = (uint8) alternativeType;
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

void window_ride_construction_mouseup_demolish_next_piece(sint32 x, sint32 y, sint32 z, sint32 direction, sint32 type)
{
    if (gGotoStartPlacementMode) {
        z &= 0xFFF0;
        _currentTrackBeginZ = z;
        _rideConstructionState = RIDE_CONSTRUCTION_STATE_FRONT;
        _currentTrackSelectionFlags = 0;
        _rideConstructionArrowPulseTime = 0;
        direction = _currentTrackPieceDirection;
        sint32 slope = _currentTrackCurve;
        sint32 slopeEnd = _previousTrackSlopeEnd;
        sint32 b2 = _currentTrackSlopeEnd;
        sint32 bankEnd = _previousTrackBankEnd;
        sint32 bankStart = _currentTrackBankEnd;
        sint32 b5 = _currentTrackAlternative;
        sint32 b4 = _currentTrackLiftHill;
        ride_construction_set_default_next_piece();
        window_ride_construction_update_active_elements();
        if (!ride_try_get_origin_element(_currentRideIndex, NULL)) {
            ride_initialise_construction_window(_currentRideIndex);
            _currentTrackPieceDirection = direction;
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
    Intent * intent = intent_create(INTENT_ACTION_RIDE_CONSTRUCTION_UPDATE_ACTIVE_ELEMENTS);
    context_broadcast_intent(intent);
    intent_release(intent);
}

void game_command_callback_place_banner(sint32 eax, sint32 ebx, sint32 ecx, sint32 edx, sint32 esi, sint32 edi, sint32 ebp)
{
    if (ebx != MONEY32_UNDEFINED) {
        sint32 bannerId = edi;

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
    sint32 toolWindowClassification = gCurrentToolWidget.window_classification;
    rct_widgetindex toolWidgetIndex = gCurrentToolWidget.widget_index;
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (toolWindowClassification == WC_TOP_TOOLBAR && toolWidgetIndex == WC_TOP_TOOLBAR__WIDX_SCENERY)
            return true;

    return false;
}

void init_scenery()
{
    Intent * intent = intent_create(INTENT_ACTION_INIT_SCENERY);
    context_broadcast_intent(intent);
    intent_release(intent);
}

void scenery_set_default_placement_configuration()
{
    Intent * intent = intent_create(INTENT_ACTION_SET_DEFAULT_SCENERY_CONFIG);
    context_broadcast_intent(intent);
    intent_release(intent);
}
