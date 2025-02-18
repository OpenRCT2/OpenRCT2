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
#include <openrct2/scenario/Scenario.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>

namespace OpenRCT2::Ui::FileBrowser
{
    static LoadSaveCallback _loadSaveCallback;

    WindowBase* OpenPreferred(int32_t type, u8string defaultPath, LoadSaveCallback callback, TrackDesign* trackDesign)
    {
        RegisterCallback(callback);

        auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
        auto& config = Config::Get().general;

        // Open system file picker?
        if (config.UseNativeBrowseDialog && hasFilePicker)
        {
            bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;
            const u8string path = OpenSystemFileBrowser(isSave, type, defaultPath, defaultPath);
            if (!path.empty())
            {
                Select(path.c_str(), type, trackDesign);
            }
            return nullptr;
        }

        // Use built-in load/save window
        return Windows::LoadsaveOpen(type, defaultPath, callback, trackDesign);
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
                return -difftime(a.date_modified, b.date_modified) < 0;
            case FileBrowserSort::DateAscending:
                return difftime(a.date_modified, b.date_modified) < 0;
            case FileBrowserSort::SizeDescending:
                return a.fileSizeBytes - b.fileSizeBytes;
            case FileBrowserSort::SizeAscending:
                return b.fileSizeBytes - a.fileSizeBytes;
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

    u8string GetLastDirectoryByType(int32_t type)
    {
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                return Config::Get().general.LastSaveGameDirectory;

            case LOADSAVETYPE_LANDSCAPE:
                return Config::Get().general.LastSaveLandscapeDirectory;

            case LOADSAVETYPE_SCENARIO:
                return Config::Get().general.LastSaveScenarioDirectory;

            case LOADSAVETYPE_TRACK:
                return Config::Get().general.LastSaveTrackDirectory;

            default:
                return u8string();
        }
    }

    u8string GetInitialDirectoryByType(const int32_t type)
    {
        std::optional<DIRID> subdir = std::nullopt;
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                subdir = DIRID::SAVE;
                break;

            case LOADSAVETYPE_LANDSCAPE:
                subdir = DIRID::LANDSCAPE;
                break;

            case LOADSAVETYPE_SCENARIO:
                subdir = DIRID::SCENARIO;
                break;

            case LOADSAVETYPE_TRACK:
                subdir = DIRID::TRACK;
                break;

            case LOADSAVETYPE_HEIGHTMAP:
                subdir = DIRID::HEIGHTMAP;
                break;
        }

