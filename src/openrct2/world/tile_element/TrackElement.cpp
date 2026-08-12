/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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

namespace OpenRCT2
{
    bool TrackElement::hasChain() const
    {
        return flags2.has(TrackTileElementFlag::hasChainLift);
    }

    void TrackElement::setHasChain(bool on)
    {
        flags2.set(TrackTileElementFlag::hasChainLift, on);
    }

    /**
     * Checks if a track element is recognised as the beginning of a block.
     * A beginning of a block can be the end of a station, the end of a lift hill,
     * or a block brake.
     */
    bool TrackElement::isBlockStart() const
    {
        switch (getTrackType())
        {
            case TrackElemType::endStation:
            case TrackElemType::cableLiftHill:
            case TrackElemType::blockBrakes:
            case TrackElemType::diagBlockBrakes:
                return true;
            case TrackElemType::up25ToFlat:
            case TrackElemType::up60ToFlat:
            case TrackElemType::diagUp25ToFlat:
            case TrackElemType::diagUp60ToFlat:
                return hasChain();
            default:
                return false;
        }
    }

    bool TrackElement::isStation() const
    {
        return trackTypeIsStation(getTrackType());
    }

    uint8_t TrackElement::getSeatRotation() const
    {
        const auto* ride = GetRide(getRideIndex());
        if (ride != nullptr && ride->getRideTypeDescriptor().flags.has(RtdFlag::hasLandscapeDoors))
            return kDefaultSeatRotation;

        return uRide.colourScheme >> 4;
    }

    void TrackElement::setSeatRotation(uint8_t newSeatRotation)
    {
        uRide.colourScheme &= ~TRACK_ELEMENT_COLOUR_SEAT_ROTATION_MASK;
        uRide.colourScheme |= (newSeatRotation << 4);
    }

    bool TrackElement::isTakingPhoto() const
    {
        return uRide.onridePhotoBits != 0;
    }

    void TrackElement::setPhotoTimeout()
    {
        uRide.onridePhotoBits = 3;
    }

    void TrackElement::setPhotoTimeout(uint8_t value)
    {
        uRide.onridePhotoBits = value;
    }

    uint8_t TrackElement::getPhotoTimeout() const
    {
        return uRide.onridePhotoBits;
    }

    void TrackElement::decrementPhotoTimeout()
    {
        uRide.onridePhotoBits = std::max(0, uRide.onridePhotoBits - 1);
    }

    uint16_t TrackElement::getMazeEntry() const
    {
        return uMaze.mazeEntry;
    }

    void TrackElement::setMazeEntry(uint16_t newMazeEntry)
    {
        uMaze.mazeEntry = newMazeEntry;
    }

    void TrackElement::mazeEntryAdd(uint16_t addVal)
    {
        uMaze.mazeEntry |= addVal;
    }

    void TrackElement::mazeEntrySubtract(uint16_t subVal)
    {
        uMaze.mazeEntry &= ~subVal;
    }

    TrackElemType TrackElement::getTrackType() const
    {
        return trackType;
    }

    void TrackElement::setTrackType(TrackElemType newType)
    {
        trackType = newType;
    }

    ride_type_t TrackElement::getRideType() const
    {
        return rideType;
    }

    void TrackElement::setRideType(const ride_type_t newRideType)
    {
        rideType = newRideType;
    }

    uint8_t TrackElement::getSequenceIndex() const
    {
        return uRide.sequence;
    }

    void TrackElement::setSequenceIndex(uint8_t newSequenceIndex)
    {
        uRide.sequence = newSequenceIndex;
    }

    StationIndex TrackElement::getStationIndex() const
    {
        return uRide.stationIndex;
    }

    void TrackElement::setStationIndex(StationIndex newStationIndex)
    {
        uRide.stationIndex = newStationIndex;
    }

    uint8_t TrackElement::getDoorAState() const
    {
        return (uRide.colourScheme & TRACK_ELEMENT_COLOUR_DOOR_A_MASK) >> 2;
    }

    uint8_t TrackElement::getDoorBState() const
    {
        return (uRide.colourScheme & TRACK_ELEMENT_COLOUR_DOOR_B_MASK) >> 5;
    }

    void TrackElement::setDoorAState(uint8_t newState)
    {
        uRide.colourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_A_MASK;
        uRide.colourScheme |= ((newState << 2) & TRACK_ELEMENT_COLOUR_DOOR_A_MASK);
    }

    void TrackElement::setDoorBState(uint8_t newState)
    {
        uRide.colourScheme &= ~TRACK_ELEMENT_COLOUR_DOOR_B_MASK;
        uRide.colourScheme |= ((newState << 5) & TRACK_ELEMENT_COLOUR_DOOR_B_MASK);
    }

    RideId TrackElement::getRideIndex() const
    {
        return rideIndex;
    }

    void TrackElement::setRideIndex(RideId newRideIndex)
    {
        rideIndex = newRideIndex;
    }

    uint8_t TrackElement::getColourScheme() const
    {
        return uRide.colourScheme & TRACK_ELEMENT_COLOUR_SCHEME_MASK;
    }

    void TrackElement::setColourScheme(RideColourScheme newColourScheme)
    {
        uRide.colourScheme &= ~TRACK_ELEMENT_COLOUR_SCHEME_MASK;
        uRide.colourScheme |= (EnumValue(newColourScheme) & TRACK_ELEMENT_COLOUR_SCHEME_MASK);
    }

    bool TrackElement::hasCableLift() const
    {
        return flags2.has(TrackTileElementFlag::isCableLift);
    }

    void TrackElement::setHasCableLift(bool on)
    {
        return flags2.set(TrackTileElementFlag::isCableLift, on);
    }

    bool TrackElement::isInverted() const
    {
        return flags2.has(TrackTileElementFlag::inverted);
    }

    void TrackElement::setInverted(bool inverted)
    {
        return flags2.set(TrackTileElementFlag::inverted, inverted);
    }

    bool TrackElement::isBrakeClosed() const
    {
        return flags2.has(TrackTileElementFlag::brakeClosed);
    }

    void TrackElement::setBrakeClosed(bool isClosed)
    {
        return flags2.set(TrackTileElementFlag::brakeClosed, isClosed);
    }

    bool TrackElement::isIndestructible() const
    {
        return flags2.has(TrackTileElementFlag::isIndestructible) && !getGameState().cheats.makeAllDestructible;
    }

    void TrackElement::setIsIndestructible(bool isIndestructible)
    {
        return flags2.set(TrackTileElementFlag::isIndestructible, isIndestructible);
    }

    uint8_t TrackElement::getBrakeBoosterSpeed() const
    {
        return uRide.brakeBoosterSpeed << 1;
    }

    void TrackElement::setBrakeBoosterSpeed(uint8_t speed)
    {
        uRide.brakeBoosterSpeed = (speed >> 1);
    }

    bool TrackElement::hasGreenLight() const
    {
        return flags2.has(TrackTileElementFlag::hasGreenLight);
    }

    void TrackElement::setHasGreenLight(bool on)
    {
        return flags2.set(TrackTileElementFlag::hasGreenLight, on);
    }

    bool TrackElement::isHighlighted() const
    {
        return flags2.has(TrackTileElementFlag::highlight);
    }

    void TrackElement::setHighlight(bool on)
    {
        return flags2.set(TrackTileElementFlag::highlight, on);
    }
} // namespace OpenRCT2
