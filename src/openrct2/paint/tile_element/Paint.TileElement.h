/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../common.h"
#include "../../world/Map.h"

struct PaintSession;

enum edge_t
{
    EdgeNE = (1 << 0),
    EdgeSE = (1 << 1),
    EdgeSW = (1 << 2),
    EdgeNW = (1 << 3),
    EdgeBottomLeft = EdgeSW,
    EdgeBottomRight = EdgeSE,
    EdgeTopLeft = EdgeNW,
    EdgeTopRight = EdgeNE
};

enum
{
    SegmentB4 = (1 << 0), // 0
    SegmentCC = (1 << 1), // 6
    SegmentBC = (1 << 2), // 2
    SegmentD4 = (1 << 3), // 8
    SegmentC0 = (1 << 4), // 3
    SegmentD0 = (1 << 5), // 7
    SegmentB8 = (1 << 6), // 1
    SegmentC8 = (1 << 7), // 5
    SegmentC4 = (1 << 8), // 4
};

enum
{
    Tunnel0 = 0,
    Tunnel1 = 1,
    Tunnel2 = 2,
    TunnelInverted3 = 3,
    TunnelInverted4 = 4,
    TunnelInverted5 = 5,
    TunnelSquareFlat = 6,
    TunnelSquare7 = 7,
    TunnelSquare8 = 8,
    TunnelSquareInverted9 = 9,
    TunnelPathAndMiniGolf = 0x0A,
    TunnelPath11 = 0x0B,
    Tunnel12 = 0x0C,
    Tunnel13 = 0x0D,
    Tunnel14 = 0x0E,
    Tunnel15 = 0x0F,
    RegularTunnelTypeCount,

    // Ghost train doors
    TunnelDoors0 = 16,
    TunnelDoors1 = 17,
    TunnelDoors2 = 18,
    TunnelDoors3 = 19,
    TunnelDoors4 = 20,
    TunnelDoors5 = 21,
    TunnelDoors6 = 22,

    TunnelTypeCount
};

namespace PaintSessionFlags
{
    constexpr uint8_t PassedSurface = 1u << 0;
    constexpr uint8_t IsTrackPiecePreview = 1u << 1;
} // namespace PaintSessionFlags

extern const int32_t SegmentsAll;
extern const uint16_t SegmentOffsets[9];

extern bool gShowSupportSegmentHeights;

extern const CoordsXY BannerBoundBoxes[][2];

extern const uint8_t PathSlopeToLandSlope[4];

void PaintUtilPushTunnelLeft(PaintSession& session, uint16_t height, uint8_t type);
void PaintUtilPushTunnelRight(PaintSession& session, uint16_t height, uint8_t type);
void PaintUtilSetVerticalTunnel(PaintSession& session, uint16_t height);

void PaintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope);
void PaintUtilForceSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope);
void PaintUtilSetSegmentSupportHeight(PaintSession& session, int32_t segments, uint16_t height, uint8_t slope);
uint16_t PaintUtilRotateSegments(uint16_t segments, uint8_t rotation);

void TileElementPaintSetup(PaintSession& session, const CoordsXY& mapCoords, bool isTrackPiecePreview = false);

void PaintEntrance(PaintSession& session, uint8_t direction, int32_t height, const EntranceElement& entranceElement);
void PaintBanner(PaintSession& session, uint8_t direction, int32_t height, const BannerElement& bannerElement);
void PaintSurface(PaintSession& session, uint8_t direction, uint16_t height, const SurfaceElement& tileElement);
void PaintPath(PaintSession& session, uint16_t height, const PathElement& tileElement);
void PaintSmallScenery(PaintSession& session, uint8_t direction, int32_t height, const SmallSceneryElement& sceneryElement);
void PaintWall(PaintSession& session, uint8_t direction, int32_t height, const WallElement& tileElement);
void PaintLargeScenery(PaintSession& session, uint8_t direction, uint16_t height, const LargeSceneryElement& tileElement);
void PaintTrack(PaintSession& session, uint8_t direction, int32_t height, const TrackElement& tileElement);

bool PaintShouldShowHeightMarkers(const PaintSession& session, const uint32_t viewportFlag);
