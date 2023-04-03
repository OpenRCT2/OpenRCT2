/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <mutex>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/Http.h>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/windows/Intent.h>
#include <sstream>
#include <string>
#include <vector>

#ifndef DISABLE_HTTP

class ObjectDownloader
{
private:
    static constexpr auto OPENRCT2_API_LEGACY_OBJECT_URL = "https://api.openrct2.io/objects/legacy/";

    struct DownloadStatusInfo
    {
        std::string Name;
        std::string Source;
        size_t Count{};
        size_t Total{};

        bool operator==(const DownloadStatusInfo& rhs)
        {
            return Name == rhs.Name && Source == rhs.Source && Count == rhs.Count && Total == rhs.Total;
        }
        bool operator!=(const DownloadStatusInfo& rhs)
        {
            return !(*this == rhs);
        }
    };

    std::vector<ObjectEntryDescriptor> _entries;
    std::vector<ObjectEntryDescriptor> _downloadedEntries;
    size_t _currentDownloadIndex{};
    std::mutex _downloadedEntriesMutex;
    std::mutex _queueMutex;
    bool _nextDownloadQueued{};

    DownloadStatusInfo _lastDownloadStatusInfo;
    DownloadStatusInfo _downloadStatusInfo;
    std::mutex _downloadStatusInfoMutex;
    std::string _lastDownloadSource;

    // TODO static due to INTENT_EXTRA_CALLBACK not allowing a std::function
    inline static bool _downloadingObjects;

public:
    void Begin(const std::vector<ObjectEntryDescriptor>& entries)
    {
        _lastDownloadStatusInfo = {};
        _downloadStatusInfo = {};
        _lastDownloadSource = {};
        _entries = entries;
        _currentDownloadIndex = 0;
        _downloadingObjects = true;
        QueueNextDownload();
    }

    bool IsDownloading() const
    {
        return _downloadingObjects;
    }

    std::vector<ObjectEntryDescriptor> GetDownloadedEntries()
    {
        std::lock_guard<std::mutex> guard(_downloadedEntriesMutex);
        return _downloadedEntries;
    }

    void Update()
    {
        std::lock_guard guard(_queueMutex);
        if (_nextDownloadQueued)
        {
            _nextDownloadQueued = false;
            NextDownload();
        }
        UpdateStatusBox();
    }

private:
    void UpdateStatusBox()
    {
        std::lock_guard<std::mutex> guard(_downloadStatusInfoMutex);
        if (_lastDownloadStatusInfo != _downloadStatusInfo)
        {
            _lastDownloadStatusInfo = _downloadStatusInfo;

            if (_downloadStatusInfo == DownloadStatusInfo())
            {
                ContextForceCloseWindowByClass(WindowClass::NetworkStatus);
            }
            else
            {
                char str_downloading_objects[256]{};
                Formatter ft;
                if (_downloadStatusInfo.Source.empty())
                {
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Count));
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Total));
                    ft.Add<char*>(_downloadStatusInfo.Name.c_str());
                    OpenRCT2::FormatStringLegacy(
                        str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS, ft.Data());
                }
                else
                {
                    ft.Add<char*>(_downloadStatusInfo.Name.c_str());
                    ft.Add<char*>(_downloadStatusInfo.Source.c_str());
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Count));
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Total));
                    OpenRCT2::FormatStringLegacy(
                        str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS_FROM, ft.Data());
                }

                auto intent = Intent(WindowClass::NetworkStatus);
                intent.PutExtra(INTENT_EXTRA_MESSAGE, std::string(str_downloading_objects));
                intent.PutExtra(INTENT_EXTRA_CALLBACK, []() -> void { _downloadingObjects = false; });
                ContextOpenIntent(&intent);
            }
        }
    }

    void UpdateProgress(const DownloadStatusInfo& info)
    {
        std::lock_guard<std::mutex> guard(_downloadStatusInfoMutex);
        _downloadStatusInfo = info;
    }

    void QueueNextDownload()
    {
        std::lock_guard guard(_queueMutex);
        _nextDownloadQueued = true;
    }

    void DownloadObject(const ObjectEntryDescriptor& entry, const std::string& name, const std::string& url)
    {
        try
        {
            Console::WriteLine("Downloading %s", url.c_str());
            Http::Request req;
            req.method = Http::Method::GET;
            req.url = url;
            Http::DoAsync(req, [this, entry, name](Http::Response response) {
                if (response.status == Http::Status::Ok)
                {
                    // Check that download operation hasn't been cancelled
                    if (_downloadingObjects)
                    {
                        auto data = reinterpret_cast<uint8_t*>(response.body.data());
                        auto dataLen = response.body.size();

                        auto& objRepo = OpenRCT2::GetContext()->GetObjectRepository();
                        objRepo.AddObjectFromFile(ObjectGeneration::DAT, name, data, dataLen);

                        std::lock_guard<std::mutex> guard(_downloadedEntriesMutex);
                        _downloadedEntries.push_back(entry);
                    }
                }
                else
                {
                    Console::Error::WriteLine("  Failed to download %s", name.c_str());
                }
                QueueNextDownload();
            });
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("  Failed to download %s", name.c_str());
            QueueNextDownload();
        }
    }

    void NextDownload()
    {
        if (!_downloadingObjects || _currentDownloadIndex >= _entries.size())
        {
            // Finished...
            _downloadingObjects = false;
            UpdateProgress({});
            return;
        }

        auto& entry = _entries[_currentDownloadIndex];
        auto name = String::Trim(std::string(entry.GetName()));
        LOG_VERBOSE("Downloading object: [%s]:", name.c_str());
        _currentDownloadIndex++;
        UpdateProgress({ name, _lastDownloadSource, _currentDownloadIndex, _entries.size() });
        try
        {
            Http::Request req;
            req.method = Http::Method::GET;
            req.url = OPENRCT2_API_LEGACY_OBJECT_URL + name;
            Http::DoAsync(req, [this, entry, name](Http::Response response) {
                if (response.status == Http::Status::Ok)
                {
                    auto jresponse = Json::FromString(response.body);
                    if (jresponse.is_object())
                    {
                        auto objName = Json::GetString(jresponse["name"]);
                        auto source = Json::GetString(jresponse["source"]);
                        auto downloadLink = Json::GetString(jresponse["download"]);
                        if (!downloadLink.empty())
                        {
                            _lastDownloadSource = source;
                            UpdateProgress({ name, source, _currentDownloadIndex, _entries.size() });
                            DownloadObject(entry, objName, downloadLink);
                        }
                    }
                }
                else if (response.status == Http::Status::NotFound)
                {
                    Console::Error::WriteLine("  %s not found", name.c_str());
                    QueueNextDownload();
                }
                else
                {
                    Console::Error::WriteLine(
                        "  %s query failed (status %d)", name.c_str(), static_cast<int32_t>(response.status));
                    QueueNextDownload();
                }
            });
        }
        catch (const std::exception&)
        {
            Console::Error::WriteLine("  Failed to query %s", name.c_str());
        }
    }
};

