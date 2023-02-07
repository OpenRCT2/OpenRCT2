#include "ObjectEntryManager.h"

#include "../Context.h"
#include "ObjectManager.h"

namespace OpenRCT2::ObjectManager
{
    const void* OpenRCT2::ObjectManager::GetObjectEntry(ObjectType type, ObjectEntryIndex idx)
    {
        auto* object = GetContext()->GetObjectManager().GetLoadedObject(type,idx);
        return object->GetLegacyData();
    }
} // namespace OpenRCT2::ObjectManager
