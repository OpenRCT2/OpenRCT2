/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <vector>
#include "../common.h"
#include "../object/Object.h"

interface   IObjectRepository;
class       Object;
struct      ObjectRepositoryItem;

interface IObjectManager
{
    virtual ~IObjectManager() { }

    virtual Object *                        GetLoadedObject(size_t index) abstract;
    virtual Object *                        GetLoadedObject(int32_t objectType, size_t index) abstract;
    virtual Object *                        GetLoadedObject(const rct_object_entry * entry) abstract;
    virtual uint8_t                           GetLoadedObjectEntryIndex(const Object * object) abstract;
    virtual std::vector<rct_object_entry>   GetInvalidObjects(const rct_object_entry * entries) abstract;

    virtual Object *    LoadObject(const rct_object_entry * entry) abstract;
    virtual void        LoadObjects(const rct_object_entry * entries, size_t count) abstract;
    virtual void        LoadDefaultObjects() abstract;
    virtual void        UnloadObjects(const rct_object_entry * entries, size_t count) abstract;
    virtual void        UnloadAll() abstract;

    virtual void ResetObjects() abstract;

    virtual std::vector<const ObjectRepositoryItem *> GetPackableObjects() abstract;
};

std::unique_ptr<IObjectManager> CreateObjectManager(std::shared_ptr<IObjectRepository> objectRepository);

void *        object_manager_get_loaded_object_by_index(size_t index);
void *        object_manager_get_loaded_object(const rct_object_entry * entry);
uint8_t         object_manager_get_loaded_object_entry_index(const void * loadedObject);
void *        object_manager_load_object(const rct_object_entry * entry);
void          object_manager_unload_objects(const rct_object_entry * entries, size_t count);
void          object_manager_unload_all_objects();
rct_string_id object_manager_get_source_game_string(const rct_object_entry * entry);
