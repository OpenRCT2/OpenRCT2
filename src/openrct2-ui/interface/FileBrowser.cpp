/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "FileBrowser.h"

#include <cstdint>
#include <functional>
#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>

#ifdef __EMSCRIPTEN__
extern "C" {
extern void EmscriptenLoadGame(LoadSaveType type);
extern void EmscriptenSaveGame(bool isTrackDesign, bool isAutosave, LoadSaveType type);
}
#endif

namespace OpenRCT2::Ui::FileBrowser
{
    static LoadSaveCallback _loadSaveCallback;

    WindowBase* OpenPreferred(
        LoadSaveAction action, LoadSaveType type, u8string defaultPath, LoadSaveCallback callback, TrackDesign* trackDesign)
    {
        RegisterCallback(callback);

#ifdef __EMSCRIPTEN__
        if (action == LoadSaveAction::save)
        {
            Select("/save.park", action, type, trackDesign);
            EmscriptenSaveGame(type == LoadSaveType::track, false, type);
        }
        else
        {
            EmscriptenLoadGame(type);
        }
        return nullptr;
#endif

        auto hasFilePicker = GetContext()->GetUiContext().HasFilePicker();
        auto& config = Config::Get().general;

        // Open system file picker?
        if (config.useNativeBrowseDialog && hasFilePicker)
        {
            const bool isSave = (action == LoadSaveAction::save);
            const auto defaultDirectory = GetDir(type);

            const u8string path = OpenSystemFileBrowser(isSave, type, defaultDirectory, defaultPath);
            if (!path.empty())
            {
                Select(path.c_str(), action, type, trackDesign);
            }
            return nullptr;
        }

        // Use built-in load/save window
        return Windows::LoadsaveOpen(action, type, defaultPath, callback, trackDesign);
    }

    bool ListItemSort(LoadSaveListItem& a, LoadSaveListItem& b)
    {
        if (a.type != b.type)
            return EnumValue(a.type) - EnumValue(b.type) < 0;

        switch (Config::Get().general.loadSaveSort)
        {
            case FileBrowserSort::NameAscending:
                return String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
            case FileBrowserSort::NameDescending:
                return -String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
            case FileBrowserSort::DateDescending:
                return -difftime(a.dateModified, b.dateModified) < 0;
            case FileBrowserSort::DateAscending:
                return difftime(a.dateModified, b.dateModified) < 0;
            case FileBrowserSort::SizeDescending:
                return a.fileSizeBytes > b.fileSizeBytes;
            case FileBrowserSort::SizeAscending:
                return a.fileSizeBytes < b.fileSizeBytes;
        }
        return String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
    }

    void SetAndSaveConfigPath(u8string& config_str, u8string_view path)
    {
        config_str = Path::GetDirectory(path);
        Config::Save();
    }

    bool IsValidPath(const char* path)
    {
        // HACK This is needed because tracks get passed through with td?
        //      I am sure this will change eventually to use the new FileScanner
        //      which handles multiple patterns
        auto filename = Path::GetFileNameWithoutExtension(path);

        return Platform::IsFilenameValid(filename);
    }

    u8string GetLastDirectoryByType(LoadSaveType type)
    {
        switch (type)
        {
            case LoadSaveType::park:
                return Config::Get().general.lastSaveGameDirectory;

            case LoadSaveType::landscape:
                return Config::Get().general.lastSaveLandscapeDirectory;

            case LoadSaveType::scenario:
                return Config::Get().general.lastSaveScenarioDirectory;

            case LoadSaveType::track:
                return Config::Get().general.lastSaveTrackDirectory;

            default:
                return u8string();
        }
    }

