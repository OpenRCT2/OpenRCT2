#pragma once

#include <jansson.h>

#include "../common.h"
#include "Exception.hpp"

namespace Json
{
    // Don't try to load JSON files that exceed 64 MiB
    constexpr uint64 MAX_JSON_SIZE = 64 * 1024 * 1024;

    json_t * ReadFromFile(const utf8 * path, size_t maxSize = MAX_JSON_SIZE);
    void     WriteToFile(const utf8 * path, const json_t * json, size_t flags = 0);
}

class JsonException : public Exception
{
private:
    json_error_t _jsonError;

public:
    JsonException(const char * message) : Exception(message) { }

    JsonException(const json_error_t * jsonError) : JsonException(jsonError->text)
    {
        _jsonError = *jsonError;
    }
};
