#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2,an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors,a full list can be found in contributors.md
 * For more information,visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation,either version 3 of the License,or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include "../../../SpriteIds.h"
#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;
static constexpr uint8_t mapLeftEighthTurnToDiag[5] = { 4, 3, 1, 2, 0 };
static constexpr uint8_t mapSmallHelixDown[8] = { 7, 5, 6, 4, 3, 1, 2, 0 };
static constexpr uint8_t mapLargeHelixDown[14] = { 13, 11, 12, 10, 8, 9, 7, 6, 4, 5, 3, 1, 2, 0 };

namespace OpenRCT2::PipelineRC
{

    static void TrackStation(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        static constexpr const uint32_t imageIds[4][3] = {
            { (SPR_TRACKS_PIPELINE_TRACK_STATION + 0), (SPR_TRACKS_PIPELINE_TRACK_STATION + 0),
              SPR_TRACKS_STATION_BASE_E_SW_NE },
            { (SPR_TRACKS_PIPELINE_TRACK_STATION + 2), (SPR_TRACKS_PIPELINE_TRACK_STATION + 2),
              SPR_TRACKS_STATION_BASE_E_NW_SE },
            { (SPR_TRACKS_PIPELINE_TRACK_STATION + 0), (SPR_TRACKS_PIPELINE_TRACK_STATION + 0),
              SPR_TRACKS_STATION_BASE_E_SW_NE },
            { (SPR_TRACKS_PIPELINE_TRACK_STATION + 2), (SPR_TRACKS_PIPELINE_TRACK_STATION + 2),
              SPR_TRACKS_STATION_BASE_E_NW_SE },
        };
        int sprite = trackElement.GetTrackType() == TrackElemType::EndStation ? 1 : 0;
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][sprite]), { 0, 0, height },
            { { 0, 7, height + 3 }, { 32, 1, 21 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(imageIds[direction][sprite] + 1), { 0, 0, height },
            { { 0, 27, height + 3 }, { 32, 1, 21 } });
        PaintAddImageAsParentRotated(
            session, direction, GetStationColourScheme(session, trackElement).WithIndex(imageIds[direction][2]),
            { 0, 0, height }, { { 0, 2, height }, { 32, 28, 2 } });
        DrawSupportsSideBySide(session, direction, height, session.SupportColours, MetalSupportType::tubes);
        TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 13, trackElement, StationBaseType::none, 0);
        PaintUtilPushTunnelRotated(session, direction, height, TunnelType::SquareFlat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackOnRidePhoto(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackPaintUtilOnridePhotoPlatformPaint(session, direction, height, MetalSupportType::tubes);
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_PIPELINE_TRACK_FLAT + 2 * (direction & 1) + 0),
            { 0, 0, height }, { { 0, 6, height }, { 32, 20, 3 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex(SPR_TRACKS_PIPELINE_TRACK_FLAT + 2 * (direction & 1) + 1),
            { 0, 0, height }, { { 0, 6, height + 27 }, { 32, 20, 0 } });
        TrackPaintUtilOnridePhotoPaint(session, direction, height + 3, trackElement);
        PaintUtilPushTunnelRotated(session, direction, height, TunnelType::StandardFlat);
        PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (trackElement.HasChain())
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT + 2 * direction + 0)),
                { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT + 2 * direction + 1)),
                { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 22 } });
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT + 2 * (direction & 1) + 0)),
                { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT + 2 * (direction & 1) + 1)),
                { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 22 } });
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE + 2 * (direction & 1) + 0)),
            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE + 2 * (direction & 1) + 1)),
            { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 22 } });
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackBlockBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE + 2 * (direction & 1) + 0)), { 0, 0, height },
            { { 0, 6, height + 0 }, { 32, 20, 8 } });
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE + 2 * (direction & 1) + 1)), { 0, 0, height },
            { { 0, 27, height + 0 }, { 32, 1, 22 } });
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackFlatToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        if (trackElement.HasChain())
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 0)),
                { 0, 0, height }, boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 1)),
                { 0, 0, height }, boundingBoxes[direction + 4]);
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 0)),
                { 0, 0, height }, boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 1)),
                { 0, 0, height }, boundingBoxes[direction + 4]);
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 2, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        if (trackElement.HasChain())
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 8)),
                { 0, 0, height }, boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 9)),
                { 0, 0, height }, boundingBoxes[direction + 4]);
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 8)),
                { 0, 0, height }, boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 9)),
                { 0, 0, height }, boundingBoxes[direction + 4]);
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 6, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        if (trackElement.HasChain())
        {
            PaintAddImageAsParentRotated(
                session, direction,
                session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 16)), { 0, 0, height },
                boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction,
                session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE + 2 * direction + 17)), { 0, 0, height },
                boundingBoxes[direction + 4]);
        }
        else
        {
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 16)),
                { 0, 0, height }, boundingBoxes[direction + 0]);
            PaintAddImageAsParentRotated(
                session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE + 2 * direction + 17)),
                { 0, 0, height }, boundingBoxes[direction + 4]);
        }
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackFlatToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackUp25ToUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 62 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } } };
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 0)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 1)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void TrackUp60ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 62 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } } };
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 8)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 9)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 12, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 24, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 72);
    }

    static void TrackUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 62 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 62 } }, { { 0, 27, 0 + height }, { 32, 1, 62 } } };
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 16)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP + 2 * direction + 17)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::centre, direction, 16, height + -3, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 104);
    }

    static void TrackDown25ToDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp60ToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown60ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToUp60(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp60(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackUp90ToUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 8)), { 0, 0, height },
                    { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 9)), { 0, 0, height },
                    { { 0, 27, height + 0 }, { 32, 1, 64 } });
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                break;
        }
    }

    static void TrackUp60ToUp90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 0)), { 0, 0, height },
                    { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 1)), { 0, 0, height },
                    { { 0, 27, height + 0 }, { 32, 1, 56 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 16, height + -3,
                        session.SupportColours);
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetVerticalTunnel(session, height + 56);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                break;
        }
    }

    static void TrackUp90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 8 + height }, { 2, 20, 31 } },  { { 0, 4, 8 + height }, { 32, 1, 31 } },
                                        { { 0, 4, 8 + height }, { 32, 1, 31 } },  { { 2, 6, 8 + height }, { 2, 20, 31 } },
                                        { { 0, 27, 8 + height }, { 32, 1, 31 } }, { { 28, 6, 8 + height }, { 2, 20, 31 } },
                                        { { 28, 6, 8 + height }, { 2, 20, 31 } }, { { 0, 27, 8 + height }, { 32, 1, 31 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 16)), { 0, 0, height },
                    boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL + 2 * direction + 17)), { 0, 0, height },
                    boundingBoxes[direction + 4]);
                PaintUtilSetVerticalTunnel(session, height + 32);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                break;
        }
    }

    static void TrackDown60ToDown90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp90ToUp60(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown90ToDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp60ToUp90(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp90(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_FLAT_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BRAKE_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagBlockBrakes(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BLOCK_BRAKE_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagFlatToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 14)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 15)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 14)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 15)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 12)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 13)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 12)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 13)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 10)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 11)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 10)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 11)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 4, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 16)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 17)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 16)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 17)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 20)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 20)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 18)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 19)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 18)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 19)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 8, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagFlatToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 10)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 11)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 10)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 11)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 12)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 13)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 12)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 13)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 14)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 15)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 14)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 15)),
                            { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 4, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 2)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 3)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 4)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 5)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 0)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 1)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (trackElement.HasChain())
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 18)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 19)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 3)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 18)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 19)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (trackElement.HasChain())
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 20)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 0)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 20)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (trackElement.HasChain())
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 16)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 17)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 2)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 16)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 17)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (trackElement.HasChain())
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_LIFT_TRACK_GENTLE_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                else
                {
                    if (direction == 1)
                    {
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                    }
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 2, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagUp25ToUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 16, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagUp60ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 9)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 21, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 22)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 23)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 21)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 32, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void TrackDiagDown25ToDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 9)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagDown60ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 62 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 2, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
        }
    }

    static void TrackDiagDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 21)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 22)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_DIAGONAL + 23)),
                        { -16, -16, height }, { { -16, -16, height + 95 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 14, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void TrackLeftQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } }, { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } }, { { 16, 16, 22 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 4)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE + 6 * direction + 5)),
                    { 0, 0, height }, { { 6, 0, height + 22 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftQuarterTurn3(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 32, 26, 8 } },   { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 26, 8 } },   { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } },  { { 0, 0, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } },  { { 0, 6, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } }, { { 16, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } }, { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },   { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },   { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 22 + height }, { 26, 32, 0 } },  { { 0, 0, 22 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 22 + height }, { 26, 32, 0 } },  { { 6, 0, 22 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 8)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE + 10 * direction + 9)),
                    { 0, 0, height }, { { 6, 0, height + 22 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftQuarterTurn5(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 16, 0 } },   { { 0, 0, 22 + height }, { 32, 16, 0 } },
                                        { { 0, 16, 22 + height }, { 32, 16, 0 } },  { { 0, 16, 22 + height }, { 32, 16, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },   { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },   { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } },  { { 16, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } },  { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 22 + height }, { 16, 16, 0 } }, { { 0, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 22 + height }, { 16, 16, 0 } },   { { 16, 0, 22 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = {
            { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
            { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
            { { 0, 16, 22 + height }, { 32, 16, 0 } },  { { 0, 16, 22 + height }, { 32, 16, 0 } },
            { { 0, 0, 22 + height }, { 32, 16, 0 } },   { { 0, 0, 22 + height }, { 32, 16, 0 } },
            { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
            { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
            { { 0, 0, 22 + height }, { 16, 16, 0 } },   { { 16, 0, 22 + height }, { 16, 16, 0 } },
            { { 16, 16, 22 + height }, { 16, 16, 0 } }, { { 0, 16, 22 + height }, { 16, 16, 0 } },
            { { 16, 0, 0 + height }, { 16, 16, 8 } },   { { 0, 0, 0 + height }, { 16, 16, 8 } },
            { { 0, 16, 0 + height }, { 16, 16, 8 } },   { { 16, 16, 0 + height }, { 16, 16, 8 } },
            { { 16, 0, 22 + height }, { 16, 16, 0 } },  { { 0, 0, 22 + height }, { 16, 16, 0 } },
            { { 0, 16, 22 + height }, { 16, 16, 0 } },  { { 16, 16, 22 + height }, { 16, 16, 0 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 32)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 33)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 34)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 35)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 36)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 37)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 38)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE + 8 * direction + 39)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftEighthToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 0)),
            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 1)),
            { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 22 } });
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0, height + -3,
                session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackFlatToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 8)),
            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
        PaintAddImageAsParentRotated(
            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 9)),
            { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 22 } });
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0, height + -3,
                session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackLeftBankToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, 1, 2, 2 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 16)), { 0, 0, height },
            boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 17)), { 0, 0, height },
            boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBankToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 2, 1, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 24)), { 0, 0, height },
            boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 25)), { 0, 0, height },
            boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackUp25ToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 32)), { 0, 0, height },
            boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 33)), { 0, 0, height },
            boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0, height + -6,
                session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackUp25ToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 40)), { 0, 0, height },
            boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 41)), { 0, 0, height },
            boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0, height + -6,
                session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToRightBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25ToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25ToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 48)), { 0, 0, height },
            { { 0, 6, height + 0 }, { 32, 20, 8 } });
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION + 2 * direction + 49)), { 0, 0, height },
            { { 0, 27, height + 0 }, { 32, 1, 22 } });
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + 1,
                session.SupportColours);
        }
        PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 32);
    }

    static void TrackRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBank(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 7)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagFlatToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 9)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagLeftBankToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToRightBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 22)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 23)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 2, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 21)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagRightBankToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 30)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 31)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 24)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 25)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 28)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 29)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 2, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 26)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 27)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagUp25ToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 38)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 39)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 32)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 33)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 6, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 36)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 37)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 34)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 35)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagUp25ToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 46)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 47)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 40)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 41)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 44)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 45)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 6, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 42)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 43)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagLeftBankToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToRightBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25ToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankToUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25ToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankToUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 1, 1, 5, 1 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 54)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 55)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 48)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 49)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 52)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 53)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 50)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_BANK_TRANSITION_DIAGONAL + 51)),
                        { -16, -16, height }, { { -16, -16, height + 22 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackDiagRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBank(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackBankedLeftQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 3, 2, -2, -2, -2, 2, 3, -2 };
        BoundBoxXYZ boundingBoxes[] = { { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } }, { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } }, { { 16, 16, 22 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 4)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_CURVE_BANKED + 6 * direction + 5)),
                    { 0, 0, height }, { { 6, 0, height + 22 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackBankedRightQuarterTurn3(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackBankedLeftQuarterTurn3(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackBankedLeftQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 2, -3, -3, -3, 2, 2, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 32, 26, 8 } },   { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 26, 8 } },   { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } },  { { 0, 0, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } },  { { 0, 6, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } }, { { 16, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } }, { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },   { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },   { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 22 + height }, { 26, 32, 0 } },  { { 0, 0, 22 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 22 + height }, { 26, 32, 0 } },  { { 6, 0, 22 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 8)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_CURVE_BANKED + 10 * direction + 9)),
                    { 0, 0, height }, { { 6, 0, height + 22 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackBankedRightQuarterTurn5(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackBankedLeftQuarterTurn5(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 1, -3, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 16, 0 } },   { { 0, 0, 22 + height }, { 32, 16, 0 } },
                                        { { 0, 16, 22 + height }, { 32, 16, 0 } },  { { 0, 16, 22 + height }, { 32, 16, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },   { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },   { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 22 + height }, { 16, 16, 0 } },  { { 16, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 22 + height }, { 16, 16, 0 } },  { { 0, 0, 22 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 22 + height }, { 16, 16, 0 } }, { { 0, 16, 22 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 22 + height }, { 16, 16, 0 } },   { { 16, 0, 22 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthBankToDiag(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToDiag(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -3, 1, 2 };
        BoundBoxXYZ boundingBoxes[] = {
            { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
            { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
            { { 0, 16, 22 + height }, { 32, 16, 0 } },  { { 0, 16, 22 + height }, { 32, 16, 0 } },
            { { 0, 0, 22 + height }, { 32, 16, 0 } },   { { 0, 0, 22 + height }, { 32, 16, 0 } },
            { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
            { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
            { { 0, 0, 22 + height }, { 16, 16, 0 } },   { { 16, 0, 22 + height }, { 16, 16, 0 } },
            { { 16, 16, 22 + height }, { 16, 16, 0 } }, { { 0, 16, 22 + height }, { 16, 16, 0 } },
            { { 16, 0, 0 + height }, { 16, 16, 8 } },   { { 0, 0, 0 + height }, { 16, 16, 8 } },
            { { 0, 16, 0 + height }, { 16, 16, 8 } },   { { 16, 16, 0 + height }, { 16, 16, 8 } },
            { { 16, 0, 22 + height }, { 16, 16, 0 } },  { { 0, 0, 22 + height }, { 16, 16, 0 } },
            { { 0, 16, 22 + height }, { 16, 16, 0 } },  { { 16, 16, 22 + height }, { 16, 16, 0 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 32)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 33)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 34)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 35)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 36)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 37)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 38)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CURVE_BANKED + 8 * direction + 39)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftEighthBankToOrthogonal(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthBankToDiag(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn3TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 6, 8, 8, 6 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 16, 1, 30 } }, { { 16, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },  { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },  { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 0, 0 + height }, { 1, 16, 30 } }, { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } }, { { 27, 16, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 3, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackRightQuarterTurn3TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 6, 8, 8, 6 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 16, 27, 0 + height }, { 16, 1, 30 } }, { { 0, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 16, 0 + height }, { 1, 16, 30 } }, { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },  { { 27, 0, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 16)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 17)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 3, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 18)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE + 4 * direction + 19)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackLeftQuarterTurn3TilesDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightQuarterTurn3TilesUp25(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn3TilesDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftQuarterTurn3TilesUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn5TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 39 + height }, { 32, 20, 0 } },  { { 0, 6, 39 + height }, { 32, 20, 0 } },
                                        { { 0, 4, 0 + height }, { 32, 22, 8 } },   { { 0, 4, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 22, 8 } },   { { 0, 6, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 22, 0 } },  { { 0, 6, 42 + height }, { 32, 22, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 42 + height }, { 16, 16, 0 } }, { { 16, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 42 + height }, { 16, 16, 0 } }, { { 0, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },   { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },   { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },  { { 0, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 50 + height }, { 26, 32, 0 } },  { { 6, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },   { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },   { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },  { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },  { { 6, 0, 50 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 3, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 8)),
                    { 0, 0, height }, boundingBoxes[direction + 32]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 9)),
                    { 0, 0, height }, boundingBoxes[direction + 36]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightQuarterTurn5TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 39 + height }, { 32, 20, 0 } },   { { 0, 6, 39 + height }, { 32, 20, 0 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 0 + height }, { 32, 22, 8 } },    { { 0, 6, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 4, 0 + height }, { 32, 22, 8 } },    { { 0, 4, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 6, 42 + height }, { 32, 22, 0 } },   { { 0, 6, 42 + height }, { 32, 22, 0 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 42 + height }, { 16, 16, 0 } },   { { 16, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 42 + height }, { 16, 16, 0 } }, { { 0, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },    { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },    { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },   { { 0, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 50 + height }, { 26, 32, 0 } },   { { 6, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 50 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 40)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 41)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 3, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 42)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 43)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 44)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 45)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 46)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 47)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 48)),
                    { 0, 0, height }, boundingBoxes[direction + 32]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE + 10 * direction + 49)),
                    { 0, 0, height }, boundingBoxes[direction + 36]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftQuarterTurn5TilesDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackRightQuarterTurn5TilesUp25(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn5TilesDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftQuarterTurn5TilesUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn1TileUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_SMALL_CURVE + 2 * direction + 0)), { 0, 0, height },
            { { 2, 2, height + 0 }, { 28, 28, 8 } });
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_SMALL_CURVE + 2 * direction + 1)), { 0, 0, height },
            { { 2, 2, height + 94 }, { 28, 28, 1 } });
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 2 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction + 1, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                    PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                    PaintSegment::right),
                direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 96);
    }

    static void TrackRightQuarterTurn1TileUp60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_SMALL_CURVE + 2 * direction + 8)), { 0, 0, height },
            { { 2, 2, height + 0 }, { 28, 28, 8 } });
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_STEEP_SMALL_CURVE + 2 * direction + 9)), { 0, 0, height },
            { { 2, 2, height + 94 }, { 28, 28, 1 } });
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 0 || direction == 1)
        {
            PaintUtilPushTunnelRotated(session, direction + 1, height + 56, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(
                    PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                    PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                    PaintSegment::right),
                direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 96);
    }

    static void TrackLeftQuarterTurn1TileDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightQuarterTurn1TileUp60(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn1TileDown60(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftQuarterTurn1TileUp60(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftQuarterTurn1Up90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 8 + height }, { 2, 20, 96 } },  { { 0, 4, 8 + height }, { 32, 1, 96 } },
                                        { { 0, 4, 8 + height }, { 32, 1, 96 } },  { { 2, 6, 8 + height }, { 2, 20, 96 } },
                                        { { 0, 27, 8 + height }, { 32, 1, 96 } }, { { 28, 6, 8 + height }, { 2, 20, 96 } },
                                        { { 28, 6, 8 + height }, { 2, 20, 96 } }, { { 0, 27, 8 + height }, { 32, 1, 96 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL_TWIST + 2 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL_TWIST + 2 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetVerticalTunnel(session, height + 48);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightQuarterTurn1Up90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 8 + height }, { 2, 20, 96 } },  { { 0, 4, 8 + height }, { 32, 1, 96 } },
                                        { { 0, 4, 8 + height }, { 32, 1, 96 } },  { { 2, 6, 8 + height }, { 2, 20, 96 } },
                                        { { 0, 27, 8 + height }, { 32, 1, 96 } }, { { 28, 6, 8 + height }, { 2, 20, 96 } },
                                        { { 28, 6, 8 + height }, { 2, 20, 96 } }, { { 0, 27, 8 + height }, { 32, 1, 96 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL_TWIST + 2 * direction + 8)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_VERTICAL_TWIST + 2 * direction + 9)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetVerticalTunnel(session, height + 48);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::right), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftQuarterTurn1Down90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightQuarterTurn1Up90(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn1Down90(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftQuarterTurn1Up90(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackUp25ToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -6, -6, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 0)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 1)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackUp25ToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -6, -6, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 8)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 9)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackLeftBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 3, 5, 9, 9 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 16)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 17)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackRightBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 9, 9, 5, 3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 24)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 25)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackDown25ToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankedUp25ToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25ToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankedUp25ToUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToRightBankedUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25ToLeftBankedUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 32)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 33)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + 2,
                session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBankedFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 40)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 41)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0, height + 2, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankedUp25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 48)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 49)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + 6,
                session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBankedUp25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 56)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 57)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0, height + 6, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankedFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankedUp25ToRightBankedFlat(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankedUp25ToLeftBankedFlat(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedDown25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankedFlatToRightBankedUp25(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedDown25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankedFlatToLeftBankedUp25(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackUp25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 64)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 65)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + 7,
                session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackUp25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 46 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 46 } }, { { 0, 27, 0 + height }, { 32, 1, 46 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 72)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 73)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0, height + 7, session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 48);
    }

    static void TrackDown25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25RightBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp25LeftBanked(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -3, -3, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 80)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 81)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -3, -3, -3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 38 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 38 } }, { { 0, 27, 0 + height }, { 32, 1, 38 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 88)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 89)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::bottomLeftSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::SlopeEnd);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackLeftBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 1, 2, 4, 3 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 96)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 97)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        else
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackRightBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 3, 4, 2, 1 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 4, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 4, 0 + height }, { 32, 1, 30 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } } };
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 104)),
            { 0, 0, height }, boundingBoxes[direction + 0]);
        PaintAddImageAsParentRotated(
            session, direction,
            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SLOPE_BANK_TRANSITION + 2 * direction + 105)),
            { 0, 0, height }, boundingBoxes[direction + 4]);
        if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
        {
            MetalASupportsPaintSetupRotated(
                session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                height + supportHeights[direction + 0], session.SupportColours);
        }
        if (direction == 0 || direction == 3)
        {
            PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
        }
        if (direction == 1 || direction == 2)
        {
            PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::FlatTo25Deg);
        }
        PaintUtilSetSegmentSupportHeight(
            session,
            PaintUtilRotateSegments(
                EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
            0xFFFF, 0);
        PaintUtilSetGeneralSupportHeight(session, height + 40);
    }

    static void TrackFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightBankedUp25ToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftBankedUp25ToFlat(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToRightBankedUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToLeftBankedUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagUp25ToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 6)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 7)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 1)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 6, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 5)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 3)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagUp25ToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 15)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 9)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 13)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 6, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 10)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 11)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 6, 6, 6, 9 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 22)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 23)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 17)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 20)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 21)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 19)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 6, 6, 6, 9 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 30)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 31)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 24)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 25)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 28)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 29)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 26)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 27)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagDown25ToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25ToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25ToLeftBankedUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 4, 8, 5 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 38)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 39)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 32)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 33)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 36)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 37)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 34)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 35)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagRightBankedFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 8, 4, 2, 5 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 46)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 47)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 40)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 41)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 44)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 45)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 42)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 43)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 5, 7, 11, 9 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 54)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 55)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 48)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 49)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 52)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 53)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 50)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 51)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 11, 7, 5, 9 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 62)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 63)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 56)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 57)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 60)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 61)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 58)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 59)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagLeftBankedFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToRightBankedFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToLeftBankedFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToLeftBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedFlatToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToRightBankedFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedFlatToLeftBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagUp25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 8, 8, 13, 12 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 70)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 71)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 64)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 65)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 68)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 69)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 66)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 67)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagUp25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 13, 8, 8, 12 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 78)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 79)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 72)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 73)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 76)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 77)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 74)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 75)),
                        { -16, -16, height }, { { -16, -16, height + 46 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagDown25LeftBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25RightBanked(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagDown25RightBanked(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagUp25LeftBanked(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToLeftBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 1, 3, 3, 3 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 86)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 87)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 80)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 81)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 84)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 85)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 82)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 83)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagFlatToRightBankedUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 3, 3, 1, 3 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 94)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 95)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 88)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 89)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 92)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 93)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 90)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 91)),
                        { -16, -16, height }, { { -16, -16, height + 38 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagLeftBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 4, 4, 4, 6 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 102)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 103)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 96)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 97)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 100)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 101)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 98)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 99)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagRightBankedUp25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 4, 4, 4, 6 };
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 110)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 111)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 104)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 105)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 108)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 109)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 106)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 8 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 107)),
                        { -16, -16, height }, { { -16, -16, height + 30 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackDiagFlatToLeftBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagRightBankedUp25ToFlat(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToRightBankedDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagLeftBankedUp25ToFlat(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagLeftBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToRightBankedUp25(
            session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagRightBankedDown25ToFlat(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackDiagFlatToLeftBankedUp25(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 3, 2, 1 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
                                        { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 42 + height }, { 16, 16, 0 } },  { { 16, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 42 + height }, { 16, 16, 0 } },  { { 0, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 4, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightEighthToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 1, 2, 3, 2 };
        BoundBoxXYZ boundingBoxes[] = {
            { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
            { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
            { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
            { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
            { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
            { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
            { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
            { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
            { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
            { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
            { { 0, 0, 42 + height }, { 16, 16, 0 } },   { { 16, 0, 42 + height }, { 16, 16, 0 } },
            { { 16, 16, 42 + height }, { 16, 16, 0 } }, { { 0, 16, 42 + height }, { 16, 16, 0 } },
            { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
            { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
            { { 16, 0, 50 + height }, { 16, 16, 0 } },  { { 0, 0, 50 + height }, { 16, 16, 0 } },
            { { 0, 16, 50 + height }, { 16, 16, 0 } },  { { 16, 16, 50 + height }, { 16, 16, 0 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 32)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 33)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 4, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 34)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 35)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 36)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 37)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 38)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 39)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftEighthToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 2, 0, 1 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 46 + height }, { 16, 16, 0 } },  { { 16, 16, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 46 + height }, { 16, 16, 0 } },  { { 0, 0, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 0 + height }, { 32, 16, 2 } },    { { 0, 0, 0 + height }, { 32, 16, 2 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 2 } },   { { 0, 16, 0 + height }, { 32, 16, 2 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 20, 0 } },   { { 0, 6, 42 + height }, { 32, 20, 0 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 2 } },    { { 0, 6, 0 + height }, { 32, 20, 2 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 2 } },    { { 0, 6, 0 + height }, { 32, 20, 2 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 20, 0 } },   { { 0, 6, 42 + height }, { 32, 20, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 64)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 65)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::rightCorner, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 66)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 67)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 68)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 69)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 70)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 71)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 4, height + -3, session.SupportColours);
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightEighthToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 0, 2, 2, 1 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 46 + height }, { 16, 16, 0 } },  { { 16, 16, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 46 + height }, { 16, 16, 0 } },  { { 0, 0, 46 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 0 + height }, { 16, 32, 2 } },   { { 0, 0, 0 + height }, { 16, 32, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 32, 2 } },    { { 16, 0, 0 + height }, { 16, 32, 2 } },
                                        { { 6, 0, 42 + height }, { 20, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 42 + height }, { 20, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 42 + height }, { 20, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 42 + height }, { 20, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 96)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 97)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::rightCorner, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 98)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 99)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 100)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 101)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 102)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE + 8 * direction + 103)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 4, height + -3, session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftEighthToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToDiag[trackSequence];
        TrackRightEighthToOrthogonalUp25(session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToDiag[trackSequence];
        TrackLeftEighthToOrthogonalUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthToDiagUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthToDiagUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedQuarterTurn3TileUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 10, 6, 2, 2, 5, 13, 7, 6 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },  { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } }, { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 16, 1, 30 } }, { { 16, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },  { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },  { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 0, 0 + height }, { 1, 16, 30 } }, { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } }, { { 27, 16, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackRightBankedQuarterTurn3TileUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 2, 2, 6, 10, 6, 7, 13, 5 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 16, 27, 0 + height }, { 16, 1, 30 } }, { { 0, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 16, 0 + height }, { 1, 16, 30 } }, { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },  { { 27, 0, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 16)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 17)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 2:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 18)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_SMALL_CURVE_BANKED + 4 * direction + 19)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackLeftBankedQuarterTurn3TileDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightBankedQuarterTurn3TileUp25(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedQuarterTurn3TileDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftBankedQuarterTurn3TileUp25(
            session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftBankedQuarterTurn5TileUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 10, 7, 7, 7, 7, 11, 7, 7 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 39 + height }, { 32, 20, 0 } },  { { 0, 6, 39 + height }, { 32, 20, 0 } },
                                        { { 0, 4, 0 + height }, { 32, 22, 8 } },   { { 0, 4, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 22, 8 } },   { { 0, 6, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 22, 0 } },  { { 0, 6, 42 + height }, { 32, 22, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 42 + height }, { 16, 16, 0 } }, { { 16, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 42 + height }, { 16, 16, 0 } }, { { 0, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },   { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },   { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },  { { 0, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 50 + height }, { 26, 32, 0 } },  { { 6, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },   { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },   { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },  { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },  { { 6, 0, 50 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 8)),
                    { 0, 0, height }, boundingBoxes[direction + 32]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 9)),
                    { 0, 0, height }, boundingBoxes[direction + 36]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightBankedQuarterTurn5TileUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 7, 7, 7, 10, 7, 7, 11, 7 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 39 + height }, { 32, 20, 0 } },   { { 0, 6, 39 + height }, { 32, 20, 0 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 0 + height }, { 32, 22, 8 } },    { { 0, 6, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 4, 0 + height }, { 32, 22, 8 } },    { { 0, 4, 0 + height }, { 32, 22, 8 } },
                                        { { 0, 6, 42 + height }, { 32, 22, 0 } },   { { 0, 6, 42 + height }, { 32, 22, 0 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 42 + height }, { 16, 16, 0 } },   { { 16, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 42 + height }, { 16, 16, 0 } }, { { 0, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 26, 32, 8 } },    { { 0, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 0, 0, 0 + height }, { 26, 32, 8 } },    { { 6, 0, 0 + height }, { 26, 32, 8 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },   { { 0, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 0, 0, 50 + height }, { 26, 32, 0 } },   { { 6, 0, 50 + height }, { 26, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 50 + height }, { 26, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 50 + height }, { 26, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 40)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 41)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 42)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 43)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 44)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 45)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 46)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 47)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 48)),
                    { 0, 0, height }, boundingBoxes[direction + 32]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex(
                        (SPR_TRACKS_PIPELINE_TRACK_GENTLE_MEDIUM_CURVE_BANKED + 10 * direction + 49)),
                    { 0, 0, height }, boundingBoxes[direction + 36]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftBankedQuarterTurn5TileDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackRightBankedQuarterTurn5TileUp25(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightBankedQuarterTurn5TileDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn5TilesToRightQuarterTurn5Tiles[trackSequence];
        TrackLeftBankedQuarterTurn5TileUp25(
            session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 9, 7, 7, 7, 3, 2, 3, 5 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
                                        { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 42 + height }, { 16, 16, 0 } },  { { 16, 16, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 42 + height }, { 16, 16, 0 } },  { { 0, 0, 42 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 112)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 113)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 114)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 115)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 116)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 117)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 118)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 119)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightEighthBankToDiagUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 7, 7, 7, 9, 5, 3, 2, 3 };
        BoundBoxXYZ boundingBoxes[] = {
            { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
            { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
            { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
            { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
            { { 0, 16, 0 + height }, { 32, 16, 8 } },   { { 0, 16, 0 + height }, { 32, 16, 8 } },
            { { 0, 0, 0 + height }, { 32, 16, 8 } },    { { 0, 0, 0 + height }, { 32, 16, 8 } },
            { { 0, 6, 50 + height }, { 32, 20, 0 } },   { { 0, 6, 50 + height }, { 32, 20, 0 } },
            { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
            { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
            { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
            { { 0, 0, 42 + height }, { 16, 16, 0 } },   { { 16, 0, 42 + height }, { 16, 16, 0 } },
            { { 16, 16, 42 + height }, { 16, 16, 0 } }, { { 0, 16, 42 + height }, { 16, 16, 0 } },
            { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
            { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
            { { 16, 0, 50 + height }, { 16, 16, 0 } },  { { 0, 0, 50 + height }, { 16, 16, 0 } },
            { { 0, 16, 50 + height }, { 16, 16, 0 } },  { { 16, 16, 50 + height }, { 16, 16, 0 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 144)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 145)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 146)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 147)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 148)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 149)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 150)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 151)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftEighthBankToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 7, 7, 7, 10, 10, 8, 7, 5 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 46 + height }, { 16, 16, 0 } },  { { 16, 16, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 46 + height }, { 16, 16, 0 } },  { { 0, 0, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 0 + height }, { 32, 16, 2 } },    { { 0, 0, 0 + height }, { 32, 16, 2 } },
                                        { { 0, 16, 0 + height }, { 32, 16, 2 } },   { { 0, 16, 0 + height }, { 32, 16, 2 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 20, 0 } },   { { 0, 6, 42 + height }, { 32, 20, 0 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 2 } },    { { 0, 6, 0 + height }, { 32, 20, 2 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 2 } },    { { 0, 6, 0 + height }, { 32, 20, 2 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 6, 42 + height }, { 32, 20, 0 } },   { { 0, 6, 42 + height }, { 32, 20, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 176)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 177)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 178)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 179)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 180)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 181)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 182)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 183)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightEighthBankToOrthogonalUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 7, 7, 7, 10, 7, 8, 10, 5 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 16, 0 + height }, { 16, 16, 2 } },   { { 16, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 2 } },   { { 0, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 16, 46 + height }, { 16, 16, 0 } },  { { 16, 16, 46 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 46 + height }, { 16, 16, 0 } },  { { 0, 0, 46 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 2 } },    { { 16, 0, 0 + height }, { 16, 16, 2 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 2 } },  { { 0, 16, 0 + height }, { 16, 16, 2 } },
                                        { { 0, 0, 50 + height }, { 16, 16, 0 } },   { { 16, 0, 50 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 50 + height }, { 16, 16, 0 } }, { { 0, 16, 50 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 0 + height }, { 16, 32, 2 } },   { { 0, 0, 0 + height }, { 16, 32, 2 } },
                                        { { 0, 0, 0 + height }, { 16, 32, 2 } },    { { 16, 0, 0 + height }, { 16, 32, 2 } },
                                        { { 6, 0, 42 + height }, { 20, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 42 + height }, { 20, 32, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 2 } },    { { 6, 0, 0 + height }, { 20, 32, 2 } },
                                        { { 6, 0, 42 + height }, { 20, 32, 0 } },   { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 6, 0, 42 + height }, { 20, 32, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 208)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 209)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 210)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 211)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomCorner, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 212)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 213)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 214)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_GENTLE_LARGE_CURVE_BANKED + 8 * direction + 215)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftEighthBankToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToDiag[trackSequence];
        TrackRightEighthBankToOrthogonalUp25(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToDiagDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToDiag[trackSequence];
        TrackLeftEighthBankToOrthogonalUp25(
            session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackLeftEighthBankToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackRightEighthBankToDiagUp25(session, ride, trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightEighthBankToOrthogonalDown25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLeftEighthTurnToOrthogonal[trackSequence];
        TrackLeftEighthBankToDiagUp25(session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftHalfBankedHelixUpSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 5, 3, -1, -1, 7, 9, 8, 7, -1, 5, 3, -1, 7, 7, 9, 8 };
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 0 + height }, { 30, 20, 8 } },   { { 0, 6, 0 + height }, { 30, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 30, 20, 8 } },   { { 2, 6, 0 + height }, { 30, 20, 8 } },
                                        { { 2, 6, 30 + height }, { 30, 20, 0 } },  { { 0, 6, 30 + height }, { 30, 20, 0 } },
                                        { { 0, 6, 30 + height }, { 30, 20, 0 } },  { { 2, 6, 30 + height }, { 30, 20, 0 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 30 + height }, { 16, 16, 0 } }, { { 0, 0, 30 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 30 + height }, { 16, 16, 0 } }, { { 16, 16, 30 + height }, { 16, 16, 0 } },
                                        { { 6, 2, 0 + height }, { 20, 30, 8 } },   { { 6, 2, 0 + height }, { 20, 30, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 30, 8 } },   { { 6, 0, 0 + height }, { 20, 30, 8 } },
                                        { { 6, 2, 30 + height }, { 20, 30, 0 } },  { { 6, 2, 30 + height }, { 20, 30, 0 } },
                                        { { 6, 0, 30 + height }, { 20, 30, 0 } },  { { 6, 0, 30 + height }, { 20, 30, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 18)),
                            { 0, 0, height }, { { 6, 2, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 19)),
                            { 0, 0, height }, { { 6, 2, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 0)),
                            { 0, 0, height }, { { 6, 2, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 1)),
                            { 0, 0, height }, { { 6, 2, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 7)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 12)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 13)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 30, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 8], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 20)),
                            { 0, 0, height }, { { 16, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 21)),
                            { 0, 0, height }, { { 16, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 2)),
                            { 0, 0, height }, { { 0, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 3)),
                            { 0, 0, height }, { { 0, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 8)),
                            { 0, 0, height }, { { 0, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 9)),
                            { 0, 0, height }, { { 0, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 14)),
                            { 0, 0, height }, { { 16, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 15)),
                            { 0, 0, height }, { { 16, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 22)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 23)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 4)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 5)),
                            { 0, 0, height }, { { 2, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 10)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 11)),
                            { 0, 0, height }, { { 2, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 16)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 17)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 12], session.SupportColours);
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightHalfBankedHelixUpSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -1, -1, 3, 5, 7, 8, 9, 7, -1, 3, 5, -1, 8, 9, 7, 7 };
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 0 + height }, { 30, 20, 8 } },    { { 0, 6, 0 + height }, { 30, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 30, 20, 8 } },    { { 2, 6, 0 + height }, { 30, 20, 8 } },
                                        { { 2, 6, 30 + height }, { 30, 20, 0 } },   { { 0, 6, 30 + height }, { 30, 20, 0 } },
                                        { { 0, 6, 30 + height }, { 30, 20, 0 } },   { { 2, 6, 30 + height }, { 30, 20, 0 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 30 + height }, { 16, 16, 0 } }, { { 0, 16, 30 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 30 + height }, { 16, 16, 0 } },   { { 16, 0, 30 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 30, 8 } },    { { 6, 0, 0 + height }, { 20, 30, 8 } },
                                        { { 6, 2, 0 + height }, { 20, 30, 8 } },    { { 6, 2, 0 + height }, { 20, 30, 8 } },
                                        { { 6, 0, 30 + height }, { 20, 30, 0 } },   { { 6, 0, 30 + height }, { 20, 30, 0 } },
                                        { { 6, 2, 30 + height }, { 20, 30, 0 } },   { { 6, 2, 30 + height }, { 20, 30, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 24)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 25)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 26)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 27)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 28)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 6 * direction + 29)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 30)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 31)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 36)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 37)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 42)),
                            { 0, 0, height }, { { 6, 2, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 43)),
                            { 0, 0, height }, { { 6, 2, height + 30 }, { 20, 30, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 24)),
                            { 0, 0, height }, { { 6, 2, height + 0 }, { 20, 30, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 25)),
                            { 0, 0, height }, { { 6, 2, height + 30 }, { 20, 30, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 8], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 32)),
                            { 0, 0, height }, { { 16, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 33)),
                            { 0, 0, height }, { { 16, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 38)),
                            { 0, 0, height }, { { 0, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 39)),
                            { 0, 0, height }, { { 0, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 44)),
                            { 0, 0, height }, { { 0, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 45)),
                            { 0, 0, height }, { { 0, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 26)),
                            { 0, 0, height }, { { 16, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 27)),
                            { 0, 0, height }, { { 16, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 34)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 35)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 40)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 41)),
                            { 0, 0, height }, { { 2, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 46)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 47)),
                            { 0, 0, height }, { { 2, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 28)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_SMALL_HELIX + 29)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 30, 20, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 12], session.SupportColours);
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftHalfBankedHelixDownSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapSmallHelixDown[trackSequence];
        TrackRightHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackRightHalfBankedHelixDownSmall(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapSmallHelixDown[trackSequence];
        TrackLeftHalfBankedHelixUpSmall(session, ride, trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackLeftHalfBankedHelixUpLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 3, 2, -2, -2, 7, 9, 9, 7, -2, 3, 2, -2, 7, 7, 9, 9 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 16, 0 + height }, { 16, 16, 8 } },  { { 16, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 0, 0 + height }, { 16, 16, 8 } },  { { 0, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 16, 30 + height }, { 16, 16, 0 } }, { { 16, 16, 30 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 30 + height }, { 16, 16, 0 } }, { { 0, 0, 30 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 2)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 3)),
                    { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 6)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 7)),
                    { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 8)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 9)),
                    { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 30)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 31)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 0)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 1)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 11)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 20)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 21)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 8], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 8:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 32)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 33)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 2)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 3)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 12)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 13)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 22)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 23)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 10:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 34)),
                            { 0, 0, height }, { { 0, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 35)),
                            { 0, 0, height }, { { 0, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 4)),
                            { 0, 0, height }, { { 16, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 5)),
                            { 0, 0, height }, { { 16, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 14)),
                            { 0, 0, height }, { { 16, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 15)),
                            { 0, 0, height }, { { 16, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 24)),
                            { 0, 0, height }, { { 0, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 25)),
                            { 0, 0, height }, { { 0, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 11:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 12:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 36)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 37)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 6)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 7)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 16)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 17)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 26)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 27)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 13:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 38)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 39)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 8)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 9)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 18)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 19)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 28)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 29)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 12], session.SupportColours);
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightHalfBankedHelixUpLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -2, -2, 2, 3, 7, 9, 9, 7, -2, 2, 3, -2, 9, 9, 7, 7 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 16, 16, 8 } },    { { 16, 0, 0 + height }, { 16, 16, 8 } },
                                        { { 16, 16, 0 + height }, { 16, 16, 8 } },  { { 0, 16, 0 + height }, { 16, 16, 8 } },
                                        { { 0, 0, 30 + height }, { 16, 16, 0 } },   { { 16, 0, 30 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 30 + height }, { 16, 16, 0 } }, { { 0, 16, 30 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 40)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 41)),
                    { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 42)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 43)),
                    { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 44)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 45)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 4:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 46)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 47)),
                    { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 6:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 48)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 10 * direction + 49)),
                    { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 7:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 50)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 51)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 60)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 61)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 70)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 71)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 40)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 41)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, 0,
                    height + supportHeights[direction + 8], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 8:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 9:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 52)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 53)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 62)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 63)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 72)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 73)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 42)),
                            { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 43)),
                            { 0, 0, height }, { { 6, 0, height + 30 }, { 20, 32, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 10:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 54)),
                            { 0, 0, height }, { { 0, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 55)),
                            { 0, 0, height }, { { 0, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 64)),
                            { 0, 0, height }, { { 16, 16, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 65)),
                            { 0, 0, height }, { { 16, 16, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 74)),
                            { 0, 0, height }, { { 16, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 75)),
                            { 0, 0, height }, { { 16, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 44)),
                            { 0, 0, height }, { { 0, 0, height + 0 }, { 16, 16, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 45)),
                            { 0, 0, height }, { { 0, 0, height + 30 }, { 16, 16, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 11:
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 12:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 56)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 57)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 66)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 67)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 76)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 77)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 46)),
                            { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 47)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 13:
                switch (direction)
                {
                    case 0:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 58)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 59)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 68)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 69)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 2:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 78)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 79)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 48)),
                            { 0, 0, height }, { { 0, 6, height + 8 }, { 32, 20, 8 } });
                        PaintAddImageAsParentRotated(
                            session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_HELIX + 49)),
                            { 0, 0, height }, { { 0, 6, height + 30 }, { 32, 20, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 12], session.SupportColours);
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftHalfBankedHelixDownLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLargeHelixDown[trackSequence];
        TrackRightHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackRightHalfBankedHelixDownLarge(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = mapLargeHelixDown[trackSequence];
        TrackLeftHalfBankedHelixUpLarge(session, ride, trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackSBendLeft(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (direction > 1)
        {
            trackSequence = 3 - trackSequence;
            direction = (direction + 2) & 3;
        }
        BoundBoxXYZ boundingBoxes[] = { { { 0, 0, 0 + height }, { 32, 26, 8 } },  { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 26, 8 } },  { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } }, { { 0, 0, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } }, { { 0, 6, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 6, 0 + height }, { 32, 26, 8 } },  { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 0 + height }, { 32, 26, 8 } },  { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } }, { { 0, 6, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } }, { { 0, 0, 22 + height }, { 32, 26, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 1)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 6)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 7)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackSBendRight(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        if (direction > 1)
        {
            trackSequence = 3 - trackSequence;
            direction = (direction + 2) & 3;
        }
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 26, 8 } },  { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 0 + height }, { 32, 26, 8 } },  { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } }, { { 0, 6, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } }, { { 0, 0, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 0, 0 + height }, { 32, 26, 8 } },  { { 0, 0, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 26, 8 } },  { { 0, 6, 0 + height }, { 32, 26, 8 } },
                                        { { 0, 0, 22 + height }, { 32, 26, 0 } }, { { 0, 0, 22 + height }, { 32, 26, 0 } },
                                        { { 0, 6, 22 + height }, { 32, 26, 0 } }, { { 0, 6, 22 + height }, { 32, 26, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 16)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 17)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 18)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 19)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 20)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction, session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * direction + 21)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 22)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_S_BEND + 8 * (direction & 1) + 23)),
                    { 0, 0, height }, { { 0, 6, height + 22 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackFlatToUp60LongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 1)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 32 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3,
                        session.SupportColours);
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 2)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 3)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 48 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 7, height + -3,
                        session.SupportColours);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 4)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 5)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 64 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 9, height + -3,
                        session.SupportColours);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 6)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 7)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 98 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 14, height + -3,
                        session.SupportColours);
                }
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 48, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
        }
    }

    static void TrackUp60ToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 32)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 33)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 98 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 16, height + -3,
                        session.SupportColours);
                }
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 34)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 35)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 64 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 16, height + -3,
                        session.SupportColours);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 36)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 37)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 48 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 13, height + -3,
                        session.SupportColours);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 38)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP + 8 * direction + 39)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 32 } });
                if (TrackPaintUtilShouldPaintSupports(session.MapPosition))
                {
                    MetalASupportsPaintSetupRotated(
                        session, supportType.metal, MetalSupportPlace::centre, direction, 5, height + -3,
                        session.SupportColours);
                }
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 8, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackFlatToDown60LongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackUp60ToFlatLongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDown60ToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackFlatToUp60LongBase(session, ride, 3 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackDiagFlatToUp60LongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 20)),
                            { -16, -16, height }, { { -20, -20, height + 8 }, { 24, 24, 1 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 4:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 5:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 6:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 11, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 7:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128);
                break;
            case 8:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128);
                break;
            case 9:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 19, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144);
                break;
        }
    }

    static void TrackDiagUp60ToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 42)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 43)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 24)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 25)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 112);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 36)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 37)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 112);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 30)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 31)),
                            { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 44)),
                            { -16, -16, height }, { { -8, -8, height + 0 }, { 16, 16, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 45)),
                            { -16, -16, height }, { { -16, -16, height + 56 }, { 40, 40, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 28, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 120);
                break;
            case 4:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 26)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 27)),
                        { -16, -16, height }, { { -16, -24, height + 56 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 38)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 39)),
                        { -16, -16, height }, { { -24, -16, height + 56 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 32)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 33)),
                            { -16, -16, height }, { { -24, -24, height + 56 }, { 40, 40, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 46)),
                            { -16, -16, height }, { { -8, -8, height + 56 }, { 16, 16, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 47)),
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 40, 40, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 10, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 7:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 28)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 29)),
                        { -16, -16, height }, { { -16, -24, height + 35 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 8:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 40)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 41)),
                        { -16, -16, height }, { { -24, -16, height + 35 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 9:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 34)),
                        { -16, -16, height }, { { 24, 24, height + 24 }, { 24, 24, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 35)),
                        { -16, -16, height }, { { -24, -24, height + 35 }, { 40, 40, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 3, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackDiagFlatToDown60LongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 34)),
                        { -16, -16, height }, { { 24, 24, height + 24 }, { 24, 24, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 35)),
                        { -16, -16, height }, { { -24, -24, height + 35 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 40)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 41)),
                        { -16, -16, height }, { { -24, -16, height + 35 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 28)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 29)),
                        { -16, -16, height }, { { -16, -24, height + 35 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 46)),
                            { -16, -16, height }, { { -8, -8, height + 56 }, { 16, 16, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 47)),
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 40, 40, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 32)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 33)),
                            { -16, -16, height }, { { -24, -24, height + 56 }, { 40, 40, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 24, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 4:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 38)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 39)),
                        { -16, -16, height }, { { -24, -16, height + 56 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 26)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 27)),
                        { -16, -16, height }, { { -16, -24, height + 56 }, { 40, 40, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 6:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 44)),
                            { -16, -16, height }, { { -8, -8, height + 0 }, { 16, 16, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 45)),
                            { -16, -16, height }, { { -16, -16, height + 56 }, { 40, 40, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 30)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 31)),
                            { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 58, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 120);
                break;
            case 7:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 36)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 37)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 112);
                break;
            case 8:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 24)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 25)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 112);
                break;
            case 9:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 42)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 43)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 14, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
        }
    }

    static void TrackDiagDown60ToFlatLongBase(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                if (direction == 3)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 10)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 11)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 144);
                break;
            case 1:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 16)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 17)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128);
                break;
            case 2:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 4)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 5)),
                        { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128);
                break;
            case 3:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 22)),
                            { -16, -16, height }, { { -16, -16, height + 32 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 23)),
                            { -16, -16, height }, { { -16, -16, height + 96 }, { 32, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 8)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 9)),
                            { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 38, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 104);
                break;
            case 4:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 14)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 15)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 5:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 2)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 3)),
                        { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 80);
                break;
            case 6:
                switch (direction)
                {
                    case 1:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 20)),
                            { -16, -16, height }, { { -20, -20, height + 8 }, { 24, 24, 1 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 21)),
                            { -16, -16, height }, { { -16, -16, height + 80 }, { 32, 32, 0 } });
                        break;
                    case 3:
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 6)),
                            { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                        PaintAddImageAsParentRotated(
                            session, direction,
                            session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 7)),
                            { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                        break;
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 12, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight, PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 7:
                if (direction == 0)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 12)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 13)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::topLeft, PaintSegment::top, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 8:
                if (direction == 2)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 0)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 1)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 9:
                if (direction == 1)
                {
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 18)),
                        { -16, -16, height }, { { -16, -16, height + 0 }, { 32, 32, 3 } });
                    PaintAddImageAsParentRotated(
                        session, direction,
                        session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_FLAT_TO_STEEP_DIAGONAL + 19)),
                        { -16, -16, height }, { { -16, -16, height + 35 }, { 32, 32, 0 } });
                }
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::leftCorner, direction, 0, height + -3,
                    session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 40);
                break;
        }
    }

    static void TrackLeftBankToLeftQuarterTurn3TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 7, 3, -1, 1 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },   { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },  { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 16, 1, 30 } },  { { 16, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 16, -8, 0 + height }, { 16, 24, 8 } }, { { 0, -8, 0 + height }, { 16, 24, 8 } },
                                        { { 0, 16, 0 + height }, { 16, 24, 8 } },  { { 16, 16, 0 + height }, { 16, 24, 8 } },
                                        { { 16, 0, 38 + height }, { 16, 16, 0 } }, { { 0, 0, 38 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 38 + height }, { 16, 16, 0 } }, { { 16, 16, 38 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },   { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 0, 0 + height }, { 1, 16, 30 } },  { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },  { { 27, 16, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -6, session.SupportColours);
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height - 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackRightBankToRightQuarterTurn3TilesUp25(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 1, -1, 3, 7 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 8 } },    { { 0, 6, 0 + height }, { 32, 20, 8 } },
                                        { { 16, 27, 0 + height }, { 16, 1, 30 } },  { { 0, 27, 0 + height }, { 16, 1, 30 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 30 } },   { { 0, 27, 0 + height }, { 32, 1, 30 } },
                                        { { 16, 16, 0 + height }, { 16, 24, 8 } },  { { 0, 16, 0 + height }, { 16, 24, 8 } },
                                        { { 0, -8, 0 + height }, { 16, 24, 8 } },   { { 16, -8, 0 + height }, { 16, 24, 8 } },
                                        { { 16, 16, 38 + height }, { 16, 16, 0 } }, { { 0, 16, 38 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 38 + height }, { 16, 16, 0 } },   { { 16, 0, 38 + height }, { 16, 16, 0 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },    { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 6, 0, 0 + height }, { 20, 32, 8 } },    { { 6, 0, 0 + height }, { 20, 32, 8 } },
                                        { { 27, 16, 0 + height }, { 1, 16, 30 } },  { { 27, 0, 0 + height }, { 1, 32, 30 } },
                                        { { 27, 0, 0 + height }, { 1, 32, 30 } },   { { 27, 0, 0 + height }, { 1, 16, 30 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 24)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 25)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0,
                    height + supportHeights[direction + 0], session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 26)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 27)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 28)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_TURN_BANK_TRANSITION + 6 * direction + 29)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -6, session.SupportColours);
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height - 8, kTunnelGroup, TunnelSubType::SlopeEnd);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::centre, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackLeftQuarterTurn3TilesDown25ToLeftBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackRightBankToRightQuarterTurn3TilesUp25(
            session, ride, trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    static void TrackRightQuarterTurn3TilesDown25ToRightBank(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        trackSequence = kMapLeftQuarterTurn3TilesToRightQuarterTurn3Tiles[trackSequence];
        TrackLeftBankToLeftQuarterTurn3TilesUp25(
            session, ride, trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftTwistDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 1)),
                    { 0, 0, height }, { { 0, 6, height + 32 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::rightCorner, direction, 0, height + 1,
                    session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 2)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 3)),
                    { 0, 0, height }, { { 0, 6, height + 32 }, { 32, 20, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 4)),
                    { 0, 0, height }, { { 0, 6, height + -16 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 5)),
                    { 0, 0, height }, { { 0, 6, height + 16 }, { 32, 20, 0 } });
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackRightTwistDownToUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 24)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 25)),
                    { 0, 0, height }, { { 0, 6, height + 32 }, { 32, 20, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, 0, height + 1, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 26)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 27)),
                    { 0, 0, height }, { { 0, 6, height + 32 }, { 32, 20, 0 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 28)),
                    { 0, 0, height }, { { 0, 6, height + -16 }, { 32, 20, 2 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_INLINE_TWIST + 6 * direction + 29)),
                    { 0, 0, height }, { { 0, 6, height + 16 }, { 32, 20, 0 } });
                if (direction == 1 || direction == 2)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 16, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 32);
                break;
        }
    }

    static void TrackLeftTwistUpToDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftTwistDownToUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackRightTwistUpToDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightTwistDownToUp(session, ride, 2 - trackSequence, (direction + 2) & 3, height, trackElement, supportType);
    }

    static void TrackHalfLoopUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = {
            { { 16, 6, 0 + height }, { 4, 20, 119 } },  { { 12, 6, 0 + height }, { 4, 20, 119 } },
            { { 12, 6, 0 + height }, { 4, 20, 119 } },  { { 16, 6, 0 + height }, { 4, 20, 119 } },
            { { 16, 27, 0 + height }, { 16, 1, 119 } }, { { 0, 27, 0 + height }, { 16, 1, 119 } },
            { { 0, 27, 0 + height }, { 16, 1, 119 } },  { { 16, 27, 0 + height }, { 16, 1, 119 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 0)), { 0, 0, height },
                    { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 1)), { 0, 0, height },
                    { { 0, 27, height + 0 }, { 32, 1, 31 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 2)), { 0, 0, height },
                    { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 3)), { 0, 0, height },
                    { { 0, 27, height + 0 }, { 32, 1, 48 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 8, height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 4)), { 0, 0, height },
                    boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 5)), { 0, 0, height },
                    boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 128);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 6)), { 0, 0, height },
                    { { 0, 4, height + 0 }, { 32, 1, 31 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_HALF_LOOP + 8 * direction + 7)), { 0, 0, height },
                    { { 0, 6, height + 40 }, { 32, 20, 1 } });
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 136, kTunnelGroup, TunnelSubType::FlatTo25Deg);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackHalfLoopUp(session, ride, 3 - trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackLeftMediumHalfLoopUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { -3, -3, 21, 21, 24, 28, 9, 0 };
        BoundBoxXYZ boundingBoxes[] = { { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 64 } },   { { 0, 27, 0 + height }, { 32, 1, 64 } },
                                        { { 0, 6, 64 + height }, { 32, 20, 1 } },   { { 0, 6, 64 + height }, { 32, 20, 1 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 64 } },   { { 0, 27, 0 + height }, { 32, 1, 64 } },
                                        { { 0, 6, 80 + height }, { 32, 20, 1 } },   { { 0, 6, 80 + height }, { 32, 20, 1 } },
                                        { { 6, 0, 0 + height }, { 10, 26, 24 } },   { { 0, 6, -4 + height }, { 32, 20, 0 } },
                                        { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 8, 6, 0 + height }, { 24, 26, 3 } },
                                        { { 0, 27, 0 + height }, { 32, 1, 112 } },  { { 0, 27, 0 + height }, { 32, 1, 112 } },
                                        { { 0, 44, 0 + height }, { 32, 1, 112 } },  { { 0, 44, 0 + height }, { 32, 1, 112 } },
                                        { { 0, 2, 0 + height }, { 32, 0, 150 } },   { { 0, 2, 0 + height }, { 32, 0, 150 } },
                                        { { 0, -8, 0 + height }, { 32, 0, 150 } },  { { 0, -8, 0 + height }, { 32, 0, 150 } },
                                        { { 0, 40, 112 + height }, { 32, 0, 38 } }, { { 0, 40, 112 + height }, { 32, 0, 38 } },
                                        { { 0, 30, 0 + height }, { 32, 0, 150 } },  { { 0, 30, 0 + height }, { 32, 0, 150 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 7, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 2)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 3)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topCorner, direction, supportHeights[direction + 0],
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 152);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 6)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 7)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 152);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 8)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 9)),
                    { 0, 0, height }, { { 0, 6, height + 56 }, { 32, 20, 3 } });
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 32, kTunnelGroup, TunnelSubType::Tall);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
        }
    }

    static void TrackRightMediumHalfLoopUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 21, 21, -3, -3, 0, 9, 28, 24 };
        BoundBoxXYZ boundingBoxes[] = {
            { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
            { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
            { { 0, 6, 64 + height }, { 32, 20, 1 } },   { { 0, 6, 64 + height }, { 32, 20, 1 } },
            { { 0, 27, 0 + height }, { 32, 1, 64 } },   { { 0, 27, 0 + height }, { 32, 1, 64 } },
            { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
            { { 0, 6, 0 + height }, { 32, 20, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
            { { 0, 6, 80 + height }, { 32, 20, 1 } },   { { 0, 6, 80 + height }, { 32, 20, 1 } },
            { { 0, 27, 0 + height }, { 32, 1, 64 } },   { { 0, 27, 0 + height }, { 32, 1, 64 } },
            { { 8, 6, 0 + height }, { 24, 26, 3 } },    { { 0, 6, 0 + height }, { 32, 20, 3 } },
            { { 0, 6, -4 + height }, { 32, 20, 0 } },   { { 6, 0, 0 + height }, { 10, 26, 24 } },
            { { 0, 44, 0 + height }, { 32, 1, 112 } },  { { 0, 44, 0 + height }, { 32, 1, 112 } },
            { { 0, 27, 0 + height }, { 32, 1, 112 } },  { { 0, 27, 0 + height }, { 32, 1, 112 } },
            { { 0, -8, 0 + height }, { 32, 0, 150 } },  { { 0, -8, 0 + height }, { 32, 0, 150 } },
            { { 0, 2, 0 + height }, { 32, 0, 150 } },   { { 0, 2, 0 + height }, { 32, 0, 150 } },
            { { 0, 30, 0 + height }, { 32, 0, 150 } },  { { 0, 30, 0 + height }, { 32, 0, 150 } },
            { { 0, 40, 112 + height }, { 32, 0, 38 } }, { { 0, 40, 112 + height }, { 32, 0, 38 } }
        };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 40)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 41)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 7, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 42)),
                    { 0, 0, height }, boundingBoxes[direction + 8]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 43)),
                    { 0, 0, height }, boundingBoxes[direction + 12]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 44)),
                    { 0, 0, height }, boundingBoxes[direction + 16]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 45)),
                    { 0, 0, height }, boundingBoxes[direction + 20]);
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::rightCorner, direction, supportHeights[direction + 0],
                    height + supportHeights[direction + 4], session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 152);
                break;
            case 3:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 46)),
                    { 0, 0, height }, boundingBoxes[direction + 24]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 47)),
                    { 0, 0, height }, boundingBoxes[direction + 28]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 152);
                break;
            case 4:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 48)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_MEDIUM_HALF_LOOP + 10 * direction + 49)),
                    { 0, 0, height }, { { 0, 6, height + 56 }, { 32, 20, 3 } });
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height + 32, kTunnelGroup, TunnelSubType::Tall);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
        }
    }

    static void TrackLeftMediumHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightMediumHalfLoopUp(session, ride, 4 - trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackRightMediumHalfLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftMediumHalfLoopUp(session, ride, 4 - trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void Track90DegToInvertedFlatQuarterLoopUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        BoundBoxXYZ boundingBoxes[] = { { { 2, 6, 8 + height }, { 2, 20, 64 } },  { { 0, 4, 8 + height }, { 32, 1, 64 } },
                                        { { 0, 4, 8 + height }, { 32, 1, 64 } },  { { 2, 6, 8 + height }, { 2, 20, 64 } },
                                        { { 0, 27, 8 + height }, { 32, 1, 64 } }, { { 28, 6, 8 + height }, { 2, 20, 64 } },
                                        { { 28, 6, 8 + height }, { 2, 20, 64 } }, { { 0, 27, 8 + height }, { 32, 1, 64 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 0)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 1)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 88);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 2)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 3)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 64 } });
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 4)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 3 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_QUARTER_LOOP + 6 * direction + 5)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 32 } });
                if (direction == 3 || direction == 0)
                {
                    PaintUtilPushTunnelRotated(session, direction, height - 8, kTunnelGroup, TunnelSubType::Tall);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight), direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
        }
    }

    static void TrackInvertedFlatTo90DegQuarterLoopDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        Track90DegToInvertedFlatQuarterLoopUp(
            session, ride, 2 - trackSequence, (direction + 0) & 3, height, trackElement, supportType);
    }

    static void TrackLeftLargeCorkscrewUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 21, 19, 19, 16 };
        BoundBoxXYZ boundingBoxes[] = { { { 16, 0, 0 + height }, { 16, 16, 0 } },  { { 0, 0, 0 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 0 + height }, { 16, 16, 0 } },  { { 16, 16, 0 + height }, { 16, 16, 0 } },
                                        { { 16, 0, 70 + height }, { 16, 16, 0 } }, { { 0, 0, 70 + height }, { 16, 16, 0 } },
                                        { { 0, 16, 70 + height }, { 16, 16, 0 } }, { { 16, 16, 70 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 0)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 1)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 30 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 2)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 3)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 30 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::top, PaintSegment::bottomLeft,
                            PaintSegment::centre, PaintSegment::topRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 4)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 5)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 6)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 7)),
                    { 0, 0, height }, { { 6, 0, height + 70 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topLeftSide, direction, 0, height + 83,
                    session.SupportColours);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 8)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 9)),
                    { 0, 0, height }, { { 6, 0, height + 54 }, { 20, 32, 0 } });
                if (direction == 2 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 24, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackRightLargeCorkscrewUp(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        int8_t supportHeights[] = { 16, 19, 19, 21 };
        BoundBoxXYZ boundingBoxes[] = { { { 16, 16, 0 + height }, { 16, 16, 0 } },  { { 0, 16, 0 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 0 + height }, { 16, 16, 0 } },    { { 16, 0, 0 + height }, { 16, 16, 0 } },
                                        { { 16, 16, 70 + height }, { 16, 16, 0 } }, { { 0, 16, 70 + height }, { 16, 16, 0 } },
                                        { { 0, 0, 70 + height }, { 16, 16, 0 } },   { { 16, 0, 70 + height }, { 16, 16, 0 } } };
        switch (trackSequence)
        {
            case 0:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 40)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 41)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 30 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::centre, direction, 0, height + -3, session.SupportColours);
                if (direction == 0 || direction == 3)
                {
                    PaintUtilPushTunnelRotated(session, direction, height, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottomRight,
                            PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 48);
                break;
            case 1:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 42)),
                    { 0, 0, height }, { { 0, 6, height + 0 }, { 32, 20, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 43)),
                    { 0, 0, height }, { { 0, 27, height + 0 }, { 32, 1, 30 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::topRightSide, direction, supportHeights[direction + 0],
                    height + -3, session.SupportColours);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::topRight, PaintSegment::bottom,
                            PaintSegment::bottomRight, PaintSegment::right),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 64);
                break;
            case 2:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 44)),
                    { 0, 0, height }, boundingBoxes[direction + 0]);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 45)),
                    { 0, 0, height }, boundingBoxes[direction + 4]);
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::bottomLeft, PaintSegment::centre, PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 3:
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
            case 4:
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 46)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 47)),
                    { 0, 0, height }, { { 6, 0, height + 70 }, { 20, 32, 0 } });
                MetalASupportsPaintSetupRotated(
                    session, supportType.metal, MetalSupportPlace::bottomRightSide, direction, 0, height + 83,
                    session.SupportColours);
                PaintUtilSetGeneralSupportHeight(session, height + 72);
                break;
            case 5:
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 48)),
                    { 0, 0, height }, { { 6, 0, height + 0 }, { 20, 32, 8 } });
                PaintAddImageAsParentRotated(
                    session, direction,
                    session.TrackColours.WithIndex((SPR_TRACKS_PIPELINE_TRACK_LARGE_CORKSCREW + 10 * direction + 49)),
                    { 0, 0, height }, { { 6, 0, height + 54 }, { 20, 32, 0 } });
                if (direction == 0 || direction == 1)
                {
                    PaintUtilPushTunnelRotated(session, direction + 1, height + 24, kTunnelGroup, TunnelSubType::Flat);
                }
                PaintUtilSetSegmentSupportHeight(
                    session,
                    PaintUtilRotateSegments(
                        EnumsToFlags(
                            PaintSegment::left, PaintSegment::topLeft, PaintSegment::bottomLeft, PaintSegment::centre,
                            PaintSegment::bottom, PaintSegment::bottomRight),
                        direction),
                    0xFFFF, 0);
                PaintUtilSetGeneralSupportHeight(session, height + 56);
                break;
        }
    }

    static void TrackRightLargeCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackLeftLargeCorkscrewUp(session, ride, 5 - trackSequence, (direction + 3) & 3, height, trackElement, supportType);
    }

    static void TrackLeftLargeCorkscrewDown(
        PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
        const TrackElement& trackElement, SupportType supportType)
    {
        TrackRightLargeCorkscrewUp(session, ride, 5 - trackSequence, (direction + 1) & 3, height, trackElement, supportType);
    }

    TrackPaintFunction GetTrackPaintFunction(OpenRCT2::TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::Flat:
                return TrackFlat;
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return TrackStation;
            case TrackElemType::Up25:
                return TrackUp25;
            case TrackElemType::Up60:
                return TrackUp60;
            case TrackElemType::FlatToUp25:
                return TrackFlatToUp25;
            case TrackElemType::Up25ToUp60:
                return TrackUp25ToUp60;
            case TrackElemType::Up60ToUp25:
                return TrackUp60ToUp25;
            case TrackElemType::Up25ToFlat:
                return TrackUp25ToFlat;
            case TrackElemType::Down25:
                return TrackDown25;
            case TrackElemType::Down60:
                return TrackDown60;
            case TrackElemType::FlatToDown25:
                return TrackFlatToDown25;
            case TrackElemType::Down25ToDown60:
                return TrackDown25ToDown60;
            case TrackElemType::Down60ToDown25:
                return TrackDown60ToDown25;
            case TrackElemType::Down25ToFlat:
                return TrackDown25ToFlat;
            case TrackElemType::LeftQuarterTurn5Tiles:
                return TrackLeftQuarterTurn5;
            case TrackElemType::RightQuarterTurn5Tiles:
                return TrackRightQuarterTurn5;
            case TrackElemType::FlatToLeftBank:
                return TrackFlatToLeftBank;
            case TrackElemType::FlatToRightBank:
                return TrackFlatToRightBank;
            case TrackElemType::LeftBankToFlat:
                return TrackLeftBankToFlat;
            case TrackElemType::RightBankToFlat:
                return TrackRightBankToFlat;
            case TrackElemType::BankedLeftQuarterTurn5Tiles:
                return TrackBankedLeftQuarterTurn5;
            case TrackElemType::BankedRightQuarterTurn5Tiles:
                return TrackBankedRightQuarterTurn5;
            case TrackElemType::LeftBankToUp25:
                return TrackLeftBankToUp25;
            case TrackElemType::RightBankToUp25:
                return TrackRightBankToUp25;
            case TrackElemType::Up25ToLeftBank:
                return TrackUp25ToLeftBank;
            case TrackElemType::Up25ToRightBank:
                return TrackUp25ToRightBank;
            case TrackElemType::LeftBankToDown25:
                return TrackLeftBankToDown25;
            case TrackElemType::RightBankToDown25:
                return TrackRightBankToDown25;
            case TrackElemType::Down25ToLeftBank:
                return TrackDown25ToLeftBank;
            case TrackElemType::Down25ToRightBank:
                return TrackDown25ToRightBank;
            case TrackElemType::LeftBank:
                return TrackLeftBank;
            case TrackElemType::RightBank:
                return TrackRightBank;
            case TrackElemType::LeftQuarterTurn5TilesUp25:
                return TrackLeftQuarterTurn5TilesUp25;
            case TrackElemType::RightQuarterTurn5TilesUp25:
                return TrackRightQuarterTurn5TilesUp25;
            case TrackElemType::LeftQuarterTurn5TilesDown25:
                return TrackLeftQuarterTurn5TilesDown25;
            case TrackElemType::RightQuarterTurn5TilesDown25:
                return TrackRightQuarterTurn5TilesDown25;
            case TrackElemType::LeftBankedQuarterTurn3TileUp25:
                return TrackLeftBankedQuarterTurn3TileUp25;
            case TrackElemType::RightBankedQuarterTurn3TileUp25:
                return TrackRightBankedQuarterTurn3TileUp25;
            case TrackElemType::LeftBankedQuarterTurn3TileDown25:
                return TrackLeftBankedQuarterTurn3TileDown25;
            case TrackElemType::RightBankedQuarterTurn3TileDown25:
                return TrackRightBankedQuarterTurn3TileDown25;
            case TrackElemType::LeftBankedQuarterTurn5TileUp25:
                return TrackLeftBankedQuarterTurn5TileUp25;
            case TrackElemType::RightBankedQuarterTurn5TileUp25:
                return TrackRightBankedQuarterTurn5TileUp25;
            case TrackElemType::LeftBankedQuarterTurn5TileDown25:
                return TrackLeftBankedQuarterTurn5TileDown25;
            case TrackElemType::RightBankedQuarterTurn5TileDown25:
                return TrackRightBankedQuarterTurn5TileDown25;
            case TrackElemType::Up25ToLeftBankedUp25:
                return TrackUp25ToLeftBankedUp25;
            case TrackElemType::Up25ToRightBankedUp25:
                return TrackUp25ToRightBankedUp25;
            case TrackElemType::LeftBankedUp25ToUp25:
                return TrackLeftBankedUp25ToUp25;
            case TrackElemType::RightBankedUp25ToUp25:
                return TrackRightBankedUp25ToUp25;
            case TrackElemType::Down25ToLeftBankedDown25:
                return TrackDown25ToLeftBankedDown25;
            case TrackElemType::Down25ToRightBankedDown25:
                return TrackDown25ToRightBankedDown25;
            case TrackElemType::LeftBankedDown25ToDown25:
                return TrackLeftBankedDown25ToDown25;
            case TrackElemType::RightBankedDown25ToDown25:
                return TrackRightBankedDown25ToDown25;
            case TrackElemType::LeftBankedFlatToLeftBankedUp25:
                return TrackLeftBankedFlatToLeftBankedUp25;
            case TrackElemType::RightBankedFlatToRightBankedUp25:
                return TrackRightBankedFlatToRightBankedUp25;
            case TrackElemType::LeftBankedUp25ToLeftBankedFlat:
                return TrackLeftBankedUp25ToLeftBankedFlat;
            case TrackElemType::RightBankedUp25ToRightBankedFlat:
                return TrackRightBankedUp25ToRightBankedFlat;
            case TrackElemType::LeftBankedFlatToLeftBankedDown25:
                return TrackLeftBankedFlatToLeftBankedDown25;
            case TrackElemType::RightBankedFlatToRightBankedDown25:
                return TrackRightBankedFlatToRightBankedDown25;
            case TrackElemType::LeftBankedDown25ToLeftBankedFlat:
                return TrackLeftBankedDown25ToLeftBankedFlat;
            case TrackElemType::RightBankedDown25ToRightBankedFlat:
                return TrackRightBankedDown25ToRightBankedFlat;
            case TrackElemType::FlatToLeftBankedUp25:
                return TrackFlatToLeftBankedUp25;
            case TrackElemType::FlatToRightBankedUp25:
                return TrackFlatToRightBankedUp25;
            case TrackElemType::LeftBankedUp25ToFlat:
                return TrackLeftBankedUp25ToFlat;
            case TrackElemType::RightBankedUp25ToFlat:
                return TrackRightBankedUp25ToFlat;
            case TrackElemType::FlatToLeftBankedDown25:
                return TrackFlatToLeftBankedDown25;
            case TrackElemType::FlatToRightBankedDown25:
                return TrackFlatToRightBankedDown25;
            case TrackElemType::LeftBankedDown25ToFlat:
                return TrackLeftBankedDown25ToFlat;
            case TrackElemType::RightBankedDown25ToFlat:
                return TrackRightBankedDown25ToFlat;
            case TrackElemType::Up25LeftBanked:
                return TrackUp25LeftBanked;
            case TrackElemType::Up25RightBanked:
                return TrackUp25RightBanked;
            case TrackElemType::Down25LeftBanked:
                return TrackDown25LeftBanked;
            case TrackElemType::Down25RightBanked:
                return TrackDown25RightBanked;
            case TrackElemType::SBendLeft:
                return TrackSBendLeft;
            case TrackElemType::SBendRight:
                return TrackSBendRight;
            case TrackElemType::LeftQuarterTurn3Tiles:
                return TrackLeftQuarterTurn3;
            case TrackElemType::RightQuarterTurn3Tiles:
                return TrackRightQuarterTurn3;
            case TrackElemType::LeftBankedQuarterTurn3Tiles:
                return TrackBankedLeftQuarterTurn3;
            case TrackElemType::RightBankedQuarterTurn3Tiles:
                return TrackBankedRightQuarterTurn3;
            case TrackElemType::LeftQuarterTurn3TilesUp25:
                return TrackLeftQuarterTurn3TilesUp25;
            case TrackElemType::RightQuarterTurn3TilesUp25:
                return TrackRightQuarterTurn3TilesUp25;
            case TrackElemType::LeftQuarterTurn3TilesDown25:
                return TrackLeftQuarterTurn3TilesDown25;
            case TrackElemType::RightQuarterTurn3TilesDown25:
                return TrackRightQuarterTurn3TilesDown25;
            case TrackElemType::LeftHalfBankedHelixUpSmall:
                return TrackLeftHalfBankedHelixUpSmall;
            case TrackElemType::RightHalfBankedHelixUpSmall:
                return TrackRightHalfBankedHelixUpSmall;
            case TrackElemType::LeftHalfBankedHelixDownSmall:
                return TrackLeftHalfBankedHelixDownSmall;
            case TrackElemType::RightHalfBankedHelixDownSmall:
                return TrackRightHalfBankedHelixDownSmall;
            case TrackElemType::LeftHalfBankedHelixUpLarge:
                return TrackLeftHalfBankedHelixUpLarge;
            case TrackElemType::RightHalfBankedHelixUpLarge:
                return TrackRightHalfBankedHelixUpLarge;
            case TrackElemType::LeftHalfBankedHelixDownLarge:
                return TrackLeftHalfBankedHelixDownLarge;
            case TrackElemType::RightHalfBankedHelixDownLarge:
                return TrackRightHalfBankedHelixDownLarge;
            case TrackElemType::LeftQuarterTurn1TileUp60:
                return TrackLeftQuarterTurn1TileUp60;
            case TrackElemType::RightQuarterTurn1TileUp60:
                return TrackRightQuarterTurn1TileUp60;
            case TrackElemType::LeftQuarterTurn1TileDown60:
                return TrackLeftQuarterTurn1TileDown60;
            case TrackElemType::RightQuarterTurn1TileDown60:
                return TrackRightQuarterTurn1TileDown60;
            case TrackElemType::Up90:
                return TrackUp90;
            case TrackElemType::Down90:
                return TrackDown90;
            case TrackElemType::Up60ToUp90:
                return TrackUp60ToUp90;
            case TrackElemType::Down90ToDown60:
                return TrackDown90ToDown60;
            case TrackElemType::Up90ToUp60:
                return TrackUp90ToUp60;
            case TrackElemType::Down60ToDown90:
                return TrackDown60ToDown90;
            case TrackElemType::LeftQuarterTurn1TileUp90:
                return TrackLeftQuarterTurn1Up90;
            case TrackElemType::RightQuarterTurn1TileUp90:
                return TrackRightQuarterTurn1Up90;
            case TrackElemType::LeftQuarterTurn1TileDown90:
                return TrackLeftQuarterTurn1Down90;
            case TrackElemType::RightQuarterTurn1TileDown90:
                return TrackRightQuarterTurn1Down90;
            case TrackElemType::FlatToUp60LongBase:
                return TrackFlatToUp60LongBase;
            case TrackElemType::Up60ToFlatLongBase:
                return TrackUp60ToFlatLongBase;
            case TrackElemType::FlatToDown60LongBase:
                return TrackFlatToDown60LongBase;
            case TrackElemType::Down60ToFlatLongBase:
                return TrackDown60ToFlatLongBase;
            case TrackElemType::DiagFlatToUp60LongBase:
                return TrackDiagFlatToUp60LongBase;
            case TrackElemType::DiagUp60ToFlatLongBase:
                return TrackDiagUp60ToFlatLongBase;
            case TrackElemType::DiagFlatToDown60LongBase:
                return TrackDiagFlatToDown60LongBase;
            case TrackElemType::DiagDown60ToFlatLongBase:
                return TrackDiagDown60ToFlatLongBase;
            case TrackElemType::LeftEighthToDiag:
                return TrackLeftEighthToDiag;
            case TrackElemType::RightEighthToDiag:
                return TrackRightEighthToDiag;
            case TrackElemType::LeftEighthToOrthogonal:
                return TrackLeftEighthToOrthogonal;
            case TrackElemType::RightEighthToOrthogonal:
                return TrackRightEighthToOrthogonal;
            case TrackElemType::LeftEighthBankToDiag:
                return TrackLeftEighthBankToDiag;
            case TrackElemType::RightEighthBankToDiag:
                return TrackRightEighthBankToDiag;
            case TrackElemType::LeftEighthBankToOrthogonal:
                return TrackLeftEighthBankToOrthogonal;
            case TrackElemType::RightEighthBankToOrthogonal:
                return TrackRightEighthBankToOrthogonal;
            case TrackElemType::DiagFlat:
                return TrackDiagFlat;
            case TrackElemType::DiagBrakes:
                return TrackDiagBrakes;
            case TrackElemType::DiagBlockBrakes:
                return TrackDiagBlockBrakes;
            case TrackElemType::DiagUp25:
                return TrackDiagUp25;
            case TrackElemType::DiagUp60:
                return TrackDiagUp60;
            case TrackElemType::DiagFlatToUp25:
                return TrackDiagFlatToUp25;
            case TrackElemType::DiagUp25ToUp60:
                return TrackDiagUp25ToUp60;
            case TrackElemType::DiagUp60ToUp25:
                return TrackDiagUp60ToUp25;
            case TrackElemType::DiagUp25ToFlat:
                return TrackDiagUp25ToFlat;
            case TrackElemType::DiagDown25:
                return TrackDiagDown25;
            case TrackElemType::DiagDown60:
                return TrackDiagDown60;
            case TrackElemType::DiagFlatToDown25:
                return TrackDiagFlatToDown25;
            case TrackElemType::DiagDown25ToDown60:
                return TrackDiagDown25ToDown60;
            case TrackElemType::DiagDown60ToDown25:
                return TrackDiagDown60ToDown25;
            case TrackElemType::DiagDown25ToFlat:
                return TrackDiagDown25ToFlat;
            case TrackElemType::DiagFlatToLeftBank:
                return TrackDiagFlatToLeftBank;
            case TrackElemType::DiagFlatToRightBank:
                return TrackDiagFlatToRightBank;
            case TrackElemType::DiagLeftBankToFlat:
                return TrackDiagLeftBankToFlat;
            case TrackElemType::DiagRightBankToFlat:
                return TrackDiagRightBankToFlat;
            case TrackElemType::DiagLeftBankToUp25:
                return TrackDiagLeftBankToUp25;
            case TrackElemType::DiagRightBankToUp25:
                return TrackDiagRightBankToUp25;
            case TrackElemType::DiagUp25ToLeftBank:
                return TrackDiagUp25ToLeftBank;
            case TrackElemType::DiagUp25ToRightBank:
                return TrackDiagUp25ToRightBank;
            case TrackElemType::DiagLeftBankToDown25:
                return TrackDiagLeftBankToDown25;
            case TrackElemType::DiagRightBankToDown25:
                return TrackDiagRightBankToDown25;
            case TrackElemType::DiagDown25ToLeftBank:
                return TrackDiagDown25ToLeftBank;
            case TrackElemType::DiagDown25ToRightBank:
                return TrackDiagDown25ToRightBank;
            case TrackElemType::DiagLeftBank:
                return TrackDiagLeftBank;
            case TrackElemType::DiagRightBank:
                return TrackDiagRightBank;
            case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
                return TrackLeftBankToLeftQuarterTurn3TilesUp25;
            case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
                return TrackRightBankToRightQuarterTurn3TilesUp25;
            case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
                return TrackLeftQuarterTurn3TilesDown25ToLeftBank;
            case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
                return TrackRightQuarterTurn3TilesDown25ToRightBank;
            case TrackElemType::LeftEighthToDiagUp25:
                return TrackLeftEighthToDiagUp25;
            case TrackElemType::RightEighthToDiagUp25:
                return TrackRightEighthToDiagUp25;
            case TrackElemType::LeftEighthToDiagDown25:
                return TrackLeftEighthToDiagDown25;
            case TrackElemType::RightEighthToDiagDown25:
                return TrackRightEighthToDiagDown25;
            case TrackElemType::LeftEighthToOrthogonalUp25:
                return TrackLeftEighthToOrthogonalUp25;
            case TrackElemType::RightEighthToOrthogonalUp25:
                return TrackRightEighthToOrthogonalUp25;
            case TrackElemType::LeftEighthToOrthogonalDown25:
                return TrackLeftEighthToOrthogonalDown25;
            case TrackElemType::RightEighthToOrthogonalDown25:
                return TrackRightEighthToOrthogonalDown25;
            case TrackElemType::DiagUp25ToLeftBankedUp25:
                return TrackDiagUp25ToLeftBankedUp25;
            case TrackElemType::DiagUp25ToRightBankedUp25:
                return TrackDiagUp25ToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToUp25:
                return TrackDiagLeftBankedUp25ToUp25;
            case TrackElemType::DiagRightBankedUp25ToUp25:
                return TrackDiagRightBankedUp25ToUp25;
            case TrackElemType::DiagDown25ToLeftBankedDown25:
                return TrackDiagDown25ToLeftBankedDown25;
            case TrackElemType::DiagDown25ToRightBankedDown25:
                return TrackDiagDown25ToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToDown25:
                return TrackDiagLeftBankedDown25ToDown25;
            case TrackElemType::DiagRightBankedDown25ToDown25:
                return TrackDiagRightBankedDown25ToDown25;
            case TrackElemType::DiagLeftBankedFlatToLeftBankedUp25:
                return TrackDiagLeftBankedFlatToLeftBankedUp25;
            case TrackElemType::DiagRightBankedFlatToRightBankedUp25:
                return TrackDiagRightBankedFlatToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToLeftBankedFlat:
                return TrackDiagLeftBankedUp25ToLeftBankedFlat;
            case TrackElemType::DiagRightBankedUp25ToRightBankedFlat:
                return TrackDiagRightBankedUp25ToRightBankedFlat;
            case TrackElemType::DiagLeftBankedFlatToLeftBankedDown25:
                return TrackDiagLeftBankedFlatToLeftBankedDown25;
            case TrackElemType::DiagRightBankedFlatToRightBankedDown25:
                return TrackDiagRightBankedFlatToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToLeftBankedFlat:
                return TrackDiagLeftBankedDown25ToLeftBankedFlat;
            case TrackElemType::DiagRightBankedDown25ToRightBankedFlat:
                return TrackDiagRightBankedDown25ToRightBankedFlat;
            case TrackElemType::DiagUp25LeftBanked:
                return TrackDiagUp25LeftBanked;
            case TrackElemType::DiagUp25RightBanked:
                return TrackDiagUp25RightBanked;
            case TrackElemType::DiagDown25LeftBanked:
                return TrackDiagDown25LeftBanked;
            case TrackElemType::DiagDown25RightBanked:
                return TrackDiagDown25RightBanked;
            case TrackElemType::DiagFlatToLeftBankedUp25:
                return TrackDiagFlatToLeftBankedUp25;
            case TrackElemType::DiagFlatToRightBankedUp25:
                return TrackDiagFlatToRightBankedUp25;
            case TrackElemType::DiagLeftBankedUp25ToFlat:
                return TrackDiagLeftBankedUp25ToFlat;
            case TrackElemType::DiagRightBankedUp25ToFlat:
                return TrackDiagRightBankedUp25ToFlat;
            case TrackElemType::DiagFlatToLeftBankedDown25:
                return TrackDiagFlatToLeftBankedDown25;
            case TrackElemType::DiagFlatToRightBankedDown25:
                return TrackDiagFlatToRightBankedDown25;
            case TrackElemType::DiagLeftBankedDown25ToFlat:
                return TrackDiagLeftBankedDown25ToFlat;
            case TrackElemType::DiagRightBankedDown25ToFlat:
                return TrackDiagRightBankedDown25ToFlat;
            case TrackElemType::LeftEighthBankToDiagUp25:
                return TrackLeftEighthBankToDiagUp25;
            case TrackElemType::RightEighthBankToDiagUp25:
                return TrackRightEighthBankToDiagUp25;
            case TrackElemType::LeftEighthBankToDiagDown25:
                return TrackLeftEighthBankToDiagDown25;
            case TrackElemType::RightEighthBankToDiagDown25:
                return TrackRightEighthBankToDiagDown25;
            case TrackElemType::LeftEighthBankToOrthogonalUp25:
                return TrackLeftEighthBankToOrthogonalUp25;
            case TrackElemType::RightEighthBankToOrthogonalUp25:
                return TrackRightEighthBankToOrthogonalUp25;
            case TrackElemType::LeftEighthBankToOrthogonalDown25:
                return TrackLeftEighthBankToOrthogonalDown25;
            case TrackElemType::RightEighthBankToOrthogonalDown25:
                return TrackRightEighthBankToOrthogonalDown25;
            case TrackElemType::Brakes:
                return TrackBrakes;
            case TrackElemType::BlockBrakes:
                return TrackBlockBrakes;
            case TrackElemType::LeftTwistUpToDown:
                return TrackLeftTwistUpToDown;
            case TrackElemType::RightTwistUpToDown:
                return TrackRightTwistUpToDown;
            case TrackElemType::LeftTwistDownToUp:
                return TrackLeftTwistDownToUp;
            case TrackElemType::RightTwistDownToUp:
                return TrackRightTwistDownToUp;
            case TrackElemType::HalfLoopUp:
                return TrackHalfLoopUp;
            case TrackElemType::HalfLoopDown:
                return TrackHalfLoopDown;
            case TrackElemType::LeftMediumHalfLoopUp:
                return TrackLeftMediumHalfLoopUp;
            case TrackElemType::RightMediumHalfLoopUp:
                return TrackRightMediumHalfLoopUp;
            case TrackElemType::LeftMediumHalfLoopDown:
                return TrackLeftMediumHalfLoopDown;
            case TrackElemType::RightMediumHalfLoopDown:
                return TrackRightMediumHalfLoopDown;
            case TrackElemType::Up90ToInvertedFlatQuarterLoop:
                return Track90DegToInvertedFlatQuarterLoopUp;
            case TrackElemType::InvertedFlatToDown90QuarterLoop:
                return TrackInvertedFlatTo90DegQuarterLoopDown;
            case TrackElemType::LeftLargeCorkscrewUp:
                return TrackLeftLargeCorkscrewUp;
            case TrackElemType::RightLargeCorkscrewUp:
                return TrackRightLargeCorkscrewUp;
            case TrackElemType::LeftLargeCorkscrewDown:
                return TrackLeftLargeCorkscrewDown;
            case TrackElemType::RightLargeCorkscrewDown:
                return TrackRightLargeCorkscrewDown;
            case TrackElemType::OnRidePhoto:
                return TrackOnRidePhoto;
            default:
                return TrackPaintFunctionDummy;
        }
    }

} // namespace OpenRCT2::PipelineRC
