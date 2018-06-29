/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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
        uint8_t   RideFlags;
        uint8_t   RideCategory[MAX_CATEGORIES_PER_RIDE];
        uint8_t   RideType[MAX_RIDE_TYPES_PER_RIDE_ENTRY];
        uint8_t   RideGroupIndex;
    } RideInfo;
    struct
    {
        std::vector<rct_object_entry> Entries;
    } SceneryGroupInfo;
};

interface IObjectRepository
{
    virtual ~IObjectRepository() = default;

    virtual void                            LoadOrConstruct(int32_t language) abstract;
    virtual void                            Construct(int32_t language) abstract;
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
void            object_draw_preview(const void * object, rct_drawpixelinfo * dpi, int32_t width, int32_t height);
