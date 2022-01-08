/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__unix__) || (defined(__APPLE__) && defined(__MACH__)) || defined(__FreeBSD__)

#    include "platform.h"

#    include "../core/Memory.hpp"
#    include "../core/Path.hpp"
#    include "../core/String.hpp"
#    include "Platform2.h"

#    include <cerrno>
#    include <clocale>
#    include <cstdlib>
#    include <cstring>
#    include <ctime>
#    include <dirent.h>
#    include <pwd.h>
#    include <sys/stat.h>

#    define FILE_BUFFER_SIZE 4096

namespace Platform
{
    uint32_t GetTicks()
    {
        return platform_get_ticks();
    }

    std::string GetEnvironmentVariable(std::string_view name)
    {
        return String::ToStd(getenv(std::string(name).c_str()));
    }

    std::string GetEnvironmentPath(const char* name)
    {
        auto value = getenv(name);
        if (value == nullptr)
        {
            return std::string();
        }

        auto colon = std::strchr(value, ':');
        if (colon == nullptr)
        {
            return std::string(value);
        }

        return std::string(value, colon);
    }

    std::string GetHomePath()
    {
        std::string path;
        auto pw = getpwuid(getuid());
        if (pw != nullptr)
        {
            path = pw->pw_dir;
        }
        else
        {
            path = GetEnvironmentVariable("HOME");
        }
        if (path.empty())
        {
            path = "/";
        }
        return path;
    }

    std::string FormatShortDate(std::time_t timestamp)
    {
        setlocale(LC_TIME, "");
        char date[20];
        std::strftime(date, sizeof(date), "%x", std::localtime(&timestamp));
        return std::string(date);
    }

    std::string FormatTime(std::time_t timestamp)
    {
        setlocale(LC_TIME, "");
        char time[20];
        std::strftime(time, sizeof(time), "%X", std::localtime(&timestamp));
        return std::string(time);
    }

    bool IsColourTerminalSupported()
    {
        static bool hasChecked = false;
        static bool isSupported = false;
        if (!hasChecked)
        {
            auto term = GetEnvironmentVariable("TERM");
            isSupported = term != "cons25" && term != "dumb" && term != "emacs";
            hasChecked = true;
        }
        return isSupported;
    }

    bool IsRunningInWine()
    {
        return false;
    }

    bool FindApp(std::string_view app, std::string* output)
    {
        return Execute(String::StdFormat("which %s 2> /dev/null", std::string(app).c_str()), output) == 0;
    }

    int32_t Execute(std::string_view command, std::string* output)
    {
#    ifndef __EMSCRIPTEN__
        log_verbose("executing \"%s\"...", std::string(command).c_str());
        FILE* fpipe = popen(std::string(command).c_str(), "r");
        if (fpipe == nullptr)
        {
            return -1;
        }
        if (output != nullptr)
        {
            // Read output into buffer
            std::vector<char> outputBuffer;
            char buffer[1024];
            size_t readBytes;
            while ((readBytes = fread(buffer, 1, sizeof(buffer), fpipe)) > 0)
            {
                outputBuffer.insert(outputBuffer.begin(), buffer, buffer + readBytes);
            }

            // Trim line breaks
            size_t outputLength = outputBuffer.size();
            for (size_t i = outputLength - 1; i != SIZE_MAX; i--)
            {
                if (outputBuffer[i] == '\n')
                {
                    outputLength = i;
                }
                else
                {
                    break;
                }
            }

            // Convert to string
            *output = std::string(outputBuffer.data(), outputLength);
        }
        else
        {
            fflush(fpipe);
        }

        // Return exit code
        return pclose(fpipe);
#    else
        log_warning("Emscripten cannot execute processes. The commandline was '%s'.", command.c_str());
        return -1;
#    endif // __EMSCRIPTEN__
    }

