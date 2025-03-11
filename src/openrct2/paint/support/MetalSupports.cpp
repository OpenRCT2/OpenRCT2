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
    Tubes = 0,
    /**
     * Used by the Junior RC and other rides.
     */
    Fork = 1,
    /**
     * Rotated version of `Fork`.
     */
    ForkAlt = 2,
    /**
     * Used by the vertical roller coasters, the Log Flume and other rides.
     */
    Boxed = 3,
    /**
     * Used by the Steeplechase.
     */
    Stick = 4,
    /**
     * No visible difference from `Stick`, also used by the Steeplechase
     */
    StickAlt = 5,
    /**
     * Every “Thick” type seems to be used for the Looping Roller Coaster’s loop, and only for that specific part.
     */
    ThickCentred = 6,
    Thick = 7,
    ThickAlt = 8,
    ThickAltCentred = 9,
    /**
     * Used by the chairlift.
     */
    Truss = 10,
    /**
     * Used by inverted rcs like the flying, lay-down, compact inverted.
     * Mostly the same as `Tubes`, but with a thinner crossbeam.
     */
    TubesInverted = 11,
    /**
     * Does not seem to be used in RCT2, but it was used in RCT1 for one of the path support types.
     */
    BoxedCoated,
};

/** rct2: 0x0097AF20, 0x0097AF21 */
// clang-format off
static constexpr CoordsXY SupportBoundBoxes[] = {
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
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::Tubes
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::Fork
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::ForkAlt
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::Boxed
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::Stick
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportGraphic::StickAlt
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportGraphic::ThickCentred
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportGraphic::Thick
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportGraphic::ThickAlt
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportGraphic::ThickAltCentred
    { 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 }, // MetalSupportGraphic::Truss
    { 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 }, // MetalSupportGraphic::TubesInverted
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportGraphic::BoxedCoated
};

/** rct2: 0x0097B142 */
static constexpr uint8_t kMetalSupportTypeToHeight[] = {
    6, // MetalSupportGraphic::Tubes
    3, // MetalSupportGraphic::Fork
    3, // MetalSupportGraphic::ForkAlt
    6, // MetalSupportGraphic::Boxed
    3, // MetalSupportGraphic::Stick
    3, // MetalSupportGraphic::StickAlt
    6, // MetalSupportGraphic::ThickCentred
    6, // MetalSupportGraphic::Thick
    6, // MetalSupportGraphic::ThickAlt
    6, // MetalSupportGraphic::ThickAltCentred
    4, // MetalSupportGraphic::Truss
    3, // MetalSupportGraphic::TubesInverted
    6, // MetalSupportGraphic::BoxedCoated
};

struct MetalSupportsImages {
    ImageIndex base;
    ImageIndex beamUncapped;
    ImageIndex beamCapped;
};

/** rct2: 0x0097B15C, 0x0097B190 */
static constexpr MetalSupportsImages kSupportBasesAndBeams[] = {
    { 3243, 3209, 3226 }, // MetalSupportGraphic::Tubes
    { 3279, 3262, 3262 }, // MetalSupportGraphic::Fork
    { 3298, 3262, 3262 }, // MetalSupportGraphic::ForkAlt
    { 3334, 3317, 3317 }, // MetalSupportGraphic::Boxed
    { kImageIndexUndefined, 3658, 3658 }, // MetalSupportGraphic::Stick
    { kImageIndexUndefined, 3658, 3658 }, // MetalSupportGraphic::StickAlt
    { kImageIndexUndefined, 3141, 3141 }, // MetalSupportGraphic::ThickCentred
    { kImageIndexUndefined, 3158, 3158 }, // MetalSupportGraphic::Thick
    { kImageIndexUndefined, 3175, 3175 }, // MetalSupportGraphic::ThickAlt
    { kImageIndexUndefined, 3192, 3192 }, // MetalSupportGraphic::ThickAltCentred
    { kImageIndexUndefined, 3124, 3124 }, // MetalSupportGraphic::Truss
    { 3243, 3209, 3226 }, // MetalSupportGraphic::TubesInverted
    { 3334, 3353, 3353 }, // MetalSupportGraphic::BoxedCoated
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
    { MetalSupportPlace::TopCorner, MetalSupportPlace::RightCorner, MetalSupportPlace::BottomCorner,
      MetalSupportPlace::LeftCorner },
    { MetalSupportPlace::LeftCorner, MetalSupportPlace::TopCorner, MetalSupportPlace::RightCorner,
      MetalSupportPlace::BottomCorner },
    { MetalSupportPlace::RightCorner, MetalSupportPlace::BottomCorner, MetalSupportPlace::LeftCorner,
      MetalSupportPlace::TopCorner },
    { MetalSupportPlace::BottomCorner, MetalSupportPlace::LeftCorner, MetalSupportPlace::TopCorner,
      MetalSupportPlace::RightCorner },
    { MetalSupportPlace::Centre, MetalSupportPlace::Centre, MetalSupportPlace::Centre, MetalSupportPlace::Centre },
    { MetalSupportPlace::TopLeftSide, MetalSupportPlace::TopRightSide, MetalSupportPlace::BottomRightSide,
      MetalSupportPlace::BottomLeftSide },
    { MetalSupportPlace::TopRightSide, MetalSupportPlace::BottomRightSide, MetalSupportPlace::BottomLeftSide,
      MetalSupportPlace::TopLeftSide },
    { MetalSupportPlace::BottomLeftSide, MetalSupportPlace::TopLeftSide, MetalSupportPlace::TopRightSide,
      MetalSupportPlace::BottomRightSide },
    { MetalSupportPlace::BottomRightSide, MetalSupportPlace::BottomLeftSide, MetalSupportPlace::TopLeftSide,
      MetalSupportPlace::TopRightSide },
};

