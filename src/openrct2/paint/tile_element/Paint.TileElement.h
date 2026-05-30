/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../world/Location.hpp"

namespace OpenRCT2
{
    struct TrackElement;
}

struct PaintSession;

enum edge_t
{
    edgeNE = (1 << 0),
    edgeSE = (1 << 1),
    edgeSW = (1 << 2),
    edgeNW = (1 << 3),
    edgeBottomLeft = edgeSW,
    edgeBottomRight = edgeSE,
    edgeTopLeft = edgeNW,
    edgeTopRight = edgeNE
};

extern const uint16_t kSegmentOffsets[9];

extern bool gShowSupportSegmentHeights;

extern const CoordsXY kBannerBoundBoxes[][2];

extern const uint8_t kPathSlopeToLandSlope[4];

void paintUtilSetGeneralSupportHeight(PaintSession& session, int16_t height);
void paintUtilForceSetGeneralSupportHeight(PaintSession& session, int16_t height, uint8_t slope);
void paintUtilSetSegmentSupportHeight(PaintSession& session, int32_t segments, uint16_t height, uint8_t slope);
uint16_t paintUtilRotateSegments(uint16_t segments, uint8_t rotation);

void tileElementPaintSetup(PaintSession& session, const CoordsXY& mapCoords, bool isTrackPiecePreview = false);

bool paintShouldShowHeightMarkers(const PaintSession& session, uint32_t viewportFlag);
