/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <SDL_keycode.h>
#include <ctime>
#include <iterator>
#include <memory>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/File.h>
#include <openrct2/core/FileScanner.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/network/network.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <string>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
#pragma region Widgets

    static constexpr StringId WINDOW_TITLE = STR_NONE;
    static constexpr int32_t WW = 350;
    static constexpr int32_t WH = 400;

    static constexpr uint16_t DATE_TIME_GAP = 2;

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_RESIZE,
        WIDX_DEFAULT,
        WIDX_UP,
        WIDX_NEW_FOLDER,
        WIDX_NEW_FILE,
        WIDX_SORT_NAME,
        WIDX_SORT_DATE,
        WIDX_SCROLL,
        WIDX_FILENAME_TEXTBOX,
        WIDX_SAVE,
        WIDX_BROWSE,
    };

    // clang-format off
    static Widget window_loadsave_widgets[] =
    {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({               0,  WH - 1}, {       WW,   1 }, WindowWidgetType::Resize,      WindowColour::Secondary                                                             ), // WIDX_RESIZE
        MakeWidget({               4,      36}, {       84,  14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_LOADSAVE_DEFAULT,              STR_LOADSAVE_DEFAULT_TIP), // WIDX_DEFAULT
        MakeWidget({              88,      36}, {       84,  14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_UP                                  ), // WIDX_UP
        MakeWidget({             172,      36}, {       87,  14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_NEW_FOLDER                          ), // WIDX_NEW_FOLDER
        MakeWidget({             259,      36}, {       87,  14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_NEW_FILE                            ), // WIDX_NEW_FILE
        MakeWidget({               4,      55}, {      170,  14 }, WindowWidgetType::TableHeader, WindowColour::Primary                                                               ), // WIDX_SORT_NAME
        MakeWidget({(WW - 5) / 2 + 1,      55}, {      170,  14 }, WindowWidgetType::TableHeader, WindowColour::Primary                                                               ), // WIDX_SORT_DATE
        MakeWidget({               4,      68}, {      342, 303 }, WindowWidgetType::Scroll,      WindowColour::Primary,   SCROLL_VERTICAL                                            ), // WIDX_SCROLL
        MakeWidget({              64, WH - 50}, { WW - 133,  14 }, WindowWidgetType::TextBox,     WindowColour::Secondary                                                             ), // WIDX_FILENAME_TEXTBOX
        MakeWidget({         WW - 65, WH - 50}, {       60,  14 }, WindowWidgetType::Button,      WindowColour::Secondary, STR_FILEBROWSER_SAVE_BUTTON                                ), // WIDX_SAVE
        MakeWidget({               4, WH - 24}, {      197,  19 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_USE_SYSTEM_WINDOW                          ), // WIDX_BROWSE
        kWidgetsEnd,
    };
    // clang-format on

#pragma endregion

    static WindowBase* WindowOverwritePromptOpen(const std::string_view name, const std::string_view path);

    enum class FileType : uint8_t
    {
        directory,
        file,
    };

    struct LoadSaveListItem
    {
        std::string name{};
        std::string path{};
        time_t date_modified{ 0 };
        std::string date_formatted{};
        std::string time_formatted{};
        FileType type{};
        bool loaded{ false };
    };

    static std::function<void(int32_t result, std::string_view)> _loadSaveCallback;
    static TrackDesign* _trackDesign;

    static std::vector<LoadSaveListItem> _listItems;
    static char _directory[MAX_PATH];
    static char _parentDirectory[MAX_PATH];
    static char _currentFilename[MAX_PATH];
    static u8string _extensionPattern;
    static u8string _defaultPath;
    static int32_t _type;

    static bool ListItemSort(LoadSaveListItem& a, LoadSaveListItem& b)
    {
        if (a.type != b.type)
            return EnumValue(a.type) - EnumValue(b.type) < 0;

        switch (Config::Get().general.LoadSaveSort)
        {
            case Sort::NameAscending:
                return String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
            case Sort::NameDescending:
                return -String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
            case Sort::DateDescending:
                return -difftime(a.date_modified, b.date_modified) < 0;
            case Sort::DateAscending:
                return difftime(a.date_modified, b.date_modified) < 0;
        }
        return String::logicalCmp(a.name.c_str(), b.name.c_str()) < 0;
    }

    static void SetAndSaveConfigPath(u8string& config_str, u8string_view path)
    {
        config_str = Path::GetDirectory(path);
        Config::Save();
    }

    static bool IsValidPath(const char* path)
    {
        // HACK This is needed because tracks get passed through with td?
        //      I am sure this will change eventually to use the new FileScanner
        //      which handles multiple patterns
        auto filename = Path::GetFileNameWithoutExtension(path);

        return Platform::IsFilenameValid(filename);
    }

    static u8string GetLastDirectoryByType(int32_t type)
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

    static u8string GetInitialDirectoryByType(const int32_t type)
    {
        std::optional<OpenRCT2::DIRID> subdir = std::nullopt;
        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                subdir = OpenRCT2::DIRID::SAVE;
                break;

            case LOADSAVETYPE_LANDSCAPE:
                subdir = OpenRCT2::DIRID::LANDSCAPE;
                break;

            case LOADSAVETYPE_SCENARIO:
                subdir = OpenRCT2::DIRID::SCENARIO;
                break;

            case LOADSAVETYPE_TRACK:
                subdir = OpenRCT2::DIRID::TRACK;
                break;

            case LOADSAVETYPE_HEIGHTMAP:
                subdir = OpenRCT2::DIRID::HEIGHTMAP;
                break;
        }

        auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
        if (subdir.has_value())
            return env->GetDirectoryPath(OpenRCT2::DIRBASE::USER, subdir.value());
        else
            return env->GetDirectoryPath(OpenRCT2::DIRBASE::USER);
    }

    static const char* GetFilterPatternByType(const int32_t type, const bool isSave)
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

    static u8string RemovePatternWildcard(u8string_view pattern)
    {
        while (pattern.length() >= 1 && pattern.front() == '*')
        {
            pattern.remove_prefix(1);
        }
        return u8string{ pattern };
    }

    static u8string GetDir(const int32_t type)
    {
        u8string result = GetLastDirectoryByType(type);
        if (result.empty() || !Path::DirectoryExists(result))
        {
            result = GetInitialDirectoryByType(type);
        }
        return result;
    }

    static void InvokeCallback(int32_t result, const utf8* path)
    {
        if (_loadSaveCallback != nullptr)
        {
            _loadSaveCallback(result, path);
        }
    }

    static void Select(const char* path)
    {
        if (!IsValidPath(path))
        {
            ContextShowError(STR_ERROR_INVALID_CHARACTERS, STR_NONE, {});
            return;
        }

        char pathBuffer[MAX_PATH];
        String::safeUtf8Copy(pathBuffer, path, sizeof(pathBuffer));

        // Closing this will cause a Ride window to pop up, so we have to do this to ensure that
        // no windows are open (besides the toolbars and LoadSave window).
        WindowCloseByClass(WindowClass::RideConstruction);
        WindowCloseAllExceptClass(WindowClass::Loadsave);

        auto& gameState = GetGameState();

        switch (_type & 0x0F)
        {
            case (LOADSAVETYPE_LOAD | LOADSAVETYPE_GAME):
                SetAndSaveConfigPath(Config::Get().general.LastSaveGameDirectory, pathBuffer);
                if (OpenRCT2::GetContext()->LoadParkFromFile(pathBuffer))
                {
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                    WindowCloseByClass(WindowClass::Loadsave);
                    GfxInvalidateScreen();
                }
                else
                {
                    // Not the best message...
                    ContextShowError(STR_LOAD_GAME, STR_FAILED_TO_LOAD_FILE_CONTAINS_INVALID_DATA, {});
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

                    WindowCloseByClass(WindowClass::Loadsave);
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
                    WindowCloseByClass(WindowClass::Loadsave);
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
                    WindowCloseByClass(WindowClass::Loadsave);
                    InvokeCallback(MODAL_RESULT_OK, pathBuffer);

                    auto* context = OpenRCT2::GetContext();
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
                WindowCloseByClass(WindowClass::Loadsave);
                InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                break;
            }

            case (LOADSAVETYPE_SAVE | LOADSAVETYPE_TRACK):
            {
                SetAndSaveConfigPath(Config::Get().general.LastSaveTrackDirectory, pathBuffer);

                const auto withExtension = Path::WithExtension(pathBuffer, ".td6");
                String::set(pathBuffer, sizeof(pathBuffer), withExtension.c_str());

                RCT2::T6Exporter t6Export{ *_trackDesign };

                auto success = t6Export.SaveTrack(pathBuffer);

                if (success)
                {
                    WindowCloseByClass(WindowClass::Loadsave);
                    WindowRideMeasurementsDesignCancel();
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
                WindowCloseByClass(WindowClass::Loadsave);
                InvokeCallback(MODAL_RESULT_OK, pathBuffer);
                break;
        }
    }

    static u8string OpenSystemFileBrowser(bool isSave)
    {
        OpenRCT2::Ui::FileDialogDesc desc = {};
        u8string extension;
        StringId title = STR_NONE;
        switch (_type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                extension = u8".park";
                title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;
                desc.Filters.emplace_back(LanguageGetString(STR_OPENRCT2_SAVED_GAME), GetFilterPatternByType(_type, isSave));
                break;

            case LOADSAVETYPE_LANDSCAPE:
                extension = u8".park";
                title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_LANDSCAPE_FILE), GetFilterPatternByType(_type, isSave));
                break;

            case LOADSAVETYPE_SCENARIO:
                extension = u8".park";
                title = STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;
                desc.Filters.emplace_back(LanguageGetString(STR_OPENRCT2_SCENARIO_FILE), GetFilterPatternByType(_type, isSave));
                break;

            case LOADSAVETYPE_TRACK:
                extension = u8".td6";
                title = isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_TRACK_DESIGN_FILE), GetFilterPatternByType(_type, isSave));
                break;

            case LOADSAVETYPE_HEIGHTMAP:
                title = STR_FILE_DIALOG_TITLE_LOAD_HEIGHTMAP;
                desc.Filters.emplace_back(
                    LanguageGetString(STR_OPENRCT2_HEIGHTMAP_FILE), GetFilterPatternByType(_type, isSave));
                break;
        }

        u8string path = _directory;
        if (isSave)
        {
            // The file browser requires a file path instead of just a directory
            if (!_defaultPath.empty())
            {
                path = Path::Combine(path, _defaultPath);
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

        desc.InitialDirectory = _directory;
        desc.Type = isSave ? OpenRCT2::Ui::FileDialogType::Save : OpenRCT2::Ui::FileDialogType::Open;
        desc.DefaultFilename = isSave ? path : u8string();

        desc.Filters.emplace_back(LanguageGetString(STR_ALL_FILES), "*");

        desc.Title = LanguageGetString(title);
        return ContextOpenCommonFileDialog(desc);
    }

    class LoadSaveWindow final : public Window
    {
    public:
        LoadSaveWindow(int32_t loadSaveType)
            : type(loadSaveType)
        {
        }

    private:
        int32_t maxDateWidth{ 0 };
        int32_t maxTimeWidth{ 0 };
        int32_t type;

    public:
        void PopulateList(bool includeNewItem, const u8string& directory, std::string_view extensionPattern)
        {
            const auto absoluteDirectory = Path::GetAbsolute(directory);
            String::safeUtf8Copy(_directory, absoluteDirectory.c_str(), std::size(_directory));
            // Note: This compares the pointers, not values
            _extensionPattern = extensionPattern;

            _listItems.clear();

            // Show "new" buttons when saving
            window_loadsave_widgets[WIDX_NEW_FILE].type = includeNewItem ? WindowWidgetType::Button : WindowWidgetType::Empty;
            window_loadsave_widgets[WIDX_NEW_FOLDER].type = includeNewItem ? WindowWidgetType::Button : WindowWidgetType::Empty;

            int32_t drives = Platform::GetDrives();
            if (directory.empty() && drives)
            {
                // List Windows drives
                disabled_widgets |= (1uLL << WIDX_NEW_FILE) | (1uLL << WIDX_NEW_FOLDER) | (1uLL << WIDX_UP);
                static constexpr auto NumDriveLetters = 26;
                for (int32_t x = 0; x < NumDriveLetters; x++)
                {
                    if (drives & (1 << x))
                    {
                        // If the drive exists, list it
                        LoadSaveListItem newListItem;
                        newListItem.path = std::string(1, 'A' + x) + ":" PATH_SEPARATOR;
                        newListItem.name = newListItem.path;
                        newListItem.type = FileType::directory;

                        _listItems.push_back(std::move(newListItem));
                    }
                }
            }
            else
            {
                // Remove the separator at the end of the path, if present
                String::safeUtf8Copy(_parentDirectory, absoluteDirectory.c_str(), std::size(_parentDirectory));
                if (_parentDirectory[strlen(_parentDirectory) - 1] == *PATH_SEPARATOR
                    || _parentDirectory[strlen(_parentDirectory) - 1] == '/')
                    _parentDirectory[strlen(_parentDirectory) - 1] = '\0';

                // Remove everything past the now last separator
                char* ch = strrchr(_parentDirectory, *PATH_SEPARATOR);
                char* posix_ch = strrchr(_parentDirectory, '/');
                ch = ch < posix_ch ? posix_ch : ch;
                if (ch != nullptr)
                {
                    *(ch + 1) = '\0';
                }
                else if (drives)
                {
                    // If on Windows, clear the entire path to show the drives
                    _parentDirectory[0] = '\0';
                }
                else
                {
                    // Else, go to the root directory
                    snprintf(_parentDirectory, MAX_PATH, "%c", *PATH_SEPARATOR);
                }

                // Disable the Up button if the current directory is the root directory
                if (String::isNullOrEmpty(_parentDirectory) && !drives)
                    disabled_widgets |= (1uLL << WIDX_UP);
                else
                    disabled_widgets &= ~(1uLL << WIDX_UP);

                // Re-enable the "new" buttons if these were disabled
                disabled_widgets &= ~(1uLL << WIDX_NEW_FILE);
                disabled_widgets &= ~(1uLL << WIDX_NEW_FOLDER);

                // List all directories
                auto subDirectories = Path::GetDirectories(absoluteDirectory);
                for (const auto& sdName : subDirectories)
                {
                    auto subDir = sdName + PATH_SEPARATOR;

                    LoadSaveListItem newListItem;
                    newListItem.path = Path::Combine(absoluteDirectory, subDir);
                    newListItem.name = std::move(subDir);
                    newListItem.type = FileType::directory;
                    newListItem.loaded = false;

                    _listItems.push_back(std::move(newListItem));
                }

                // List all files with the wanted extensions
                bool showExtension = false;
                for (const u8string& extToken : String::split(extensionPattern, ";"))
                {
                    const u8string filter = Path::Combine(directory, extToken);
                    auto scanner = Path::ScanDirectory(filter, false);
                    while (scanner->Next())
                    {
                        LoadSaveListItem newListItem;
                        newListItem.path = scanner->GetPath();
                        newListItem.type = FileType::file;
                        newListItem.date_modified = Platform::FileGetModifiedTime(newListItem.path.c_str());

                        // Cache a human-readable version of the modified date.
                        newListItem.date_formatted = Platform::FormatShortDate(newListItem.date_modified);
                        newListItem.time_formatted = Platform::FormatTime(newListItem.date_modified);

                        // Mark if file is the currently loaded game
                        newListItem.loaded = newListItem.path.compare(gCurrentLoadedPath) == 0;

                        // Remove the extension (but only the first extension token)
                        if (!showExtension)
                        {
                            newListItem.name = Path::GetFileNameWithoutExtension(newListItem.path);
                        }
                        else
                        {
                            newListItem.name = Path::GetFileName(newListItem.path);
                        }

                        _listItems.push_back(std::move(newListItem));
                    }

                    showExtension = true; // Show any extension after the first iteration
                }

                SortList();
            }

            Invalidate();
        }

        void ComputeMaxDateWidth()
        {
            // Generate a time object for a relatively wide time: 2000-02-20 00:00:00
            std::tm tm;
            tm.tm_sec = 0;
            tm.tm_min = 0;
            tm.tm_hour = 0;
            tm.tm_mday = 20;
            tm.tm_mon = 2;
            tm.tm_year = 100;
            tm.tm_wday = 5;
            tm.tm_yday = 51;
            tm.tm_isdst = -1;

            std::time_t long_time = mktime(&tm);

            // Check how this date is represented (e.g. 2000-02-20, or 00/02/20)
            std::string date = Platform::FormatShortDate(long_time);
            maxDateWidth = GfxGetStringWidth(date.c_str(), FontStyle::Medium) + DATE_TIME_GAP;

            // Some locales do not use leading zeros for months and days, so let's try October, too.
            tm.tm_mon = 10;
            tm.tm_yday = 294;
            long_time = mktime(&tm);

            // Again, check how this date is represented (e.g. 2000-10-20, or 00/10/20)
            date = Platform::FormatShortDate(long_time);
            maxDateWidth = std::max(maxDateWidth, GfxGetStringWidth(date.c_str(), FontStyle::Medium) + DATE_TIME_GAP);

            // Time appears to be universally represented with two digits for minutes, so 12:00 or 00:00 should be
            // representable.
            std::string time = Platform::FormatTime(long_time);
            maxTimeWidth = GfxGetStringWidth(time.c_str(), FontStyle::Medium) + DATE_TIME_GAP;
        }

        void SortList()
        {
            std::sort(_listItems.begin(), _listItems.end(), ListItemSort);
        }

#pragma region Events
    public:
        void OnOpen() override
        {
            widgets = window_loadsave_widgets;

            const auto uiContext = OpenRCT2::GetContext()->GetUiContext();
            if (!uiContext->HasFilePicker())
            {
                disabled_widgets |= (1uLL << WIDX_BROWSE);
                window_loadsave_widgets[WIDX_BROWSE].type = WindowWidgetType::Empty;
            }

            // TODO: Split LOADSAVETYPE_* into two proper enum classes (one for load/save, the other for the type)
            const bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;
            const auto path = GetDir(type);

            // Pause the game if not on title scene, nor in network play.
            if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused |= GAME_PAUSED_MODAL;
                Audio::StopAll();
            }

            if (isSave)
            {
                widgets[WIDX_FILENAME_TEXTBOX].type = WindowWidgetType::TextBox;
                widgets[WIDX_FILENAME_TEXTBOX].string = _currentFilename;
                widgets[WIDX_SAVE].type = WindowWidgetType::Button;

                // Set current filename
                String::set(_currentFilename, sizeof(_currentFilename), _defaultPath.c_str());

                // Focus textbox
                WindowStartTextbox(*this, WIDX_FILENAME_TEXTBOX, _currentFilename, sizeof(_currentFilename));
            }
            else
            {
                widgets[WIDX_FILENAME_TEXTBOX].type = WindowWidgetType::Empty;
                widgets[WIDX_SAVE].type = WindowWidgetType::Empty;
            }

            // Populate file list
            const char* pattern = GetFilterPatternByType(type, isSave);
            PopulateList(isSave, path, pattern);
            no_list_items = static_cast<uint16_t>(_listItems.size());
            selected_list_item = -1;

            // Reset window dimensions
            InitScrollWidgets();
            ComputeMaxDateWidth();
            min_width = WW;
            min_height = WH / 2;
            max_width = WW * 2;
            max_height = WH * 2;
        }

        void OnClose() override
        {
            _listItems.clear();
            WindowCloseByClass(WindowClass::LoadsaveOverwritePrompt);

            // Unpause the game if not on title scene, nor in network play.
            if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused &= ~GAME_PAUSED_MODAL;
                Audio::Resume();
            }
        }

        void OnResize() override
        {
            if (width < min_width)
            {
                Invalidate();
                width = min_width;
            }
            if (height < min_height)
            {
                Invalidate();
                height = min_height;
            }
        }

        void OnUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                WidgetInvalidate(*this, WIDX_FILENAME_TEXTBOX);
            }
        }

        void OnPrepareDraw() override
        {
            ResizeFrameWithPage();

            Widget* date_widget = &window_loadsave_widgets[WIDX_SORT_DATE];
            date_widget->right = width - 5;
            date_widget->left = date_widget->right
                - (maxDateWidth + maxTimeWidth + (4 * DATE_TIME_GAP) + (kScrollBarWidth + 1));

            window_loadsave_widgets[WIDX_SORT_NAME].left = 4;
            window_loadsave_widgets[WIDX_SORT_NAME].right = window_loadsave_widgets[WIDX_SORT_DATE].left - 1;

            window_loadsave_widgets[WIDX_SCROLL].right = width - 5;
            window_loadsave_widgets[WIDX_SCROLL].bottom = height - 30;

            if (_type & LOADSAVETYPE_SAVE)
            {
                window_loadsave_widgets[WIDX_SCROLL].bottom -= 18;

                // Get 'Save' button string width
                auto saveLabel = LanguageGetString(STR_FILEBROWSER_SAVE_BUTTON);
                auto saveLabelWidth = GfxGetStringWidth(saveLabel, FontStyle::Medium) + 16;

                window_loadsave_widgets[WIDX_SAVE].top = height - 42;
                window_loadsave_widgets[WIDX_SAVE].bottom = height - 30;
                window_loadsave_widgets[WIDX_SAVE].left = width - saveLabelWidth - 5;
                window_loadsave_widgets[WIDX_SAVE].right = width - 5;

                // Get 'Filename:' string width
                auto filenameLabel = LanguageGetString(STR_FILENAME);
                auto filenameLabelWidth = GfxGetStringWidth(filenameLabel, FontStyle::Medium);

                window_loadsave_widgets[WIDX_FILENAME_TEXTBOX].top = height - 42;
                window_loadsave_widgets[WIDX_FILENAME_TEXTBOX].bottom = height - 30;
                window_loadsave_widgets[WIDX_FILENAME_TEXTBOX].left = 4 + filenameLabelWidth + 6;
                window_loadsave_widgets[WIDX_FILENAME_TEXTBOX].right = window_loadsave_widgets[WIDX_SAVE].left - 5;
            }

            // 'Use system file browser'
            window_loadsave_widgets[WIDX_BROWSE].top = height - 24;
            window_loadsave_widgets[WIDX_BROWSE].bottom = height - 6;
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            const auto shortPath = ShortenPath(_directory, width - 8, FontStyle::Medium);

            // Format text
            std::string buffer;
            buffer.assign("{BLACK}");
            buffer += shortPath;

            // Draw path text
            const auto normalisedPath = Platform::StrDecompToPrecomp(buffer.data());
            const auto* normalisedPathC = normalisedPath.c_str();
            auto ft = Formatter();
            ft.Add<const char*>(normalisedPathC);
            DrawTextEllipsised(dpi, windowPos + ScreenCoordsXY{ 4, 20 }, width - 8, STR_STRING, ft);

            // Name button text
            StringId id = STR_NONE;
            if (Config::Get().general.LoadSaveSort == Sort::NameAscending)
                id = STR_UP;
            else if (Config::Get().general.LoadSaveSort == Sort::NameDescending)
                id = STR_DOWN;

            // Draw name button indicator.
            auto& sort_name_widget = widgets[WIDX_SORT_NAME];
            ft = Formatter();
            ft.Add<StringId>(id);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ sort_name_widget.left + 11, sort_name_widget.top + 1 }, STR_NAME, ft,
                { COLOUR_GREY });

            // Date button text
            if (Config::Get().general.LoadSaveSort == Sort::DateAscending)
                id = STR_UP;
            else if (Config::Get().general.LoadSaveSort == Sort::DateDescending)
                id = STR_DOWN;
            else
                id = STR_NONE;

            auto& sort_date_widget = widgets[WIDX_SORT_DATE];
            ft = Formatter();
            ft.Add<StringId>(id);
            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ sort_date_widget.left + 5, sort_date_widget.top + 1 }, STR_DATE, ft,
                { COLOUR_GREY });

            // 'Filename:' label
            if (_type & LOADSAVETYPE_SAVE)
            {
                auto& widget = widgets[WIDX_FILENAME_TEXTBOX];
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 5, widget.top + 2 }, STR_FILENAME, ft, { COLOUR_GREY });
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            bool isSave = (_type & 0x01) == LOADSAVETYPE_SAVE;
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;

                case WIDX_UP:
                    PopulateList(isSave, _parentDirectory, _extensionPattern);
                    InitScrollWidgets();
                    no_list_items = static_cast<uint16_t>(_listItems.size());
                    break;

                case WIDX_NEW_FILE:
                    WindowTextInputOpen(
                        this, WIDX_NEW_FILE, STR_NONE, STR_FILEBROWSER_FILE_NAME_PROMPT, {}, STR_STRING,
                        reinterpret_cast<uintptr_t>(_defaultPath.c_str()), 64);
                    break;

                case WIDX_NEW_FOLDER:
                    WindowTextInputRawOpen(this, WIDX_NEW_FOLDER, STR_NONE, STR_FILEBROWSER_FOLDER_NAME_PROMPT, {}, "", 64);
                    break;

                case WIDX_BROWSE:
                {
                    u8string path = OpenSystemFileBrowser(isSave);
                    if (!path.empty())
                    {
                        Select(path.c_str());
                    }
                    else
                    {
                        // If user cancels file dialog, refresh list
                        PopulateList(isSave, _directory, _extensionPattern);
                        InitScrollWidgets();
                        no_list_items = static_cast<uint16_t>(_listItems.size());
                    }
                }
                break;

                case WIDX_SORT_NAME:
                    if (Config::Get().general.LoadSaveSort == Sort::NameAscending)
                    {
                        Config::Get().general.LoadSaveSort = Sort::NameDescending;
                    }
                    else
                    {
                        Config::Get().general.LoadSaveSort = Sort::NameAscending;
                    }
                    Config::Save();
                    SortList();
                    Invalidate();
                    break;

                case WIDX_SORT_DATE:
                    if (Config::Get().general.LoadSaveSort == Sort::DateDescending)
                    {
                        Config::Get().general.LoadSaveSort = Sort::DateAscending;
                    }
                    else
                    {
                        Config::Get().general.LoadSaveSort = Sort::DateDescending;
                    }
                    Config::Save();
                    SortList();
                    Invalidate();
                    break;

                case WIDX_DEFAULT:
                    PopulateList(isSave, GetInitialDirectoryByType(_type).c_str(), _extensionPattern);
                    InitScrollWidgets();
                    no_list_items = static_cast<uint16_t>(_listItems.size());
                    break;

                case WIDX_FILENAME_TEXTBOX:
                    WindowStartTextbox(*this, widgetIndex, _currentFilename, sizeof(_currentFilename));
                    break;

                case WIDX_SAVE:
                {
                    const u8string path = Path::WithExtension(
                        Path::Combine(_directory, _currentFilename), RemovePatternWildcard(_extensionPattern));

                    if (File::Exists(path))
                        WindowOverwritePromptOpen(_currentFilename, path);
                    else
                        Select(path.c_str());
                }
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            if (!Platform::IsFilenameValid(text))
            {
                ContextShowError(STR_ERROR_INVALID_CHARACTERS, STR_NONE, {});
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_NEW_FOLDER:
                {
                    const u8string path = Path::Combine(_directory, text);
                    if (!Path::CreateDirectory(path))
                    {
                        ContextShowError(STR_UNABLE_TO_CREATE_FOLDER, STR_NONE, {});
                        return;
                    }

                    no_list_items = 0;
                    selected_list_item = -1;

                    PopulateList((_type & 1) == LOADSAVETYPE_SAVE, path, _extensionPattern);
                    InitScrollWidgets();

                    no_list_items = static_cast<uint16_t>(_listItems.size());
                    Invalidate();
                    break;
                }

                case WIDX_NEW_FILE:
                {
                    const u8string path = Path::WithExtension(
                        Path::Combine(_directory, text), RemovePatternWildcard(_extensionPattern));

                    if (File::Exists(path))
                        WindowOverwritePromptOpen(text, path);
                    else
                        Select(path.c_str());
                    break;
                }

                case WIDX_FILENAME_TEXTBOX:
                {
                    std::string tempText = text.data();
                    const char* cStr = tempText.c_str();
                    if (strcmp(_currentFilename, cStr) == 0)
                        return;

                    String::safeUtf8Copy(_currentFilename, cStr, sizeof(_currentFilename));
                }
            }
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, no_list_items * kScrollableRowHeight };
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedItem;

            selectedItem = screenCoords.y / kScrollableRowHeight;
            if (selectedItem >= no_list_items)
                return;

            if (selected_list_item != selectedItem)
            {
                selected_list_item = selectedItem;
                Invalidate();
            }
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedItem;

            selectedItem = screenCoords.y / kScrollableRowHeight;
            if (selectedItem >= no_list_items)
                return;

            if (_listItems[selectedItem].type == FileType::directory)
            {
                // The selected item is a folder
                no_list_items = 0;
                selected_list_item = -1;
                bool includeNewItem = (_type & 1) == LOADSAVETYPE_SAVE;

                char directory[MAX_PATH];
                String::safeUtf8Copy(directory, _listItems[selectedItem].path.c_str(), sizeof(directory));

                PopulateList(includeNewItem, directory, _extensionPattern);
                InitScrollWidgets();

                no_list_items = static_cast<uint16_t>(_listItems.size());
            }
            else // FileType::file
            {
                // Load or overwrite
                String::set(_currentFilename, std::size(_currentFilename), _listItems[selectedItem].name.c_str());
                WidgetInvalidate(*this, WIDX_FILENAME_TEXTBOX);

                if ((_type & 0x01) == LOADSAVETYPE_SAVE)
                    WindowOverwritePromptOpen(_listItems[selectedItem].name, _listItems[selectedItem].path);
                else
                    Select(_listItems[selectedItem].path.c_str());
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            GfxFillRect(
                dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } },
                ColourMapA[colours[1].colour].mid_light);
            const int32_t listWidth = widgets[WIDX_SCROLL].width();
            const int32_t dateAnchor = widgets[WIDX_SORT_DATE].left + maxDateWidth + DATE_TIME_GAP;

            for (int32_t i = 0; i < no_list_items; i++)
            {
                int32_t y = i * kScrollableRowHeight;
                if (y > dpi.y + dpi.height)
                    break;

                if (y + kScrollableRowHeight < dpi.y)
                    continue;

                StringId stringId = STR_BLACK_STRING;

                // If hovering over item, change the color and fill the backdrop.
                if (i == selected_list_item)
                {
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    GfxFilterRect(dpi, { 0, y, listWidth, y + kScrollableRowHeight }, FilterPaletteID::PaletteDarken1);
                }
                // display a marker next to the currently loaded game file
                if (_listItems[i].loaded)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_RIGHTGUILLEMET);
                    DrawTextBasic(dpi, { 0, y }, stringId, ft);
                }

                // Folders get a folder icon
                if (_listItems[i].type == FileType::directory)
                {
                    GfxDrawSprite(dpi, ImageId(SPR_G2_FOLDER), { 1, y });
                }

                // Print filename
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(_listItems[i].name.c_str());
                int32_t max_file_width = widgets[WIDX_SORT_NAME].width() - 15;
                DrawTextEllipsised(dpi, { 15, y }, max_file_width, stringId, ft);

                // Print formatted modified date, if this is a file
                if (_listItems[i].type == FileType::file)
                {
                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].date_formatted.c_str());
                    DrawTextEllipsised(
                        dpi, { dateAnchor - DATE_TIME_GAP, y }, maxDateWidth, stringId, ft, { TextAlignment::RIGHT });

                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].time_formatted.c_str());
                    DrawTextEllipsised(dpi, { dateAnchor + DATE_TIME_GAP, y }, maxTimeWidth, stringId, ft);
                }
            }
        }