#endif

// clang-format off
enum WindowObjectLoadErrorWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_COLUMN_OBJECT_NAME,
    WIDX_COLUMN_OBJECT_SOURCE,
    WIDX_COLUMN_OBJECT_TYPE,
    WIDX_SCROLL,
    WIDX_COPY_CURRENT,
    WIDX_COPY_ALL,
    WIDX_DOWNLOAD_ALL
};

static constexpr const StringId WINDOW_TITLE = STR_OBJECT_LOAD_ERROR_TITLE;
static constexpr const int32_t WW = 450;
static constexpr const int32_t WH = 400;
static constexpr const int32_t WW_LESS_PADDING = WW - 5;
constexpr int32_t NAME_COL_LEFT = 4;
constexpr int32_t SOURCE_COL_LEFT = (WW_LESS_PADDING / 4) + 1;
constexpr int32_t TYPE_COL_LEFT = 5 * WW_LESS_PADDING / 8 + 1;

static Widget window_object_load_error_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  NAME_COL_LEFT,  57}, {108,  14}, WindowWidgetType::TableHeader, WindowColour::Primary, STR_OBJECT_NAME                                   ), // 'Object name' header
    MakeWidget({SOURCE_COL_LEFT,  57}, {166,  14}, WindowWidgetType::TableHeader, WindowColour::Primary, STR_OBJECT_SOURCE                                 ), // 'Object source' header
    MakeWidget({  TYPE_COL_LEFT,  57}, {166,  14}, WindowWidgetType::TableHeader, WindowColour::Primary, STR_OBJECT_TYPE                                   ), // 'Object type' header
    MakeWidget({  NAME_COL_LEFT,  70}, {442, 298}, WindowWidgetType::Scroll,       WindowColour::Primary, SCROLL_VERTICAL                                   ), // Scrollable list area
    MakeWidget({  NAME_COL_LEFT, 377}, {145,  14}, WindowWidgetType::Button,       WindowColour::Primary, STR_COPY_SELECTED,           STR_COPY_SELECTED_TIP), // Copy selected button
    MakeWidget({            152, 377}, {145,  14}, WindowWidgetType::Button,       WindowColour::Primary, STR_COPY_ALL,                STR_COPY_ALL_TIP     ), // Copy all button
#ifndef DISABLE_HTTP
    MakeWidget({            300, 377}, {146,  14}, WindowWidgetType::Button,       WindowColour::Primary, STR_DOWNLOAD_ALL,            STR_DOWNLOAD_ALL_TIP ), // Download all button
