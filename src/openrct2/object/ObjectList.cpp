/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectList.h"

#include "../Context.h"
#include "../Game.h"
#include "../core/EnumUtils.hpp"
#include "../core/SawyerCoding.h"
#include "../object/Object.h"
#include "ObjectLimits.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

#include <array>
#include <cstring>

// 0x0098DA00
static constexpr std::array<int32_t, EnumValue(ObjectType::Count)> kObjectEntryGroupCounts = {
    kMaxRideObjects,         // rides
    kMaxSmallSceneryObjects, // small scenery
    kMaxLargeSceneryObjects, // large scenery
    kMaxWallSceneryObjects,  // walls
    kMaxBannerObjects,       // banners
    kMaxPathObjects,         // paths
    kMaxPathAdditionObjects, // path additions
    kMaxSceneryGroupObjects, // scenery sets
    kMaxParkEntranceObjects, // park entrance
    kMaxWaterObjects,        // water
    kMaxScenarioTextObjects, // scenario text
    kMaxTerrainSurfaceObjects, kMaxTerrainEdgeObjects,     kMaxStationObjects,
    kMaxMusicObjects,          kMaxFootpathSurfaceObjects, kMaxFootpathRailingsObjects,
    kMaxAudioObjects,          kMaxPeepNamesObjects,       kMaxPeepAnimationsObjects,
};
static_assert(std::size(kObjectEntryGroupCounts) == EnumValue(ObjectType::Count));

size_t getObjectEntryGroupCount(ObjectType objectType)
{
    return kObjectEntryGroupCounts[EnumValue(objectType)];
}

size_t getObjectTypeLimit(ObjectType type)
{
    auto index = EnumValue(type);
    if (index >= EnumValue(ObjectType::Count))
        return 0;
    return static_cast<size_t>(kObjectEntryGroupCounts[index]);
}

ObjectList::const_iterator::const_iterator(const ObjectList* parent, bool end)
{
    _parent = parent;
    _subList = _parent->_subLists.size();
    _index = 0;
}

void ObjectList::const_iterator::MoveToNextEntry()
{
    do
    {
        if (_subList < _parent->_subLists.size())
        {
            auto subListSize = _parent->_subLists[_subList].size();
            if (_index < subListSize)
            {
                _index++;
                if (_index == subListSize)
                {
                    _subList++;
                    _index = 0;
                }
            }
        }
        else
        {
            break;
        }
    } while (!_parent->_subLists[_subList][_index].HasValue());
}

ObjectList::const_iterator& ObjectList::const_iterator::operator++()
{
    MoveToNextEntry();
    return *this;
}

ObjectList::const_iterator ObjectList::const_iterator::operator++(int)
{
    return *this;
}

const ObjectEntryDescriptor& ObjectList::const_iterator::operator*()
{
    return _parent->_subLists[_subList][_index];
}

bool ObjectList::const_iterator::operator==(const_iterator& rhs)
{
    return _parent == rhs._parent && _subList == rhs._subList && _index == rhs._index;
}

bool ObjectList::const_iterator::operator!=(const_iterator& rhs)
{
    return !(*this == rhs);
}

ObjectList::const_iterator ObjectList::begin() const
{
    return const_iterator(this, false);
}

ObjectList::const_iterator ObjectList::end() const
{
    return const_iterator(this, true);
}

std::vector<ObjectEntryDescriptor>& ObjectList::GetList(ObjectType type)
{
    auto index = static_cast<size_t>(type);
    while (_subLists.size() <= index)
    {
        _subLists.resize(static_cast<size_t>(index) + 1);
    }
    return _subLists[index];
}

std::vector<ObjectEntryDescriptor>& ObjectList::GetList(ObjectType type) const
{
    return const_cast<ObjectList*>(this)->GetList(type);
}

const ObjectEntryDescriptor& ObjectList::GetObject(ObjectType type, ObjectEntryIndex index) const
{
    const auto& subList = GetList(type);
    if (subList.size() > index)
    {
        return subList[index];
    }

    static ObjectEntryDescriptor placeholder;
    return placeholder;
}

void ObjectList::Add(const ObjectEntryDescriptor& entry)
{
    auto& subList = GetList(entry.GetType());
    subList.push_back(entry);
}

void ObjectList::SetObject(ObjectEntryIndex index, const ObjectEntryDescriptor& entry)
{
    auto& subList = GetList(entry.GetType());
    if (subList.size() <= index)
    {
        subList.resize(static_cast<size_t>(index) + 1);
    }
    subList[index] = entry;
}

void ObjectList::SetObject(ObjectType type, ObjectEntryIndex index, std::string_view identifier)
{
    auto entry = ObjectEntryDescriptor(identifier);
    entry.Type = type;
    SetObject(index, entry);
}

ObjectEntryIndex ObjectList::Find(ObjectType type, std::string_view identifier) const
{
    auto& subList = GetList(type);
    for (size_t i = 0; i < subList.size(); i++)
    {
        if (subList[i].Generation == ObjectGeneration::JSON && subList[i].Identifier == identifier)
        {
            return static_cast<ObjectEntryIndex>(i);
        }
    }
    return kObjectEntryIndexNull;
}

// Intended to be used to find non-custom legacy objects. For internal use only.
ObjectEntryIndex ObjectList::FindLegacy(ObjectType type, std::string_view identifier) const
{
    auto& subList = GetList(type);
    for (size_t i = 0; i < subList.size(); i++)
    {
        if (subList[i].Generation == ObjectGeneration::DAT && subList[i].Entry.GetName() == identifier
            && subList[i].Entry.GetSourceGame() != ObjectSourceGame::Custom)
        {
            return static_cast<ObjectEntryIndex>(i);
        }
    }
    return kObjectEntryIndexNull;
}

/**
 *
 *  rct2: 0x006AB344
 */
void ObjectCreateIdentifierName(char* string_buffer, size_t size, const RCTObjectEntry* object)
{
    snprintf(string_buffer, size, "%.8s/%4X%4X", object->name, object->flags, object->checksum);
}

void ObjectGetTypeEntryIndex(size_t index, ObjectType* outObjectType, ObjectEntryIndex* outEntryIndex)
{
    uint8_t objectType = EnumValue(ObjectType::Ride);
    for (size_t groupCount : kObjectEntryGroupCounts)
    {
        if (index >= groupCount)
        {
            index -= groupCount;
            objectType++;
        }
        else
        {
            break;
        }
    }

    if (outObjectType != nullptr)
        *outObjectType = static_cast<ObjectType>(objectType);
    if (outEntryIndex != nullptr)
        *outEntryIndex = static_cast<ObjectEntryIndex>(index);
}

void ObjectEntryGetNameFixed(utf8* buffer, size_t bufferSize, const RCTObjectEntry* entry)
{
    bufferSize = std::min(static_cast<size_t>(kDatNameLength) + 1, bufferSize);
    std::memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}

void* ObjectEntryGetChunk(ObjectType objectType, ObjectEntryIndex index)
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto* object = objectMgr.GetLoadedObject(objectType, index);
    if (object != nullptr)
    {
        return object->GetLegacyData();
    }
    return nullptr;
}

const Object* ObjectEntryGetObject(ObjectType objectType, ObjectEntryIndex index)
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objectMgr.GetLoadedObject(objectType, index);
}
