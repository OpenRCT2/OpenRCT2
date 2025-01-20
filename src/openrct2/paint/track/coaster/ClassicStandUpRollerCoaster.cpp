/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <cstdint>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

static constexpr ImageIndex kClassicStandUpRcFlatToRightBank0 = SPR_CSG_BEGIN + 67009;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBank2 = SPR_CSG_BEGIN + 67010;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBank0 = SPR_CSG_BEGIN + 67011;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBank2 = SPR_CSG_BEGIN + 67012;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBank3 = SPR_CSG_BEGIN + 67013;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBank1 = SPR_CSG_BEGIN + 67014;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBank3 = SPR_CSG_BEGIN + 67015;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBank1 = SPR_CSG_BEGIN + 67016;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked0_0 = SPR_CSG_BEGIN + 67017;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked0_1 = SPR_CSG_BEGIN + 67018;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked0_2 = SPR_CSG_BEGIN + 67019;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked0_3 = SPR_CSG_BEGIN + 67020;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked0_4 = SPR_CSG_BEGIN + 67021;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked1_0 = SPR_CSG_BEGIN + 67022;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked1_1 = SPR_CSG_BEGIN + 67023;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked1_2 = SPR_CSG_BEGIN + 67024;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked1_3 = SPR_CSG_BEGIN + 67025;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked1_4 = SPR_CSG_BEGIN + 67026;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked2_0 = SPR_CSG_BEGIN + 67027;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked2_1 = SPR_CSG_BEGIN + 67028;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked2_2 = SPR_CSG_BEGIN + 67029;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked2_3 = SPR_CSG_BEGIN + 67030;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked2_4 = SPR_CSG_BEGIN + 67031;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked3_0 = SPR_CSG_BEGIN + 67032;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked3_1 = SPR_CSG_BEGIN + 67033;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked3_2 = SPR_CSG_BEGIN + 67034;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked3_3 = SPR_CSG_BEGIN + 67035;
static constexpr ImageIndex kClassicStandUpRcRightTurn5TilesBanked3_4 = SPR_CSG_BEGIN + 67036;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatRightBanked0 = SPR_CSG_BEGIN + 67037;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatRightBanked1 = SPR_CSG_BEGIN + 67038;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatRightBanked2 = SPR_CSG_BEGIN + 67039;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatRightBanked3 = SPR_CSG_BEGIN + 67040;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatLeftBanked0 = SPR_CSG_BEGIN + 67041;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatLeftBanked1 = SPR_CSG_BEGIN + 67042;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatLeftBanked2 = SPR_CSG_BEGIN + 67043;
static constexpr ImageIndex kClassicStandUpRc25DegUpToFlatLeftBanked3 = SPR_CSG_BEGIN + 67044;
static constexpr ImageIndex kClassicStandUpRcLeftBankedTo25DegUp0 = SPR_CSG_BEGIN + 67045;
static constexpr ImageIndex kClassicStandUpRcLeftBankedTo25DegUp1 = SPR_CSG_BEGIN + 67046;
static constexpr ImageIndex kClassicStandUpRcLeftBankedTo25DegUp2 = SPR_CSG_BEGIN + 67047;
static constexpr ImageIndex kClassicStandUpRcLeftBankedTo25DegUp3 = SPR_CSG_BEGIN + 67048;
static constexpr ImageIndex kClassicStandUpRcRightBankedTo25DegUp0 = SPR_CSG_BEGIN + 67049;
static constexpr ImageIndex kClassicStandUpRcRightBankedTo25DegUp1 = SPR_CSG_BEGIN + 67050;
static constexpr ImageIndex kClassicStandUpRcRightBankedTo25DegUp2 = SPR_CSG_BEGIN + 67051;
static constexpr ImageIndex kClassicStandUpRcRightBankedTo25DegUp3 = SPR_CSG_BEGIN + 67052;
static constexpr ImageIndex kClassicStandUpRcLeftBanked0 = SPR_CSG_BEGIN + 67053;
static constexpr ImageIndex kClassicStandUpRcLeftBanked1 = SPR_CSG_BEGIN + 67054;
static constexpr ImageIndex kClassicStandUpRcLeftBanked2 = SPR_CSG_BEGIN + 67055;
static constexpr ImageIndex kClassicStandUpRcLeftBanked3 = SPR_CSG_BEGIN + 67056;

