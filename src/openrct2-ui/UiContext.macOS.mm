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

#if defined(__APPLE__) && defined(__MACH__)

#import <Cocoa/Cocoa.h>

#include <mach-o/dyld.h>
#include <dlfcn.h>
#include <openrct2/common.h>
#include <openrct2/core/String.hpp>
#include <openrct2/ui/UiContext.h>
#include "UiContext.h"

#include <SDL.h>

namespace OpenRCT2 { namespace Ui
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
				if ([panel runModal] == NSFileHandlingPanelCancelButton){
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
				utf8 *url = NULL;
				if ([panel runModal] == NSFileHandlingPanelOKButton)
				{
					NSString *selectedPath = panel.URL.path;
					const char *path = selectedPath.UTF8String;
					url = _strdup(path);
				}
				return url;
			}
        }

    private:
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
} }

#endif // __APPLE__ && __MACH__
