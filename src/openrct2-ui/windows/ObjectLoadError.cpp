/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <mutex>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/Console.hpp>
#include <openrct2/core/Http.h>
#include <openrct2/core/Json.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <sstream>
#include <string>
#include <vector>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
#ifndef DISABLE_HTTP

    // TODO: move to its own compilation unit
    class ObjectDownloader
    {
    private:
        static constexpr auto kOpenRCT2ApiLegacyObjectURL = "https://api.openrct2.io/objects/legacy/";

        struct DownloadStatusInfo
        {
            std::string Name;
            std::string Source;
            size_t Count{};
            size_t Total{};

            bool operator==(const DownloadStatusInfo& rhs) const
            {
                return Name == rhs.Name && Source == rhs.Source && Count == rhs.Count && Total == rhs.Total;
            }
            bool operator!=(const DownloadStatusInfo& rhs) const
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
                    ContextForceCloseWindowByClass(WindowClass::networkStatus);
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
                        FormatStringLegacy(
                            str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS, ft.Data());
                    }
                    else
                    {
                        ft.Add<char*>(_downloadStatusInfo.Name.c_str());
                        ft.Add<char*>(_downloadStatusInfo.Source.c_str());
                        ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Count));
                        ft.Add<int16_t>(static_cast<int16_t>(_downloadStatusInfo.Total));
                        FormatStringLegacy(
                            str_downloading_objects, sizeof(str_downloading_objects), STR_DOWNLOADING_OBJECTS_FROM, ft.Data());
                    }

                    auto intent = Intent(WindowClass::networkStatus);
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

                            auto& objRepo = GetContext()->GetObjectRepository();
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
            auto name = String::trim(std::string(entry.GetName()));
            LOG_VERBOSE("Downloading object: [%s]:", name.c_str());
            _currentDownloadIndex++;
            UpdateProgress({ name, _lastDownloadSource, _currentDownloadIndex, _entries.size() });
            try
            {
                Http::Request req;
                req.method = Http::Method::GET;
                req.url = kOpenRCT2ApiLegacyObjectURL + name;
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

    enum WindowObjectLoadErrorWidgetIdx
    {
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

    static constexpr StringId kWindowTitle = STR_OBJECT_LOAD_ERROR_TITLE;
    static constexpr ScreenSize kWindowSize = { 450, 400 };
    static constexpr int32_t kWindowWidthLessPadding = kWindowSize.width - 5;
    constexpr int32_t kNameColLeft = 4;
    constexpr int32_t kSourceColLeft = (kWindowWidthLessPadding / 4) + 1;
    constexpr int32_t kTypeColLeft = 5 * kWindowWidthLessPadding / 8 + 1;

    // clang-format off
    static constexpr auto window_object_load_error_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  kNameColLeft,  57}, {108,  14}, WidgetType::tableHeader, WindowColour::primary, STR_OBJECT_NAME                         ), // 'Object name' header
        makeWidget({kSourceColLeft,  57}, {166,  14}, WidgetType::tableHeader, WindowColour::primary, STR_OBJECT_SOURCE                       ), // 'Object source' header
        makeWidget({  kTypeColLeft,  57}, {166,  14}, WidgetType::tableHeader, WindowColour::primary, STR_OBJECT_TYPE                         ), // 'Object type' header
        makeWidget({  kNameColLeft,  70}, {442, 298}, WidgetType::scroll,      WindowColour::primary, SCROLL_VERTICAL                         ), // Scrollable list area
        makeWidget({  kNameColLeft, 377}, {145,  14}, WidgetType::button,      WindowColour::primary, STR_COPY_SELECTED, STR_COPY_SELECTED_TIP), // Copy selected button
        makeWidget({           152, 377}, {145,  14}, WidgetType::button,      WindowColour::primary, STR_COPY_ALL,      STR_COPY_ALL_TIP     )  // Copy all button
    #ifndef DISABLE_HTTP
      , makeWidget({           300, 377}, {146,  14}, WidgetType::button,      WindowColour::primary, STR_DOWNLOAD_ALL,  STR_DOWNLOAD_ALL_TIP )  // Download all button
    #endif
    );
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
            case ObjectType::ride:
                return STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS;
            case ObjectType::smallScenery:
                return STR_OBJECT_SELECTION_SMALL_SCENERY;
            case ObjectType::largeScenery:
                return STR_OBJECT_SELECTION_LARGE_SCENERY;
            case ObjectType::walls:
                return STR_OBJECT_SELECTION_WALLS_FENCES;
            case ObjectType::banners:
                return STR_OBJECT_SELECTION_PATH_SIGNS;
            case ObjectType::paths:
                return STR_OBJECT_SELECTION_FOOTPATHS;
            case ObjectType::pathAdditions:
                return STR_OBJECT_SELECTION_PATH_EXTRAS;
            case ObjectType::sceneryGroup:
                return STR_OBJECT_SELECTION_SCENERY_GROUPS;
            case ObjectType::parkEntrance:
                return STR_OBJECT_SELECTION_PARK_ENTRANCE;
            case ObjectType::water:
                return STR_OBJECT_SELECTION_WATER;
            default:
                return STR_UNKNOWN_OBJECT_TYPE;
        }
    }

    class ObjectLoadErrorWindow final : public Window
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
            numListItems = static_cast<uint16_t>(_invalidEntries.size());
        }
