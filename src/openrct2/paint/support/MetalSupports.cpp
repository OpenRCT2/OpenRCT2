/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "MetalSupports.h"

#include "../../drawing/Drawing.h"
#include "../../interface/Viewport.h"
#include "../../util/Util.h"
#include "../../world/Surface.h"
#include "../../world/tile_element/Slope.h"
#include "../Paint.SessionFlags.h"
#include "../Paint.h"

constexpr auto kMetalSupportSkip = 9 * 4 * 2;

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
static constexpr uint8_t _97AF32[] = {
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
static constexpr CoordsXY Loc97B052[] = {
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
static constexpr CoordsXY _97B062[] = {
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
static constexpr uint32_t _metalSupportTypeToCrossbeamImages[][8] = {
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::Tubes
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::Fork
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::ForkAlt
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::Boxed
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::Stick
    { 3374, 3375, 3374, 3375, 3376, 3377, 3376, 3377 }, // MetalSupportType::StickAlt
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportType::ThickCentred
    { 3378, 3383, 3378, 3383, 3380, 3385, 3380, 3385 }, // MetalSupportType::Thick
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportType::ThickAlt
    { 3382, 3379, 3382, 3379, 3384, 3381, 3384, 3381 }, // MetalSupportType::ThickAltCentred
    { 3378, 3379, 3378, 3379, 3380, 3381, 3380, 3381 }, // MetalSupportType::Truss
    { 3386, 3387, 3386, 3387, 3388, 3389, 3388, 3389 }, // MetalSupportType::TubesInverted
    { 3370, 3371, 3370, 3371, 3372, 3373, 3372, 3373 }, // MetalSupportType::BoxedCoated
};

/** rct2: 0x0097B142 */
static constexpr uint8_t supportTypeToHeight[] = {
    6, // MetalSupportType::Tubes
    3, // MetalSupportType::Fork
    3, // MetalSupportType::ForkAlt
    6, // MetalSupportType::Boxed
    3, // MetalSupportType::Stick
    3, // MetalSupportType::StickAlt
    6, // MetalSupportType::ThickCentred
    6, // MetalSupportType::Thick
    6, // MetalSupportType::ThickAlt
    6, // MetalSupportType::ThickAltCentred
    4, // MetalSupportType::Truss
    3, // MetalSupportType::TubesInverted
    6, // MetalSupportType::BoxedCoated
};

struct MetalSupportsImages {
    ImageIndex base;
    ImageIndex beamA;
    ImageIndex beamB;
};

/** rct2: 0x0097B15C, 0x0097B190 */
static constexpr MetalSupportsImages kSupportBasesAndBeams[] = {
    { 3243, 3209, 3226 }, // MetalSupportType::Tubes
    { 3279, 3262, 3262 }, // MetalSupportType::Fork
    { 3298, 3262, 3262 }, // MetalSupportType::ForkAlt
    { 3334, 3317, 3317 }, // MetalSupportType::Boxed
    {    0, 3658, 3658 }, // MetalSupportType::Stick
    {    0, 3658, 3658 }, // MetalSupportType::StickAlt
    {    0, 3141, 3141 }, // MetalSupportType::ThickCentred
    {    0, 3158, 3158 }, // MetalSupportType::Thick
    {    0, 3175, 3175 }, // MetalSupportType::ThickAlt
    {    0, 3192, 3192 }, // MetalSupportType::ThickAltCentred
    {    0, 3124, 3124 }, // MetalSupportType::Truss
    { 3243, 3209, 3226 }, // MetalSupportType::TubesInverted
    { 3334, 3353, 3353 }, // MetalSupportType::BoxedCoated
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

constexpr uint8_t MetalSupportTypeCount = 13;
constexpr MetalSupportType kMetalSupportTypeRotated[MetalSupportTypeCount][kNumOrthogonalDirections] = {
    { MetalSupportType::Tubes, MetalSupportType::Tubes, MetalSupportType::Tubes, MetalSupportType::Tubes },
    { MetalSupportType::Fork, MetalSupportType::ForkAlt, MetalSupportType::Fork, MetalSupportType::ForkAlt },
    { MetalSupportType::ForkAlt, MetalSupportType::Fork, MetalSupportType::ForkAlt, MetalSupportType::Fork },
    { MetalSupportType::Boxed, MetalSupportType::Boxed, MetalSupportType::Boxed, MetalSupportType::Boxed },
    { MetalSupportType::Stick, MetalSupportType::StickAlt, MetalSupportType::Stick, MetalSupportType::StickAlt },
    { MetalSupportType::StickAlt, MetalSupportType::Stick, MetalSupportType::StickAlt, MetalSupportType::Stick },
    { MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred, MetalSupportType::Thick, MetalSupportType::ThickAlt },
    { MetalSupportType::Thick, MetalSupportType::ThickAlt, MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred },
    { MetalSupportType::ThickAlt, MetalSupportType::ThickCentred, MetalSupportType::ThickAltCentred, MetalSupportType::Thick },
    { MetalSupportType::ThickAltCentred, MetalSupportType::Thick, MetalSupportType::ThickAlt, MetalSupportType::ThickCentred },
    { MetalSupportType::Truss, MetalSupportType::Truss, MetalSupportType::Truss, MetalSupportType::Truss },
    { MetalSupportType::TubesInverted, MetalSupportType::TubesInverted, MetalSupportType::TubesInverted,
      MetalSupportType::TubesInverted },
    { MetalSupportType::BoxedCoated, MetalSupportType::BoxedCoated, MetalSupportType::BoxedCoated,
      MetalSupportType::BoxedCoated },
};

/**
 * Metal pole supports
 * @param supportType (edi)
 * @param segment (ebx)
 * @param special (ax)
 * @param height (edx)
 * @param imageTemplate (ebp)
 *  rct2: 0x00663105
 */
bool MetalASupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate)
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

    uint8_t segment = EnumValue(placement);
    auto supportType = EnumValue(supportTypeMember);
    SupportHeight* supportSegments = session.SupportSegments;
    int16_t originalHeight = height;
    const auto originalSegment = segment;

    uint16_t unk9E3294 = 0xFFFF;
    if (height < supportSegments[segment].height)
    {
        unk9E3294 = height;

        height -= supportTypeToHeight[supportType];
        if (height < 0)
            return false;

        const uint8_t* esi = &(_97AF32[session.CurrentRotation * 2]);

        uint8_t newSegment = esi[segment * 8];
        if (height <= supportSegments[newSegment].height)
        {
            esi += kMetalSupportSkip;
            newSegment = esi[segment * 8];
            if (height <= supportSegments[newSegment].height)
            {
                esi += kMetalSupportSkip;
                newSegment = esi[segment * 8];
                if (height <= supportSegments[newSegment].height)
                {
                    esi += kMetalSupportSkip;
                    newSegment = esi[segment * 8];
                    if (height <= supportSegments[newSegment].height)
                    {
                        return false;
                    }
                }
            }
        }

        uint8_t ebp = esi[segment * 8 + 1];

        auto offset = CoordsXYZ{ SupportBoundBoxes[segment] + Loc97B052[ebp], height };
        auto boundBoxLength = CoordsXYZ(_97B062[ebp], 1);

        auto image_id = imageTemplate.WithIndex(_metalSupportTypeToCrossbeamImages[supportType][ebp]);
        PaintAddImageAsParent(session, image_id, offset, boundBoxLength);

        segment = newSegment;
    }
    int16_t si = height;
    if (supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery || height - supportSegments[segment].height < 6
        || kSupportBasesAndBeams[supportType].base == 0)
    {
        height = supportSegments[segment].height;
    }
    else
    {
        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        auto imageIndex = kSupportBasesAndBeams[supportType].base;
        imageIndex += kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, supportSegments[segment].height }, { 0, 0, 5 });

        height = supportSegments[segment].height + 6;
    }

    // Work out if a small support segment required to bring support to normal
    // size (aka floor2(x, 16))
    int16_t heightDiff = Floor2(height + 16, 16);
    if (heightDiff > si)
    {
        heightDiff = si;
    }

    heightDiff -= height;

    if (heightDiff > 0)
    {
        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = kSupportBasesAndBeams[supportType].beamA;
        imageIndex += heightDiff - 1;
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { 0, 0, heightDiff - 1 });
    }

    height += heightDiff;
    // 6632e6

    for (uint8_t count = 0;; count++)
    {
        if (count >= 4)
            count = 0;

        int16_t beamLength = height + 16;
        if (beamLength > si)
        {
            beamLength = si;
        }

        beamLength -= height;
        if (beamLength <= 0)
            break;

        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        uint32_t imageIndex = kSupportBasesAndBeams[supportType].beamA;
        imageIndex += beamLength - 1;

        if (count == 3 && beamLength == 0x10)
            imageIndex++;

        auto image_id = imageTemplate.WithIndex(imageIndex);
        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { 0, 0, beamLength - 1 });

        height += beamLength;
    }

    supportSegments[segment].height = unk9E3294;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    height = originalHeight;
    segment = originalSegment;
    if (special == 0)
        return true;

    if (special < 0)
    {
        special = -special;
        height--;
    }

    CoordsXYZ boundBoxOffset = CoordsXYZ(SupportBoundBoxes[segment], height);
    const auto combinedHeight = height + special;

    while (1)
    {
        int16_t beamLength = height + 16;
        if (beamLength > combinedHeight)
        {
            beamLength = combinedHeight;
        }

        beamLength -= height;
        if (beamLength <= 0)
            break;

        int8_t xOffset = SupportBoundBoxes[segment].x;
        int8_t yOffset = SupportBoundBoxes[segment].y;

        auto imageIndex = kSupportBasesAndBeams[supportType].beamB;
        imageIndex += beamLength - 1;
        auto image_id = imageTemplate.WithIndex(imageIndex);

        PaintAddImageAsParent(session, image_id, { xOffset, yOffset, height }, { boundBoxOffset, { 0, 0, 0 } });

        height += beamLength;
    }

    return true;
}

