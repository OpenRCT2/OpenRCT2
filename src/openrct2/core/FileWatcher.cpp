#include <array>
#include <stdexcept>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <fcntl.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "../core/String.hpp"
#include "FileWatcher.h"

FileWatcher::FileWatcher(const std::string &directoryPath)
{
#ifdef _WIN32
    _directoryHandle = CreateFileA(directoryPath.c_str(), FILE_LIST_DIRECTORY, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, nullptr);
    if (_directoryHandle == INVALID_HANDLE_VALUE)
    {
        throw std::runtime_error("Unable to open directory '" + directoryPath + "'");
    }
#else
    auto fd = inotify_init();
    if (fd >= 0)
    {
        auto wd = inotify_add_watch(fd, directoryPath.c_str(), IN_CLOSE_WRITE);
        if (wd >= 0)
        {
            _fileDesc = fd;
            _watchDesc = wd;
        }
        else
        {
            close(fd);
            throw std::runtime_error("Unable to watch directory '" + directoryPath + "'");
        }
    }
#endif
    _watchThread = std::thread(std::bind(&FileWatcher::WatchDirectory, this));
}

FileWatcher::~FileWatcher()
{
#ifdef _WIN32
    CancelIoEx(_directoryHandle, nullptr);
    CloseHandle(_directoryHandle);
#else
    inotify_rm_watch(_fileDesc, _watchDesc);
    close(_fileDesc);
#endif
    _watchThread.join();
}

void FileWatcher::WatchDirectory()
{
#ifdef _WIN32
    std::array<char, 1024> eventData;
    DWORD bytesReturned;
    while (ReadDirectoryChangesW(_directoryHandle, eventData.data(), (DWORD)eventData.size(), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, &bytesReturned, nullptr, nullptr))
    {
        auto onFileChanged = OnFileChanged;
        if (onFileChanged)
        {
            FILE_NOTIFY_INFORMATION * notifyInfo;
            size_t offset = 0;
            do
            {
                notifyInfo = (FILE_NOTIFY_INFORMATION*)(eventData.data() + offset);
                offset += notifyInfo->NextEntryOffset;

                std::wstring fileNameW(notifyInfo->FileName, notifyInfo->FileNameLength / sizeof(wchar_t));
                auto fileName = String::ToUtf8(fileNameW);
                onFileChanged(fileName);
            }
            while (notifyInfo->NextEntryOffset != 0);
        }
    }
#else
    std::array<char, 1024> eventData;
    auto length = read(_fileDesc, eventData.data(), eventData.size());
    if (length >= 0)
    {
        auto onFileChanged = OnFileChanged;
        if (onFileChanged)
        {
            int offset = 0;
            while (offset < length)
            {
                auto e = (inotify_event*)(eventData.data() + offset);
                if ((e->mask & IN_CLOSE_WRITE) && !(e->mask & IN_ISDIR))
                {
                    onFileChanged(e->name);
                }
                offset += sizeof(inotify_event) + e->len;
            }
        }
    }
#endif
}
