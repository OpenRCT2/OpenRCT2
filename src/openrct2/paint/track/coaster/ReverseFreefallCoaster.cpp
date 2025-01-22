/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../drawing/Drawing.h"
#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.h"
#include "../../support/WoodenSupports.h"
#include "../../support/WoodenSupports.hpp"
#include "../../tile_element/Paint.TileElement.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_REVERSE_FREEFALL_RC_STATION_SW_NE = 22162,
    SPR_REVERSE_FREEFALL_RC_STATION_NW_SE = 22163,
    SPR_REVERSE_FREEFALL_RC_FLAT_SW_NE = 22164,
    SPR_REVERSE_FREEFALL_RC_FLAT_NW_SE = 22165,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SW_NE = 22166,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_NW_SE = 22167,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_NE_SW = 22168,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SE_NW = 22169,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_SW_NE = 22170,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_NW_SE = 22171,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_NE_SW = 22172,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_SE_NW = 22173,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_0 = 22174,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_1 = 22175,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_2 = 22176,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_3 = 22177,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_4 = 22178,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_6 = 22179,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_0 = 22180,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_1 = 22181,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_2 = 22182,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_3 = 22183,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_4 = 22184,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_6 = 22185,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_0 = 22186,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_1 = 22187,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_2 = 22188,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_3 = 22189,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_4 = 22190,
    SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_6 = 22191,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_0 = 22192,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_1 = 22193,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_2 = 22194,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_3 = 22195,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_4 = 22196,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_6 = 22197,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_0 = 22198,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_1 = 22199,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_2 = 22200,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_3 = 22201,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_4 = 22202,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_6 = 22203,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_5 = 22204,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_0 = 22205,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_1 = 22206,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_2 = 22207,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_3 = 22208,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_4 = 22209,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_6 = 22210,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_5 = 22211,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_0 = 22212,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_1 = 22213,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_2 = 22214,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_3 = 22215,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_4 = 22216,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_6 = 22217,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_5 = 22218,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_0 = 22219,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_1 = 22220,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_2 = 22221,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_3 = 22222,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_4 = 22223,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_6 = 22224,
    SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_5 = 22225,

    SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE = 22226,
    SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE = 22227,
};

static constexpr uint32_t kPiecesStation[4] = {
    SPR_REVERSE_FREEFALL_RC_STATION_SW_NE,
    SPR_REVERSE_FREEFALL_RC_STATION_NW_SE,
    SPR_REVERSE_FREEFALL_RC_STATION_SW_NE,
    SPR_REVERSE_FREEFALL_RC_STATION_NW_SE,
};

static constexpr uint32_t kPiecesSlope[7][4] = {
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_0,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_1,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_2,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_3,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_4,
    },
    { 0, 0, 0, 0 },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SW_NE_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NW_SE_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_NE_SW_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SE_NW_6,
    },
};

static constexpr uint32_t kPiecesSlopeSupports[7][4] = {
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_0,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_0,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_1,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_1,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_2,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_2,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_3,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_3,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_4,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_4,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_5,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_5,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_5,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_5,
    },
    {
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SW_NE_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NW_SE_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_NE_SW_6,
        SPR_REVERSE_FREEFALL_RC_SLOPE_SUPPORTS_SE_NW_6,
    },
};

static constexpr uint32_t kPiecesVertical[4] = {
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SW_NE,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_NW_SE,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_NE_SW,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SE_NW,
};

static constexpr uint32_t kPiecesVerticalSupports[4] = {
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_SW_NE,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_NW_SE,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_NE_SW,
    SPR_REVERSE_FREEFALL_RC_VERTICAL_SUPPORTS_SE_NW,
};

static void PaintReverseFreefallRCFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    if (direction & 1)
    {
        auto imageId = session.TrackColours.WithIndex(SPR_REVERSE_FREEFALL_RC_FLAT_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
        PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::Flat);
    }
    else
    {
        auto imageId = session.TrackColours.WithIndex(SPR_REVERSE_FREEFALL_RC_FLAT_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
        PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::Flat);
    }

    DrawSupportForSequenceA<TrackElemType::Flat>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintReverseFreefallRCStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction == 0 || direction == 2)
    {
        // height -= 2 (height - 2)
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_SW_NE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 0, 2, height }, { 32, 28, 1 } });
        // height += 2 (height)

        imageId = session.TrackColours.WithIndex(kPiecesStation[direction]);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
    }
    else if (direction == 1 || direction == 3)
    {
        // height -= 2 (height - 2)
        imageId = GetStationColourScheme(session, trackElement).WithIndex(SPR_STATION_BASE_B_NW_SE);
        PaintAddImageAsParent(session, imageId, { 0, 0, height - 2 }, { { 2, 0, height }, { 28, 32, 1 } });
        // height += 2 (height)

        imageId = session.TrackColours.WithIndex(kPiecesStation[direction]);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
    }
    DrawSupportForSequenceA<TrackElemType::EndStation>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    TrackPaintUtilDrawStationTunnel(session, direction, height);

    TrackPaintUtilDrawNarrowStationPlatform(session, ride, direction, height, 5, trackElement);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

