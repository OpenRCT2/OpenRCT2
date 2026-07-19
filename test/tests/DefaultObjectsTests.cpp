/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <gtest/gtest.h>
#include <openrct2/object/DefaultObjects.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/StationObject.h>

using namespace OpenRCT2;

class DefaultObjectsObjectManager final : public IObjectManager
{
public:
    ObjectEntryIndex PlainStationIndex = kObjectEntryIndexNull;
    std::array<StationObject*, kMaxStationObjects> StationObjects{};

    Object* GetLoadedObject(ObjectType objectType, size_t index) override
    {
        if (objectType != ObjectType::station || index >= StationObjects.size())
        {
            return nullptr;
        }

        return StationObjects[index];
    }

    Object* GetLoadedObject(const ObjectEntryDescriptor& entry) override
    {
        return nullptr;
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(std::string_view identifier) override
    {
        if (identifier == "rct2.station.plain")
        {
            return PlainStationIndex;
        }

        return kObjectEntryIndexNull;
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(const ObjectEntryDescriptor& descriptor) override
    {
        return kObjectEntryIndexNull;
    }

    ObjectEntryIndex GetLoadedObjectEntryIndex(const Object* object) override
    {
        return kObjectEntryIndexNull;
    }

    ObjectList GetLoadedObjects() override
    {
        return {};
    }

    std::unique_ptr<Object> LoadTempObject(std::string_view identifier, bool loadImages) override
    {
        return nullptr;
    }

    Object* LoadObject(std::string_view identifier) override
    {
        return nullptr;
    }

    Object* LoadObject(const RCTObjectEntry* entry) override
    {
        return nullptr;
    }

    Object* LoadObject(const ObjectEntryDescriptor& descriptor) override
    {
        return nullptr;
    }

    Object* LoadObject(const ObjectEntryDescriptor& descriptor, ObjectEntryIndex slot) override
    {
        return nullptr;
    }

    Object* LoadRepositoryItem(const ObjectRepositoryItem& ori) override
    {
        return nullptr;
    }

    void LoadObjects(const ObjectList& entries, bool reportProgress = false) override
    {
    }

    void UnloadObjects(const std::vector<ObjectEntryDescriptor>& entries) override
    {
    }

    void UnloadAllTransient() override
    {
    }

    void UnloadAll() override
    {
    }

    void ResetObjects() override
    {
    }

    std::vector<const ObjectRepositoryItem*> GetPackableObjects() override
    {
        return {};
    }

    const std::vector<ObjectEntryIndex>& GetAllRideEntries(ride_type_t rideType) override
    {
        return _rideEntries;
    }

private:
    std::vector<ObjectEntryIndex> _rideEntries;
};

TEST(DefaultObjectsTests, DefaultStationObjectPrefersPlainStation)
{
    DefaultObjectsObjectManager objectManager;
    objectManager.PlainStationIndex = 4;

    StationObject stationObject;
    objectManager.StationObjects[1] = &stationObject;

    ASSERT_EQ(GetDefaultStationObject(objectManager), 4);
}

TEST(DefaultObjectsTests, DefaultStationObjectFallsBackToFirstLoadedStation)
{
    DefaultObjectsObjectManager objectManager;

    StationObject stationObject;
    objectManager.StationObjects[3] = &stationObject;

    ASSERT_EQ(GetDefaultStationObject(objectManager), 3);
}

TEST(DefaultObjectsTests, DefaultStationObjectReturnsNullWhenNoStationIsLoaded)
{
    DefaultObjectsObjectManager objectManager;

    ASSERT_EQ(GetDefaultStationObject(objectManager), kObjectEntryIndexNull);
}