#endif
    WIDGETS_END,
};
// clang-format on

/**
 *  Returns an StringId that represents an RCTObjectEntry's type.
 *
 *  Could possibly be moved out of the window file if other
 *  uses exist and a suitable location is found.
 */
static constexpr StringId GetStringFromObjectType(const ObjectType type)
{
    switch (type)
    {
        case ObjectType::Ride:
            return STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS;
        case ObjectType::SmallScenery:
            return STR_OBJECT_SELECTION_SMALL_SCENERY;
        case ObjectType::LargeScenery:
            return STR_OBJECT_SELECTION_LARGE_SCENERY;
        case ObjectType::Walls:
            return STR_OBJECT_SELECTION_WALLS_FENCES;
        case ObjectType::Banners:
            return STR_OBJECT_SELECTION_PATH_SIGNS;
        case ObjectType::Paths:
            return STR_OBJECT_SELECTION_FOOTPATHS;
        case ObjectType::PathBits:
            return STR_OBJECT_SELECTION_PATH_EXTRAS;
        case ObjectType::SceneryGroup:
            return STR_OBJECT_SELECTION_SCENERY_GROUPS;
        case ObjectType::ParkEntrance:
            return STR_OBJECT_SELECTION_PARK_ENTRANCE;
        case ObjectType::Water:
            return STR_OBJECT_SELECTION_WATER;
        default:
            return STR_UNKNOWN_OBJECT_TYPE;
    }
}

class ObjectLoadErrorWindow final : public WindowBase
{
private:
    std::vector<ObjectEntryDescriptor> _invalidEntries;
    int32_t _highlightedIndex = -1;
    std::string _filePath;
#ifndef DISABLE_HTTP
    ObjectDownloader _objDownloader;
    bool _updatedListAfterDownload{};

    void DownloadAllObjects()
    {
        if (!_objDownloader.IsDownloading())
        {
            _updatedListAfterDownload = false;
            _objDownloader.Begin(_invalidEntries);
        }
    }

    void UpdateObjectList()
    {
        const auto entries = _objDownloader.GetDownloadedEntries();
        for (auto& de : entries)
        {
            _invalidEntries.erase(
                std::remove_if(
                    _invalidEntries.begin(), _invalidEntries.end(),
                    [de](const ObjectEntryDescriptor& e) { return de.GetName() == e.GetName(); }),
                _invalidEntries.end());
        }
        no_list_items = static_cast<uint16_t>(_invalidEntries.size());
    }
#endif

    /**
     *  Returns a newline-separated string listing all object names.
     *  Used for placing all names on the clipboard.
     */
    void CopyObjectNamesToClipboard()
    {
        std::stringstream stream;
        for (uint16_t i = 0; i < no_list_items; i++)
        {
            const auto& entry = _invalidEntries[i];
            stream << entry.GetName();
            stream << PLATFORM_NEWLINE;
        }

        const auto clip = stream.str();
        OpenRCT2::GetContext()->GetUiContext()->SetClipboardText(clip.c_str());
    }

    void SelectObjectFromList(const int32_t index)
    {
        if (index < 0 || index > no_list_items)
        {
            selected_list_item = -1;
        }
        else
        {
            selected_list_item = index;
        }
        WidgetInvalidate(*this, WIDX_SCROLL);
    }

public:
    void OnOpen() override
    {
        widgets = window_object_load_error_widgets;

        WindowInitScrollWidgets(*this);
        colours[0] = COLOUR_LIGHT_BLUE;
        colours[1] = COLOUR_LIGHT_BLUE;
        colours[2] = COLOUR_LIGHT_BLUE;
    }

    void OnClose() override
    {
        _invalidEntries.clear();
        _invalidEntries.shrink_to_fit();
    }

