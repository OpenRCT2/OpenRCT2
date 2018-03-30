#pragma once

#include <functional>
#include <string>
#include <thread>

#ifdef _WIN32
typedef void * HANDLE;
#endif

/**
 * Creates a new thread that watches a directory tree for file modifications.
 */
class FileWatcher
{
private:
    std::thread _watchThread;
#ifdef _WIN32
    HANDLE _directoryHandle{};
#else
    int _fileDesc{};
    int _watchDesc{};
#endif

public:
    std::function<void(const std::string &path)> OnFileChanged;

    FileWatcher(const std::string &directoryPath);
    ~FileWatcher();

private:
    void WatchDirectory();
};