static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked0_0 = SPR_CSG_BEGIN + 67229;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked0_1 = SPR_CSG_BEGIN + 67230;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked0_2 = SPR_CSG_BEGIN + 67231;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked1_0 = SPR_CSG_BEGIN + 67232;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked1_1 = SPR_CSG_BEGIN + 67233;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked1_2 = SPR_CSG_BEGIN + 67234;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked2_0 = SPR_CSG_BEGIN + 67235;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked2_1 = SPR_CSG_BEGIN + 67236;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked2_2 = SPR_CSG_BEGIN + 67237;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked3_0 = SPR_CSG_BEGIN + 67238;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked3_1 = SPR_CSG_BEGIN + 67239;
static constexpr ImageIndex kClassicStandUpRcRightTurn3TilesBanked3_2 = SPR_CSG_BEGIN + 67240;

static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag0_0 = SPR_CSG_BEGIN + 67361;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag0_1 = SPR_CSG_BEGIN + 67362;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag0_2 = SPR_CSG_BEGIN + 67363;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag0_3 = SPR_CSG_BEGIN + 67364;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag1_0 = SPR_CSG_BEGIN + 67365;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag1_1 = SPR_CSG_BEGIN + 67366;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag1_2 = SPR_CSG_BEGIN + 67367;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag1_3 = SPR_CSG_BEGIN + 67368;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag2_0 = SPR_CSG_BEGIN + 67369;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag2_1 = SPR_CSG_BEGIN + 67370;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag2_2 = SPR_CSG_BEGIN + 67371;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag2_3 = SPR_CSG_BEGIN + 67372;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag3_0 = SPR_CSG_BEGIN + 67373;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag3_1 = SPR_CSG_BEGIN + 67374;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag3_2 = SPR_CSG_BEGIN + 67375;
static constexpr ImageIndex kClassicStandUpRcRightBankedOrthogonalToDiag3_3 = SPR_CSG_BEGIN + 67376;

static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag0_0 = SPR_CSG_BEGIN + 67377;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag0_1 = SPR_CSG_BEGIN + 67378;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag0_2 = SPR_CSG_BEGIN + 67379;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag0_3 = SPR_CSG_BEGIN + 67380;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag1_0 = SPR_CSG_BEGIN + 67381;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag1_1 = SPR_CSG_BEGIN + 67382;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag1_2 = SPR_CSG_BEGIN + 67383;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag1_3 = SPR_CSG_BEGIN + 67384;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag2_0 = SPR_CSG_BEGIN + 67385;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag2_1 = SPR_CSG_BEGIN + 67386;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag2_2 = SPR_CSG_BEGIN + 67387;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag2_3 = SPR_CSG_BEGIN + 67388;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag3_0 = SPR_CSG_BEGIN + 67389;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag3_1 = SPR_CSG_BEGIN + 67390;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag3_2 = SPR_CSG_BEGIN + 67391;
static constexpr ImageIndex kClassicStandUpRcLeftBankedOrthogonalToDiag3_3 = SPR_CSG_BEGIN + 67392;

