/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Duck.h"

#include "../../entity/Duck.h"
#include "../../paint/Paint.h"
#include "../../profiling/Profiling.h"

namespace OpenRCT2
{
    void PaintDuck(PaintSession& session, const Duck& duck, int32_t imageDirection)
    {
        PROFILED_FUNCTION();

        auto& rt = session.rt;
        if (rt.zoom_level > ZoomLevel{ 1 })
            return;

        uint32_t imageId = duck.GetFrameImage(imageDirection);
        if (imageId != 0)
        {
            paintAddImageAsParent(session, ImageId(imageId), { 0, 0, duck.z }, { 1, 1, 0 });
        }
    }
} // namespace OpenRCT2
