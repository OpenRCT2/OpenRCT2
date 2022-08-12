/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

#    include "UiContext.h"

#    include <openrct2/common.h>
#    include <openrct2/core/String.hpp>
#    include <openrct2/ui/UiContext.h>

// undefine `interface` and `abstract`, because it's causing conflicts with Objective-C's keywords
#    undef interface
#    undef abstract

#    include <ApplicationServices/ApplicationServices.h>
#    import <Cocoa/Cocoa.h>
#    include <CoreFoundation/CFBundle.h>
#    include <SDL.h>
#    include <mach-o/dyld.h>
#    include <string>

namespace OpenRCT2::Ui
{
    class macOSContext final : public IPlatformUiContext
    {
    private:
    public:
        macOSContext()
        {
            @autoreleasepool
            {
                if ([NSWindow respondsToSelector:@selector(setAllowsAutomaticWindowTabbing:)])
                {
                    [NSWindow setAllowsAutomaticWindowTabbing:NO];
                }
            }
        }

        void SetWindowIcon(SDL_Window* window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
            STUB();
            return false;
        }

        void ShowMessageBox(SDL_Window* window, const std::string& message) override
        {
            @autoreleasepool
            {
                NSAlert* alert = [[[NSAlert alloc] init] autorelease];
                [alert addButtonWithTitle:@"OK"];
                alert.messageText = [NSString stringWithUTF8String:message.c_str()];
                [alert runModal];
            }
        }

        bool HasMenuSupport() override
        {
            return false;
        }

        int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text) override
        {
            return -1;
        }

        void OpenFolder(const std::string& path) override
        {
            @autoreleasepool
            {
                NSString* nsPath = [NSString stringWithUTF8String:path.c_str()];
                NSURL* folderURL = [NSURL fileURLWithPath:nsPath];
                [[NSWorkspace sharedWorkspace] openURL:folderURL];
            }
        }

        void OpenURL(const std::string& url) override
        {
            CFURLRef urlRef = CFURLCreateWithBytes(
                nullptr, reinterpret_cast<const UInt8*>(url.c_str()), url.length(), kCFStringEncodingUTF8, nullptr);
            LSOpenCFURLRef(urlRef, 0);
            CFRelease(urlRef);
        }

        std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) override
        {
            @autoreleasepool
            {
                NSMutableArray* extensions = [NSMutableArray new];
                for (const OpenRCT2::Ui::FileDialogDesc::Filter& filter : desc.Filters)
                {
                    if (filter.Pattern != "")
                    {
                        NSString* fp = [NSString stringWithUTF8String:filter.Pattern.c_str()];
                        fp = [fp stringByReplacingOccurrencesOfString:@"*." withString:@""];
                        [extensions addObjectsFromArray:[fp componentsSeparatedByString:@";"]];
                    }
                }

                NSString* directory;
                NSSavePanel* panel;
                if (desc.Type == FileDialogType::Save)
                {
                    NSString* filePath = [NSString stringWithUTF8String:desc.DefaultFilename.c_str()];
                    directory = filePath.stringByDeletingLastPathComponent;
                    NSString* basename = filePath.lastPathComponent;
                    panel = [NSSavePanel savePanel];
                    panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@", basename, extensions.firstObject];
                }
                else if (desc.Type == FileDialogType::Open)
                {
                    directory = [NSString stringWithUTF8String:desc.InitialDirectory.c_str()];
                    NSOpenPanel* open = [NSOpenPanel openPanel];
                    open.canChooseDirectories = false;
                    open.canChooseFiles = true;
                    open.allowsMultipleSelection = false;
                    panel = open;
                }
                else
                {
                    return std::string();
                }

                panel.title = [NSString stringWithUTF8String:desc.Title.c_str()];
                panel.allowedFileTypes = extensions;
                panel.directoryURL = [NSURL fileURLWithPath:directory];
                if ([panel runModal] == NSModalResponseCancel)
                {
                    return std::string();
                }
                else
                {
                    return panel.URL.path.UTF8String;
                }
            }
        }

        std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) override
        {
            @autoreleasepool
            {
                NSOpenPanel* panel = [NSOpenPanel openPanel];
                panel.canChooseFiles = false;
                panel.canChooseDirectories = true;
                panel.allowsMultipleSelection = false;
                if ([panel runModal] == NSModalResponseOK)
                {
                    NSString* selectedPath = panel.URL.path;
                    const char* path = selectedPath.UTF8String;
                    return path;
                }
                else
                {
                    return "";
                }
            }
        }

        bool HasFilePicker() const override
        {
            return true;
        }

    private:
        static int32_t Execute(const std::string& command, std::string* output = nullptr)
        {
            log_verbose("executing \"%s\"...", command.c_str());
            FILE* fpipe = popen(command.c_str(), "r");
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

    std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext()
    {
        return std::make_unique<macOSContext>();
    }
} // namespace OpenRCT2::Ui

#endif // __APPLE__ && __MACH__
