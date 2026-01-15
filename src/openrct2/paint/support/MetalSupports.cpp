/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MetalSupports.h"

#include "../../core/EnumUtils.hpp"
#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../world/Footpath.h"
#include "../../world/tile_element/Slope.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Numerics;

// There are 13 types of metal support graphics, including rotated versions. A graphic showing all of them is available here:
// https://cloud.githubusercontent.com/assets/737603/19420485/7eaba28e-93ec-11e6-83cb-03190accc094.png
enum class MetalSupportGraphic : uint8_t
{
    /**
     * Used by the Steel Twister, Looping RC, and other rides.
     */
    tubes = 0,
    /**
     * Used by the Junior RC and other rides.
     */
    fork = 1,
    /**
     * Rotated version of `Fork`.
     */
    forkAlt = 2,
    /**
     * Used by the vertical roller coasters, the Log Flume and other rides.
     */
    boxed = 3,
    /**
     * Used by the Steeplechase.
     */
    stick = 4,
    /**
     * No visible difference from `Stick`, also used by the Steeplechase
     */
    stickAlt = 5,
    /**
     * Every “Thick” type seems to be used for the Looping Roller Coaster’s loop, and only for that specific part.
     */
    thickCentred = 6,
    thick = 7,
    thickAlt = 8,
    thickAltCentred = 9,
    /**
     * Used by the chairlift.
     */
    truss = 10,
    /**
     * Used by inverted rcs like the flying, lay-down, compact inverted.
     * Mostly the same as `Tubes`, but with a thinner crossbeam.
     */
    tubesInverted = 11,
    /**
     * Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
     */
    boxedCoated,
};

/** rct2: 0x0097AF20, 0x0097AF21 */
// clang-format off
static constexpr CoordsXY kMetalSupportBoundBoxOffsets[] = {
    {  4,  4 },
    { 28,  4 },
    {  4, 28 },
    { 28, 28 },
    { 16, 16 },
    { 16,  4 },
    {  4, 16 },
    { 28, 16 },
    { 16, 28 },
};

struct RepositionPair
{
    MetalSupportPlace place = MetalSupportPlace::centre;
    uint8_t crossBeamIndex = 0xFF;
    
    constexpr bool isNull() const
    {
        return crossBeamIndex == 0xFF;
    }
};
struct RepositionRow
{
    std::array<RepositionPair, kNumOrthogonalDirections> pairs;
    
    constexpr RepositionRow(const RepositionPair pair)
    {
        pairs = { pair, pair, pair, pair };
    }
    
    constexpr RepositionRow(const RepositionPair pair1, const RepositionPair pair2, const RepositionPair pair3, const RepositionPair pair4)
    {
        pairs = { pair1, pair2, pair3, pair4 };
    }
    
    constexpr RepositionPair operator[](const Direction direction) const noexcept
    {
        return pairs[direction];
    }
};

constexpr uint8_t kNumMetalSupportPlaces = 9;
using RepositionAttempt = std::array<RepositionRow, kNumMetalSupportPlaces>;

static constexpr RepositionAttempt kMetalSupportSegmentOffsets0 = {
    {
         { { MetalSupportPlace::topLeftSide, 2 } },     /* topCorner          */
         { { MetalSupportPlace::bottomLeftSide, 1 } },  /* leftCorner         */
         { { MetalSupportPlace::topRightSide, 3 } },    /* rightCorner        */
         { { MetalSupportPlace::bottomRightSide, 0 } }, /* bottomCorner       */
         {                                               
             { MetalSupportPlace::topLeftSide, 3 },            /* centre, rotation 0 */
             { MetalSupportPlace::topRightSide, 0 },           /* centre, rotation 1 */
             { MetalSupportPlace::bottomRightSide, 1 },        /* centre, rotation 2 */
             { MetalSupportPlace::bottomLeftSide, 2 }          /* centre, rotation 3 */
         },                                             
         { { MetalSupportPlace::leftCorner, 2 } },      /* topLeftSide        */
         { { MetalSupportPlace::topCorner, 3 } },       /* topRightSide       */
         { { MetalSupportPlace::bottomCorner, 1 } },    /* bottomLeftSide     */
         { { MetalSupportPlace::rightCorner, 0 } },     /* bottomRightSide    */
    }
};

