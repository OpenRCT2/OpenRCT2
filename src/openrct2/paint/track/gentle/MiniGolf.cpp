/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../interface/Viewport.h"
#include "../../../ride/RideData.h"
#include "../../../ride/TrackData.h"
#include "../../../ride/TrackPaint.h"
#include "../../../ride/Vehicle.h"
#include "../../../world/Map.h"
#include "../../../world/tile_element/Slope.h"
#include "../../../world/tile_element/SurfaceElement.h"
#include "../../../world/tile_element/TrackElement.h"
#include "../../Paint.SessionFlags.h"
#include "../../Paint.h"
#include "../../support/MetalSupports.h"
#include "../../support/WoodenSupports.h"
#include "../../tile_element/Segment.h"
#include "../../track/Segment.h"
#include "../../track/Support.h"

#include <iterator>

using namespace OpenRCT2;

static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Standard;

enum
{
    SprMiniGolfFlatSwNe = 14404,
    SprMiniGolfFlatNwSe = 14405,
    SprMiniGolfFlatFenceBackSwNe = 14406,
    SprMiniGolfFlatFenceBackNwSe = 14407,
    SprMiniGolfFlatFenceFrontSwNe = 14408,
    SprMiniGolfFlatFenceFrontNwSe = 14409,
    SprMiniGolfStationFloor = 14410,
    SprMiniGolfQuarterTurn1TileSwNw = 14411,
    SprMiniGolfQuarterTurn1TileNwNe = 14412,
    SprMiniGolfQuarterTurn1TileNeSe = 14413,
    SprMiniGolfQuarterTurn1TileSeSw = 14414,
    SprMiniGolfQuarterTurn1TileFenceBackSwNw = 14415,
    SprMiniGolfQuarterTurn1TileFenceBackNwNe = 14416,
    SprMiniGolfQuarterTurn1TileFenceBackNeSe = 14417,
    SprMiniGolfQuarterTurn1TileFenceBackSeSw = 14418,
    SprMiniGolfQuarterTurn1TileFenceFrontSwNw = 14419,
    SprMiniGolfQuarterTurn1TileFenceFrontNwNe = 14420,
    SprMiniGolfQuarterTurn1TileFenceFrontNeSe = 14421,
    SprMiniGolfQuarterTurn1TileFenceFrontSeSw = 14422,
    SprMiniGolfQuarterTurn1TileFenceInsideSwNw = 14423,
    SprMiniGolfQuarterTurn1TileFenceInsideNeSe = 14424,
    SprMiniGolfFlatTo25DegUpSwNe = 14425,
    SprMiniGolfFlatTo25DegUpNwSe = 14426,
    SprMiniGolfFlatTo25DegUpNeSw = 14427,
    SprMiniGolfFlatTo25DegUpSeNw = 14428,
    SprMiniGolf25DegUpToFlatSwNe = 14429,
    SprMiniGolf25DegUpToFlatNwSe = 14430,
    SprMiniGolf25DegUpToFlatNeSw = 14431,
    SprMiniGolf25DegUpToFlatSeNw = 14432,
    SprMiniGolf25DegUpSwNe = 14433,
    SprMiniGolf25DegUpNwSe = 14434,
    SprMiniGolf25DegUpNeSw = 14435,
    SprMiniGolf25DegUpSeNw = 14436,
    SprMiniGolfFlatTo25DegUpFenceBackSwNe = 14437,
    SprMiniGolfFlatTo25DegUpFenceBackNwSe = 14438,
    SprMiniGolfFlatTo25DegUpFenceBackNeSw = 14439,
    SprMiniGolfFlatTo25DegUpFenceBackSeNw = 14440,
    SprMiniGolf25DegUpToFlatFenceBackSwNe = 14441,
    SprMiniGolf25DegUpToFlatFenceBackNwSe = 14442,
    SprMiniGolf25DegUpToFlatFenceBackNeSw = 14443,
    SprMiniGolf25DegUpToFlatFenceBackSeNw = 14444,
    SprMiniGolfFlatTo25DegUpFenceFrontSwNe = 14445,
    SprMiniGolfFlatTo25DegUpFenceFrontNwSe = 14446,
    SprMiniGolfFlatTo25DegUpFenceFrontNeSw = 14447,
    SprMiniGolfFlatTo25DegUpFenceFrontSeNw = 14448,
    SprMiniGolf25DegUpToFlatFenceFrontSwNe = 14449,
    SprMiniGolf25DegUpToFlatFenceFrontNwSe = 14450,
    SprMiniGolf25DegUpToFlatFenceFrontNeSw = 14451,
    SprMiniGolf25DegUpToFlatFenceFrontSeNw = 14452,
    SprMiniGolf25DegUpFenceBackSwNe = 14453,
    SprMiniGolf25DegUpFenceBackNwSe = 14454,
    SprMiniGolf25DegUpFenceBackNeSw = 14455,
    SprMiniGolf25DegUpFenceBackSeNw = 14456,
    SprMiniGolf25DegUpFenceFrontSwNe = 14457,
    SprMiniGolf25DegUpFenceFrontNwSe = 14458,
    SprMiniGolf25DegUpFenceFrontNeSw = 14459,
    SprMiniGolf25DegUpFenceFrontSeNw = 14460,

