/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef _WIN32

// Windows.h needs to be included first
// clang-format off
#    include <windows.h>
#    include <shellapi.h>
#    include <commdlg.h>
// clang-format on
#    undef CreateWindow

// Then the rest
#    include "UiContext.h"

#    include <SDL.h>
#    include <SDL_syswm.h>
#    include <algorithm>
#    include <openrct2/common.h>
#    include <openrct2/core/Path.hpp>
#    include <openrct2/core/String.hpp>
#    include <openrct2/ui/UiContext.h>
#    include <shlobj.h>
#    include <sstream>

// Native resource IDs
#    include "../../resources/resource.h"

static std::wstring SHGetPathFromIDListLongPath(LPCITEMIDLIST pidl)
{
    // Limit path length to 32K
    std::wstring pszPath(std::numeric_limits<int16_t>().max(), 0);
    auto result = SHGetPathFromIDListEx(pidl, pszPath.data(), static_cast<DWORD>(pszPath.size()), GPFIDL_DEFAULT);
    if (result)
    {
        // Truncate at first null terminator
        auto length = pszPath.find(L'\0');
        if (length != std::wstring::npos)
        {
            pszPath.resize(length);
            pszPath.shrink_to_fit();
        }
        return pszPath;
    }
    return std::wstring();
}

namespace OpenRCT2::Ui
{
    class Win32Context : public IPlatformUiContext
    {
    private:
        HMODULE _win32module;

    public:
        Win32Context()
        {
            _win32module = GetModuleHandleA(nullptr);
        }

        void SetWindowIcon(SDL_Window* window) override
        {
            if (_win32module != nullptr)
            {
                HICON icon = LoadIconA(_win32module, MAKEINTRESOURCEA(IDI_ICON));
                if (icon != nullptr)
                {
                    HWND hwnd = GetHWND(window);
                    if (hwnd != nullptr)
                    {
                        SendMessageA(hwnd, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
                    }
                }
            }
        }

        bool IsSteamOverlayAttached() override
        {
            return (GetModuleHandleA("GameOverlayRenderer.dll") != nullptr);
        }

        void ShowMessageBox(SDL_Window* window, const std::string& message) override
        {
            HWND hwnd = GetHWND(window);
            std::wstring messageW = String::ToWideChar(message);
            MessageBoxW(hwnd, messageW.c_str(), L"OpenRCT2", MB_OK);
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
            std::wstring pathW = String::ToWideChar(path);
            ShellExecuteW(NULL, L"open", pathW.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }

        void OpenURL(const std::string& url) override
        {
            std::wstring urlW = String::ToWideChar(url);
            ShellExecuteW(NULL, L"open", urlW.c_str(), NULL, NULL, SW_SHOWNORMAL);
        }

        std::string ShowFileDialog(SDL_Window* window, const FileDialogDesc& desc) override
        {
            std::wstring wcFilename = String::ToWideChar(desc.DefaultFilename);
            wcFilename.resize(std::max<size_t>(wcFilename.size(), MAX_PATH));

            std::wstring wcTitle = String::ToWideChar(desc.Title);
            std::wstring wcInitialDirectory = String::ToWideChar(desc.InitialDirectory);
            std::wstring wcFilters = GetFilterString(desc.Filters);

            // Set open file name options
            OPENFILENAMEW openFileName = {};
            openFileName.lStructSize = sizeof(OPENFILENAMEW);
            openFileName.lpstrTitle = wcTitle.c_str();
            openFileName.lpstrInitialDir = wcInitialDirectory.c_str();
            openFileName.lpstrFilter = wcFilters.c_str();
            openFileName.lpstrFile = &wcFilename[0];
            openFileName.nMaxFile = static_cast<DWORD>(wcFilename.size());

            // Open dialog
            BOOL dialogResult = FALSE;
            DWORD commonFlags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
            if (desc.Type == FileDialogType::Open)
            {
                openFileName.Flags = commonFlags | OFN_NONETWORKBUTTON | OFN_FILEMUSTEXIST;
                dialogResult = GetOpenFileNameW(&openFileName);
            }
            else if (desc.Type == FileDialogType::Save)
            {
                openFileName.Flags = commonFlags | OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
                dialogResult = GetSaveFileNameW(&openFileName);
            }

            std::string resultFilename;
            if (dialogResult)
            {
                resultFilename = String::ToUtf8(openFileName.lpstrFile);

                // If there is no extension, append the pattern
                std::string resultExtension = Path::GetExtension(resultFilename);
                if (resultExtension.empty())
                {
                    int32_t filterIndex = openFileName.nFilterIndex - 1;

                    assert(filterIndex >= 0);
                    assert(filterIndex < static_cast<int32_t>(desc.Filters.size()));

                    std::string pattern = desc.Filters[filterIndex].Pattern;
                    std::string patternExtension = Path::GetExtension(pattern);
                    if (!patternExtension.empty())
                    {
                        resultFilename += patternExtension;
                    }
                }
            }
            return resultFilename;
        }

        std::string ShowDirectoryDialog(SDL_Window* window, const std::string& title) override
        {
            std::string result;

            // Initialize COM
            if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
            {
                std::wstring titleW = String::ToWideChar(title);
                BROWSEINFOW bi = {};
                bi.lpszTitle = titleW.c_str();
                bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;

                LPITEMIDLIST pidl = SHBrowseForFolderW(&bi);
                if (pidl != nullptr)
                {
                    result = String::ToUtf8(SHGetPathFromIDListLongPath(pidl));
                }
                CoTaskMemFree(pidl);

                CoUninitialize();
            }
            else
            {
                log_error("Error opening directory browse window");
            }

            // SHBrowseForFolderW might minimize the main window,
            // so make sure that it's visible again.
            ShowWindow(GetHWND(window), SW_RESTORE);

            return result;
        }

        bool HasFilePicker() const override
        {
            return true;
        }

    private:
        HWND GetHWND(SDL_Window* window)
        {
            HWND result = nullptr;
            if (window != nullptr)
            {
                SDL_SysWMinfo wmInfo;
                SDL_VERSION(&wmInfo.version);
                if (SDL_GetWindowWMInfo(window, &wmInfo) != SDL_TRUE)
                {
                    log_error("SDL_GetWindowWMInfo failed %s", SDL_GetError());
                    exit(-1);
                }

                result = wmInfo.info.win.window;
            }
            return result;
        }

        static std::wstring GetFilterString(const std::vector<FileDialogDesc::Filter>& filters)
        {
            std::wstringstream filtersb;
            for (const auto& filter : filters)
            {
                filtersb << String::ToWideChar(filter.Name) << '\0' << String::ToWideChar(filter.Pattern) << '\0';
            }
            return filtersb.str();
        }
    };

    std::unique_ptr<IPlatformUiContext> CreatePlatformUiContext()
    {
        return std::make_unique<Win32Context>();
    }
} // namespace OpenRCT2::Ui

#endif // _WIN32