static constexpr RepositionAttempt kMetalSupportSegmentOffsets1 = {
{
         { { MetalSupportPlace::topRightSide, 1 } },    /* topCorner          */
         { { MetalSupportPlace::topLeftSide, 0 } },     /* leftCorner         */
         { { MetalSupportPlace::bottomRightSide, 2 } }, /* rightCorner        */
         { { MetalSupportPlace::bottomLeftSide, 3 } },  /* bottomCorner       */
         { 
             { MetalSupportPlace::topRightSide, 0 },           /* centre, rotation 0 */
             { MetalSupportPlace::bottomRightSide, 1},         /* centre, rotation 1 */
             { MetalSupportPlace::bottomLeftSide, 2},          /* centre, rotation 2 */
             { MetalSupportPlace::topLeftSide, 3 }             /* centre, rotation 3 */
         },
         { { MetalSupportPlace::topCorner, 0 } },       /* topLeftSide        */
         { { MetalSupportPlace::rightCorner, 1 } },     /* topRightSide       */
         { { MetalSupportPlace::leftCorner, 3 } },      /* bottomLeftSide     */
         { { MetalSupportPlace::bottomCorner, 2 } },    /* bottomRightSide    */
    }
};
static constexpr RepositionAttempt kMetalSupportSegmentOffsets2 = {
    {
         { { MetalSupportPlace::leftCorner, 6 } },      /* topCorner          */ 
         { { MetalSupportPlace::bottomCorner, 5 } },    /* leftCorner         */
         { { MetalSupportPlace::topCorner, 7 } },       /* rightCorner        */
         { { MetalSupportPlace::rightCorner, 4 } },     /* bottomCorner       */
         {                                               
             { MetalSupportPlace::bottomRightSide, 1 },        /* centre, rotation 0 */ 
             { MetalSupportPlace::bottomLeftSide, 2 },         /* centre, rotation 1 */
             { MetalSupportPlace::topLeftSide, 3 },            /* centre, rotation 2 */
             { MetalSupportPlace::topRightSide, 0 }            /* centre, rotation 3 */
         },                                             
         { { MetalSupportPlace::centre, 1 } },          /* topLeftSide        */
         { { MetalSupportPlace::centre, 2 } },          /* topRightSide       */
         { { MetalSupportPlace::centre, 0 } },          /* bottomLeftSide     */
         { { MetalSupportPlace::centre, 3 } },          /* bottomRightSide    */
    }
};
static constexpr RepositionAttempt kMetalSupportSegmentOffsets3 = {
    {
         { { MetalSupportPlace::rightCorner, 5 } },     /* topCorner          */
         { { MetalSupportPlace::topCorner, 4 } },       /* leftCorner         */
         { { MetalSupportPlace::bottomCorner, 6 } },    /* rightCorner        */
         { { MetalSupportPlace::leftCorner, 7 } },      /* bottomCorner       */
         {                                               
             { MetalSupportPlace::bottomLeftSide, 2 },         /* centre, rotation 0 */
             { MetalSupportPlace::topLeftSide, 3 },            /* centre, rotation 1 */
             { MetalSupportPlace::topRightSide, 0 },           /* centre, rotation 2 */
             { MetalSupportPlace::bottomRightSide, 1 }         /* centre, rotation 3 */
         },                                             
         { { MetalSupportPlace::bottomRightSide, 5 } }, /* topLeftSide        */
         { { MetalSupportPlace::bottomLeftSide, 6 } },  /* topRightSide       */
         { { MetalSupportPlace::topRightSide, 4 } },    /* bottomLeftSide     */
         { { MetalSupportPlace::topLeftSide, 7 } },     /* bottomRightSide    */
    }
};

