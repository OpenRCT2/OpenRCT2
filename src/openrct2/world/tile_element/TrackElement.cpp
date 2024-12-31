/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackElement.h"

#include "../../GameState.h"
#include "../../ride/RideData.h"
#include "../../ride/Track.h"

using namespace OpenRCT2;

bool TrackElement::HasChain() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
}

void TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_CHAIN_LIFT;
    }
}

/**
 * Checks if a track element is recognised as the beginning of a block.
 * A beginning of a block can be the end of a station, the end of a lift hill,
 * or a block brake.
 */
bool TrackElement::IsBlockStart() const
{
    switch (GetTrackType())
    {
        case TrackElemType::EndStation:
        case TrackElemType::CableLiftHill:
        case TrackElemType::BlockBrakes:
        case TrackElemType::DiagBlockBrakes:
            return true;
        case TrackElemType::Up25ToFlat:
        case TrackElemType::Up60ToFlat:
        case TrackElemType::DiagUp25ToFlat:
        case TrackElemType::DiagUp60ToFlat:
            return HasChain();
        default:
            return false;
    }
}

bool TrackElement::IsStation() const
{
    return TrackTypeIsStation(GetTrackType());
}

uint8_t TrackElement::GetSeatRotation() const
{
    const auto* ride = GetRide(GetRideIndex());
    if (ride != nullptr && ride->GetRideTypeDescriptor().HasFlag(RtdFlag::hasLandscapeDoors))
        return DEFAULT_SEAT_ROTATION;

    return URide.ColourScheme >> 4;
}

void TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK;
    URide.ColourScheme |= (newSeatRotation << 4);
}

bool TrackElement::IsTakingPhoto() const
{
    return URide.OnridePhotoBits != 0;
}

void TrackElement::SetPhotoTimeout()
{
    URide.OnridePhotoBits = 3;
}

void TrackElement::SetPhotoTimeout(uint8_t value)
{
    URide.OnridePhotoBits = value;
}

uint8_t TrackElement::GetPhotoTimeout() const
{
    return URide.OnridePhotoBits;
}

void TrackElement::DecrementPhotoTimeout()
{
    URide.OnridePhotoBits = std::max(0, URide.OnridePhotoBits - 1);
}

uint16_t TrackElement::GetMazeEntry() const
{
    return UMaze.MazeEntry;
}

void TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    UMaze.MazeEntry = newMazeEntry;
}

void TrackElement::MazeEntryAdd(uint16_t addVal)
{
    UMaze.MazeEntry |= addVal;
}

void TrackElement::MazeEntrySubtract(uint16_t subVal)
{
    UMaze.MazeEntry &= ~subVal;
}

OpenRCT2::TrackElemType TrackElement::GetTrackType() const
{
    return TrackType;
}

void TrackElement::SetTrackType(OpenRCT2::TrackElemType newType)
{
    TrackType = newType;
}

ride_type_t TrackElement::GetRideType() const
{
    return RideType;
}

void TrackElement::SetRideType(const ride_type_t rideType)
{
    RideType = rideType;
}

uint8_t TrackElement::GetSequenceIndex() const
{
    return URide.Sequence;
}

void TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    URide.Sequence = newSequenceIndex;
}

StationIndex TrackElement::GetStationIndex() const
{
    return URide.stationIndex;
}

void TrackElement::SetStationIndex(StationIndex newStationIndex)
{
    URide.stationIndex = newStationIndex;
}

uint8_t TrackElement::GetDoorAState() const
{
    return (URide.ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_A_MASK) >> 2;
}

uint8_t TrackElement::GetDoorBState() const
{
    return (URide.ColourScheme & TRACK_ELEMENT_COLOUR_DOOR_B_MASK) >> 5;
}

void TrackElement::SetDoorAState(uint8_t newState)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_A_MASK;
    URide.ColourScheme |= ((newState << 2) & TRACK_ELEMENT_COLOUR_DOOR_A_MASK);
}

void TrackElement::SetDoorBState(uint8_t newState)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_B_MASK;
    URide.ColourScheme |= ((newState << 5) & TRACK_ELEMENT_COLOUR_DOOR_B_MASK);
}

RideId TrackElement::GetRideIndex() const
{
    return RideIndex;
}

void TrackElement::SetRideIndex(RideId newRideIndex)
{
    RideIndex = newRideIndex;
}

uint8_t TrackElement::GetColourScheme() const
{
    return URide.ColourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK;
}

void TrackElement::SetColourScheme(RideColourScheme newColourScheme)
{
    URide.ColourScheme &= ~TRACK_ELEMENT_COLOUR_SCHEME_MASK;
    URide.ColourScheme |= (EnumValue(newColourScheme) & TRACK_ELEMENT_COLOUR_SCHEME_MASK);
}

bool TrackElement::HasCableLift() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

void TrackElement::SetHasCableLift(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_CABLE_LIFT;
}

bool TrackElement::IsInverted() const
{
    return Flags2 & TRACK_ELEMENT_FLAGS2_INVERTED;
}

void TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INVERTED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INVERTED;
    }
}

bool TrackElement::IsBrakeClosed() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED) != 0;
}

void TrackElement::SetBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED;
    }
}

bool TrackElement::IsIndestructible() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE) != 0 && !GetGameState().Cheats.makeAllDestructible;
}

void TrackElement::SetIsIndestructible(bool isIndestructible)
{
    if (isIndestructible)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
    else
    {
        Flags2 &= ~TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE;
    }
}

uint8_t TrackElement::GetBrakeBoosterSpeed() const
{
    return URide.BrakeBoosterSpeed << 1;
}

void TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    URide.BrakeBoosterSpeed = (speed >> 1);
}

bool TrackElement::HasGreenLight() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT) != 0;
}

void TrackElement::SetHasGreenLight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    if (on)
    {
        Flags2 |= TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT;
    }
}

bool TrackElement::IsHighlighted() const
{
    return (Flags2 & TRACK_ELEMENT_FLAGS2_HIGHLIGHT);
}

void TrackElement::SetHighlight(bool on)
{
    Flags2 &= ~TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
    if (on)
        Flags2 |= TRACK_ELEMENT_FLAGS2_HIGHLIGHT;
}
