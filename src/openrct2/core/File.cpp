#pragma region Copyright (c) 2014-2018 OpenRCT2 Developers
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

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#else
    #include <sys/stat.h>
#endif

#include <fstream>
#include "File.h"
#include "FileStream.hpp"
#include "String.hpp"
#include "../util/Util.h"

#include "../platform/platform.h"

namespace File
{
    bool Exists(const std::string &path)
    {
        return platform_file_exists(path.c_str());
    }

    bool Copy(const std::string &srcPath, const std::string &dstPath, bool overwrite)
    {
        return platform_file_copy(srcPath.c_str(), dstPath.c_str(), overwrite);
    }

    bool Delete(const std::string &path)
    {
        return platform_file_delete(path.c_str());
    }

    bool Move(const std::string &srcPath, const std::string &dstPath)
    {
        return platform_file_move(srcPath.c_str(), dstPath.c_str());
    }

    std::vector<uint8> ReadAllBytes(const std::string_view& path)
    {
        std::vector<uint8> result;

#if defined(_WIN32) && !defined(__MINGW32__)
        auto pathW = String::ToUtf16(std::string(path));
        std::ifstream fs(pathW, std::ios::in | std::ios::binary);
#else
        std::ifstream fs(std::string(path), std::ios::in | std::ios::binary);
#endif
        if (!fs.is_open())
        {
            throw IOException("Unable to open " + std::string(path.data()));
        }

        fs.seekg(0, std::ios::end);
        auto fsize = (size_t)fs.tellg();
        if (fsize > SIZE_MAX)
        {
            std::string message = String::StdFormat("'%s' exceeds maximum length of %lld bytes.", SIZE_MAX);
            throw IOException(message);
        }
        else
        {
            result.resize(fsize);
            fs.seekg(0);
            fs.read((char *)result.data(), result.size());
            fs.exceptions(fs.failbit);
        }
        return result;
    }

    std::string ReadAllText(const std::string_view& path)
    {
        auto bytes = ReadAllBytes(path);
        // TODO skip BOM
        std::string result(bytes.size(), 0);
        std::copy(bytes.begin(), bytes.end(), result.begin());
        return result;
    }

    void WriteAllBytes(const std::string &path, const void * buffer, size_t length)
    {
        auto fs = FileStream(path, FILE_MODE_WRITE);
        fs.Write(buffer, length);
    }

    std::vector<std::string> ReadAllLines(const std::string &path)
    {
        std::vector<std::string> lines;
        auto data = ReadAllBytes(path);
        auto lineStart = (const char *)data.data();
        auto ch = lineStart;
        char lastC = 0;
        for (size_t i = 0; i < data.size(); i++)
        {
            char c = *ch;
            if (c == '\n' && lastC == '\r')
            {
                // Ignore \r\n
                lineStart = ch + 1;
            }
            else if (c == '\n' || c == '\r')
            {
                lines.emplace_back(lineStart, ch - lineStart);
                lineStart = ch + 1;
            }
            lastC = c;
            ch++;
        }

        // Last line
        lines.emplace_back(lineStart, ch - lineStart);
        return lines;
    }

    uint64 GetLastModified(const std::string &path)
    {
        uint64 lastModified = 0;
#ifdef _WIN32
        auto pathW = utf8_to_widechar(path.c_str());
        auto hFile = CreateFileW(pathW, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            FILETIME ftCreate, ftAccess, ftWrite;
            if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
            {
                lastModified = ((uint64)ftWrite.dwHighDateTime << 32ULL) | (uint64)ftWrite.dwLowDateTime;
            }
            CloseHandle(hFile);
        }
        free(pathW);
#else
        struct stat statInfo{};
        if (stat(path.c_str(), &statInfo) == 0)
        {
            lastModified = statInfo.st_mtime;
        }
#endif
        return lastModified; 
    }
} // namespace File

bool writeentirefile(const utf8 * path, const void * buffer, size_t length)
{
    try
    {
        File::WriteAllBytes(String::ToStd(path), buffer, length);
        return true;
    }
    catch (const std::exception &)
    {
        return false;
    }
}