/** rct2: 0x0097AF32 */
static constexpr std::array<RepositionAttempt, 4> kMetalSupportSegmentOffsets = {
    kMetalSupportSegmentOffsets0,
    kMetalSupportSegmentOffsets1,
    kMetalSupportSegmentOffsets2,
    kMetalSupportSegmentOffsets3,
};

/** rct2: 0x0097B052, 0x0097B053 */
static constexpr CoordsXY kMetalSupportCrossBeamBoundBoxOffsets[] = {
    { -15,  -1 },
    {   0,  -2 },
    {  -2,  -1 },
    {  -1, -15 },
    { -26,  -1 },
    {   0,  -2 },
    {  -2,  -1 },
    {  -1, -26 },
};

/** rct2: 0x0097B062, 0x0097B063 */
static constexpr CoordsXY kMetalSupportCrossBeamBoundBoxLengths[] = {
    { 18,  3 },
    {  3, 18 },
    { 18,  3 },
    {  3, 18 },
    { 32,  3 },
    {  3, 32 },
    { 32,  3 },
    {  3, 32 }
};

/** rct2: 0x0097B072 */
static constexpr uint32_t kMetalSupportTypeToCrossbeamImages[][8] = {
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::tubes
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::fork
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::forkAlt
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::boxed
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::stick
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::stickAlt
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportGraphic::thickCentred
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportGraphic::thick
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportGraphic::thickAlt
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportGraphic::thickAltCentred
    { 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 }, // MetalSupportGraphic::truss
    { 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 }, // MetalSupportGraphic::tubesInverted
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::boxedCoated
};

/** rct2: 0x0097B142 */
static constexpr uint8_t kMetalSupportTypeToHeight[] = {
    6, // MetalSupportGraphic::tubes
    3, // MetalSupportGraphic::fork
    3, // MetalSupportGraphic::forkAlt
    6, // MetalSupportGraphic::boxed
    3, // MetalSupportGraphic::stick
    3, // MetalSupportGraphic::stickAlt
    6, // MetalSupportGraphic::thickCentred
    6, // MetalSupportGraphic::thick
    6, // MetalSupportGraphic::thickAlt
    6, // MetalSupportGraphic::thickAltCentred
    4, // MetalSupportGraphic::truss
    3, // MetalSupportGraphic::tubesInverted
    6, // MetalSupportGraphic::boxedCoated
};

struct MetalSupportsImages {
    ImageIndex base;
    ImageIndex beamUncapped;
    ImageIndex beamCapped;
};

/** rct2: 0x0097B15C, 0x0097B190 */
static constexpr MetalSupportsImages kSupportBasesAndBeams[] = {
    { 3243, 3209, 3226 }, // MetalSupportGraphic::tubes
    { 3279, 3262, 3262 }, // MetalSupportGraphic::fork
    { 3298, 3262, 3262 }, // MetalSupportGraphic::forkAlt
    { 3334, 3317, 3317 }, // MetalSupportGraphic::boxed
    { kImageIndexUndefined, 3658, 3658 }, // MetalSupportGraphic::stick
    { kImageIndexUndefined, 3658, 3658 }, // MetalSupportGraphic::stickAlt
    { kImageIndexUndefined, 3141, 3141 }, // MetalSupportGraphic::thickCentred
    { kImageIndexUndefined, 3158, 3158 }, // MetalSupportGraphic::thick
    { kImageIndexUndefined, 3175, 3175 }, // MetalSupportGraphic::thickAlt
    { kImageIndexUndefined, 3192, 3192 }, // MetalSupportGraphic::thickAltCentred
    { kImageIndexUndefined, 3124, 3124 }, // MetalSupportGraphic::truss
    { 3243, 3209, 3226 }, // MetalSupportGraphic::tubesInverted
    { 3334, 3353, 3353 }, // MetalSupportGraphic::boxedCoated
};

