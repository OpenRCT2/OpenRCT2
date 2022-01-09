/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
#    include <windows.h>
#else
#    include <sys/stat.h>
#endif

#include "../platform/Platform2.h"
#include "../util/Util.h"
#include "File.h"
#include "FileStream.h"
#include "String.hpp"

#include <fstream>

namespace File
{
    bool Exists(std::string_view path)
    {
        fs::path file = u8path(path);
        log_verbose("Checking if file exists: %s", std::string(path).c_str());
        return fs::exists(file);
    }

    bool Copy(std::string_view srcPath, std::string_view dstPath, bool overwrite)
    {
        if (!overwrite && Exists(dstPath))
        {
            log_warning("File::Copy(): Not overwriting %s, because overwrite flag == false", std::string(dstPath).c_str());
            return false;
        }

        return fs::copy_file(u8path(srcPath), u8path(dstPath));
    }

    bool Delete(std::string_view path)
    {
        return fs::remove(u8path(path));
    }

    bool Move(std::string_view srcPath, std::string_view dstPath)
    {
        try
        {
            fs::rename(u8path(srcPath), u8path(dstPath));
            return true;
        }
        catch (const fs::filesystem_error&)
        {
            return false;
        }
    }

    std::vector<uint8_t> ReadAllBytes(std::string_view path)
    {
#if defined(_WIN32) && !defined(__MINGW32__)
        auto pathW = String::ToWideChar(path);
        std::ifstream fs(pathW, std::ios::in | std::ios::binary);
#else
        std::ifstream fs(std::string(path), std::ios::in | std::ios::binary);
#endif
        if (!fs.is_open())
        {
            throw IOException("Unable to open " + std::string(path));
        }

        std::vector<uint8_t> result;
        auto fsize = Platform::GetFileSize(path);
        if (fsize > SIZE_MAX)
        {
            std::string message = String::StdFormat(
                "'%s' exceeds maximum length of %lld bytes.", std::string(path).c_str(), SIZE_MAX);
            throw IOException(message);
        }
        else
        {
            result.resize(fsize);
            fs.read(reinterpret_cast<char*>(result.data()), result.size());
            fs.exceptions(fs.failbit);
        }
        return result;
    }

    std::string ReadAllText(std::string_view path)
    {
        auto bytes = ReadAllBytes(path);
        // TODO skip BOM
        std::string result(bytes.size(), 0);
        std::copy(bytes.begin(), bytes.end(), result.begin());
        return result;
    }

    std::vector<std::string> ReadAllLines(std::string_view path)
    {
        std::vector<std::string> lines;
        auto data = ReadAllBytes(path);
        auto lineStart = reinterpret_cast<const char*>(data.data());
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

    void WriteAllBytes(std::string_view path, const void* buffer, size_t length)
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
        fs.Write(buffer, length);
    }

    uint64_t GetLastModified(std::string_view path)
    {
        return Platform::GetLastModified(path);
    }

    uint64_t GetSize(std::string_view path)
    {
        return Platform::GetFileSize(path);
    }
} // namespace File
