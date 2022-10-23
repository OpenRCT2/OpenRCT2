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
    EDGE_NE = (1 << 0),
    EDGE_SE = (1 << 1),
    EDGE_SW = (1 << 2),
    EDGE_NW = (1 << 3),
    EDGE_BOTTOMLEFT = EDGE_SW,
    EDGE_BOTTOMRIGHT = EDGE_SE,
    EDGE_TOPLEFT = EDGE_NW,
    EDGE_TOPRIGHT = EDGE_NE
};

enum
{
    SEGMENT_B4 = (1 << 0), // 0
    SEGMENT_CC = (1 << 1), // 6
    SEGMENT_BC = (1 << 2), // 2
    SEGMENT_D4 = (1 << 3), // 8
    SEGMENT_C0 = (1 << 4), // 3
    SEGMENT_D0 = (1 << 5), // 7
    SEGMENT_B8 = (1 << 6), // 1
    SEGMENT_C8 = (1 << 7), // 5
    SEGMENT_C4 = (1 << 8), // 4
};

enum
{
    TUNNEL_0 = 0,
    TUNNEL_1 = 1,
    TUNNEL_2 = 2,
    TUNNEL_INVERTED_3 = 3,
    TUNNEL_INVERTED_4 = 4,
    TUNNEL_INVERTED_5 = 5,
    TUNNEL_SQUARE_FLAT = 6,
    TUNNEL_SQUARE_7 = 7,
    TUNNEL_SQUARE_8 = 8,
    TUNNEL_SQUARE_INVERTED_9 = 9,
    TUNNEL_PATH_AND_MINI_GOLF = 0x0A,
    TUNNEL_PATH_11 = 0x0B,
    TUNNEL_12 = 0x0C,
    TUNNEL_13 = 0x0D,
    TUNNEL_14 = 0x0E,
    TUNNEL_15 = 0x0F,
    REGULAR_TUNNEL_TYPE_COUNT,

    // Ghost train doors
    TUNNEL_DOORS_0 = 16,
    TUNNEL_DOORS_1 = 17,
    TUNNEL_DOORS_2 = 18,
    TUNNEL_DOORS_3 = 19,
    TUNNEL_DOORS_4 = 20,
    TUNNEL_DOORS_5 = 21,
    TUNNEL_DOORS_6 = 22,

    TUNNEL_TYPE_COUNT
};

namespace PaintSessionFlags
{
    constexpr uint8_t PassedSurface = 1u << 0;
    constexpr uint8_t IsTrackPiecePreview = 1u << 1;
} // namespace PaintSessionFlags

extern const int32_t SEGMENTS_ALL;
extern const uint16_t segment_offsets[9];

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