/** rct2: 0x0097B404 */
static constexpr uint8_t kMetalSupportsSlopeImageOffsetMap[] = {
    0,  // kTileSlopeFlat
    1,  // kTileSlopeNCornerUp
    2,  // kTileSlopeECornerUp
    3,  // kTileSlopeNESideUp
    4,  // kTileSlopeSCornerUp
    5,  // kTileSlopeNSValley
    6,  // kTileSlopeSESideUp
    7,  // kTileSlopeWCornerDown
    8,  // kTileSlopeWCornerUp
    9,  // kTileSlopeNWSideUp
    10, // kTileSlopeWEValley
    11, // kTileSlopeSCornerDown
    12, // kTileSlopeSWSideUp
    13, // kTileSlopeECornerDown
    14, // kTileSlopeNCornerDown
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    15, // kTileSlopeDiagonalFlag | kTileSlopeWCornerDown
    0,  // (invalid)
    0,  // (invalid)
    0,  // (invalid)
    16, // kTileSlopeDiagonalFlag | kTileSlopeSCornerDown
    0,  // (invalid)
    17, // kTileSlopeDiagonalFlag | kTileSlopeECornerDown
    18, // kTileSlopeDiagonalFlag | kTileSlopeNCornerDown
    0,  // (invalid)
};
// clang-format on

static constexpr MetalSupportPlace kMetalSupportPlacementRotated[][kNumOrthogonalDirections] = {
    { MetalSupportPlace::topCorner, MetalSupportPlace::rightCorner, MetalSupportPlace::bottomCorner,
      MetalSupportPlace::leftCorner },
    { MetalSupportPlace::leftCorner, MetalSupportPlace::topCorner, MetalSupportPlace::rightCorner,
      MetalSupportPlace::bottomCorner },
    { MetalSupportPlace::rightCorner, MetalSupportPlace::bottomCorner, MetalSupportPlace::leftCorner,
      MetalSupportPlace::topCorner },
    { MetalSupportPlace::bottomCorner, MetalSupportPlace::leftCorner, MetalSupportPlace::topCorner,
      MetalSupportPlace::rightCorner },
    { MetalSupportPlace::centre, MetalSupportPlace::centre, MetalSupportPlace::centre, MetalSupportPlace::centre },
    { MetalSupportPlace::topLeftSide, MetalSupportPlace::topRightSide, MetalSupportPlace::bottomRightSide,
      MetalSupportPlace::bottomLeftSide },
    { MetalSupportPlace::topRightSide, MetalSupportPlace::bottomRightSide, MetalSupportPlace::bottomLeftSide,
      MetalSupportPlace::topLeftSide },
    { MetalSupportPlace::bottomLeftSide, MetalSupportPlace::topLeftSide, MetalSupportPlace::topRightSide,
      MetalSupportPlace::bottomRightSide },
    { MetalSupportPlace::bottomRightSide, MetalSupportPlace::bottomLeftSide, MetalSupportPlace::topLeftSide,
      MetalSupportPlace::topRightSide },
};

