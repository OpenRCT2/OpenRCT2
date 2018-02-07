#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#pragma warning(disable : 4706) // assignment within conditional expression

#include <cstdlib>
#include <cstring>
#include <unordered_map>
#include "../Context.h"
#include "../core/Math.hpp"
#include "../core/Memory.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "../interface/Cursors.h"
#include "../localisation/Language.h"
#include "../PlatformEnvironment.h"
#include "../sprites.h"
#include "Object.h"
#include "ObjectFactory.h"
#include "ObjectJsonHelpers.h"

using namespace OpenRCT2;

namespace ObjectJsonHelpers
{
    bool GetBoolean(const json_t * obj, const std::string &name, bool defaultValue)
    {
        auto value = json_object_get(obj, name.c_str());
        return json_is_boolean(value) ?
            json_boolean_value(value) :
            defaultValue;
    }

    std::string GetString(const json_t * value)
    {
        return json_is_string(value) ?
            std::string(json_string_value(value)) :
            std::string();
    }

    std::string GetString(const json_t * obj, const std::string &name, const std::string &defaultValue)
    {
        auto value = json_object_get(obj, name.c_str());
        return json_is_string(value) ?
            json_string_value(value) :
            defaultValue;
    }

    sint32 GetInteger(const json_t * obj, const std::string &name, const sint32 &defaultValue)
    {
        auto value = json_object_get(obj, name.c_str());
        if (json_is_integer(value))
        {
            sint64 val = json_integer_value(value);
            if (val >= std::numeric_limits<sint32>::min() &&
                val <= std::numeric_limits<sint32>::max())
            {
                return static_cast<sint32>(val);
            }
        }
        return defaultValue;
    }

    float GetFloat(const json_t * obj, const std::string &name, const float &defaultValue)
    {
        auto value = json_object_get(obj, name.c_str());
        return json_is_number(value) ?
            json_number_value(value) :
            defaultValue;
    }

    std::vector<std::string> GetJsonStringArray(const json_t * arr)
    {
        std::vector<std::string> result;
        if (json_is_array(arr))
        {
            auto count = json_array_size(arr);
            result.reserve(count);
            for (size_t i = 0; i < count; i++)
            {
                auto element = json_string_value(json_array_get(arr, i));
                result.push_back(element);
            }
        }
        else if (json_is_string(arr))
        {
            result.push_back(json_string_value(arr));
        }
        return result;
    }

    std::vector<sint32> GetJsonIntegerArray(const json_t * arr)
    {
        std::vector<sint32> result;
        if (json_is_array(arr))
        {
            auto count = json_array_size(arr);
            result.reserve(count);
            for (size_t i = 0; i < count; i++)
            {
                auto element = json_integer_value(json_array_get(arr, i));
                result.push_back(element);
            }
        }
        else if (json_is_integer(arr))
        {
            result.push_back(json_integer_value(arr));
        }
        return result;
    }

    uint8 ParseCursor(const std::string &s, uint8 defaultValue)
    {
        static const std::unordered_map<std::string, uint8> LookupTable
        {
            { "CURSOR_BLANK",           CURSOR_BLANK },
            { "CURSOR_UP_ARROW",        CURSOR_UP_ARROW },
            { "CURSOR_UP_DOWN_ARROW",   CURSOR_UP_DOWN_ARROW },
            { "CURSOR_HAND_POINT",      CURSOR_HAND_POINT },
            { "CURSOR_ZZZ",             CURSOR_ZZZ },
            { "CURSOR_DIAGONAL_ARROWS", CURSOR_DIAGONAL_ARROWS },
            { "CURSOR_PICKER",          CURSOR_PICKER },
            { "CURSOR_TREE_DOWN",       CURSOR_TREE_DOWN },
            { "CURSOR_FOUNTAIN_DOWN",   CURSOR_FOUNTAIN_DOWN },
            { "CURSOR_STATUE_DOWN",     CURSOR_STATUE_DOWN },
            { "CURSOR_BENCH_DOWN",      CURSOR_BENCH_DOWN },
            { "CURSOR_CROSS_HAIR",      CURSOR_CROSS_HAIR },
            { "CURSOR_BIN_DOWN",        CURSOR_BIN_DOWN },
            { "CURSOR_LAMPPOST_DOWN",   CURSOR_LAMPPOST_DOWN },
            { "CURSOR_FENCE_DOWN",      CURSOR_FENCE_DOWN },
            { "CURSOR_FLOWER_DOWN",     CURSOR_FLOWER_DOWN },
            { "CURSOR_PATH_DOWN",       CURSOR_PATH_DOWN },
            { "CURSOR_DIG_DOWN",        CURSOR_DIG_DOWN },
            { "CURSOR_WATER_DOWN",      CURSOR_WATER_DOWN },
            { "CURSOR_HOUSE_DOWN",      CURSOR_HOUSE_DOWN },
            { "CURSOR_VOLCANO_DOWN",    CURSOR_VOLCANO_DOWN },
            { "CURSOR_WALK_DOWN",       CURSOR_WALK_DOWN },
            { "CURSOR_PAINT_DOWN",      CURSOR_PAINT_DOWN },
            { "CURSOR_ENTRANCE_DOWN",   CURSOR_ENTRANCE_DOWN },
            { "CURSOR_HAND_OPEN",       CURSOR_HAND_OPEN },
            { "CURSOR_HAND_CLOSED",     CURSOR_HAND_CLOSED },
            { "CURSOR_ARROW",           CURSOR_ARROW },
        };

        auto result = LookupTable.find(s);
        return (result != LookupTable.end()) ?
            result->second :
            defaultValue;
    }

