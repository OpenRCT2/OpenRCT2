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
    standardFlat = 0,
    standardUp25 = 1,
    standardDown25 = 2,
    standardFlatToDown25 = 3,
    invertedFlat = 4,
    invertedUp25 = 5,
    invertedDown25 = 6,
    invertedFlatToDown25 = 7,
    squareFlat = 8,
    squareUp25 = 9,
    squareDown25 = 10,
    squareFlatToDown25 = 11,
    invertedSquareFlat = 12,
    invertedSquareFlatToDown25 = 13,
    path = 14,
    pathOpenBack = 15,

    // Ghost train doors
    doorClosed = 16,
    doorOpeningOutward = 17,
    doorOpenOutward = 18,
    doorOpeningInward = 19,
    doorOpenInward = 20,
    doorClosedFlatToDown25 = 21,
    doorOpeningOutwardFlatToDown25 = 22,
    doorOpenOutwardFlatToDown25 = 23,
    doorOpeningInwardFlatToDown25 = 24,
    doorOpenInwardFlatToDown25 = 25,
};
constexpr uint8_t kRegularTunnelTypeCount = 16;
constexpr uint8_t kTunnelTypeCount = 26;

enum class TunnelGroup : uint8_t
{
    uninverted,
    inverted,
    invertedFlying,
};
constexpr uint8_t kTunnelGroupCount = 3;

enum class TunnelStyle : uint8_t
{
    standard,
    inverted,
    square,
    invertedSquare,
    standardWithPath,
};
constexpr uint8_t kTunnelStyleCount = 5;

enum class TunnelSlope : uint8_t
{
    flat,
    up25,
    down25,
    flatToDown25,
    tall,
};
constexpr uint8_t kTunnelSlopeCount = 5;

struct TunnelEntry
{
    uint8_t height;
    TunnelType type;

    constexpr TunnelEntry(const uint8_t _height, const TunnelType _type)
        : height(_height)
        , type(_type) {};
};

TunnelType GetTunnelType(TunnelStyle style, TunnelSlope slope);

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height);
void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, TunnelType type);

inline void PaintUtilPushTunnelLeft(
    PaintSession& session, const uint16_t height, const TunnelStyle style, const TunnelSlope slope)
{
    PaintUtilPushTunnelLeft(session, height, GetTunnelType(style, slope));
}

inline void PaintUtilPushTunnelRight(
    PaintSession& session, const uint16_t height, const TunnelStyle style, const TunnelSlope slope)
{
    PaintUtilPushTunnelRight(session, height, GetTunnelType(style, slope));
}

inline void PaintUtilPushTunnelRotated(
    PaintSession& session, const uint8_t direction, const uint16_t height, const TunnelStyle style, const TunnelSlope slope)
{
    PaintUtilPushTunnelRotated(session, direction, height, GetTunnelType(style, slope));
}
