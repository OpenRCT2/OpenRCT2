/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/platform/platform.h>
#include <openrct2/windows/Intent.h>
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

    std::vector<rct_object_entry> _entries;
    std::vector<rct_object_entry> _downloadedEntries;
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
    void Begin(const std::vector<rct_object_entry>& entries)
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

    std::vector<rct_object_entry> GetDownloadedEntries()
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
                context_force_close_window_by_class(WC_NETWORK_STATUS);
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
                    format_string(str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS, ft.Data());
                }
                else
                {
                    ft.Add<char*>(_downloadStatusInfo.Name.c_str());
                    ft.Add<char*>(_downloadStatusInfo.Source.c_str());
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Count));
                    ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Total));
                    format_string(
                        str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS_FROM, ft.Data());
                }

                auto intent = Intent(WC_NETWORK_STATUS);
                intent.putExtra(INTENT_EXTRA_MESSAGE, std::string(str_downloading_objects));
                intent.putExtra(INTENT_EXTRA_CALLBACK, []() -> void { _downloadingObjects = false; });
                context_open_intent(&intent);
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

    void DownloadObject(const rct_object_entry& entry, const std::string name, const std::string url)
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
                        objRepo.AddObjectFromFile(name, data, dataLen);

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
        auto name = String::Trim(std::string(entry.name, sizeof(entry.name)));
        log_verbose("Downloading object: [%s]:", name.c_str());
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
enum WINDOW_OBJECT_LOAD_ERROR_WIDGET_IDX {
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

static constexpr const rct_string_id WINDOW_TITLE = STR_OBJECT_LOAD_ERROR_TITLE;
static constexpr const int32_t WW = 450;
static constexpr const int32_t WH = 400;
static constexpr const int32_t WW_LESS_PADDING = WW - 5;
constexpr int32_t NAME_COL_LEFT = 4;
constexpr int32_t SOURCE_COL_LEFT = (WW_LESS_PADDING / 4) + 1;
constexpr int32_t TYPE_COL_LEFT = 5 * WW_LESS_PADDING / 8 + 1;

static rct_widget window_object_load_error_widgets[] = {
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
    { WIDGETS_END },
};

static rct_string_id get_object_type_string(const rct_object_entry *entry);
static void window_object_load_error_close(rct_window *w);
static void window_object_load_error_update(rct_window *w);
static void window_object_load_error_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_object_load_error_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_object_load_error_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_object_load_error_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_object_load_error_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_object_load_error_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);
#ifndef DISABLE_HTTP
static void window_object_load_error_download_all(rct_window* w);
static void window_object_load_error_update_list(rct_window* w);
#endif

static rct_window_event_list window_object_load_error_events([](auto& events)
{
    events.close = &window_object_load_error_close;
    events.mouse_up = &window_object_load_error_mouseup;
    events.update = &window_object_load_error_update;
    events.get_scroll_size = &window_object_load_error_scrollgetsize;
    events.scroll_mousedown = &window_object_load_error_scrollmousedown;
    events.scroll_mouseover = &window_object_load_error_scrollmouseover;
    events.paint = &window_object_load_error_paint;
    events.scroll_paint = &window_object_load_error_scrollpaint;
});
// clang-format on

static std::vector<rct_object_entry> _invalid_entries;
static int32_t highlighted_index = -1;
static std::string file_path;
#ifndef DISABLE_HTTP
static ObjectDownloader _objDownloader;
static bool _updatedListAfterDownload;
#endif

/**
 *  Returns an rct_string_id that represents an rct_object_entry's type.
 *
 *  Could possibly be moved out of the window file if other
 *  uses exist and a suitable location is found.
 */
static rct_string_id get_object_type_string(const rct_object_entry* entry)
{
    rct_string_id result;
    switch (entry->GetType())
    {
        case ObjectType::Ride:
            result = STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS;
            break;
        case ObjectType::SmallScenery:
            result = STR_OBJECT_SELECTION_SMALL_SCENERY;
            break;
        case ObjectType::LargeScenery:
            result = STR_OBJECT_SELECTION_LARGE_SCENERY;
            break;
        case ObjectType::Walls:
            result = STR_OBJECT_SELECTION_WALLS_FENCES;
            break;
        case ObjectType::Banners:
            result = STR_OBJECT_SELECTION_PATH_SIGNS;
            break;
        case ObjectType::Paths:
            result = STR_OBJECT_SELECTION_FOOTPATHS;
            break;
        case ObjectType::PathBits:
            result = STR_OBJECT_SELECTION_PATH_EXTRAS;
            break;
        case ObjectType::SceneryGroup:
            result = STR_OBJECT_SELECTION_SCENERY_GROUPS;
            break;
        case ObjectType::ParkEntrance:
            result = STR_OBJECT_SELECTION_PARK_ENTRANCE;
            break;
        case ObjectType::Water:
            result = STR_OBJECT_SELECTION_WATER;
            break;
        default:
            result = STR_UNKNOWN_OBJECT_TYPE;
    }
    return result;
}

