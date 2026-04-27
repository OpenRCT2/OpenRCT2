/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Paint.Litter.h"

#include "../../SpriteIds.h"
#include "../../entity/Litter.h"
#include "../../paint/Paint.h"
#include "../../profiling/Profiling.h"

namespace OpenRCT2
{

    struct LitterSprite
    {
        uint16_t base_id;
        uint8_t direction_mask;
    };

    static constexpr LitterSprite kLitterSprites[] = {
        { SPR_LITTER_SICK, 0x1 },
        { SPR_LITTER_SICK_ALT, 0x1 },
        { SPR_LITTER_EMPTY_CAN, 0x1 },
        { SPR_LITTER_RUBBISH, 0x1 },
        { SPR_LITTER_EMPTY_BURGER_BOX, 0x1 },
        { SPR_LITTER_EMPTY_CUP, 0x1 },
        { SPR_LITTER_EMPTY_BOX, 0x1 },
        { SPR_LITTER_EMPTY_BOTTLE, 0x1 },
        { SPR_LITTER_EMPTY_BOWL_RED, 0x3 },
        { SPR_LITTER_EMPTY_DRINK_CART, 0x3 },
        { SPR_LITTER_EMPTY_JUICE_CUP, 0x3 },
        { SPR_LITTER_EMPTY_BOWL_BLUE, 0x3 },
    };

    void PaintLitter(PaintSession& session, const OpenRCT2::Litter& litter, int32_t imageDirection)
    {
        PROFILED_FUNCTION();

        auto& rt = session.rt;
        if (rt.zoom_level > ZoomLevel{ 0 })
            return;

        imageDirection >>= 3;

        imageDirection &= kLitterSprites[EnumValue(litter.SubType)].direction_mask;

        uint32_t image_id = imageDirection + kLitterSprites[EnumValue(litter.SubType)].base_id;

        PaintAddImageAsParent(session, ImageId(image_id), { 0, 0, litter.z }, { { -4, -4, litter.z + 4 }, { 5, 5, -1 } });
    }
} // namespace OpenRCT2