    SprMiniGolfHoleABasePart1SwNe = 14461,
    SprMiniGolfHoleABasePart2SwNe = 14462,
    SprMiniGolfHoleABasePart1NwSe = 14463,
    SprMiniGolfHoleABasePart2NwSe = 14464,
    SprMiniGolfHoleABasePart1NeSw = 14465,
    SprMiniGolfHoleABasePart2NeSw = 14466,
    SprMiniGolfHoleABasePart1SeNw = 14467,
    SprMiniGolfHoleABasePart2SeNw = 14468,
    SprMiniGolfHoleATrimPart1SwNe = 14469,
    SprMiniGolfHoleATrimPart2SwNe = 14470,
    SprMiniGolfHoleATrimPart1NwSe = 14471,
    SprMiniGolfHoleATrimPart2NwSe = 14472,
    SprMiniGolfHoleATrimPart1NeSw = 14473,
    SprMiniGolfHoleATrimPart2NeSw = 14474,
    SprMiniGolfHoleATrimPart1SeNw = 14475,
    SprMiniGolfHoleATrimPart2SeNw = 14476,

    SprMiniGolfHoleBBasePart1SwNe = 14477,
    SprMiniGolfHoleBBasePart2SwNe = 14478,
    SprMiniGolfHoleBBasePart1NwSe = 14479,
    SprMiniGolfHoleBBasePart2NwSe = 14480,
    SprMiniGolfHoleBBasePart1NeSw = 14481,
    SprMiniGolfHoleBBasePart2NeSw = 14482,
    SprMiniGolfHoleBBasePart1SeNw = 14483,
    SprMiniGolfHoleBBasePart2SeNw = 14484,
    SprMiniGolfHoleBTrimPart1SwNe = 14485,
    SprMiniGolfHoleBTrimPart2SwNe = 14486,
    SprMiniGolfHoleBTrimPart1NwSe = 14487,
    SprMiniGolfHoleBTrimPart2NwSe = 14488,
    SprMiniGolfHoleBTrimPart1NeSw = 14489,
    SprMiniGolfHoleBTrimPart2NeSw = 14490,
    SprMiniGolfHoleBTrimPart1SeNw = 14491,
    SprMiniGolfHoleBTrimPart2SeNw = 14492,

    SprMiniGolfHoleCBasePart1SwNe = 14493,
    SprMiniGolfHoleCBasePart2SwNe = 14494,
    SprMiniGolfHoleCBasePart1NwSe = 14495,
    SprMiniGolfHoleCBasePart2NwSe = 14496,
    SprMiniGolfHoleCBasePart1NeSw = 14497,
    SprMiniGolfHoleCBasePart2NeSw = 14498,
    SprMiniGolfHoleCBasePart1SeNw = 14499,
    SprMiniGolfHoleCBasePart2SeNw = 14500,
    SprMiniGolfHoleCTrimPart1SwNe = 14501,
    SprMiniGolfHoleCTrimPart2SwNe = 14502,
    SprMiniGolfHoleCTrimPart1NwSe = 14503,
    SprMiniGolfHoleCTrimPart2NwSe = 14504,
    SprMiniGolfHoleCTrimPart1NeSw = 14505,
    SprMiniGolfHoleCTrimPart2NeSw = 14506,
    SprMiniGolfHoleCTrimPart1SeNw = 14507,
    SprMiniGolfHoleCTrimPart2SeNw = 14508,

    SprMiniGolfHoleDBasePart1SwSe = 14509,
    SprMiniGolfHoleDBasePart2SwSe = 14510,
    SprMiniGolfHoleDBasePart3SwSe = 14511,
    SprMiniGolfHoleDBasePart1NwSw = 14512,
    SprMiniGolfHoleDBasePart2NwSw = 14513,
    SprMiniGolfHoleDBasePart3NwSw = 14514,
    SprMiniGolfHoleDBasePart1NeNw = 14515,
    SprMiniGolfHoleDBasePart2NeNw = 14516,
    SprMiniGolfHoleDBasePart3NeNw = 14517,
    SprMiniGolfHoleDBasePart1SeNe = 14518,
    SprMiniGolfHoleDBasePart2SeNe = 14519,
    SprMiniGolfHoleDBasePart3SeNe = 14520,
    SprMiniGolfHoleDTrimPart1SwSe = 14521,
    SprMiniGolfHoleDTrimPart2SwSe = 14522,
    SprMiniGolfHoleDTrimPart3SwSe = 14523,
    SprMiniGolfHoleDTrimPart1NwSw = 14524,
    SprMiniGolfHoleDTrimPart2NwSw = 14525,
    SprMiniGolfHoleDTrimPart3NwSw = 14526,
    SprMiniGolfHoleDTrimPart1NeNw = 14527,
    SprMiniGolfHoleDTrimPart2NeNw = 14528,
    SprMiniGolfHoleDTrimPart3NeNw = 14529,
    SprMiniGolfHoleDTrimPart1SeNe = 14530,
    SprMiniGolfHoleDTrimPart2SeNe = 14531,
    SprMiniGolfHoleDTrimPart3SeNe = 14532,