static constexpr ImageIndex kClassicStandUpRcLeftBankedStraightDiag0 = SPR_CSG_BEGIN + 67421;
static constexpr ImageIndex kClassicStandUpRcLeftBankedStraightDiag1 = SPR_CSG_BEGIN + 67422;
static constexpr ImageIndex kClassicStandUpRcLeftBankedStraightDiag2 = SPR_CSG_BEGIN + 67423;
static constexpr ImageIndex kClassicStandUpRcLeftBankedStraightDiag3 = SPR_CSG_BEGIN + 67424;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBankDiag0 = SPR_CSG_BEGIN + 67425;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBankDiag1 = SPR_CSG_BEGIN + 67426;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBankDiag2 = SPR_CSG_BEGIN + 67427;
static constexpr ImageIndex kClassicStandUpRcFlatToLeftBankDiag3 = SPR_CSG_BEGIN + 67428;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBankDiag0 = SPR_CSG_BEGIN + 67429;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBankDiag1 = SPR_CSG_BEGIN + 67430;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBankDiag2 = SPR_CSG_BEGIN + 67431;
static constexpr ImageIndex kClassicStandUpRcFlatToRightBankDiag3 = SPR_CSG_BEGIN + 67432;
static constexpr ImageIndex kClassicStandUpRc25DegUpToLeftBankDiag0 = SPR_CSG_BEGIN + 67433;
static constexpr ImageIndex kClassicStandUpRc25DegUpToLeftBankDiag1 = SPR_CSG_BEGIN + 67434;
static constexpr ImageIndex kClassicStandUpRc25DegUpToLeftBankDiag2 = SPR_CSG_BEGIN + 67435;
static constexpr ImageIndex kClassicStandUpRc25DegUpToLeftBankDiag3 = SPR_CSG_BEGIN + 67436;
static constexpr ImageIndex kClassicStandUpRc25DegUpToRightBankDiag0 = SPR_CSG_BEGIN + 67437;
static constexpr ImageIndex kClassicStandUpRc25DegUpToRightBankDiag1 = SPR_CSG_BEGIN + 67438;
static constexpr ImageIndex kClassicStandUpRc25DegUpToRightBankDiag2 = SPR_CSG_BEGIN + 67439;
static constexpr ImageIndex kClassicStandUpRc25DegUpToRightBankDiag3 = SPR_CSG_BEGIN + 67440;
static constexpr ImageIndex kClassicStandUpRcLeftBankTo25DegUpDiag0 = SPR_CSG_BEGIN + 67441;
static constexpr ImageIndex kClassicStandUpRcLeftBankTo25DegUpDiag1 = SPR_CSG_BEGIN + 67442;
static constexpr ImageIndex kClassicStandUpRcLeftBankTo25DegUpDiag2 = SPR_CSG_BEGIN + 67443;
static constexpr ImageIndex kClassicStandUpRcLeftBankTo25DegUpDiag3 = SPR_CSG_BEGIN + 67444;
static constexpr ImageIndex kClassicStandUpRcRightBankTo25DegUpDiag0 = SPR_CSG_BEGIN + 67445;
static constexpr ImageIndex kClassicStandUpRcRightBankTo25DegUpDiag1 = SPR_CSG_BEGIN + 67446;
static constexpr ImageIndex kClassicStandUpRcRightBankTo25DegUpDiag2 = SPR_CSG_BEGIN + 67447;
static constexpr ImageIndex kClassicStandUpRcRightBankTo25DegUpDiag3 = SPR_CSG_BEGIN + 67448;

static void classicStandUpRCTrackFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRcFlatToLeftBank0,
        kClassicStandUpRcFlatToLeftBank1,
        kClassicStandUpRcFlatToLeftBank2,
        kClassicStandUpRcFlatToLeftBank3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRcFlatToRightBank0,
        kClassicStandUpRcFlatToRightBank1,
        kClassicStandUpRcFlatToRightBank2,
        kClassicStandUpRcFlatToRightBank3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    return classicStandUpRCTrackFlatToRightBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    return classicStandUpRCTrackFlatToLeftBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackBankedRightQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
        {
            constexpr ImageIndex map[kNumOrthogonalDirections] = {
                kClassicStandUpRcRightTurn5TilesBanked0_0,
                kClassicStandUpRcRightTurn5TilesBanked1_0,
                kClassicStandUpRcRightTurn5TilesBanked2_0,
                kClassicStandUpRcRightTurn5TilesBanked3_0,
            };

            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        }
        case 1:
            break;

        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked0_1),
                        { 0, 16, height }, { 32, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked1_1),
                        { 0, 16, height }, { 32, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked2_1),
                        { 0, 0, height }, { 32, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked3_1),
                        { 0, 0, height }, { 32, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::rightCorner, PaintSegment::centre,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;

        case 3:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked0_2),
                        { 0, 0, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked1_2),
                        { 16, 0, height }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked2_2),
                        { 16, 16, height }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked3_2),
                        { 0, 16, height }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;

        case 4:
            break;

        case 5:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked0_3),
                        { 16, 0, height }, { 16, 32, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked1_3),
                        { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked2_3),
                        { 0, 0, height }, { 16, 32, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(kClassicStandUpRcRightTurn5TilesBanked3_3),
                        { 16, 0, height }, { 16, 32, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::bottomCorner, PaintSegment::centre,
                        PaintSegment::bottomLeftSide, PaintSegment::topLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;

        case 6:
        {
            constexpr ImageIndex map[kNumOrthogonalDirections] = {
                kClassicStandUpRcRightTurn5TilesBanked0_4,
                kClassicStandUpRcRightTurn5TilesBanked1_4,
                kClassicStandUpRcRightTurn5TilesBanked2_4,
                kClassicStandUpRcRightTurn5TilesBanked3_4,
            };

            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(map[direction]), { 6, 0, height }, { 20, 32, 3 });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            if (direction == 1 || direction == 0)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackBankedLeftQuarterTurn5(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
    direction = (direction + 1) % kNumOrthogonalDirections;
    return classicStandUpRCTrackBankedRightQuarterTurn5(
        session, ride, trackSequence, direction, height, trackElement, supportType);
}

static void classicStandUpRCTrackLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRcLeftBankedTo25DegUp0,
        kClassicStandUpRcLeftBankedTo25DegUp1,
        kClassicStandUpRcLeftBankedTo25DegUp2,
        kClassicStandUpRcLeftBankedTo25DegUp3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 3, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void classicStandUpRCTrackRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRcRightBankedTo25DegUp0,
        kClassicStandUpRcRightBankedTo25DegUp1,
        kClassicStandUpRcRightBankedTo25DegUp2,
        kClassicStandUpRcRightBankedTo25DegUp3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 3, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void classicStandUpRCTrack25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRc25DegUpToFlatLeftBanked0,
        kClassicStandUpRc25DegUpToFlatLeftBanked1,
        kClassicStandUpRc25DegUpToFlatLeftBanked2,
        kClassicStandUpRc25DegUpToFlatLeftBanked3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void classicStandUpRCTrack25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRc25DegUpToFlatRightBanked0,
        kClassicStandUpRc25DegUpToFlatRightBanked1,
        kClassicStandUpRc25DegUpToFlatRightBanked2,
        kClassicStandUpRc25DegUpToFlatRightBanked3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 6, height, session.SupportColours);
    }

    if (direction == 0 || direction == 3)
    {
        PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
    }
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

