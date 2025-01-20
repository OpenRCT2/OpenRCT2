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
#include "RideObject.h"

#include <memory>
#include <vector>

namespace OpenRCT2
{
    struct IStream;
    struct IPlatformEnvironment;
} // namespace OpenRCT2

class Object;

namespace OpenRCT2::Localisation
{
    class LocalisationService;
}

struct DrawPixelInfo;

enum ObjectItemFlags : uint8_t
{
    IsCompatibilityObject = 1,
};

struct ObjectRepositoryItem
{
    size_t Id;
    ObjectType Type;
    ObjectGeneration Generation;
    std::string Identifier; // e.g. rct2.c3d
    RCTObjectEntry ObjectEntry;
    std::string Path;
    std::string Name;
    ObjectVersion Version;
    std::vector<std::string> Authors;
    std::vector<ObjectSourceGame> Sources;
    uint8_t Flags{};
    std::shared_ptr<Object> LoadedObject{};
    struct
    {
        uint8_t RideFlags;
        uint8_t RideCategory[OpenRCT2::RCT2::ObjectLimits::kMaxCategoriesPerRide];
        ride_type_t RideType[OpenRCT2::RCT2::ObjectLimits::kMaxRideTypesPerRideEntry];
    } RideInfo;
    struct
    {
        std::vector<ObjectEntryDescriptor> Entries;
    } SceneryGroupInfo;
    struct
    {
        uint8_t Flags{};
    } FootpathSurfaceInfo;
    struct
    {
        uint8_t PeepType{};
    } PeepAnimationsInfo;

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

    virtual void LoadOrConstruct(int32_t language) = 0;
    virtual void Construct(int32_t language) = 0;
    [[nodiscard]] virtual size_t GetNumObjects() const = 0;
    [[nodiscard]] virtual const ObjectRepositoryItem* GetObjects() const = 0;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObjectLegacy(std::string_view legacyIdentifier) const = 0;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(std::string_view identifier) const = 0;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(const RCTObjectEntry* objectEntry) const = 0;
    [[nodiscard]] virtual const ObjectRepositoryItem* FindObject(const ObjectEntryDescriptor& oed) const = 0;

    [[nodiscard]] virtual std::unique_ptr<Object> LoadObject(const ObjectRepositoryItem* ori) = 0;
    virtual void RegisterLoadedObject(const ObjectRepositoryItem* ori, std::unique_ptr<Object>&& object) = 0;
    virtual void UnregisterLoadedObject(const ObjectRepositoryItem* ori, Object* object) = 0;

    virtual void AddObject(const RCTObjectEntry* objectEntry, const void* data, size_t dataSize) = 0;
    virtual void AddObjectFromFile(ObjectGeneration generation, std::string_view objectName, const void* data, size_t dataSize)
        = 0;

    virtual void ExportPackedObject(OpenRCT2::IStream* stream) = 0;
};

[[nodiscard]] std::unique_ptr<IObjectRepository> CreateObjectRepository(
    const std::shared_ptr<OpenRCT2::IPlatformEnvironment>& env);

[[nodiscard]] bool IsObjectCustom(const ObjectRepositoryItem* object);

[[nodiscard]] size_t ObjectRepositoryGetItemsCount();
[[nodiscard]] const ObjectRepositoryItem* ObjectRepositoryGetItems();
[[nodiscard]] const ObjectRepositoryItem* ObjectRepositoryFindObjectByEntry(const RCTObjectEntry* entry);
[[nodiscard]] const ObjectRepositoryItem* ObjectRepositoryFindObjectByName(const char* name);
[[nodiscard]] std::unique_ptr<Object> ObjectRepositoryLoadObject(const RCTObjectEntry* objectEntry);
