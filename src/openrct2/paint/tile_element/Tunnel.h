/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
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
    _12 = 12,
    _13 = 13,
    _14 = 14,
    _15 = 15,

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

struct TunnelEntry
{
    uint8_t height;
    TunnelType type;
};

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height);
void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, TunnelType type);

void TrackPaintUtilRightQuarterTurn5TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType);

void TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType0,
    TunnelType tunnelType3);
void TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType0,
    TunnelType tunnelType3);

void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType);
void TrackPaintUtilLeftQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, TunnelType tunnelType, Direction direction, uint8_t trackSequence);

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel);
void TrackPaintUtilRightQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel);