constexpr MetalSupportGraphic kMetalSupportGraphicRotated[kMetalSupportTypeCount][kNumOrthogonalDirections] = {
    { MetalSupportGraphic::tubes, MetalSupportGraphic::tubes, MetalSupportGraphic::tubes, MetalSupportGraphic::tubes },
    { MetalSupportGraphic::fork, MetalSupportGraphic::forkAlt, MetalSupportGraphic::fork, MetalSupportGraphic::forkAlt },
    { MetalSupportGraphic::boxed, MetalSupportGraphic::boxed, MetalSupportGraphic::boxed, MetalSupportGraphic::boxed },
    { MetalSupportGraphic::stick, MetalSupportGraphic::stickAlt, MetalSupportGraphic::stick, MetalSupportGraphic::stickAlt },
    { MetalSupportGraphic::thick, MetalSupportGraphic::thickAlt, MetalSupportGraphic::thickCentred,
      MetalSupportGraphic::thickAltCentred },
    { MetalSupportGraphic::truss, MetalSupportGraphic::truss, MetalSupportGraphic::truss, MetalSupportGraphic::truss },
    { MetalSupportGraphic::tubesInverted, MetalSupportGraphic::tubesInverted, MetalSupportGraphic::tubesInverted,
      MetalSupportGraphic::tubesInverted },
    { MetalSupportGraphic::boxedCoated, MetalSupportGraphic::boxedCoated, MetalSupportGraphic::boxedCoated,
      MetalSupportGraphic::boxedCoated },
};

constexpr const int32_t kMetalSupportBaseHeight = 6;
constexpr const int32_t kMetalSupportMaxSectionHeight = 16;
constexpr const int32_t kMetalSupportJointInterval = 4;
constexpr const int32_t kMetalSupportCrossbeamTwoSegmentOffsetIndex = 4;

static inline MetalSupportGraphic RotateMetalSupportGraphic(MetalSupportType supportType, Direction direction);

template<bool typeB>
static bool MetalSupportsPaintSetupCommon(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement,
    const int32_t heightExtra, const int32_t height, ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::paletteDarken1);
    }

    int32_t currentHeight = height;
    const uint32_t supportType = EnumValue(supportTypeMember);

    // Offset the support and draw a crossbeam if necessary
    const uint8_t originalSegment = EnumValue(placement);
    uint8_t segment = originalSegment;
    uint16_t segmentHeight = 0xFFFF;
    SupportHeight* const supportSegments = session.SupportSegments;
    if (currentHeight < supportSegments[segment].height)
    {
        segmentHeight = currentHeight;

        currentHeight -= kMetalSupportTypeToHeight[supportType];
        if (currentHeight < 0)
            return false;

        RepositionPair newPlacement = {};
        for (size_t attempt = 0; attempt < kMetalSupportSegmentOffsets.size(); attempt++)
        {
            const auto& candidate = kMetalSupportSegmentOffsets[attempt][segment][session.CurrentRotation];
            if (currentHeight > supportSegments[EnumValue(candidate.place)].height)
            {
                newPlacement = candidate;
                break;
            }
        }
        if (newPlacement.isNull())
        {
            return false;
        }

        const uint8_t crossBeamIndex = newPlacement.crossBeamIndex;
        if constexpr (typeB)
        {
            if (crossBeamIndex >= kMetalSupportCrossbeamTwoSegmentOffsetIndex)
                return false;
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kMetalSupportTypeToCrossbeamImages[supportType][crossBeamIndex]),
            { kMetalSupportBoundBoxOffsets[segment] + kMetalSupportCrossBeamBoundBoxOffsets[crossBeamIndex], currentHeight },
            { kMetalSupportCrossBeamBoundBoxLengths[crossBeamIndex], 1 });

        segment = EnumValue(newPlacement.place);
    }

    const int16_t crossbeamHeight = currentHeight;

    // Draw support bases
    if (supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery
        || currentHeight - supportSegments[segment].height < kMetalSupportBaseHeight
        || kSupportBasesAndBeams[supportType].base == kImageIndexUndefined)
    {
        currentHeight = supportSegments[segment].height;
    }
    else
    {
        const auto imageIndex = kSupportBasesAndBeams[supportType].base
            + kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex),
            { kMetalSupportBoundBoxOffsets[segment], supportSegments[segment].height }, { 0, 0, kMetalSupportBaseHeight - 1 });

        currentHeight = supportSegments[segment].height + kMetalSupportBaseHeight;
    }

    const auto supportBeamImageIndex = kSupportBasesAndBeams[supportType].beamUncapped;

    // Draw an initial support segment to get the main segment height to a multiple of kMetalSupportMaxSectionHeight
    const int16_t heightDiff = std::min<int16_t>(
                                   floor2(currentHeight + kMetalSupportMaxSectionHeight, kMetalSupportMaxSectionHeight),
                                   crossbeamHeight)
        - currentHeight;
    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(supportBeamImageIndex + heightDiff - 1),
            { kMetalSupportBoundBoxOffsets[segment], currentHeight }, { 0, 0, heightDiff - 1 });
    }

    currentHeight += heightDiff;

    // Draw main support segments
    for (uint8_t count = 1;; count++)
    {
        const int16_t beamLength = std::min<int16_t>(currentHeight + kMetalSupportMaxSectionHeight, crossbeamHeight)
            - currentHeight;
        if (beamLength <= 0)
            break;

        uint32_t imageIndex = supportBeamImageIndex + beamLength - 1;
        if (count % kMetalSupportJointInterval == 0 && beamLength == kMetalSupportMaxSectionHeight)
            imageIndex++;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { kMetalSupportBoundBoxOffsets[segment], currentHeight },
            { 0, 0, beamLength - 1 });

        currentHeight += beamLength;
    }

    supportSegments[segment].height = segmentHeight;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    // Draw extra support segments above height with a zero height bounding box
    currentHeight = heightExtra < 0 ? height - 1 : height;
    const auto extraSupportBeamImageIndex = heightExtra >= 0 ? kSupportBasesAndBeams[supportType].beamUncapped
                                                             : kSupportBasesAndBeams[supportType].beamCapped;
    const auto totalHeightExtra = heightExtra < 0 ? currentHeight + (-heightExtra) : currentHeight + heightExtra;
    const CoordsXYZ boundBoxOffset = CoordsXYZ(kMetalSupportBoundBoxOffsets[originalSegment], currentHeight);
    while (true)
    {
        const int16_t beamLength = std::min(currentHeight + kMetalSupportMaxSectionHeight, totalHeightExtra) - currentHeight;
        if (beamLength <= 0)
            break;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(extraSupportBeamImageIndex + beamLength - 1),
            { kMetalSupportBoundBoxOffsets[originalSegment], currentHeight }, { boundBoxOffset, { 0, 0, 0 } });

        currentHeight += beamLength;
    }

    return true;
}

bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, const int32_t heightExtra, int32_t height,
    ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, 0);
    return MetalSupportsPaintSetupCommon<false>(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

bool MetalASupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction,
    const int32_t heightExtra, int32_t height, ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, direction);
    placement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalSupportsPaintSetupCommon<false>(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

bool MetalBSupportsPaintSetup(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const int32_t heightExtra,
    int32_t height, const ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, 0);
    return MetalSupportsPaintSetupCommon<true>(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const Direction direction,
    const int32_t heightExtra, const int32_t height, const ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, direction);
    const auto rotatedPlacement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalSupportsPaintSetupCommon<true>(session, supportGraphic, rotatedPlacement, heightExtra, height, imageTemplate);
}

static inline MetalSupportGraphic RotateMetalSupportGraphic(const MetalSupportType supportType, const Direction direction)
{
    assert(direction < kNumOrthogonalDirections);
    return kMetalSupportGraphicRotated[EnumValue(supportType)][direction];
}

void DrawSupportsSideBySide(
    PaintSession& session, const Direction direction, const uint16_t height, const ImageId colour,
    const MetalSupportType supportType, const int32_t heightExtra)
{
    auto graphic = RotateMetalSupportGraphic(supportType, direction);

    if (direction & 1)
    {
        MetalSupportsPaintSetupCommon<false>(session, graphic, MetalSupportPlace::topRightSide, heightExtra, height, colour);
        MetalSupportsPaintSetupCommon<false>(session, graphic, MetalSupportPlace::bottomLeftSide, heightExtra, height, colour);
    }
    else
    {
        MetalSupportsPaintSetupCommon<false>(session, graphic, MetalSupportPlace::topLeftSide, heightExtra, height, colour);
        MetalSupportsPaintSetupCommon<false>(session, graphic, MetalSupportPlace::bottomRightSide, heightExtra, height, colour);
    }
}

