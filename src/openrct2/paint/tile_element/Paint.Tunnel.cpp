
#include "Paint.Tunnel.h"

#include "../../core/EnumUtils.hpp"
#include "../Paint.h"

#include <cstdint>

using namespace OpenRCT2;

using TunnelSlopeMap = std::array<TunnelType, kTunnelSlopeCount>;
static constexpr std::array<TunnelSlopeMap, kTunnelStyleCount> kTunnelMap = {
    TunnelSlopeMap{ TunnelType::Null, TunnelType::StandardFlat, TunnelType::StandardSlopeStart, TunnelType::StandardSlopeEnd,
                    TunnelType::StandardFlatTo25Deg, TunnelType::InvertedFlat, TunnelType::StandardFlat },
    TunnelSlopeMap{ TunnelType::Null, TunnelType::SquareFlat, TunnelType::SquareSlopeStart, TunnelType::SquareSlopeEnd,
                    TunnelType::SquareFlatTo25Deg, TunnelType::InvertedSquare, TunnelType::SquareFlat },
    TunnelSlopeMap{ TunnelType::Null, TunnelType::InvertedFlat, TunnelType::InvertedSlopeStart, TunnelType::InvertedSlopeEnd,
                    TunnelType::InvertedFlatTo25Deg, TunnelType::InvertedFlat, TunnelType::InvertedFlat },
    TunnelSlopeMap{ TunnelType::Null, TunnelType::InvertedSquare, TunnelType::SquareSlopeStart, TunnelType::SquareSlopeEnd,
                    TunnelType::InvertedSquare, TunnelType::InvertedSquare, TunnelType::InvertedSquare },
    TunnelSlopeMap{ TunnelType::Null, TunnelType::PathAndMiniGolf, TunnelType::StandardSlopeStart, TunnelType::StandardSlopeEnd,
                    TunnelType::PathAndMiniGolf, TunnelType::InvertedFlat, TunnelType::StandardFlat },
};

TunnelType GetTunnelType(TunnelStyle tunnelStyle, TunnelSlope tunnelSlope)
{
    return kTunnelMap[EnumValue(tunnelStyle)][EnumValue(tunnelSlope)];
}

static constexpr std::array<TunnelType, 8> kTunnelDoorOpeningInwardsToImage = { {
    TunnelType::Doors2, // Closed
    TunnelType::Doors2, // Unused?
    TunnelType::Doors5, // Half open
    TunnelType::Doors6, // Fully open
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
} };

static constexpr std::array<TunnelType, 8> kTunnelDoorOpeningOutwardsToImage = { {
    TunnelType::Doors2, // Closed
    TunnelType::Doors2, // Unused?
    TunnelType::Doors3, // Half open
    TunnelType::Doors4, // Fully open
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
    TunnelType::Doors2, // Unused?
} };

static constexpr const std::array<std::array<TunnelType, 8>, kNumOrthogonalDirections> kTunnelDoorDirections = { {
    kTunnelDoorOpeningInwardsToImage,
    kTunnelDoorOpeningOutwardsToImage,
    kTunnelDoorOpeningOutwardsToImage,
    kTunnelDoorOpeningOutwardsToImage,
} };

TunnelType GetTunnelTypeDoors(const TrackElement& trackElement, const Direction tunnelDirection)
{
    const uint8_t doorAState = trackElement.GetDoorAState();
    const uint8_t doorBState = trackElement.GetDoorBState();
    const std::array<uint8_t, kNumOrthogonalDirections> doorStates = { {
        doorAState,
        doorBState,
        doorBState,
        doorBState,
    } };
    return kTunnelDoorDirections[tunnelDirection][doorStates[tunnelDirection]];
}

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
