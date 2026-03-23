/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Balloon.h"

#include "../../drawing/ImageId.hpp"
#include "../../entity/Balloon.h"
#include "../../profiling/Profiling.h"
#include "../../profiling/ProfilingMacros.hpp"
#include "../Paint.h"

using namespace OpenRCT2;

void PaintBalloon(PaintSession& session, const Balloon& balloon, int32_t imageDirection)
{
    PROFILED_FUNCTION();

    uint32_t imageId = 22651 + (balloon.frame & 7);
    if (balloon.popped != 0)
    {
        imageId += 8;
    }

    auto image = ImageId(imageId, balloon.colour);
    PaintAddImageAsParent(session, image, { 0, 0, balloon.z }, { 1, 1, 0 });
}
