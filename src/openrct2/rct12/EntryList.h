/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <string>
#include <string_view>
#include <vector>

using ObjectEntryIndex = uint16_t;

namespace RCT12
{
    class EntryList
    {
    private:
        std::vector<std::string> _entries;

    public:
        size_t GetCount() const
        {
            return _entries.size();
        }

        const std::vector<std::string>& GetEntries() const
        {
            return _entries;
        }

        ObjectEntryIndex GetOrAddEntry(std::string_view identifier)
        {
            for (size_t i = 0; i < _entries.size(); i++)
            {
                if (_entries[i] == identifier)
                {
                    return static_cast<ObjectEntryIndex>(i);
                }
            }
            _entries.emplace_back(identifier);
            return static_cast<ObjectEntryIndex>(_entries.size() - 1);
        }

        void AddRange(std::initializer_list<const char*> initializerList)
        {
            for (auto entry : initializerList)
            {
                GetOrAddEntry(entry);
            }
        }

        template<uint32_t i> void AddRange(const std::string_view (&list)[i])
        {
            for (auto entry : list)
            {
                GetOrAddEntry(entry);
            }
        }
    };
} // namespace RCT12
