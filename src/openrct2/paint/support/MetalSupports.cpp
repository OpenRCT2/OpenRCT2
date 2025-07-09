/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
using namespace OpenRCT2::Numerics;

constexpr auto kMetalSupportSkip = 9 * 4 * 2;

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

/** rct2: 0x0097AF32 */
static constexpr uint8_t kMetalSupportSegmentOffsets[] = {
    5, 2, 5, 2, 5, 2, 5, 2,
    7, 1, 7, 1, 7, 1, 7, 1,
    6, 3, 6, 3, 6, 3, 6, 3,
    8, 0, 8, 0, 8, 0, 8, 0,
    5, 3, 6, 0, 8, 1, 7, 2,
    1, 2, 1, 2, 1, 2, 1, 2,
    0, 3, 0, 3, 0, 3, 0, 3,
    3, 1, 3, 1, 3, 1, 3, 1,
    2, 0, 2, 0, 2, 0, 2, 0,

    6, 1, 6, 1, 6, 1, 6, 1,
    5, 0, 5, 0, 5, 0, 5, 0,
    8, 2, 8, 2, 8, 2, 8, 2,
    7, 3, 7, 3, 7, 3, 7, 3,
    6, 0, 8, 1, 7, 2, 5, 3,
    0, 0, 0, 0, 0, 0, 0, 0,
    2, 1, 2, 1, 2, 1, 2, 1,
    1, 3, 1, 3, 1, 3, 1, 3,
    3, 2, 3, 2, 3, 2, 3, 2,

    1, 6, 1, 6, 1, 6, 1, 6,
    3, 5, 3, 5, 3, 5, 3, 5,
    0, 7, 0, 7, 0, 7, 0, 7,
    2, 4, 2, 4, 2, 4, 2, 4,
    8, 1, 7, 2, 5, 3, 6, 0,
    4, 1, 4, 1, 4, 1, 4, 1,
    4, 2, 4, 2, 4, 2, 4, 2,
    4, 0, 4, 0, 4, 0, 4, 0,
    4, 3, 4, 3, 4, 3, 4, 3,

    2, 5, 2, 5, 2, 5, 2, 5,
    0, 4, 0, 4, 0, 4, 0, 4,
    3, 6, 3, 6, 3, 6, 3, 6,
    1, 7, 1, 7, 1, 7, 1, 7,
    7, 2, 5, 3, 6, 0, 8, 1,
    8, 5, 8, 5, 8, 5, 8, 5,
    7, 6, 7, 6, 7, 6, 7, 6,
    6, 4, 6, 4, 6, 4, 6, 4,
    5, 7, 5, 7, 5, 7, 5, 7,
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

static bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportGraphic supportTypeMember, MetalSupportPlace placement, int32_t heightExtra,
    int32_t height, ImageId imageTemplate);
static bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportGraphic supportTypeMember, MetalSupportPlace placement, int32_t heightExtra,
    int32_t height, ImageId imageTemplate);
