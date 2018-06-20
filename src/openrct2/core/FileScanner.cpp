/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../common.h"

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
#endif

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    #include <dirent.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <unistd.h>
#elif defined(_WIN32)
    // Windows needs this for widechar <-> utf8 conversion utils
    #include "../localisation/Language.h"
#endif

#include <memory>
#include <stack>
#include <string>
#include <vector>
#include "FileScanner.h"
#include "Memory.hpp"
#include "Path.hpp"
#include "String.hpp"

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
    uint64_t Size         = 0;
    uint64_t LastModified = 0;
};

static uint32_t GetPathChecksum(const utf8 * path);
static bool MatchWildcard(const utf8 * fileName, const utf8 * pattern);

class FileScannerBase : public IFileScanner
{
private:
    struct DirectoryState
    {
        std::string                 Path;
        std::vector<DirectoryChild> Listing;
        int32_t                      Index = 0;
    };

    // Options
    std::string                 _rootPath;
    std::vector<std::string>    _patterns;
    bool                        _recurse;

    // State
    bool                        _started = false;
    std::stack<DirectoryState>  _directoryStack;

    // Current
    FileInfo    * _currentFileInfo;
    utf8        * _currentPath;

public:
    FileScannerBase(const std::string &pattern, bool recurse)
    {
        _rootPath = Path::GetDirectory(pattern);
        _recurse = recurse;
        _patterns = GetPatterns(Path::GetFileName(pattern));

        _currentPath = Memory::Allocate<utf8>(MAX_PATH);
        _currentFileInfo = Memory::Allocate<FileInfo>();

        Reset();
    }

    ~FileScannerBase() override
    {
        Memory::Free(_currentPath);
        Memory::Free(_currentFileInfo);
    }

    const FileInfo * GetFileInfo() const override
    {
        return _currentFileInfo;
    }

    const utf8 * GetPath() const override
    {
        return _currentPath;
    }

    const utf8 * GetPathRelative() const override
    {
        // +1 to remove the path separator
        return _currentPath + _rootPath.size() + 1;
    }

    void Reset() override
    {
        _started = false;
        _directoryStack = std::stack<DirectoryState>();
        _currentPath[0] = 0;
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
            DirectoryState * state = &_directoryStack.top();
            state->Index++;
            if (state->Index >= (int32_t)state->Listing.size())
            {
                _directoryStack.pop();
            }
            else
            {
                const DirectoryChild * child = &state->Listing[state->Index];
                if (child->Type == DIRECTORY_CHILD_TYPE::DC_DIRECTORY)
                {
                    if (_recurse)
                    {
                        utf8 childPath[MAX_PATH];
                        String::Set(childPath, sizeof(childPath), state->Path.c_str());
                        Path::Append(childPath, sizeof(childPath), child->Name.c_str());

                        PushState(childPath);
                    }
                }
                else if (PatternMatch(child->Name))
                {
                    String::Set(_currentPath, MAX_PATH, state->Path.c_str());
                    Path::Append(_currentPath, MAX_PATH, child->Name.c_str());

                    _currentFileInfo->Name = child->Name.c_str();
                    _currentFileInfo->Size = child->Size;
                    _currentFileInfo->LastModified = child->LastModified;
                    return true;
                }
            }
        }
        return false;
    }

    virtual void GetDirectoryChildren(std::vector<DirectoryChild> &children, const std::string &path) abstract;

private:
    void PushState(const std::string &directory)
    {
        DirectoryState newState;
        newState.Path = directory;
        newState.Index = -1;
        GetDirectoryChildren(newState.Listing, directory);
        _directoryStack.push(newState);
    }

    bool PatternMatch(const std::string &fileName)
    {
        for (const auto &pattern : _patterns)
        {
            if (MatchWildcard(fileName.c_str(), pattern.c_str()))
            {
                return true;
            }
        }
        return false;
    }

    static std::vector<std::string> GetPatterns(const std::string &delimitedPatterns)
    {
        std::vector<std::string> patterns;

        const utf8 * start = delimitedPatterns.c_str();
        const utf8 * ch = start;
        utf8 c;
        do
        {
            c = *ch;
            if (c == '\0' || c == ';')
            {
                size_t length = (size_t)(ch - start);
                if (length > 0)
                {
                    std::string newPattern = std::string(start, length);
                    patterns.push_back(newPattern);
                }
                start = ch + 1;
            }
            ch++;
        }
        while (c != '\0');

        patterns.shrink_to_fit();
        return patterns;
    }
};

#ifdef _WIN32

