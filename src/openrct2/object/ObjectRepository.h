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

#include "../common.h"

#ifdef __cplusplus
extern "C"
{
#endif
    #include "../object.h"
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
    interface   IPlatformEnvironment;
    interface   IStream;
    class       Object;
#else
    typedef struct Object Object;
#endif

typedef struct ObjectRepositoryItem
{
    size_t             Id;
    rct_object_entry   ObjectEntry;
    utf8 *             Path;
    utf8 *             Name;
    Object *           LoadedObject;
    union
    {
        struct
        {
            uint8   RideFlags;
            uint8   RideCategory[2];
            uint8   RideType[3];
        };
        struct
        {
            uint16             NumThemeObjects;
            rct_object_entry * ThemeObjects;
        };
    };
} ObjectRepositoryItem;

#ifdef __cplusplus

interface IObjectRepository
{
    virtual ~IObjectRepository() { }

    virtual void                            LoadOrConstruct() abstract;
    virtual void                            Construct() abstract;
    virtual size_t                          GetNumObjects() const abstract;
    virtual const ObjectRepositoryItem *    GetObjects() const abstract;
    virtual const ObjectRepositoryItem *    FindObject(const utf8 * name) const abstract;
    virtual const ObjectRepositoryItem *    FindObject(const rct_object_entry * objectEntry) const abstract;

    virtual Object *                        LoadObject(const ObjectRepositoryItem * ori) abstract;
    virtual void                            RegisterLoadedObject(const ObjectRepositoryItem * ori, Object * object) abstract;
    virtual void                            UnregisterLoadedObject(const ObjectRepositoryItem * ori, Object * object) abstract;

    virtual void                            AddObject(const rct_object_entry * objectEntry,
                                                      const void * data,
                                                      size_t dataSize) abstract;

    virtual void                            ExportPackedObject(IStream * stream) abstract;
    virtual void                            WritePackedObjects(IStream * stream, std::vector<const ObjectRepositoryItem *> &objects) abstract;
};

IObjectRepository * CreateObjectRepository(IPlatformEnvironment * env);
IObjectRepository * GetObjectRepository();

bool IsObjectCustom(const ObjectRepositoryItem * object);

#endif

#ifdef __cplusplus
extern "C"
{
#endif

size_t                          object_repository_get_items_count();
const ObjectRepositoryItem *    object_repository_get_items();
const ObjectRepositoryItem *    object_repository_find_object_by_entry(const rct_object_entry * entry);
const ObjectRepositoryItem *    object_repository_find_object_by_name(const char * name);
void *                          object_repository_load_object(const rct_object_entry * objectEntry);

void            object_delete(void * object);
const utf8 *    object_get_description(const void * object);
const utf8 *    object_get_capacity(const void * object);
void            object_draw_preview(const void * object, rct_drawpixelinfo * dpi, sint32 width, sint32 height);

#ifdef __cplusplus
}
#endif

enum ORI_RIDE_FLAG
{
    ORI_RIDE_FLAG_SEPARATE = 1 << 0,
};
