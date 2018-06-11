/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3
 *****************************************************************************/

#pragma once

#include <initializer_list>
#include <string>
#include <utility>
#include <vector>
#include "../common.h"
#include "../core/Json.hpp"
#include "../drawing/Drawing.h"
#include "../object/Object.h"
#include "ImageTable.h"
#include "StringTable.h"

namespace ObjectJsonHelpers
{
    bool GetBoolean(const json_t * obj, const std::string &name, bool defaultValue = false);
    std::string GetString(const json_t * value);
    std::string GetString(const json_t * obj, const std::string &name, const std::string &defaultValue = "");
    sint32 GetInteger(const json_t * obj, const std::string &name, const sint32 &defaultValue = 0);
    float GetFloat(const json_t * obj, const std::string &name, const float &defaultValue = 0);
    std::vector<std::string> GetJsonStringArray(const json_t * arr);
    std::vector<sint32> GetJsonIntegerArray(const json_t * arr);
    uint8 ParseCursor(const std::string &s, uint8 defaultValue);
    rct_object_entry ParseObjectEntry(const std::string & s);
    void LoadStrings(const json_t * root, StringTable &stringTable);
    void LoadImages(IReadObjectContext * context, const json_t * root, ImageTable &imageTable);

    template<typename T>
    static T GetFlags(const json_t * obj, std::initializer_list<std::pair<std::string, T>> list)
    {
        T flags = 0;
        for (const auto &item : list)
        {
            if (GetBoolean(obj, item.first))
            {
                flags |= item.second;
            }
        }
        return flags;
    }
} // namespace ObjectJsonHelpers