    SprMiniGolfHoleEBasePart1SeSw = 14533,
    SprMiniGolfHoleEBasePart2SeSw = 14534,
    SprMiniGolfHoleEBasePart3SeSw = 14535,
    SprMiniGolfHoleEBasePart1NeSe = 14536,
    SprMiniGolfHoleEBasePart2NeSe = 14537,
    SprMiniGolfHoleEBasePart3NeSe = 14538,
    SprMiniGolfHoleEBasePart1NwNe = 14539,
    SprMiniGolfHoleEBasePart2NwNe = 14540,
    SprMiniGolfHoleEBasePart3NwNe = 14541,
    SprMiniGolfHoleEBasePart1SwNw = 14542,
    SprMiniGolfHoleEBasePart2SwNw = 14543,
    SprMiniGolfHoleEBasePart3SwNw = 14544,
    SprMiniGolfHoleETrimPart1SeSw = 14545,
    SprMiniGolfHoleETrimPart2SeSw = 14546,
    SprMiniGolfHoleETrimPart3SeSw = 14547,
    SprMiniGolfHoleETrimPart1NeSe = 14548,
    SprMiniGolfHoleETrimPart2NeSe = 14549,
    SprMiniGolfHoleETrimPart3NeSe = 14550,
    SprMiniGolfHoleETrimPart1NwNe = 14551,
    SprMiniGolfHoleETrimPart2NwNe = 14552,
    SprMiniGolfHoleETrimPart3NwNe = 14553,
    SprMiniGolfHoleETrimPart1SwNw = 14554,
    SprMiniGolfHoleETrimPart2SwNw = 14555,
    SprMiniGolfHoleETrimPart3SwNw = 14556,
};

