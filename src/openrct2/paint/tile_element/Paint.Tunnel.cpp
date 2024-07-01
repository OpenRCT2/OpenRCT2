
#include "Paint.Tunnel.h"

#include "../Paint.h"

#include <cstdint>

using namespace OpenRCT2;

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
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }
    if (direction == 0 && trackSequence == 6)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
    }
    if (direction == 1 && trackSequence == 6)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
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

void TrackPaintUtilRightQuarterTurn3Tiles25DegUpTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType0,
    TunnelType tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height - 8, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height + 8, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height + 8, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height - 8, tunnelType0);
    }
}

void TrackPaintUtilRightQuarterTurn3Tiles25DegDownTunnel(
    PaintSession& session, int16_t height, Direction direction, uint8_t trackSequence, TunnelType tunnelType0,
    TunnelType tunnelType3)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height + 8, tunnelType0);
    }
    if (direction == 0 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height - 8, tunnelType3);
    }
    if (direction == 1 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height - 8, tunnelType3);
    }
    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height + 8, tunnelType0);
    }
}

void TrackPaintUtilLeftQuarterTurn3TilesTunnel(
    PaintSession& session, int16_t height, TunnelType tunnelType, Direction direction, uint8_t trackSequence)
{
    if (direction == 0 && trackSequence == 0)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }

    if (direction == 2 && trackSequence == 3)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 0)
    {
        PaintUtilPushTunnelRight(session, height, tunnelType);
    }

    if (direction == 3 && trackSequence == 3)
    {
        PaintUtilPushTunnelLeft(session, height, tunnelType);
    }
}

void TrackPaintUtilRightQuarterTurn1TileTunnel(
    PaintSession& session, Direction direction, uint16_t baseHeight, int8_t startOffset, TunnelType startTunnel,
    int8_t endOffset, TunnelType endTunnel)
{
    TrackPaintUtilLeftQuarterTurn1TileTunnel(
        session, (direction + 3) % kNumOrthogonalDirections, baseHeight, endOffset, endTunnel, startOffset, startTunnel);
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