    u8string GetInitialDirectoryByType(const LoadSaveType type)
    {
        std::optional<DirId> subdir = std::nullopt;
        switch (type)
        {
            case LoadSaveType::park:
                subdir = DirId::saves;
                break;

            case LoadSaveType::landscape:
                subdir = DirId::landscapes;
                break;

            case LoadSaveType::scenario:
                subdir = DirId::scenarios;
                break;

            case LoadSaveType::track:
                subdir = DirId::trackDesigns;
                break;

            case LoadSaveType::heightmap:
                subdir = DirId::heightmaps;
                break;
        }

        auto& env = GetContext()->GetPlatformEnvironment();
        if (subdir.has_value())
            return env.GetDirectoryPath(DirBase::user, subdir.value());
        else
            return env.GetDirectoryPath(DirBase::user);
    }

    const char* GetFilterPatternByType(const LoadSaveType type, const bool isSave)
    {
        switch (type)
        {
            case LoadSaveType::park:
                return isSave ? "*.park" : "*.park;*.sv6;*.sc6;*.sc4;*.sv4;*.sv7;*.sea";

            case LoadSaveType::landscape:
                return isSave ? "*.park" : "*.park;*.sc6;*.sv6;*.sc4;*.sv4;*.sv7;*.sea";

            case LoadSaveType::scenario:
                return isSave ? "*.park" : "*.park;*.sc6;*.sc4";

            case LoadSaveType::track:
                return isSave ? "*.td6" : "*.td6;*.td4";

            case LoadSaveType::heightmap:
                return "*.bmp;*.png";

            default:
                Guard::Fail("Unsupported load/save directory type.");
        }

        return nullptr;
    }

    u8string RemovePatternWildcard(u8string_view pattern)
    {
        while (pattern.length() >= 1 && pattern.front() == '*')
        {
            pattern.remove_prefix(1);
        }
        return u8string{ pattern };
    }

    u8string GetDir(const LoadSaveType type)
    {
        u8string result = GetLastDirectoryByType(type);
        if (result.empty() || !Path::DirectoryExists(result))
        {
            result = GetInitialDirectoryByType(type);
        }
        return result;
    }

    void RegisterCallback(LoadSaveCallback callback)
    {
        _loadSaveCallback = callback;
    }

    void InvokeCallback(ModalResult result, const utf8* path)
    {
        if (_loadSaveCallback != nullptr)
        {
            _loadSaveCallback(result, path);
        }
    }

    void Select(const char* path, LoadSaveAction action, LoadSaveType type, TrackDesign* trackDesignPtr)
    {
        if (!IsValidPath(path))
        {
            ContextShowError(STR_ERROR_INVALID_CHARACTERS, kStringIdNone, {});
            return;
        }

        char pathBuffer[MAX_PATH];
        String::safeUtf8Copy(pathBuffer, path, sizeof(pathBuffer));

        // Closing this will cause a Ride window to pop up, so we have to do this to ensure that
        // no windows are open (besides the toolbars and LoadSave window).
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::rideConstruction);
        windowMgr->CloseAllExceptClass(WindowClass::loadsave);

        auto& gameState = getGameState();

