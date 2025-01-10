/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "TileElementBase.h"

using ride_type_t = uint16_t;
enum class RideColourScheme : uint8_t;

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;
}

enum
{
    TRACK_ELEMENT_FLAGS2_CHAIN_LIFT = 1 << 0,
    TRACK_ELEMENT_FLAGS2_INVERTED = 1 << 1,
    // Used for giga coaster
    TRACK_ELEMENT_FLAGS2_CABLE_LIFT = 1 << 2,
    TRACK_ELEMENT_FLAGS2_HIGHLIGHT = 1 << 3,
    TRACK_ELEMENT_FLAGS2_HAS_GREEN_LIGHT = 1 << 4,
    TRACK_ELEMENT_FLAGS2_BRAKE_CLOSED = 1 << 5,
    TRACK_ELEMENT_FLAGS2_INDESTRUCTIBLE_TRACK_PIECE = 1 << 6,
};

enum
{
    TRACK_ELEMENT_COLOUR_SCHEME_MASK = 0b00000011,
    // Not colour related, but shares the field.
    TRACK_ELEMENT_COLOUR_DOOR_A_MASK = 0b00011100,
    TRACK_ELEMENT_COLOUR_DOOR_B_MASK = 0b11100000,
    TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK = 0b11110000,
};

enum
{
    LANDSCAPE_DOOR_CLOSED = 0,
    LANDSCAPE_DOOR_HALF_OPEN = 2,
    LANDSCAPE_DOOR_OPEN = 3,
};

#pragma pack(push, 1)

struct TrackElement : TileElementBase
{
    static constexpr TileElementType kElementType = TileElementType::Track;

private:
    OpenRCT2::TrackElemType TrackType;
    union
    {
        struct
        {
            uint8_t Sequence;
            uint8_t ColourScheme;
            union
            {
                // - Bits 3 and 4 are never set
                // - Bits 1 and 2 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
                // - If any of the bits 1-4 are set, the game counts it as a photo being taken.
                uint8_t OnridePhotoBits;
                // Contains the brake/booster speed, divided by 2.
                uint8_t BrakeBoosterSpeed;
            };
            StationIndex stationIndex;
        } URide;
        struct
        {
            uint16_t MazeEntry; // 6
        } UMaze;
    };
    uint8_t Flags2;
    RideId RideIndex;
    ride_type_t RideType;

public:
    OpenRCT2::TrackElemType GetTrackType() const;
    void SetTrackType(OpenRCT2::TrackElemType newEntryIndex);

    ride_type_t GetRideType() const;
    void SetRideType(const ride_type_t rideType);

    uint8_t GetSequenceIndex() const;
    void SetSequenceIndex(uint8_t newSequenceIndex);

    RideId GetRideIndex() const;
    void SetRideIndex(RideId newRideIndex);

    uint8_t GetColourScheme() const;
    void SetColourScheme(RideColourScheme newColourScheme);

    StationIndex GetStationIndex() const;
    void SetStationIndex(StationIndex newStationIndex);

    bool HasChain() const;
    void SetHasChain(bool on);

    bool HasCableLift() const;
    void SetHasCableLift(bool on);

    bool IsInverted() const;
    void SetInverted(bool inverted);

    bool IsBrakeClosed() const;
    void SetBrakeClosed(bool isClosed);

    bool IsIndestructible() const;
    void SetIsIndestructible(bool isIndestructible);

    uint8_t GetBrakeBoosterSpeed() const;
    void SetBrakeBoosterSpeed(uint8_t speed);

    bool HasGreenLight() const;
    void SetHasGreenLight(bool on);

    uint8_t GetSeatRotation() const;
    void SetSeatRotation(uint8_t newSeatRotation);

    uint16_t GetMazeEntry() const;
    void SetMazeEntry(uint16_t newMazeEntry);
    void MazeEntryAdd(uint16_t addVal);
    void MazeEntrySubtract(uint16_t subVal);

    bool IsTakingPhoto() const;
    void SetPhotoTimeout();
    void SetPhotoTimeout(uint8_t newValue);
    void DecrementPhotoTimeout();
    uint8_t GetPhotoTimeout() const;

    bool IsHighlighted() const;
    void SetHighlight(bool on);

    // Used by ghost train, RCT1 feature, will be reintroduced at some point.
    // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
    uint8_t GetDoorAState() const;
    uint8_t GetDoorBState() const;
    void SetDoorAState(uint8_t newState);
    void SetDoorBState(uint8_t newState);

    bool IsStation() const;
    bool IsBlockStart() const;
};
static_assert(sizeof(TrackElement) == kTileElementSize);

#pragma pack(pop)
