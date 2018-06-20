/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <stdexcept>
#include <string>
#include <jansson.h>

#include "../common.h"

namespace Json
{
    // Don't try to load JSON files that exceed 64 MiB
    constexpr uint64_t MAX_JSON_SIZE = 64 * 1024 * 1024;

    json_t * ReadFromFile(const utf8 * path, size_t maxSize = MAX_JSON_SIZE);
    void     WriteToFile(const utf8 * path, const json_t * json, size_t flags = 0);

    json_t * FromString(const std::string & raw);
}

class JsonException final : public std::runtime_error
{
private:
    json_error_t _jsonError = {};

public:
    explicit JsonException(const std::string &message) : std::runtime_error(message) { }

    explicit JsonException(const json_error_t * jsonError) : JsonException(std::string(jsonError->text))
    {
        _jsonError = *jsonError;
    }
};
