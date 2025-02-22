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
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/FileBrowser.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Editor.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
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
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <string>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    using namespace OpenRCT2::Ui::FileBrowser;

#pragma region Widgets

    static constexpr ScreenSize kWindowSizeInit = { 400, 350 };
    static constexpr ScreenSize kWindowSizeMin = kWindowSizeInit / 2;
    static constexpr ScreenSize kWindowSizeMax = kWindowSizeInit * 2;

    static constexpr int kKibiByte = 1024;
    static constexpr int kMebiByte = kKibiByte * 1024;

    static constexpr uint16_t kDateTimeGap = 2;

    static std::vector<LoadSaveListItem> _listItems;
    static char _directory[MAX_PATH];
    static char _parentDirectory[MAX_PATH];
    static char _currentFilename[MAX_PATH];
    static u8string _extensionPattern;
    static u8string _defaultPath;
    static TrackDesign* _trackDesign;

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
        WIDX_SORT_SIZE,
        WIDX_SORT_DATE,
        WIDX_SORT_CUSTOMISE,
        WIDX_SCROLL,
        WIDX_FILENAME_TEXTBOX,
        WIDX_SAVE,
        WIDX_BROWSE,
    };

    static constexpr int16_t WW = kWindowSizeInit.width;
    static constexpr int16_t WH = kWindowSizeInit.height;

    // clang-format off
    static constexpr Widget window_loadsave_widgets[] =
    {
        WINDOW_SHIM(kStringIdNone, WW, WH),
        MakeWidget({                0,      15 }, {       WW,  WH - 15 }, WindowWidgetType::Resize,      WindowColour::Secondary                                                             ), // WIDX_RESIZE
        MakeWidget({                4,      36 }, {       84,       14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_LOADSAVE_DEFAULT,              STR_LOADSAVE_DEFAULT_TIP), // WIDX_DEFAULT
        MakeWidget({               88,      36 }, {       84,       14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_UP                                  ), // WIDX_UP
        MakeWidget({              172,      36 }, {       87,       14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_NEW_FOLDER                          ), // WIDX_NEW_FOLDER
        MakeWidget({              259,      36 }, {       87,       14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_ACTION_NEW_FILE                            ), // WIDX_NEW_FILE
        MakeWidget({                4,      55 }, {      160,       14 }, WindowWidgetType::TableHeader, WindowColour::Primary                                                               ), // WIDX_SORT_NAME
        MakeWidget({ (WW - 5) / 3 + 1,      55 }, {      160,       14 }, WindowWidgetType::TableHeader, WindowColour::Primary                                                               ), // WIDX_SORT_SIZE
        MakeWidget({ (WW - 5) / 3 + 1,      55 }, {      160,       14 }, WindowWidgetType::TableHeader, WindowColour::Primary                                                               ), // WIDX_SORT_DATE
        MakeWidget({        (WW - 19),      55 }, {       14,       14 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_DROPDOWN_GLYPH                                         ), // WIDX_SORT_CUSTOMISE
        MakeWidget({                4,      68 }, {      342,      303 }, WindowWidgetType::Scroll,      WindowColour::Primary,   SCROLL_VERTICAL                                            ), // WIDX_SCROLL
        MakeWidget({               64, WH - 50 }, { WW - 133,       14 }, WindowWidgetType::TextBox,     WindowColour::Secondary                                                             ), // WIDX_FILENAME_TEXTBOX
        MakeWidget({          WW - 65, WH - 50 }, {       60,       14 }, WindowWidgetType::Button,      WindowColour::Secondary, STR_FILEBROWSER_SAVE_BUTTON                                ), // WIDX_SAVE
        MakeWidget({                4, WH - 24 }, {      197,       19 }, WindowWidgetType::Button,      WindowColour::Primary,   STR_FILEBROWSER_USE_SYSTEM_WINDOW                          ), // WIDX_BROWSE
    };
    // clang-format on

#pragma endregion

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
            widgets[WIDX_NEW_FILE].type = includeNewItem ? WindowWidgetType::Button : WindowWidgetType::Empty;
            widgets[WIDX_NEW_FOLDER].type = includeNewItem ? WindowWidgetType::Button : WindowWidgetType::Empty;

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
                        newListItem.dateModified = Platform::FileGetModifiedTime(newListItem.path.c_str());

                        // Cache a human-readable version of the modified date.
                        newListItem.dateFormatted = Platform::FormatShortDate(newListItem.dateModified);
                        newListItem.timeFormatted = Platform::FormatTime(newListItem.dateModified);

                        // File size
                        newListItem.fileSizeBytes = Platform::GetFileSize(newListItem.path.c_str());
                        if (newListItem.fileSizeBytes > kMebiByte)
                        {
                            newListItem.fileSizeFormatted = newListItem.fileSizeBytes / kMebiByte;
                            newListItem.fileSizeUnit = STR_SIZE_MEGABYTE;
                        }
                        else if (newListItem.fileSizeBytes > kKibiByte)
                        {
                            newListItem.fileSizeFormatted = newListItem.fileSizeBytes / kKibiByte;
                            newListItem.fileSizeUnit = STR_SIZE_KILOBYTE;
                        }
                        else
                        {
                            newListItem.fileSizeFormatted = newListItem.fileSizeBytes;
                            newListItem.fileSizeUnit = STR_SIZE_BYTE;
                        }

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
            maxDateWidth = GfxGetStringWidth(date.c_str(), FontStyle::Medium) + kDateTimeGap;

            // Some locales do not use leading zeros for months and days, so let's try October, too.
            tm.tm_mon = 10;
            tm.tm_yday = 294;
            long_time = mktime(&tm);

            // Again, check how this date is represented (e.g. 2000-10-20, or 00/10/20)
            date = Platform::FormatShortDate(long_time);
            maxDateWidth = std::max(maxDateWidth, GfxGetStringWidth(date.c_str(), FontStyle::Medium) + kDateTimeGap);

            // Time appears to be universally represented with two digits for minutes, so 12:00 or 00:00 should be
            // representable.
            std::string time = Platform::FormatTime(long_time);
            maxTimeWidth = GfxGetStringWidth(time.c_str(), FontStyle::Medium) + kDateTimeGap;
        }

        void SortList()
        {
            std::sort(_listItems.begin(), _listItems.end(), ListItemSort);
        }

#pragma region Events
    public:
        void OnOpen() override
        {
            SetWidgets(window_loadsave_widgets);

            const auto uiContext = OpenRCT2::GetContext()->GetUiContext();
            if (!uiContext->HasFilePicker())
            {
                disabled_widgets |= (1uLL << WIDX_BROWSE);
                widgets[WIDX_BROWSE].type = WindowWidgetType::Empty;
            }

            // TODO: Split LOADSAVETYPE_* into two proper enum classes (one for load/save, the other for the type)
            const bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;

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
            const auto path = GetDir(type);
            PopulateList(isSave, path, pattern);
            no_list_items = static_cast<uint16_t>(_listItems.size());
            selected_list_item = -1;

            // Reset window dimensions
            InitScrollWidgets();
            ComputeMaxDateWidth();

            min_width = kWindowSizeMin.width;
            min_height = kWindowSizeMin.height;
            max_width = kWindowSizeMax.width;
            max_height = kWindowSizeMax.height;
        }

        void OnClose() override
        {
            _listItems.clear();

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseByClass(WindowClass::LoadsaveOverwritePrompt);

            Config::Save();

            // Unpause the game if not on title scene, nor in network play.
            if (!(gScreenFlags & SCREEN_FLAGS_TITLE_DEMO) && NetworkGetMode() == NETWORK_MODE_NONE)
            {
                gGamePaused &= ~GAME_PAUSED_MODAL;
                Audio::Resume();
            }
        }

        void OnResize() override
        {
            WindowSetResize(*this, kWindowSizeMin.width, kWindowSizeMin.height, kWindowSizeMax.width, kWindowSizeMax.height);

            auto& config = Config::Get().general;
            config.FileBrowserWidth = width;
            config.FileBrowserHeight = height;
        }

        void OnUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_FILENAME_TEXTBOX);
            }
        }

        void OnPrepareDraw() override
        {
            ResizeFrameWithPage();

            Widget& customiseWidget = widgets[WIDX_SORT_CUSTOMISE];
            customiseWidget.right = width - 5;
            customiseWidget.left = customiseWidget.right - 14;

            auto& config = Config::Get().general;
            if (config.FileBrowserShowDateColumn)
            {
                // Date column on the right
                Widget& dateWidget = widgets[WIDX_SORT_DATE];
                dateWidget.type = WindowWidgetType::TableHeader;
                dateWidget.right = customiseWidget.left - 1;
                dateWidget.left = dateWidget.right - (maxDateWidth + maxTimeWidth + (4 * kDateTimeGap) + (kScrollBarWidth + 1));

                if (config.FileBrowserShowSizeColumn)
                {
                    // File size column in the middle
                    Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                    sizeWidget.type = WindowWidgetType::TableHeader;
                    sizeWidget.right = dateWidget.left - 1;
                    sizeWidget.left = sizeWidget.right - 65;

                    // Name column is next to size column
                    widgets[WIDX_SORT_NAME].right = sizeWidget.left - 1;
                }
                else
                {
                    // Hide file size header
                    Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                    sizeWidget.type = WindowWidgetType::Empty;

                    // Name column is next to date column
                    widgets[WIDX_SORT_NAME].right = dateWidget.left - 1;
                }
            }
            else if (config.FileBrowserShowSizeColumn)
            {
                // Hide date header
                Widget& dateWidget = widgets[WIDX_SORT_DATE];
                dateWidget.type = WindowWidgetType::Empty;

                // File size column on the right
                Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                sizeWidget.type = WindowWidgetType::TableHeader;
                sizeWidget.right = customiseWidget.left - 1;
                sizeWidget.left = sizeWidget.right - 65;

                // Name column is next to size column
                widgets[WIDX_SORT_NAME].right = sizeWidget.left - 1;
            }
            else
            {
                // Name is the only column
                widgets[WIDX_SORT_NAME].right = customiseWidget.left - 1;

                // Hide other columns
                widgets[WIDX_SORT_SIZE].type = WindowWidgetType::Empty;
                widgets[WIDX_SORT_DATE].type = WindowWidgetType::Empty;
            }

            widgets[WIDX_SCROLL].right = width - 5;
            widgets[WIDX_SCROLL].bottom = height - 30;

            if (type & LOADSAVETYPE_SAVE)
            {
                widgets[WIDX_SCROLL].bottom -= 18;

                // Get 'Save' button string width
                auto saveLabel = LanguageGetString(STR_FILEBROWSER_SAVE_BUTTON);
                auto saveLabelWidth = GfxGetStringWidth(saveLabel, FontStyle::Medium) + 16;

                widgets[WIDX_SAVE].type = WindowWidgetType::Button;
                widgets[WIDX_SAVE].top = height - 42;
                widgets[WIDX_SAVE].bottom = height - 30;
                widgets[WIDX_SAVE].left = width - saveLabelWidth - 5;
                widgets[WIDX_SAVE].right = width - 5;

                // Get 'Filename:' string width
                auto filenameLabel = LanguageGetString(STR_FILENAME_LABEL);
                auto filenameLabelWidth = GfxGetStringWidth(filenameLabel, FontStyle::Medium);

                widgets[WIDX_FILENAME_TEXTBOX].type = WindowWidgetType::TextBox;
                widgets[WIDX_FILENAME_TEXTBOX].top = height - 42;
                widgets[WIDX_FILENAME_TEXTBOX].bottom = height - 30;
                widgets[WIDX_FILENAME_TEXTBOX].left = 4 + filenameLabelWidth + 6;
                widgets[WIDX_FILENAME_TEXTBOX].right = widgets[WIDX_SAVE].left - 5;
            }
            else
            {
                widgets[WIDX_SAVE].type = WindowWidgetType::Empty;
                widgets[WIDX_FILENAME_TEXTBOX].type = WindowWidgetType::Empty;
            }

            // 'Use system file browser'
            widgets[WIDX_BROWSE].top = height - 24;
            widgets[WIDX_BROWSE].bottom = height - 6;
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            {
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
            }

            const auto drawButtonCaption = [dpi, this](
                                               Widget& widget, StringId strId, FileBrowserSort ascSort,
                                               FileBrowserSort descSort) {
                StringId indicatorId = kStringIdNone;
                if (Config::Get().general.LoadSaveSort == ascSort)
                    indicatorId = STR_UP;
                else if (Config::Get().general.LoadSaveSort == descSort)
                    indicatorId = STR_DOWN;

                auto ft = Formatter();
                ft.Add<StringId>(indicatorId);

                auto cdpi = const_cast<const DrawPixelInfo&>(dpi);
                DrawTextBasic(cdpi, windowPos + ScreenCoordsXY{ widget.left + 5, widget.top + 1 }, strId, ft, { COLOUR_GREY });
            };

            auto& config = Config::Get().general;
            drawButtonCaption(
                widgets[WIDX_SORT_NAME], STR_NAME_COLUMN, FileBrowserSort::NameAscending, FileBrowserSort::NameDescending);

            if (config.FileBrowserShowSizeColumn)
                drawButtonCaption(
                    widgets[WIDX_SORT_SIZE], STR_FILEBROWSER_SIZE_COLUMN, FileBrowserSort::SizeAscending,
                    FileBrowserSort::SizeDescending);

            if (config.FileBrowserShowDateColumn)
                drawButtonCaption(
                    widgets[WIDX_SORT_DATE], STR_DATE_COLUMN, FileBrowserSort::DateAscending, FileBrowserSort::DateDescending);

            // 'Filename:' label
            if (type & LOADSAVETYPE_SAVE)
            {
                auto& widget = widgets[WIDX_FILENAME_TEXTBOX];
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ 5, widget.top + 2 }, STR_FILENAME_LABEL, {}, { COLOUR_GREY });
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;
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
                        this, WIDX_NEW_FILE, kStringIdNone, STR_FILEBROWSER_FILE_NAME_PROMPT, {}, STR_STRING,
                        reinterpret_cast<uintptr_t>(_defaultPath.c_str()), 64);
                    break;

                case WIDX_NEW_FOLDER:
                    WindowTextInputRawOpen(
                        this, WIDX_NEW_FOLDER, kStringIdNone, STR_FILEBROWSER_FOLDER_NAME_PROMPT, {}, "", 64);
                    break;

                case WIDX_BROWSE:
                {
                    u8string path = OpenSystemFileBrowser(isSave, type, _directory, _defaultPath);
                    if (!path.empty())
                    {
                        Select(path.c_str(), type, _trackDesign);
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
                    if (Config::Get().general.LoadSaveSort == FileBrowserSort::NameAscending)
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::NameDescending;
                    }
                    else
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::NameAscending;
                    }
                    Config::Save();
                    SortList();
                    Invalidate();
                    break;

                case WIDX_SORT_SIZE:
                    if (Config::Get().general.LoadSaveSort == FileBrowserSort::SizeDescending)
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::SizeAscending;
                    }
                    else
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::SizeDescending;
                    }
                    Config::Save();
                    SortList();
                    Invalidate();
                    break;

                case WIDX_SORT_DATE:
                    if (Config::Get().general.LoadSaveSort == FileBrowserSort::DateDescending)
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::DateAscending;
                    }
                    else
                    {
                        Config::Get().general.LoadSaveSort = FileBrowserSort::DateDescending;
                    }
                    Config::Save();
                    SortList();
                    Invalidate();
                    break;

                case WIDX_DEFAULT:
                    PopulateList(isSave, GetInitialDirectoryByType(type).c_str(), _extensionPattern);
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
                        WindowOverwritePromptOpen(_currentFilename, path, type, _trackDesign);
                    else
                        Select(path.c_str(), type, _trackDesign);
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex != WIDX_SORT_CUSTOMISE)
                return;

            gDropdownItems[0].Format = STR_TOGGLE_OPTION;
            gDropdownItems[1].Format = STR_TOGGLE_OPTION;
            gDropdownItems[2].Format = STR_TOGGLE_OPTION;
            gDropdownItems[0].Args = STR_FILEBROWSER_CUSTOMISE_FILENAME;
            gDropdownItems[1].Args = STR_FILEBROWSER_CUSTOMISE_SIZE;
            gDropdownItems[2].Args = STR_FILEBROWSER_CUSTOMISE_DATE;

            Widget* widget = &widgets[WIDX_SORT_CUSTOMISE];

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left - 70, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0,
                Dropdown::Flag::StayOpen, 3, 90);

            auto& config = Config::Get().general;

            Dropdown::SetChecked(0, true);
            Dropdown::SetChecked(1, config.FileBrowserShowSizeColumn);
            Dropdown::SetChecked(2, config.FileBrowserShowDateColumn);
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (widgetIndex != WIDX_SORT_CUSTOMISE)
                return;

            auto& config = Config::Get().general;
            bool changed = false;
            if (selectedIndex == 1)
            {
                config.FileBrowserShowSizeColumn ^= true;
                changed = true;
            }
            else if (selectedIndex == 2)
            {
                config.FileBrowserShowDateColumn ^= true;
                changed = true;
            }

            if (changed)
            {
                Config::Save();
                Invalidate();
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            if (!Platform::IsFilenameValid(text))
            {
                ContextShowError(STR_ERROR_INVALID_CHARACTERS, kStringIdNone, {});
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_NEW_FOLDER:
                {
                    const u8string path = Path::Combine(_directory, text);
                    if (!Path::CreateDirectory(path))
                    {
                        ContextShowError(STR_UNABLE_TO_CREATE_FOLDER, kStringIdNone, {});
                        return;
                    }

                    no_list_items = 0;
                    selected_list_item = -1;

                    PopulateList((type & 1) == LOADSAVETYPE_SAVE, path, _extensionPattern);
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
                        WindowOverwritePromptOpen(text, path, type, _trackDesign);
                    else
                        Select(path.c_str(), type, _trackDesign);
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
                bool includeNewItem = (type & 1) == LOADSAVETYPE_SAVE;

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
                InvalidateWidget(WIDX_FILENAME_TEXTBOX);

                if ((type & 0x01) == LOADSAVETYPE_SAVE)
                    WindowOverwritePromptOpen(_listItems[selectedItem].name, _listItems[selectedItem].path, type, _trackDesign);
                else
                    Select(_listItems[selectedItem].path.c_str(), type, _trackDesign);
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            GfxFillRect(
                dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } },
                ColourMapA[colours[1].colour].mid_light);

            const int32_t listWidth = widgets[WIDX_SCROLL].width();
            const auto sizeColumnLeft = widgets[WIDX_SORT_SIZE].left;
            const auto dateColumnLeft = widgets[WIDX_SORT_DATE].left;
            const int32_t dateAnchor = dateColumnLeft + maxDateWidth + kDateTimeGap;

            auto& config = Config::Get().general;

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
                if (_listItems[i].type != FileType::file)
                    continue;

                if (config.FileBrowserShowSizeColumn)
                {
                    ft = Formatter();
                    ft.Add<StringId>(STR_FILEBROWSER_FILE_SIZE_VALUE);
                    ft.Add<uint32_t>(_listItems[i].fileSizeFormatted);
                    ft.Add<StringId>(_listItems[i].fileSizeUnit);
                    DrawTextEllipsised(dpi, { sizeColumnLeft + 2, y }, maxDateWidth + maxTimeWidth, stringId, ft);
                }

                if (config.FileBrowserShowDateColumn)
                {
                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].dateFormatted.c_str());
                    DrawTextEllipsised(
                        dpi, { dateAnchor - kDateTimeGap, y }, maxDateWidth, stringId, ft, { TextAlignment::RIGHT });

                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].timeFormatted.c_str());
                    DrawTextEllipsised(dpi, { dateAnchor + kDateTimeGap, y }, maxTimeWidth, stringId, ft);
                }
            }
        }
