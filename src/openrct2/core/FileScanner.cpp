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
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    #include <dirent.h>
    #include <sys/stat.h>
    #include <sys/types.h>
    #include <unistd.h>
#elif defined(_WIN32)
    // Windows needs this for widechar <-> utf8 conversion utils
    #include "../localisation/Language.h"
#endif

#include "FileScanner.h"
#include "Memory.hpp"
#include "Numerics.hpp"
#include "Path.hpp"
#include "String.hpp"

#include <memory>
#include <stack>
#include <string>
#include <vector>

using namespace OpenRCT2;

enum class DIRECTORY_CHILD_TYPE
{
    DC_DIRECTORY,
    DC_FILE,
};

struct DirectoryChild
{
    DIRECTORY_CHILD_TYPE Type;
    std::string Name;

    // Files only
    uint64_t Size = 0;
    uint64_t LastModified = 0;
};

static uint32_t GetPathChecksum(u8string_view path);
static bool MatchWildcard(const utf8* fileName, const utf8* pattern);

class FileScannerBase : public IFileScanner
{
private:
    struct DirectoryState
    {
        u8string Path;
        std::vector<DirectoryChild> Listing;
        int32_t Index = 0;
    };

    // Options
    const u8string _rootPath;
    const std::vector<std::string> _patterns;
    const bool _recurse;

    // State
    bool _started = false;
    std::stack<DirectoryState> _directoryStack;

    // Current
    FileScanner::FileInfo _currentFileInfo;
    u8string _currentPath;

public:
    FileScannerBase(u8string_view pattern, bool recurse)
        : _rootPath(Path::GetDirectory(pattern))
        , _patterns(GetPatterns(Path::GetFileName(pattern)))
        , _recurse(recurse)
    {
    }

    ~FileScannerBase() override = default;

    const FileScanner::FileInfo& GetFileInfo() const override
    {
        return _currentFileInfo;
    }

    const u8string& GetPath() const override
    {
        return _currentPath;
    }

    u8string GetPathRelative() const override
    {
        return Path::GetRelative(_currentPath, _rootPath);
    }

    void Reset() override
    {
        _started = false;
        _directoryStack = {};
        _currentPath.clear();
    }

    bool Next() override
    {
        if (!_started)
        {
            _started = true;
            PushState(_rootPath);
        }

        while (!_directoryStack.empty())
        {
            DirectoryState* state = &_directoryStack.top();
            state->Index++;
            if (state->Index >= static_cast<int32_t>(state->Listing.size()))
            {
                _directoryStack.pop();
            }
            else
            {
                const DirectoryChild* child = &state->Listing[state->Index];
                if (child->Type == DIRECTORY_CHILD_TYPE::DC_DIRECTORY)
                {
                    if (_recurse)
                    {
                        auto childPath = Path::Combine(state->Path, child->Name);
                        PushState(childPath);
                    }
                }
                else if (PatternMatch(child->Name))
                {
                    _currentPath = Path::Combine(state->Path, child->Name);

                    _currentFileInfo.Name = child->Name;
                    _currentFileInfo.Size = child->Size;
                    _currentFileInfo.LastModified = child->LastModified;
                    return true;
                }
            }
        }
        return false;
    }

    virtual void GetDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) = 0;

private:
    void PushState(const std::string& directory)
    {
        DirectoryState newState;
        newState.Path = directory;
        newState.Index = -1;
        GetDirectoryChildren(newState.Listing, directory);
        _directoryStack.push(newState);
    }

    bool PatternMatch(const std::string& fileName)
    {
        for (const auto& pattern : _patterns)
        {
            if (MatchWildcard(fileName.c_str(), pattern.c_str()))
            {
                return true;
            }
        }
        return false;
    }

    static std::vector<std::string> GetPatterns(const std::string& delimitedPatterns)
    {
        std::vector<std::string> patterns;

        const utf8* start = delimitedPatterns.c_str();
        const utf8* ch = start;
        utf8 c;
        do
        {
            c = *ch;
            if (c == '\0' || c == ';')
            {
                size_t length = static_cast<size_t>(ch - start);
                if (length > 0)
                {
                    patterns.emplace_back(start, length);
                }
                start = ch + 1;
            }
            ch++;
        } while (c != '\0');

        patterns.shrink_to_fit();
        return patterns;
    }
};

#ifdef _WIN32

class FileScannerWindows final : public FileScannerBase
{
public:
    FileScannerWindows(u8string_view pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void GetDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) override
    {
        auto pattern = path + "\\*";
        auto wPattern = String::toWideChar(pattern.c_str());

        WIN32_FIND_DATAW findData;
        HANDLE hFile = FindFirstFileW(wPattern.c_str(), &findData);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (lstrcmpW(findData.cFileName, L".") != 0 && lstrcmpW(findData.cFileName, L"..") != 0)
                {
                    children.push_back(CreateChild(&findData));
                }
            } while (FindNextFileW(hFile, &findData));
            FindClose(hFile);
        }
    }

