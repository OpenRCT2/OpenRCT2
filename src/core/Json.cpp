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
