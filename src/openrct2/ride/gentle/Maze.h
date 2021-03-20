/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifndef _MAZE_H_
#define _MAZE_H_

#include "../../world/Location.hpp"

#include <array>
#include <functional>
#include <optional>
#include <utility>

/**
 * Get the new maze subTileIndex as used by Peep::var_37 from the previous subTitleIndex and a direction.
 * Div the returned value by 4 to get the subTileIndex as used the maze pathfinding methods.
 */
extern const uint8_t gMazeGetNewDirectionFromEdge[][4];

/**
 * Get the bit index of the flag in a mazeEntry indicating the presence of a hedge for a subTileIndex and a direction.
 */
extern const uint8_t gMazeCurrentDirectionToHedge[][4];

class PeepMazeStateRegister
{
    /**
     * Immediate memory of a guest when moving within a maze.
     * 7-5: [unused]
     * 4:   PeekFlag,                           True if the guest jumped to check his position
     * 3-2: LastDirectionAtLastIntersection,    Last direction at the previous intersection
     * 1-0: LastDirection,                      Last direction at the previous subTile
     * @TODO: Replace by individual attributs when the new save format will be implemented.
     */
    uint8_t Data;

public:
    void SetLastDirection(Direction lastEdge);
    Direction GetLastDirection() const;
    void ReverseLastDirection();

    void SetDirectionAtLastIntersection(Direction lastEdge);
    Direction GetDirectionAtLastIntersection() const;

    bool GetPeekFlag() const;
    bool CheckAndResetPeekFlag();
    void SetPeekFlag();
};
static_assert(sizeof(PeepMazeStateRegister) == sizeof(uint8_t));

class MazePathfindingEntry
{
    /**
     * The data linked to an intersection perviously encountered by a guest.
     * 31-21:   YTile,              Y coordonate of the tile containing the intersection
     * 20:      CompletlyVisited,   Indicates if all the directions from this intersection are completly visited
     * 19-16:   VisitedFlags,       Each bit indicates if the 'bit rank' direction is already completly visited`
     * 15-5:    XTile,              X coordonate of the tile containing the intersection
     * 4:       [unused]
     * 3-2:     Origin,             Direction from which the guest meets for the first time the intersection
     * 1-0:     SubTileIndex,       Index of the subTile corresponding to the intersection
     * @TODO: Replace by individual attributs when the new save format will be implemented.
     */
    uint32_t Data;

    void SetSubTileIndex(uint8_t subTileIndex);
    uint8_t GetSubTileIndex() const;
    void SetOrigin(Direction origin);
    void SetXTile(uint16_t xTile);
    uint16_t GetXTile() const;
    void SetVisitedFlags(uint8_t visitedFlags);
    void AddVisitedFlags(Direction visitedDirection);
    void DelVisitedFlags(Direction visitedDirection);
    void SetCompletlyVisited(bool completlyVisited);
    void SetYTile(uint16_t yTile);
    uint16_t GetYTile() const;

public:
    /**
     * Set the entry as empty.
     */
    void Reset();

    /**
     * Clear the pathfinding data for the global pathfinding algorithms when the guest exits the maze.
     */
    void Free();

    /**
     * Initialize the entry.
     * @param coords        The XY coordinates of the tile containing the intersection.
     * @param subTileIndex  Subtile index on the tile of the intersection.
     * @param origin        Direction from which the guest first encountered the intersection.
     */
    void Set(const CoordsXY& coords, uint8_t subTileIndex, Direction origin);

    /**
     * Get the XY coordinates and the subTile index of the intersection.
     */
    std::pair<CoordsXY, uint8_t> GetCoords() const;

    /**
     * Get the direction from which the guest first encountered the intersection.
     */
    Direction GetOrigin() const;

    /**
     * Update / mark as visited a direction at the intersection.
     * @param source          Direction from which the guest comes.
     * @param openEdgesCount  Number of edges open at the intersection.
     */
    void MarkVisited(Direction source, uint8_t openEdgesCount);

    /**
     * Forget / unmark a direction at this intersection.
     * @param direction     Direction to forget. Does nothing if the direction is the origin or is already not marked.
     */
    void UnmarkVisited(Direction direction);

    /**
     * Get the flags corresponding to the directions already visited from this intersection.
     */
    uint8_t GetVisited() const;

    /**
     * Check if every path starting from this intersection has been visited.
     */
    bool IsCompletlyVisited() const;

    /**
     * Check if coordinates match with these of the intersection.
     */
    bool MatchCoords(const CoordsXY& coords, uint8_t subTileIndex) const;

    /**
     * Check if another intersection coordinates with these of the intersection.
     */
    bool MatchCoords(const MazePathfindingEntry& e) const;
};
static_assert(sizeof(MazePathfindingEntry) == sizeof(uint32_t));

class MazePathfindingHistory
{
    std::array<MazePathfindingEntry, 5> Stack;

public:
    /**
     * Initialize the pathfinding history when the guest enters into the maze.
     */
    void Initialize();

    /**
     * Clear the pathfinding data for the global pathfinding algorithms when the guest exits the maze.
     */
    void Free();

    /**
     * When the guest encounters a intersection, update the pathfinding history and the corresponding entry.
     * @param coords        The XY coordinates of the tile containing the intersection.
     * @param subTileIndex  Subtile index on the tile of the intersection.
     * @param source        Direction from which the guest encountered the intersection.
     * @return The corresponding entry to the intersection, the index of the entry into the history.
     */
    std::pair<MazePathfindingEntry&, uint8_t> MeetIntersection(const CoordsXY& coords, uint8_t subTileIndex, Direction source);

    /**
     * Get the entry corresponding to the last intersection encountered by the guest.
     */
    MazePathfindingEntry& GetLast();

    /**
     * Get a read-only access to the all history entries.
     */
    const std::array<MazePathfindingEntry, 5>& GetHistory() const;
};
// The maze pathfinding data should not take more space than the usual peep pathfinding data.
static_assert(sizeof(MazePathfindingHistory) == sizeof(uint32_t) * 5);

#endif
