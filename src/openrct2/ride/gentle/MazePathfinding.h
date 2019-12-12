/*****************************************************************************
 * Copyright (c) 2014-2019 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _MAZE_PATHFINDING_H_
#define _MAZE_PATHFINDING_H_

#include <array>
#include <functional>
#include <optional>
#include <utility>

class MazePathfindingEntry;
void swap(MazePathfindingEntry& a, MazePathfindingEntry& b);

// TODO: Check if these classes need to move into guest.h/cpp
class MazePathfindingEntry
{
    // 31-21: yTile (31-16: yCoord)
    // 20: completlyVisited
    // 19-16: visitedFlags
    // 15-5: xTile (15-0: xCoord)
    // 4: peekedFlag
    // 3-2: origin
    // 1-0: subTile
    uint32_t data;
    // TODO: Move peekedFlag into another peep attribut (maze_last_edge)

public:
    friend void swap(MazePathfindingEntry& a, MazePathfindingEntry& b);

    void reset();
    void clear();
    void set(const CoordsXY& coords, uint8_t subTileIndex, Direction origin);
    std::tuple<CoordsXY, uint8_t> getCoords() const;
    Direction getOrigin() const;
    void markVisited(Direction origin, uint8_t openEdgesCount);
    uint8_t getVisited() const;
    bool isCompletlyVisited() const;
    void notifyPeeking();
    bool hasPeeked();
    bool getPeekedState() const;
    bool matchCoords(const CoordsXY& coords, uint8_t subTileIndex) const;
    bool operator==(const MazePathfindingEntry& entry) const;
};

class MazePathfindingHistory
{
    std::array<MazePathfindingEntry, 5> stack;

public:
    void initialize();
    void clear();
    std::tuple<MazePathfindingEntry&, uint8_t, bool> meetIntersection(
        const CoordsXY& coords, uint8_t subTileIndex, Direction origin);
    std::optional<std::reference_wrapper<MazePathfindingEntry>> updateIntersection(
        const CoordsXY& coords, uint8_t subTileIndex);
    const MazePathfindingEntry& getLast() const;
    void notifyPeeking();
    const std::array<MazePathfindingEntry, 5>& readHistory() const;
};
static_assert(sizeof(MazePathfindingHistory) == sizeof(uint32_t) * 5);

#endif /* _MAZE_PATHFINDING_H_ */
