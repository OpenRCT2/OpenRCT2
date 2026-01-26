/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/Diagnostic.h>
#include <openrct2/Editor.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/File.h>
#include <openrct2/core/FileScanner.h>
#include <openrct2/core/FileStream.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.Date.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/park/ParkPreview.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/rct2/T6Exporter.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <string>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    using namespace OpenRCT2::Drawing;
    using namespace OpenRCT2::Ui::FileBrowser;

#pragma region Widgets

    static constexpr ScreenSize kWindowSize = { 400, 350 };
    static constexpr ScreenSize kWindowSizeMin = { 300, kWindowSize.height / 2 };
    static constexpr ScreenSize kWindowSizeMax = kWindowSize * 3;

    static constexpr auto kPadding = 5;

    static constexpr auto kPreviewWidthScreenshot = 250;
    static constexpr auto kPreviewWidthMiniMap = 180;

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
        WIDX_PARENT_FOLDER,
        WIDX_NEW_FOLDER,
        WIDX_DEFAULT_FOLDER,
        WIDX_SYSTEM_BROWSER,
        WIDX_SORT_NAME,
        WIDX_SORT_SIZE,
        WIDX_SORT_DATE,
        WIDX_SORT_CUSTOMISE,
        WIDX_SCROLL,
        WIDX_FILENAME_TEXTBOX,
        WIDX_SAVE,
    };

    // clang-format off
    static constexpr auto window_loadsave_widgets = makeWidgets(
        makeWindowShim(kStringIdNone, kWindowSize),
        makeWidget({                               0,                      15 }, {       kWindowSize.width, kWindowSize.height - 15 }, WidgetType::resize,      WindowColour::secondary                                                                ), // WIDX_RESIZE
        makeWidget({     kWindowSize.width - 100 - 4,                      20 }, {                      20,                      20 }, WidgetType::flatBtn,     WindowColour::primary,   SPR_G2_FOLDER_PARENT,        STR_PARENT_FOLDER_TIP            ), // WIDX_PARENT_FOLDER
        makeWidget({     kWindowSize.width -  50 - 4,                      20 }, {                      20,                      20 }, WidgetType::flatBtn,     WindowColour::primary,   SPR_G2_FOLDER_NEW,           STR_FILEBROWSER_ACTION_NEW_FOLDER), // WIDX_NEW_FOLDER
        makeWidget({     kWindowSize.width -  75 - 4,                      20 }, {                      20,                      20 }, WidgetType::flatBtn,     WindowColour::primary,   SPR_G2_FOLDER_DEFAULT,       STR_LOADSAVE_DEFAULT_TIP         ), // WIDX_DEFAULT_FOLDER
        makeWidget({     kWindowSize.width -  25 - 4,                      20 }, {                      20,                      20 }, WidgetType::flatBtn,     WindowColour::primary,   SPR_G2_SYSTEM_BROWSER,       STR_FILEBROWSER_USE_SYSTEM_WINDOW), // WIDX_SYSTEM_BROWSER
        makeWidget({                               4,                      45 }, {                     160,                      14 }, WidgetType::tableHeader, WindowColour::primary                                                                  ), // WIDX_SORT_NAME
        makeWidget({ (kWindowSize.width - 5) / 3 + 1,                      45 }, {                     160,                      14 }, WidgetType::tableHeader, WindowColour::primary                                                                  ), // WIDX_SORT_SIZE
        makeWidget({ (kWindowSize.width - 5) / 3 + 1,                      45 }, {                     160,                      14 }, WidgetType::tableHeader, WindowColour::primary                                                                  ), // WIDX_SORT_DATE
        makeWidget({          kWindowSize.width - 19,                      45 }, {                      14,                      14 }, WidgetType::button,      WindowColour::primary,   STR_DROPDOWN_GLYPH                                            ), // WIDX_SORT_CUSTOMISE
        makeWidget({                               4,                      58 }, {                     342,                     303 }, WidgetType::scroll,      WindowColour::primary,   SCROLL_VERTICAL                                               ), // WIDX_SCROLL
        makeWidget({                              64, kWindowSize.height - 50 }, { kWindowSize.width - 133,                      14 }, WidgetType::textBox,     WindowColour::secondary                                                                ), // WIDX_FILENAME_TEXTBOX
        makeWidget({          kWindowSize.width - 65, kWindowSize.height - 50 }, {                      60,                      14 }, WidgetType::button,      WindowColour::secondary, STR_FILEBROWSER_SAVE_BUTTON                                   )  // WIDX_SAVE
    );
    // clang-format on