#pragma endregion
    };

    WindowBase* LoadsaveOpen(
        int32_t type, std::string_view defaultPath, std::function<void(int32_t result, std::string_view)> callback,
        TrackDesign* trackDesign)
    {
        _loadSaveCallback = callback;
        _trackDesign = trackDesign;
        _type = type;
        _defaultPath = defaultPath;

        bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;

        // Bypass the lot?
        auto hasFilePicker = OpenRCT2::GetContext()->GetUiContext()->HasFilePicker();
        if (Config::Get().general.UseNativeBrowseDialog && hasFilePicker)
        {
            const u8string path = OpenSystemFileBrowser(isSave);
            if (!path.empty())
            {
                Select(path.c_str());
            }
            return nullptr;
        }

        const u8string path = GetDir(type);

        auto* w = static_cast<LoadSaveWindow*>(WindowBringToFrontByClass(WindowClass::Loadsave));
        if (w == nullptr)
        {
            w = WindowCreate<LoadSaveWindow>(
                WindowClass::Loadsave, WW, WH, WF_STICK_TO_FRONT | WF_RESIZABLE | WF_AUTO_POSITION | WF_CENTRE_SCREEN, type);
        }

        switch (type & 0x0E)
        {
            case LOADSAVETYPE_GAME:
                w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_GAME : STR_FILE_DIALOG_TITLE_LOAD_GAME;
                break;

            case LOADSAVETYPE_LANDSCAPE:
                w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_LANDSCAPE
                                                     : STR_FILE_DIALOG_TITLE_LOAD_LANDSCAPE;
                break;

            case LOADSAVETYPE_SCENARIO:
                w->widgets[WIDX_TITLE].text = STR_FILE_DIALOG_TITLE_SAVE_SCENARIO;
                break;

            case LOADSAVETYPE_TRACK:
                w->widgets[WIDX_TITLE].text = isSave ? STR_FILE_DIALOG_TITLE_SAVE_TRACK
                                                     : STR_FILE_DIALOG_TITLE_INSTALL_NEW_TRACK_DESIGN;
                break;

            case LOADSAVETYPE_HEIGHTMAP:
                Guard::Assert(!isSave, "Cannot save images through loadsave window");
                w->widgets[WIDX_TITLE].text = STR_FILE_DIALOG_TITLE_LOAD_HEIGHTMAP;
                break;

            default:
                Guard::Fail("Unsupported load/save type: %d", type & 0x0F);
                break;
        }

        return w;
    }

    void WindowLoadSaveInputKey(WindowBase* w, uint32_t keycode)
    {
        if (w->classification != WindowClass::Loadsave)
        {
            return;
        }

        auto loadSaveWindow = static_cast<LoadSaveWindow*>(w);

        if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
        {
            loadSaveWindow->OnMouseUp(WIDX_SAVE);
        }
        else if (keycode == SDLK_ESCAPE)
        {
            loadSaveWindow->Close();
        }
    }