static constexpr uint32_t kMiniGolfTrackSprites25DegUp[][3] = {
    {
        SprMiniGolf25DegUpSwNe,
        SprMiniGolf25DegUpFenceBackSwNe,
        SprMiniGolf25DegUpFenceFrontSwNe,
    },
    {
        SprMiniGolf25DegUpNwSe,
        SprMiniGolf25DegUpFenceBackNwSe,
        SprMiniGolf25DegUpFenceFrontNwSe,
    },
    {
        SprMiniGolf25DegUpNeSw,
        SprMiniGolf25DegUpFenceBackNeSw,
        SprMiniGolf25DegUpFenceFrontNeSw,
    },
    {
        SprMiniGolf25DegUpSeNw,
        SprMiniGolf25DegUpFenceBackSeNw,
        SprMiniGolf25DegUpFenceFrontSeNw,
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesFlatTo25DegUp[][3] = {
    {
        SprMiniGolfFlatTo25DegUpSwNe,
        SprMiniGolfFlatTo25DegUpFenceBackSwNe,
        SprMiniGolfFlatTo25DegUpFenceFrontSwNe,
    },
    {
        SprMiniGolfFlatTo25DegUpNwSe,
        SprMiniGolfFlatTo25DegUpFenceBackNwSe,
        SprMiniGolfFlatTo25DegUpFenceFrontNwSe,
    },
    {
        SprMiniGolfFlatTo25DegUpNeSw,
        SprMiniGolfFlatTo25DegUpFenceBackNeSw,
        SprMiniGolfFlatTo25DegUpFenceFrontNeSw,
    },
    {
        SprMiniGolfFlatTo25DegUpSeNw,
        SprMiniGolfFlatTo25DegUpFenceBackSeNw,
        SprMiniGolfFlatTo25DegUpFenceFrontSeNw,
    },
};

static constexpr uint32_t kMiniGolfTrackSprites25DegUpToFlat[][3] = {
    {
        SprMiniGolf25DegUpToFlatSwNe,
        SprMiniGolf25DegUpToFlatFenceBackSwNe,
        SprMiniGolf25DegUpToFlatFenceFrontSwNe,
    },
    {
        SprMiniGolf25DegUpToFlatNwSe,
        SprMiniGolf25DegUpToFlatFenceBackNwSe,
        SprMiniGolf25DegUpToFlatFenceFrontNwSe,
    },
    {
        SprMiniGolf25DegUpToFlatNeSw,
        SprMiniGolf25DegUpToFlatFenceBackNeSw,
        SprMiniGolf25DegUpToFlatFenceFrontNeSw,
    },
    {
        SprMiniGolf25DegUpToFlatSeNw,
        SprMiniGolf25DegUpToFlatFenceBackSeNw,
        SprMiniGolf25DegUpToFlatFenceFrontSeNw,
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesQuarterTurn1Tile[] = {
    SprMiniGolfQuarterTurn1TileSwNw,
    SprMiniGolfQuarterTurn1TileNwNe,
    SprMiniGolfQuarterTurn1TileNeSe,
    SprMiniGolfQuarterTurn1TileSeSw,
};

static constexpr uint32_t kMiniGolfTrackSpritesQuarterTurn1TileFenceFront[] = {
    SprMiniGolfQuarterTurn1TileFenceFrontSwNw,
    SprMiniGolfQuarterTurn1TileFenceFrontNwNe,
    SprMiniGolfQuarterTurn1TileFenceFrontNeSe,
    SprMiniGolfQuarterTurn1TileFenceFrontSeSw,
};

static constexpr uint32_t kMiniGolfTrackSpritesHoleA[4][2][2] = {
    {
        { SprMiniGolfHoleABasePart1SwNe, SprMiniGolfHoleATrimPart1SwNe },
        { SprMiniGolfHoleABasePart2SwNe, SprMiniGolfHoleATrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleABasePart1NwSe, SprMiniGolfHoleATrimPart1NwSe },
        { SprMiniGolfHoleABasePart2NwSe, SprMiniGolfHoleATrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleABasePart1NeSw, SprMiniGolfHoleATrimPart1NeSw },
        { SprMiniGolfHoleABasePart2NeSw, SprMiniGolfHoleATrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleABasePart1SeNw, SprMiniGolfHoleATrimPart1SeNw },
        { SprMiniGolfHoleABasePart2SeNw, SprMiniGolfHoleATrimPart2SeNw },
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesHoleB[4][2][2] = {
    {
        { SprMiniGolfHoleBBasePart1SwNe, SprMiniGolfHoleBTrimPart1SwNe },
        { SprMiniGolfHoleBBasePart2SwNe, SprMiniGolfHoleBTrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleBBasePart1NwSe, SprMiniGolfHoleBTrimPart1NwSe },
        { SprMiniGolfHoleBBasePart2NwSe, SprMiniGolfHoleBTrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleBBasePart1NeSw, SprMiniGolfHoleBTrimPart1NeSw },
        { SprMiniGolfHoleBBasePart2NeSw, SprMiniGolfHoleBTrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleBBasePart1SeNw, SprMiniGolfHoleBTrimPart1SeNw },
        { SprMiniGolfHoleBBasePart2SeNw, SprMiniGolfHoleBTrimPart2SeNw },
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesHoleC[][2][2] = {
    {
        { SprMiniGolfHoleCBasePart1SwNe, SprMiniGolfHoleCTrimPart1SwNe },
        { SprMiniGolfHoleCBasePart2SwNe, SprMiniGolfHoleCTrimPart2SwNe },
    },
    {
        { SprMiniGolfHoleCBasePart1NwSe, SprMiniGolfHoleCTrimPart1NwSe },
        { SprMiniGolfHoleCBasePart2NwSe, SprMiniGolfHoleCTrimPart2NwSe },
    },
    {
        { SprMiniGolfHoleCBasePart1NeSw, SprMiniGolfHoleCTrimPart1NeSw },
        { SprMiniGolfHoleCBasePart2NeSw, SprMiniGolfHoleCTrimPart2NeSw },
    },
    {
        { SprMiniGolfHoleCBasePart1SeNw, SprMiniGolfHoleCTrimPart1SeNw },
        { SprMiniGolfHoleCBasePart2SeNw, SprMiniGolfHoleCTrimPart2SeNw },
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesHoleD[][3][2] = {
    {
        { SprMiniGolfHoleDBasePart1SwSe, SprMiniGolfHoleDTrimPart1SwSe },
        { SprMiniGolfHoleDBasePart2SwSe, SprMiniGolfHoleDTrimPart2SwSe },
        { SprMiniGolfHoleDBasePart3SwSe, SprMiniGolfHoleDTrimPart3SwSe },
    },
    {
        { SprMiniGolfHoleDBasePart1NwSw, SprMiniGolfHoleDTrimPart1NwSw },
        { SprMiniGolfHoleDBasePart2NwSw, SprMiniGolfHoleDTrimPart2NwSw },
        { SprMiniGolfHoleDBasePart3NwSw, SprMiniGolfHoleDTrimPart3NwSw },
    },
    {
        { SprMiniGolfHoleDBasePart1NeNw, SprMiniGolfHoleDTrimPart1NeNw },
        { SprMiniGolfHoleDBasePart2NeNw, SprMiniGolfHoleDTrimPart2NeNw },
        { SprMiniGolfHoleDBasePart3NeNw, SprMiniGolfHoleDTrimPart3NeNw },
    },
    {
        { SprMiniGolfHoleDBasePart1SeNe, SprMiniGolfHoleDTrimPart1SeNe },
        { SprMiniGolfHoleDBasePart2SeNe, SprMiniGolfHoleDTrimPart2SeNe },
        { SprMiniGolfHoleDBasePart3SeNe, SprMiniGolfHoleDTrimPart3SeNe },
    },
};

static constexpr uint32_t kMiniGolfTrackSpritesHoleE[][3][2] = {
    {
        { SprMiniGolfHoleEBasePart1SwNw, SprMiniGolfHoleETrimPart1SwNw },
        { SprMiniGolfHoleEBasePart2SwNw, SprMiniGolfHoleETrimPart2SwNw },
        { SprMiniGolfHoleEBasePart3SwNw, SprMiniGolfHoleETrimPart3SwNw },
    },
    {
        { SprMiniGolfHoleEBasePart1NwNe, SprMiniGolfHoleETrimPart1NwNe },
        { SprMiniGolfHoleEBasePart2NwNe, SprMiniGolfHoleETrimPart2NwNe },
        { SprMiniGolfHoleEBasePart3NwNe, SprMiniGolfHoleETrimPart3NwNe },
    },
    {
        { SprMiniGolfHoleEBasePart1NeSe, SprMiniGolfHoleETrimPart1NeSe },
        { SprMiniGolfHoleEBasePart2NeSe, SprMiniGolfHoleETrimPart2NeSe },
        { SprMiniGolfHoleEBasePart3NeSe, SprMiniGolfHoleETrimPart3NeSe },
    },
    {
        { SprMiniGolfHoleEBasePart1SeSw, SprMiniGolfHoleETrimPart1SeSw },
        { SprMiniGolfHoleEBasePart2SeSw, SprMiniGolfHoleETrimPart2SeSw },
        { SprMiniGolfHoleEBasePart3SeSw, SprMiniGolfHoleETrimPart3SeSw },
    },
};

static bool MiniGolfPaintUtilShouldDrawFence(PaintSession& session, const TrackElement& trackElement)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        // Should be above ground (have passed surface rendering)
        return false;
    }

    const SurfaceElement* surfaceElement = session.Surface;
    if (surfaceElement->BaseHeight != trackElement.BaseHeight)
    {
        return true;
    }

    if (surfaceElement->GetSlope() != kTileSlopeFlat)
    {
        return true;
    }

    return false;
}

/** rct2: 0x0087F10C */
static void PaintMiniGolfTrackFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    if (direction & 1)
    {
        imageId = session.TrackColours.WithIndex(SprMiniGolfFlatNwSe);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 6, 0, height }, { 20, 32, 1 } });
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(SprMiniGolfFlatSwNe);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);

    if (MiniGolfPaintUtilShouldDrawFence(session, trackElement))
    {
        if (direction & 1)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceBackNwSe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 10, 0, height + 2 }, { 1, 32, 7 } });

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceFrontNwSe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 22, 0, height + 2 }, { 1, 32, 7 } });
        }
        else
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceBackSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 10, height + 2 }, { 32, 1, 7 } });

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceFrontSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 22, height + 2 }, { 32, 1, 7 } });
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F11C */
static void PaintMiniGolfTrack25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSprites25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { 32, 20, 1 });

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSprites25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 10, height + 2 }, { 32, 1, 15 } });

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSprites25DegUp[direction][2]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 22, height + 2 }, { 32, 1, 15 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::SlopeStart);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 56);
}

