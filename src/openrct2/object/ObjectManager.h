/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../object/Object.h"

#include <memory>
#include <vector>

struct IObjectRepository;
class Object;
class ObjectList;
struct ObjectRepositoryItem;

struct IObjectManager
{
    virtual ~IObjectManager()
    {
    }

    virtual Object* GetLoadedObject(ObjectType objectType, size_t index) = 0;
    template<typename TClass>
    TClass* GetLoadedObject(size_t index)
    {
        return static_cast<TClass*>(GetLoadedObject(TClass::kObjectType, index));
    }
    virtual Object* GetLoadedObject(const ObjectEntryDescriptor& entry) = 0;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) = 0;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) = 0;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) = 0;
    virtual ObjectList GetLoadedObjects() = 0;

    virtual Object* LoadObject(std::string_view identifier) = 0;
    virtual Object* LoadObject(const RCTObjectEntry* entry) = 0;
    virtual Object* LoadObject(const ObjectEntryDescriptor& descriptor) = 0;
    virtual Object* LoadObject(const ObjectEntryDescriptor& descriptor, ObjectEntryIndex slot) = 0;
    virtual void LoadObjects(const ObjectList& entries, const bool reportProgress = false) = 0;
    virtual void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) = 0;
    virtual void UnloadAllTransient() = 0;
    virtual void UnloadAll() = 0;

    virtual void ResetObjects() = 0;

    virtual std::vector<const ObjectRepositoryItem*> GetPackableObjects() = 0;
    virtual const std::vector<ObjectEntryIndex>& GetAllRideEntries(ride_type_t rideType) = 0;
};

[[nodiscard]] std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository);

[[nodiscard]] Object* ObjectManagerGetLoadedObject(const ObjectEntryDescriptor& entry);
[[nodiscard]] ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const Object* loadedObject);
[[nodiscard]] ObjectEntryIndex ObjectManagerGetLoadedObjectEntryIndex(const ObjectEntryDescriptor& entry);
Object* ObjectManagerLoadObject(const RCTObjectEntry* entry);
void ObjectManagerUnloadObjects(const std::vector<ObjectEntryDescriptor>& entries);
void ObjectManagerUnloadAllObjects();
[[nodiscard]] StringId ObjectManagerGetSourceGameString(const ObjectSourceGame sourceGame);