constexpr MetalSupportGraphic kMetalSupportGraphicRotated[kMetalSupportTypeCount][kNumOrthogonalDirections] = {
    { MetalSupportGraphic::Tubes, MetalSupportGraphic::Tubes, MetalSupportGraphic::Tubes, MetalSupportGraphic::Tubes },
    { MetalSupportGraphic::Fork, MetalSupportGraphic::ForkAlt, MetalSupportGraphic::Fork, MetalSupportGraphic::ForkAlt },
    { MetalSupportGraphic::Boxed, MetalSupportGraphic::Boxed, MetalSupportGraphic::Boxed, MetalSupportGraphic::Boxed },
    { MetalSupportGraphic::Stick, MetalSupportGraphic::StickAlt, MetalSupportGraphic::Stick, MetalSupportGraphic::StickAlt },
    { MetalSupportGraphic::Thick, MetalSupportGraphic::ThickAlt, MetalSupportGraphic::ThickCentred,
      MetalSupportGraphic::ThickAltCentred },
    { MetalSupportGraphic::Truss, MetalSupportGraphic::Truss, MetalSupportGraphic::Truss, MetalSupportGraphic::Truss },
    { MetalSupportGraphic::TubesInverted, MetalSupportGraphic::TubesInverted, MetalSupportGraphic::TubesInverted,
      MetalSupportGraphic::TubesInverted },
    { MetalSupportGraphic::BoxedCoated, MetalSupportGraphic::BoxedCoated, MetalSupportGraphic::BoxedCoated,
      MetalSupportGraphic::BoxedCoated },
};

constexpr int32_t kMetalSupportBaseHeight = 6;

static bool MetalASupportsPaintSetup(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement,
    const int32_t special, int32_t height, const bool drawCap, ImageId imageTemplate);
static bool MetalBSupportsPaintSetup(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement,
    const int32_t special, int32_t height, const bool drawCap, ImageId imageTemplate);
static inline MetalSupportGraphic RotateMetalSupportGraphic(const MetalSupportType supportType, const Direction direction);

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *  rct2: 0x00663105
 */
static bool MetalASupportsPaintSetup(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement,
    const int32_t special, const int32_t height, const bool drawCap, ImageId imageTemplate)
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

    const uint8_t originalSegment = EnumValue(placement);
    const uint32_t supportType = EnumValue(supportTypeMember);
    SupportHeight* const supportSegments = session.SupportSegments;

    int32_t currentHeight = height;
    uint8_t segment = originalSegment;
    uint16_t segmentHeight = 0xFFFF;
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
            { SupportBoundBoxes[segment] + kMetalSupportCrossBeamBoundBoxOffsets[crossBeamIndex], currentHeight },
            { kMetalSupportCrossBeamBoundBoxLengths[crossBeamIndex], 1 });

        segment = newSegment;
    }

    const int16_t crossbeamHeight = currentHeight;

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
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], supportSegments[segment].height },
            { 1, 1, kMetalSupportBaseHeight - 1 });

        currentHeight = supportSegments[segment].height + kMetalSupportBaseHeight;
    }

    const auto supportBeamImageIndex = drawCap ? kSupportBasesAndBeams[supportType].beamCapped
                                               : kSupportBasesAndBeams[supportType].beamUncapped;

    // Work out if a small support segment required to bring support to normal
    // size (aka floor2(x, 16))
    const int16_t heightDiff = std::min<int16_t>(floor2(currentHeight + 16, 16), crossbeamHeight) - currentHeight;
    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(supportBeamImageIndex + heightDiff - 1),
            { SupportBoundBoxes[segment], currentHeight }, { 1, 1, heightDiff - 1 });
    }

    currentHeight += heightDiff;

    for (uint8_t count = 1;; count++)
    {
        const int16_t beamLength = std::min<int16_t>(currentHeight + 16, crossbeamHeight) - currentHeight;
        if (beamLength <= 0)
            break;

        uint32_t imageIndex = supportBeamImageIndex + beamLength - 1;
        if (count % 4 == 0 && beamLength == 16)
            imageIndex++;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], currentHeight },
            { 1, 1, beamLength - 1 });

        currentHeight += beamLength;
    }

    supportSegments[segment].height = segmentHeight;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    currentHeight = height;
    const CoordsXYZ boundBoxOffset = CoordsXYZ(SupportBoundBoxes[originalSegment], currentHeight);
    while (true)
    {
        const int16_t beamLength = std::min(currentHeight + 16, height + special) - currentHeight;
        if (beamLength <= 0)
            break;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(supportBeamImageIndex + beamLength - 1),
            { SupportBoundBoxes[originalSegment], currentHeight }, { boundBoxOffset, { 1, 1, 0 } });

        currentHeight += beamLength;
    }

    return true;
}