        switch (action)
        {
            case LoadSaveAction::load:
            {
                switch (type)
                {
                    case (LoadSaveType::park):
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveGameDirectory, pathBuffer);
                        if (GetContext()->LoadParkFromFile(pathBuffer))
                        {
                            InvokeCallback(ModalResult::ok, pathBuffer);
                            windowMgr->CloseByClass(WindowClass::loadsave);
                            GfxInvalidateScreen();
                        }
                        else
                        {
                            auto windowManager = GetWindowManager();
                            if (!windowManager->FindByClass(WindowClass::error))
                            {
                                // Not the best message...
                                ContextShowError(STR_LOAD_GAME, STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, {});
                            }
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case (LoadSaveType::landscape):
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveLandscapeDirectory, pathBuffer);
                        if (Editor::LoadLandscape(pathBuffer))
                        {
                            gCurrentLoadedPath = pathBuffer;
                            GfxInvalidateScreen();
                            InvokeCallback(ModalResult::ok, pathBuffer);
                        }
                        else
                        {
                            // Not the best message...
                            ContextShowError(STR_LOAD_LANDSCAPE, STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, {});
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case (LoadSaveType::scenario):
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveScenarioDirectory, pathBuffer);
                        int32_t parkFlagsBackup = gameState.park.flags;
                        gameState.park.flags &= ~PARK_FLAGS_SPRITES_INITIALISED;
                        gameState.editorStep = EditorStep::Invalid;
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        int32_t success = ScenarioSave(gameState, pathBuffer, Config::Get().general.savePluginData ? 3 : 2);
                        gameState.park.flags = parkFlagsBackup;

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::loadsave);
                            InvokeCallback(ModalResult::ok, pathBuffer);

                            auto* context = GetContext();
                            context->SetActiveScene(context->GetTitleScene());
                        }
                        else
                        {
                            ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_SCENARIO_SAVE_FAILED, {});
                            gameState.editorStep = EditorStep::ObjectiveSelection;
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case (LoadSaveType::track):
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveTrackDirectory, pathBuffer);
                        auto intent = Intent(WindowClass::installTrack);
                        intent.PutExtra(INTENT_EXTRA_PATH, std::string{ pathBuffer });
                        ContextOpenIntent(&intent);
                        windowMgr->CloseByClass(WindowClass::loadsave);
                        InvokeCallback(ModalResult::ok, pathBuffer);
                        break;
                    }
                    case (LoadSaveType::heightmap):
                    {
                        windowMgr->CloseByClass(WindowClass::loadsave);
                        InvokeCallback(ModalResult::ok, pathBuffer);
                        break;
                    }
                }
                break;
            }
            case LoadSaveAction::save:
            {
                switch (type)
                {
                    case LoadSaveType::park:
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveGameDirectory, pathBuffer);
                        if (ScenarioSave(gameState, pathBuffer, Config::Get().general.savePluginData ? 1 : 0))
                        {
                            gScenarioSavePath = pathBuffer;
                            gCurrentLoadedPath = pathBuffer;
                            gIsAutosaveLoaded = false;
                            gFirstTimeSaving = false;

                            windowMgr->CloseByClass(WindowClass::loadsave);
                            GfxInvalidateScreen();

                            InvokeCallback(ModalResult::ok, pathBuffer);
                        }
                        else
                        {
                            ContextShowError(STR_SAVE_GAME, STR_GAME_SAVE_FAILED, {});
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case LoadSaveType::landscape:
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveLandscapeDirectory, pathBuffer);
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        if (ScenarioSave(gameState, pathBuffer, Config::Get().general.savePluginData ? 3 : 2))
                        {
                            gCurrentLoadedPath = pathBuffer;
                            windowMgr->CloseByClass(WindowClass::loadsave);
                            GfxInvalidateScreen();
                            InvokeCallback(ModalResult::ok, pathBuffer);
                        }
                        else
                        {
                            ContextShowError(STR_SAVE_LANDSCAPE, STR_LANDSCAPE_SAVE_FAILED, {});
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case LoadSaveType::scenario:
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveScenarioDirectory, pathBuffer);
                        int32_t parkFlagsBackup = gameState.park.flags;
                        gameState.park.flags &= ~PARK_FLAGS_SPRITES_INITIALISED;
                        gameState.editorStep = EditorStep::Invalid;
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        int32_t success = ScenarioSave(gameState, pathBuffer, Config::Get().general.savePluginData ? 3 : 2);
                        gameState.park.flags = parkFlagsBackup;

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::loadsave);
                            InvokeCallback(ModalResult::ok, pathBuffer);

                            auto* context = GetContext();
                            context->SetActiveScene(context->GetTitleScene());
                        }
                        else
                        {
                            ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_SCENARIO_SAVE_FAILED, {});
                            gameState.editorStep = EditorStep::ObjectiveSelection;
                            InvokeCallback(ModalResult::fail, pathBuffer);
                        }
                        break;
                    }
                    case LoadSaveType::track:
                    {
                        SetAndSaveConfigPath(Config::Get().general.lastSaveTrackDirectory, pathBuffer);

                        const auto withExtension = Path::WithExtension(pathBuffer, ".td6");
                        String::set(pathBuffer, sizeof(pathBuffer), withExtension.c_str());

                        RCT2::T6Exporter t6Export{ *trackDesignPtr };

                        auto success = t6Export.SaveTrack(pathBuffer);

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::loadsave);
                            Windows::WindowRideMeasurementsDesignCancel();
                            InvokeCallback(ModalResult::ok, path);
                        }
                        else
                        {
                            ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_TRACK, STR_TRACK_SAVE_FAILED, {});
                            InvokeCallback(ModalResult::fail, path);
                        }
                        break;
                    }
                    case LoadSaveType::heightmap:
                    {
                        break;
                    }
                }
                break;
            }
        }
    }

    StringId GetTitleStringId(LoadSaveType type, bool isSave)
    {
        switch (type)
        {
            case LoadSaveType::park:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;

            case LoadSaveType::landscape:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;

            case LoadSaveType::scenario:
                return STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;

            case LoadSaveType::track:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;

            case LoadSaveType::heightmap:
                return STR_FILE_DIALOG_TITLE_LOAD_HEIGHTMAP;

            default:
                return kStringIdNone;
        }
    }

    static u8string GetDefaultExtensionByType(LoadSaveType type)
    {
        switch (type)
        {
            case LoadSaveType::park:
                return u8".park";

            case LoadSaveType::landscape:
                return u8".park";

            case LoadSaveType::scenario:
                return u8".park";

            case LoadSaveType::track:
                return u8".td6";

            default:
                return {};
        }
    }

    static FileDialogDesc::Filter GetFilterForType(LoadSaveType type, bool isSave)
    {
        switch (type)
        {
            case LoadSaveType::park:
                return { LanguageGetString(STR_OPENRCT2_SAVED_GAME), GetFilterPatternByType(type, isSave) };

            case LoadSaveType::landscape:
                return { LanguageGetString(STR_OPENRCT2_LANDSCAPE_FILE), GetFilterPatternByType(type, isSave) };

            case LoadSaveType::scenario:
                return { LanguageGetString(STR_OPENRCT2_SCENARIO_FILE), GetFilterPatternByType(type, isSave) };

            case LoadSaveType::track:
                return { LanguageGetString(STR_OPENRCT2_TRACK_DESIGN_FILE), GetFilterPatternByType(type, isSave) };

            case LoadSaveType::heightmap:
                return { LanguageGetString(STR_OPENRCT2_HEIGHTMAP_FILE), GetFilterPatternByType(type, isSave) };

            default:
                Guard::Fail("Unsupported load/save directory type.");
                return { "", "" };
        }
    }

    u8string OpenSystemFileBrowser(bool isSave, LoadSaveType type, u8string defaultDirectory, u8string defaultPath)
    {
        u8string path = defaultDirectory;
        if (isSave)
        {
            // The file browser requires a file path instead of just a directory
            if (!defaultPath.empty())
            {
                path = Path::Combine(path, defaultPath);
            }
            else
            {
                auto buffer = getGameState().park.name;
                if (buffer.empty())
                {
                    buffer = LanguageGetString(STR_UNNAMED_PARK);
                }
                path = Path::Combine(path, buffer);
            }
        }

        u8string extension = GetDefaultExtensionByType(type);
        StringId title = GetTitleStringId(type, isSave);

        FileDialogDesc desc = {
            .Type = isSave ? FileDialogType::Save : FileDialogType::Open,
            .Title = LanguageGetString(title),
            .InitialDirectory = defaultDirectory,
            .DefaultFilename = isSave ? path : u8string(),
            .Filters = { GetFilterForType(type, isSave), { LanguageGetString(STR_ALL_FILES), "*" } },
        };

        return ContextOpenCommonFileDialog(desc);
    }
} // namespace OpenRCT2::Ui::FileBrowser

#ifdef __EMSCRIPTEN__
extern "C" void LoadGameCallback(const char* path, LoadSaveType action)
{
    OpenRCT2::Ui::FileBrowser::Select(path, LoadSaveAction::load, action, nullptr);
}
#endif
