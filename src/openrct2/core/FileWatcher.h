#pragma once

#include <functional>
#include <string>
#include <thread>
#include <vector>

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
    struct FileDescriptor
    {
        int Fd = -1;

        ~FileDescriptor();
        void Initialise();
        void Close();
    };

    struct WatchDescriptor
    {
        int const Fd;
        int const Wd;
        std::string const Path;

        WatchDescriptor(int fd, const std::string& path);
        ~WatchDescriptor();
    };

    FileDescriptor _fileDesc;
    std::vector<WatchDescriptor> _watchDescs;
#endif

public:
    std::function<void(const std::string &path)> OnFileChanged;

    FileWatcher(const std::string &directoryPath);
    ~FileWatcher();

private:
    bool _finished{};

    void WatchDirectory();
};