static void classicStandUpRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrack25DegUpToRightBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrack25DegUpToLeftBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackRightBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackLeftBankTo25DegUp(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[kNumOrthogonalDirections] = {
        kClassicStandUpRcLeftBanked0,
        kClassicStandUpRcLeftBanked1,
        kClassicStandUpRcLeftBanked2,
        kClassicStandUpRcLeftBanked3,
    };

    PaintAddImageAsParentRotated(
        session, direction, session.TrackColours.WithIndex(map[direction]), { 0, 6, height }, { 32, 20, 3 });

    if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
    {
        MetalASupportsPaintSetupRotated(
            session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
    }

    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
    PaintUtilSetSegmentSupportHeight(session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    return classicStandUpRCTrackLeftBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackLeftEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[5][kNumOrthogonalDirections] = {
        { kClassicStandUpRcLeftBankedOrthogonalToDiag0_0, kClassicStandUpRcLeftBankedOrthogonalToDiag1_0,
          kClassicStandUpRcLeftBankedOrthogonalToDiag2_0, kClassicStandUpRcLeftBankedOrthogonalToDiag3_0 },
        { kClassicStandUpRcLeftBankedOrthogonalToDiag0_1, kClassicStandUpRcLeftBankedOrthogonalToDiag1_1,
          kClassicStandUpRcLeftBankedOrthogonalToDiag2_1, kClassicStandUpRcLeftBankedOrthogonalToDiag3_1 },
        { kClassicStandUpRcLeftBankedOrthogonalToDiag0_2, kClassicStandUpRcLeftBankedOrthogonalToDiag1_2,
          kClassicStandUpRcLeftBankedOrthogonalToDiag2_2, kClassicStandUpRcLeftBankedOrthogonalToDiag3_2 },
        { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined },
        { kClassicStandUpRcLeftBankedOrthogonalToDiag0_3, kClassicStandUpRcLeftBankedOrthogonalToDiag1_3,
          kClassicStandUpRcLeftBankedOrthogonalToDiag2_3, kClassicStandUpRcLeftBankedOrthogonalToDiag3_3 },
    };

    const auto imageId = map[trackSequence][direction];

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide, PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::BottomCorner, direction, 0, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

static void classicStandUpRCTrackRightEighthBankToDiag(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[5][kNumOrthogonalDirections] = {
        { kClassicStandUpRcRightBankedOrthogonalToDiag0_0, kClassicStandUpRcRightBankedOrthogonalToDiag1_0,
          kClassicStandUpRcRightBankedOrthogonalToDiag2_0, kClassicStandUpRcRightBankedOrthogonalToDiag3_0 },
        { kClassicStandUpRcRightBankedOrthogonalToDiag0_1, kClassicStandUpRcRightBankedOrthogonalToDiag1_1,
          kClassicStandUpRcRightBankedOrthogonalToDiag2_1, kClassicStandUpRcRightBankedOrthogonalToDiag3_1 },
        { kClassicStandUpRcRightBankedOrthogonalToDiag0_2, kClassicStandUpRcRightBankedOrthogonalToDiag1_2,
          kClassicStandUpRcRightBankedOrthogonalToDiag2_2, kClassicStandUpRcRightBankedOrthogonalToDiag3_2 },
        { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined },
        { kClassicStandUpRcRightBankedOrthogonalToDiag0_3, kClassicStandUpRcRightBankedOrthogonalToDiag1_3,
          kClassicStandUpRcRightBankedOrthogonalToDiag2_3, kClassicStandUpRcRightBankedOrthogonalToDiag3_3 },
    };

    const auto imageId = map[trackSequence][direction];

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                { { 0, 6, height }, { 32, 20, 3 } });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            if (direction == 0 || direction == 3)
            {
                PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session, PaintUtilRotateSegments(BlockedSegments::kStraightFlat, direction), 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 32, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 34, 16, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 32, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 4, 4, height }, { 28, 28, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 16, 16, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 4:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 0, height }, { 16, 16, 3 } });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 0, height }, { 16, 16, 3 } });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 0, 16, height }, { 16, 18, 3 } });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height },
                        { { 16, 16, height }, { 16, 16, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide,
                        PaintSegment::bottomLeftSide, PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
    }
}

