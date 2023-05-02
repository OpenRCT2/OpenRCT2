/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectTypes.h"

#include <algorithm>

bool ObjectTypeIsTransient(ObjectType type)
{
    return std::find(TransientObjectTypes.begin(), TransientObjectTypes.end(), type) != std::end(TransientObjectTypes);
}

bool ObjectTypeIsIntransient(ObjectType type)
{
    return std::find(IntransientObjectTypes.begin(), IntransientObjectTypes.end(), type) != std::end(IntransientObjectTypes);
}
