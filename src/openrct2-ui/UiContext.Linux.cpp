/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if (defined(__unix__) || defined(__HAIKU__) || defined(__EMSCRIPTEN__)) && !defined(__ANDROID__) && !defined(__APPLE__)

    #include "UiContext.h"

    #include "UiStringIds.h"

    #include <SDL.h>
    #include <algorithm>
    #include <dlfcn.h>
    #include <openrct2/Diagnostic.h>
    #include <openrct2/core/Path.hpp>
    #include <openrct2/core/String.hpp>
    #include <openrct2/core/StringBuilder.h>
    #include <openrct2/core/StringTypes.h>
    #include <openrct2/localisation/Language.h>
    #include <openrct2/platform/Platform.h>
    #include <openrct2/ui/UiContext.h>
    #include <sstream>
    #include <stdexcept>
    #include <unistd.h>

    #ifdef __EMSCRIPTEN__
        #include <emscripten.h>
    #endif

namespace OpenRCT2::Ui
{
    enum class DialogType
    {
        none,
        kdialog,
        zenity,
    };

    class LinuxContext final : public IPlatformUiContext
    {
    private:
        mutable std::optional<bool> _hasFilePicker = std::nullopt;

    public:
        LinuxContext()
        {
        }

        void SetWindowIcon(SDL_Window* window) override
        {
        }

        bool IsSteamOverlayAttached() override
        {
    #ifdef __linux__
            // See http://syprog.blogspot.ru/2011/12/listing-loaded-shared-objects-in-linux.html
            struct lmap
            {
                void* base_address;
                char* path;
                void* unused;
                lmap* next;
                lmap* prev;
            };

            struct dummy
            {
                void* pointers[3];
                dummy* ptr;
            };

            bool result = false;
            void* processHandle = dlopen(nullptr, RTLD_NOW);
            if (processHandle != nullptr)
            {
                dummy* p = (static_cast<dummy*>(processHandle))->ptr;
                lmap* pl = reinterpret_cast<lmap*>(p->ptr);
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
    #else
            return false; // Needed for OpenBSD, likely all other Unixes.
    #endif
        }

        void ShowMessageBox(SDL_Window* window, const std::string& message) override
        {
            LOG_VERBOSE(message.c_str());

            std::string executablePath;
            DialogType dtype = GetDialogApp(&executablePath);
            switch (dtype)
            {
                case DialogType::kdialog:
                {
                    const char* args[] = {
                        executablePath.c_str(), "--title", "OpenRCT2", "--msgbox", message.c_str(), nullptr
                    };
                    Platform::Execute(args);
                    break;
                }
                case DialogType::zenity:
                {
                    const char* args[] = {
                        executablePath.c_str(), "--title=OpenRCT2", "--info", "--text", message.c_str(), nullptr
                    };
                    Platform::Execute(args);
                    break;
                }
                default:
                    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "OpenRCT2", message.c_str(), window);
                    break;
            }
        }

        void OpenFolder(const std::string& path) override
        {
    #ifdef __HAIKU__
            const char* args[] = { "open", path.c_str(), nullptr };
    #else
            const char* args[] = { "xdg-open", path.c_str(), nullptr };
    #endif
            Platform::Execute(args);
        }

        void OpenURL(const std::string& url) override
        {
    #ifndef __EMSCRIPTEN__
        #ifdef __HAIKU__
            const char* args[] = { "open", url.c_str(), nullptr };
        #else
            const char* args[] = { "xdg-open", url.c_str(), nullptr };
        #endif
            Platform::Execute(args);
    #else
            MAIN_THREAD_EM_ASM({ window.open(UTF8ToString($0)); }, url.c_str());
    #endif
        }