#pragma endregion
    };

    WindowBase* LoadsaveOpen(int32_t type, std::string_view defaultPath, LoadSaveCallback callback, TrackDesign* trackDesign)
    {
        _trackDesign = trackDesign;
        _defaultPath = defaultPath;

        RegisterCallback(callback);

        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<LoadSaveWindow*>(windowMgr->BringToFrontByClass(WindowClass::Loadsave));
        if (w == nullptr)
        {
            auto& config = Config::Get().general;
            if (config.FileBrowserWidth < kWindowSizeMin.width || config.FileBrowserHeight < kWindowSizeMin.height
                || config.FileBrowserWidth > kWindowSizeMax.width || config.FileBrowserHeight > kWindowSizeMax.height)
            {
                config.FileBrowserWidth = kWindowSizeInit.width;
                config.FileBrowserHeight = kWindowSizeInit.height;
                Config::Save();
            }

            auto width = config.FileBrowserWidth;
            auto height = config.FileBrowserHeight;

            w = windowMgr->Create<LoadSaveWindow>(
                WindowClass::Loadsave, width, height, WF_STICK_TO_FRONT | WF_RESIZABLE | WF_AUTO_POSITION | WF_CENTRE_SCREEN,
                type);
        }

        bool isSave = (type & 0x01) == LOADSAVETYPE_SAVE;

        if ((type & 0x0E) == LOADSAVETYPE_HEIGHTMAP && !isSave)
        {
            Guard::Fail("Cannot save images through loadsave window");
        }

        w->widgets[WIDX_TITLE].text = GetTitleStringId(type, isSave);
        if (w->widgets[WIDX_TITLE].text == kStringIdNone)
        {
            Guard::Fail("Unsupported load/save type: %d", type & 0x0F);
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
} // namespace OpenRCT2::Ui::Windows