/** rct2: 0x0087F12C */
static void PaintMiniGolfTrackFlatTo25DegUp(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesFlatTo25DegUp[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSpritesFlatTo25DegUp[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 10, height + 2 }, { 32, 1, 11 } });

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSpritesFlatTo25DegUp[direction][2]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 22, height + 2 }, { 32, 1, 11 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height, kTunnelGroup, TunnelSubType::SlopeEnd);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 48);
}

/** rct2: 0x0087F13C */
static void PaintMiniGolfTrack25DegUpToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSprites25DegUpToFlat[direction][0]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 6, height }, { 32, 20, 1 } });

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 8, height, session.SupportColours);
    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(PaintSegment::bottomLeftSide, PaintSegment::centre, PaintSegment::topRightSide), direction),
        0xFFFF, 0);

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSprites25DegUpToFlat[direction][1]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 10, height + 2 }, { 32, 1, 11 } });

    imageId = GetStationColourScheme(session, trackElement).WithIndex(kMiniGolfTrackSprites25DegUpToFlat[direction][2]);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 22, height + 2 }, { 32, 1, 11 } });

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
        case 1:
            PaintUtilPushTunnelRight(session, height + 8, TunnelType::PathAndMiniGolf);
            break;
        case 2:
            PaintUtilPushTunnelLeft(session, height + 8, TunnelType::PathAndMiniGolf);
            break;
        case 3:
            PaintUtilPushTunnelRight(session, height - 8, kTunnelGroup, TunnelSubType::Flat);
            break;
    }

    PaintUtilSetGeneralSupportHeight(session, height + 40);
}