#pragma endregion

    class LoadSaveWindow final : public Window
    {
    public:
        LoadSaveWindow(LoadSaveAction _action, LoadSaveType _type)
            : action(_action)
            , type(_type)
        {
        }

    private:
        int32_t maxDateWidth{ 0 };
        int32_t maxTimeWidth{ 0 };
        LoadSaveAction action;
        LoadSaveType type;
        ParkPreview _preview;
        BackgroundWorker::Job _previewLoadJob;

        bool ShowPreviews()
        {
            auto& config = Config::Get().general;
            return config.fileBrowserPreviewType != ParkPreviewPref::disabled;
        }

        ScreenSize GetPreviewSize() const
        {
            auto& config = Config::Get().general;
            switch (config.fileBrowserPreviewType)
            {
                case ParkPreviewPref::screenshot:
                    return { kPreviewWidthScreenshot, kPreviewWidthScreenshot / 5 * 4 };
                case ParkPreviewPref::miniMap:
                    return { kPreviewWidthMiniMap, kPreviewWidthMiniMap };
                case ParkPreviewPref::disabled:
                default:
                    return { 0, 0 };
            }
        }

        ScreenSize GetMinimumWindowSize() const
        {
            return kWindowSizeMin + GetPreviewSize();
        }

        void PopulateList(const u8string& directory, std::string_view extensionPattern)
        {
            const auto absoluteDirectory = Path::GetAbsolute(directory);
            String::safeUtf8Copy(_directory, absoluteDirectory.c_str(), std::size(_directory));
            // Note: This compares the pointers, not values
            _extensionPattern = extensionPattern;

            _listItems.clear();

            int32_t drives = Platform::GetDrives();
            if (directory.empty() && drives)
            {
                // List Windows drives
                disabledWidgets |= (1uLL << WIDX_NEW_FOLDER) | (1uLL << WIDX_PARENT_FOLDER);
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
                    disabledWidgets |= (1uLL << WIDX_PARENT_FOLDER);
                else
                    disabledWidgets &= ~(1uLL << WIDX_PARENT_FOLDER);

                // Re-enable the "new" button if it was disabled
                disabledWidgets &= ~(1uLL << WIDX_NEW_FOLDER);

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
                for (const u8string_view extToken : String::split(extensionPattern, ";"))
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

            invalidate();
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
            maxDateWidth = GfxGetStringWidth(date.c_str(), FontStyle::medium) + kDateTimeGap;

            // Some locales do not use leading zeros for months and days, so let's try October, too.
            tm.tm_mon = 10;
            tm.tm_yday = 294;
            long_time = mktime(&tm);

            // Again, check how this date is represented (e.g. 2000-10-20, or 00/10/20)
            date = Platform::FormatShortDate(long_time);
            maxDateWidth = std::max(maxDateWidth, GfxGetStringWidth(date.c_str(), FontStyle::medium) + kDateTimeGap);

            // Time appears to be universally represented with two digits for minutes, so 12:00 or 00:00 should be
            // representable.
            std::string time = Platform::FormatTime(long_time);
            maxTimeWidth = GfxGetStringWidth(time.c_str(), FontStyle::medium) + kDateTimeGap;
        }

        void LoadPreview()
        {
            _preview = {};

            if (selectedListItem == -1)
                return;

            if (!ShowPreviews())
                return;

            if (type == LoadSaveType::track || type == LoadSaveType::heightmap)
                return;

            if (_listItems[selectedListItem].type == FileType::directory)
                return;

            auto path = _listItems[selectedListItem].path;

            auto& bgWorker = GetContext()->GetBackgroundWorker();

            if (_previewLoadJob.isValid())
            {
                _previewLoadJob.cancel();
            }

            _previewLoadJob = bgWorker.addJob(
                [path]() {
                    try
                    {
                        auto fs = FileStream(path, FileMode::open);

                        ClassifiedFileInfo info;
                        if (!TryClassifyFile(&fs, &info) || info.Type != ::FileType::park)
                            return ParkPreview{};

                        auto& objectRepository = GetContext()->GetObjectRepository();
                        auto parkImporter = ParkImporter::CreateParkFile(objectRepository);
                        parkImporter->LoadFromStream(&fs, false, true, path.c_str());
                        return parkImporter->GetParkPreview();
                    }
                    catch (const std::exception& e)
                    {
                        LOG_ERROR("Could not get preview:", e.what());
                        return ParkPreview{};
                    }
                },
                [](const ParkPreview preview) {
                    auto* windowMgr = GetContext()->GetUiContext().GetWindowManager();
                    auto* wnd = windowMgr->FindByClass(WindowClass::loadsave);
                    if (wnd == nullptr)
                    {
                        return;
                    }
                    auto* loadSaveWnd = static_cast<LoadSaveWindow*>(wnd);
                    loadSaveWnd->UpdateParkPreview(preview);
                });
        }

        void UpdateParkPreview(const ParkPreview& preview)
        {
            _preview = preview;
            if (ShowPreviews())
            {
                invalidate();
            }
        }

        void DrawPreview(RenderTarget& rt)
        {
            // Find preview image to draw
            PreviewImage* image = nullptr;
            auto targetPref = Config::Get().general.fileBrowserPreviewType;
            auto targetType = targetPref == ParkPreviewPref::screenshot ? PreviewImageType::screenshot
                                                                        : PreviewImageType::miniMap;
            for (auto& candidate : _preview.images)
            {
                if (candidate.type == targetType)
                {
                    image = &candidate;
                    break;
                }
            }

            const auto previewPaneSize = GetPreviewSize();
            auto& widget = widgets[WIDX_SCROLL];

            // Draw park name
            {
                auto namePos = windowPos
                    + ScreenCoordsXY(width - previewPaneSize.width / 2 - kPadding, widget.top - kButtonFaceHeight);
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(_preview.parkName.c_str());
                DrawTextEllipsised(
                    rt, namePos, previewPaneSize.width - kPadding * 2, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { TextAlignment::centre });
            }

            const bool drawFrame = image != nullptr || targetType == PreviewImageType::screenshot;
            const auto previewWidth = image != nullptr ? image->width : previewPaneSize.width;
            const auto previewHeight = image != nullptr ? image->height : previewPaneSize.height;

            auto hCentre = (previewPaneSize.width - previewWidth) / 2 - kPadding;
            auto frameStartPos = windowPos + ScreenCoordsXY(width - previewPaneSize.width + hCentre - 1, widget.top);
            auto frameEndPos = frameStartPos + ScreenCoordsXY(previewWidth + 1, previewHeight + 1);

            if (drawFrame)
            {
                Rectangle::fillInset(
                    rt, { frameStartPos, frameEndPos }, colours[1], Rectangle::BorderStyle::inset,
                    Rectangle::FillBrightness::dark, Rectangle::FillMode::dontLightenWhenInset);
            }

            // Draw image, or placeholder if no preview was found
            if (image != nullptr)
            {
                auto imagePos = frameStartPos + ScreenCoordsXY(1, 1);
                drawPreviewImage(*image, rt, imagePos);
            }
            else
            {
                auto imagePos = frameStartPos + ScreenCoordsXY(1, 1);

                if (targetType == PreviewImageType::screenshot)
                {
                    auto colour = getColourMap(colours[1].colour).dark;
                    GfxDrawSpriteSolid(rt, ImageId(SPR_G2_LOGO_MONO_DITHERED), imagePos, colour);
                }

                auto textPos = imagePos + ScreenCoordsXY(previewWidth / 2, previewHeight / 2 - 6);

                // NOTE: Can't simplify this as the compiler complains about different enumeration types.
                StringId previewText = STR_NO_PREVIEW_AVAILABLE;
                if (_previewLoadJob.isValid())
                {
                    previewText = STR_LOADING_GENERIC;
                }

                DrawTextBasic(
                    rt, textPos, previewText, {},
                    { ColourWithFlags{ Drawing::Colour::white }.withFlag(ColourFlag::withOutline, true),
                      TextAlignment::centre });
                return;
            }

            auto summaryCoords = windowPos
                + ScreenCoordsXY(width - previewPaneSize.width - kPadding, widget.top + previewHeight + kListRowHeight);

            // Date
            {
                auto ft = Formatter();
                ft.Add<StringId>(DateFormatStringFormatIds[Config::Get().general.dateFormat]);
                ft.Add<StringId>(DateDayNames[_preview.day]);
                ft.Add<int16_t>(_preview.month);
                ft.Add<int16_t>(_preview.year + 1);
                DrawTextBasic(rt, summaryCoords, STR_SUMMARY_DATE, ft);
                summaryCoords.y += kListRowHeight;
            }

            // Park Rating
            {
                auto ft = Formatter();
                ft.Add<money64>(_preview.parkRating);
                DrawTextBasic(rt, summaryCoords, STR_SUMMARY_PARK_RATING, ft);
                summaryCoords.y += kListRowHeight;
            }

            // Cash
            if (_preview.parkUsesMoney)
            {
                auto ft = Formatter();
                ft.Add<money64>(_preview.cash);
                DrawTextBasic(rt, summaryCoords, STR_SUMMARY_CASH, ft);
                summaryCoords.y += kListRowHeight;
            }

            // Num. Rides
            {
                auto ft = Formatter();
                ft.Add<money64>(_preview.numRides);
                DrawTextBasic(rt, summaryCoords, STR_SUMMARY_NUM_RIDES, ft);
                summaryCoords.y += kListRowHeight;
            }

            // Num. Guests
            {
                auto ft = Formatter();
                ft.Add<money64>(_preview.numGuests);
                DrawTextBasic(rt, summaryCoords, STR_SUMMARY_NUM_GUESTS, ft);
                summaryCoords.y += kListRowHeight;
            }
        }

        void SortList()
        {
            std::sort(_listItems.begin(), _listItems.end(), ListItemSort);
        }

#pragma region Events
    public:
        void onOpen() override
        {
            setWidgets(window_loadsave_widgets);

            const auto& uiContext = GetContext()->GetUiContext();
            if (!uiContext.HasFilePicker())
            {
                disabledWidgets |= (1uLL << WIDX_SYSTEM_BROWSER);
                widgets[WIDX_SYSTEM_BROWSER].type = WidgetType::empty;
            }

            const bool isSave = action == LoadSaveAction::save;

            // Pause the game if not on title scene, nor in network play.
            if (gLegacyScene != LegacyScene::titleSequence && Network::GetMode() == Network::Mode::none)
            {
                gGamePaused |= GAME_PAUSED_MODAL;
                Audio::StopAll();
            }

            if (isSave)
            {
                widgets[WIDX_FILENAME_TEXTBOX].type = WidgetType::textBox;
                widgets[WIDX_FILENAME_TEXTBOX].string = _currentFilename;
                widgets[WIDX_SAVE].type = WidgetType::button;

                // Set current filename
                String::set(_currentFilename, sizeof(_currentFilename), _defaultPath.c_str());

                // Focus textbox
                WindowStartTextbox(*this, WIDX_FILENAME_TEXTBOX, _currentFilename, sizeof(_currentFilename));
            }
            else
            {
                widgets[WIDX_FILENAME_TEXTBOX].type = WidgetType::empty;
                widgets[WIDX_SAVE].type = WidgetType::empty;
            }

            // Populate file list
            const char* pattern = GetFilterPatternByType(type, isSave);
            const auto path = GetDir(type);
            PopulateList(path, pattern);
            numListItems = static_cast<uint16_t>(_listItems.size());
            selectedListItem = -1;

            // Reset window dimensions
            initScrollWidgets();
            ComputeMaxDateWidth();

            WindowSetResize(*this, GetMinimumWindowSize(), kWindowSizeMax);
        }

        void onClose() override
        {
            _listItems.clear();

            auto* windowMgr = GetWindowManager();
            windowMgr->CloseByClass(WindowClass::loadsaveOverwritePrompt);

            Config::Save();

            // Unpause the game if not on title scene, nor in network play.
            if (gLegacyScene != LegacyScene::titleSequence && Network::GetMode() == Network::Mode::none)
            {
                gGamePaused &= ~GAME_PAUSED_MODAL;
                Audio::Resume();
            }
        }

        void onResize() override
        {
            WindowSetResize(*this, GetMinimumWindowSize(), kWindowSizeMax);

            auto& config = Config::Get().general;
            config.fileBrowserWidth = width;
            config.fileBrowserHeight = height - getTitleBarDiffNormal();
        }

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILENAME_TEXTBOX);
            }
        }

        void onPrepareDraw() override
        {
            auto toolbarXPos = width - 5;
            for (auto widgetIndex = 3; widgetIndex >= 0; widgetIndex--)
            {
                auto& widget = widgets[EnumValue(WIDX_PARENT_FOLDER) + widgetIndex];
                widget.right = toolbarXPos;
                widget.left = toolbarXPos - 20;
                toolbarXPos = widget.left - 1;
            }

            auto paddingBottom = ShowPreviews() ? kPadding : kPadding + 10;

            widgets[WIDX_SCROLL].right = width - kPadding;
            widgets[WIDX_SCROLL].bottom = height - paddingBottom;
            if (ShowPreviews())
                widgets[WIDX_SCROLL].right -= GetPreviewSize().width + kPadding;

            Widget& customiseWidget = widgets[WIDX_SORT_CUSTOMISE];
            customiseWidget.right = widgets[WIDX_SCROLL].right;
            customiseWidget.left = customiseWidget.right - 14;

            auto& config = Config::Get().general;
            if (config.fileBrowserShowDateColumn)
            {
                // Date column on the right
                Widget& dateWidget = widgets[WIDX_SORT_DATE];
                dateWidget.type = WidgetType::tableHeader;
                dateWidget.right = customiseWidget.left - 1;
                dateWidget.left = dateWidget.right - (maxDateWidth + maxTimeWidth + (4 * kDateTimeGap) + (kScrollBarWidth + 1));

                if (config.fileBrowserShowSizeColumn)
                {
                    // File size column in the middle
                    Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                    sizeWidget.type = WidgetType::tableHeader;
                    sizeWidget.right = dateWidget.left - 1;
                    sizeWidget.left = sizeWidget.right - 65;

                    // Name column is next to size column
                    widgets[WIDX_SORT_NAME].right = sizeWidget.left - 1;
                }
                else
                {
                    // Hide file size header
                    Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                    sizeWidget.type = WidgetType::empty;

                    // Name column is next to date column
                    widgets[WIDX_SORT_NAME].right = dateWidget.left - 1;
                }
            }
            else if (config.fileBrowserShowSizeColumn)
            {
                // Hide date header
                Widget& dateWidget = widgets[WIDX_SORT_DATE];
                dateWidget.type = WidgetType::empty;

                // File size column on the right
                Widget& sizeWidget = widgets[WIDX_SORT_SIZE];
                sizeWidget.type = WidgetType::tableHeader;
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
                widgets[WIDX_SORT_SIZE].type = WidgetType::empty;
                widgets[WIDX_SORT_DATE].type = WidgetType::empty;
            }

            if (action == LoadSaveAction::save)
            {
                widgets[WIDX_SCROLL].bottom -= 18;

                // Get 'Save' button string width
                auto saveLabel = LanguageGetString(STR_FILEBROWSER_SAVE_BUTTON);
                auto saveLabelWidth = GfxGetStringWidth(saveLabel, FontStyle::medium) + 12;

                widgets[WIDX_SAVE].type = WidgetType::button;
                widgets[WIDX_SAVE].top = height - paddingBottom - 15;
                widgets[WIDX_SAVE].bottom = height - paddingBottom - 3;
                widgets[WIDX_SAVE].right = widgets[WIDX_SCROLL].right;
                widgets[WIDX_SAVE].left = widgets[WIDX_SAVE].right - saveLabelWidth;

                // Get 'Filename:' string width
                auto filenameLabel = LanguageGetString(STR_FILENAME_LABEL);
                auto filenameLabelWidth = GfxGetStringWidth(filenameLabel, FontStyle::medium);

                widgets[WIDX_FILENAME_TEXTBOX].type = WidgetType::textBox;
                widgets[WIDX_FILENAME_TEXTBOX].top = height - paddingBottom - 15;
                widgets[WIDX_FILENAME_TEXTBOX].bottom = height - paddingBottom - 3;
                widgets[WIDX_FILENAME_TEXTBOX].left = 4 + filenameLabelWidth + 6;
                widgets[WIDX_FILENAME_TEXTBOX].right = widgets[WIDX_SAVE].left - 5;
            }
            else
            {
                widgets[WIDX_SAVE].type = WidgetType::empty;
                widgets[WIDX_FILENAME_TEXTBOX].type = WidgetType::empty;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            if (ShowPreviews())
                DrawPreview(rt);

            {
                const auto& widget = widgets[WIDX_PARENT_FOLDER];
                const auto pathWidth = widget.left - 8;
                const auto shortPath = ShortenPath(_directory, pathWidth, FontStyle::medium);

                // Format text
                std::string buffer;
                buffer.assign("{BLACK}");
                buffer += shortPath;

                // Draw path text
                const auto normalisedPath = Platform::StrDecompToPrecomp(buffer.data());
                const auto* normalisedPathC = normalisedPath.c_str();

                auto ft = Formatter();
                ft.Add<const char*>(normalisedPathC);

                auto pathPos = windowPos + ScreenCoordsXY{ 4, widget.top + 4 };
                DrawTextEllipsised(rt, pathPos, pathWidth, STR_STRING, ft);
            }

            const auto drawButtonCaption =
                [rt, this](Widget& widget, StringId strId, FileBrowserSort ascSort, FileBrowserSort descSort) {
                    StringId indicatorId = kStringIdNone;
                    if (Config::Get().general.loadSaveSort == ascSort)
                        indicatorId = STR_UP;
                    else if (Config::Get().general.loadSaveSort == descSort)
                        indicatorId = STR_DOWN;

                    auto ft = Formatter();
                    ft.Add<StringId>(indicatorId);

                    auto cRT = const_cast<const RenderTarget&>(rt);
                    DrawTextEllipsised(
                        cRT, windowPos + ScreenCoordsXY{ widget.left + 5, widget.top + 1 }, widget.width() - 1, strId, ft,
                        { Drawing::Colour::grey });
                };

            auto& config = Config::Get().general;
            drawButtonCaption(
                widgets[WIDX_SORT_NAME], STR_NAME_COLUMN, FileBrowserSort::NameAscending, FileBrowserSort::NameDescending);

            if (config.fileBrowserShowSizeColumn)
                drawButtonCaption(
                    widgets[WIDX_SORT_SIZE], STR_FILEBROWSER_SIZE_COLUMN, FileBrowserSort::SizeAscending,
                    FileBrowserSort::SizeDescending);

            if (config.fileBrowserShowDateColumn)
                drawButtonCaption(
                    widgets[WIDX_SORT_DATE], STR_DATE_COLUMN, FileBrowserSort::DateAscending, FileBrowserSort::DateDescending);

            // 'Filename:' label
            if (action == LoadSaveAction::save)
            {
                auto& widget = widgets[WIDX_FILENAME_TEXTBOX];
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 5, widget.top + 2 }, STR_FILENAME_LABEL, {}, { Drawing::Colour::grey });
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            bool isSave = action == LoadSaveAction::save;
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    InvokeCallback(ModalResult::cancel, "");
                    close();
                    break;

                case WIDX_PARENT_FOLDER:
                    PopulateList(_parentDirectory, _extensionPattern);
                    initScrollWidgets();
                    numListItems = static_cast<uint16_t>(_listItems.size());
                    break;

                case WIDX_NEW_FOLDER:
                    WindowTextInputRawOpen(
                        this, WIDX_NEW_FOLDER, STR_FILEBROWSER_ACTION_NEW_FOLDER, STR_FILEBROWSER_FOLDER_NAME_PROMPT, {}, "",
                        64);
                    break;

                case WIDX_SYSTEM_BROWSER:
                {
                    u8string path = OpenSystemFileBrowser(isSave, type, _directory, _defaultPath);
                    if (!path.empty())
                    {
                        Select(path.c_str(), action, type, _trackDesign);
                    }
                    else
                    {
                        // If user cancels file dialog, refresh list
                        PopulateList(_directory, _extensionPattern);
                        initScrollWidgets();
                        numListItems = static_cast<uint16_t>(_listItems.size());
                    }
                }
                break;

                case WIDX_SORT_NAME:
                    if (Config::Get().general.loadSaveSort == FileBrowserSort::NameAscending)
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::NameDescending;
                    }
                    else
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::NameAscending;
                    }
                    Config::Save();
                    SortList();
                    invalidate();
                    break;

                case WIDX_SORT_SIZE:
                    if (Config::Get().general.loadSaveSort == FileBrowserSort::SizeDescending)
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::SizeAscending;
                    }
                    else
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::SizeDescending;
                    }
                    Config::Save();
                    SortList();
                    invalidate();
                    break;

                case WIDX_SORT_DATE:
                    if (Config::Get().general.loadSaveSort == FileBrowserSort::DateDescending)
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::DateAscending;
                    }
                    else
                    {
                        Config::Get().general.loadSaveSort = FileBrowserSort::DateDescending;
                    }
                    Config::Save();
                    SortList();
                    invalidate();
                    break;

                case WIDX_DEFAULT_FOLDER:
                    PopulateList(GetInitialDirectoryByType(type).c_str(), _extensionPattern);
                    initScrollWidgets();
                    numListItems = static_cast<uint16_t>(_listItems.size());
                    break;

                case WIDX_FILENAME_TEXTBOX:
                    WindowStartTextbox(*this, widgetIndex, _currentFilename, sizeof(_currentFilename));
                    break;

                case WIDX_SAVE:
                {
                    const u8string path = Path::WithExtension(
                        Path::Combine(_directory, _currentFilename), RemovePatternWildcard(_extensionPattern));

                    if (File::Exists(path))
                        WindowOverwritePromptOpen(_currentFilename, path, action, type, _trackDesign);
                    else
                        Select(path.c_str(), action, type, _trackDesign);
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex != WIDX_SORT_CUSTOMISE)
                return;

            gDropdown.items[0] = Dropdown::ToggleOption(STR_FILEBROWSER_CUSTOMISE_FILENAME);
            gDropdown.items[1] = Dropdown::ToggleOption(STR_FILEBROWSER_CUSTOMISE_SIZE);
            gDropdown.items[2] = Dropdown::ToggleOption(STR_FILEBROWSER_CUSTOMISE_DATE);

            gDropdown.items[3] = Dropdown::Separator();

            gDropdown.items[4] = Dropdown::MenuLabel(STR_FILEBROWSER_PREVIEW_DISABLED);
            gDropdown.items[5] = Dropdown::MenuLabel(STR_FILEBROWSER_PREVIEW_MINIMAP);
            gDropdown.items[6] = Dropdown::MenuLabel(STR_FILEBROWSER_PREVIEW_SCREENSHOT);

            Widget* widget = &widgets[WIDX_SORT_CUSTOMISE];

            WindowDropdownShowTextCustomWidth(
                { windowPos.x + widget->left - 70, windowPos.y + widget->top }, widget->height(), colours[1], 0,
                Dropdown::Flag::StayOpen, 7, 90);

            auto& config = Config::Get().general;

            gDropdown.items[0].setChecked(true);
            gDropdown.items[1].setChecked(config.fileBrowserShowSizeColumn);
            gDropdown.items[2].setChecked(config.fileBrowserShowDateColumn);
            gDropdown.items[4].setChecked(config.fileBrowserPreviewType == ParkPreviewPref::disabled);
            gDropdown.items[5].setChecked(config.fileBrowserPreviewType == ParkPreviewPref::miniMap);
            gDropdown.items[6].setChecked(config.fileBrowserPreviewType == ParkPreviewPref::screenshot);
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (widgetIndex != WIDX_SORT_CUSTOMISE)
                return;

            auto& config = Config::Get().general;
            bool changed = false;
            if (selectedIndex == 1)
            {
                config.fileBrowserShowSizeColumn ^= true;
                changed = true;
            }
            else if (selectedIndex == 2)
            {
                config.fileBrowserShowDateColumn ^= true;
                changed = true;
            }
            else if (selectedIndex >= 4 && selectedIndex <= 6)
            {
                auto newPref = ParkPreviewPref(selectedIndex - 4);
                if (config.fileBrowserPreviewType != newPref)
                {
                    invalidate();
                    width -= GetPreviewSize().width;

                    config.fileBrowserPreviewType = newPref;
                    width += GetPreviewSize().width;

                    changed = true;
                }
            }

            if (changed)
            {
                Config::Save();
                invalidate();
                resizeFrame();
                onResize();
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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

                    numListItems = 0;
                    selectedListItem = -1;

                    PopulateList(path, _extensionPattern);
                    initScrollWidgets();

                    numListItems = static_cast<uint16_t>(_listItems.size());
                    invalidate();
                    break;
                }

                case WIDX_FILENAME_TEXTBOX:
                {
                    std::string tempText{ text };
                    const char* cStr = tempText.c_str();
                    if (strcmp(_currentFilename, cStr) == 0)
                        return;

                    String::safeUtf8Copy(_currentFilename, cStr, sizeof(_currentFilename));
                }
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, numListItems * kScrollableRowHeight };
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedItem = screenCoords.y / kScrollableRowHeight;
            if (selectedItem >= numListItems)
                return;

            if (selectedListItem != selectedItem)
            {
                selectedListItem = selectedItem;
                LoadPreview();
                invalidate();
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedItem;

            selectedItem = screenCoords.y / kScrollableRowHeight;
            if (selectedItem >= numListItems)
                return;

            if (_listItems[selectedItem].type == FileType::directory)
            {
                // The selected item is a folder
                numListItems = 0;
                selectedListItem = -1;

                char directory[MAX_PATH];
                String::safeUtf8Copy(directory, _listItems[selectedItem].path.c_str(), sizeof(directory));

                PopulateList(directory, _extensionPattern);
                initScrollWidgets();

                numListItems = static_cast<uint16_t>(_listItems.size());
            }
            else // FileType::file
            {
                // Load or overwrite
                String::set(_currentFilename, std::size(_currentFilename), _listItems[selectedItem].name.c_str());
                invalidateWidget(WIDX_FILENAME_TEXTBOX);

                if (action == LoadSaveAction::save)
                {
                    WindowOverwritePromptOpen(
                        _listItems[selectedItem].name, _listItems[selectedItem].path, action, type, _trackDesign);
                }
                else
                {
                    Select(_listItems[selectedItem].path.c_str(), action, type, _trackDesign);
                }
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            Rectangle::fill(
                rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);

            const int32_t listWidth = widgets[WIDX_SCROLL].width() - 1;
            const auto sizeColumnLeft = widgets[WIDX_SORT_SIZE].left;
            const auto dateColumnLeft = widgets[WIDX_SORT_DATE].left;
            const int32_t dateAnchor = dateColumnLeft + maxDateWidth + kDateTimeGap;

            auto& config = Config::Get().general;

            for (int32_t i = 0; i < numListItems; i++)
            {
                int32_t y = i * kScrollableRowHeight;
                if (y > rt.y + rt.height)
                    break;

                if (y + kScrollableRowHeight < rt.y)
                    continue;

                StringId stringId = STR_BLACK_STRING;

                // If hovering over item, change the color and fill the backdrop.
                if (i == selectedListItem)
                {
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    Rectangle::filter(rt, { 0, y, listWidth, y + kScrollableRowHeight }, FilterPaletteID::paletteDarken1);
                }
                // display a marker next to the currently loaded game file
                if (_listItems[i].loaded)
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_RIGHTGUILLEMET);
                    DrawTextBasic(rt, { 0, y }, stringId, ft);
                }

                // Folders get a folder icon
                if (_listItems[i].type == FileType::directory)
                {
                    GfxDrawSprite(rt, ImageId(SPR_G2_FOLDER), { 1, y });
                }

                // Print filename
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<char*>(_listItems[i].name.c_str());
                int32_t max_file_width = widgets[WIDX_SORT_NAME].width() - 16;
                DrawTextEllipsised(rt, { 15, y }, max_file_width, stringId, ft);

                // Print formatted modified date, if this is a file
                if (_listItems[i].type != FileType::file)
                    continue;

                if (config.fileBrowserShowSizeColumn)
                {
                    ft = Formatter();
                    ft.Add<StringId>(STR_FILEBROWSER_FILE_SIZE_VALUE);
                    ft.Add<uint32_t>(_listItems[i].fileSizeFormatted);
                    ft.Add<StringId>(_listItems[i].fileSizeUnit);
                    DrawTextEllipsised(rt, { sizeColumnLeft + 2, y }, maxDateWidth + maxTimeWidth, stringId, ft);
                }

                if (config.fileBrowserShowDateColumn)
                {
                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].dateFormatted.c_str());
                    DrawTextEllipsised(
                        rt, { dateAnchor - kDateTimeGap, y }, maxDateWidth, stringId, ft, { TextAlignment::right });

                    ft = Formatter();
                    ft.Add<StringId>(STR_STRING);
                    ft.Add<char*>(_listItems[i].timeFormatted.c_str());
                    DrawTextEllipsised(rt, { dateAnchor + kDateTimeGap, y }, maxTimeWidth, stringId, ft);
                }
            }
        }
