
#include "Paint.Tunnel.h"

#include "../../core/EnumUtils.hpp"
#include "../Paint.h"

#include <cstdint>

using namespace OpenRCT2;

using TunnelGroupMap = std::array<TunnelType, kTunnelSubTypeCount>;
static std::array<TunnelGroupMap, kTunnelGroupCount> tunnelMap = {
    TunnelGroupMap{ TunnelType::StandardFlat, TunnelType::StandardSlopeStart, TunnelType::StandardSlopeEnd,
                    TunnelType::StandardFlatTo25Deg, TunnelType::InvertedFlat },
    TunnelGroupMap{ TunnelType::SquareFlat, TunnelType::SquareSlopeStart, TunnelType::SquareSlopeEnd,
                    TunnelType::SquareFlatTo25Deg, TunnelType::InvertedSquare },
    TunnelGroupMap{ TunnelType::InvertedFlat, TunnelType::InvertedSlopeStart, TunnelType::InvertedSlopeEnd,
                    TunnelType::InvertedFlatTo25Deg, TunnelType::InvertedFlat },
};

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel);
void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType);

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type)
{
    session.LeftTunnels[session.LeftTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session.LeftTunnelCount < kTunnelMaxCount - 1)
    {
        session.LeftTunnels[session.LeftTunnelCount + 1] = { 0xFF, TunnelType::Null };
        session.LeftTunnelCount++;
    }
}

void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type)
{
    session.RightTunnels[session.RightTunnelCount] = { static_cast<uint8_t>((height / 16)), type };
    if (session.RightTunnelCount < kTunnelMaxCount - 1)
    {
        session.RightTunnels[session.RightTunnelCount + 1] = { 0xFF, TunnelType::Null };
        session.RightTunnelCount++;
    }
}

void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height)
{
    session.VerticalTunnelHeight = height / 16;
}

void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, TunnelType type)
{
    if (direction & 1)
    {
        PaintUtilPushTunnelRight(session, height, type);
    }
    else
    {
        PaintUtilPushTunnelLeft(session, height, type);
    }
}

void TrackPaintUtilRightQuarterTurn5TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }
    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }
    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }
}

void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
    }
}

void TrackPaintUtilRightQuarterTurn3TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }

    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }

    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }
}

void TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
    PaintSession& session, TunnelGroup group, int16_t height, Direction direction, uint8_t trackSequence,
    TunnelSubType tunnelType0, TunnelSubType tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height - 8, group, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height + 8, group, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height + 8, group, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height - 8, group, tunnelType0);
    }
}

void TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
    PaintSession& session, TunnelGroup group, int16_t height, Direction direction, uint8_t trackSequence,
    TunnelSubType tunnelType0, TunnelSubType tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height + 8, group, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height - 8, group, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height - 8, group, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height + 8, group, tunnelType0);
    }
}

void TrackPaintUtilLeftQuarterTurn3TilesTunnel(
    PaintSession& session, TunnelGroup group, TunnelSubType tunnelType, int16_t height, Direction direction,
    uint8_t trackSequence)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }

    if (direction == 2 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, group, tunnelType);
    }

    if (direction == 3 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height, group, tunnelType);
    }
}

void TrackPaintUtilRightQuarterTurn1TileTunnel(
    PaintSession& session, TunnelGroup group, Direction direction, uint16_t baseHeight, int8_t startOffset,
    TunnelSubType startTunnel, int8_t endOffset, TunnelSubType endTunnel)
{
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, group, DirectionPrev(direction), baseHeight, endOffset, endTunnel, startOffset, startTunnel);
}

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel)
{
    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, baseHeight + startOffset, startTunnel);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, baseHeight + endOffset, endTunnel);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, baseHeight + startOffset, startTunnel);
            PaintUtilPushTunnelLeft(session, baseHeight + endOffset, endTunnel);
            break;
    }
}

void TrackPaintUtilLeftQuarterTurn1TileTunnel(
    PaintSession& session, TunnelGroup group, Direction direction, uint16_t baseHeight, int8_t startOffset,
    TunnelSubType startTunnel, int8_t endOffset, TunnelSubType endTunnel)
{
    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, baseHeight + startOffset, group, startTunnel);
            break;
        case 2:
            PaintUtilPushTunnelRight(session, baseHeight + endOffset, group, endTunnel);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, baseHeight + startOffset, group, startTunnel);
            PaintUtilPushTunnelLeft(session, baseHeight + endOffset, group, endTunnel);
            break;
    }
}

TunnelType GetTunnelType(TunnelGroup tunnelGroup, TunnelSubType tunnelSubType)
{
    return tunnelMap[EnumValue(tunnelGroup)][EnumValue(tunnelSubType)];
}