        auto env = GetContext()->GetPlatformEnvironment();
        if (subdir.has_value())
            return env->GetDirectoryPath(DIRBASE::USER, subdir.value());
        else
            return env->GetDirectoryPath(DIRBASE::USER);
    }

    const char* GetFilterPatternByType(const int32_t type, const bool isSave)
    {
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                return isSave ? "*.park" : "*.park;*.sv6;*.sc6;*.sc4;*.sv4;*.sv7;*.sea";

            case LOADSAVETYPE_LANDSCAPE:
                return isSave ? "*.park" : "*.park;*.sc6;*.sv6;*.sc4;*.sv4;*.sv7;*.sea";

            case LOADSAVETYPE_SCENARIO:
                return isSave ? "*.park" : "*.park;*.sc6;*.sc4";

            case LOADSAVETYPE_TRACK:
                return isSave ? "*.td6" : "*.td6;*.td4";

            case LOADSAVETYPE_HEIGHTMAP:
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

    u8string GetDir(const int32_t type)
    {
        u8string result = GetLastDirectoryByType(type);
        if (result.empty() || !Path::DirectoryExists(result))
        {
            result = GetInitialDirectoryByType(type);
        }
        return result;
    }

    void RegisterCallback(std::function<void(int32_t result, std::string_view)> callback)
    {
        _loadSaveCallback = callback;
    }

    void InvokeCallback(int32_t result, const utf8* path)
    {
        if (_loadSaveCallback != nullptr)
        {
            _loadSaveCallback(result, path);
        }
    }

    void Select(const char* path, int32_t type, TrackDesign* trackDesignPtr)
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

        auto& gameState = GetGameState();

        switch (type & 0x0F)
        {
            case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME):
                SetAndSaveConfigPath(Config::Get().general.LastSaveGameDirectory, pathBuffer);
                if (GetContext()->LoadParkFromFile(pathBuffer))
                {
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);
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
                    InvokeCallback(MODAL_RESULT_FAIL, pathBuffer);
                }
                break;

            case (LOADSAVETYPE_SAVE | LOADSAVETYPE_GAME):
                SetAndSaveConfigPath(Config::Get().general.LastSaveGameDirectory, pathBuffer);
                if (ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 1 : 0))
                {
                    gScenarioSavePath = pathBuffer;
                    gCurrentLoadedPath = pathBuffer;
                    gIsAutosaveLoaded = false;
                    gFirstTimeSaving = false;

                    windowMgr->CloseByClass(WindowClass::Loadsave);
                    GfxInvalidateScreen();

                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                }
                else
                {
                    ContextShowError(STR_SAVE_GAME, STR_GAME_SAVE_FAILED, {});
                    InvokeCallback(MODAL_RESULT_FAIL, pathBuffer);
                }
                break;

            case (LOADSAVETYPE_LOAD | LOADSAVETYPE_LANDSCAPE):
                SetAndSaveConfigPath(Config::Get().general.LastSaveLandscapeDirectory, pathBuffer);
                if (Editor::LoadLandscape(pathBuffer))
                {
                    gCurrentLoadedPath = pathBuffer;
                    GfxInvalidateScreen();
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                }
                else
                {
                    // Not the best message...
                    ContextShowError(STR_LOAD_LANDSCAPE, STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, {});
                    InvokeCallback(MODAL_RESULT_FAIL, pathBuffer);
                }
                break;

            case (LOADSAVETYPE_SAVE | LOADSAVETYPE_LANDSCAPE):
                SetAndSaveConfigPath(Config::Get().general.LastSaveLandscapeDirectory, pathBuffer);
                gameState.ScenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                if (ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 3 : 2))
                {
                    gCurrentLoadedPath = pathBuffer;
                    windowMgr->CloseByClass(WindowClass::Loadsave);
                    GfxInvalidateScreen();
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                }
                else
                {
                    ContextShowError(STR_SAVE_LANDSCAPE, STR_LANDSCAPE_SAVE_FAILED, {});
                    InvokeCallback(MODAL_RESULT_FAIL, pathBuffer);
                }
                break;

            case (LOADSAVETYPE_SAVE | LOADSAVETYPE_SCENARIO):
            {
                SetAndSaveConfigPath(Config::Get().general.LastSaveScenarioDirectory, pathBuffer);
                int32_t parkFlagsBackup = gameState.Park.Flags;
                gameState.Park.Flags &= ~PARK_FLAGS_SPRITES_INITIALISED;
                gameState.EditorStep = EditorStep::Invalid;
                gameState.ScenarioFileName = std::string(String::toStringView(pathBuffer, std::size(pathBuffer)));
                int32_t success = ScenarioSave(gameState, pathBuffer, Config::Get().general.SavePluginData ? 3 : 2);
                gameState.Park.Flags = parkFlagsBackup;

                if (success)
                {
                    windowMgr->CloseByClass(WindowClass::Loadsave);
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);

                    auto* context = GetContext();
                    context->SetActiveScene(context->GetTitleScene());
                }
                else
                {
                    ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_SCENARIO, STR_SCENARIO_SAVE_FAILED, {});
                    gameState.EditorStep = EditorStep::ObjectiveSelection;
                    InvokeCallback(MODAL_RESULT_FAIL, pathBuffer);
                }
                break;
            }

            case (LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK):
            {
                SetAndSaveConfigPath(Config::Get().general.LastSaveTrackDirectory, pathBuffer);
                auto intent = Intent(WindowClass::InstallTrack);
                intent.PutExtra(INTENT_EXTRA_PATH, std::string{ pathBuffer });
                ContextOpenIntent(&intent);
                windowMgr->CloseByClass(WindowClass::Loadsave);
                InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                break;
            }

            case (LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK):
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
                    InvokeCallback(MODAL_RESULT_OK, path);
                }
                else
                {
                    ContextShowError(STR_FILE_DIALOG_TITLE_SAVE_TRACK, STR_TRACK_SAVE_FAILED, {});
                    InvokeCallback(MODAL_RESULT_FAIL, path);
                }
                break;
            }

            case (LOADSAVETYPE_LOAD | LOADSAVETYPE_HEIGHTMAP):
                windowMgr->CloseByClass(WindowClass::Loadsave);
                InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                break;
        }
    }

    StringId GetTitleStringId(int32_t type, bool isSave)
    {
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;

            case LOADSAVETYPE_LANDSCAPE:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;

            case LOADSAVETYPE_SCENARIO:
                return STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;

            case LOADSAVETYPE_TRACK:
                return isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;

            case LOADSAVETYPE_HEIGHTMAP:
                return STR_FILE_DIALOG_TITLE_LOAD_HEIGHTMAP;

            default:
                return kStringIdNone;
        }
    }

    u8string OpenSystemFileBrowser(bool isSave, int32_t type, u8string defaultDirectory, u8string defaultPath)
    {
        Ui::FileDialogDesc desc = {};
        u8string extension;
        StringId title = GetTitleStringId(type, isSave);
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                extension = u8".park";
                desc.Filters.emplace_back(LanguageGetString(STR_OPENRCT2_SAVED_GAME), GetFilterPatternByType(type, isSave));
                break;

            case LOADSAVETYPE_LANDSCAPE:
                extension = u8".park";
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_LANDSCAPE_FILE), GetFilterPatternByType(type, isSave));
                break;

            case LOADSAVETYPE_SCENARIO:
                extension = u8".park";
                desc.Filters.emplace_back(LanguageGetString(STR_OPENRCT2_SCENARIO_FILE), GetFilterPatternByType(type, isSave));
                break;

            case LOADSAVETYPE_TRACK:
                extension = u8".td6";
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_TRACK_DESIGN_FILE), GetFilterPatternByType(type, isSave));
                break;

            case LOADSAVETYPE_HEIGHTMAP:
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_HEIGHTMAP_FILE), GetFilterPatternByType(type, isSave));
                break;
        }

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
                auto buffer = GetGameState().Park.Name;
                if (buffer.empty())
                {
                    buffer = LanguageGetString(STR_UNNAMED_PARK);
                }
                path = Path::Combine(path, buffer);
            }
        }

        desc.InitialDirectory = defaultDirectory;
        desc.Type = isSave ? FileDialogType::Save : FileDialogType::Open;
        desc.DefaultFilename = isSave ? path : u8string();

        desc.Filters.emplace_back(LanguageGetString(STR_ALL_FILES), "*");

        desc.Title = LanguageGetString(title);
        return ContextOpenCommonFileDialog(desc);
    }
} // namespace OpenRCT2::Ui::FileBrowser