private:
    static DirectoryChild CreateChild(const WIN32_FIND_DATAW* child)
    {
        DirectoryChild result;

        result.Name = String::toUtf8(child->cFileName);
        if (child->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_DIRECTORY;
        }
        else
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_FILE;
            result.Size = (static_cast<uint64_t>(child->nFileSizeHigh) << 32uLL) | static_cast<uint64_t>(child->nFileSizeLow);
            result.LastModified = (static_cast<uint64_t>(child->ftLastWriteTime.dwHighDateTime) << 32uLL)
                | static_cast<uint64_t>(child->ftLastWriteTime.dwLowDateTime);
        }
        return result;
    }
};

#endif // _WIN32

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

class FileScannerUnix final : public FileScannerBase
{
public:
    FileScannerUnix(u8string_view pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void GetDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) override
    {
        struct dirent** namelist;
        int32_t count = scandir(path.c_str(), &namelist, FilterFunc, alphasort);
        if (count > 0)
        {
            for (int32_t i = 0; i < count; i++)
            {
                const struct dirent* node = namelist[i];
                if (!String::equals(node->d_name, ".") && !String::equals(node->d_name, ".."))
                {
                    children.push_back(CreateChild(path.c_str(), node));
                }
                free(namelist[i]);
            }
            free(namelist);
        }
    }

private:
    static int32_t FilterFunc(const struct dirent* d)
    {
        return 1;
    }

    static DirectoryChild CreateChild(const utf8* directory, const struct dirent* node)
    {
        DirectoryChild result;
        result.Name = std::string(node->d_name);
        if (node->d_type == DT_DIR)
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_DIRECTORY;
        }
        else
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_FILE;

            // Get the full path of the file
            auto path = Path::Combine(directory, node->d_name);

            struct stat statInfo{};
            int32_t statRes = stat(path.c_str(), &statInfo);
            if (statRes != -1)
            {
                result.Size = statInfo.st_size;
                result.LastModified = statInfo.st_mtime;

                if (S_ISDIR(statInfo.st_mode))
                {
                    result.Type = DIRECTORY_CHILD_TYPE::DC_DIRECTORY;
                }
            }
        }
        return result;
    }
};

#endif // defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

std::unique_ptr<IFileScanner> Path::ScanDirectory(const std::string& pattern, bool recurse)
{
#ifdef _WIN32
    return std::make_unique<FileScannerWindows>(pattern, recurse);
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    return std::make_unique<FileScannerUnix>(pattern, recurse);
#endif
}

void Path::QueryDirectory(QueryDirectoryResult* result, const std::string& pattern)
{
    auto scanner = Path::ScanDirectory(pattern, true);
    while (scanner->Next())
    {
        const FileScanner::FileInfo& fileInfo = scanner->GetFileInfo();
        const u8string& path = scanner->GetPath();

        result->TotalFiles++;
        result->TotalFileSize += fileInfo.Size;
        result->FileDateModifiedChecksum ^= static_cast<uint32_t>(fileInfo.LastModified >> 32)
            ^ static_cast<uint32_t>(fileInfo.LastModified & 0xFFFFFFFF);
        result->FileDateModifiedChecksum = Numerics::ror32(result->FileDateModifiedChecksum, 5);
        result->PathChecksum += GetPathChecksum(path);
    }
}

std::vector<std::string> Path::GetDirectories(const std::string& path)
{
    auto scanner = ScanDirectory(path, false);
    auto baseScanner = static_cast<FileScannerBase*>(scanner.get());

    std::vector<DirectoryChild> children;
    baseScanner->GetDirectoryChildren(children, path);

    std::vector<std::string> subDirectories;
    for (const auto& c : children)
    {
        if (c.Type == DIRECTORY_CHILD_TYPE::DC_DIRECTORY)
        {
            subDirectories.push_back(c.Name);
        }
    }
    return subDirectories;
}

static uint32_t GetPathChecksum(u8string_view path)
{
    uint32_t hash = 0xD8430DED;
    for (const utf8 ch : path)
    {
        hash += ch;
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

/**
 * Due to FindFirstFile / FindNextFile searching for DOS names as well, *.doc also matches *.docx which isn't what the pattern
 * specified. This will verify if a filename does indeed match the pattern we asked for.
 * @remarks Based on algorithm (http://xoomer.virgilio.it/acantato/dev/wildcard/wildmatch.html)
 */
static bool MatchWildcard(const utf8* fileName, const utf8* pattern)
{
    while (*fileName != '\0')
    {
        switch (*pattern)
        {
            case '?':
                if (*fileName == '.')
                {
                    return false;
                }
                break;
            case '*':
                do
                {
                    pattern++;
                } while (*pattern == '*');
                if (*pattern == '\0')
                {
                    return false;
                }
                while (*fileName != '\0')
                {
                    if (MatchWildcard(fileName++, pattern))
                    {
                        return true;
                    }
                }
                return false;
            default:
                if (toupper(*fileName) != toupper(*pattern))
                {
                    return false;
                }
                break;
        }
        pattern++;
        fileName++;
    }
    while (*pattern == '*')
    {
        ++fileName;
    }
    return *pattern == '\0';
}
