/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "ObjectTypes.h"

namespace OpenRCT2::ObjectManager
{
    const void* GetObjectEntry(ObjectType type, ObjectEntryIndex idx);

    template<typename T>
    const T* GetObjectEntry(ObjectEntryIndex idx)
    {
        return reinterpret_cast<const T*>(GetObjectEntry(T::kObjectType, idx));
    }
} // namespace OpenRCT2::ObjectManager
