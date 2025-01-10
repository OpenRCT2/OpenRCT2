/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/stat.h>
#endif

#include "../Diagnostic.h"
#include "../platform/Platform.h"
#include "File.h"
#include "FileStream.h"
#include "String.hpp"

#include <fstream>

namespace OpenRCT2::File
{
    bool Exists(u8string_view path)
    {
        fs::path file = fs::u8path(path);
        LOG_VERBOSE("Checking if file exists: %s", u8string(path).c_str());
        std::error_code ec;
        const auto result = fs::exists(file, ec);
        return result && ec.value() == 0;
    }

    bool Copy(u8string_view srcPath, u8string_view dstPath, bool overwrite)
    {
        if (!overwrite && Exists(dstPath))
        {
            LOG_WARNING("File::Copy(): Not overwriting %s, because overwrite flag == false", u8string(dstPath).c_str());
            return false;
        }

        std::error_code ec;
        const auto result = fs::copy_file(fs::u8path(srcPath), fs::u8path(dstPath), ec);
        return result && ec.value() == 0;
    }

    bool Delete(u8string_view path)
    {
        std::error_code ec;
        const auto result = fs::remove(fs::u8path(path), ec);
        return result && ec.value() == 0;
    }

    bool Move(u8string_view srcPath, u8string_view dstPath)
    {
        std::error_code ec;
        fs::rename(fs::u8path(srcPath), fs::u8path(dstPath), ec);
        return ec.value() == 0;
    }

    std::vector<uint8_t> ReadAllBytes(u8string_view path)
    {
        std::ifstream fs(fs::u8path(u8string(path)), std::ios::in | std::ios::binary);
        if (!fs.is_open())
        {
            throw IOException("Unable to open " + u8string(path));
        }

        std::vector<uint8_t> result;
        auto fsize = Platform::GetFileSize(path);
        if (fsize > SIZE_MAX)
        {
            u8string message = String::stdFormat(
                "'%s' exceeds maximum length of %lld bytes.", u8string(path).c_str(), SIZE_MAX);
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

    u8string ReadAllText(u8string_view path)
    {
        auto bytes = ReadAllBytes(path);
        // TODO skip BOM
        u8string result(bytes.size(), 0);
        std::copy(bytes.begin(), bytes.end(), result.begin());
        return result;
    }

    std::vector<u8string> ReadAllLines(u8string_view path)
    {
        std::vector<u8string> lines;
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

    void WriteAllBytes(u8string_view path, const void* buffer, size_t length)
    {
        auto fs = OpenRCT2::FileStream(path, OpenRCT2::FILE_MODE_WRITE);
        fs.Write(buffer, length);
    }

    uint64_t GetLastModified(u8string_view path)
    {
        return Platform::GetLastModified(path);
    }

    uint64_t GetSize(u8string_view path)
    {
        return Platform::GetFileSize(path);
    }
} // namespace OpenRCT2::File
