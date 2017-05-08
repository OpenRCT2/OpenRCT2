#pragma region Copyright (c) 2014-2016 OpenRCT2 Developers
/*****************************************************************************
* OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
*
* OpenRCT2 is the work of many authors, a full list can be found in contributors.md
* For more information, visit https://github.com/OpenRCT2/OpenRCT2
*
* OpenRCT2 is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* A full copy of the GNU General Public License can be found in licence.txt
*****************************************************************************/
#pragma endregion

#ifdef __linux__

#include <dlfcn.h>
#include <sstream>
#include <openrct2/common.h>
#include <openrct2/core/String.hpp>
#include <openrct2/ui/UiContext.h>
#include "UiContext.h"

#include <SDL.h>

namespace OpenRCT2 { namespace Ui
{
    enum class DIALOG_TYPE
    {
        NONE,
        KDIALOG,
        ZENITY,
    };

    class LinuxContext final : public IPlatformUiContext
    {
    private:

    public:
        LinuxContext()
        {
        }

        void SetWindowIcon(SDL_Window * window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            // See http://syprog.blogspot.ru/2011/12/listing-loaded-shared-objects-in-linux.html
            struct lmap
            {
                void * base_address;
                char * path;
                void * unused;
                lmap * next;
                lmap * prev;
            };

            struct dummy
            {
                void *  pointers[3];
                dummy * ptr;
            };

            bool result = false;
            void * processHandle = dlopen(nullptr, RTLD_NOW);
            if (processHandle != nullptr)
            {
                dummy * p = ((dummy *)processHandle)->ptr;
                lmap * pl = (lmap *)p->ptr;
                while (pl != nullptr)
                {
                    if (strstr(pl->path, "gameoverlayrenderer.so") != nullptr)
                    {
                        result = true;
                        break;
                    }
                    pl = pl->next;
                }
                dlclose(processHandle);
            }
            return result;
        }

        void ShowMessageBox(SDL_Window * window, const std::string &message) override
        {
            log_verbose(message.c_str());

            std::string executablePath;
            DIALOG_TYPE dtype = GetDialogApp(&executablePath);

            switch (dtype) {
            case DIALOG_TYPE::KDIALOG:
            {
                std::string cmd = String::Format("%s --title \"OpenRCT2\" --msgbox \"%s\"", executablePath.c_str(), message.c_str());
                Execute(cmd);
                break;
            }
            case DIALOG_TYPE::ZENITY:
            {
                std::string cmd = String::Format("%s --title=\"OpenRCT2\" --info --text=\"%s\"", executablePath.c_str(), message.c_str());
                Execute(cmd);
                break;
            }
            default:
                SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", message.c_str(), window);
                break;
            }
        }

        std::string ShowFileDialog(SDL_Window * window, const FileDialogDesc &desc) override
        {
            std::string result;
            std::string executablePath;
            DIALOG_TYPE dtype = GetDialogApp(&executablePath);
            switch (dtype) {
            case DIALOG_TYPE::KDIALOG:
            {
                std::string action =
                    (desc.Type == FILE_DIALOG_TYPE::OPEN) ? "--getopenfilename" :
                                                            "--getsavefilename";
                std::string filter = GetKDialogFilterString(desc.Filters);
                std::string cmd = String::StdFormat("%s --title '%s' %s '%s/' ~ '%s'", executablePath, desc.Title, action, desc.InitialDirectory, filter);
                std::string output;
                if (Execute(cmd, &output) != 0)
                {
                    result = output;
                }
                break;
            }
            case DIALOG_TYPE::ZENITY:
            {
                std::string action = "--file-selection";
                std::string flags;
                if (desc.Type == FILE_DIALOG_TYPE::SAVE)
                {
                    flags = "--confirm-overwrite --save";
                }
                std::string filters = GetZenityFilterString(desc.Filters);
                std::string cmd = String::StdFormat("%s %s --filename='%s/' %s --title='%s' / %s", executablePath, action, desc.InitialDirectory, flags, desc.Title, filters);
                std::string output;
                if (Execute(cmd, &output) != 0)
                {
                    result = output;
                }
                break;
            }
            default:
                log_error("KDialog or Zenity not installed.");
                break;
            }

            log_verbose("filename = %s", result.c_str());

            if (desc.Type == FILE_DIALOG_TYPE::OPEN && access(result.c_str(), F_OK) == -1)
            {
                std::string msg = String::StdFormat("\"%s\" not found: %s, please choose another file\n", result.c_str(), strerror(errno));
                ShowMessageBox(window, msg);
                return ShowFileDialog(window, desc);
            }
            else if (desc.Type == FILE_DIALOG_TYPE::SAVE && access(result.c_str(), F_OK) != -1 && dtype == DIALOG_TYPE::KDIALOG)
            {
                std::string cmd = String::StdFormat("%s --yesno \"Overwrite %s?\"", executablePath, result.c_str());
                if (Execute(cmd) != 0)
                {
                    result = std::string();
                }
            }
            return result;
        }

