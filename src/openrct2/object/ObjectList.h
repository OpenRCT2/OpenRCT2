/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "Object.h"
#include "ObjectLimits.h"

#include <vector>

class ObjectList
{
private:
    std::vector<std::vector<ObjectEntryDescriptor>> _subLists;

public:
    void Add(const ObjectEntryDescriptor& entry);
    std::vector<ObjectEntryDescriptor>& GetList(ObjectType type);
    std::vector<ObjectEntryDescriptor>& GetList(ObjectType type) const;
    const ObjectEntryDescriptor& GetObject(ObjectType type, ObjectEntryIndex index) const;
    void SetObject(ObjectEntryIndex index, const ObjectEntryDescriptor& entry);
    void SetObject(ObjectType type, ObjectEntryIndex index, std::string_view identifier);
    ObjectEntryIndex Find(ObjectType type, std::string_view identifier);

    struct const_iterator
    {
    private:
        const ObjectList* _parent;
        size_t _subList;
        size_t _index;

        void MoveToNextEntry();

    public:
        const_iterator(const ObjectList* parent, bool end);
        const ObjectEntryDescriptor& operator*();
        bool operator==(const_iterator& rhs);
        bool operator!=(const_iterator& rhs);
        const_iterator& operator++();
        const_iterator operator++(int);
    };

    const_iterator begin() const;
    const_iterator end() const;
};

void get_type_entry_index(size_t index, ObjectType* outObjectType, ObjectEntryIndex* outEntryIndex);