/**
 *  Returns a newline-separated string listing all object names.
 *  Used for placing all names on the clipboard.
 */
static void copy_object_names_to_clipboard(rct_window* w)
{
    // Something has gone wrong, this shouldn't happen.
    // We don't want to allocate stupidly large amounts of memory for no reason
    assert(w->no_list_items > 0 && w->no_list_items <= OBJECT_ENTRY_COUNT);

    // No system has a newline over 2 characters
    size_t line_sep_len = strnlen(PLATFORM_NEWLINE, 2);
    size_t buffer_len = (w->no_list_items * (8 + line_sep_len)) + 1;
    utf8* buffer = new utf8[buffer_len]{};

    size_t cur_len = 0;
    for (uint16_t i = 0; i < w->no_list_items; i++)
    {
        cur_len += (8 + line_sep_len);
        assert(cur_len < buffer_len);

        uint16_t nameLength = 8;
        for (; nameLength > 0; nameLength--)
        {
            if (_invalid_entries[i].name[nameLength - 1] != ' ')
                break;
        }

        strncat(buffer, _invalid_entries[i].name, nameLength);
        strncat(buffer, PLATFORM_NEWLINE, buffer_len - strlen(buffer) - 1);
    }

    platform_place_string_on_clipboard(buffer);
    delete[] buffer;
}

rct_window* window_object_load_error_open(utf8* path, size_t numMissingObjects, const rct_object_entry* missingObjects)
{
    _invalid_entries = std::vector<rct_object_entry>(missingObjects, missingObjects + numMissingObjects);

    // Check if window is already open
    rct_window* window = window_bring_to_front_by_class(WC_OBJECT_LOAD_ERROR);
    if (window == nullptr)
    {
        window = WindowCreateCentred(WW, WH, &window_object_load_error_events, WC_OBJECT_LOAD_ERROR, 0);

        window->widgets = window_object_load_error_widgets;
        window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_COPY_CURRENT) | (1 << WIDX_COPY_ALL)
            | (1 << WIDX_DOWNLOAD_ALL);

        WindowInitScrollWidgets(window);
        window->colours[0] = COLOUR_LIGHT_BLUE;
        window->colours[1] = COLOUR_LIGHT_BLUE;
        window->colours[2] = COLOUR_LIGHT_BLUE;
    }

    // Refresh list items and path
    window->no_list_items = static_cast<uint16_t>(numMissingObjects);
    file_path = path;

    window->Invalidate();
    return window;
}

static void window_object_load_error_close(rct_window* w)
{
    _invalid_entries.clear();
    _invalid_entries.shrink_to_fit();
}

static void window_object_load_error_update(rct_window* w)
{
    w->frame_no++;

    // Check if the mouse is hovering over the list
    if (!WidgetIsHighlighted(w, WIDX_SCROLL))
    {
        highlighted_index = -1;
        widget_invalidate(w, WIDX_SCROLL);
    }

#ifndef DISABLE_HTTP
    _objDownloader.Update();

    // Remove downloaded objects from our invalid entry list
    if (_objDownloader.IsDownloading())
    {
        // Don't do this too often as it isn't particularly efficient
        if (w->frame_no % 64 == 0)
        {
            window_object_load_error_update_list(w);
        }
    }
    else if (!_updatedListAfterDownload)
    {
        window_object_load_error_update_list(w);
        _updatedListAfterDownload = true;
    }
#endif
}

static void window_object_load_error_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_COPY_CURRENT:
            if (w->selected_list_item > -1 && w->selected_list_item < w->no_list_items)
            {
                utf8* selected_name = strndup(_invalid_entries[w->selected_list_item].name, 8);
                utf8* strp = strchr(selected_name, ' ');
                if (strp != nullptr)
                    *strp = '\0';

                platform_place_string_on_clipboard(selected_name);
                SafeFree(selected_name);
            }
            break;
        case WIDX_COPY_ALL:
            copy_object_names_to_clipboard(w);
            break;
