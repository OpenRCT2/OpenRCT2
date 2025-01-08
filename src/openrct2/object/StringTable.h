/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/JsonFwd.hpp"
#include "../localisation/Language.h"

#include <string>
#include <vector>

struct IReadObjectContext;
namespace OpenRCT2
{
    struct IStream;
}

enum class ObjectStringID : uint8_t
{
    UNKNOWN = 255,
    NAME = 0,
    DESCRIPTION,
    SCENARIO_NAME = 0,
    PARK_NAME = 1,
    SCENARIO_DETAILS = 2,
    CAPACITY = 2,
    VEHICLE_NAME = 3,
};

struct StringTableEntry
{
    ObjectStringID Id = ObjectStringID::UNKNOWN;
    uint8_t LanguageId = LANGUAGE_UNDEFINED;
    std::string Text;
};

class StringTable
{
private:
    std::vector<StringTableEntry> _strings;
    static ObjectStringID ParseStringId(const std::string& s);

public:
    StringTable() = default;
    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    void Read(IReadObjectContext* context, OpenRCT2::IStream* stream, ObjectStringID id);
    /**
     * @note root is deliberately left non-const: json_t behaviour changes when const
     */
    void ReadJson(json_t& root);
    void Sort();
    std::string GetString(ObjectStringID id) const;
    std::string GetString(uint8_t language, ObjectStringID id) const;
    void SetString(ObjectStringID id, uint8_t language, const std::string& text);
};
