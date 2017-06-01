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
}