#endif

        /**
         *  Returns a newline-separated string listing all object names.
         *  Used for placing all names on the clipboard.
         */
        void CopyObjectNamesToClipboard()
        {
            std::stringstream stream;
            for (uint16_t i = 0; i < numListItems; i++)
            {
                const auto& entry = _invalidEntries[i];
                stream << entry.GetName();
                stream << PLATFORM_NEWLINE;
            }

            const auto clip = stream.str();
            GetContext()->GetUiContext().SetClipboardText(clip.c_str());
        }

        void SelectObjectFromList(const int32_t index)
        {
            if (index < 0 || index > numListItems)
            {
                selectedListItem = -1;
            }
            else
            {
                selectedListItem = index;
            }
            invalidateWidget(WIDX_SCROLL);
        }

    public:
        void onOpen() override
        {
            setWidgets(window_object_load_error_widgets);

            WindowInitScrollWidgets(*this);
            colours[0] = Drawing::Colour::lightBlue;
            colours[1] = Drawing::Colour::lightBlue;
            colours[2] = Drawing::Colour::lightBlue;
        }

        void onClose() override
        {
            _invalidEntries.clear();
            _invalidEntries.shrink_to_fit();
        }

        void onMouseUp(const WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    return;
                case WIDX_COPY_CURRENT:
                    if (selectedListItem > -1 && selectedListItem < numListItems)
                    {
                        const auto name = std::string(_invalidEntries[selectedListItem].GetName());
                        GetContext()->GetUiContext().SetClipboardText(name.c_str());
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

        void onUpdate() override
        {
            currentFrame++;

            // Check if the mouse is hovering over the list
            if (!widgetIsHighlighted(*this, WIDX_SCROLL))
            {
                _highlightedIndex = -1;
                invalidateWidget(WIDX_SCROLL);
            }

#ifndef DISABLE_HTTP
            _objDownloader.Update();

            // Remove downloaded objects from our invalid entry list
            if (_objDownloader.IsDownloading())
            {
                // Don't do this too often as it isn't particularly efficient
                if (currentFrame % 64 == 0)
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

        ScreenSize onScrollGetSize(const int32_t scrollIndex) override
        {
            return ScreenSize(0, numListItems * kScrollableRowHeight);
        }

        void onScrollMouseDown(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const auto selectedItem = screenCoords.y / kScrollableRowHeight;
            SelectObjectFromList(selectedItem);
        }

        void onScrollMouseOver(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // Highlight item that the cursor is over, or remove highlighting if none
            const auto selectedItem = screenCoords.y / kScrollableRowHeight;
            if (selectedItem < 0 || selectedItem >= numListItems)
                _highlightedIndex = -1;
            else
                _highlightedIndex = selectedItem;

            invalidateWidget(WIDX_SCROLL);
        }

        void onDraw(RenderTarget& rt) override
        {
            WindowDrawWidgets(*this, rt);

            auto screenPos = windowPos + ScreenCoordsXY{ 5, widgets[WIDX_TITLE].bottom };

            // Draw explanatory message
            auto ft = Formatter();
            ft.Add<StringId>(STR_OBJECT_ERROR_WINDOW_EXPLANATION);
            DrawTextWrapped(rt, screenPos + ScreenCoordsXY{ 0, 4 }, kWindowSize.width - 10, STR_BLACK_STRING, ft);

            // Draw file name
            ft = Formatter();
            ft.Add<StringId>(STR_OBJECT_ERROR_WINDOW_FILE);
            ft.Add<utf8*>(_filePath.c_str());
            DrawTextEllipsised(rt, screenPos + ScreenCoordsXY{ 0, 29 }, kWindowSize.width - 5, STR_BLACK_STRING, ft);
        }

        void onScrollDraw(const int32_t scrollIndex, RenderTarget& rt) override
        {
            auto rtCoords = ScreenCoordsXY{ rt.x, rt.y };
            Rectangle::fill(
                rt, { rtCoords, rtCoords + ScreenCoordsXY{ rt.width - 1, rt.height - 1 } },
                getColourMap(colours[1].colour).midLight);
            const int32_t listWidth = widgets[WIDX_SCROLL].width() - 1;

            for (int32_t i = 0; i < numListItems; i++)
            {
                ScreenCoordsXY screenCoords;
                screenCoords.y = i * kScrollableRowHeight;
                if (screenCoords.y > rt.y + rt.height)
                    break;

                if (screenCoords.y + kScrollableRowHeight < rt.y)
                    continue;

                const auto screenRect = ScreenRect{ { 0, screenCoords.y },
                                                    { listWidth, screenCoords.y + kScrollableRowHeight - 1 } };
                // If hovering over item, change the color and fill the backdrop.
                if (i == selectedListItem)
                    Rectangle::fill(rt, screenRect, getColourMap(colours[1].colour).darker);
                else if (i == _highlightedIndex)
                    Rectangle::fill(rt, screenRect, getColourMap(colours[1].colour).midDark);
                else if ((i & 1) != 0) // odd / even check
                    Rectangle::fill(rt, screenRect, getColourMap(colours[1].colour).light);

                // Draw the actual object entry's name...
                screenCoords.x = kNameColLeft - 3;

                const auto& entry = _invalidEntries[i];

                auto name = entry.GetName();
                char buffer[256];
                String::set(buffer, sizeof(buffer), name.data(), name.size());
                DrawText(rt, screenCoords, { Drawing::Colour::darkGreen }, buffer);

                if (entry.Generation == ObjectGeneration::DAT)
                {
                    // ... source game ...
                    const auto sourceStringId = ObjectManagerGetSourceGameString(entry.Entry.GetSourceGame());
                    DrawTextBasic(
                        rt, { kSourceColLeft - 3, screenCoords.y }, sourceStringId, {}, { Drawing::Colour::darkGreen });
                }

                // ... and type
                const auto type = GetStringFromObjectType(entry.GetType());
                DrawTextBasic(rt, { kTypeColLeft - 3, screenCoords.y }, type, {}, { Drawing::Colour::darkGreen });
            }
        }

        void initialise(utf8* path, const size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects)
        {
            _invalidEntries = std::vector<ObjectEntryDescriptor>(missingObjects, missingObjects + numMissingObjects);

            // Refresh list items and path
            numListItems = static_cast<uint16_t>(numMissingObjects);
            _filePath = path;

            invalidate();
        }
    };

    WindowBase* ObjectLoadErrorOpen(utf8* path, size_t numMissingObjects, const ObjectEntryDescriptor* missingObjects)
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::objectLoadError);
        if (window == nullptr)
        {
            window = windowMgr->Create<ObjectLoadErrorWindow>(WindowClass::objectLoadError, kWindowSize, {});
        }

        static_cast<ObjectLoadErrorWindow*>(window)->initialise(path, numMissingObjects, missingObjects);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