#pragma endregion
    };

    WindowBase* LoadsaveOpen(
        LoadSaveAction action, LoadSaveType type, std::string_view defaultPath, LoadSaveCallback callback,
        TrackDesign* trackDesign)
    {
        _trackDesign = trackDesign;
        _defaultPath = defaultPath;

        RegisterCallback(callback);

        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<LoadSaveWindow*>(windowMgr->BringToFrontByClass(WindowClass::loadsave));
        if (w == nullptr)
        {
            auto& config = Config::Get().general;
            if (config.fileBrowserWidth < kWindowSizeMin.width || config.fileBrowserHeight < kWindowSizeMin.height
                || config.fileBrowserWidth > kWindowSizeMax.width || config.fileBrowserHeight > kWindowSizeMax.height)
            {
                config.fileBrowserWidth = kWindowSize.width;
                config.fileBrowserHeight = kWindowSize.height;
                Config::Save();
            }

            ScreenSize windowSize = { config.fileBrowserWidth, config.fileBrowserHeight };

            w = windowMgr->Create<LoadSaveWindow>(
                WindowClass::loadsave, windowSize,
                { WindowFlag::stickToFront, WindowFlag::resizable, WindowFlag::autoPosition, WindowFlag::centreScreen }, action,
                type);
        }

        bool isSave = action == LoadSaveAction::save;

        if (type == LoadSaveType::heightmap && isSave)
        {
            Guard::Fail("Cannot save images through loadsave window");
        }

        w->widgets[WIDX_TITLE].text = GetTitleStringId(type, isSave);
        if (w->widgets[WIDX_TITLE].text == kStringIdNone)
        {
            Guard::Fail("Unsupported load/save type: %d", EnumValue(type));
        }

        return w;
    }

    void WindowLoadSaveInputKey(WindowBase* w, uint32_t keycode)
    {
        if (w->classification != WindowClass::loadsave)
        {
            return;
        }

        auto loadSaveWindow = static_cast<LoadSaveWindow*>(w);

        if (keycode == SDLK_RETURN || keycode == SDLK_KP_ENTER)
        {
            loadSaveWindow->onMouseUp(WIDX_SAVE);
        }
        else if (keycode == SDLK_ESCAPE)
        {
            loadSaveWindow->close();
        }
    }
} // namespace OpenRCT2::Ui::Windows
