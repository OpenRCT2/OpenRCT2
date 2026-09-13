/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.JumpingFountain.h"

#include "../../SpriteIds.h"
#include "../../entity/JumpingFountain.h"
#include "../../profiling/Profiling.h"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintJumpingFountain(PaintSession& session, const JumpingFountain& jumpingFountain, int32_t imageDirection)
{
    PROFILED_FUNCTION();

    auto& rt = session.rt;
    if (rt.zoom_level > ZoomLevel{ 0 })
    {
        return;
    }

    uint16_t height = jumpingFountain.z + 6;
    imageDirection = imageDirection / 8;

    // Fountain is firing anti clockwise
    bool reversed = jumpingFountain.fountainFlags.has(FountainFlag::direction);
    // Fountain rotation
    bool rotated = (jumpingFountain.orientation / 16) & 1;
    bool isAntiClockwise = (imageDirection / 2) & 1; // Clockwise or Anti-clockwise

    // These cancel each other out
    if (reversed != rotated)
    {
        isAntiClockwise = !isAntiClockwise;
    }

    uint32_t baseImageId = (jumpingFountain.FountainType == JumpingFountainType::Snow) ? SPR_JUMPING_FOUNTAIN_SNOW
                                                                                       : SPR_JUMPING_FOUNTAIN_WATER;
    auto imageId = ImageId(baseImageId + imageDirection * 16 + jumpingFountain.frame);
    constexpr std::array kAntiClockWiseBoundingBoxes = {
        CoordsXY{ -kCoordsXYStep, -3 },
        CoordsXY{ 0, -3 },
    };
    constexpr std::array kClockWiseBoundingBoxes = {
        CoordsXY{ -kCoordsXYStep, 3 },
        CoordsXY{ 0, 3 },
    };

    auto bb = isAntiClockwise ? kAntiClockWiseBoundingBoxes : kClockWiseBoundingBoxes;

    PaintAddImageAsParentRotated(
        session, imageDirection, imageId, { 0, 0, height }, { { bb[imageDirection & 1], height }, { 32, 1, 3 } });
}
