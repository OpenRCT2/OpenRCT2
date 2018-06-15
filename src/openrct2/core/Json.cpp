/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FileStream.hpp"
#include "Json.hpp"
#include "Memory.hpp"
#include "String.hpp"

namespace Json
{
    json_t * ReadFromFile(const utf8 * path, size_t maxSize)
    {
        json_t  * json = nullptr;
        auto fs = FileStream(path, FILE_MODE_OPEN);

        size_t fileLength = (size_t)fs.GetLength();
        if (fileLength > maxSize)
        {
            throw IOException("Json file too large.");
        }

        utf8 * fileData = Memory::Allocate<utf8>(fileLength + 1);
        fs.Read(fileData, fileLength);
        fileData[fileLength] = '\0';

        json_error_t jsonLoadError;
        json = json_loads(fileData, 0, &jsonLoadError);
        Memory::Free(fileData);

        if (json == nullptr)
        {
            throw JsonException(&jsonLoadError);
        }

        return json;
    }

    void WriteToFile(const utf8 * path, const json_t * json, size_t flags)
    {
        // Serialise JSON
        const char * jsonOutput = json_dumps(json, flags);

        // Write to file
        auto fs = FileStream(path, FILE_MODE_WRITE);
        size_t jsonOutputSize = String::SizeOf(jsonOutput);
        fs.Write(jsonOutput, jsonOutputSize);
    }

    json_t * FromString(const std::string & raw)
    {
        json_t *     root;
        json_error_t error;
        root = json_loads(raw.c_str(), 0, &error);
        if (root == nullptr)
        {
            throw JsonException(&error);
        }
        return root;
    }
} // namespace Json