bool MetalASupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, Direction direction,
    int32_t special, int32_t height, ImageId imageTemplate)
{
    supportTypeMember = kMetalSupportTypeRotated[EnumValue(supportTypeMember)][direction];
    placement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalASupportsPaintSetup(session, supportTypeMember, placement, special, height, imageTemplate);
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
bool MetalBSupportsPaintSetup(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, int32_t special, int32_t height,
    ImageId imageTemplate)
{
    if (!(session.Flags & PaintSessionFlags::PassedSurface))
    {
        return false; // AND
    }

    if (session.ViewFlags & VIEWPORT_FLAG_HIDE_SUPPORTS)
    {
        if (session.ViewFlags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS)
        {
            return false;
        }
        imageTemplate = ImageId(0).WithTransparency(FilterPaletteID::PaletteDarken1);
    }

    const uint8_t segment = EnumValue(placement);
    auto supportType = EnumValue(supportTypeMember);
    SupportHeight* supportSegments = session.SupportSegments;
    uint16_t unk9E3294 = 0xFFFF;
    int32_t baseHeight = height;

    if (height < supportSegments[segment].height)
    {
        unk9E3294 = height;

        baseHeight -= supportTypeToHeight[supportType];
        if (baseHeight < 0)
        {
            return false; // AND
        }

        uint16_t baseIndex = session.CurrentRotation * 2;

        uint8_t ebp = _97AF32[baseIndex + segment * 8];
        if (baseHeight <= supportSegments[ebp].height)
        {
            baseIndex += kMetalSupportSkip; // 9 segments, 4 directions, 2 values
            uint8_t ebp2 = _97AF32[baseIndex + segment * 8];
            if (baseHeight <= supportSegments[ebp2].height)
            {
                baseIndex += kMetalSupportSkip;
                uint8_t ebp3 = _97AF32[baseIndex + segment * 8];
                if (baseHeight <= supportSegments[ebp3].height)
                {
                    baseIndex += kMetalSupportSkip;
                    uint8_t ebp4 = _97AF32[baseIndex + segment * 8];
                    if (baseHeight <= supportSegments[ebp4].height)
                    {
                        return true; // STC
                    }
                }
            }
        }

        ebp = _97AF32[baseIndex + segment * 8 + 1];
        if (ebp >= 4)
        {
            return true; // STC
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(_metalSupportTypeToCrossbeamImages[supportType][ebp]),
            { SupportBoundBoxes[segment] + Loc97B052[ebp], baseHeight }, { _97B062[ebp], 1 });
    }

    int32_t si = baseHeight;

    if ((supportSegments[segment].slope & kTileSlopeAboveTrackOrScenery) || (baseHeight - supportSegments[segment].height < 6)
        || (kSupportBasesAndBeams[supportType].beamA == 0))
    {
        baseHeight = supportSegments[segment].height;
    }
    else
    {
        uint32_t imageOffset = kMetalSupportsSlopeImageOffsetMap[supportSegments[segment].slope & kTileSlopeMask];
        uint32_t imageId = kSupportBasesAndBeams[supportType].base + imageOffset;

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[segment], supportSegments[segment].height },
            { 0, 0, 5 });

        baseHeight = supportSegments[segment].height + 6;
    }

    int16_t heightDiff = Floor2(baseHeight + 16, 16);
    if (heightDiff > si)
    {
        heightDiff = si;
    }

    heightDiff -= baseHeight;
    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(kSupportBasesAndBeams[supportType].beamA + (heightDiff - 1)),
            { SupportBoundBoxes[segment], baseHeight }, { 0, 0, heightDiff - 1 });
    }

    baseHeight += heightDiff;

    int16_t endHeight;

    int32_t i = 1;
    while (true)
    {
        endHeight = baseHeight + 16;
        if (endHeight > si)
        {
            endHeight = si;
        }

        int16_t beamLength = endHeight - baseHeight;

        if (beamLength <= 0)
        {
            break;
        }

        uint32_t imageId = kSupportBasesAndBeams[supportType].beamA + (beamLength - 1);

        if (i % 4 == 0)
        {
            // Each fourth run, draw a special image
            if (beamLength == 16)
            {
                imageId += 1;
            }
        }

        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, beamLength - 1 });

        baseHeight += beamLength;
        i++;
    }

    supportSegments[segment].height = unk9E3294;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    if (special != 0)
    {
        baseHeight = height;
        const auto si2 = height + special;
        while (true)
        {
            endHeight = baseHeight + 16;
            if (endHeight > si2)
            {
                endHeight = si2;
            }

            int16_t beamLength = endHeight - baseHeight;
            if (beamLength <= 0)
            {
                break;
            }

            uint32_t imageId = kSupportBasesAndBeams[supportType].beamA + (beamLength - 1);
            PaintAddImageAsParent(
                session, imageTemplate.WithIndex(imageId), { SupportBoundBoxes[segment], baseHeight },
                { { SupportBoundBoxes[segment], height }, { 0, 0, 0 } });
            baseHeight += beamLength;
        }
    }

    return false; // AND
}

