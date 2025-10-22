/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../paint/track/coaster/FlyingRollerCoasterInverted.h"
#include "../../../ride/RideData.h"
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

    if (trackElement.GetTrackType() == TrackElemType::endStation)
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
    FlyingRCInverted::trackPaintHalfLoopInvertedUp(
        session, ride, 3 - trackSequence, direction, height, trackElement, supportType);
}

static void FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    FlyingRCInverted::trackPaintRightFlyingLargeHalfLoopInvertedUp(
        session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

static void FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    FlyingRCInverted::trackPaintLeftFlyingLargeHalfLoopInvertedUp(
        session, ride, 6 - trackSequence, direction, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionFlyingRC(TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::endStation:
        case TrackElemType::beginStation:
        case TrackElemType::middleStation:
            return FlyingRCTrackStation;
        // OpenRCT2-specific paint code
        case TrackElemType::multiDimUp90ToInvertedFlatQuarterLoop:
            return TwisterRCTrack90DegToInvertedFlatQuarterLoopUp;
            // OpenRCT2-specific track elements
        case TrackElemType::leftFlyerLargeHalfLoopUninvertedUp:
            return TwisterRCTrackLeftLargeHalfLoopUp;
        case TrackElemType::rightFlyerLargeHalfLoopUninvertedUp:
            return TwisterRCTrackRightLargeHalfLoopUp;
        case TrackElemType::flyerHalfLoopUninvertedDown:
            return FlyingRCTrackHalfLoopUninvertedDown;
        case TrackElemType::leftFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackLeftFlyingLargeHalfLoopUninvertedDown;
        case TrackElemType::rightFlyerLargeHalfLoopUninvertedDown:
            return FlyingRCTrackRightFlyingLargeHalfLoopUninvertedDown;
        case TrackElemType::leftTwistDownToUp:
        case TrackElemType::rightTwistDownToUp:
        case TrackElemType::leftTwistUpToDown:
        case TrackElemType::rightTwistUpToDown:
        case TrackElemType::halfLoopUp:
        case TrackElemType::halfLoopDown:
        case TrackElemType::leftCorkscrewUp:
        case TrackElemType::rightCorkscrewUp:
        case TrackElemType::leftCorkscrewDown:
        case TrackElemType::rightCorkscrewDown:
        case TrackElemType::leftBarrelRollUpToDown:
        case TrackElemType::rightBarrelRollUpToDown:
        case TrackElemType::leftBarrelRollDownToUp:
        case TrackElemType::rightBarrelRollDownToUp:
        case TrackElemType::leftLargeHalfLoopUp:
        case TrackElemType::rightLargeHalfLoopUp:
        case TrackElemType::leftLargeHalfLoopDown:
        case TrackElemType::rightLargeHalfLoopDown:
        case TrackElemType::up90ToInvertedFlatQuarterLoop:
        case TrackElemType::invertedFlatToDown90QuarterLoop:
        case TrackElemType::leftLargeCorkscrewUp:
        case TrackElemType::rightLargeCorkscrewUp:
        case TrackElemType::leftLargeCorkscrewDown:
        case TrackElemType::rightLargeCorkscrewDown:
        case TrackElemType::leftMediumHalfLoopUp:
        case TrackElemType::rightMediumHalfLoopUp:
        case TrackElemType::leftMediumHalfLoopDown:
        case TrackElemType::rightMediumHalfLoopDown:
        case TrackElemType::leftZeroGRollUp:
        case TrackElemType::rightZeroGRollUp:
        case TrackElemType::leftZeroGRollDown:
        case TrackElemType::rightZeroGRollDown:
        case TrackElemType::leftLargeZeroGRollUp:
        case TrackElemType::rightLargeZeroGRollUp:
        case TrackElemType::leftLargeZeroGRollDown:
        case TrackElemType::rightLargeZeroGRollDown:
        case TrackElemType::leftEighthDiveLoopUpToOrthogonal:
        case TrackElemType::rightEighthDiveLoopUpToOrthogonal:
        case TrackElemType::leftEighthDiveLoopDownToDiag:
        case TrackElemType::rightEighthDiveLoopDownToDiag:
            return TrackPaintFunctionDummy;
        default:
            return GetTrackPaintFunctionTwisterRC(trackType);
    }
}