/** rct2: 0x0087F14C */
static void PaintMiniGolfTrack25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrack25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F15C */
static void PaintMiniGolfTrackFlatTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrack25DegUpToFlat(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F16C */
static void PaintMiniGolfTrack25DegDownToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrackFlatTo25DegUp(session, ride, trackSequence, (direction + 2) % 4, height, trackElement, supportType);
}

/** rct2: 0x0087F17C, 0x0087F18C, 0x0087F19C */
static void PaintMiniGolfStation(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    const auto* stationObj = ride.GetStationObject();
    ImageId imageId;
    bool hasFence;

    imageId = session.TrackColours.WithIndex(SprMiniGolfStationFloor);
    PaintAddImageAsParentRotated(session, direction, imageId, { 0, 0, height }, { { 0, 0, height }, { 32, 28, 1 } });

    if (direction & 1)
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceBackNwSe);
            PaintAddImageAsParent(session, imageId, { -10, 0, height }, { { 0, 0, height + 2 }, { 1, 32, 7 } });
        }

        bool hasSWFence = TrackPaintUtilHasFence(EDGE_SW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasSWFence)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceFrontNwSe);
            PaintAddImageAsParent(session, imageId, { 10, 0, height }, { { 31, 0, height + 2 }, { 1, 32, 7 } });
        }

        auto stationColour = GetStationColourScheme(session, trackElement);
        TrackPaintUtilDrawStationCovers(session, EDGE_NE, hasFence, stationObj, height, stationColour);
        TrackPaintUtilDrawStationCovers(session, EDGE_SW, hasSWFence, stationObj, height, stationColour);
    }
    else
    {
        hasFence = TrackPaintUtilHasFence(EDGE_NW, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasFence)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceBackSwNe);
            PaintAddImageAsParent(session, imageId, { 0, -10, height }, { { 0, 0, height + 2 }, { 32, 1, 7 } });
        }

        bool hasSEFence = TrackPaintUtilHasFence(EDGE_SE, session.MapPosition, trackElement, ride, session.CurrentRotation);
        if (hasSEFence)
        {
            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfFlatFenceFrontSwNe);
            PaintAddImageAsParent(session, imageId, { 0, 10, height }, { { 0, 31, height + 2 }, { 32, 1, 7 } });
        }

        auto stationColour = GetStationColourScheme(session, trackElement);
        TrackPaintUtilDrawStationCovers(session, EDGE_NW, hasFence, stationObj, height, stationColour);
        TrackPaintUtilDrawStationCovers(session, EDGE_SE, hasSEFence, stationObj, height, stationColour);
    }
    TrackPaintUtilDrawStationTunnel(session, direction, height);

    WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F1AC */
static void PaintMiniGolfTrackLeftQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;

    TrackPaintUtilLeftQuarterTurn1TilePaint(
        session, 1, height, 0, direction, session.TrackColours, kMiniGolfTrackSpritesQuarterTurn1Tile);

    MetalASupportsPaintSetup(session, supportType.metal, MetalSupportPlace::Centre, 0, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(
        session,
        PaintUtilRotateSegments(
            EnumsToFlags(
                PaintSegment::leftCorner, PaintSegment::topLeftSide, PaintSegment::centre, PaintSegment::bottomLeftSide),
            direction),
        0xFFFF, 0);

    const bool shouldDrawFence = MiniGolfPaintUtilShouldDrawFence(session, trackElement);

    switch (direction)
    {
        case 0:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            if (!shouldDrawFence)
                break;

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceBackSwNw);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 6, 2, height }, { 26, 24, 1 } });

            break;

        case 1:
            if (!shouldDrawFence)
                break;

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceBackNwNe);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 0, 0, height }, { 26, 26, 1 } });
            break;

        case 2:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            if (!shouldDrawFence)
                break;

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceBackNeSe);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 2, 6, height }, { 24, 26, 1 } });
            break;

        case 3:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            if (!shouldDrawFence)
                break;

            imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceBackSeSw);
            PaintAddImageAsChild(session, imageId, { 0, 0, height }, { { 6, 6, height }, { 24, 24, 1 } });
            break;
    }

    if (shouldDrawFence)
    {
        // TODO: The back fence uses the same x/y offsets, but uses another paint function. See if this occurs more often.
        TrackPaintUtilLeftQuarterTurn1TilePaint(
            session, 0, height, 24, direction, GetStationColourScheme(session, trackElement),
            kMiniGolfTrackSpritesQuarterTurn1TileFenceFront);

        switch (direction)
        {
            case 0:
                imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceInsideSwNw);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 24, 0, height + 2 }, { 5, 5, 5 } });
                break;
            case 2:
                imageId = GetStationColourScheme(session, trackElement).WithIndex(SprMiniGolfQuarterTurn1TileFenceInsideNeSe);
                PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 0, 24, height + 2 }, { 5, 5, 5 } });
                break;
        }
    }

    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);
}