bool MetalBSupportsPaintSetupRotated(
    PaintSession& session, MetalSupportType supportTypeMember, MetalSupportPlace placement, Direction direction,
    int32_t special, int32_t height, ImageId imageTemplate)
{
    supportTypeMember = kMetalSupportTypeRotated[EnumValue(supportTypeMember)][direction];
    placement = kMetalSupportPlacementRotated[EnumValue(placement)][direction];
    return MetalBSupportsPaintSetup(session, supportTypeMember, placement, special, height, imageTemplate);
}

void DrawSupportsSideBySide(
    PaintSession& session, Direction direction, uint16_t height, ImageId colour, MetalSupportType type, int32_t special)
{
    type = kMetalSupportTypeRotated[EnumValue(type)][direction];

    if (direction & 1)
    {
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::TopRightSide, special, height, colour);
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::BottomLeftSide, special, height, colour);
    }
    else
    {
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::TopLeftSide, special, height, colour);
        MetalASupportsPaintSetup(session, type, MetalSupportPlace::BottomRightSide, special, height, colour);
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
    PaintSession& session, MetalSupportPlace supportPlace, bool isSloped, int32_t height, ImageId imageTemplate,
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
            { SupportBoundBoxes[segment].x, SupportBoundBoxes[segment].y, baseHeight }, { 0, 0, 5 });
        baseHeight += 6;
    }

    // si = height
    // dx = baseHeight

    int16_t heightDiff = Floor2(baseHeight + 16, 16);
    if (heightDiff > height)
    {
        heightDiff = height;
    }

    heightDiff -= baseHeight;

    if (heightDiff > 0)
    {
        PaintAddImageAsParent(
            session, imageTemplate.WithIndex(pathPaintInfo.BridgeImageId + 20 + (heightDiff - 1)),
            { SupportBoundBoxes[segment], baseHeight }, { 0, 0, heightDiff - 1 });
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
                { SupportBoundBoxes[segment], baseHeight }, { 0, 0, (z - 1) });

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
            session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, (z - 1) });

        baseHeight += z;
    }

    // Loc6A34D8
    supportSegments[segment].height = 0xFFFF;
    supportSegments[segment].slope = kTileSlopeAboveTrackOrScenery;

    if (isSloped)
    {
        int16_t si = baseHeight + COORDS_Z_STEP;

        while (true)
        {
            int16_t z = baseHeight + (2 * COORDS_Z_STEP);
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
                session, imageTemplate.WithIndex(imageIndex), { SupportBoundBoxes[segment], baseHeight }, { 0, 0, 0 });

            baseHeight += z;
        }
    }

    return false; // AND
}
