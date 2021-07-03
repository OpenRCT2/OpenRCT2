/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Json.hpp"

#include "FileStream.h"
#include "Memory.hpp"
#include "String.hpp"

namespace Json
{
    json_t ReadFromFile(const utf8* path, size_t maxSize)
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_OPEN);

        size_t fileLength = static_cast<size_t>(fs.GetLength());
        if (fileLength > maxSize)
        {
            throw IOException("Json file too large.");
        }

        auto fileData = std::string(static_cast<size_t>(fileLength) + 1, '\0');
        fs.Read(static_cast<void*>(fileData.data()), fileLength);

        json_t json;

        try
        {
            json = json_t::parse(fileData);
        }
        catch (const json_t::exception& e)
        {
            throw JsonException(String::Format("Unable to parse JSON file (%s)\n\t%s", path, e.what()));
        }

        return json;
    }

    json_t ReadFromFile(const fs::path& path, size_t maxSize)
    {
        auto path8 = path.u8string();
        return ReadFromFile(path8.c_str(), maxSize);
    }

    void WriteToFile(const utf8* path, const json_t& jsonData, int indentSize)
    {
        // Serialise JSON
        std::string jsonOutput = jsonData.dump(indentSize);

        // Write to file
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
        fs.Write(jsonOutput.data(), jsonOutput.size());
    }

    void WriteToFile(const fs::path& path, const json_t& jsonData, int indentSize)
    {
        auto path8 = path.u8string();
        WriteToFile(path8.c_str(), jsonData, indentSize);
    }

    json_t FromString(std::string_view raw)
    {
        json_t json;

        try
        {
            json = json_t::parse(raw);
        }
        catch (const json_t::exception& e)
        {
            log_error("Unable to parse JSON string (%s)\n\t%s", raw, e.what());
        }

        return json;
    }

    json_t FromVector(const std::vector<uint8_t>& vec)
    {
        json_t json;

        try
        {
            json = json_t::parse(vec.begin(), vec.end());
        }
        catch (const json_t::exception& e)
        {
            log_error("Unable to parse JSON vector (%s)\n\t%s", vec.data(), e.what());
        }

        return json;
    }

    std::string GetString(const json_t& jsonObj, const std::string& defaultValue)
    {
        return jsonObj.is_string() ? jsonObj.get<std::string>() : defaultValue;
    }

    bool GetBoolean(const json_t& jsonObj, bool defaultValue)
    {
        return jsonObj.is_boolean() ? jsonObj.get<bool>() : defaultValue;
    }

    json_t AsObject(const json_t& jsonObj)
    {
        return jsonObj.is_object() ? jsonObj : json_t::object();
    }

    json_t AsArray(const json_t& jsonObj)
    {
        if (jsonObj.is_array())
        {
            return jsonObj;
        }

        json_t retVal = json_t::array();

        if (jsonObj.is_object())
        {
            for (const auto& jItem : jsonObj)
            {
                retVal.push_back(jItem);
            }
        }
        else if (!jsonObj.is_null())
        {
            retVal.push_back(jsonObj);
        }

        return retVal;
    }
} // namespace Json
