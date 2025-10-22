/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../paint/track/coaster/FlyingRollerCoasterInverted.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"
#include "TwisterRollerCoaster.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

/** rct2: 0x007C7244, 0x007C7254, 0x007C7264 */
static void FlyingRCTrackStation(
    PaintSession& session, const Ride& ride, [[maybe_unused]] uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex imageIds[4][2] = {
        { 17154, 17150 },
        { 17155, 17151 },
        { 17154, 17150 },
        { 17155, 17151 },
    };

    if (trackElement.GetTrackType() == TrackElemType::EndStation)
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][1]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    else
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][0]), { 0, 0, height },
            { { 0, 6, height + 3 }, { 32, 20, 1 } });
    }
    if (TrackPaintUtilDrawStation2(session, ride, direction, height, trackElement, StationBaseType::a, 0, 9, 11))
    {
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, supportType.metal);
    }
    else if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
    }

    TrackPaintUtilDrawStationTunnel(session, direction, height);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void FlyingRCTrackHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    FlyingRCTrackHalfLoopInvertedUp(session, ride, 3 - trackSequence, direction, height, trackElement, supportType);
}

static void FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    FlyingRCTrackRightFlyingLargeHalfLoopInvertedUp(
        session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

static void FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    FlyingRCTrackLeftFlyingLargeHalfLoopInvertedUp(
        session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionFlyingRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return FlyingRCTrackStation;
        // OpenRCT2-specific paint code
        case TrackElemType::MultiDimUp90ToInvertedFlatQuarterLoop:
            return TwisterRCTrack90DegToInvertedFlatQuarterLoopUp;
            // OpenRCT2-specific track elements
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedUp:
            return TwisterRCTrackLeftLargeHalfLoopUp;
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedUp:
            return TwisterRCTrackRightLargeHalfLoopUp;
        case TrackElemType::FlyerHalfLoopUninvertedDown:
            return FlyingRCTrackHalfLoopUninvertedDown;
        case TrackElemType::LeftFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown;
        case TrackElemType::RightFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown;
        case TrackElemType::LeftTwistDownToUp:
        case TrackElemType::RightTwistDownToUp:
        case TrackElemType::LeftTwistUpToDown:
        case TrackElemType::RightTwistUpToDown:
        case TrackElemType::HalfLoopUp:
        case TrackElemType::HalfLoopDown:
        case TrackElemType::LeftCorkscrewUp:
        case TrackElemType::RightCorkscrewUp:
        case TrackElemType::LeftCorkscrewDown:
        case TrackElemType::RightCorkscrewDown:
        case TrackElemType::LeftBarrelRollUpToDown:
        case TrackElemType::RightBarrelRollUpToDown:
        case TrackElemType::LeftBarrelRollDownToUp:
        case TrackElemType::RightBarrelRollDownToUp:
        case TrackElemType::LeftLargeHalfLoopUp:
        case TrackElemType::RightLargeHalfLoopUp:
        case TrackElemType::LeftLargeHalfLoopDown:
        case TrackElemType::RightLargeHalfLoopDown:
        case TrackElemType::Up90ToInvertedFlatQuarterLoop:
        case TrackElemType::InvertedFlatToDown90QuarterLoop:
        case TrackElemType::LeftLargeCorkscrewUp:
        case TrackElemType::RightLargeCorkscrewUp:
        case TrackElemType::LeftLargeCorkscrewDown:
        case TrackElemType::RightLargeCorkscrewDown:
        case TrackElemType::LeftMediumHalfLoopUp:
        case TrackElemType::RightMediumHalfLoopUp:
        case TrackElemType::LeftMediumHalfLoopDown:
        case TrackElemType::RightMediumHalfLoopDown:
        case TrackElemType::LeftZeroGRollUp:
        case TrackElemType::RightZeroGRollUp:
        case TrackElemType::LeftZeroGRollDown:
        case TrackElemType::RightZeroGRollDown:
        case TrackElemType::LeftLargeZeroGRollUp:
        case TrackElemType::RightLargeZeroGRollUp:
        case TrackElemType::LeftLargeZeroGRollDown:
        case TrackElemType::RightLargeZeroGRollDown:
        case TrackElemType::LeftEighthDiveLoopUpToOrthogonal:
        case TrackElemType::RightEighthDiveLoopUpToOrthogonal:
        case TrackElemType::LeftEighthDiveLoopDownToDiag:
        case TrackElemType::RightEighthDiveLoopDownToDiag:
            return TrackPaintFunctionDummy;
        default:
            return GetTrackPaintFunctionTwisterRC(trackType);
    }
}
