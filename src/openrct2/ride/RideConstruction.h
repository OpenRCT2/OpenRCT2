/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../world/Location.hpp"
#include "Station.h"
#include "Track.h"

#include <cstdint>
#include <optional>

using track_type_t = uint16_t;

struct TileElement;
struct CoordsXYE;
struct RideTypeDescriptor;
struct TrackDrawerDescriptor;
struct TrackDrawerEntry;

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

extern money64 _currentTrackPrice;

extern uint32_t _currentTrackCurve;
extern RideConstructionState _rideConstructionState;
extern RideId _currentRideIndex;

extern CoordsXYZ _currentTrackBegin;

extern uint8_t _currentTrackPieceDirection;
extern track_type_t _currentTrackPieceType;
extern uint8_t _currentTrackSelectionFlags;
extern uint32_t _rideConstructionNextArrowPulse;
extern TrackPitch _currentTrackPitchEnd;
extern TrackRoll _currentTrackRollEnd;
extern uint8_t _currentTrackLiftHill;
extern uint8_t _currentTrackAlternative;
extern track_type_t _selectedTrackType;

extern TrackRoll _previousTrackRollEnd;
extern TrackPitch _previousTrackPitchEnd;

extern CoordsXYZ _previousTrackPiece;

extern uint8_t _currentBrakeSpeed;
extern uint8_t _currentSeatRotationAngle;

extern CoordsXYZD _unkF440C5;

extern uint8_t gRideEntranceExitPlaceType;
extern RideId gRideEntranceExitPlaceRideIndex;
extern StationIndex gRideEntranceExitPlaceStationIndex;
extern RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8_t gRideEntranceExitPlaceDirection;

void RideEntranceExitPlaceProvisionalGhost();
void RideEntranceExitRemoveGhost();
void RideRestoreProvisionalTrackPiece();
void RideRemoveProvisionalTrackPiece();

void RideConstructionRemoveGhosts();

void RideConstructionInvalidateCurrentTrack();

void RideConstructionSetDefaultNextPiece();

void RideSelectNextSection();
void RideSelectPreviousSection();

bool RideModify(const CoordsXYE& input);

CoordsXYZD RideGetEntranceOrExitPositionFromScreenPosition(const ScreenCoordsXY& screenCoords);

bool RideSelectBackwardsFromFront();
bool RideSelectForwardsFromBack();

void RideConstructionStart(Ride& ride);

TrackDrawerDescriptor getCurrentTrackDrawerDescriptor(const RideTypeDescriptor& rtd);
TrackDrawerEntry getCurrentTrackDrawerEntry(const RideTypeDescriptor& rtd);