#ifndef DISABLE_HTTP
        case WIDX_DOWNLOAD_ALL:
            window_object_load_error_download_all(w);
            break;
#endif
    }
}

static void window_object_load_error_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    // Highlight item that the cursor is over, or remove highlighting if none
    int32_t selected_item;
    selected_item = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (selected_item < 0 || selected_item >= w->no_list_items)
        highlighted_index = -1;
    else
        highlighted_index = selected_item;

    widget_invalidate(w, WIDX_SCROLL);
}

static void window_object_load_error_select_element_from_list(rct_window* w, int32_t index)
{
    if (index < 0 || index > w->no_list_items)
    {
        w->selected_list_item = -1;
    }
    else
    {
        w->selected_list_item = index;
    }
    widget_invalidate(w, WIDX_SCROLL);
}

static void window_object_load_error_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t selected_item;
    selected_item = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    window_object_load_error_select_element_from_list(w, selected_item);
}

static void window_object_load_error_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = w->no_list_items * SCROLLABLE_ROW_HEIGHT;
}

static void window_object_load_error_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    // Draw explanatory message
    auto ft = Formatter();
    ft.Add<rct_string_id>(STR_OBJECT_ERROR_WINDOW_EXPLANATION);
    DrawTextWrapped(dpi, w->windowPos + ScreenCoordsXY{ 5, 18 }, WW - 10, STR_BLACK_STRING, ft);

    // Draw file name
    ft = Formatter();
    ft.Add<rct_string_id>(STR_OBJECT_ERROR_WINDOW_FILE);
    ft.Add<utf8*>(file_path.c_str());
    DrawTextEllipsised(dpi, { w->windowPos.x + 5, w->windowPos.y + 43 }, WW - 5, STR_BLACK_STRING, ft);
}

static void window_object_load_error_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto dpiCoords = ScreenCoordsXY{ dpi->x, dpi->y };
    gfx_fill_rect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi->width - 1, dpi->height - 1 } }, ColourMapA[w->colours[1]].mid_light);
    const int32_t list_width = w->widgets[WIDX_SCROLL].width();

    for (int32_t i = 0; i < w->no_list_items; i++)
    {
        ScreenCoordsXY screenCoords;
        screenCoords.y = i * SCROLLABLE_ROW_HEIGHT;
        if (screenCoords.y > dpi->y + dpi->height)
            break;

        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT < dpi->y)
            continue;

        auto screenRect = ScreenRect{ { 0, screenCoords.y }, { list_width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } };
        // If hovering over item, change the color and fill the backdrop.
        if (i == w->selected_list_item)
            gfx_fill_rect(dpi, screenRect, ColourMapA[w->colours[1]].darker);
        else if (i == highlighted_index)
            gfx_fill_rect(dpi, screenRect, ColourMapA[w->colours[1]].mid_dark);
        else if ((i & 1) != 0) // odd / even check
            gfx_fill_rect(dpi, screenRect, ColourMapA[w->colours[1]].light);

        // Draw the actual object entry's name...
        screenCoords.x = NAME_COL_LEFT - 3;
        gfx_draw_string(dpi, screenCoords, strndup(_invalid_entries[i].name, 8), { COLOUR_DARK_GREEN });

        // ... source game ...
        rct_string_id sourceStringId = object_manager_get_source_game_string(_invalid_entries[i].GetSourceGame());
        DrawTextBasic(dpi, { SOURCE_COL_LEFT - 3, screenCoords.y }, sourceStringId, {}, { COLOUR_DARK_GREEN });

        // ... and type
        rct_string_id type = get_object_type_string(&_invalid_entries[i]);
        DrawTextBasic(dpi, { TYPE_COL_LEFT - 3, screenCoords.y }, type, {}, { COLOUR_DARK_GREEN });
    }
}

#ifndef DISABLE_HTTP

static void window_object_load_error_download_all(rct_window* w)
{
    if (!_objDownloader.IsDownloading())
    {
        _updatedListAfterDownload = false;
        _objDownloader.Begin(_invalid_entries);
    }
}

static void window_object_load_error_update_list(rct_window* w)
{
    auto entries = _objDownloader.GetDownloadedEntries();
    for (auto& de : entries)
    {
        _invalid_entries.erase(
            std::remove_if(
                _invalid_entries.begin(), _invalid_entries.end(),
                [de](const rct_object_entry& e) { return std::memcmp(de.name, e.name, sizeof(e.name)) == 0; }),
            _invalid_entries.end());
        w->no_list_items = static_cast<uint16_t>(_invalid_entries.size());
    }
}

#endif
