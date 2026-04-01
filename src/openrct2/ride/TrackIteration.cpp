#include "TrackIteration.h"

#include "../ui/WindowManager.h"
#include "../world/Map.h"
#include "../world/tile_element/TileElement.h"
#include "../world/tile_element/TrackElement.h"
#include "Ride.h"
#include "RideData.h"
#include "Track.h"
#include "TrackData.h"
#include "ted/TrackElementDescriptor.h"

namespace OpenRCT2
{
    void trackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first)
    {
        it->last = first;
        it->first = nullptr;
        it->firstIteration = true;
        it->looped = false;
    }

    bool trackCircuitIteratorPrevious(TrackCircuitIterator* it)
    {
        TrackBeginEnd trackBeginEnd;

        if (it->first == nullptr)
        {
            if (!trackBlockGetPrevious({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
                return false;

            it->current.x = trackBeginEnd.begin_x;
            it->current.y = trackBeginEnd.begin_y;
            it->current.element = trackBeginEnd.begin_element;
            it->currentZ = trackBeginEnd.begin_z;
            it->currentDirection = trackBeginEnd.begin_direction;

            it->first = it->current.element;
            return true;
        }

        if (!it->firstIteration && it->first == it->current.element)
        {
            it->looped = true;
            return false;
        }

        it->firstIteration = false;
        it->last = it->current;

        if (trackBlockGetPrevious({ it->last.x, it->last.y, it->last.element }, &trackBeginEnd))
        {
            it->current.x = trackBeginEnd.end_x;
            it->current.y = trackBeginEnd.end_y;
            it->current.element = trackBeginEnd.begin_element;
            it->currentZ = trackBeginEnd.begin_z;
            it->currentDirection = trackBeginEnd.begin_direction;
            return true;
        }

        return false;
    }

    bool trackCircuitIteratorNext(TrackCircuitIterator* it)
    {
        if (it->first == nullptr)
        {
            if (!trackBlockGetNext(&it->last, &it->current, &it->currentZ, &it->currentDirection))
                return false;

            it->first = it->current.element;
            return true;
        }

        if (!it->firstIteration && it->first == it->current.element)
        {
            it->looped = true;
            return false;
        }

        it->firstIteration = false;
        it->last = it->current;
        return trackBlockGetNext(&it->last, &it->current, &it->currentZ, &it->currentDirection);
    }

    bool trackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt)
    {
        return (
            firstIt->currentZ == secondIt->currentZ && firstIt->currentDirection == secondIt->currentDirection
            && firstIt->current.x == secondIt->current.x && firstIt->current.y == secondIt->current.y);
    }

    void trackGetBack(const CoordsXYE& input, CoordsXYE* output)
    {
        CoordsXYE lastTrack = input;
        TrackBeginEnd currentTrack;
        bool result;

        do
        {
            result = trackBlockGetPrevious(lastTrack, &currentTrack);
            if (result)
            {
                lastTrack.x = currentTrack.begin_x;
                lastTrack.y = currentTrack.begin_y;
                lastTrack.element = currentTrack.begin_element;
            }
        } while (result);
        *output = lastTrack;
    }

    void trackGetFront(const CoordsXYE& input, CoordsXYE* output)
    {
        CoordsXYE lastTrack, currentTrack;
        int32_t z, direction;
        bool result;

        lastTrack = input;
        do
        {
            result = trackBlockGetNext(&lastTrack, &currentTrack, &z, &direction);
            if (result)
            {
                lastTrack = currentTrack;
            }
        } while (result);
        *output = lastTrack;
    }

    TrackElement* trackGetPreviousBlock(CoordsXYZ& location, TileElement* tileElement)
    {
        CoordsXYZ startLocation = location;
        TrackBeginEnd trackBeginEnd, slowTrackBeginEnd;
        TileElement slowTileElement = *tileElement;
        bool counter = true;
        CoordsXY slowLocation = location;
        do
        {
            if (!trackBlockGetPrevious({ location, tileElement }, &trackBeginEnd))
            {
                return nullptr;
            }
            if (trackBeginEnd.begin_x == startLocation.x && trackBeginEnd.begin_y == startLocation.y
                && tileElement == trackBeginEnd.begin_element)
            {
                return nullptr;
            }

            location.x = trackBeginEnd.end_x;
            location.y = trackBeginEnd.end_y;
            location.z = trackBeginEnd.begin_z;
            tileElement = trackBeginEnd.begin_element;

            // #2081: prevent infinite loop
            counter = !counter;
            if (counter)
            {
                trackBlockGetPrevious({ slowLocation, &slowTileElement }, &slowTrackBeginEnd);
                slowLocation.x = slowTrackBeginEnd.end_x;
                slowLocation.y = slowTrackBeginEnd.end_y;
                slowTileElement = *(slowTrackBeginEnd.begin_element);
                if (slowLocation == location && slowTileElement.GetBaseZ() == tileElement->GetBaseZ()
                    && slowTileElement.GetType() == tileElement->GetType()
                    && slowTileElement.GetDirection() == tileElement->GetDirection())
                {
                    return nullptr;
                }
            }
        } while (!(trackBeginEnd.begin_element->AsTrack()->IsBlockStart()));

        // Get the start of the track block instead of the end
        location = { trackBeginEnd.begin_x, trackBeginEnd.begin_y, trackBeginEnd.begin_z };
        auto trackOrigin = OpenRCT2::MapGetTrackElementAtOfTypeSeq(
            location, trackBeginEnd.begin_element->AsTrack()->GetTrackType(), 0);
        if (trackOrigin == nullptr)
        {
            return nullptr;
        }

        return trackOrigin->AsTrack();
    }

    /**
     *
     * rct2: 0x006C6096
     * Gets the next track block coordinates from the
     * coordinates of the first of element of a track block.
     * Use track_block_get_next if you are unsure if you are
     * on the first element of a track block
     */
    bool trackBlockGetNextFromZero(
        const CoordsXYZ& startPos, const Ride& ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
        bool isGhost)
    {
        auto trackPos = startPos;

        if (!TrackPieceDirectionIsDiagonal(direction_start))
        {
            trackPos += CoordsDirectionDelta[direction_start];
        }

        TileElement* tileElement = MapGetFirstElementAt(trackPos);
        if (tileElement == nullptr)
        {
            output->element = nullptr;
            output->x = kLocationNull;
            return false;
        }

        do
        {
            const auto* trackElement = tileElement->AsTrack();
            if (trackElement == nullptr)
                continue;

            if (trackElement->GetRideIndex() != ride.id)
                continue;

            if (trackElement->GetSequenceIndex() != 0)
                continue;

            if (tileElement->IsGhost() != isGhost)
                continue;

            const auto& ted = TrackMetadata::GetTrackElementDescriptor(trackElement->GetTrackType());
            if (ted.sequenceData.numSequences == 0)
                continue;

            const auto& nextTrackCoordinate = ted.coordinates;
            uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotationBegin)
                | (nextTrackCoordinate.rotationBegin & kTrackDirectionDiagonalMask);

            if (nextRotation != direction_start)
                continue;

            int16_t nextZ = nextTrackCoordinate.zBegin - ted.sequenceData.sequences[0].clearance.z + tileElement->GetBaseZ();
            if (nextZ != trackPos.z)
                continue;

            if (z != nullptr)
                *z = tileElement->GetBaseZ();
            if (direction != nullptr)
                *direction = nextRotation;
            *output = { trackPos, tileElement };
            return true;
        } while (!(tileElement++)->IsLastForTile());

        if (direction != nullptr)
            *direction = direction_start;
        if (z != nullptr)
            *z = trackPos.z;
        *output = { trackPos, --tileElement };
        return false;
    }

    /**
     *
     *  rct2: 0x006C60C2
     */
    bool trackBlockGetNext(const CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction)
    {
        if (input == nullptr || input->element == nullptr)
            return false;

        const auto* inputElement = input->element->AsTrack();
        if (inputElement == nullptr)
            return false;

        auto rideIndex = inputElement->GetRideIndex();
        const auto* ride = GetRide(rideIndex);
        if (ride == nullptr)
            return false;

        const auto& ted = TrackMetadata::GetTrackElementDescriptor(inputElement->GetTrackType());
        auto sequenceIndex = inputElement->GetSequenceIndex();
        if (sequenceIndex >= ted.sequenceData.numSequences)
            return false;

        const auto& trackBlock = ted.sequenceData.sequences[sequenceIndex].clearance;
        const auto& trackCoordinate = ted.coordinates;

        int32_t x = input->x;
        int32_t y = input->y;
        int32_t OriginZ = inputElement->GetBaseZ();

        uint8_t rotation = inputElement->GetDirection();

        CoordsXY coords = { x, y };
        CoordsXY trackCoordOffset = { trackCoordinate.x, trackCoordinate.y };
        CoordsXY trackBlockOffset = { trackBlock.x, trackBlock.y };
        coords += trackCoordOffset.Rotate(rotation);
        coords += trackBlockOffset.Rotate(DirectionReverse(rotation));

        OriginZ -= trackBlock.z;
        OriginZ += trackCoordinate.zEnd;

        uint8_t directionStart = ((trackCoordinate.rotationEnd + rotation) & kTileElementDirectionMask)
            | (trackCoordinate.rotationEnd & kTrackDirectionDiagonalMask);

        return trackBlockGetNextFromZero({ coords, OriginZ }, *ride, directionStart, output, z, direction, false);
    }

    /**
     * Returns the begin position / direction and end position / direction of the
     * track piece that proceeds the given location. Gets the previous track block
     * coordinates from the coordinates of the first of element of a track block.
     * Use track_block_get_previous if you are unsure if you are on the first
     * element of a track block
     *  rct2: 0x006C63D6
     */
    bool trackBlockGetPreviousFromZero(
        const CoordsXYZ& startPos, const Ride& ride, uint8_t direction, TrackBeginEnd* outTrackBeginEnd)
    {
        uint8_t directionStart = direction;
        direction = DirectionReverse(direction);
        auto trackPos = startPos;

        if (!TrackPieceDirectionIsDiagonal(direction))
        {
            trackPos += CoordsDirectionDelta[direction];
        }

        TileElement* tileElement = MapGetFirstElementAt(trackPos);
        if (tileElement == nullptr)
        {
            outTrackBeginEnd->end_x = trackPos.x;
            outTrackBeginEnd->end_y = trackPos.y;
            outTrackBeginEnd->begin_element = nullptr;
            outTrackBeginEnd->begin_direction = DirectionReverse(directionStart);
            return false;
        }

        do
        {
            const auto* trackElement = tileElement->AsTrack();
            if (trackElement == nullptr)
                continue;

            if (trackElement->GetRideIndex() != ride.id)
                continue;

            const auto& ted = TrackMetadata::GetTrackElementDescriptor(trackElement->GetTrackType());
            if (ted.sequenceData.numSequences == 0)
                continue;

            auto sequenceIndex = trackElement->GetSequenceIndex();
            if ((sequenceIndex + 1) != ted.sequenceData.numSequences)
                continue;

            const auto& currentBlock = ted.sequenceData.sequences[sequenceIndex].clearance;

            const auto& nextTrackCoordinate = ted.coordinates;
            uint8_t nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotationEnd)
                | (nextTrackCoordinate.rotationEnd & kTrackDirectionDiagonalMask);

            if (nextRotation != directionStart)
                continue;

            int16_t nextZ = nextTrackCoordinate.zEnd - currentBlock.z + tileElement->GetBaseZ();
            if (nextZ != trackPos.z)
                continue;

            nextRotation = tileElement->GetDirectionWithOffset(nextTrackCoordinate.rotationBegin)
                | (nextTrackCoordinate.rotationBegin & kTrackDirectionDiagonalMask);
            outTrackBeginEnd->begin_element = tileElement;
            outTrackBeginEnd->begin_x = trackPos.x;
            outTrackBeginEnd->begin_y = trackPos.y;
            outTrackBeginEnd->end_x = trackPos.x;
            outTrackBeginEnd->end_y = trackPos.y;

            CoordsXY coords = { outTrackBeginEnd->begin_x, outTrackBeginEnd->begin_y };
            CoordsXY offsets = { nextTrackCoordinate.x, nextTrackCoordinate.y };
            coords += offsets.Rotate(DirectionReverse(nextRotation));
            outTrackBeginEnd->begin_x = coords.x;
            outTrackBeginEnd->begin_y = coords.y;

            outTrackBeginEnd->begin_z = tileElement->GetBaseZ();

            const auto& firstBlock = ted.sequenceData.sequences[0].clearance;
            outTrackBeginEnd->begin_z += firstBlock.z - currentBlock.z;
            outTrackBeginEnd->begin_direction = nextRotation;
            outTrackBeginEnd->end_direction = DirectionReverse(directionStart);
            return true;
        } while (!(tileElement++)->IsLastForTile());

        outTrackBeginEnd->end_x = trackPos.x;
        outTrackBeginEnd->end_y = trackPos.y;
        outTrackBeginEnd->begin_z = trackPos.z;
        outTrackBeginEnd->begin_element = nullptr;
        outTrackBeginEnd->end_direction = DirectionReverse(directionStart);
        return false;
    }

    /**
     *
     *  rct2: 0x006C6402
     *
     * @remarks outTrackBeginEnd.begin_x and outTrackBeginEnd.begin_y will be in the
     * higher two bytes of ecx and edx where as outTrackBeginEnd.end_x and
     * outTrackBeginEnd.end_y will be in the lower two bytes (cx and dx).
     */
    bool trackBlockGetPrevious(const CoordsXYE& trackPos, TrackBeginEnd* outTrackBeginEnd)
    {
        if (trackPos.element == nullptr)
            return false;

        auto trackElement = trackPos.element->AsTrack();
        if (trackElement == nullptr)
            return false;

        const auto& ted = TrackMetadata::GetTrackElementDescriptor(trackElement->GetTrackType());

        auto rideIndex = trackElement->GetRideIndex();
        const auto* ride = GetRide(rideIndex);
        if (ride == nullptr)
            return false;

        auto sequenceIndex = trackElement->GetSequenceIndex();
        if (sequenceIndex >= ted.sequenceData.numSequences)
            return false;

        const auto& trackBlock = ted.sequenceData.sequences[sequenceIndex].clearance;
        auto trackCoordinate = ted.coordinates;

        int32_t z = trackElement->GetBaseZ();

        uint8_t rotation = trackElement->GetDirection();
        CoordsXY coords = CoordsXY{ trackPos };
        CoordsXY offsets = { trackBlock.x, trackBlock.y };
        coords += offsets.Rotate(DirectionReverse(rotation));

        z -= trackBlock.z;
        z += trackCoordinate.zBegin;

        rotation = ((trackCoordinate.rotationBegin + rotation) & kTileElementDirectionMask)
            | (trackCoordinate.rotationBegin & kTrackDirectionDiagonalMask);

        return trackBlockGetPreviousFromZero({ coords, z }, *ride, rotation, outTrackBeginEnd);
    }

    /**
     *
     * Make sure to pass in the x and y of the start track element too.
     *  rct2: 0x006CB02F
     * ax result x
     * bx result y
     * esi input / output map element
     */
    bool findTrackGap(const Ride& ride, const CoordsXYE& input, CoordsXYE* output)
    {
        if (input.element == nullptr || input.element->GetType() != TileElementType::Track)
            return false;

        const auto& rtd = ride.getRideTypeDescriptor();
        if (rtd.specialType == RtdSpecialType::maze)
            return false;

        auto* windowMgr = Ui::GetWindowManager();
        const WindowBase* w = windowMgr->FindByClass(WindowClass::rideConstruction);
        if (w != nullptr && _rideConstructionState != RideConstructionState::State0 && _currentRideIndex == ride.id)
        {
            RideConstructionInvalidateCurrentTrack();
        }

        bool moveSlowIt = true;
        TrackCircuitIterator it = {};
        trackCircuitIteratorBegin(&it, input);
        TrackCircuitIterator slowIt = it;
        while (trackCircuitIteratorNext(&it))
        {
            if (!TrackIsConnectedByShape(it.last.element, it.current.element))
            {
                *output = it.current;
                return true;
            }
            // #2081: prevent an infinite loop
            moveSlowIt = !moveSlowIt;
            if (moveSlowIt)
            {
                trackCircuitIteratorNext(&slowIt);
                if (trackCircuitIteratorsMatch(&it, &slowIt))
                {
                    *output = it.current;
                    return true;
                }
            }
        }
        if (!it.looped)
        {
            *output = it.last;
            return true;
        }

        return false;
    }
} // namespace OpenRCT2
