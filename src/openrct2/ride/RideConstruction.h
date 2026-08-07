/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../Identifiers.h"
#include "../actions/CommandFlag.h"
#include "../core/FlagHolder.hpp"
#include "../core/Money.hpp"

#include <cstdint>
#include <optional>

struct CoordsXYE;
struct CoordsXYZ;
struct CoordsXYZD;

namespace OpenRCT2::GameActions
{
    enum class RideSetSetting : uint8_t;
}

enum class RideColourScheme : uint8_t;

namespace OpenRCT2
{
    struct Ride;
    struct RideTypeDescriptor;
    struct TrackDrawerDescriptor;
    struct TrackDrawerEntry;
    struct TileElement;

    enum class TrackElemType : uint16_t;

    namespace TrackMetadata
    {
        enum class TrackCurve : uint8_t;
        enum class TrackPitch : uint8_t;
        enum class TrackRoll : uint8_t;
        struct TypeOrCurve;
    } // namespace TrackMetadata

    enum class TrackElementSetFlag : uint8_t
    {
        highlightOff,
        highlightOn,
        colourScheme,
        cableLiftOn,
        cableLiftOff,
        seatRotation,
        brakeClosed,
        brakeBoosterSpeed,
    };
    using TrackElementSetFlags = FlagHolder<uint16_t, TrackElementSetFlag>;

    enum class TrackSelectionFlag : uint8_t
    {
        arrow,
        track,
        entranceOrExit,
        recheck,
        trackPlaceActionQueued,
    };
    using TrackSelectionFlags = FlagHolder<uint8_t, TrackSelectionFlag>;

    enum class RideConstructionState : uint8_t
    {
        state0,
        front,
        back,
        selected,
        place,
        entranceExit,
        mazeBuild,
        mazeMove,
        mazeFill
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

    extern money64 _currentTrackPrice;

    extern TrackMetadata::TypeOrCurve _currentlySelectedTrack;
    extern RideConstructionState _rideConstructionState;
    extern RideId _currentRideIndex;

    extern CoordsXYZ _currentTrackBegin;

    extern uint8_t _currentTrackPieceDirection;
    extern TrackElemType _currentTrackPieceType;
    extern TrackSelectionFlags _currentTrackSelectionFlags;
    extern uint32_t _rideConstructionNextArrowPulse;
    extern TrackMetadata::TrackPitch _currentTrackPitchEnd;
    extern TrackMetadata::TrackRoll _currentTrackRollEnd;
    extern bool _currentTrackHasLiftHill;
    extern SelectedAlternative _currentTrackAlternative;
    extern TrackElemType _selectedTrackType;

    extern TrackMetadata::TrackRoll _previousTrackRollEnd;
    extern TrackMetadata::TrackPitch _previousTrackPitchEnd;

    extern CoordsXYZ _previousTrackPiece;

    extern uint8_t _currentBrakeSpeed;
    extern RideColourScheme _currentColourScheme;
    extern uint8_t _currentSeatRotationAngle;

    extern CoordsXYZD _unkF440C5;

    extern uint8_t gRideEntranceExitPlaceType;
    extern RideId gRideEntranceExitPlaceRideIndex;
    extern StationIndex gRideEntranceExitPlaceStationIndex;
    extern RideConstructionState gRideEntranceExitPlacePreviousRideConstructionState;
    extern uint8_t gRideEntranceExitPlaceDirection;

    void RideEntranceExitPlaceProvisionalGhost();
    void RideEntranceExitRemoveGhost();

    void RideConstructionRemoveGhosts();

    void RideConstructionInvalidateCurrentTrack();

    void RideConstructionSetDefaultNextPiece();

    void RideSelectNextSection();
    void RideSelectPreviousSection();

    bool RideModify(const CoordsXYE& input);

    money64 SetOperatingSetting(RideId rideId, GameActions::RideSetSetting setting, uint8_t value);
    money64 SetOperatingSettingNested(
        RideId rideId, GameActions::RideSetSetting setting, uint8_t value, GameActions::CommandFlags flags);

    bool RideSelectBackwardsFromFront();
    bool RideSelectForwardsFromBack();

    void RideConstructionStart(Ride& ride);

    TrackDrawerDescriptor getCurrentTrackDrawerDescriptor(const RideTypeDescriptor& rtd);
    TrackDrawerEntry getCurrentTrackDrawerEntry(const RideTypeDescriptor& rtd);
    TrackElemType GetTrackTypeFromCurve(
        TrackMetadata::TrackCurve curve, bool startsDiagonal, TrackMetadata::TrackPitch startSlope,
        TrackMetadata::TrackPitch endSlope, TrackMetadata::TrackRoll startBank, TrackMetadata::TrackRoll endBank);

    std::optional<CoordsXYZ> GetTrackElementOriginAndApplyChanges(
        const CoordsXYZD& location, TrackElemType type, uint16_t extra_params, TileElement** output_element,
        TrackElementSetFlags flags);
} // namespace OpenRCT2
