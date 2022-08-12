/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
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

    virtual Object* GetLoadedObject(ObjectType objectType, size_t index) abstract;
    virtual Object* GetLoadedObject(const ObjectEntryDescriptor& entry) abstract;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) abstract;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) abstract;
    virtual ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) abstract;
    virtual ObjectList GetLoadedObjects() abstract;

    virtual Object* LoadObject(std::string_view identifier) abstract;
    virtual Object* LoadObject(const rct_object_entry* entry) abstract;
    virtual Object* LoadObject(const ObjectEntryDescriptor& descriptor) abstract;
    virtual void LoadObjects(const ObjectList& entries) abstract;
    virtual void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) abstract;
    virtual void UnloadAllTransient() abstract;
    virtual void UnloadAll() abstract;

    virtual void ResetObjects() abstract;

    virtual std::vector<const ObjectRepositoryItem*> GetPackableObjects() abstract;
    virtual const std::vector<ObjectEntryIndex>& GetAllRideEntries(uint8_t rideType) abstract;
};

[[nodiscard]] std::unique_ptr<IObjectManager> CreateObjectManager(IObjectRepository& objectRepository);

[[nodiscard]] Object* object_manager_get_loaded_object(const ObjectEntryDescriptor& entry);
[[nodiscard]] ObjectEntryIndex object_manager_get_loaded_object_entry_index(const Object* loadedObject);
[[nodiscard]] ObjectEntryIndex object_manager_get_loaded_object_entry_index(const ObjectEntryDescriptor& entry);
Object* object_manager_load_object(const rct_object_entry* entry);
void object_manager_unload_objects(const std::vector<ObjectEntryDescriptor>& entries);
void object_manager_unload_all_objects();
[[nodiscard]] StringId object_manager_get_source_game_string(const ObjectSourceGame sourceGame);
