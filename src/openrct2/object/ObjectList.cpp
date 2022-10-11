/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ObjectList.h"

#include "../Context.h"
#include "../Game.h"
#include "../object/Object.h"
#include "../util/SawyerCoding.h"
#include "../util/Util.h"
#include "ObjectManager.h"
#include "ObjectRepository.h"

#include <algorithm>
#include <array>
#include <cstring>

// 98DA00
int32_t object_entry_group_counts[] = {
    MAX_RIDE_OBJECTS,          // rides
    MAX_SMALL_SCENERY_OBJECTS, // small scenery
    MAX_LARGE_SCENERY_OBJECTS, // large scenery
    MAX_WALL_SCENERY_OBJECTS,  // walls
    MAX_BANNER_OBJECTS,        // banners
    MAX_PATH_OBJECTS,          // paths
    MAX_PATH_ADDITION_OBJECTS, // path bits
    MAX_SCENERY_GROUP_OBJECTS, // scenery sets
    MAX_PARK_ENTRANCE_OBJECTS, // park entrance
    MAX_WATER_OBJECTS,         // water
    MAX_SCENARIO_TEXT_OBJECTS, // scenario text
    MAX_TERRAIN_SURFACE_OBJECTS,
    MAX_TERRAIN_EDGE_OBJECTS,
    MAX_STATION_OBJECTS,
    MAX_MUSIC_OBJECTS,
    MAX_FOOTPATH_SURFACE_OBJECTS,
    MAX_FOOTPATH_RAILINGS_OBJECTS,
    MAX_AUDIO_OBJECTS,
};
static_assert(std::size(object_entry_group_counts) == EnumValue(ObjectType::Count));

// 98DA2C
// clang-format off
int32_t object_entry_group_encoding[] = {
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_RLE,
    CHUNK_ENCODING_ROTATE,
};
// clang-format on

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

ObjectEntryIndex ObjectList::Find(ObjectType type, std::string_view identifier)
{
    auto& subList = GetList(type);
    for (size_t i = 0; i < subList.size(); i++)
    {
        if (subList[i].Identifier == identifier)
        {
            return static_cast<ObjectEntryIndex>(i);
        }
    }
    return OBJECT_ENTRY_INDEX_NULL;
}

/**
 *
 *  rct2: 0x006AB344
 */
void object_create_identifier_name(char* string_buffer, size_t size, const rct_object_entry* object)
{
    snprintf(string_buffer, size, "%.8s/%4X%4X", object->name, object->flags, object->checksum);
}

void get_type_entry_index(size_t index, ObjectType* outObjectType, ObjectEntryIndex* outEntryIndex)
{
    uint8_t objectType = EnumValue(ObjectType::Ride);
    for (size_t groupCount : object_entry_group_counts)
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

void object_entry_get_name_fixed(utf8* buffer, size_t bufferSize, const rct_object_entry* entry)
{
    bufferSize = std::min(static_cast<size_t>(DAT_NAME_LENGTH) + 1, bufferSize);
    std::memcpy(buffer, entry->name, bufferSize - 1);
    buffer[bufferSize - 1] = 0;
}

void* object_entry_get_chunk(ObjectType objectType, ObjectEntryIndex index)
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    auto* object = objectMgr.GetLoadedObject(objectType, index);
    if (object != nullptr)
    {
        return object->GetLegacyData();
    }
    return nullptr;
}

const Object* object_entry_get_object(ObjectType objectType, ObjectEntryIndex index)
{
    auto& objectMgr = OpenRCT2::GetContext()->GetObjectManager();
    return objectMgr.GetLoadedObject(objectType, index);
}