static void classicStandUpRCTrackLeftEighthDiagBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    direction = (direction + 2) % kNumOrthogonalDirections;
    classicStandUpRCTrackRightEighthBankToDiag(session, ride, trackSequence, direction, height, trackElement, supportType);
}

static void classicStandUpRCTrackRightEighthDiagBankToOrthogonal(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
    direction = (direction + 3) % kNumOrthogonalDirections;
    classicStandUpRCTrackLeftEighthBankToDiag(session, ride, trackSequence, direction, height, trackElement, supportType);
}

static void classicStandUpRCTrackRightBankedQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr ImageIndex map[4][kNumOrthogonalDirections] = {
        { kClassicStandUpRcRightTurn3TilesBanked0_0, kClassicStandUpRcRightTurn3TilesBanked1_0,
          kClassicStandUpRcRightTurn3TilesBanked2_0, kClassicStandUpRcRightTurn3TilesBanked3_0 },
        { kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined, kImageIndexUndefined },
        { kClassicStandUpRcRightTurn3TilesBanked0_1, kClassicStandUpRcRightTurn3TilesBanked1_1,
          kClassicStandUpRcRightTurn3TilesBanked2_1, kClassicStandUpRcRightTurn3TilesBanked3_1 },

        { kClassicStandUpRcRightTurn3TilesBanked0_2, kClassicStandUpRcRightTurn3TilesBanked1_2,
          kClassicStandUpRcRightTurn3TilesBanked2_2, kClassicStandUpRcRightTurn3TilesBanked3_2 },
    };

    auto imageId = map[trackSequence][direction];

    switch (trackSequence)
    {
        case 0:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageId), { 0, 6, height }, { 32, 20, 3 });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            switch (direction)
            {
                case 0:
                    PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
                case 3:
                    PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            break;
        case 2:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 16, 16, height }, { 16, 16, 3 });
                    break;
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 16, height }, { 16, 16, 3 });
                    break;
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 0, 0, height }, { 16, 16, 3 });
                    break;
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(imageId), { 16, 0, height }, { 16, 16, 3 });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageId), { 6, 0, height }, { 20, 32, 3 });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::Centre, direction, 0, height, session.SupportColours);
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackLeftBankedQuarterTurn3(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
    direction = (direction + 1) % kNumOrthogonalDirections;
    classicStandUpRCTrackRightBankedQuarterTurn3(session, ride, trackSequence, direction, height, trackElement, supportType);
}