    rct_object_entry ParseObjectEntry(const std::string & s)
    {
        rct_object_entry entry = { 0 };
        std::fill_n(entry.name, sizeof(entry.name), ' ');
        auto copyLen = std::min<size_t>(8, s.size());
        std::copy_n(s.c_str(), copyLen, entry.name);
        return entry;
    }

    static std::vector<sint32> ParseRange(std::string s)
    {
        // Currently only supports [###] or [###..###]
        std::vector<sint32> result = { };
        if (s.length() >= 3 && s[0] == '[' && s[s.length() - 1] == ']')
        {
            s = s.substr(1, s.length() - 2);
            auto parts = String::Split(s, "..");
            if (parts.size() == 1)
            {
                result.push_back(std::stoi(parts[0]));
            }
            else
            {
                auto left = std::stoi(parts[0]);
                auto right = std::stoi(parts[1]);
                if (left <= right)
                {
                    for (auto i = left; i <= right; i++)
                    {
                        result.push_back(i);
                    }
                }
                else
                {
                    for (auto i = right; i >= left; i--)
                    {
                        result.push_back(i);
                    }
                }
            }
        }
        return result;
    }

    static std::vector<rct_g1_element> LoadObjectImages(const std::string &name, uint32 start, uint32 end)
    {
        std::vector<rct_g1_element> result;
        const auto env = GetContext()->GetPlatformEnvironment();
        auto objectsPath = env->GetDirectoryPath(DIRBASE::RCT2, DIRID::OBJECT);
        auto objectPath = Path::Combine(objectsPath, name);
        auto obj = ObjectFactory::CreateObjectFromLegacyFile(objectPath.c_str());
        if (obj != nullptr)
        {
            auto &imgTable = static_cast<const Object *>(obj)->GetImageTable();
            auto numImages = imgTable.GetCount();
            auto images = imgTable.GetImages();
            for (uint32 i = start; i < Math::Min(numImages, end); i++)
            {
                auto &objg1 = images[i];
                auto length = g1_calculate_data_size(&objg1);
                auto g1 = objg1;
                g1.offset = (uint8 *)std::malloc(length);
                std::memcpy(g1.offset, objg1.offset, length);
                result.push_back(g1);
            }
            delete obj;
        }
        else
        {
            log_warning("Unable to open '%s'", objectPath.c_str());
        }

        // Add place holders
        auto placeHolders = (size_t)(end - start) - result.size();
        if (placeHolders > 0)
        {
            log_warning("Adding %d placeholders", placeHolders);
            for (size_t i = 0; i < placeHolders; i++)
            {
                auto g1 = rct_g1_element{};
                result.push_back(g1);
            }
        }
        return result;
    }

    static std::vector<rct_g1_element> ParseImages(std::string s)
    {
        std::vector<rct_g1_element> result;
        if (s.empty())
        {
            rct_g1_element emptyg1 = { 0 };
            result.push_back(emptyg1);
        }
        else if (String::StartsWith(s, "$CSG"))
        {
            if (is_csg_loaded())
            {
                auto range = ParseRange(s.substr(4));
                if (range.size() > 0)
                {
                    for (auto i : range)
                    {
                        auto &csg1 = *gfx_get_g1_element(SPR_CSG_BEGIN + i);
                        auto length = g1_calculate_data_size(&csg1);
                        auto g1 = csg1;
                        g1.offset = (uint8 *)std::malloc(length);
                        std::memcpy(g1.offset, csg1.offset, length);
                        result.push_back(g1);
                    }
                }
            }
        }
        else if (String::StartsWith(s, "$RCT2:OBJDATA/"))
        {
            auto name = s.substr(14);
            auto rangeStart = name.find('[');
            auto imgStart = 0;
            auto imgEnd = INT16_MAX;
            //auto range = std::vector<sint32>({ 0 });
            if (rangeStart != std::string::npos)
            {
                auto rangeString = name.substr(rangeStart);
                auto range = ParseRange(name.substr(rangeStart));
                name = name.substr(0, rangeStart);

                if (range.size() > 0)
                {
                    imgStart = range.front();
                    imgEnd = range.back();
                }
            }
            return LoadObjectImages(name, imgStart, imgEnd);
        }
        return result;
    }

    static uint8 ParseStringId(const std::string &s)
    {
        if (s == "name") return OBJ_STRING_ID_NAME;
        if (s == "description") return OBJ_STRING_ID_DESCRIPTION;
        if (s == "capacity") return OBJ_STRING_ID_CAPACITY;
        if (s == "vehicleName") return OBJ_STRING_ID_VEHICLE_NAME;
        return OBJ_STRING_ID_UNKNOWN;
    }

    void LoadStrings(const json_t * root, StringTable &stringTable)
    {
        auto jsonStrings = json_object_get(root, "strings");
        const char * key;
        json_t     * jlanguages;
        json_object_foreach(jsonStrings, key, jlanguages)
        {
            auto stringId = ParseStringId(key);
            if (stringId != OBJ_STRING_ID_UNKNOWN)
            {
                const char * locale;
                json_t     * jstring;
                json_object_foreach(jlanguages, locale, jstring)
                {
                    auto langId = language_get_id_from_locale(locale);
                    if (langId != LANGUAGE_UNDEFINED)
                    {
                        auto string = json_string_value(jstring);
                        stringTable.SetString(stringId, langId, string);
                    }
                }
            }
        }
    }

    void LoadImages(const json_t * root, ImageTable &imageTable)
    {
        auto jsonImages = json_object_get(root, "images");
        auto imageElements = GetJsonStringArray(jsonImages);
        for (const auto &ie : imageElements)
        {
            auto images = ParseImages(ie);
            for (const auto &g1 : images)
            {
                imageTable.AddImage(&g1);
                std::free(g1.offset);
            }
        }
    }
}
