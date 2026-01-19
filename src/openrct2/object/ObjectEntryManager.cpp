/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectEntryManager.h"

#include "../Context.h"
#include "Object.h"
#include "ObjectManager.h"

namespace OpenRCT2::ObjectManager
{
    const void* GetObjectEntry(ObjectType type, ObjectEntryIndex idx)
    {
        auto* object = GetContext()->GetObjectManager().GetLoadedObject(type, idx);
        if (object == nullptr)
        {
            return nullptr;
        }
        return object->GetLegacyData();
    }
} // namespace OpenRCT2::ObjectManager