bool MetalASupportsPaintSetup(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const int32_t special,
    int32_t height, const bool drawCap, ImageId imageTemplate)
{
    return MetalASupportsPaintSetup(
        session, RotateMetalSupportGraphic(supportType, 0), placement, special, height, drawCap, imageTemplate);
}

bool MetalASupportsPaintSetupRotated(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const Direction direction,
    const int32_t special, int32_t height, const bool drawCap, ImageId imageTemplate)
{
    return MetalASupportsPaintSetup(
        session, RotateMetalSupportGraphic(supportType, direction),
        kMetalSupportPlacementRotated[EnumValue(placement)][direction], special, height, drawCap, imageTemplate);
}

/**
 * Metal pole supports
 *  rct2: 0x00663584
 *
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *
 * @return (Carry Flag)
 */
static bool MetalBSupportsPaintSetup(
    PaintSession& session, const MetalSupportGraphic supportTypeMember, const MetalSupportPlace placement,
    const int32_t special, const int32_t height, const bool drawCap, ImageId imageTemplate)
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

    const uint8_t originalSegment = EnumValue(placement);
    const uint32_t supportType = EnumValue(supportTypeMember);
    SupportHeight* const supportSegments = session.SupportSegments;

    int32_t currentHeight = height;
    uint8_t segment = originalSegment;
    uint16_t segmentHeight = 0xFFFF;
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
                        return true;
                    }
                }
            }
        }

        const uint8_t crossBeamIndex = kMetalSupportSegmentOffsets[baseIndex + segment * 8 + 1];
        if (crossBeamIndex >= 4)
            return true;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kMetalSupportTypeToCrossbeamImages[supportType][crossBeamIndex]),
            { SupportBoundBoxes[segment] + kMetalSupportCrossBeamBoundBoxOffsets[crossBeamIndex], currentHeight },
            { kMetalSupportCrossBeamBoundBoxLengths[crossBeamIndex], 1 });

        segment = newSegment;
    }

    const int16_t crossbeamHeight = currentHeight;

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
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], supportSegments[segment].height },
            { 1, 1, kMetalSupportBaseHeight - 1 });

        currentHeight = supportSegments[segment].height + kMetalSupportBaseHeight;
    }

    const auto supportBeamImageIndex = drawCap ? kSupportBasesAndBeams[supportType].beamCapped
                                               : kSupportBasesAndBeams[supportType].beamUncapped;

    // Work out if a small support segment required to bring support to normal
    // size (aka floor2(x, 16))
    const int16_t heightDiff = std::min<int16_t>(floor2(currentHeight + 16, 16), crossbeamHeight) - currentHeight;
    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(supportBeamImageIndex + heightDiff - 1),
            { SupportBoundBoxes[segment], currentHeight }, { 1, 1, heightDiff - 1 });
    }

    currentHeight += heightDiff;

    for (uint8_t count = 1;; count++)
    {
        const int16_t beamLength = std::min<int16_t>(currentHeight + 16, crossbeamHeight) - currentHeight;
        if (beamLength <= 0)
            break;

        uint32_t imageIndex = supportBeamImageIndex + beamLength - 1;
        if (count % 4 == 0 && beamLength == 16)
            imageIndex++;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], currentHeight },
            { 1, 1, beamLength - 1 });

        currentHeight += beamLength;
    }

    supportSegments[segment].height = segmentHeight;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    currentHeight = height;
    const CoordsXYZ boundBoxOffset = CoordsXYZ(SupportBoundBoxes[originalSegment], currentHeight);
    while (true)
    {
        const int16_t beamLength = std::min(currentHeight + 16, height + special) - currentHeight;
        if (beamLength <= 0)
            break;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(supportBeamImageIndex + beamLength - 1),
            { SupportBoundBoxes[originalSegment], currentHeight }, { boundBoxOffset, { 1, 1, 0 } });

        currentHeight += beamLength;
    }

    return false; // AND
}

