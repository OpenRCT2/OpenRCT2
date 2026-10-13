/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../Identifiers.h"
#include "../../core/FlagHolder.hpp"
#include "TileElementBase.h"

using ride_type_t = uint16_t;
enum class RideColourScheme : uint8_t;

namespace OpenRCT2
{
    enum class TrackElemType : uint16_t;

    enum class TrackTileElementFlag : uint8_t
    {
        hasChainLift,
        inverted,
        // Used for giga coaster
        isCableLift,
        highlight,
        hasGreenLight,
        brakeClosed,
        isIndestructible,
    };
    using TrackTileElementFlags = FlagHolder<uint8_t, TrackTileElementFlag>;

    constexpr uint8_t kTrackElementColourSchemeMask = 0b00000011;
    // Not colour related, but shares the field.
    constexpr uint8_t kTrackElementColourDoorAMask = 0b00011100;
    constexpr uint8_t kTrackElementColourDoorBMask = 0b11100000;
    constexpr uint8_t kTrackElementColourSeatRotationMask = 0b11110000;

    constexpr int32_t kLandEdgeDoorFrameClosed = 0;
    constexpr int32_t kLandEdgeDoorFrameOpening = 1;
    constexpr int32_t kLandEdgeDoorFrameOpen = 3;
    constexpr int32_t kLandEdgeDoorFrameClosing = 4;
    constexpr int32_t kLandEdgeDoorFrameEnd = 6;
    constexpr int32_t kLandEdgeDoorFrameCount = 8;

#pragma pack(push, 1)

    struct TrackElement : TileElementBase
    {
        static constexpr TileElementType kElementType = TileElementType::track;

    private:
        TrackElemType trackType;
        union
        {
            struct
            {
                uint8_t sequence;
                uint8_t colourScheme;
                union
                {
                    // - Bits 3 and 4 are never set
                    // - Bits 1 and 2 are set when a vehicle triggers the on-ride photo and act like a countdown from 3.
                    // - If any of the bits 1-4 are set, the game counts it as a photo being taken.
                    uint8_t onridePhotoBits;
                    // Contains the brake/booster speed, divided by 2.
                    uint8_t brakeBoosterSpeed;
                };
                StationIndex stationIndex;
            } uRide;
            struct
            {
                uint16_t mazeEntry; // 6
            } uMaze;
        };
        TrackTileElementFlags flags2;
        RideId rideIndex;
        ride_type_t rideType;

    public:
        TrackElemType getTrackType() const;
        void setTrackType(TrackElemType newType);

        ride_type_t getRideType() const;
        void setRideType(ride_type_t rideType);

        uint8_t getSequenceIndex() const;
        void setSequenceIndex(uint8_t newSequenceIndex);

        RideId getRideIndex() const;
        void setRideIndex(RideId newRideIndex);

        uint8_t getColourScheme() const;
        void setColourScheme(RideColourScheme newColourScheme);

        StationIndex getStationIndex() const;
        void setStationIndex(StationIndex newStationIndex);

        bool hasChain() const;
        void setHasChain(bool on);

        bool hasCableLift() const;
        void setHasCableLift(bool on);

        bool isInverted() const;
        void setInverted(bool inverted);

        bool isBrakeClosed() const;
        void setBrakeClosed(bool isClosed);

        bool isIndestructible() const;
        void setIsIndestructible(bool isIndestructible);

        uint8_t getBrakeBoosterSpeed() const;
        void setBrakeBoosterSpeed(uint8_t speed);

        bool hasGreenLight() const;
        void setHasGreenLight(bool on);

        uint8_t getSeatRotation() const;
        void setSeatRotation(uint8_t newSeatRotation);

        uint16_t getMazeEntry() const;
        void setMazeEntry(uint16_t newMazeEntry);
        void mazeEntryAdd(uint16_t addVal);
        void mazeEntrySubtract(uint16_t subVal);

        bool isTakingPhoto() const;
        void setPhotoTimeout();
        void setPhotoTimeout(uint8_t newValue);
        void decrementPhotoTimeout();
        uint8_t getPhotoTimeout() const;

        bool isHighlighted() const;
        void setHighlight(bool on);

        // Used by ghost train, RCT1 feature, will be reintroduced at some point.
        // (See https://github.com/OpenRCT2/OpenRCT2/issues/7059)
        uint8_t getDoorAState() const;
        uint8_t getDoorBState() const;
        void setDoorAState(uint8_t newState);
        void setDoorBState(uint8_t newState);

        bool isStation() const;
        bool isBlockStart() const;
    };
    static_assert(sizeof(TrackElement) == kTileElementSize);

#pragma pack(pop)
} // namespace OpenRCT2
