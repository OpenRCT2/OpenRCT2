/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#import <Cocoa/Cocoa.h>

#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <openrct2/common.h>
#include <openrct2/core/String.hpp>
#include <openrct2/ui/UiContext.h>
#include "UiContext.h"

#include <SDL.h>

namespace OpenRCT2::Ui
{
    class macOSContext final : public IPlatformUiContext
    {
    private:

    public:
        macOSContext()
        {
            @autoreleasepool {
                if ([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)]) {
                    [NSWindow setAllowsAutomaticWindowTabbing:NO];
                }
            }
        }

        void SetWindowIcon(SDL_Window * window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            STUB();
            return false;
        }

        void ShowMessageBox(SDL_Window * window, const std::string &message) override
        {
            @autoreleasepool
            {
                NSAlert *alert = [[[NSAlert alloc] init] autorelease];
                [alert addButtonWithTitle:@"OK"];
                alert.messageText = [NSString stringWithUTF8String:message.c_str()];
                [alert runModal];
            }
        }

        std::string ShowFileDialog(SDL_Window * window, const FileDialogDesc &desc) override
        {
            @autoreleasepool
            {
                NSMutableArray *extensions = [NSMutableArray new];
                for (const OpenRCT2::Ui::FileDialogDesc::Filter &filter: desc.Filters) {
                    if (filter.Pattern != "") {
                        NSString *fp = [NSString stringWithUTF8String:filter.Pattern.c_str()];
                        fp = [fp stringByReplacingOccurrencesOfString:@"*." withString:@""];
                        [extensions addObjectsFromArray:[fp componentsSeparatedByString:@";"]];
                    }
                }

                NSString *directory;
                NSSavePanel *panel;
                if (desc.Type == FILE_DIALOG_TYPE::SAVE)
                {
                    NSString *filePath = [NSString stringWithUTF8String:desc.DefaultFilename.c_str()];
                    directory = filePath.stringByDeletingLastPathComponent;
                    NSString *basename = filePath.lastPathComponent;
                    panel = [NSSavePanel savePanel];
                    panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@", basename, extensions.firstObject];
                }
                else if (desc.Type == FILE_DIALOG_TYPE::OPEN)
                {
                    directory = [NSString stringWithUTF8String:desc.InitialDirectory.c_str()];
                    NSOpenPanel *open = [NSOpenPanel openPanel];
                    open.canChooseDirectories = false;
                    open.canChooseFiles = true;
                    open.allowsMultipleSelection = false;
                    panel = open;
                } else {
                    return std::string();
                }

                panel.title = [NSString stringWithUTF8String:desc.Title.c_str()];
                panel.allowedFileTypes = extensions;
                panel.directoryURL = [NSURL fileURLWithPath:directory];
                if ([panel runModal] == NSModalResponseCancel)
                {
                    return std::string();
                } else {
                    return panel.URL.path.UTF8String;
                }
            }
        }

        std::string ShowDirectoryDialog(SDL_Window * window, const std::string &title) override
        {
            @autoreleasepool
            {
                NSOpenPanel *panel = [NSOpenPanel openPanel];
                panel.canChooseFiles = false;
                panel.canChooseDirectories = true;
                panel.allowsMultipleSelection = false;
                if ([panel runModal] == NSModalResponseOK)
                {
                    NSString *selectedPath = panel.URL.path;
                    const char *path = selectedPath.UTF8String;
                    return path;
                } else {
                    return "";
                }
            }
        }

    private:
        static int32_t Execute(const std::string &command, std::string * output = nullptr)
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
        }
    };

    IPlatformUiContext * CreatePlatformUiContext()
    {
        return new macOSContext();
    }
} // namespace OpenRCT2::Ui

#endif // __APPLE__ && __MACH__
