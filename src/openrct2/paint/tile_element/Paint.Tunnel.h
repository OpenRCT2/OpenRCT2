/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../world/Location.hpp"

#include <cstdint>

struct PaintSession;

constexpr uint8_t kTunnelMaxCount = 65;

enum class TunnelType : uint8_t
{
    StandardFlat = 0,       // Used on flat track (or the flat part of flat-to-sloped track)
    StandardSlopeStart = 1, // Used on the lower end of sloped track
    StandardSlopeEnd = 2,   // Used on the upper end of sloped track
    InvertedFlat = 3,       // Used on flat track (or the flat part of flat-to-sloped track)
    InvertedSlopeStart = 4, // Used on the lower end of sloped track
    InvertedSlopeEnd = 5,   // Used on the upper end of sloped track
    SquareFlat = 6,         // Used on flat track (or the flat part of flat-to-sloped track)
    SquareSlopeStart = 7,   // Used on the lower end of sloped track
    SquareSlopeEnd = 8,     // Used on the upper end of sloped track
    InvertedSquare = 9,
    PathAndMiniGolf = 10,
    Path11 = 11,
    StandardFlatTo25Deg = 12,
    InvertedFlatTo25Deg = 13,
    SquareFlatTo25Deg = 14,
    InvertedSquareFlatTo25Deg = 15,

    // Ghost train doors
    Doors0 = 16,
    Doors1 = 17,
    Doors2 = 18,
    Doors3 = 19,
    Doors4 = 20,
    Doors5 = 21,
    Doors6 = 22,

    Count,

    Null = 255,
};
constexpr uint8_t kRegularTunnelTypeCount = 16;

enum class TunnelGroup : uint8_t
{
    Standard = 0,
    Square = 1,
    Inverted = 2,
};
constexpr uint8_t kTunnelGroupCount = 3;

enum class TunnelSubType : uint8_t
{
    Flat = 0,
    SlopeStart = 1,
    SlopeEnd = 2,
    FlatTo25Deg = 3,
    Tall = 4,
};
constexpr uint8_t kTunnelSubTypeCount = 5;

struct TunnelEntry
{
    uint8_t height;
    TunnelType type;
};

TunnelType GetTunnelType(TunnelGroup tunnelGroup, TunnelSubType tunnelSubType);

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height);
void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, TunnelType type);

inline void PaintUtilPushTunnelLeft(
    PaintSession& session, uint16_t height, TunnelGroup tunnelGroup, TunnelSubType tunnelSubType)
{
    PaintUtilPushTunnelLeft(session, height, GetTunnelType(tunnelGroup, tunnelSubType));
}

inline void PaintUtilPushTunnelRight(
    PaintSession& session, uint16_t height, TunnelGroup tunnelGroup, TunnelSubType tunnelSubType)
{
    PaintUtilPushTunnelRight(session, height, GetTunnelType(tunnelGroup, tunnelSubType));
}

inline void PaintUtilPushTunnelRotated(
    PaintSession& session, uint8_t direction, uint16_t height, TunnelGroup tunnelGroup, TunnelSubType tunnelSubType)
{
    PaintUtilPushTunnelRotated(session, direction, height, GetTunnelType(tunnelGroup, tunnelSubType));
}

void TrackPaintUtilRightQuarterTurn5TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence);

void TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
    PaintSession& session, TunnelGroup group, int16_t height, Direction direction, uint8_t trackSequence,
    TunnelSubType tunnelType0, TunnelSubType tunnelType3);
void TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
    PaintSession& session, TunnelGroup group, int16_t height, Direction direction, uint8_t trackSequence,
    TunnelSubType tunnelType0, TunnelSubType tunnelType3);

void TrackPaintUtilLeftQuarterTurn3TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence);
void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence);

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, TunnelGroup group, Direction direction, uint16_t baseHeight, int8_t startOffset,
    TunnelSubType startTunnel, int8_t endOffset, TunnelSubType endTunnel);
void TrackPaintUtilRightQuarterTurn1TileTunnel(
    PaintSession& session, TunnelGroup group, Direction direction, uint16_t baseHeight, int8_t startOffset,
    TunnelSubType startTunnel, int8_t endOffset, TunnelSubType endTunnel);