template<ImageIndex img0, ImageIndex img1, ImageIndex img2, ImageIndex img3>
static inline void classicStandUpRCTrackDiagFlatWithBank(
    PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img3), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }

            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img0), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }

            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img2), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }

            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img1), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 0, height, session.SupportColours);

            break;
    }

    PaintUtilSetSegmentSupportHeight(
        session, PaintUtilRotateSegments(BlockedSegments::kDiagStraightFlat[trackSequence], direction), 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void classicStandUpRCTrackDiagFlatToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiagFlatWithBank<
        kClassicStandUpRcFlatToLeftBankDiag0, kClassicStandUpRcFlatToLeftBankDiag1, kClassicStandUpRcFlatToLeftBankDiag2,
        kClassicStandUpRcFlatToLeftBankDiag3>(session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiagFlatToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiagFlatWithBank<
        kClassicStandUpRcFlatToRightBankDiag0, kClassicStandUpRcFlatToRightBankDiag1, kClassicStandUpRcFlatToRightBankDiag2,
        kClassicStandUpRcFlatToRightBankDiag3>(session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiagLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return classicStandUpRCTrackDiagFlatToRightBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackDiagRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return classicStandUpRCTrackDiagFlatToLeftBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

static void classicStandUpRCTrackDiagLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiagFlatWithBank<
        kClassicStandUpRcLeftBankedStraightDiag0, kClassicStandUpRcLeftBankedStraightDiag1,
        kClassicStandUpRcLeftBankedStraightDiag2, kClassicStandUpRcLeftBankedStraightDiag3>(
        session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiagRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    return classicStandUpRCTrackDiagLeftBank(
        session, ride, trackSequence, (direction + 2) % kNumOrthogonalDirections, height, trackElement, supportType);
}

template<ImageIndex img0, ImageIndex img1, ImageIndex img2, ImageIndex img3>
static void classicStandUpRCTrackDiagFlatTo25DegUpBanked(
    PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img3), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img0), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img2), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img1), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 4, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

static void classicStandUpRCTrackDiagLeftBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiagFlatTo25DegUpBanked<
        kClassicStandUpRcLeftBankTo25DegUpDiag0, kClassicStandUpRcLeftBankTo25DegUpDiag1,
        kClassicStandUpRcLeftBankTo25DegUpDiag2, kClassicStandUpRcLeftBankTo25DegUpDiag3>(
        session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiagRightBankTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiagFlatTo25DegUpBanked<
        kClassicStandUpRcRightBankTo25DegUpDiag0, kClassicStandUpRcRightBankTo25DegUpDiag1,
        kClassicStandUpRcRightBankTo25DegUpDiag2, kClassicStandUpRcRightBankTo25DegUpDiag3>(
        session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiag25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    direction = (direction + 2) % kNumOrthogonalDirections;
    classicStandUpRCTrackDiagRightBankTo25DegUp(session, ride, trackSequence, direction, height, trackElement, supportType);
}

static void classicStandUpRCTrackDiag25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    direction = (direction + 2) % kNumOrthogonalDirections;
    classicStandUpRCTrackDiagLeftBankTo25DegUp(session, ride, trackSequence, direction, height, trackElement, supportType);
}

template<ImageIndex img0, ImageIndex img1, ImageIndex img2, ImageIndex img3>
static void classicStandUpRCTrackDiag25DegUpToFlatBanked(
    PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height, SupportType supportType)
{
    switch (trackSequence)
    {
        case 0:
            switch (direction)
            {
                case 3:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img3), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::rightCorner, PaintSegment::centre, PaintSegment::topRightSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 1:
            switch (direction)
            {
                case 0:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img0), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::topCorner, PaintSegment::centre, PaintSegment::topLeftSide, PaintSegment::topRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 2:
            switch (direction)
            {
                case 2:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img2), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottomCorner, PaintSegment::centre, PaintSegment::bottomLeftSide,
                        PaintSegment::bottomRightSide),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            switch (direction)
            {
                case 1:
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex(img1), { -16, -16, height },
                        { { -16, -16, height }, { 32, 32, 3 } });
                    break;
            }
            MetalBSupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::LeftCorner, direction, 8, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::leftCorner, PaintSegment::centre, PaintSegment::topLeftSide,
                        PaintSegment::bottomLeftSide),
                    direction),
                0xFFFF, 0);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

