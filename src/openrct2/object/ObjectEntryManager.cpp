#include "ObjectEntryManager.h"

#include "../Context.h"
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
