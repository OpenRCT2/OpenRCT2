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

#include <openrct2/common.h>
#include <SDL.h>
#include <openrct2/core/String.hpp>
#include "UiContext.h"

namespace OpenRCT2 { namespace Ui
{
    enum class DIALOG_TYPE
    {
        NONE,
        KDIALOG,
        ZENITY,
    };

    class LinuxContext : public IPlatformUiContext
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
    };

    IPlatformUiContext * CreatePlatformUiContext()
    {
        return new LinuxContext();
    }
} }

#endif // __linux__
