/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
#include <openrct2/interface/Window.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::FileBrowser
{
    static LoadSaveCallback _loadSaveCallback;

    WindowBase* OpenPreferred(
        LoadSaveAction action, LoadSaveType type, u8string defaultPath, LoadSaveCallback callback, TrackDesign* trackDesign)
    {
        RegisterCallback(callback);

        auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
        auto& config = Config::Get().general;

        // Open system file picker?
        if (config.UseNativeBrowseDialog && hasFilePicker)
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

        switch (Config::Get().general.LoadSaveSort)
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
                return Config::Get().general.LastSaveGameDirectory;

            case LoadSaveType::landscape:
                return Config::Get().general.LastSaveLandscapeDirectory;

            case LoadSaveType::scenario:
                return Config::Get().general.LastSaveScenarioDirectory;

            case LoadSaveType::track:
                return Config::Get().general.LastSaveTrackDirectory;

            default:
                return u8string();
        }
    }

    u8string GetInitialDirectoryByType(const LoadSaveType type)
    {
        std::optional<DIRID> subdir = std::nullopt;
        switch (type)
        {
            case LoadSaveType::park:
                subdir = DIRID::SAVE;
                break;

            case LoadSaveType::landscape:
                subdir = DIRID::LANDSCAPE;
                break;

            case LoadSaveType::scenario:
                subdir = DIRID::SCENARIO;
                break;

            case LoadSaveType::track:
                subdir = DIRID::TRACK;
                break;

            case LoadSaveType::heightmap:
                subdir = DIRID::HEIGHTMAP;
                break;
        }

        auto env = GetContext()->GetPlatformEnvironment();
        if (subdir.has_value())
            return env->GetDirectoryPath(DIRBASE::USER, subdir.value());
        else
            return env->GetDirectoryPath(DIRBASE::USER);
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

        return "";
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

    void RegisterCallback(std::function<void(ModalResult result, std::string_view)> callback)
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
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::RideConstruction);
        windowMgr->CloseAllExceptClass(WindowClass::Loadsave);

        auto& gameState = getGameState();

        switch (action)
        {
            case LoadSaveAction::load:
            {
                switch (type)
                {
                    case (LoadSaveType::park):
                    {
                        SetAndSaveConfigPath(Config::Get().general.LastSaveGameDirectory, pathBuffer);
                        if (GetContext()->LoadParkFromFile(pathBuffer))
                        {
                            InvokeCallback(ModalResult::ok, pathBuffer);
                            windowMgr->CloseByClass(WindowClass::Loadsave);
                            GfxInvalidateScreen();
                        }
                        else
                        {
                            auto windowManager = GetWindowManager();
                            if (!windowManager->FindByClass(WindowClass::Error))
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveLandscapeDirectory, pathBuffer);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveScenarioDirectory, pathBuffer);
                        int32_t parkFlagsBackup = gameState.park.Flags;
                        gameState.park.Flags &= ~PARK_FLAGS_SPRITES_INITIALISED;
                        gameState.editorStep = EditorStep::Invalid;
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        int32_t success = ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 3 : 2);
                        gameState.park.Flags = parkFlagsBackup;

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::Loadsave);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveTrackDirectory, pathBuffer);
                        auto intent = Intent(WindowClass::InstallTrack);
                        intent.PutExtra(INTENT_EXTRA_PATH, std::string{ pathBuffer });
                        ContextOpenIntent(&intent);
                        windowMgr->CloseByClass(WindowClass::Loadsave);
                        InvokeCallback(ModalResult::ok, pathBuffer);
                        break;
                    }
                    case (LoadSaveType::heightmap):
                    {
                        windowMgr->CloseByClass(WindowClass::Loadsave);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveGameDirectory, pathBuffer);
                        if (ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 1 : 0))
                        {
                            gScenarioSavePath = pathBuffer;
                            gCurrentLoadedPath = pathBuffer;
                            gIsAutosaveLoaded = false;
                            gFirstTimeSaving = false;

                            windowMgr->CloseByClass(WindowClass::Loadsave);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveLandscapeDirectory, pathBuffer);
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        if (ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 3 : 2))
                        {
                            gCurrentLoadedPath = pathBuffer;
                            windowMgr->CloseByClass(WindowClass::Loadsave);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveScenarioDirectory, pathBuffer);
                        int32_t parkFlagsBackup = gameState.park.Flags;
                        gameState.park.Flags &= ~PARK_FLAGS_SPRITES_INITIALISED;
                        gameState.editorStep = EditorStep::Invalid;
                        gameState.scenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                        int32_t success = ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 3 : 2);
                        gameState.park.Flags = parkFlagsBackup;

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::Loadsave);
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
                        SetAndSaveConfigPath(Config::Get().general.LastSaveTrackDirectory, pathBuffer);

                        const auto withExtension = Path::WithExtension(pathBuffer, ".td6");
                        String::set(pathBuffer, sizeof(pathBuffer), withExtension.c_str());

                        RCT2::T6Exporter t6Export{ *trackDesignPtr };

                        auto success = t6Export.SaveTrack(pathBuffer);

                        if (success)
                        {
                            windowMgr->CloseByClass(WindowClass::Loadsave);
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

    static Ui::FileDialogDesc::Filter GetFilterForType(LoadSaveType type, bool isSave)
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
                auto buffer = getGameState().park.Name;
                if (buffer.empty())
                {
                    buffer = LanguageGetString(STR_UNNAMED_PARK);
                }
                path = Path::Combine(path, buffer);
            }
        }

        u8string extension = GetDefaultExtensionByType(type);
        StringId title = GetTitleStringId(type, isSave);

        Ui::FileDialogDesc desc = {
            .Type = isSave ? FileDialogType::Save : FileDialogType::Open,
            .Title = LanguageGetString(title),
            .InitialDirectory = defaultDirectory,
            .DefaultFilename = isSave ? path : u8string(),
            .Filters = { GetFilterForType(type, isSave), { LanguageGetString(STR_ALL_FILES), "*" } },
        };

        return ContextOpenCommonFileDialog(desc);
    }
} // namespace OpenRCT2::Ui::FileBrowser