/**
 *
 *  rct2: 0x006A326B
 *
 * @param segment (ebx)
 * @param heightExtra (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param railingsDescriptor (0x00F3EF6C)
 *
 * @return Whether supports were drawn
 */
bool PathPoleSupportsPaintSetup(
    PaintSession& session, const MetalSupportPlace supportPlace, const bool isSloped, const int32_t height,
    ImageId imageTemplate, const PathRailingsDescriptor& railings)
{
    auto segment = EnumValue(supportPlace);

    SupportHeight* supportSegments = session.SupportSegments;

    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false;
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::paletteDarken1);
    }

    if (height < supportSegments[segment].height)
    {
        return true; // STC
    }

    uint16_t baseHeight;

    if ((supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery) || (height - supportSegments[segment].height < 6)
        || !(railings.flags & RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE))
    {
        baseHeight = supportSegments[segment].height;
    }
    else
    {
        uint8_t imageOffset = kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        baseHeight = supportSegments[segment].height;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(railings.bridgeImage + 37 + imageOffset),
            { kMetalSupportBoundBoxOffsets[segment].x, kMetalSupportBoundBoxOffsets[segment].y, baseHeight }, { 0, 0, 5 });
        baseHeight += 6;
    }

    // si = height
    // dx = baseHeight

    int16_t heightDiff = floor2(baseHeight + kMetalSupportMaxSectionHeight, kMetalSupportMaxSectionHeight);
    if (heightDiff > height)
    {
        heightDiff = height;
    }

    heightDiff -= baseHeight;

    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(railings.bridgeImage + 20 + (heightDiff - 1)),
            { kMetalSupportBoundBoxOffsets[segment], baseHeight }, { 0, 0, heightDiff - 1 });
    }

    baseHeight += heightDiff;

    bool keepGoing = true;
    while (keepGoing)
    {
        int16_t z;

        for (int32_t i = 0; i < 4; ++i)
        {
            z = baseHeight + kMetalSupportMaxSectionHeight;
            if (z > height)
            {
                z = height;
            }
            z -= baseHeight;

            if (z <= 0)
            {
                keepGoing = false;
                break;
            }

            if (i == 3)
            {
                // Only do the z check in the fourth run.
                break;
            }

            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(railings.bridgeImage + 20 + (z - 1)),
                { kMetalSupportBoundBoxOffsets[segment], baseHeight }, { 0, 0, (z - 1) });

            baseHeight += z;
        }

        if (!keepGoing)
        {
            break;
        }

        ImageIndex imageIndex = railings.bridgeImage + 20 + (z - 1);
        if (z == kMetalSupportMaxSectionHeight)
        {
            imageIndex += 1;
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { kMetalSupportBoundBoxOffsets[segment], baseHeight },
            { 0, 0, (z - 1) });

        baseHeight += z;
    }

    // Loc6A34D8
    supportSegments[segment].height = 0xFFFF;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    if (isSloped)
    {
        int16_t si = baseHeight + kCoordsZStep;

        while (true)
        {
            int16_t z = baseHeight + (2 * kCoordsZStep);
            if (z > si)
            {
                z = si;
            }

            z -= baseHeight;
            if (z <= 0)
            {
                break;
            }

            ImageIndex imageIndex = railings.bridgeImage + 20 + (z - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { kMetalSupportBoundBoxOffsets[segment], baseHeight },
                { 0, 0, 0 });

            baseHeight += z;
        }
    }

    return false; // AND
}