        std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) override
        {
            std::string result;
            std::string executablePath;
            u8string directory = desc.InitialDirectory + '/';
            DialogType dtype = GetDialogApp(&executablePath);
            switch (dtype)
            {
                case DialogType::kdialog:
                {
                    std::string action = (desc.Type == FileDialogType::Open) ? "--getopenfilename" : "--getsavefilename";
                    std::vector<std::string> filters = GetKDialogFilterString(desc.Filters);
                    std::vector<std::string> argsVec = { executablePath, "--title", desc.Title, action, directory };
                    for (const auto& filter : filters)
                    {
                        argsVec.push_back(filter);
                    }
                    std::vector<const char*> args;
                    for (const auto& arg : argsVec)
                    {
                        args.push_back(arg.c_str());
                    }
                    args.push_back(nullptr);

                    std::string output;
                    if (Platform::Execute(args.data(), &output) == 0)
                    {
                        result = output;
                    }
                    break;
                }
                case DialogType::zenity:
                {
                    std::string action = "--file-selection";
                    std::vector<std::string> filters = GetZenityFilterString(desc.Filters);
                    u8string directoryarg = String::stdFormat("--filename=%s", directory.c_str());
                    std::vector<std::string> argsVec = { executablePath, action, directoryarg };
                    if (desc.Type == FileDialogType::Save)
                    {
                        argsVec.push_back("--confirm-overwrite");
                        argsVec.push_back("--save");
                    }
                    argsVec.push_back("--title");
                    argsVec.push_back(desc.Title);
                    argsVec.push_back("/");
                    for (const auto& filter : filters)
                    {
                        argsVec.push_back(filter);
                    }
                    std::vector<const char*> args;
                    for (const auto& arg : argsVec)
                    {
                        args.push_back(arg.c_str());
                    }
                    args.push_back(nullptr);

                    std::string output;
                    if (Platform::Execute(args.data(), &output) == 0)
                    {
                        if (desc.Type == FileDialogType::Save)
                        {
                            // The default file extension is taken from the **first** available filter, since
                            // we cannot obtain it from zenity's output. This means that the FileDialogDesc::Filters
                            // array must be carefully populated, at least the first element.
                            std::string pattern = desc.Filters[0].Pattern;
                            std::string defaultExtension = pattern.substr(pattern.find_last_of('.'));

                            const auto filename = Path::GetFileName(output.c_str());

                            // If there is no extension, append the pattern
                            const auto extension = Path::GetExtension(filename);
                            result = output;
                            if (extension[0] == '\0' && !defaultExtension.empty())
                            {
                                result = output.append(defaultExtension);
                            }
                        }
                        else
                        {
                            result = output;
                        }
                    }
                    break;
                }
                default:
                    ThrowMissingDialogApp();
                    break;
            }

            if (!result.empty())
            {
                if (desc.Type == FileDialogType::Open && access(result.c_str(), F_OK) == -1)
                {
                    std::string msg = String::stdFormat(
                        "\"%s\" not found: %s, please choose another file\n", result.c_str(), strerror(errno));
                    ShowMessageBox(window, msg);
                    return ShowFileDialog(window, desc);
                }
                if (desc.Type == FileDialogType::Save && access(result.c_str(), F_OK) != -1 && dtype == DialogType::kdialog)
                {
                    u8string overwrite = String::stdFormat("Overwrite %s?", result.c_str());
                    const char* args[] = { executablePath.c_str(), "--yesno", overwrite.c_str(), nullptr };
                    if (Platform::Execute(args) != 0)
                    {
                        result = std::string();
                    }
                }
            }
            return result;
        }

