/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ImageId.hpp"

#include "../SpriteIds.h"

ImageCatalogue ImageId::GetCatalogue() const
{
    auto index = GetIndex();
    if (index >= SPR_TEMP_BEGIN && index < SPR_TEMP_END)
    {
        return ImageCatalogue::temporary;
    }
    if (index < SPR_RCTC_G1_END)
    {
        return ImageCatalogue::g1;
    }
    if (index < SPR_G2_END)
    {
        return ImageCatalogue::g2;
    }
    if (index < SPR_CSG_END)
    {
        return ImageCatalogue::csg;
    }
    if (index < SPR_IMAGE_LIST_END)
    {
        return ImageCatalogue::object;
    }
    return ImageCatalogue::unknown;
}
