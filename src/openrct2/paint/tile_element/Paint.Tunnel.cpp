
#include "Paint.Tunnel.h"

#include "../../core/EnumUtils.hpp"
#include "../Paint.h"

#include <cstdint>

using namespace OpenRCT2;

using TunnelSlopeMap = std::array<TunnelType, kTunnelSlopeCount>;
static constexpr const std::array<TunnelSlopeMap, kTunnelStyleCount> kTunnelStyleSlopeMap = { {
    { TunnelType::StandardFlat, TunnelType::StandardSlopeStart, TunnelType::StandardSlopeEnd, TunnelType::StandardFlatTo25Deg,
      TunnelType::InvertedFlat }, // standard
    { TunnelType::SquareFlat, TunnelType::SquareSlopeStart, TunnelType::SquareSlopeEnd, TunnelType::SquareFlatTo25Deg,
      TunnelType::InvertedSquare }, // square
    { TunnelType::InvertedFlat, TunnelType::InvertedSlopeStart, TunnelType::InvertedSlopeEnd, TunnelType::InvertedFlatTo25Deg,
      TunnelType::InvertedFlat }, // inverted
} };

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

TunnelType GetTunnelType(const TunnelStyle style, const TunnelSlope slope)
{
    return kTunnelStyleSlopeMap[EnumValue(style)][EnumValue(slope)];
}
