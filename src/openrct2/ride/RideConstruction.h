/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"
#include "../world/Location.hpp"
#include "RideColour.h"
#include "Station.h"
#include "Track.h"

#include <cstdint>
#include <optional>

struct TileElement;
struct CoordsXYE;
struct RideTypeDescriptor;
struct TrackDrawerDescriptor;
struct TrackDrawerEntry;

namespace OpenRCT2
{
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

    enum class AlternativeTrackFlag : uint8_t
    {
        alternativePieces, // Dinghy slide and Water Coaster
        inverted,          // Flying RC, Lay-down RC, Multi-dimension RC
    };
    using SelectedAlternative = FlagHolder<uint8_t, AlternativeTrackFlag>;

    enum class LiftHillAndInverted : uint8_t
    {
        liftHill,
        inverted,
    };
    using SelectedLiftAndInverted = FlagHolder<uint32_t, LiftHillAndInverted>;
} // namespace OpenRCT2

extern money64 _currentTrackPrice;

extern TypeOrCurve _currentlySelectedTrack;
extern OpenRCT2::RideConstructionState _rideConstructionState;
extern RideId _currentRideIndex;

extern CoordsXYZ _currentTrackBegin;

extern uint8_t _currentTrackPieceDirection;
extern OpenRCT2::TrackElemType _currentTrackPieceType;
extern uint8_t _currentTrackSelectionFlags;
extern uint32_t _rideConstructionNextArrowPulse;
extern TrackPitch _currentTrackPitchEnd;
extern TrackRoll _currentTrackRollEnd;
extern bool _currentTrackHasLiftHill;
extern OpenRCT2::SelectedAlternative _currentTrackAlternative;
extern OpenRCT2::TrackElemType _selectedTrackType;

extern TrackRoll _previousTrackRollEnd;
extern TrackPitch _previousTrackPitchEnd;

extern CoordsXYZ _previousTrackPiece;

extern uint8_t _currentBrakeSpeed;
extern RideColourScheme _currentColourScheme;
extern uint8_t _currentSeatRotationAngle;

extern CoordsXYZD _unkF440C5;

extern uint8_t gRideEntranceExitPlaceType;
extern RideId gRideEntranceExitPlaceRideIndex;
extern StationIndex gRideEntranceExitPlaceStationIndex;
extern OpenRCT2::RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
extern uint8_t gRideEntranceExitPlaceDirection;

void RideEntranceExitPlaceProvisionalGhost();
void RideEntranceExitRemoveGhost();

void RideConstructionRemoveGhosts();

void RideConstructionInvalidateCurrentTrack();

void RideConstructionSetDefaultNextPiece();

void RideSelectNextSection();
void RideSelectPreviousSection();

bool RideModify(const CoordsXYE& input);

bool RideSelectBackwardsFromFront();
bool RideSelectForwardsFromBack();

void RideConstructionStart(Ride& ride);

TrackDrawerDescriptor getCurrentTrackDrawerDescriptor(const RideTypeDescriptor& rtd);
TrackDrawerEntry getCurrentTrackDrawerEntry(const RideTypeDescriptor& rtd);
OpenRCT2::TrackElemType GetTrackTypeFromCurve(
    TrackCurve curve, bool startsDiagonal, TrackPitch startSlope, TrackPitch endSlope, TrackRoll startBank, TrackRoll endBank);
