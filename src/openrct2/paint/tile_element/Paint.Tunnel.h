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
    square,
    inverted,
};
constexpr uint8_t kTunnelStyleCount = 3;

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
