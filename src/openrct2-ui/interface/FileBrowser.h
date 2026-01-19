/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include <cstdint>
#include <ctime>
#include <functional>
#include <openrct2/core/StringTypes.h>
#include <openrct2/localisation/StringIdType.h>
#include <string>

enum class LoadSaveAction : uint8_t;
enum class LoadSaveType : uint8_t;
enum class ModalResult : int8_t;
struct TrackDesign;

namespace OpenRCT2
{
    struct WindowBase;
}

namespace OpenRCT2::Ui::FileBrowser
{
    enum class FileType : uint8_t
    {
        directory,
        file,
    };

    struct LoadSaveListItem
    {
        std::string name{};
        std::string path{};
        time_t dateModified{ 0 };
        std::string dateFormatted{};
        std::string timeFormatted{};
        uint64_t fileSizeBytes;
        uint64_t fileSizeFormatted;
        StringId fileSizeUnit;
        FileType type{};
        bool loaded{ false };
    };

    using LoadSaveCallback = std::function<void(ModalResult result, std::string_view)>;

    bool ListItemSort(LoadSaveListItem& a, LoadSaveListItem& b);
    void SetAndSaveConfigPath(u8string& config_str, u8string_view path);
    bool IsValidPath(const char* path);
    u8string GetLastDirectoryByType(LoadSaveType type);
    u8string GetInitialDirectoryByType(LoadSaveType type);
    const char* GetFilterPatternByType(LoadSaveType type, bool isSave);
    u8string RemovePatternWildcard(u8string_view pattern);
    u8string GetDir(LoadSaveType type);
    void RegisterCallback(LoadSaveCallback callback);
    void InvokeCallback(ModalResult result, const utf8* path);
    void Select(const char* path, LoadSaveAction action, LoadSaveType type, TrackDesign* trackDesignPtr);
    StringId GetTitleStringId(LoadSaveType type, bool isSave);
    u8string OpenSystemFileBrowser(bool isSave, LoadSaveType type, u8string defaultDirectory, u8string defaultPath);
    WindowBase* OpenPreferred(
        LoadSaveAction action, LoadSaveType type, u8string defaultPath, LoadSaveCallback callback, TrackDesign* trackDesign);
} // namespace OpenRCT2::Ui::FileBrowser

#ifdef __EMSCRIPTEN__
extern "C" void LoadGameCallback(const char* path, LoadSaveType action);
#endif
