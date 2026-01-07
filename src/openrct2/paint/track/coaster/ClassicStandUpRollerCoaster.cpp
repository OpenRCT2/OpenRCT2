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
#include "../../../ride/Ride.h"
#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight),
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
                        PaintSegment::bottom, PaintSegment::right, PaintSegment::centre, PaintSegment::bottomLeft,
                        PaintSegment::bottomRight, PaintSegment::topRight),
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
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
                        PaintSegment::left, PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft,
                        PaintSegment::topLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
            if (direction == 1 || direction == 0)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 3, height, session.SupportColours);
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 3, height, session.SupportColours);
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 6, height, session.SupportColours);
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
            session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight, PaintSegment::bottomLeft),
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
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
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
                session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                        PaintSegment::right, PaintSegment::bottom, PaintSegment::centre, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                        PaintSegment::top, PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft,
                        PaintSegment::topRight),
                    direction),
                0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
            break;
        case 3:
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(
                        PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight,
                        PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
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
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::topRight),
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
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
                    direction),
                0xFFFF, 0);
            break;
        case 3:
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex(imageId), { 6, 0, height }, { 20, 32, 3 });

            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height, session.SupportColours);
            if (direction == 0 || direction == 1)
            {
                PaintUtilPushTunnelRotated(session, direction ^ 1, height, kTunnelGroup, TunnelSubType::Flat);
            }
            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height, session.SupportColours);

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
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
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
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 4, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
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
                    EnumsToFlags(PaintSegment::right, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight),
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
                    EnumsToFlags(PaintSegment::top, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::topRight),
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
                        PaintSegment::bottom, PaintSegment::centre, PaintSegment::bottomLeft, PaintSegment::bottomRight),
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
                session, supportType.metal, MetalSupportPlace::leftCorner, direction, 8, height, session.SupportColours);

            PaintUtilSetSegmentSupportHeight(
                session,
                PaintUtilRotateSegments(
                    EnumsToFlags(PaintSegment::left, PaintSegment::centre, PaintSegment::topLeft, PaintSegment::bottomLeft),
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

TrackPaintFunction GetTrackPaintFunctionClassicStandUpRC(TrackElemType trackType)
{
    if (!IsCsgLoaded())
    {
        return GetTrackPaintFunctionStandUpRC(trackType);
    }

    switch (trackType)
    {
        case TrackElemType::flatToLeftBank:
            return classicStandUpRCTrackFlatToLeftBank;
        case TrackElemType::flatToRightBank:
            return classicStandUpRCTrackFlatToRightBank;
        case TrackElemType::leftBankToFlat:
            return classicStandUpRCTrackLeftBankToFlat;
        case TrackElemType::rightBankToFlat:
            return classicStandUpRCTrackRightBankToFlat;
        case TrackElemType::bankedLeftQuarterTurn5Tiles:
            return classicStandUpRCTrackBankedLeftQuarterTurn5;
        case TrackElemType::bankedRightQuarterTurn5Tiles:
            return classicStandUpRCTrackBankedRightQuarterTurn5;
        case TrackElemType::leftBankToUp25:
            return classicStandUpRCTrackLeftBankTo25DegUp;
        case TrackElemType::rightBankToUp25:
            return classicStandUpRCTrackRightBankTo25DegUp;
        case TrackElemType::up25ToLeftBank:
            return classicStandUpRCTrack25DegUpToLeftBank;
        case TrackElemType::up25ToRightBank:
            return classicStandUpRCTrack25DegUpToRightBank;
        case TrackElemType::leftBankToDown25:
            return classicStandUpRCTrackLeftBankTo25DegDown;
        case TrackElemType::rightBankToDown25:
            return classicStandUpRCTrackRightBankTo25DegDown;
        case TrackElemType::down25ToLeftBank:
            return classicStandUpRCTrack25DegDownToLeftBank;
        case TrackElemType::down25ToRightBank:
            return classicStandUpRCTrack25DegDownToRightBank;
        case TrackElemType::leftBank:
            return classicStandUpRCTrackLeftBank;
        case TrackElemType::rightBank:
            return classicStandUpRCTrackRightBank;

        case TrackElemType::leftEighthBankToDiag:
            return classicStandUpRCTrackLeftEighthBankToDiag;
        case TrackElemType::rightEighthBankToDiag:
            return classicStandUpRCTrackRightEighthBankToDiag;
        case TrackElemType::leftEighthBankToOrthogonal:
            return classicStandUpRCTrackLeftEighthDiagBankToOrthogonal;
        case TrackElemType::rightEighthBankToOrthogonal:
            return classicStandUpRCTrackRightEighthDiagBankToOrthogonal;

        case TrackElemType::leftBankedQuarterTurn3Tiles:
            return classicStandUpRCTrackLeftBankedQuarterTurn3;
        case TrackElemType::rightBankedQuarterTurn3Tiles:
            return classicStandUpRCTrackRightBankedQuarterTurn3;
        case TrackElemType::diagFlatToLeftBank:
            return classicStandUpRCTrackDiagFlatToLeftBank;
        case TrackElemType::diagFlatToRightBank:
            return classicStandUpRCTrackDiagFlatToRightBank;
        case TrackElemType::diagLeftBankToFlat:
            return classicStandUpRCTrackDiagLeftBankToFlat;
        case TrackElemType::diagRightBankToFlat:
            return classicStandUpRCTrackDiagRightBankToFlat;
        case TrackElemType::diagLeftBankToUp25:
            return classicStandUpRCTrackDiagLeftBankTo25DegUp;
        case TrackElemType::diagRightBankToUp25:
            return classicStandUpRCTrackDiagRightBankTo25DegUp;
        case TrackElemType::diagUp25ToLeftBank:
            return classicStandUpRCTrackDiag25DegUpToLeftBank;
        case TrackElemType::diagUp25ToRightBank:
            return classicStandUpRCTrackDiag25DegUpToRightBank;
        case TrackElemType::diagLeftBankToDown25:
            return classicStandUpRCTrackDiagLeftBankTo25DegDown;
        case TrackElemType::diagRightBankToDown25:
            return classicStandUpRCTrackDiagRightBankTo25DegDown;
        case TrackElemType::diagDown25ToLeftBank:
            return classicStandUpRCTrackDiag25DegDownToLeftBank;
        case TrackElemType::diagDown25ToRightBank:
            return classicStandUpRCTrackDiag25DegDownToRightBank;
        case TrackElemType::diagLeftBank:
            return classicStandUpRCTrackDiagLeftBank;
        case TrackElemType::diagRightBank:
            return classicStandUpRCTrackDiagRightBank;

        case TrackElemType::up25ToLeftBankedUp25:
        case TrackElemType::up25ToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToUp25:
        case TrackElemType::rightBankedUp25ToUp25:
        case TrackElemType::down25ToLeftBankedDown25:
        case TrackElemType::down25ToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToDown25:
        case TrackElemType::rightBankedDown25ToDown25:
        case TrackElemType::leftBankedFlatToLeftBankedUp25:
        case TrackElemType::rightBankedFlatToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToLeftBankedFlat:
        case TrackElemType::rightBankedUp25ToRightBankedFlat:
        case TrackElemType::leftBankedFlatToLeftBankedDown25:
        case TrackElemType::rightBankedFlatToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToLeftBankedFlat:
        case TrackElemType::rightBankedDown25ToRightBankedFlat:
        case TrackElemType::down25LeftBanked:
        case TrackElemType::down25RightBanked:
        case TrackElemType::flatToLeftBankedUp25:
        case TrackElemType::flatToRightBankedUp25:
        case TrackElemType::leftBankedUp25ToFlat:
        case TrackElemType::rightBankedUp25ToFlat:
        case TrackElemType::flatToLeftBankedDown25:
        case TrackElemType::flatToRightBankedDown25:
        case TrackElemType::leftBankedDown25ToFlat:
        case TrackElemType::rightBankedDown25ToFlat:
        case TrackElemType::up25LeftBanked:
        case TrackElemType::up25RightBanked:
        case TrackElemType::leftBankedQuarterTurn3TileUp25:
        case TrackElemType::rightBankedQuarterTurn3TileUp25:
        case TrackElemType::leftBankedQuarterTurn3TileDown25:
        case TrackElemType::rightBankedQuarterTurn3TileDown25:
        case TrackElemType::leftBankedQuarterTurn5TileUp25:
        case TrackElemType::rightBankedQuarterTurn5TileUp25:
        case TrackElemType::leftBankedQuarterTurn5TileDown25:
        case TrackElemType::rightBankedQuarterTurn5TileDown25:
        case TrackElemType::diagUp25ToLeftBankedUp25:
        case TrackElemType::diagUp25ToRightBankedUp25:
        case TrackElemType::diagLeftBankedUp25ToUp25:
        case TrackElemType::diagRightBankedUp25ToUp25:
        case TrackElemType::diagDown25ToLeftBankedDown25:
        case TrackElemType::diagDown25ToRightBankedDown25:
        case TrackElemType::diagLeftBankedDown25ToDown25:
        case TrackElemType::diagRightBankedDown25ToDown25:
        case TrackElemType::diagLeftBankedFlatToLeftBankedUp25:
        case TrackElemType::diagRightBankedFlatToRightBankedUp25:
        case TrackElemType::diagLeftBankedUp25ToLeftBankedFlat:
        case TrackElemType::diagRightBankedUp25ToRightBankedFlat:
        case TrackElemType::diagLeftBankedFlatToLeftBankedDown25:
        case TrackElemType::diagRightBankedFlatToRightBankedDown25:
        case TrackElemType::diagLeftBankedDown25ToLeftBankedFlat:
        case TrackElemType::diagRightBankedDown25ToRightBankedFlat:
        case TrackElemType::diagUp25LeftBanked:
        case TrackElemType::diagUp25RightBanked:
        case TrackElemType::diagDown25LeftBanked:
        case TrackElemType::diagDown25RightBanked:
        case TrackElemType::diagFlatToLeftBankedUp25:
        case TrackElemType::diagFlatToRightBankedUp25:
        case TrackElemType::diagLeftBankedUp25ToFlat:
        case TrackElemType::diagRightBankedUp25ToFlat:
        case TrackElemType::diagFlatToLeftBankedDown25:
        case TrackElemType::diagFlatToRightBankedDown25:
        case TrackElemType::diagLeftBankedDown25ToFlat:
        case TrackElemType::diagRightBankedDown25ToFlat:
        case TrackElemType::leftEighthBankToDiagUp25:
        case TrackElemType::rightEighthBankToDiagUp25:
        case TrackElemType::leftEighthBankToDiagDown25:
        case TrackElemType::rightEighthBankToDiagDown25:
        case TrackElemType::leftEighthBankToOrthogonalUp25:
        case TrackElemType::rightEighthBankToOrthogonalUp25:
        case TrackElemType::leftEighthBankToOrthogonalDown25:
        case TrackElemType::rightEighthBankToOrthogonalDown25:
        case TrackElemType::leftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::rightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::leftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::rightQuarterTurn3TilesDown25ToRightBank:
            return TrackPaintFunctionDummy;

        default:
            return GetTrackPaintFunctionStandUpRC(trackType);
    }
}
