/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32
    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif
    #include <windows.h>
#elif defined(__unix__) || defined(__HAIKU__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__ANDROID__)
    #include <dirent.h>
    #include <sys/stat.h>
#endif

#include "../platform/Platform.h"
#include "FileScanner.h"
#include "Numerics.hpp"
#include "Path.hpp"
#include "String.hpp"

#include <memory>
#include <set>
#include <stack>
#include <string>
#include <vector>

using namespace OpenRCT2;

enum class DirectoryChildType
{
    directory,
    file,
};

struct DirectoryChild
{
    DirectoryChildType type;
    std::string name;

    // Files only
    uint64_t size = 0;
    uint64_t lastModified = 0;
};

static uint32_t getPathChecksum(u8string_view path);
static bool matchWildcard(const utf8* fileName, const utf8* pattern);

class FileScannerBase : public IFileScanner
{
private:
    struct DirectoryState
    {
        u8string path;
        std::vector<DirectoryChild> listing;
        int32_t index = 0;
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
        , _patterns(getPatterns(Path::GetFileName(pattern)))
        , _recurse(recurse)
    {
    }

    ~FileScannerBase() override = default;

    const FileScanner::FileInfo& getFileInfo() const override
    {
        return _currentFileInfo;
    }

    const u8string& getPath() const override
    {
        return _currentPath;
    }

    u8string getPathRelative() const override
    {
        return Path::GetRelative(_currentPath, _rootPath);
    }

    void reset() override
    {
        _started = false;
        _directoryStack = {};
        _currentPath.clear();
    }

    bool next() override
    {
        if (!_started)
        {
            _started = true;
            pushState(_rootPath);
        }

        while (!_directoryStack.empty())
        {
            DirectoryState* state = &_directoryStack.top();
            state->index++;
            if (state->index >= static_cast<int32_t>(state->listing.size()))
            {
                _directoryStack.pop();
            }
            else
            {
                const DirectoryChild* child = &state->listing[state->index];
                if (child->type == DirectoryChildType::directory)
                {
                    if (_recurse)
                    {
                        auto childPath = Path::Combine(state->path, child->name);
                        pushState(childPath);
                    }
                }
                else if (patternMatch(child->name))
                {
                    _currentPath = Path::Combine(state->path, child->name);

                    _currentFileInfo.name = child->name;
                    _currentFileInfo.size = child->size;
                    _currentFileInfo.lastModified = child->lastModified;
                    return true;
                }
            }
        }
        return false;
    }

    virtual void getDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) = 0;

private:
    void pushState(const std::string& directory)
    {
        DirectoryState newState;
        newState.path = directory;
        newState.index = -1;
        getDirectoryChildren(newState.listing, directory);
        _directoryStack.push(newState);
    }

    bool patternMatch(const std::string& fileName)
    {
        for (const auto& pattern : _patterns)
        {
            if (matchWildcard(fileName.c_str(), pattern.c_str()))
            {
                return true;
            }
        }
        return false;
    }

    static std::vector<std::string> getPatterns(const std::string& delimitedPatterns)
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

    void getDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) override
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
                    children.push_back(createChild(&findData));
                }
            } while (FindNextFileW(hFile, &findData));
            FindClose(hFile);
        }
    }

private:
    static DirectoryChild createChild(const WIN32_FIND_DATAW* child)
    {
        DirectoryChild result;

        result.name = String::toUtf8(child->cFileName);
        if (child->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.type = DirectoryChildType::directory;
        }
        else
        {
            result.type = DirectoryChildType::file;
            result.size = (static_cast<uint64_t>(child->nFileSizeHigh) << 32uLL) | static_cast<uint64_t>(child->nFileSizeLow);
            result.lastModified = (static_cast<uint64_t>(child->ftLastWriteTime.dwHighDateTime) << 32uLL)
                | static_cast<uint64_t>(child->ftLastWriteTime.dwLowDateTime);
        }
        return result;
    }
};

#endif // _WIN32

#ifdef __ANDROID__

class FileScannerAndroidAssets final : public FileScannerBase
{
public:
    FileScannerAndroidAssets(u8string_view pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void getDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) override
    {
        const auto& assetList = Platform::GetAssetList();
        std::string prefix = path.substr(Platform::kAndroidAssetPathPrefix.length());
        if (!prefix.empty() && prefix.back() != '/')
        {
            prefix += '/';
        }

        std::set<std::string> seen;

        for (const auto& entry : assetList)
        {
            if (entry.path.size() > prefix.size() && String::startsWith(entry.path, prefix))
            {
                std::string_view relative = std::string_view(entry.path).substr(prefix.size());
                auto slashPos = relative.find('/');
                if (slashPos != std::string_view::npos)
                {
                    std::string dirName = std::string(relative.substr(0, slashPos));
                    if (seen.insert(dirName).second)
                    {
                        DirectoryChild child;
                        child.name = dirName;
                        child.type = DirectoryChildType::directory;
                        children.push_back(child);
                    }
                }
                else
                {
                    std::string fileName = std::string(relative);
                    if (seen.insert(fileName).second)
                    {
                        DirectoryChild child;
                        child.name = fileName;
                        child.type = DirectoryChildType::file;
                        child.size = entry.size;

                        children.push_back(child);
                    }
                }
            }
        }
    }
};
#endif // __ANDROID__

