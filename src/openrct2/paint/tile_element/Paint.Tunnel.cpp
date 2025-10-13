
#include "Paint.Tunnel.h"

#include "../../core/EnumUtils.hpp"
#include "../Paint.h"

#include <cstdint>

using namespace OpenRCT2;

using TunnelSlopeMap = std::array<TunnelType, kTunnelSlopeCount>;
static constexpr const std::array<TunnelSlopeMap, kTunnelStyleCount> kTunnelStyleSlopeMap = { {
    { TunnelType::standardFlat, TunnelType::standardUp25, TunnelType::standardDown25, TunnelType::standardFlatToDown25,
      TunnelType::invertedFlat }, // standard
    { TunnelType::invertedFlat, TunnelType::invertedUp25, TunnelType::invertedDown25, TunnelType::invertedFlatToDown25,
      TunnelType::invertedFlat }, // inverted
    { TunnelType::squareFlat, TunnelType::squareUp25, TunnelType::squareDown25, TunnelType::squareFlatToDown25,
      TunnelType::invertedSquareFlat }, // square
    { TunnelType::invertedSquareFlat, TunnelType::squareUp25, TunnelType::squareDown25, TunnelType::invertedSquareFlatToDown25,
      TunnelType::invertedSquareFlat }, // invertedSquare
    { TunnelType::path, TunnelType::standardUp25, TunnelType::standardDown25, TunnelType::path,
      TunnelType::invertedFlat }, // standardWithPath
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

static constexpr std::array<TunnelType, kLandEdgeDoorFrameCount> kTunnelDoorOpeningInwardsToImage = {
    TunnelType::doorClosed,        TunnelType::doorOpeningInward, TunnelType::doorOpeningInward, TunnelType::doorOpenInward,
    TunnelType::doorOpeningInward, TunnelType::doorOpeningInward, TunnelType::doorClosed,        TunnelType::doorClosed,
};

static constexpr std::array<TunnelType, kLandEdgeDoorFrameCount> kTunnelDoorOpeningOutwardsToImage = {
    TunnelType::doorClosed,         TunnelType::doorOpeningOutward, TunnelType::doorOpeningOutward, TunnelType::doorOpenOutward,
    TunnelType::doorOpeningOutward, TunnelType::doorOpeningOutward, TunnelType::doorClosed,         TunnelType::doorClosed,
};

static constexpr std::array<TunnelType, kLandEdgeDoorFrameCount> kTunnelDoorFlatTo25DegOpeningInwardsToImage = {
    TunnelType::doorClosedFlatToDown25,        TunnelType::doorOpeningInwardFlatToDown25,
    TunnelType::doorOpeningInwardFlatToDown25, TunnelType::doorOpenInwardFlatToDown25,
    TunnelType::doorOpeningInwardFlatToDown25, TunnelType::doorOpeningInwardFlatToDown25,
    TunnelType::doorClosedFlatToDown25,        TunnelType::doorClosedFlatToDown25,
};

static constexpr std::array<TunnelType, kLandEdgeDoorFrameCount> kTunnelDoorFlatTo25DegOpeningOutwardsToImage = {
    TunnelType::doorClosedFlatToDown25,         TunnelType::doorOpeningOutwardFlatToDown25,
    TunnelType::doorOpeningOutwardFlatToDown25, TunnelType::doorOpenOutwardFlatToDown25,
    TunnelType::doorOpeningOutwardFlatToDown25, TunnelType::doorOpeningOutwardFlatToDown25,
    TunnelType::doorClosedFlatToDown25,         TunnelType::doorClosedFlatToDown25,
};

static constexpr const std::array<std::array<std::array<TunnelType, kLandEdgeDoorFrameCount>, 2>, 2> kTunnelDoorDirections = {
    { { kTunnelDoorOpeningInwardsToImage, kTunnelDoorOpeningOutwardsToImage },
      { kTunnelDoorFlatTo25DegOpeningInwardsToImage, kTunnelDoorFlatTo25DegOpeningOutwardsToImage } }
};

TunnelType GetTunnelTypeDoors(const OpenRCT2::TrackElement& trackElement, const Direction direction, const bool flatToDown25)
{
    const bool index = direction != 0;
    const std::array<uint8_t, 2> doorStates = { trackElement.GetDoorAState(), trackElement.GetDoorBState() };
    return kTunnelDoorDirections[flatToDown25][index][doorStates[index]];
}