/** rct2: 0x0087F1BC */
static void PaintMiniGolfTrackRightQuarterTurn1Tile(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfTrackLeftQuarterTurn1Tile(
        session, ride, trackSequence, (direction + 3) % 4, height, trackElement, supportType);
}

static void PaintMiniGolfHoleAb(
    PaintSession& session, uint8_t trackSequence, uint8_t direction, int32_t height, const uint32_t sprites[4][2][2])
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    auto bb = BoundBoxXYZ{ { boundBoxOffset, height + 24 }, { boundBox, 0 } };

    imageId = session.TrackColours.WithIndex(sprites[direction][trackSequence][1]);
    PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

    bb.offset.z = height;
    bb.length.z = 1;

    if (drewSupports)
    {
        imageId = session.SupportColours.WithIndex(((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex(sprites[direction][trackSequence][0]);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(sprites[direction][trackSequence][0]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }
}

/** rct2: 0x0087F1CC */
static void PaintMiniGolfHoleA(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfHoleAb(session, trackSequence, direction, height, kMiniGolfTrackSpritesHoleA);
}

/** rct2: 0x0087F1DC */
static void PaintMiniGolfHoleB(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    PaintMiniGolfHoleAb(session, trackSequence, direction, height, kMiniGolfTrackSpritesHoleB);
}

/** rct2: 0x0087F1EC */
static void PaintMiniGolfHoleC(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    bool drewSupports = WoodenASupportsPaintSetupRotated(
        session, WoodenSupportType::Truss, WoodenSupportSubType::NeSw, direction, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    if ((direction == 0 && trackSequence == 0) || (direction == 2 && trackSequence == 1))
    {
        PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
    }
    else if ((direction == 3 && trackSequence == 0) || (direction == 1 && trackSequence == 1))
    {
        PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
    }

    if (direction & 1)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleC[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x20:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 26, 3 } });
            break;
        case 0x10:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 3, 30, height + 4 }, { 26, 2, 3 } });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height },
                { { boundBoxOffset.x, boundBoxOffset.y, height + 24 }, { boundBox.x, boundBox.y, 0 } });
            break;
    }

    auto bb = BoundBoxXYZ{ { boundBoxOffset, height }, { boundBox.x, boundBox.y, 1 } };
    if (drewSupports)
    {
        imageId = session.SupportColours.WithIndex(((direction & 1) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);

        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleC[direction][trackSequence][0]);
        PaintAddImageAsChild(session, imageId, { 0, 0, height }, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleC[direction][trackSequence][0]);
        PaintAddImageAsParent(session, imageId, { 0, 0, height }, bb);
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleD(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    auto supportSubType = (direction & 1) ? WoodenSupportSubType::NwSe : WoodenSupportSubType::NeSw;
    if (trackSequence == 2)
        supportSubType = (direction & 1) ? WoodenSupportSubType::NeSw : WoodenSupportSubType::NwSe;
    bool drewSupports = WoodenASupportsPaintSetup(
        session, WoodenSupportType::Truss, supportSubType, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    switch ((direction << 4) | trackSequence)
    {
        case 0x00:
        case 0x12:
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;

        case 0x02:
        case 0x30:
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }

    if (supportSubType == WoodenSupportSubType::NeSw)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleD[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 26, 3 } });
            break;
        case 0x02:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 3, 30, height + 4 }, { 23, 2, 3 } });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 24, 3 } });
            break;
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 3, 30, height + 4 }, { 26, 2, 3 } });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height },
                { { boundBoxOffset.x, boundBoxOffset.y, height + 24 }, { boundBox.x, boundBox.y, 0 } });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x02:
            boundBox = { 23, 32 };
            break;
        case 0x10:
            boundBox = { 24, 32 };
            break;
    }

    auto bb = BoundBoxXYZ{ { boundBoxOffset, height }, { boundBox.x, boundBox.y, 1 } };
    auto offset = CoordsXYZ{ 0, 0, height };
    if (drewSupports)
    {
        imageId = session.SupportColours.WithIndex(
            ((supportSubType == WoodenSupportSubType::NeSw) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(session, imageId, offset, bb);

        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleD[direction][trackSequence][0]);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleD[direction][trackSequence][0]);
        PaintAddImageAsParent(session, imageId, offset, bb);
    }
}

