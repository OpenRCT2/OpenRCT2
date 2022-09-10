/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <cstdio>
#include <stdexcept>

#ifdef _WIN32
#    include <windows.h>
#elif defined(__linux__)
#    include <fcntl.h>
#    include <sys/inotify.h>
#    include <sys/types.h>
#    include <unistd.h>
#endif

#include "../core/String.hpp"
#include "FileSystem.hpp"
#include "FileWatcher.h"

#if defined(__linux__)
FileWatcher::FileDescriptor::~FileDescriptor()
{
    Close();
}

void FileWatcher::FileDescriptor::Initialise()
{
    int fd = inotify_init();
    if (fd >= 0)
    {
        // Mark file as non-blocking
        int flags = fcntl(fd, F_GETFL, 0);
        fcntl(fd, F_SETFL, flags | O_NONBLOCK);

        Fd = fd;
        log_verbose("FileWatcher: inotify_init succeeded");
    }
    else
    {
        log_verbose("FileWatcher: inotify_init failed");
        throw std::runtime_error("inotify_init failed");
    }
}

void FileWatcher::FileDescriptor::Close()
{
    if (Fd != -1)
    {
        close(Fd);
        Fd = -1;
    }
}

FileWatcher::WatchDescriptor::WatchDescriptor(int fd, const std::string& path)
    : Fd(fd)
    , Wd(inotify_add_watch(fd, path.c_str(), IN_CLOSE_WRITE))
    , Path(path)
{
    if (Wd >= 0)
    {
        log_verbose("FileWatcher: inotify watch added for %s", path.c_str());
    }
    else
    {
        log_verbose("FileWatcher: inotify_add_watch failed for %s", path.data());
        throw std::runtime_error("inotify_add_watch failed for '" + std::string(path) + "'");
    }
}

FileWatcher::WatchDescriptor::~WatchDescriptor()
{
    inotify_rm_watch(Fd, Wd);
    log_verbose("FileWatcher: inotify watch removed");
}
#endif

FileWatcher::FileWatcher(std::string_view directoryPath)
{
#ifdef _WIN32
    _path = directoryPath;
    _directoryHandle = CreateFileA(
        directoryPath.data(), FILE_LIST_DIRECTORY, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,
        nullptr);
    if (_directoryHandle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Unable to open directory '" + _path + "'");
    }
#elif defined(__linux__)
    _fileDesc.Initialise();
    _watchDescs.emplace_back(_fileDesc.Fd, directoryPath);
    for (auto& p : fs::recursive_directory_iterator(directoryPath))
    {
        if (p.status().type() == fs::file_type::directory)
        {
            _watchDescs.emplace_back(_fileDesc.Fd, p.path().string());
        }
    }
#else
    throw std::runtime_error("FileWatcher not supported on this platform.");
#endif
    _watchThread = std::thread(std::bind(&FileWatcher::WatchDirectory, this));
}

FileWatcher::~FileWatcher()
{
#ifdef _WIN32
    CancelIoEx(_directoryHandle, nullptr);
    CloseHandle(_directoryHandle);
#elif defined(__linux__)
    _finished = true;
    _fileDesc.Close();
#else
    return;
#endif
    _watchThread.join();
}

void FileWatcher::WatchDirectory()
{
#if defined(_WIN32)
    std::array<char, 1024> eventData;
    DWORD bytesReturned;
    while (ReadDirectoryChangesW(
        _directoryHandle, eventData.data(), static_cast<DWORD>(eventData.size()), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE,
        &bytesReturned, nullptr, nullptr))
    {
        auto onFileChanged = OnFileChanged;
        if (onFileChanged)
        {
            FILE_NOTIFY_INFORMATION* notifyInfo;
            size_t offset = 0;
            do
            {
                notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(eventData.data() + offset);
                offset += notifyInfo->NextEntryOffset;

                std::wstring fileNameW(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(wchar_t));
                auto fileName = String::ToUtf8(fileNameW);
                auto path = fs::path(_path) / fs::path(fileName);
                onFileChanged(path.u8string());
            } while (notifyInfo->NextEntryOffset != 0);
        }
    }
#elif defined(__linux__)
    log_verbose("FileWatcher: reading event data...");
    std::array<char, 1024> eventData;
    while (!_finished)
    {
        int length = read(_fileDesc.Fd, eventData.data(), eventData.size());
        if (length >= 0)
        {
            log_verbose("FileWatcher: inotify event data received");
            auto onFileChanged = OnFileChanged;
            if (onFileChanged)
            {
                int offset = 0;
                while (offset < length)
                {
                    auto e = reinterpret_cast<inotify_event*>(eventData.data() + offset);
                    if ((e->mask & IN_CLOSE_WRITE) && !(e->mask & IN_ISDIR))
                    {
                        log_verbose("FileWatcher: inotify event received for %s", e->name);

                        // Find watch descriptor
                        int wd = e->wd;
                        auto findResult = std::find_if(
                            _watchDescs.begin(), _watchDescs.end(),
                            [wd](const WatchDescriptor& watchDesc) { return wd == watchDesc.Wd; });
                        if (findResult != _watchDescs.end())
                        {
                            auto directory = findResult->Path;
                            auto path = fs::path(directory) / fs::path(e->name);
                            onFileChanged(path.u8string());
                        }
                    }
                    offset += sizeof(inotify_event) + e->len;
                }
            }
        }

        // Sleep for 1/2 second
        usleep(500000);
    }
#endif
}
