#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
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

#include "../common.h"

#ifdef __cplusplus
extern "C"
{
#endif
    #include "../object.h"
#ifdef __cplusplus
}
#endif

typedef struct ObjectRepositoryItem
{
    rct_object_entry   ObjectEntry;
    utf8 *             Path;
    uint32             NumImages;
    utf8 *             Name;
    size_t             ChunkSize;
    uint16             NumRequiredObjects;
    rct_object_entry * RequiredObjects;
    union
    {
        struct
        {
            uint16             NumThemeObjects;
            rct_object_entry * ThemeObjects;
        };
        struct
        {
            uint8   RideFlags;
            uint8   RideCategory[2];
            uint8   RideType[3];
        };
    };
} ObjectRepositoryItem;

#ifdef __cplusplus

class Object;

interface IObjectRepository
{
    virtual ~IObjectRepository() { }

    virtual const size_t                    GetNumObjects() const abstract;
    virtual const ObjectRepositoryItem *    GetObjects() const abstract;
    virtual const ObjectRepositoryItem *    FindObject(const utf8 * name) const abstract;
    virtual const ObjectRepositoryItem *    FindObject(const rct_object_entry * objectEntry) const abstract;

    virtual Object *                        LoadObject(const rct_object_entry * objectEntry) abstract;
    virtual void                            AddObject(const rct_object_entry * objectEntry,
                                                      const void * data,
                                                      size_t dataSize) abstract;
};

IObjectRepository * GetObjectRepository();

#else

size_t                          object_repository_get_items_count();
const ObjectRepositoryItem *    object_repository_get_items();
const ObjectRepositoryItem *    object_repository_find_object_by_entry(const rct_object_entry * entry);
void *                          object_repository_load_object(const rct_object_entry * objectEntry);
void                            object_repository_unload(size_t itemIndex);

void            object_delete(void * object);
const utf8 *    object_get_description(const void * object);
void            object_draw_preview(const void * object, rct_drawpixelinfo * dpi);

#endif