static void PaintReverseFreefallRCSlope(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr int8_t bbHeights03[] = { 1, 6, 14, 37, 64 };
    static constexpr int8_t bbHeights12[] = { 1, 6, 14, 27, 59 };
    static constexpr int32_t supportHeights[] = { 48, 64, 128, 176, 208, 240, 240 };
    static constexpr int32_t tunnelOffsets03[] = { 0, 0, 0, 16, 64 };

    auto supportsImageId = session.SupportColours.WithIndex(kPiecesSlopeSupports[trackSequence][direction]);
    auto trackImageId = session.TrackColours.WithIndex(kPiecesSlope[trackSequence][direction]);
    int8_t bbHeight;
    bool isDirection03 = (direction == 0 || direction == 3);
    switch (trackSequence)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            if (isDirection03)
            {
                bbHeight = bbHeights03[trackSequence];
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, bbHeight } });
                PaintAddImageAsChildRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, bbHeight } });

                int32_t tunnelOffset = tunnelOffsets03[trackSequence];
                if (direction & 1)
                {
                    PaintUtilPushTunnelRight(session, height + tunnelOffset, kTunnelGroup, TunnelSubType::Flat);
                }
                else
                {
                    PaintUtilPushTunnelLeft(session, height + tunnelOffset, kTunnelGroup, TunnelSubType::Flat);
                }
            }
            else
            {
                bbHeight = bbHeights12[trackSequence];
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, bbHeight } });
                PaintAddImageAsChildRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, bbHeight } });
            }

            DrawSupportForSequenceA<TrackElemType::ReverseFreefallSlope>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + supportHeights[trackSequence]);
            break;
        case 5:
            if (DrawSupportForSequenceA<TrackElemType::ReverseFreefallSlope>(
                    session, supportType.wooden, trackSequence, direction, height, session.SupportColours))
            {
                ImageId floorImageId;
                if (direction & 1)
                {
                    floorImageId = session.SupportColours.WithIndex(SPR_FLOOR_PLANKS_90_DEG);
                }
                else
                {
                    floorImageId = session.SupportColours.WithIndex(SPR_FLOOR_PLANKS);
                }
                PaintAddImageAsParent(session, floorImageId, { 0, 0, height }, { { 3, 3, height }, { 26, 26, 126 } });
                PaintAddImageAsChildRotated(
                    session, direction, supportsImageId, { 0, 0, height },
                    { { isDirection03 ? 3 : 11, 3, height }, { isDirection03 ? 26 : 18, 26, 126 } });
            }
            else
            {
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height },
                    { { isDirection03 ? 3 : 11, 3, height }, { isDirection03 ? 26 : 18, 26, 126 } });
            }
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + supportHeights[trackSequence]);
            break;
        case 6:
            if (isDirection03)
            {
                PaintAddImageAsParentRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { { 0, 6, height + 128 }, { 5, 20, 79 } });
                PaintAddImageAsChildRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 0, 6, height + 128 }, { 5, 20, 79 } });
            }
            else
            {
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 27, 6, height }, { 1, 20, 126 } });
                PaintAddImageAsChildRotated(
                    session, direction, supportsImageId, { 0, 0, height }, { { 27, 6, height }, { 1, 20, 126 } });
            }
            DrawSupportForSequenceA<TrackElemType::ReverseFreefallSlope>(
                session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + supportHeights[trackSequence]);
            break;
    }
}

static void PaintReverseFreefallRCVertical(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId supportsImageId, trackImageId;
    switch (trackSequence)
    {
        case 0:
            supportsImageId = session.SupportColours.WithIndex(kPiecesVerticalSupports[direction]);
            PaintAddImageAsParent(session, supportsImageId, { 0, 0, height }, { { 3, 3, height }, { 26, 26, 79 } });
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
        case 1:
            trackImageId = session.TrackColours.WithIndex(kPiecesVertical[direction]);
            if (direction == 0 || direction == 3)
            {
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 0, 6, height }, { 2, 20, 79 } });
            }
            else
            {
                PaintAddImageAsParentRotated(
                    session, direction, trackImageId, { 0, 0, height }, { { 30, 6, height }, { 2, 20, 79 } });
            }
            PaintUtilSetVerticalTunnel(session, height + 80);
            PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
            PaintUtilSetGeneralSupportHeight(session, height + 80);
            break;
    }
}

static void PaintReverseFreefallRCOnridePhoto(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    static constexpr uint32_t imageIds[4] = {
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_SW_NE,
        SPR_AIR_POWERED_VERTICAL_RC_FLAT_NW_SE,
    };

    // The straight track without booster is borrowed from the APVC.
    // It has one track colour, instead of the two that the Reverse Freefall Colour has.
    auto colour = session.TrackColours;
    if (!trackElement.IsGhost() && !trackElement.IsHighlighted())
    {
        colour = colour.WithPrimary(colour.GetSecondary());
    }

    PaintAddImageAsParentRotated(
        session, direction, colour.WithIndex(imageIds[direction]), { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });

    DrawSupportForSequenceA<TrackElemType::OnRidePhoto>(
        session, supportType.wooden, trackSequence, direction, height, session.SupportColours);
    ;

    TrackPaintUtilOnridePhotoPaint2(session, direction, trackElement, height);
}

TrackPaintFunction GetTrackPaintFunctionReverseFreefallRC(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintReverseFreefallRCFlat;
        case TrackElemType::EndStation:
        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
            return PaintReverseFreefallRCStation;
        case TrackElemType::ReverseFreefallSlope:
            return PaintReverseFreefallRCSlope;
        case TrackElemType::ReverseFreefallVertical:
            return PaintReverseFreefallRCVertical;
        case TrackElemType::OnRidePhoto:
            return PaintReverseFreefallRCOnridePhoto;
        default:
            return TrackPaintFunctionDummy;
    }
}
