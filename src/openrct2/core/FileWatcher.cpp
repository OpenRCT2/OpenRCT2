/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
#elif defined(__APPLE__)
#    include <CoreServices/CoreServices.h>
#endif

#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/String.hpp"
#include "FileSystem.hpp"
#include "FileWatcher.h"
namespace OpenRCT2
{
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
            LOG_VERBOSE("FileWatcher: inotify_init succeeded");
        }
        else
        {
            LOG_VERBOSE("FileWatcher: inotify_init failed");
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
            LOG_VERBOSE("FileWatcher: inotify watch added for %s", path.c_str());
        }
        else
        {
            LOG_VERBOSE("FileWatcher: inotify_add_watch failed for %s", path.c_str());
            throw std::runtime_error("inotify_add_watch failed for '" + path + "'");
        }
    }

    FileWatcher::WatchDescriptor::~WatchDescriptor()
    {
        inotify_rm_watch(Fd, Wd);
        LOG_VERBOSE("FileWatcher: inotify watch removed");
    }
#endif

#if defined(__APPLE__)

    static int eventModified = kFSEventStreamEventFlagItemFinderInfoMod | kFSEventStreamEventFlagItemModified
        | kFSEventStreamEventFlagItemInodeMetaMod | kFSEventStreamEventFlagItemChangeOwner
        | kFSEventStreamEventFlagItemXattrMod;

    static int eventRenamed = kFSEventStreamEventFlagItemCreated | kFSEventStreamEventFlagItemRemoved
        | kFSEventStreamEventFlagItemRenamed;

    void FileWatcher::FSEventsCallback(
        ConstFSEventStreamRef streamRef, void* clientCallBackInfo, size_t numEvents, void* eventPaths,
        const FSEventStreamEventFlags eventFlags[], const FSEventStreamEventId eventIds[])
    {
        FileWatcher* fileWatcher = static_cast<FileWatcher*>(clientCallBackInfo);

        Guard::Assert(fileWatcher != nullptr, "FileWatcher is null");
        Guard::Assert(fileWatcher->OnFileChanged != nullptr, "OnFileChanged is null");

        char** paths = static_cast<char**>(eventPaths);
        for (size_t i = 0; i < numEvents; i++)
        {
            if (eventFlags[i] & eventModified)
                LOG_VERBOSE("Modified: %s\n", paths[i]);
            if (eventFlags[i] & eventRenamed)
                LOG_VERBOSE("Renamed: %s\n", paths[i]);

            if (eventFlags[i] & eventModified || eventFlags[i] & eventRenamed)
            {
                fileWatcher->OnFileChanged(paths[i]);
            }
        }
    }
#endif

    FileWatcher::FileWatcher(u8string_view directoryPath)
    {
#ifdef _WIN32
        _path = fs::u8path(directoryPath);
        _directoryHandle = CreateFileW(
            String::ToWideChar(directoryPath).c_str(), FILE_LIST_DIRECTORY,
            FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS,
            nullptr);
        if (_directoryHandle == INVALID_HANDLE_VALUE)
        {
            throw std::runtime_error("Unable to open directory '" + u8string(directoryPath) + "'");
        }
#elif defined(__linux__)
        _fileDesc.Initialise();
        _watchDescs.emplace_back(_fileDesc.Fd, u8string(directoryPath));
        for (auto& p : fs::recursive_directory_iterator(directoryPath))
        {
            if (p.status().type() == fs::file_type::directory)
            {
                _watchDescs.emplace_back(_fileDesc.Fd, p.path().string());
            }
        }
#elif defined(__APPLE__)
        CFStringRef path = CFStringCreateWithCString(kCFAllocatorDefault, directoryPath.data(), kCFStringEncodingUTF8);
        CFArrayRef pathsToWatch = CFArrayCreate(kCFAllocatorDefault, reinterpret_cast<const void**>(&path), 1, nullptr);
        CFAbsoluteTime latencyInSeconds = 0.5;
        FSEventStreamContext context = { 0, this, nullptr, nullptr, nullptr };

        _stream = FSEventStreamCreate(
            NULL, &FSEventsCallback, &context, pathsToWatch, kFSEventStreamEventIdSinceNow, latencyInSeconds,
            kFSEventStreamCreateFlagFileEvents);

        if (!_stream)
        {
            throw std::runtime_error("Unable to create FSEventStream");
        }

        _runLoop = CFRunLoopGetCurrent();
#else
        throw std::runtime_error("FileWatcher not supported on this platform.");
#endif
        _watchThread = std::thread(&FileWatcher::WatchDirectory, this);
    }

    FileWatcher::~FileWatcher()
    {
#ifdef _WIN32
        CancelIoEx(_directoryHandle, nullptr);
        _watchThread.join();
        CloseHandle(_directoryHandle);
#elif defined(__linux__)
        _finished = true;
        _watchThread.join();
        _fileDesc.Close();
#elif defined(__APPLE__)
        if (_stream)
        {
            FSEventStreamStop(_stream);
            FSEventStreamUnscheduleFromRunLoop(_stream, _runLoop, kCFRunLoopDefaultMode);
            FSEventStreamInvalidate(_stream);
            FSEventStreamRelease(_stream);
        }
        _watchThread.join();
        _stream = nullptr;
#else
        return;
#endif
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
            if (bytesReturned != 0 && OnFileChanged)
            {
                FILE_NOTIFY_INFORMATION* notifyInfo;
                size_t offset = 0;
                do
                {
                    notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(eventData.data() + offset);
                    offset += notifyInfo->NextEntryOffset;

                    std::wstring_view fileNameW(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(wchar_t));
                    auto path = _path / fs::path(fileNameW);
                    OnFileChanged(path.u8string());
                } while (notifyInfo->NextEntryOffset != 0);
            }
        }
#elif defined(__linux__)
        LOG_VERBOSE("FileWatcher: reading event data...");
        std::array<char, 1024> eventData;
        while (!_finished)
        {
            int length = read(_fileDesc.Fd, eventData.data(), eventData.size());
            if (length >= 0)
            {
                LOG_VERBOSE("FileWatcher: inotify event data received");
                auto onFileChanged = OnFileChanged;
                if (onFileChanged)
                {
                    int offset = 0;
                    while (offset < length)
                    {
                        auto e = reinterpret_cast<inotify_event*>(eventData.data() + offset);
                        if ((e->mask & IN_CLOSE_WRITE) && !(e->mask & IN_ISDIR))
                        {
                            LOG_VERBOSE("FileWatcher: inotify event received for %s", e->name);

                            // Find watch descriptor
                            int wd = e->wd;
                            auto findResult = std::find_if(
                                _watchDescs.begin(), _watchDescs.end(),
                                [wd](const WatchDescriptor& watchDesc) { return wd == watchDesc.Wd; });
                            if (findResult != _watchDescs.end())
                            {
                                onFileChanged(Path::Combine(findResult->Path, e->name));
                            }
                        }
                        offset += sizeof(inotify_event) + e->len;
                    }
                }
            }

            // Sleep for 1/2 second
            usleep(500000);
        }
#elif defined(__APPLE__)
        if (_stream)
        {
            FSEventStreamScheduleWithRunLoop(_stream, _runLoop, kCFRunLoopDefaultMode);
            FSEventStreamStart(_stream);
            CFRunLoopRun();
        }
#endif
    }
} // namespace OpenRCT2