#pragma region Overwrite prompt

    constexpr int32_t OVERWRITE_WW = 200;
    constexpr int32_t OVERWRITE_WH = 100;

    enum
    {
        WIDX_OVERWRITE_BACKGROUND,
        WIDX_OVERWRITE_TITLE,
        WIDX_OVERWRITE_CLOSE,
        WIDX_OVERWRITE_OVERWRITE,
        WIDX_OVERWRITE_CANCEL
    };

    static Widget window_overwrite_prompt_widgets[] = {
        WINDOW_SHIM_WHITE(STR_FILEBROWSER_OVERWRITE_TITLE, OVERWRITE_WW, OVERWRITE_WH),
        { WindowWidgetType::Button, 0, 10, 94, OVERWRITE_WH - 20, OVERWRITE_WH - 9, STR_FILEBROWSER_OVERWRITE_TITLE, STR_NONE },
        { WindowWidgetType::Button, 0, OVERWRITE_WW - 95, OVERWRITE_WW - 11, OVERWRITE_WH - 20, OVERWRITE_WH - 9,
          STR_SAVE_PROMPT_CANCEL, STR_NONE },
        kWidgetsEnd,
    };

    class OverwritePromptWindow final : public Window
    {
        std::string _name;
        std::string _path;

    public:
        OverwritePromptWindow(const std::string_view name, const std::string_view path)
            : _name(name)
            , _path(path)
        {
        }

        void OnOpen() override
        {
            widgets = window_overwrite_prompt_widgets;
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_OVERWRITE_OVERWRITE:
                {
                    Select(_path.c_str());

                    // As the LoadSaveWindow::Select function can change the order of the
                    // windows we can't use WindowClose(w).
                    WindowCloseByClass(WindowClass::LoadsaveOverwritePrompt);
                    break;
                }

                case WIDX_OVERWRITE_CANCEL:
                case WIDX_OVERWRITE_CLOSE:
                    Close();
                    break;
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<char*>(_name.c_str());

            ScreenCoordsXY stringCoords(windowPos.x + width / 2, windowPos.y + (height / 2) - 3);
            DrawTextWrapped(dpi, stringCoords, width - 4, STR_FILEBROWSER_OVERWRITE_PROMPT, ft, { TextAlignment::CENTRE });
        }
    };

    static WindowBase* WindowOverwritePromptOpen(const std::string_view name, const std::string_view path)
    {
        WindowCloseByClass(WindowClass::LoadsaveOverwritePrompt);

        return WindowCreate<OverwritePromptWindow>(
            WindowClass::LoadsaveOverwritePrompt, OVERWRITE_WW, OVERWRITE_WH,
            WF_TRANSPARENT | WF_STICK_TO_FRONT | WF_CENTRE_SCREEN, name, path);
    }

    void WindowLoadSaveOverwritePromptInputKey(WindowBase* w, uint32_t keycode)
    {
        if (w->classification != WindowClass::LoadsaveOverwritePrompt)
        {
            return;
        }

        auto promptWindow = static_cast<OverwritePromptWindow*>(w);

        if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
        {
            promptWindow->OnMouseUp(WIDX_OVERWRITE_OVERWRITE);
        }
        else if (keycode == SDLK_ESCAPE)
        {
            promptWindow->OnMouseUp(WIDX_OVERWRITE_CANCEL);
        }
    }

#pragma endregion
} // namespace OpenRCT2::Ui::Windows