static void classicStandUpRCTrackDiag25DegUpToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiag25DegUpToFlatBanked<
        kClassicStandUpRc25DegUpToLeftBankDiag0, kClassicStandUpRc25DegUpToLeftBankDiag1,
        kClassicStandUpRc25DegUpToLeftBankDiag2, kClassicStandUpRc25DegUpToLeftBankDiag3>(
        session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiag25DegUpToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    classicStandUpRCTrackDiag25DegUpToFlatBanked<
        kClassicStandUpRc25DegUpToRightBankDiag0, kClassicStandUpRc25DegUpToRightBankDiag1,
        kClassicStandUpRc25DegUpToRightBankDiag2, kClassicStandUpRc25DegUpToRightBankDiag3>(
        session, trackSequence, direction, height, supportType);
}

static void classicStandUpRCTrackDiagLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    direction = (direction + 2) % kNumOrthogonalDirections;
    classicStandUpRCTrackDiag25DegUpToRightBank(session, ride, trackSequence, direction, height, trackElement, supportType);
}

static void classicStandUpRCTrackDiagRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    trackSequence = kMapReversedDiagonalStraight[trackSequence];
    direction = (direction + 2) % kNumOrthogonalDirections;
    classicStandUpRCTrackDiag25DegUpToLeftBank(session, ride, trackSequence, direction, height, trackElement, supportType);
}

