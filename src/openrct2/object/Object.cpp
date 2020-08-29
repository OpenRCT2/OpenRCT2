/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Object.h"

#include "../Context.h"
#include "../core/Memory.hpp"
#include "../core/String.hpp"
#include "../localisation/Language.h"
#include "../localisation/LocalisationService.h"
#include "../localisation/StringIds.h"
#include "../world/Scenery.h"
#include "ObjectLimits.h"

#include <algorithm>
#include <cstring>
#include <stdexcept>

Object::Object(const rct_object_entry& entry)
{
    _objectEntry = entry;
}

void* Object::GetLegacyData()
{
    throw std::runtime_error("Not supported.");
}

void Object::ReadLegacy(IReadObjectContext* context, OpenRCT2::IStream* stream)
{
    throw std::runtime_error("Not supported.");
}

std::string Object::GetOverrideString(uint8_t index) const
{
    auto legacyIdentifier = GetLegacyIdentifier();
    const auto& localisationService = OpenRCT2::GetContext()->GetLocalisationService();
    auto stringId = localisationService.GetObjectOverrideStringId(legacyIdentifier, index);

    const utf8* result = nullptr;
    if (stringId != STR_NONE)
    {
        result = language_get_string(stringId);
    }
    return String::ToStd(result);
}

std::string Object::GetString(ObjectStringID index) const
{
    auto sz = GetOverrideString(static_cast<uint8_t>(index));
    if (sz.empty())
    {
        sz = GetStringTable().GetString(index);
    }
    return sz;
}

std::string Object::GetString(int32_t language, ObjectStringID index) const
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

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic push
#    pragma GCC diagnostic ignored "-Wsuggest-final-methods"
#endif

std::string Object::GetName() const
{
    return GetString(ObjectStringID::NAME);
}

std::string Object::GetName(int32_t language) const
{
    return GetString(language, ObjectStringID::NAME);
}

void rct_object_entry::SetName(const std::string_view& value)
{
    std::memset(name, ' ', sizeof(name));
    std::memcpy(name, value.data(), std::min(sizeof(name), value.size()));
}

const std::vector<std::string>& Object::GetAuthors() const
{
    return _authors;
}

void Object::SetAuthors(const std::vector<std::string>&& authors)
{
    _authors = authors;
}

std::optional<uint8_t> rct_object_entry::GetSceneryType() const
{
    switch (GetType())
    {
        case OBJECT_TYPE_SMALL_SCENERY:
            return SCENERY_TYPE_SMALL;
        case OBJECT_TYPE_LARGE_SCENERY:
            return SCENERY_TYPE_LARGE;
        case OBJECT_TYPE_WALLS:
            return SCENERY_TYPE_WALL;
        case OBJECT_TYPE_BANNERS:
            return SCENERY_TYPE_BANNER;
        case OBJECT_TYPE_PATH_BITS:
            return SCENERY_TYPE_PATH_ITEM;
        default:
            return std::nullopt;
    }
}

#ifdef __WARN_SUGGEST_FINAL_METHODS__
#    pragma GCC diagnostic pop
#endif
