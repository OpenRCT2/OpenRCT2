#include "ObjectTypes.h"

namespace OpenRCT2::ObjectManager
{
    const void* GetObjectEntry(ObjectType type, ObjectEntryIndex idx);

    template<typename T> const T* GetObjectEntry(ObjectEntryIndex idx)
    {
        return reinterpret_cast<const T*>(GetObjectEntry(T::kObjectType, idx));
    }
}