        std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) override
        {
            std::string result;
            std::string executablePath;
            DialogType dtype = GetDialogApp(&executablePath);
            switch (dtype)
            {
                case DialogType::kdialog:
                {
                    std::string output;
                    std::string cmd = String::stdFormat(
                        "%s --title '%s' --getexistingdirectory /", executablePath.c_str(), title.c_str());
                    const char* args[] = { executablePath.c_str(),   "--title", title.c_str(),
                                           "--getexistingdirectory", "/",       nullptr };
                    if (Platform::Execute(args, &output) == 0)
                    {
                        result = output;
                    }
                    break;
                }
                case DialogType::zenity:
                {
                    std::string output;
                    std::string cmd = String::stdFormat(
                        "%s --title='%s' --file-selection --directory /", executablePath.c_str(), title.c_str());
                    const char* args[] = { executablePath.c_str(), "--title", title.c_str(), "--file-selection",
                                           "--directory",          "/",       nullptr };
                    if (Platform::Execute(args, &output) == 0)
                    {
                        result = output;
                    }
                    break;
                }
                default:
                    ThrowMissingDialogApp();
                    break;
            }
            return result;
        }

        bool HasFilePicker() const override
        {
            if (!_hasFilePicker.has_value())
            {
                std::string dummy;
                _hasFilePicker = (GetDialogApp(&dummy) != DialogType::none);
            }

            return _hasFilePicker.value();
        }

        bool HasMenuSupport() override
        {
            std::string executablePath;
            DialogType dtype = GetDialogApp(&executablePath);
            return dtype != DialogType::none;
        }

        int32_t ShowMenuDialog(
            const std::vector<std::string>& options, const std::string& title, const std::string& text) override
        {
            std::string executablePath;
            DialogType dtype = GetDialogApp(&executablePath);

            size_t longest_string = 0;
            for (const auto& option : options)
            {
                if (option.size() > longest_string)
                {
                    longest_string = option.size();
                }
            }

            // zenity and kdialog don't support automatic scaling, this is an approximation
            int width = (longest_string + 1) * 8;
            int height = 350;

            switch (dtype)
            {
                case DialogType::zenity:
                {
                    std::vector<std::string> argsVec = { executablePath,
                                                         "--list",
                                                         "--column",
                                                         "",
                                                         String::stdFormat("--width=%d", width),
                                                         String::stdFormat("--height=%d", height) };
                    for (const auto& option : options)
                    {
                        argsVec.push_back(option);
                    }
                    argsVec.push_back("--title");
                    argsVec.push_back(title);
                    argsVec.push_back("--text");
                    argsVec.push_back(text);
                    std::vector<const char*> args;
                    for (const auto& arg : argsVec)
                    {
                        args.push_back(arg.c_str());
                    }
                    args.push_back(nullptr);
                    std::string buff;
                    Platform::Execute(args.data(), &buff);
                    return std::find(options.begin(), options.end(), buff) - options.begin();
                }
                case DialogType::kdialog:
                {
                    std::vector<std::string> argsVec = {
                        executablePath, "--geometry", String::stdFormat("%dx%d", width, height), "--title", title,
                        "--menu",       text
                    };
                    for (const auto& option : options)
                    {
                        argsVec.push_back(option);
                        argsVec.push_back(option);
                    }
                    std::vector<const char*> args;
                    for (const auto& arg : argsVec)
                    {
                        args.push_back(arg.c_str());
                    }
                    args.push_back(nullptr);
                    std::string buff;
                    Platform::Execute(args.data(), &buff);
                    return std::find(options.begin(), options.end(), buff) - options.begin();
                }
                default:
                {
                    break;
                }
            }

            return -1;
        }

    private:
        static DialogType GetDialogApp(std::string* executablePath)
        {
            // Prefer zenity as it offers more required features, e.g., overwrite
            // confirmation and selecting only existing files.
            // OpenRCT2 will fall back to an SDL pop-up if the user has neither.
            if (Platform::FindApp("zenity", executablePath))
            {
                return DialogType::zenity;
            }
            if (Platform::FindApp("kdialog", executablePath))
            {
                return DialogType::kdialog;
            }
            return DialogType::none;
        }

        static std::vector<std::string> GetKDialogFilterString(const std::vector<FileDialogDesc::Filter>& filters)
        {
            std::vector<std::string> filterStrings;
            for (const auto& filter : filters)
            {
                std::stringstream filtersb;
                filtersb << filter.Name.c_str() << " (";
                AddFilterCaseInsensitive(filtersb, filter.Pattern);
                filtersb << ")";
                filterStrings.push_back(filtersb.str());
            }
            return filterStrings;
        }

        static std::vector<std::string> GetZenityFilterString(const std::vector<FileDialogDesc::Filter>& filters)
        {
            std::vector<std::string> filterStrings;
            for (const auto& filter : filters)
            {
                std::stringstream filtersb;
                filtersb << "--file-filter=" << filter.Name << " | ";
                AddFilterCaseInsensitive(filtersb, filter.Pattern);
                filterStrings.push_back(filtersb.str());
            }
            return filterStrings;
        }

        static void AddFilterCaseInsensitive(std::stringstream& stream, u8string pattern)
        {
            for (char c : pattern)
            {
                if (c == ';')
                {
                    stream << ' ';
                }
                else if (isalpha(static_cast<unsigned char>(c)))
                {
                    auto uc = static_cast<unsigned char>(c);
                    stream << '[' << static_cast<char>(tolower(uc)) << static_cast<char>(toupper(uc)) << ']';
                }
                else
                {
                    stream << c;
                }
            }
        }

        static void ThrowMissingDialogApp()
        {
            auto& uiContext = GetContext()->GetUiContext();
            std::string dialogMissingWarning = LanguageGetString(STR_MISSING_DIALOG_APPLICATION_ERROR);
            uiContext.ShowMessageBox(dialogMissingWarning);
            throw std::runtime_error(dialogMissingWarning);
        }
    };

    std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext()
    {
        return std::make_unique<LinuxContext>();
    }
} // namespace OpenRCT2::Ui

#endif // (defined(__unix__) || defined(__EMSCRIPTEN__)) && !defined(__ANDROID__) && !defined(__APPLE__)
