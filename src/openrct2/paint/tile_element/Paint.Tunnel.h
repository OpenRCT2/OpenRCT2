/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../core/EnumUtils.hpp"
#include "../../ride/Track.h"
#include "../../world/Location.hpp"
#include "../../world/tile_element/TrackElement.h"

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

    DoorsFlatTo25Deg0 = 23,
    DoorsFlatTo25Deg1 = 24,
    DoorsFlatTo25Deg2 = 25,
    DoorsFlatTo25Deg3 = 26,
    DoorsFlatTo25Deg4 = 27,
    DoorsFlatTo25Deg5 = 28,
    DoorsFlatTo25Deg6 = 29,

    Count,

    Null = 255,
};
constexpr uint8_t kRegularTunnelTypeCount = 16;

enum class TunnelGroup : uint8_t
{
    uninverted,
    inverted,
    invertedFlying,
};
constexpr uint8_t kTunnelGroupCount = 3;

enum class TunnelStyle : uint8_t
{
    standard = 0,
    square = 1,
    invertedStandard = 2,
    semiInvertedSquare = 3,
    standardWithPath = 4,
};
constexpr uint8_t kTunnelStyleCount = 5;

enum class TunnelSlope : uint8_t
{
    none = 0,
    flat = 1,
    slopeStart = 2,
    slopeEnd = 3,
    flatTo25Deg = 4,
    tall = 5,
    flatNoPath = 6,
};
constexpr uint8_t kTunnelSlopeCount = 7;

struct TunnelEntry
{
    uint8_t height;
    TunnelType type;
};

TunnelType GetTunnelType(TunnelStyle tunnelStyle, TunnelSlope tunnelSlope);

TunnelType GetTunnelTypeDoors(const TrackElement& trackElement, const Direction tunnelDirection);

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelType type);
void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height);
void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, TunnelType type);

inline void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, TunnelStyle tunnelStyle, TunnelSlope tunnelSlope)
{
    PaintUtilPushTunnelLeft(session, height, GetTunnelType(tunnelStyle, tunnelSlope));
}

inline void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, TunnelStyle tunnelStyle, TunnelSlope tunnelSlope)
{
    PaintUtilPushTunnelRight(session, height, GetTunnelType(tunnelStyle, tunnelSlope));
}

inline void PaintUtilPushTunnelRotated(
    PaintSession& session, uint8_t direction, uint16_t height, TunnelStyle tunnelStyle, TunnelSlope tunnelSlope)
{
    PaintUtilPushTunnelRotated(session, direction, height, GetTunnelType(tunnelStyle, tunnelSlope));
}

constexpr std::array<TunnelStyle, EnumValue(TrackGroup::count)> kTrackGroupTunnelStylesStandard = []() consteval {
    std::array<TunnelStyle, EnumValue(TrackGroup::count)> array{};
    array.fill(TunnelStyle::standard);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::square;
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::square;
    return array;
}();

constexpr std::array<TunnelStyle, EnumValue(TrackGroup::count)> kTrackGroupTunnelStylesSquare = []() consteval {
    std::array<TunnelStyle, EnumValue(TrackGroup::count)> array{};
    array.fill(TunnelStyle::square);
    return array;
}();

constexpr std::array<TunnelStyle, EnumValue(TrackGroup::count)> kTrackGroupTunnelStylesInverted = []() consteval {
    std::array<TunnelStyle, EnumValue(TrackGroup::count)> array{};
    array.fill(TunnelStyle::invertedStandard);
    array[EnumValue(TrackGroup::stationEnd)] = TunnelStyle::semiInvertedSquare;
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::square;
    return array;
}();

constexpr std::array<TunnelStyle, EnumValue(TrackGroup::count)> kTrackGroupTunnelStylesSemiInvertedSquare = []() consteval {
    std::array<TunnelStyle, EnumValue(TrackGroup::count)> array{};
    array.fill(TunnelStyle::semiInvertedSquare);
    return array;
}();

constexpr std::array<TunnelStyle, EnumValue(TrackGroup::count)> kTrackGroupTunnelStylesWoodenRollerCoaster = []() consteval {
    std::array<TunnelStyle, EnumValue(TrackGroup::count)> array{};
    array.fill(TunnelStyle::square);
    array[EnumValue(TrackGroup::onridePhoto)] = TunnelStyle::semiInvertedSquare;
    return array;
}();
