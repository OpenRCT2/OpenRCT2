/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Segment.h"

#include <cstdint>

struct CoordsXY;
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

extern const OpenRCT2::PaintSegment kSegmentOffsets[9];

extern bool gShowSupportSegmentHeights;

extern const CoordsXY kBannerBoundBoxes[][2];

extern const uint8_t kPathSlopeToLandSlope[4];

namespace OpenRCT2
{
    enum class ViewportFlag : uint32_t;
}

void PaintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height);
void PaintUtilForceSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope);
void PaintUtilSetSegmentSupportHeight(PaintSession& session, OpenRCT2::PaintSegments segments, uint16_t height, uint8_t slope);
OpenRCT2::PaintSegments PaintUtilRotateSegments(OpenRCT2::PaintSegments segments, uint8_t rotation);

void TileElementPaintSetup(PaintSession& session, const CoordsXY& mapCoords, bool isTrackPiecePreview = false);

bool PaintShouldShowHeightMarkers(const PaintSession& session, OpenRCT2::ViewportFlag viewportFlag);