        std::string ShowDirectoryDialog(SDL_Window * window, const std::string &title) override
        {
            std::string result;
            std::string executablePath;
            DIALOG_TYPE dtype = GetDialogApp(&executablePath);
            switch (dtype) {
            case DIALOG_TYPE::KDIALOG:
            {
                std::string output;
                std::string cmd = String::Format("%s --title '%s' --getexistingdirectory /", executablePath.c_str(), title);
                if (Execute(cmd, &output) == 0)
                {
                    result = output;
                }
                break;
            }
            case DIALOG_TYPE::ZENITY:
            {
                std::string output;
                std::string cmd = String::Format("%s --title='%s' --file-selection --directory /", executablePath.c_str(), title);
                if (Execute(cmd, &output) == 0)
                {
                    result = output;
                }
                break;
            }
            default:
                log_error("KDialog or Zenity not installed.");
                break;
            }
            return result;
        }

    private:
        static DIALOG_TYPE GetDialogApp(std::string * executablePath)
        {
            // Prefer zenity as it offers more required features, e.g., overwrite
            // confirmation and selecting only existing files
            if (Execute("which zenity", executablePath) == 0)
            {
                return DIALOG_TYPE::ZENITY;
            }
            if (Execute("which kdialog", executablePath))
            {
                return DIALOG_TYPE::KDIALOG;
            }
            return DIALOG_TYPE::NONE;
        }

        static sint32 Execute(const std::string &command, std::string * output = nullptr)
        {
            log_verbose("executing \"%s\"...\n", command.c_str());
            FILE * fpipe = popen(command.c_str(), "r");
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
                for (size_t i = outputBuffer.size() - 1; i >= 0; i--)
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
        }

        static std::string GetKDialogFilterString(const std::vector<FileDialogDesc::Filter> filters)
        {
            std::stringstream filtersb;
            bool first = true;
            for (const auto &filter : filters)
            {
                // KDialog wants filters space-delimited and we don't expect ';' anywhere else
                std::string pattern = filter.Pattern;
                for (size_t i = 0; i < pattern.size(); i++)
                {
                    if (pattern[i] == ';')
                    {
                        pattern[i] = ' ';
                    }
                }

                if (first)
                {
                    filtersb << String::StdFormat("%s | %s", pattern.c_str(), filter.Name.c_str());
                    first = false;
                }
                else
                {
                    filtersb << String::StdFormat("\\n%s | %s", pattern.c_str(), filter.Name.c_str());
                }
            }
            return filtersb.str();
        }

        static std::string GetZenityFilterString(const std::vector<FileDialogDesc::Filter> filters)
        {
            // Zenity seems to be case sensitive, while KDialog isn't
            std::stringstream filtersb;
            for (const auto &filter : filters)
            {
                filtersb << " --file-filter='" << filter.Name << " | ";
                for (char c : filter.Pattern)
                {
                    if (c == ';')
                    {
                        filtersb << ' ';
                    }
                    else if (isalpha(c))
                    {
                        filtersb << '['
                                 << toupper(c)
                                 << tolower(c)
                                 << ']';
                    }
                    else
                    {
                        filtersb << c;
                    }
                }
                filtersb << "'";
            }
            return filtersb.str();
        }
    };

    IPlatformUiContext * CreatePlatformUiContext()
    {
        return new LinuxContext();
    }
} }

#endif // __linux__
