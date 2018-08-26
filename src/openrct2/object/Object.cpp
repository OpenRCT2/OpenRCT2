/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Object.h"

#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/StringIds.h"
#include "ObjectLimits.h"

#include <algorithm>

Object::Object(const rct_object_entry& entry)
{
    _objectEntry = entry;

    char name[DAT_NAME_LENGTH + 1] = { 0 };
    std::copy_n(entry.name, DAT_NAME_LENGTH, name);
    _identifier = String::Duplicate(name);

    if (IsOpenRCT2OfficialObject())
    {
        SetSourceGames({ OBJECT_SOURCE_OPENRCT2_OFFICIAL });
    }
}

Object::~Object()
{
    Memory::Free(_identifier);
}

std::string Object::GetOverrideString(uint8_t index) const
{
    const char* identifier = GetIdentifier();
    rct_string_id stringId = language_get_object_override_string_id(identifier, index);

    const utf8* result = nullptr;
    if (stringId != STR_NONE)
    {
        result = language_get_string(stringId);
    }
    return String::ToStd(result);
}

std::string Object::GetString(uint8_t index) const
{
    auto sz = GetOverrideString(index);
    if (sz.empty())
    {
        sz = GetStringTable().GetString(index);
    }
    return sz;
}

std::string Object::GetString(int32_t language, uint8_t index) const
{
    return GetStringTable().GetString(language, index);
}

rct_object_entry Object::GetScgWallsHeader()
{
    return Object::CreateHeader("SCGWALLS", 207140231, 3518650219);
}

rct_object_entry Object::GetScgPathXHeader()
{
    return Object::CreateHeader("SCGPATHX", 207140231, 890227440);
}

rct_object_entry Object::CreateHeader(const char name[DAT_NAME_LENGTH + 1], uint32_t flags, uint32_t checksum)
{
    rct_object_entry header = {};
    header.flags = flags;
    std::copy_n(name, DAT_NAME_LENGTH, header.name);
    header.checksum = checksum;
    return header;
}

std::vector<uint8_t> Object::GetSourceGames()
{
    return _sourceGames;
}

void Object::SetSourceGames(const std::vector<uint8_t>& sourceGames)
{
    _sourceGames = sourceGames;
}

bool Object::IsOpenRCT2OfficialObject()
{
    static const char _openRCT2OfficialObjects[][9] = {
        // Offical extended scenery set
        "XXBBBR01",
        "TTRFTL02",
        "TTRFTL03",
        "TTRFTL04",
        "TTRFTL07",
        "TTRFTL08",
        "TTPIRF02",
        "TTPIRF03",
        "TTPIRF04",
        "TTPIRF05",
        "TTPIRF07",
        "TTPIRF08",
        "MG-PRAR ",
        "TTRFWD01",
        "TTRFWD02",
        "TTRFWD03",
        "TTRFWD04",
        "TTRFWD05",
        "TTRFWD06",
        "TTRFWD07",
        "TTRFWD08",
        "TTRFGL01",
        "TTRFGL02",
        "TTRFGL03",
        "ACWW33  ",
        "ACWWF32 ",

        // Official DLC
        "BIGPANDA",
        "LITTERPA",
        "PANDAGR ",
        "SCGPANDA",
        "WTRPINK ",
        "ZPANDA  ",
    };

    for (const auto entry : _openRCT2OfficialObjects)
    {
        if (String::Equals(_identifier, entry))
            return true;
    }

    return false;
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif

std::string Object::GetName() const
{
    return GetString(OBJ_STRING_ID_NAME);
}

std::string Object::GetName(int32_t language) const
{
    return GetString(language, OBJ_STRING_ID_NAME);
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif
