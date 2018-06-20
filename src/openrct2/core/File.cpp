/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

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

    std::vector<uint8_t> ReadAllBytes(const std::string_view& path)
    {
        std::vector<uint8_t> result;

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

    uint64_t GetLastModified(const std::string &path)
    {
        uint64_t lastModified = 0;
#ifdef _WIN32
        auto pathW = utf8_to_widechar(path.c_str());
        auto hFile = CreateFileW(pathW, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, 0, nullptr);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            FILETIME ftCreate, ftAccess, ftWrite;
            if (GetFileTime(hFile, &ftCreate, &ftAccess, &ftWrite))
            {
                lastModified = ((uint64_t)ftWrite.dwHighDateTime << 32ULL) | (uint64_t)ftWrite.dwLowDateTime;
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