bool MetalBSupportsPaintSetup(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const int32_t special,
    int32_t height, const bool drawCap, ImageId imageTemplate)
{
    return MetalBSupportsPaintSetup(
        session, RotateMetalSupportGraphic(supportType, 0), placement, special, height, drawCap, imageTemplate);
}

bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, const MetalSupportType supportType, const MetalSupportPlace placement, const Direction direction,
    const int32_t special, int32_t height, const bool drawCap, ImageId imageTemplate)
{
    return MetalBSupportsPaintSetup(
        session, RotateMetalSupportGraphic(supportType, direction),
        kMetalSupportPlacementRotated[EnumValue(placement)][direction], special, height, drawCap, imageTemplate);
}

static inline MetalSupportGraphic RotateMetalSupportGraphic(const MetalSupportType supportType, const Direction direction)
{
    assert(direction < kNumOrthogonalDirections);
    return kMetalSupportGraphicRotated[EnumValue(supportType)][direction];
}

void DrawSupportsSideBySide(
    PaintSession& session, const Direction direction, const uint16_t height, ImageId colour, const MetalSupportType supportType,
    const int32_t special)
{
    auto graphic = RotateMetalSupportGraphic(supportType, direction);

    if (direction & 1)
    {
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::TopRightSide, special, height, false, colour);
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::BottomLeftSide, special, height, false, colour);
    }
    else
    {
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::TopLeftSide, special, height, false, colour);
        MetalASupportsPaintSetup(session, graphic, MetalSupportPlace::BottomRightSide, special, height, false, colour);
    }
}

/**
 *
 *  rct2: 0x006A326B
 *
 * @param segment (ebx)
 * @param special (ax)
 * @param height (dx)
 * @param imageTemplate (ebp)
 * @param railingsDescriptor (0x00F3EF6C)
 *
 * @return Whether supports were drawn
 */
bool PathPoleSupportsPaintSetup(
    PaintSession& session, MetalSupportPlace supportPlace, const bool isSloped, int32_t height, ImageId imageTemplate,
    const FootpathPaintInfo& pathPaintInfo)
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

    uint16_t currentHeight;

    if ((supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery) || (height - supportSegments[segment].height < 6)
        || !(pathPaintInfo.RailingFlags & RAILING_ENTRY_FLAG_HAS_SUPPORT_BASE_SPRITE))
    {
        currentHeight = supportSegments[segment].height;
    }
    else
    {
        uint8_t imageOffset = kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        currentHeight = supportSegments[segment].height;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 37 + imageOffset),
            { SupportBoundBoxes[segment].x, SupportBoundBoxes[segment].y, currentHeight }, { 0, 0, 5 });
        currentHeight += 6;
    }

    // si = height
    // dx = currentHeight

    int16_t heightDiff = floor2(currentHeight + 16, 16);
    if (heightDiff > height)
    {
        heightDiff = height;
    }

    heightDiff -= currentHeight;

    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (heightDiff - 1)),
            { SupportBoundBoxes[segment], currentHeight }, { 0, 0, heightDiff - 1 });
    }

    currentHeight += heightDiff;

    bool keepGoing = true;
    while (keepGoing)
    {
        int16_t z;

        for (int32_t i = 0; i < 4; ++i)
        {
            z = currentHeight + 16;
            if (z > height)
            {
                z = height;
            }
            z -= currentHeight;

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
                { SupportBoundBoxes[segment], currentHeight }, { 0, 0, (z - 1) });

            currentHeight += z;
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
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], currentHeight }, { 0, 0, (z - 1) });

        currentHeight += z;
    }

    // Loc6A34D8
    supportSegments[segment].height = 0xFFFF;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    if (isSloped)
    {
        int16_t si = currentHeight + kCoordsZStep;

        while (true)
        {
            int16_t z = currentHeight + (2 * kCoordsZStep);
            if (z > si)
            {
                z = si;
            }

            z -= currentHeight;
            if (z <= 0)
            {
                break;
            }

            ImageIndex imageIndex = pathPaintInfo.BridgeImageId + 20 + (z - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], currentHeight }, { 0, 0, 0 });

            currentHeight += z;
        }
    }

    return false; // AND
}