class FileScannerWindows final : public FileScannerBase
{
public:
    FileScannerWindows(const std::string &pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void GetDirectoryChildren(std::vector<DirectoryChild> &children, const std::string &path) override
    {
        std::string pattern = path + "\\*";
        wchar_t * wPattern = utf8_to_widechar(pattern.c_str());

        WIN32_FIND_DATAW findData;
        HANDLE hFile = FindFirstFileW(wPattern, &findData);
        if (hFile != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (lstrcmpW(findData.cFileName, L".") != 0 &&
                    lstrcmpW(findData.cFileName, L"..") != 0)
                {
                    DirectoryChild child = CreateChild(&findData);
                    children.push_back(child);
                }
            }
            while (FindNextFileW(hFile, &findData));
            FindClose(hFile);
        }

        Memory::Free(wPattern);
    }

private:
    static DirectoryChild CreateChild(const WIN32_FIND_DATAW * child)
    {
        DirectoryChild result;

        utf8 * name = widechar_to_utf8(child->cFileName);
        result.Name = std::string(name);
        Memory::Free(name);

        if (child->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_DIRECTORY;
        }
        else
        {
            result.Type = DIRECTORY_CHILD_TYPE::DC_FILE;
            result.Size = ((uint64_t)child->nFileSizeHigh << 32ULL) | (uint64_t)child->nFileSizeLow;
            result.LastModified = ((uint64_t)child->ftLastWriteTime.dwHighDateTime << 32ULL) | (uint64_t)child->ftLastWriteTime.dwLowDateTime;
        }
        return result;
    }
};

#endif // _WIN32

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

class FileScannerUnix final : public FileScannerBase
{
public:
    FileScannerUnix(const std::string &pattern, bool recurse)
        : FileScannerBase(pattern, recurse)
    {
    }

    void GetDirectoryChildren(std::vector<DirectoryChild> &children, const std::string &path) override
    {
        struct dirent * * namelist;
        int32_t count = scandir(path.c_str(), &namelist, FilterFunc, alphasort);
        if (count > 0)
        {
            for (int32_t i = 0; i < count; i++)
            {
                const struct dirent * node = namelist[i];
                if (!String::Equals(node->d_name, ".") &&
                    !String::Equals(node->d_name, ".."))
                {
                    DirectoryChild child = CreateChild(path.c_str(), node);
                    children.push_back(child);
                }
                free(namelist[i]);
            }
            free(namelist);
        }
    }

private:
    static int32_t FilterFunc(const struct dirent * d)
    {
        return 1;
    }

    static DirectoryChild CreateChild(const utf8 * directory, const struct dirent * node)
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
            size_t pathSize = String::SizeOf(directory) + 1 + String::SizeOf(node->d_name) + 1;
            utf8 * path = Memory::Allocate<utf8>(pathSize);
            String::Set(path, pathSize, directory);
            Path::Append(path, pathSize, node->d_name);

            struct stat statInfo{};
            int32_t statRes = stat(path, &statInfo);
            if (statRes != -1)
            {
                result.Size = statInfo.st_size;
                result.LastModified = statInfo.st_mtime;

                if (S_ISDIR(statInfo.st_mode))
                {
                    result.Type = DIRECTORY_CHILD_TYPE::DC_DIRECTORY;
                }
            }

            Memory::Free(path);
        }
        return result;
    }
};

#endif // defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))

IFileScanner * Path::ScanDirectory(const std::string &pattern, bool recurse)
{
#ifdef _WIN32
    return new FileScannerWindows(pattern, recurse);
#elif defined(__unix__) || (defined(__APPLE__) && defined(__MACH__))
    return new FileScannerUnix(pattern, recurse);
#endif
}

void Path::QueryDirectory(QueryDirectoryResult * result, const std::string &pattern)
{
    IFileScanner * scanner = Path::ScanDirectory(pattern, true);
    while (scanner->Next())
    {
        const FileInfo * fileInfo = scanner->GetFileInfo();
        const utf8 * path = scanner->GetPath();

        result->TotalFiles++;
        result->TotalFileSize += fileInfo->Size;
        result->FileDateModifiedChecksum ^=
            (uint32_t)(fileInfo->LastModified >> 32) ^
            (uint32_t)(fileInfo->LastModified & 0xFFFFFFFF);
        result->FileDateModifiedChecksum = ror32(result->FileDateModifiedChecksum, 5);
        result->PathChecksum += GetPathChecksum(path);
    }
    delete scanner;
}

std::vector<std::string> Path::GetDirectories(const std::string &path)
{
    auto scanner = std::unique_ptr<IFileScanner>(ScanDirectory(path, false));
    auto baseScanner = static_cast<FileScannerBase *>(scanner.get());

    std::vector<DirectoryChild> children;
    baseScanner->GetDirectoryChildren(children, path);

    std::vector<std::string> subDirectories;
    for (const auto &c : children)
    {
        if (c.Type == DIRECTORY_CHILD_TYPE::DC_DIRECTORY)
        {
            subDirectories.push_back(c.Name);
        }
    }
    return subDirectories;
}

static uint32_t GetPathChecksum(const utf8 * path)
{
    uint32_t hash = 0xD8430DED;
    for (const utf8 * ch = path; *ch != '\0'; ch++)
    {
        hash += (*ch);
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
static bool MatchWildcard(const utf8 * fileName, const utf8 * pattern)
{
    while (*fileName != '\0')
    {
        switch (*pattern) {
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
            }
            while (*pattern == '*');
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
