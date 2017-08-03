#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma once

#ifdef __cplusplus
    #include <vector>
#endif


#ifdef __cplusplus
extern "C"
{
#endif
    #include "../common.h"
    #include "../object.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus

interface   IObjectRepository;
class       Object;
struct      ObjectRepositoryItem;

interface IObjectManager
{
    virtual ~IObjectManager() { }

    virtual Object *                        GetLoadedObject(size_t index) abstract;
    virtual Object *                        GetLoadedObject(const rct_object_entry * entry) abstract;
    virtual uint8                           GetLoadedObjectEntryIndex(const Object * object) abstract;
    virtual std::vector<rct_object_entry>   GetInvalidObjects(const rct_object_entry * entries) abstract;

    virtual Object *    LoadObject(const rct_object_entry * entry) abstract;
    virtual bool        LoadObjects(const rct_object_entry * entries, size_t count) abstract;
    virtual void        UnloadObjects(const rct_object_entry * entries, size_t count) abstract;
    virtual void        UnloadAll() abstract;

    virtual void ResetObjects() abstract;

    virtual std::vector<const ObjectRepositoryItem *> GetPackableObjects() abstract;
};

IObjectManager * CreateObjectManager(IObjectRepository * objectRepository);
IObjectManager * GetObjectManager();

#endif

#ifdef __cplusplus
extern "C"
{
#endif

void *        object_manager_get_loaded_object_by_index(size_t index);
void *        object_manager_get_loaded_object(const rct_object_entry * entry);
uint8         object_manager_get_loaded_object_entry_index(const void * loadedObject);
void *        object_manager_load_object(const rct_object_entry * entry);
void          object_manager_unload_objects(const rct_object_entry * entries, size_t count);
void          object_manager_unload_all_objects();
rct_string_id object_manager_get_source_game_string(const rct_object_entry * entry);

#ifdef __cplusplus
}
#endif