static inline MetalSupportGraphic RotateMetalSupportGraphic(MetalSupportType supportType, Direction direction);

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param heightExtra (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *  rct2: 0x00663105
 */
static bool MetalASupportsPaintSetup(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement, int32_t heightExtra,
    const int32_t height, ImageId imageTemplate)
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
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::PaletteDarken1);
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

        uint16_t baseIndex = session.CurrentRotation * 2;
        uint8_t newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
        if (currentHeight <= supportSegments[newSegment].height)
        {
            baseIndex += kMetalSupportSkip;
            newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
            if (currentHeight <= supportSegments[newSegment].height)
            {
                baseIndex += kMetalSupportSkip;
                newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
                if (currentHeight <= supportSegments[newSegment].height)
                {
                    baseIndex += kMetalSupportSkip;
                    newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
                    if (currentHeight <= supportSegments[newSegment].height)
                    {
                        return false;
                    }
                }
            }
        }

        const uint8_t crossBeamIndex = kMetalSupportSegmentOffsets[baseIndex + segment * 8 + 1];

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kMetalSupportTypeToCrossbeamImages[supportType][crossBeamIndex]),
            { kMetalSupportBoundBoxOffsets[segment] + kMetalSupportCrossBeamBoundBoxOffsets[crossBeamIndex], currentHeight },
            { kMetalSupportCrossBeamBoundBoxLengths[crossBeamIndex], 1 });

        segment = newSegment;
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

    // Draw an initial support segment to get the main segment height to a multiple of 16
    const int16_t heightDiff = std::min<int16_t>(floor2(currentHeight + 16, 16), crossbeamHeight) - currentHeight;
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
        const int16_t beamLength = std::min<int16_t>(currentHeight + 16, crossbeamHeight) - currentHeight;
        if (beamLength <= 0)
            break;

        uint32_t imageIndex = supportBeamImageIndex + beamLength - 1;
        if (count % 4 == 0 && beamLength == 16)
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
    const auto totalHeightExtra = heightExtra < 0 ? currentHeight + (-heightExtra) : currentHeight + heightExtra;
    const CoordsXYZ boundBoxOffset = CoordsXYZ(kMetalSupportBoundBoxOffsets[originalSegment], currentHeight);
    while (true)
    {
        const int16_t beamLength = std::min(currentHeight + 16, totalHeightExtra) - currentHeight;
        if (beamLength <= 0)
            break;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kSupportBasesAndBeams[supportType].beamCapped + beamLength - 1),
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
    return MetalASupportsPaintSetup(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

bool MetalASupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportType, MetalSupportPlace placement, Direction direction,
    const int32_t heightExtra, int32_t height, ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, direction);
    placement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalASupportsPaintSetup(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 *
 * @param supportType (edi)
 * @param segment (ebx)
 * @param heightExtra (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *
 * @return (Carry Flag)
 */
static bool MetalBSupportsPaintSetup(
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
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::PaletteDarken1);
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

        uint16_t baseIndex = session.CurrentRotation * 2;
        uint8_t newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
        if (currentHeight <= supportSegments[newSegment].height)
        {
            baseIndex += kMetalSupportSkip;
            newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
            if (currentHeight <= supportSegments[newSegment].height)
            {
                baseIndex += kMetalSupportSkip;
                newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
                if (currentHeight <= supportSegments[newSegment].height)
                {
                    baseIndex += kMetalSupportSkip;
                    newSegment = kMetalSupportSegmentOffsets[baseIndex + segment * 8];
                    if (currentHeight <= supportSegments[newSegment].height)
                    {
                        return false;
                    }
                }
            }
        }

        const uint8_t crossBeamIndex = kMetalSupportSegmentOffsets[baseIndex + segment * 8 + 1];

        if (crossBeamIndex >= 4)
            return false;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kMetalSupportTypeToCrossbeamImages[supportType][crossBeamIndex]),
            { kMetalSupportBoundBoxOffsets[segment] + kMetalSupportCrossBeamBoundBoxOffsets[crossBeamIndex], currentHeight },
            { kMetalSupportCrossBeamBoundBoxLengths[crossBeamIndex], 1 });

        segment = newSegment;
    }

    const int32_t crossbeamHeight = currentHeight;

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

    // Draw an initial support segment to get the main segment height to a multiple of 16
    const int16_t heightDiff = std::min<int16_t>(floor2(currentHeight + 16, 16), crossbeamHeight) - currentHeight;
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
        const int16_t beamLength = std::min<int16_t>(currentHeight + 16, crossbeamHeight) - currentHeight;
        if (beamLength <= 0)
            break;

        uint32_t imageIndex = supportBeamImageIndex + beamLength - 1;
        if (count % 4 == 0 && beamLength == 16)
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
    const auto totalHeightExtra = heightExtra < 0 ? currentHeight + (-heightExtra) : currentHeight + heightExtra;
    const CoordsXYZ boundBoxOffset = CoordsXYZ(kMetalSupportBoundBoxOffsets[segment], currentHeight);
    while (true)
    {
        const int16_t beamLength = std::min(currentHeight + 16, totalHeightExtra) - currentHeight;
        if (beamLength <= 0)
            break;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kSupportBasesAndBeams[supportType].beamCapped + beamLength - 1),
            { kMetalSupportBoundBoxOffsets[segment], currentHeight }, { boundBoxOffset, { 0, 0, 0 } });

        currentHeight += beamLength;
    }

    return false; // AND
}

bool MetalBSupportsPaintSetup(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const int32_t heightExtra,
    int32_t height, const ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, 0);
    return MetalBSupportsPaintSetup(session, supportGraphic, placement, heightExtra, height, imageTemplate);
}

bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const Direction direction,
    const int32_t heightExtra, const int32_t height, const ImageId imageTemplate)
{
    auto supportGraphic = RotateMetalSupportGraphic(supportType, direction);
    const auto rotatedPlacement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalBSupportsPaintSetup(session, supportGraphic, rotatedPlacement, heightExtra, height, imageTemplate);
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
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::topRightSide, heightExtra, height, colour);
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::bottomLeftSide, heightExtra, height, colour);
    }
    else
    {
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::topLeftSide, heightExtra, height, colour);
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::bottomRightSide, heightExtra, height, colour);
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
    ImageId imageTemplate, const FootpathPaintInfo& pathPaintInfo)
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
        imageTemplate = ImageId().WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    if (height < supportSegments[segment].height)
    {
        return true; // STC
    }

    uint16_t baseHeight;

    if ((supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery) || (height - supportSegments[segment].height < 6)
        || !(pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE))
    {
        baseHeight = supportSegments[segment].height;
    }
    else
    {
        uint8_t imageOffset = kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        baseHeight = supportSegments[segment].height;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 37 + imageOffset),
            { kMetalSupportBoundBoxOffsets[segment].x, kMetalSupportBoundBoxOffsets[segment].y, baseHeight }, { 0, 0, 5 });
        baseHeight += 6;
    }

    // si = height
    // dx = baseHeight

    int16_t heightDiff = floor2(baseHeight + 16, 16);
    if (heightDiff > height)
    {
        heightDiff = height;
    }

    heightDiff -= baseHeight;

    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (heightDiff - 1)),
            { kMetalSupportBoundBoxOffsets[segment], baseHeight }, { 0, 0, heightDiff - 1 });
    }

    baseHeight += heightDiff;

    bool keepGoing = true;
    while (keepGoing)
    {
        int16_t z;

        for (int32_t i = 0; i < 4; ++i)
        {
            z = baseHeight + 16;
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
                session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (z - 1)),
                { kMetalSupportBoundBoxOffsets[segment], baseHeight }, { 0, 0, (z - 1) });

            baseHeight += z;
        }

        if (!keepGoing)
        {
            break;
        }

        ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 20 + (z - 1);
        if (z == 16)
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

            ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 20 + (z - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { kMetalSupportBoundBoxOffsets[segment], baseHeight },
                { 0, 0, 0 });

            baseHeight += z;
        }
    }

    return false; // AND
}
