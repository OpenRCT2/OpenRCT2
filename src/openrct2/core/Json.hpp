/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/String.hpp"

#include <nlohmann/json.hpp>
#include <string>
#include <string_view>

#if NLOHMANN_JSON_VERSION_MAJOR < 3 || (NLOHMANN_JSON_VERSION_MAJOR == 3 && NLOHMANN_JSON_VERSION_MINOR < 6)
#    error "Unsupported version of nlohmann json library, must be >= 3.6"
#endif // NLOHMANN_JSON_VERSION_MAJOR < 3 || (NLOHMANN_JSON_VERSION_MAJOR == 3 && NLOHMANN_JSON_VERSION_MINOR < 6)

using json_t = nlohmann::json;

namespace Json
{
    // Don't try to load JSON files that exceed 64 MiB
    constexpr uint64_t MAX_JSON_SIZE = 64 * 1024 * 1024;

    /**
     * Read JSON file and parse contents
     * @param path Path to the source file
     * @param maxSize Max file size in bytes allowed
     * @return A JSON representation of the file
     * @note This function will throw an exception if the JSON file cannot be parsed
     */
    json_t ReadFromFile(u8string_view path, size_t maxSize = MAX_JSON_SIZE);

    /**
     * Read JSON file and parse the contents
     * @param path Path to the destination file
     * @param jsonData A JSON object
     * @param indentSize The number of spaces in an indent, or removes whitespace on -1
     */
    void WriteToFile(u8string_view path, const json_t& jsonData, int indentSize = 4);

    /**
     * Parse JSON from a string
     * @param raw JSON string
     * @return A JSON representation of the string
     * @note This function will throw an exception if the JSON string cannot be parsed
     */
    json_t FromString(std::string_view raw);

    /**
     * Parse JSON from a vector of characters
     * @param vec Vector of characters containing JSON
     * @return A JSON representation of the vector
     * @note This function will throw an exception if the JSON vector cannot be parsed
     */
    json_t FromVector(const std::vector<uint8_t>& vec);

    /**
     * Explicit type conversion between a JSON object and a compatible number value
     * @param T Destination numeric type
     * @param jsonObj JSON object holding the value
     * @param defaultValue Default value to return if the JSON object is not a number type
     * @return Copy of the JSON value converted to the given type
     */
    template<typename T> T GetNumber(const json_t& jsonObj, T defaultValue = 0)
    {
        static_assert(std::is_arithmetic<T>::value, "GetNumber template parameter must be arithmetic");

        return jsonObj.is_number() ? jsonObj.get<T>() : defaultValue;
    }

    /**
     * Explicit type conversion between a JSON object and a compatible enum value
     * @param T Destination enum
     * @param jsonObj JSON object holding the value
     * @param defaultValue Default value to return if the JSON object is not an enum type
     * @return Copy of the JSON value converted to the given enum type
     */
    template<typename T> T GetEnum(const json_t& jsonObj, T defaultValue)
    {
        static_assert(std::is_enum<T>::value, "GetEnum template parameter must be an enum");

        return jsonObj.is_number_integer() ? jsonObj.get<T>() : defaultValue;
    }

    /**
     * Explicit type conversion between a JSON object and an std::string
     * @param jsonObj JSON object holding the value
     * @param defaultValue Default value to return if the JSON object is not a string
     * @return Copy of the JSON value converted to std::string
     */
    std::string GetString(const json_t& jsonObj, const std::string& defaultValue = std::string());

    /**
     * Explicit type conversion between a JSON object and a boolean
     * @param jsonObj JSON object holding the value
     * @param defaultValue Default value to return if the JSON object is not a boolean
     * @return Copy of the JSON value converted to bool
     */
    bool GetBoolean(const json_t& jsonObj, bool defaultValue = false);

    /**
     * Ensures a given JSON object is an object type
     * @param jsonObj JSON object
     * @return The JSON object if it is an object type, or an empty object otherwise
     */
    json_t AsObject(const json_t& jsonObj);

    /**
     * Ensures a given JSON object is an array type
     * @param jsonObj JSON object
     * @return The JSON object if it is an array type, or an array containing the JSON object otherwise
     */
    json_t AsArray(const json_t& jsonObj);

    /**
     * Helper function to convert a json object and an initializer list to binary flags
     * @param T Type to return
     * @param jsonObj JSON object containing boolean values
     * @param list List of pairs of keys and bits to enable if that key in the object is true
     * @return Value with relevant bits flipped
     */
    template<typename T> T GetFlags(const json_t& jsonObj, std::initializer_list<std::pair<std::string, T>> list)
    {
        static_assert(std::is_convertible<T, int>::value, "GetFlags template parameter must be integral or a weak enum");

        T flags{};
        for (const auto& item : list)
        {
            if (jsonObj.contains(item.first) && Json::GetBoolean(jsonObj[item.first]))
            {
                flags = static_cast<T>(flags | item.second);
            }
        }
        return flags;
    }

    /**
     * Used by the GetFlags function to allow for inverted values
     */
    enum class FlagType : uint8_t
    {
        // Flag is turned on if the key is true
        Normal,
        // Flag is turned on if the key is false
        Inverted
    };

    /**
     * Helper function to convert a json object and an initializer list to binary flags
     * @param T Type to return
     * @param jsonObj JSON object containing boolean values
     * @param list List of tuples of keys, bits to change and flag type
     * @return Value with relevant bits flipped
     * @note FLAG_NORMAL behaves like the other GetFlags function, but FLAG_INVERTED will turn the flag on when false
     */
    template<typename T> T GetFlags(const json_t& jsonObj, std::initializer_list<std::tuple<std::string, T, FlagType>> list)
    {
        static_assert(std::is_convertible<T, int>::value, "GetFlags template parameter must be integral or a weak enum");

        T flags{};
        for (const auto& item : list)
        {
            if (std::get<2>(item) == FlagType::Normal)
            {
                if (jsonObj.contains(std::get<0>(item)) && Json::GetBoolean(jsonObj[std::get<0>(item)]))
                {
                    flags = static_cast<T>(flags | std::get<1>(item));
                }
            }
            else
            {
                // if the json flag doesn't exist, assume it's false
                if (!jsonObj.contains(std::get<0>(item)) || !Json::GetBoolean(jsonObj[std::get<0>(item)]))
                {
                    flags = static_cast<T>(flags | std::get<1>(item));
                }
            }
        }
        return flags;
    }
} // namespace Json

class JsonException final : public std::runtime_error
{
public:
    explicit JsonException(const std::string& message)
        : std::runtime_error(message)
    {
    }
};
