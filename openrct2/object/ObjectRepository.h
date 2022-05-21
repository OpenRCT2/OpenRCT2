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
#include "RideObject.h"

#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct IStream;
}

class Object;
namespace OpenRCT2
{
    struct IPlatformEnvironment;
}

namespace OpenRCT2::Localisation
{
    class LocalisationService;
}

struct rct_drawpixelinfo;

struct ObjectRepositoryItem
{
    size_t Id;
    ObjectType Type;
    ObjectGeneration Generation;
    std::string Identifier; // e.g. rct2.c3d
    rct_object_entry ObjectEntry;
    std::string Path;
    std::string Name;
    std::vector<std::string> Authors;
    std::vector<ObjectSourceGame> Sources;
    std::shared_ptr<Object> LoadedObject{};
    struct
    {
        uint8_t RideFlags;
        uint8_t RideCategory[RCT2::ObjectLimits::MaxCategoriesPerRide];
        uint8_t RideType[RCT2::ObjectLimits::MaxRideTypesPerRideEntry];
    } RideInfo;
    struct
    {
        std::vector<ObjectEntryDescriptor> Entries;
    } SceneryGroupInfo;
    struct
    {
        uint8_t Flags{};
    } FootpathSurfaceInfo;

    [[nodiscard]] ObjectSourceGame GetFirstSourceGame() const
    {
        if (Sources.empty())
            return ObjectSourceGame::Custom;

        return static_cast<ObjectSourceGame>(Sources[0]);
    }
};

struct IObjectRepository
{
    virtual ~IObjectRepository() = default;

    virtual void LoadOrConstruct(int32_t language) abstract;
    virtual void Construct(int32_t language) abstract;
    [[nodiscard]] virtual size_t GetNumObjects() const abstract;
    [[nodiscard]] virtual const ObjectRepositoryItem* GetObjects() const abstract;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObjectLegacy(std::string_view legacyIdentifier) const abstract;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(std::string_view identifier) const abstract;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(const rct_object_entry* objectEntry) const abstract;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(const ObjectEntryDescriptor& oed) const abstract;

    [[nodiscard]] virtual std::unique_ptr<Object> LoadObject(const ObjectRepositoryItem* ori) abstract;
    virtual void RegisterLoadedObject(const ObjectRepositoryItem* ori, std::unique_ptr<Object>&& object) abstract;
    virtual void UnregisterLoadedObject(const ObjectRepositoryItem* ori, Object* object) abstract;

    virtual void AddObject(const rct_object_entry* objectEntry, const void* data, size_t dataSize) abstract;
    virtual void AddObjectFromFile(
        ObjectGeneration generation, std::string_view objectName, const void* data, size_t dataSize) abstract;

    virtual void ExportPackedObject(OpenRCT2::IStream* stream) abstract;
};

[[nodiscard]] std::unique_ptr<IObjectRepository> CreateObjectRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);

[[nodiscard]] bool IsObjectCustom(const ObjectRepositoryItem* object);

[[nodiscard]] size_t object_repository_get_items_count();
[[nodiscard]] const ObjectRepositoryItem* object_repository_get_items();
[[nodiscard]] const ObjectRepositoryItem* object_repository_find_object_by_entry(const rct_object_entry* entry);
[[nodiscard]] const ObjectRepositoryItem* object_repository_find_object_by_name(const char* name);
[[nodiscard]] std::unique_ptr<Object> object_repository_load_object(const rct_object_entry* objectEntry);
