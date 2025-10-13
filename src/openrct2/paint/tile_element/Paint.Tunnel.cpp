
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

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type)
{
    if (!session.LeftTunnels.full())
    {
        session.LeftTunnels.emplace_back(height / kCoordsZPerTinyZ, type);
    }
}

void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type)
{
    if (!session.RightTunnels.full())
    {
        session.RightTunnels.emplace_back(height / kCoordsZPerTinyZ, type);
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

TunnelType GetTunnelType(TunnelGroup tunnelGroup, TunnelSubType tunnelSubType)
{
    return tunnelMap[EnumValue(tunnelGroup)][EnumValue(tunnelSubType)];
}
