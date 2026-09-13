#pragma once

#include "../world/Location.hpp"

struct Ride;

namespace OpenRCT2
{
    struct TrackElement;

    struct TrackCircuitIterator
    {
        CoordsXYE last;
        CoordsXYE current;
        int32_t currentZ;
        int32_t currentDirection;
        TileElement* first;
        bool firstIteration;
        bool looped;
    };

    struct TrackBeginEnd
    {
        int32_t begin_x;
        int32_t begin_y;
        int32_t begin_z;
        int32_t begin_direction;
        TileElement* begin_element;
        int32_t end_x;
        int32_t end_y;
        int32_t end_direction;
        TileElement* end_element;
    };

    void trackCircuitIteratorBegin(TrackCircuitIterator* it, CoordsXYE first);
    bool trackCircuitIteratorPrevious(TrackCircuitIterator* it);
    bool trackCircuitIteratorNext(TrackCircuitIterator* it);
    bool trackCircuitIteratorsMatch(const TrackCircuitIterator* firstIt, const TrackCircuitIterator* secondIt);

    void trackGetBack(const CoordsXYE& input, CoordsXYE* output);
    void trackGetFront(const CoordsXYE& input, CoordsXYE* output);

    TrackElement* trackGetPreviousBlock(CoordsXYZ& location, TileElement* tileElement);

    bool trackBlockGetNext(const CoordsXYE* input, CoordsXYE* output, int32_t* z, int32_t* direction);
    bool trackBlockGetNextFromZero(
        const CoordsXYZ& startPos, const Ride& ride, uint8_t direction_start, CoordsXYE* output, int32_t* z, int32_t* direction,
        bool isGhost);

    bool trackBlockGetPrevious(const CoordsXYE& trackPos, TrackBeginEnd* outTrackBeginEnd);
    bool trackBlockGetPreviousFromZero(
        const CoordsXYZ& startPos, const Ride& ride, uint8_t direction, TrackBeginEnd* outTrackBeginEnd);

    bool findTrackGap(const Ride& ride, const CoordsXYE& input, CoordsXYE* output);
} // namespace OpenRCT2