    uint64_t GetLastModified(std::string_view path)
    {
        uint64_t lastModified = 0;
        struct stat statInfo
        {
        };
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            lastModified = statInfo.st_mtime;
        }
        return lastModified;
    }

    uint64_t GetFileSize(std::string_view path)
    {
        uint64_t size = 0;
        struct stat statInfo
        {
        };
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            size = statInfo.st_size;
        }
        return size;
    }

    bool ShouldIgnoreCase()
    {
        return false;
    }

    bool IsPathSeparator(char c)
    {
        return c == '/';
    }

    utf8* GetAbsolutePath(utf8* buffer, size_t bufferSize, const utf8* relativePath)
    {
        utf8* absolutePath = realpath(relativePath, nullptr);
        if (absolutePath == nullptr)
        {
            return String::Set(buffer, bufferSize, relativePath);
        }

        String::Set(buffer, bufferSize, absolutePath);
        Memory::Free(absolutePath);
        return buffer;
    }

    std::string ResolveCasing(std::string_view path, bool fileExists)
    {
        std::string result;
        if (fileExists)
        {
            // Windows is case insensitive so it will exist and that is all that matters
            // for now. We can properly resolve the casing if we ever need to.
            result = path;
        }
        else
        {
            std::string fileName = Path::GetFileName(path);
            std::string directory = Path::GetDirectory(path);

            struct dirent** files;
            auto count = scandir(directory.c_str(), &files, nullptr, alphasort);
            if (count != -1)
            {
                // Find a file which matches by name (case insensitive)
                for (int32_t i = 0; i < count; i++)
                {
                    if (String::Equals(files[i]->d_name, fileName.c_str(), true))
                    {
                        result = Path::Combine(directory, std::string(files[i]->d_name));
                        break;
                    }
                }

                // Free memory
                for (int32_t i = 0; i < count; i++)
                {
                    free(files[i]);
                }
                free(files);
            }
        }
        return result;
    }

    bool RequireNewWindow(bool openGL)
    {
        return true;
    }

    std::string GetUsername()
    {
        std::string result;
        auto pw = getpwuid(getuid());
        if (pw != nullptr)
        {
            result = std::string(pw->pw_name);
        }
        return result;
    }

    bool CopyFile(std::string_view srcPath, std::string_view dstPath, bool overwrite)
    {
        log_verbose("Copying %s to %s", std::string(srcPath).c_str(), std::string(dstPath).c_str());

        FILE* dstFile;

        if (overwrite)
        {
            dstFile = fopen(std::string(std::string(dstPath).c_str()).c_str(), "wb");
        }
        else
        {
            // Portability note: check your libc's support for "wbx"
            dstFile = fopen(std::string(dstPath).c_str(), "wbx");
        }

        if (dstFile == nullptr)
        {
            if (errno == EEXIST)
            {
                log_warning(
                    "Platform::CopyFile(): Not overwriting %s, because overwrite flag == false", std::string(dstPath).c_str());
                return false;
            }

            log_error("Could not open destination file %s for copying", std::string(dstPath).c_str());
            return false;
        }

        // Open both files and check whether they are opened correctly
        FILE* srcFile = fopen(std::string(srcPath).c_str(), "rb");
        if (srcFile == nullptr)
        {
            fclose(dstFile);
            log_error("Could not open source file %s for copying", std::string(srcPath).c_str());
            return false;
        }

        size_t amount_read = 0;
        size_t file_offset = 0;

        // Copy file in FILE_BUFFER_SIZE-d chunks
        char* buffer = static_cast<char*>(malloc(FILE_BUFFER_SIZE));
        while ((amount_read = fread(buffer, FILE_BUFFER_SIZE, 1, srcFile)))
        {
            fwrite(buffer, amount_read, 1, dstFile);
            file_offset += amount_read;
        }

        // Finish the left-over data from file, which may not be a full
        // FILE_BUFFER_SIZE-d chunk.
        fseek(srcFile, file_offset, SEEK_SET);
        amount_read = fread(buffer, 1, FILE_BUFFER_SIZE, srcFile);
        fwrite(buffer, amount_read, 1, dstFile);

        fclose(srcFile);
        fclose(dstFile);
        free(buffer);

        return true;
    }

    bool MoveFile(std::string_view srcPath, std::string_view dstPath)
    {
        return rename(std::string(srcPath).c_str(), std::string(dstPath).c_str()) == 0;
    }

    bool DeleteFile(std::string_view path)
    {
        int32_t ret = unlink(std::string(path).c_str());
        return ret == 0;
    }

} // namespace Platform

#endif