#if defined(__unix__) || defined(__HAIKU__) || (defined(__APPLE__) && defined(__MACH__))

class FileScannerUnix final : public FileScannerBase
{
public:
    FileScannerUnix(u8string_view pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void getDirectoryChildren(std::vector<DirectoryChild>& children, const std::string& path) override
    {
        struct dirent** namelist;
        int32_t count = scandir(path.c_str(), &namelist, filterFunc, alphasort);
        if (count > 0)
        {
            for (int32_t i = 0; i < count; i++)
            {
                const struct dirent* node = namelist[i];
                if (!String::equals(node->d_name, ".") && !String::equals(node->d_name, ".."))
                {
                    children.push_back(createChild(path.c_str(), node));
                }
                free(namelist[i]);
            }
            free(namelist);
        }
    }

private:
    static int32_t filterFunc(const struct dirent* d)
    {
        return 1;
    }

    static DirectoryChild createChild(const utf8* directory, const struct dirent* node)
    {
        DirectoryChild result;
        result.name = std::string(node->d_name);
    #ifdef __HAIKU__
        struct stat stbuf;
        stat(node->d_name, &stbuf);
        if (S_ISDIR(stbuf.st_mode))
    #else
        if (node->d_type == DT_DIR)
    #endif
        {
            result.type = DirectoryChildType::directory;
        }
        else
        {
            result.type = DirectoryChildType::file;

            // Get the full path of the file
            auto path = Path::Combine(directory, node->d_name);

            struct stat statInfo{};
            int32_t statRes = stat(path.c_str(), &statInfo);
            if (statRes != -1)
            {
                result.size = statInfo.st_size;
                result.lastModified = statInfo.st_mtime;

                if (S_ISDIR(statInfo.st_mode))
                {
                    result.type = DirectoryChildType::directory;
                }
            }
        }
        return result;
    }
};

#endif // defined(__unix__) || defined(__HAIKU__) || (defined(__APPLE__) && defined(__MACH__))

std::unique_ptr<IFileScanner> Path::scanDirectory(const std::string& pattern, bool recurse)
{
#ifdef __ANDROID__
    if (String::startsWith(pattern, Platform::kAndroidAssetPathPrefix))
    {
        return std::make_unique<FileScannerAndroidAssets>(pattern, recurse);
    }
#endif
#ifdef _WIN32
    return std::make_unique<FileScannerWindows>(pattern, recurse);
#elif defined(__unix__) || defined(__HAIKU__) || (defined(__APPLE__) && defined(__MACH__))
    return std::make_unique<FileScannerUnix>(pattern, recurse);
#endif
}

void Path::queryDirectory(QueryDirectoryResult* result, const std::string& pattern)
{
    auto scanner = scanDirectory(pattern, true);
    while (scanner->next())
    {
        const FileScanner::FileInfo& fileInfo = scanner->getFileInfo();
        const u8string& path = scanner->getPath();

        result->totalFiles++;
        result->totalFileSize += fileInfo.size;
        result->fileDateModifiedChecksum ^= static_cast<uint32_t>(fileInfo.lastModified >> 32)
            ^ static_cast<uint32_t>(fileInfo.lastModified & 0xFFFFFFFF);
        result->fileDateModifiedChecksum = Numerics::ror32(result->fileDateModifiedChecksum, 5);
        result->pathChecksum += getPathChecksum(path);
    }
}

std::vector<std::string> Path::getDirectories(const std::string& path)
{
    auto scanner = scanDirectory(path, false);
    auto baseScanner = static_cast<FileScannerBase*>(scanner.get());

    std::vector<DirectoryChild> children;
    baseScanner->getDirectoryChildren(children, path);

    std::vector<std::string> subDirectories;
    for (const auto& c : children)
    {
        if (c.type == DirectoryChildType::directory)
        {
            subDirectories.push_back(c.name);
        }
    }
    return subDirectories;
}

static uint32_t getPathChecksum(u8string_view path)
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
static bool matchWildcard(const utf8* fileName, const utf8* pattern)
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
                    if (matchWildcard(fileName++, pattern))
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
