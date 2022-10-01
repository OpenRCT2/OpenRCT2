/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../common.h"
#include "../world/Location.hpp"
#include "Station.h"

#include <cstdint>
#include <optional>

using track_type_t = uint16_t;

struct TileElement;
struct CoordsXYE;

enum class RideConstructionState : uint8_t
{
    State0,
    Front,
    Back,
    Selected,
    Place,
    EntranceExit,
    MazeBuild,
    MazeMove,
    MazeFill
};

extern money32 _currentTrackPrice;

extern uint32_t _currentTrackCurve;
extern RideConstructionState _rideConstructionState;
extern RideId _currentRideIndex;

extern CoordsXYZ _currentTrackBegin;

extern uint8_t _currentTrackPieceDirection;
extern track_type_t _currentTrackPieceType;
extern uint8_t _currentTrackSelectionFlags;
extern uint32_t _rideConstructionNextArrowPulse;
extern uint8_t _currentTrackSlopeEnd;
extern uint8_t _currentTrackBankEnd;
extern uint8_t _currentTrackLiftHill;
extern uint8_t _currentTrackAlternative;
extern track_type_t _selectedTrackType;

extern uint8_t _previousTrackBankEnd;
extern uint8_t _previousTrackSlopeEnd;

extern CoordsXYZ _previousTrackPiece;

extern uint8_t _currentBrakeSpeed2;
extern uint8_t _currentSeatRotationAngle;

extern CoordsXYZD _unkF440C5;

extern uint8_t gRideEntranceExitPlaceType;
extern RideId gRideEntranceExitPlaceRideIndex;
extern StationIndex gRideEntranceExitPlaceStationIndex;
extern RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8_t gRideEntranceExitPlaceDirection;

std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
    const CoordsXYZD& location, track_type_t type, uint16_t extra_params, TileElement** output_element, uint16_t flags);

void ride_entrance_exit_place_provisional_ghost();
void ride_entrance_exit_remove_ghost();
void ride_restore_provisional_track_piece();
void ride_remove_provisional_track_piece();

void ride_construction_remove_ghosts();

void ride_construction_invalidate_current_track();

void ride_construction_set_default_next_piece();

void ride_select_next_section();
void ride_select_previous_section();

bool ride_modify(const CoordsXYE& input);

CoordsXYZD ride_get_entrance_or_exit_position_from_screen_position(const ScreenCoordsXY& screenCoords);

bool ride_select_backwards_from_front();
bool ride_select_forwards_from_back();
