/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

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

extern const uint16_t kSegmentOffsets[9];

extern bool gShowSupportSegmentHeights;

extern const CoordsXY kBannerBoundBoxes[][2];

extern const uint8_t kPathSlopeToLandSlope[4];

void PaintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height);
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
