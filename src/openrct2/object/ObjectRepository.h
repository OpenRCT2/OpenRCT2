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

#include <memory>
#include <vector>
#include "../common.h"
#include "../object/Object.h"
#include "../ride/Ride.h"

interface   IStream;
class       Object;
namespace OpenRCT2
{
    interface IPlatformEnvironment;
}

namespace OpenRCT2::Localisation
{
    class LocalisationService;
}

struct rct_drawpixelinfo;

struct ObjectRepositoryItem
{
    size_t             Id;
    rct_object_entry   ObjectEntry;
    std::string        Path;
    std::string        Name;
    Object *           LoadedObject{};
    struct
    {
        uint8   RideFlags;
        uint8   RideCategory[MAX_CATEGORIES_PER_RIDE];
        uint8   RideType[MAX_RIDE_TYPES_PER_RIDE_ENTRY];
        uint8   RideGroupIndex;
    } RideInfo;
    struct
    {
        std::vector<rct_object_entry> Entries;
    } SceneryGroupInfo;
};

interface IObjectRepository
{
    virtual ~IObjectRepository() = default;

    virtual void                            LoadOrConstruct(sint32 language) abstract;
    virtual void                            Construct(sint32 language) abstract;
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

std::unique_ptr<IObjectRepository> CreateObjectRepository(const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);

bool IsObjectCustom(const ObjectRepositoryItem * object);

size_t                          object_repository_get_items_count();
const ObjectRepositoryItem *    object_repository_get_items();
const ObjectRepositoryItem *    object_repository_find_object_by_entry(const rct_object_entry * entry);
const ObjectRepositoryItem *    object_repository_find_object_by_name(const char * name);
void *                          object_repository_load_object(const rct_object_entry * objectEntry);

void            object_delete(void * object);
void            object_draw_preview(const void * object, rct_drawpixelinfo * dpi, sint32 width, sint32 height);
