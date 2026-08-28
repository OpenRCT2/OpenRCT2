/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if defined(__APPLE__) && defined(__MACH__)

    #include "UiContext.h"

    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-enum-enum-conversion"
    #pragma clang diagnostic ignored "-Wdeprecated-anon-enum-enum-conversion"
    #pragma clang diagnostic ignored "-Wunguarded-availability-new"
    #pragma clang diagnostic ignored "-Wold-style-cast"
    #pragma clang diagnostic ignored "-Wundef"
    #pragma clang diagnostic ignored "-Wmissing-method-return-type"
    #pragma clang diagnostic ignored "-Wavailability"
    #include <ApplicationServices/ApplicationServices.h>
    #include <Cocoa/Cocoa.h>
    #include <CoreFoundation/CFBundle.h>
    #include <SDL.h>
    #ifndef SDL_VIDEO_DRIVER_COCOA
        #define SDL_VIDEO_DRIVER_COCOA 1
    #endif
    #include <SDL_syswm.h>
    #include <mach-o/dyld.h>
    #pragma clang diagnostic pop
    #include <openrct2/Diagnostic.h>
    #include "MacNativeInput.h"
    #include <openrct2/config/Config.h>
    #include <openrct2/ui/UiContext.h>
    #include <atomic>
    #include <string>

namespace OpenRCT2::Ui
{
    static std::atomic<float> gNativeScrollX{ 0 };
    static std::atomic<float> gNativeScrollY{ 0 };
    static std::atomic<int> gNativePinch{ 0 };

    void PollNativeMacOSScroll(float& x, float& y)
    {
        x = gNativeScrollX.exchange(0);
        y = gNativeScrollY.exchange(0);
    }

    int PollNativeMacOSPinch()
    {
        return gNativePinch.exchange(0);
    }

    class macOSContext final : public IPlatformUiContext
    {
    private:
        id _nativeGestureMonitor = nil;
        NSWindow* _window = nil;
        bool _nativePinchHandled = false;

        void InstallNativeGestureMonitor()
        {
            if (_nativeGestureMonitor != nil)
                return;

            _nativeGestureMonitor = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskScrollWheel | NSEventMaskMagnify
                handler:^NSEvent*(NSEvent* event) {
                    if (!Config::Get().general.nativeMacOSControls || event.window != _window)
                        return event;
                    if (event.type == NSEventTypeMagnify)
                    {
                        if (event.phase == NSEventPhaseBegan)
                            _nativePinchHandled = false;

                        // Map the observed trackpad directions directly: pinch in zooms
                        // out and pinch out zooms in. The first non-zero event completes
                        // the gesture, so there is no distance threshold or slow buildup.
                        if (!_nativePinchHandled && event.magnification > 0)
                        {
                            gNativePinch.fetch_add(1);
                            _nativePinchHandled = true;
                        }
                        else if (!_nativePinchHandled && event.magnification < 0)
                        {
                            gNativePinch.fetch_sub(1);
                            _nativePinchHandled = true;
                        }

                        if (event.phase == NSEventPhaseEnded || event.phase == NSEventPhaseCancelled)
                            _nativePinchHandled = false;
                    }
                    else
                    {
                        // Preserve both live and momentum scroll events. AppKit's scrollingDelta is already subpixel.
                        gNativeScrollX.fetch_add(event.scrollingDeltaX);
                        gNativeScrollY.fetch_add(event.scrollingDeltaY);
                    }
                    return nil;
                }];
        }

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

        ~macOSContext() override
        {
            @autoreleasepool
            {
                if (_nativeGestureMonitor != nil)
                {
                    [NSEvent removeMonitor:_nativeGestureMonitor];
                    _nativeGestureMonitor = nil;
                }
            }
        }

        void SetWindowIcon(SDL_Window* window) override
        {
            SDL_SysWMinfo wmInfo;
            SDL_VERSION(&wmInfo.version);
            if (SDL_GetWindowWMInfo(window, &wmInfo) == SDL_TRUE && wmInfo.subsystem == SDL_SYSWM_COCOA)
            {
                _window = wmInfo.info.cocoa.window;
                InstallNativeGestureMonitor();
            }
        }

        bool IsSteamOverlayAttached() override
        {
            LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
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
            return true;
        }

        int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text) override
        {
            @autoreleasepool
            {
                NSAlert* alert = [[[NSAlert alloc] init] autorelease];
                for (const std::string& option : options)
                {
                    [alert addButtonWithTitle:[NSString stringWithUTF8String:option.c_str()]];
                }

                alert.messageText = [NSString stringWithUTF8String:title.c_str()];
                alert.informativeText = [NSString stringWithUTF8String:text.c_str()];
                NSModalResponse response = [alert runModal];
                if (response >= 1000)
                {
                    return static_cast<int32_t>(response - 1000);
                }
                return -1;
            }
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
                if (desc.Type == FileDialogType::save)
                {
                    NSString* filePath = [NSString stringWithUTF8String:desc.DefaultFilename.c_str()];
                    directory = filePath.stringByDeletingLastPathComponent;
                    NSString* basename = filePath.lastPathComponent;
                    panel = [NSSavePanel savePanel];
                    panel.nameFieldStringValue = [NSString stringWithFormat:@"%@.%@", basename, extensions.firstObject];
                }
                else if (desc.Type == FileDialogType::open)
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
                    SDL_RaiseWindow(window);
                    return std::string();
                }

                panel.title = [NSString stringWithUTF8String:desc.Title.c_str()];
                panel.allowedFileTypes = extensions;
                panel.directoryURL = [NSURL fileURLWithPath:directory];
                if ([panel runModal] == NSModalResponseCancel)
                {
                    SDL_RaiseWindow(window);
                    return std::string();
                }
                else
                {
                    SDL_RaiseWindow(window);
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
                    SDL_RaiseWindow(window);
                    return path;
                }
                else
                {
                    SDL_RaiseWindow(window);
                    return {};
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
            LOG_VERBOSE("executing \"%s\"...", command.c_str());
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