TrackPaintFunction GetTrackPaintFunctionClassicStandUpRC(OpenRCT2::TrackElemType trackType)
{
    if (!IsCsgLoaded())
    {
        return GetTrackPaintFunctionStandUpRC(trackType);
    }

    switch (trackType)
    {
        case TrackElemType::FlatToLeftBank:
            return classicStandUpRCTrackFlatToLeftBank;
        case TrackElemType::FlatToRightBank:
            return classicStandUpRCTrackFlatToRightBank;
        case TrackElemType::LeftBankToFlat:
            return classicStandUpRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return classicStandUpRCTrackRightBankToFlat;
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
            return classicStandUpRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::BankedRightQuarterTurn5Tiles:
            return classicStandUpRCTrackBankedRightQuarterTurn5;
        case TrackElemType::LeftBankToUp25:
            return classicStandUpRCTrackLeftBankTo25DegUp;
        case TrackElemType::RightBankToUp25:
            return classicStandUpRCTrackRightBankTo25DegUp;
        case TrackElemType::Up25ToLeftBank:
            return classicStandUpRCTrack25DegUpToLeftBank;
        case TrackElemType::Up25ToRightBank:
            return classicStandUpRCTrack25DegUpToRightBank;
        case TrackElemType::LeftBankToDown25:
            return classicStandUpRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return classicStandUpRCTrackRightBankTo25DegDown;
        case TrackElemType::Down25ToLeftBank:
            return classicStandUpRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return classicStandUpRCTrack25DegDownToRightBank;
        case TrackElemType::LeftBank:
            return classicStandUpRCTrackLeftBank;
        case TrackElemType::RightBank:
            return classicStandUpRCTrackRightBank;

        case TrackElemType::LeftEighthBankToDiag:
            return classicStandUpRCTrackLeftEighthBankToDiag;
        case TrackElemType::RightEighthBankToDiag:
            return classicStandUpRCTrackRightEighthBankToDiag;
        case TrackElemType::LeftEighthBankToOrthogonal:
            return classicStandUpRCTrackLeftEighthDiagBankToOrthogonal;
        case TrackElemType::RightEighthBankToOrthogonal:
            return classicStandUpRCTrackRightEighthDiagBankToOrthogonal;

        case TrackElemType::LeftBankedQuarterTurn3Tiles:
            return classicStandUpRCTrackLeftBankedQuarterTurn3;
        case TrackElemType::RightBankedQuarterTurn3Tiles:
            return classicStandUpRCTrackRightBankedQuarterTurn3;
        case TrackElemType::DiagFlatToLeftBank:
            return classicStandUpRCTrackDiagFlatToLeftBank;
        case TrackElemType::DiagFlatToRightBank:
            return classicStandUpRCTrackDiagFlatToRightBank;
        case TrackElemType::DiagLeftBankToFlat:
            return classicStandUpRCTrackDiagLeftBankToFlat;
        case TrackElemType::DiagRightBankToFlat:
            return classicStandUpRCTrackDiagRightBankToFlat;
        case TrackElemType::DiagLeftBankToUp25:
            return classicStandUpRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::DiagRightBankToUp25:
            return classicStandUpRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::DiagUp25ToLeftBank:
            return classicStandUpRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::DiagUp25ToRightBank:
            return classicStandUpRCTrackDiag25DegUpToRightBank;
        case TrackElemType::DiagLeftBankToDown25:
            return classicStandUpRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::DiagRightBankToDown25:
            return classicStandUpRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::DiagDown25ToLeftBank:
            return classicStandUpRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::DiagDown25ToRightBank:
            return classicStandUpRCTrackDiag25DegDownToRightBank;
        case TrackElemType::DiagLeftBank:
            return classicStandUpRCTrackDiagLeftBank;
        case TrackElemType::DiagRightBank:
            return classicStandUpRCTrackDiagRightBank;

        case TrackElemType::Up25ToLeftBankedUp25:
        case TrackElemType::Up25ToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToUp25:
        case TrackElemType::RightBankedUp25ToUp25:
        case TrackElemType::Down25ToLeftBankedDown25:
        case TrackElemType::Down25ToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToDown25:
        case TrackElemType::RightBankedDown25ToDown25:
        case TrackElemType::LeftBankedFlatToLeftBankedUp25:
        case TrackElemType::RightBankedFlatToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::RightBankedUp25ToRightBankedFlat:
        case TrackElemType::LeftBankedFlatToLeftBankedDown25:
        case TrackElemType::RightBankedFlatToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::RightBankedDown25ToRightBankedFlat:
        case TrackElemType::Down25LeftBanked:
        case TrackElemType::Down25RightBanked:
        case TrackElemType::FlatToLeftBankedUp25:
        case TrackElemType::FlatToRightBankedUp25:
        case TrackElemType::LeftBankedUp25ToFlat:
        case TrackElemType::RightBankedUp25ToFlat:
        case TrackElemType::FlatToLeftBankedDown25:
        case TrackElemType::FlatToRightBankedDown25:
        case TrackElemType::LeftBankedDown25ToFlat:
        case TrackElemType::RightBankedDown25ToFlat:
        case TrackElemType::Up25LeftBanked:
        case TrackElemType::Up25RightBanked:
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
        case TrackElemType::DiagUp25ToLeftBankedUp25:
        case TrackElemType::DiagUp25ToRightBankedUp25:
        case TrackElemType::DiagLeftBankedUp25ToUp25:
        case TrackElemType::DiagRightBankedUp25ToUp25:
        case TrackElemType::DiagDown25ToLeftBankedDown25:
        case TrackElemType::DiagDown25ToRightBankedDown25:
        case TrackElemType::DiagLeftBankedDown25ToDown25:
        case TrackElemType::DiagRightBankedDown25ToDown25:
        case TrackElemType::DiagLeftBankedFlatToLeftBankedUp25:
        case TrackElemType::DiagRightBankedFlatToRightBankedUp25:
        case TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::DiagRightBankedUp25ToRightBankedFlat:
        case TrackElemType::DiagLeftBankedFlatToLeftBankedDown25:
        case TrackElemType::DiagRightBankedFlatToRightBankedDown25:
        case TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::DiagRightBankedDown25ToRightBankedFlat:
        case TrackElemType::DiagUp25LeftBanked:
        case TrackElemType::DiagUp25RightBanked:
        case TrackElemType::DiagDown25LeftBanked:
        case TrackElemType::DiagDown25RightBanked:
        case TrackElemType::DiagFlatToLeftBankedUp25:
        case TrackElemType::DiagFlatToRightBankedUp25:
        case TrackElemType::DiagLeftBankedUp25ToFlat:
        case TrackElemType::DiagRightBankedUp25ToFlat:
        case TrackElemType::DiagFlatToLeftBankedDown25:
        case TrackElemType::DiagFlatToRightBankedDown25:
        case TrackElemType::DiagLeftBankedDown25ToFlat:
        case TrackElemType::DiagRightBankedDown25ToFlat:
        case TrackElemType::LeftEighthBankToDiagUp25:
        case TrackElemType::RightEighthBankToDiagUp25:
        case TrackElemType::LeftEighthBankToDiagDown25:
        case TrackElemType::RightEighthBankToDiagDown25:
        case TrackElemType::LeftEighthBankToOrthogonalUp25:
        case TrackElemType::RightEighthBankToOrthogonalUp25:
        case TrackElemType::LeftEighthBankToOrthogonalDown25:
        case TrackElemType::RightEighthBankToOrthogonalDown25:
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            return TrackPaintFunctionDummy;

        default:
            return GetTrackPaintFunctionStandUpRC(trackType);
    }
}