/** rct2: 0x0087F1FC */
static void PaintMiniGolfHoleE(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    ImageId imageId;
    CoordsXY boundBox, boundBoxOffset;

    auto supportSubType = (direction & 1) ? WoodenSupportSubType::NwSe : WoodenSupportSubType::NeSw;
    if (trackSequence == 2)
        supportSubType = (direction & 1) ? WoodenSupportSubType::NeSw : WoodenSupportSubType::NwSe;
    bool drewSupports = WoodenASupportsPaintSetup(
        session, WoodenSupportType::Truss, supportSubType, height, session.SupportColours);

    PaintUtilSetSegmentSupportHeight(session, kSegmentsAll, 0xFFFF, 0);
    PaintUtilSetGeneralSupportHeight(session, height + kDefaultGeneralSupportHeight);

    switch ((direction << 4) | trackSequence)
    {
        case (0 << 4 | 0):
        case (3 << 4 | 2):
            PaintUtilPushTunnelLeft(session, height, TunnelType::PathAndMiniGolf);
            break;

        case (2 << 4 | 2):
        case (3 << 4 | 0):
            PaintUtilPushTunnelRight(session, height, TunnelType::PathAndMiniGolf);
            break;
    }

    if (supportSubType == WoodenSupportSubType::NwSe)
    {
        boundBox = { 26, 32 };
        boundBoxOffset = { 3, 0 };
    }
    else
    {
        boundBox = { 32, 26 };
        boundBoxOffset = { 0, 3 };
    }

    imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleE[direction][trackSequence][1]);

    switch ((direction << 4) | trackSequence)
    {
        case 0x01:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 26, 3 } });
            break;
        case 0x02:
        case 0x20:
        case 0x31:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 3, 30, height + 4 }, { 26, 2, 3 } });
            break;
        case 0x10:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 24, 3 } });
            break;
        case 0x32:
            PaintAddImageAsParent(session, imageId, { 0, 0, height }, { { 30, 3, height + 4 }, { 2, 23, 3 } });
            break;
        default:
            PaintAddImageAsParent(
                session, imageId, { 0, 0, height },
                { { boundBoxOffset.x, boundBoxOffset.y, height + 24 }, { boundBox.x, boundBox.y, 0 } });
            break;
    }

    switch ((direction << 4) | trackSequence)
    {
        case 0x10:
            boundBox = { 24, 32 };
            break;
        case 0x32:
            boundBox = { 32, 23 };
            break;
    }

    auto bb = BoundBoxXYZ{ { boundBoxOffset, height }, { boundBox.x, boundBox.y, 1 } };
    auto offset = CoordsXYZ{ 0, 0, height };
    if (drewSupports)
    {
        imageId = session.SupportColours.WithIndex(
            ((supportSubType == WoodenSupportSubType::NwSe) ? SPR_FLOOR_PLANKS_90_DEG : SPR_FLOOR_PLANKS));
        PaintAddImageAsParent(session, imageId, offset, bb);

        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleE[direction][trackSequence][0]);
        PaintAddImageAsChild(session, imageId, offset, bb);
    }
    else
    {
        imageId = session.TrackColours.WithIndex(kMiniGolfTrackSpritesHoleE[direction][trackSequence][0]);
        PaintAddImageAsParent(session, imageId, offset, bb);
    }
}

/**
 * rct2: 0x0087EDC4
 */
TrackPaintFunction GetTrackPaintFunctionMiniGolf(OpenRCT2::TrackElemType trackType)
{
    switch (trackType)
    {
        case TrackElemType::Flat:
            return PaintMiniGolfTrackFlat;

        case TrackElemType::BeginStation:
        case TrackElemType::MiddleStation:
        case TrackElemType::EndStation:
            return PaintMiniGolfStation;

        case TrackElemType::Up25:
            return PaintMiniGolfTrack25DegUp;
        case TrackElemType::FlatToUp25:
            return PaintMiniGolfTrackFlatTo25DegUp;
        case TrackElemType::Up25ToFlat:
            return PaintMiniGolfTrack25DegUpToFlat;

        case TrackElemType::Down25:
            return PaintMiniGolfTrack25DegDown;
        case TrackElemType::FlatToDown25:
            return PaintMiniGolfTrackFlatTo25DegDown;
        case TrackElemType::Down25ToFlat:
            return PaintMiniGolfTrack25DegDownToFlat;

        case TrackElemType::LeftQuarterTurn1Tile:
            return PaintMiniGolfTrackLeftQuarterTurn1Tile;
        case TrackElemType::RightQuarterTurn1Tile:
            return PaintMiniGolfTrackRightQuarterTurn1Tile;

        case TrackElemType::MinigolfHoleA:
            return PaintMiniGolfHoleA;
        case TrackElemType::MinigolfHoleB:
            return PaintMiniGolfHoleB;
        case TrackElemType::MinigolfHoleC:
            return PaintMiniGolfHoleC;
        case TrackElemType::MinigolfHoleD:
            return PaintMiniGolfHoleD;
        case TrackElemType::MinigolfHoleE:
            return PaintMiniGolfHoleE;
        default:
            return TrackPaintFunctionDummy;
    }
}
