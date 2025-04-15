/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../tile_element/Paint.Tunnel.h"
#include "../../tile_element/Segment.h"

using namespace OpenRCT2;

static void WoodenRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackPaintSpriteSupportColoursWithChild(session, ride, trackSequence, direction, height, trackElement, supportType);
    WoodenASupportsPaintSetupRotated(
        session, supportType.wooden, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);
    PaintUtilPushTunnelRotated(session, direction, height, TunnelGroup::Square, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void WoodenRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
}

// Stylistically, this coaster is _very_ similar to the regular Wooden Roller Coaster.
// The only difference is the degree of the banking.
// As such, all non-banked pieces are simply drawn as regular wooden roller coaster pieces.
TrackPaintFunction GetTrackPaintFunctionClassicWoodenTwisterRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::LeftBank:
            return WoodenRCTrackLeftBank;
        case TrackElemType::RightBank:
            return WoodenRCTrackRightBank;
        default:
            return GetTrackPaintFunctionWoodenRC(trackType);
    }
}