    void OnMouseUp(const WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                WindowClose(*this);
                return;
            case WIDX_COPY_CURRENT:
                if (selected_list_item > -1 && selected_list_item < no_list_items)
                {
                    const auto name = std::string(_invalidEntries[selected_list_item].GetName());
                    OpenRCT2::GetContext()->GetUiContext()->SetClipboardText(name.c_str());
                }
                break;
            case WIDX_COPY_ALL:
                CopyObjectNamesToClipboard();
                break;
#ifndef DISABLE_HTTP
            case WIDX_DOWNLOAD_ALL:
                DownloadAllObjects();
                break;
#endif
        }
    }

    void OnUpdate() override
    {
        frame_no++;

        // Check if the mouse is hovering over the list
        if (!WidgetIsHighlighted(*this, WIDX_SCROLL))
        {
            _highlightedIndex = -1;
            WidgetInvalidate(*this, WIDX_SCROLL);
        }

#ifndef DISABLE_HTTP
        _objDownloader.Update();

        // Remove downloaded objects from our invalid entry list
        if (_objDownloader.IsDownloading())
        {
            // Don't do this too often as it isn't particularly efficient
            if (frame_no % 64 == 0)
            {
                UpdateObjectList();
            }
        }
        else if (!_updatedListAfterDownload)
        {
            UpdateObjectList();
            _updatedListAfterDownload = true;
        }
#endif
    }

    ScreenSize OnScrollGetSize(const int32_t scrollIndex) override
    {
        return ScreenSize(0, no_list_items * SCROLLABLE_ROW_HEIGHT);
    }

    void OnScrollMouseDown(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        const auto selectedItem = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        SelectObjectFromList(selectedItem);
    }

    void OnScrollMouseOver(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        // Highlight item that the cursor is over, or remove highlighting if none
        const auto selectedItem = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (selectedItem < 0 || selectedItem >= no_list_items)
            _highlightedIndex = -1;
        else
            _highlightedIndex = selectedItem;

        WidgetInvalidate(*this, WIDX_SCROLL);
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        WindowDrawWidgets(*this, dpi);

        // Draw explanatory message
        auto ft = Formatter();
        ft.Add<StringId>(STR_OBJECT_ERROR_WINDOW_EXPLANATION);
        DrawTextWrapped(dpi, windowPos + ScreenCoordsXY{ 5, 18 }, WW - 10, STR_BLACK_STRING, ft);

        // Draw file name
        ft = Formatter();
        ft.Add<StringId>(STR_OBJECT_ERROR_WINDOW_FILE);
        ft.Add<utf8*>(_filePath.c_str());
        DrawTextEllipsised(dpi, { windowPos.x + 5, windowPos.y + 43 }, WW - 5, STR_BLACK_STRING, ft);
    }

    void OnScrollDraw(const int32_t scrollIndex, DrawPixelInfo& dpi) override
    {
        auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
        GfxFillRect(
            dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width - 1, dpi.height - 1 } }, ColourMapA[colours[1]].mid_light);
        const int32_t listWidth = widgets[WIDX_SCROLL].width();

        for (int32_t i = 0; i < no_list_items; i++)
        {
            ScreenCoordsXY screenCoords;
            screenCoords.y = i * SCROLLABLE_ROW_HEIGHT;
            if (screenCoords.y > dpi.y + dpi.height)
                break;

            if (screenCoords.y + SCROLLABLE_ROW_HEIGHT < dpi.y)
                continue;

            const auto screenRect = ScreenRect{ { 0, screenCoords.y },
                                                { listWidth, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } };
            // If hovering over item, change the color and fill the backdrop.
            if (i == selected_list_item)
                GfxFillRect(dpi, screenRect, ColourMapA[colours[1]].darker);
            else if (i == _highlightedIndex)
                GfxFillRect(dpi, screenRect, ColourMapA[colours[1]].mid_dark);
            else if ((i & 1) != 0) // odd / even check
                GfxFillRect(dpi, screenRect, ColourMapA[colours[1]].light);

            // Draw the actual object entry's name...
            screenCoords.x = NAME_COL_LEFT - 3;

            const auto& entry = _invalidEntries[i];

            auto name = entry.GetName();
            char buffer[256];
            String::Set(buffer, sizeof(buffer), name.data(), name.size());
            GfxDrawString(dpi, screenCoords, buffer, { COLOUR_DARK_GREEN });

            if (entry.Generation == ObjectGeneration::DAT)
            {
                // ... source game ...
                const auto sourceStringId = ObjectManagerGetSourceGameString(entry.Entry.GetSourceGame());
                DrawTextBasic(dpi, { SOURCE_COL_LEFT - 3, screenCoords.y }, sourceStringId, {}, { COLOUR_DARK_GREEN });
            }

            // ... and type
            const auto type = GetStringFromObjectType(entry.GetType());
            DrawTextBasic(dpi, { TYPE_COL_LEFT - 3, screenCoords.y }, type, {}, { COLOUR_DARK_GREEN });
        }
    }

    void Initialise(utf8* path, const size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects)
    {
        _invalidEntries = std::vector<ObjectEntryDescriptor>(missingObjects, missingObjects + numMissingObjects);

        // Refresh list items and path
        no_list_items = static_cast<uint16_t>(numMissingObjects);
        _filePath = path;

        Invalidate();
    }
};

WindowBase* WindowObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects)
{
    // Check if window is already open
    auto* window = WindowBringToFrontByClass(WindowClass::ObjectLoadError);
    if (window == nullptr)
    {
        window = WindowCreate<ObjectLoadErrorWindow>(WindowClass::ObjectLoadError, WW, WH, 0);
    }

    static_cast<ObjectLoadErrorWindow*>(window)->Initialise(path, numMissingObjects, missingObjects);

    return window;
}
