/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <vector>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_SELECT_DESIGN;
    static constexpr ScreenSize kWindowSize = { 600, 441 };
    static constexpr int32_t kDebugPathHeight = 12;
    static constexpr int32_t kRotateAndSceneryButtonSize = 24;
    static constexpr int32_t kWindowPadding = 5;

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_BACK,
        WIDX_FILTER_STRING,
        WIDX_FILTER_CLEAR,
        WIDX_TRACK_LIST,
        WIDX_TRACK_PREVIEW,
        WIDX_ROTATE,
        WIDX_TOGGLE_SCENERY,
    };

    VALIDATE_GLOBAL_WIDX(WC_TRACK_DESIGN_LIST, WIDX_ROTATE);

    // clang-format off
    static constexpr auto kFlatBtnSize = ScreenSize{kRotateAndSceneryButtonSize, kRotateAndSceneryButtonSize};
    static constexpr auto _trackListWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  4,  18}, {218,  13},   WidgetType::tableHeader,  WindowColour::primary, STR_SELECT_OTHER_RIDE                                       ),
        makeWidget({  4,  32}, {124,  13},   WidgetType::textBox,      WindowColour::secondary                                                            ),
        makeWidget({130,  32}, { 92,  13},   WidgetType::button,       WindowColour::primary, STR_OBJECT_SEARCH_CLEAR                                     ),
        makeWidget({  4,  46}, {218, 381},   WidgetType::scroll,       WindowColour::primary, SCROLL_VERTICAL,         STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP),
        makeWidget({224,  18}, {372, 219},   WidgetType::flatBtn,      WindowColour::primary                                                              ),
        makeWidget({572, 405}, kFlatBtnSize, WidgetType::flatBtn,      WindowColour::primary, ImageId(SPR_ROTATE_ARROW),        STR_ROTATE_90_TIP                  ),
        makeWidget({572, 381}, kFlatBtnSize, WidgetType::flatBtn,      WindowColour::primary, ImageId(SPR_SCENERY),             STR_TOGGLE_SCENERY_TIP             )
    );
    // clang-format on

    constexpr uint16_t kTrackDesignIndexUnloaded = UINT16_MAX;

    RideSelection _window_track_list_item;

    class TrackListWindow final : public Window
    {
    private:
        std::vector<TrackDesignFileRef> _trackDesigns;
        utf8 _filterString[kUserStringMaxLength]{};
        std::vector<uint16_t> _filteredTrackIds;
        uint16_t _loadedTrackDesignIndex;
        std::unique_ptr<TrackDesign> _loadedTrackDesign;
        std::vector<uint8_t> _trackDesignPreviewPixels;
        bool _selectedItemIsBeingUpdated;
        bool _reloadTrackDesigns;

        void FilterList()
        {
            _filteredTrackIds.clear();

            // Nothing to filter, so fill the list with all indices
            if (String::lengthOf(_filterString) == 0)
            {
                for (uint16_t i = 0; i < _trackDesigns.size(); i++)
                    _filteredTrackIds.push_back(i);

                return;
            }

            // Convert filter to uppercase
            const auto filterStringUpper = String::toUpper(_filterString);

            // Fill the set with indices for tracks that match the filter
            for (uint16_t i = 0; i < _trackDesigns.size(); i++)
            {
                const auto trackNameUpper = String::toUpper(_trackDesigns[i].name);
                if (trackNameUpper.find(filterStringUpper) != std::string::npos)
                {
                    _filteredTrackIds.push_back(i);
                }
            }

            // Ensure that the selected item is still in the list.
            if (static_cast<size_t>(selectedListItem) >= _filteredTrackIds.size())
            {
                selectedListItem = 0;
            }
        }

        void SelectFromList(int32_t listIndex)
        {
            Audio::Play(Audio::SoundId::click1, 0, this->windowPos.x + (this->width / 2));
            if (!(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                if (listIndex == 0)
                {
                    close();
                    RideConstructNew(_window_track_list_item);
                    return;
                }
                listIndex--;
            }

            // Displays a message if the ride can't load, fix #4080
            if (_loadedTrackDesign == nullptr)
            {
                ContextShowError(STR_CANT_BUILD_THIS_HERE, STR_TRACK_LOAD_FAILED_ERROR, {});
                return;
            }

            if (_loadedTrackDesign->gameStateData.hasFlag(TrackDesignGameStateFlag::SceneryUnavailable))
            {
                gTrackDesignSceneryToggle = true;
            }

            uint16_t trackDesignIndex = _filteredTrackIds[listIndex];
            TrackDesignFileRef* tdRef = &_trackDesigns[trackDesignIndex];
            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                auto intent = Intent(WindowClass::manageTrackDesign);
                intent.PutExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
                ContextOpenIntent(&intent);
            }
            else
            {
                if (_loadedTrackDesignIndex != kTrackDesignIndexUnloaded
                    && (_loadedTrackDesign->gameStateData.hasFlag(TrackDesignGameStateFlag::VehicleUnavailable)))
                {
                    ContextShowError(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, kStringIdNone, {});
                }

                auto intent = Intent(WindowClass::trackDesignPlace);
                intent.PutExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
                ContextOpenIntent(&intent);
            }
        }

        int32_t GetListItemFromPosition(const ScreenCoordsXY& screenCoords)
        {
            size_t maxItems = _filteredTrackIds.size();
            if (!(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                // Extra item: custom design
                maxItems++;
            }

            int32_t index = screenCoords.y / kScrollableRowHeight;
            if (index < 0 || static_cast<uint32_t>(index) >= maxItems)
            {
                index = -1;
            }
            return index;
        }

        void LoadDesignsList(RideSelection item)
        {
            auto repo = GetContext()->GetTrackDesignRepository();
            std::string entryName;
            if (item.Type < 0x80)
            {
                if (GetRideTypeDescriptor(item.Type).HasFlag(RtdFlag::listVehiclesSeparately))
                {
                    entryName = GetRideEntryName(item.EntryIndex);
                }
            }
            _trackDesigns = repo->GetItemsForObjectEntry(item.Type, entryName);

            FilterList();
        }

        bool LoadDesignPreview(const u8string& path)
        {
            _loadedTrackDesign = TrackDesignImport(path.c_str());
            if (_loadedTrackDesign != nullptr)
            {
                TrackDesignDrawPreview(*_loadedTrackDesign, _trackDesignPreviewPixels.data(), !gTrackDesignSceneryToggle);
                return true;
            }
            return false;
        }

    public:
        TrackListWindow(const RideSelection item)
        {
            _window_track_list_item = item;
        }

        void onOpen() override
        {
            String::set(_filterString, sizeof(_filterString), "");
            setWidgets(_trackListWidgets);
            widgets[WIDX_FILTER_STRING].string = _filterString;

            LoadDesignsList(_window_track_list_item);

            WindowInitScrollWidgets(*this);
            _selectedItemIsBeingUpdated = false;
            _reloadTrackDesigns = false;
            // Start with first track highlighted
            selectedListItem = 0;
            if (_trackDesigns.size() != 0 && !(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                selectedListItem = 1;
            }
            gTrackDesignSceneryToggle = false;
            WindowPushOthersRight(*this);
            _currentTrackPieceDirection = 2;
            _trackDesignPreviewPixels.resize(4 * kTrackPreviewImageSize);

            _loadedTrackDesign = nullptr;
            _loadedTrackDesignIndex = kTrackDesignIndexUnloaded;
        }

        void ReopenTrackManager()
        {
            auto* windowMgr = GetWindowManager();
            windowMgr->CloseByNumber(WindowClass::manageTrackDesign, number);
            windowMgr->CloseByNumber(WindowClass::trackDeletePrompt, number);
            Editor::LoadTrackManager();
        }

        void onClose() override
        {
            // Dispose track design and preview
            _loadedTrackDesign = nullptr;
            _trackDesignPreviewPixels.clear();
            _trackDesignPreviewPixels.shrink_to_fit();

            // Dispose track list
            _trackDesigns.clear();

            // If gScreenAge is zero, we're already in the process
            // of loading the track manager, so we shouldn't try
            // to do it again. Otherwise, this window will get
            // another close signal from the track manager load function,
            // try to load the track manager again, and an infinite loop will result.
            if ((gLegacyScene == LegacyScene::trackDesignsManager) && gScreenAge != 0)
            {
                ReopenTrackManager();
            }
        }

        void onMouseUp(const WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_ROTATE:
                    _currentTrackPieceDirection++;
                    _currentTrackPieceDirection %= 4;
                    invalidate();
                    break;
                case WIDX_TOGGLE_SCENERY:
                    gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
                    _loadedTrackDesignIndex = kTrackDesignIndexUnloaded;
                    invalidate();
                    break;
                case WIDX_BACK:
                    close();
                    if (!(gLegacyScene == LegacyScene::trackDesignsManager))
                    {
                        ContextOpenWindow(WindowClass::constructRide);
                    }
                    else
                    {
                        ReopenTrackManager();
                    }
                    break;
                case WIDX_FILTER_STRING:
                    WindowStartTextbox(*this, widgetIndex, _filterString, sizeof(_filterString));
                    break;
                case WIDX_FILTER_CLEAR:
                    // Keep the highlighted item selected
                    if (gLegacyScene == LegacyScene::trackDesignsManager)
                    {
                        if (selectedListItem != -1 && _filteredTrackIds.size() > static_cast<size_t>(selectedListItem))
                            selectedListItem = _filteredTrackIds[selectedListItem];
                        else
                            selectedListItem = -1;
                    }
                    else
                    {
                        if (selectedListItem != 0)
                            selectedListItem = _filteredTrackIds[selectedListItem - 1] + 1;
                    }

                    String::set(_filterString, sizeof(_filterString), "");
                    FilterList();
                    invalidate();
                    break;
            }
        }

        ScreenSize onScrollGetSize(const int32_t scrollIndex) override
        {
            size_t numItems = _filteredTrackIds.size();
            if (!(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                // Extra item: custom design
                numItems++;
            }
            int32_t scrollHeight = static_cast<int32_t>(numItems * kScrollableRowHeight);

            return { width, scrollHeight };
        }

        void onScrollMouseDown(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (!_selectedItemIsBeingUpdated)
            {
                int32_t i = GetListItemFromPosition(screenCoords);
                if (i != -1)
                {
                    SelectFromList(i);
                }
            }
        }

        void onScrollMouseOver(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (!_selectedItemIsBeingUpdated)
            {
                int32_t i = GetListItemFromPosition(screenCoords);
                if (i != -1 && selectedListItem != i)
                {
                    selectedListItem = i;
                    invalidate();
                }
            }
        }

        void onTextInput(const WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_FILTER_STRING)
                return;

            if (String::equals(_filterString, std::string(text).c_str()))
                return;

            String::set(_filterString, sizeof(_filterString), std::string(text).c_str());

            FilterList();

            scrolls->contentOffsetY = 0;

            invalidate();
        }

        void onPrepareDraw() override
        {
            StringId stringId = kStringIdNone;
            const auto* entry = GetRideEntryByIndex(_window_track_list_item.EntryIndex);

            if (entry != nullptr)
            {
                RideNaming rideName = GetRideNaming(_window_track_list_item.Type, entry);
                stringId = rideName.Name;
            }

            Formatter::Common().Add<StringId>(stringId);
            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS;
                widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
            }
            else
            {
                widgets[WIDX_TITLE].text = STR_SELECT_DESIGN;
                widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
            }

            if ((gLegacyScene == LegacyScene::trackDesignsManager) || selectedListItem != 0)
            {
                pressedWidgets |= 1uLL << WIDX_TRACK_PREVIEW;
                disabledWidgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
                widgets[WIDX_ROTATE].type = WidgetType::flatBtn;
                widgets[WIDX_TOGGLE_SCENERY].type = WidgetType::flatBtn;
                if (gTrackDesignSceneryToggle)
                {
                    pressedWidgets &= ~(1uLL << WIDX_TOGGLE_SCENERY);
                }
                else
                {
                    pressedWidgets |= (1uLL << WIDX_TOGGLE_SCENERY);
                }
            }
            else
            {
                pressedWidgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
                disabledWidgets |= (1uLL << WIDX_TRACK_PREVIEW);
                widgets[WIDX_ROTATE].type = WidgetType::empty;
                widgets[WIDX_TOGGLE_SCENERY].type = WidgetType::empty;
            }

            // When debugging tools are on, shift everything up a bit to make room for displaying the path.
            const int32_t bottomMargin = Config::Get().general.debuggingTools ? (kWindowPadding + kDebugPathHeight)
                                                                              : kWindowPadding;
            widgets[WIDX_TRACK_LIST].bottom = height - bottomMargin;
            widgets[WIDX_ROTATE].bottom = height - bottomMargin;
            widgets[WIDX_ROTATE].top = widgets[WIDX_ROTATE].bottom - kRotateAndSceneryButtonSize;
            widgets[WIDX_TOGGLE_SCENERY].bottom = widgets[WIDX_ROTATE].top;
            widgets[WIDX_TOGGLE_SCENERY].top = widgets[WIDX_TOGGLE_SCENERY].bottom - kRotateAndSceneryButtonSize;
        }

        void onUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILTER_STRING); // TODO Check this
            }

            if (_reloadTrackDesigns)
            {
                LoadDesignsList(_window_track_list_item);
                selectedListItem = 0;
                invalidate();
                _reloadTrackDesigns = false;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            int32_t listItemIndex = selectedListItem;
            if ((gLegacyScene == LegacyScene::trackDesignsManager) == 0)
            {
                // Because the first item in the list is "Build a custom design", lower the index by one
                listItemIndex--;
            }

            if (_filteredTrackIds.empty() || listItemIndex == -1)
                return;

            int32_t trackIndex = _filteredTrackIds[listItemIndex];

            // Track preview
            auto& tdWidget = widgets[WIDX_TRACK_PREVIEW];
            auto colour = getColourMap(colours[0].colour).darkest;
            u8string path = _trackDesigns[trackIndex].path;

            // Show track file path (in debug mode)
            if (Config::Get().general.debuggingTools)
            {
                const auto shortPath = ShortenPath(path, width, FontStyle::medium);
                auto ft = Formatter();
                ft.Add<utf8*>(shortPath.c_str());
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 0, height - kDebugPathHeight - 3 }, STR_STRING, ft,
                    { colours[1] }); // TODO Check dpi
            }

            auto screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.top + 1 };
            Rectangle::fill(rt, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour); // TODO Check dpi

            if (_loadedTrackDesignIndex != trackIndex)
            {
                if (LoadDesignPreview(path))
                {
                    _loadedTrackDesignIndex = trackIndex;
                }
                else
                {
                    _loadedTrackDesignIndex = kTrackDesignIndexUnloaded;
                }
            }

            if (!_loadedTrackDesign)
            {
                return;
            }

            auto trackPreview = screenPos;
            screenPos = windowPos + ScreenCoordsXY{ tdWidget.midX(), tdWidget.midY() };

            G1Element g1temp = {};
            g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * kTrackPreviewImageSize);
            g1temp.width = 370;
            g1temp.height = 217;
            g1temp.flags = { G1Flag::hasTransparency };
            GfxSetG1Element(SPR_TEMP, &g1temp);
            DrawingEngineInvalidateImage(SPR_TEMP);
            GfxDrawSprite(rt, ImageId(SPR_TEMP), trackPreview);

            screenPos.y = windowPos.y + tdWidget.bottom - 12;

            // Warnings
            if ((_loadedTrackDesign->gameStateData.hasFlag(TrackDesignGameStateFlag::VehicleUnavailable))
                && !(gLegacyScene == LegacyScene::trackDesignsManager))
            {
                // Vehicle design not available
                DrawTextEllipsised(rt, screenPos, 368, STR_VEHICLE_DESIGN_UNAVAILABLE, {}, { TextAlignment::centre });
                screenPos.y -= kScrollableRowHeight;
            }

            if (_loadedTrackDesign->gameStateData.hasFlag(TrackDesignGameStateFlag::SceneryUnavailable))
            {
                if (!gTrackDesignSceneryToggle)
                {
                    // Scenery not available
                    DrawTextEllipsised(
                        rt, screenPos, 368, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::centre });
                    screenPos.y -= kScrollableRowHeight;
                }
            }

            // Track design name
            auto ft = Formatter();
            ft.Add<const utf8*>(_trackDesigns[trackIndex].name.c_str());
            DrawTextEllipsised(rt, screenPos, 368, STR_TRACK_PREVIEW_NAME_FORMAT, ft, { TextAlignment::centre });

            // Information
            screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.bottom + 2 };

            // Stats
            ft = Formatter();
            ft.Add<fixed32_2dp>(_loadedTrackDesign->statistics.ratings.excitement);
            DrawTextBasic(rt, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, ft);
            screenPos.y += kListRowHeight;

            ft = Formatter();
            ft.Add<fixed32_2dp>(_loadedTrackDesign->statistics.ratings.intensity);
            DrawTextBasic(rt, screenPos, STR_TRACK_LIST_INTENSITY_RATING, ft);
            screenPos.y += kListRowHeight;

            ft = Formatter();
            ft.Add<fixed32_2dp>(_loadedTrackDesign->statistics.ratings.nausea);
            DrawTextBasic(rt, screenPos, STR_TRACK_LIST_NAUSEA_RATING, ft);
            screenPos.y += kListRowHeight + 4;

            // Information for tracked rides.
            if (GetRideTypeDescriptor(_loadedTrackDesign->trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasTrack))
            {
                const auto& rtd = GetRideTypeDescriptor(_loadedTrackDesign->trackAndVehicle.rtdIndex);
                if (rtd.specialType != RtdSpecialType::maze)
                {
                    if (rtd.specialType == RtdSpecialType::miniGolf)
                    {
                        // Holes
                        ft = Formatter();
                        ft.Add<uint16_t>(_loadedTrackDesign->statistics.holes);
                        DrawTextBasic(rt, screenPos, STR_HOLES, ft);
                        screenPos.y += kListRowHeight;
                    }
                    else
                    {
                        // Maximum speed
                        ft = Formatter();
                        ft.Add<uint16_t>(ToHumanReadableSpeed(_loadedTrackDesign->statistics.maxSpeed << 16));
                        DrawTextBasic(rt, screenPos, STR_MAX_SPEED, ft);
                        screenPos.y += kListRowHeight;

                        // Average speed
                        ft = Formatter();
                        ft.Add<uint16_t>(ToHumanReadableSpeed(_loadedTrackDesign->statistics.averageSpeed << 16));
                        DrawTextBasic(rt, screenPos, STR_AVERAGE_SPEED, ft);
                        screenPos.y += kListRowHeight;
                    }

                    // Ride length
                    ft = Formatter();
                    ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                    ft.Add<uint16_t>(_loadedTrackDesign->statistics.rideLength);
                    DrawTextEllipsised(rt, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
                    screenPos.y += kListRowHeight;
                }

                if (GetRideTypeDescriptor(_loadedTrackDesign->trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasGForces))
                {
                    // Maximum positive vertical Gs
                    ft = Formatter();
                    ft.Add<int32_t>(_loadedTrackDesign->statistics.maxPositiveVerticalG);
                    DrawTextBasic(rt, screenPos, STR_MAX_POSITIVE_VERTICAL_G, ft);
                    screenPos.y += kListRowHeight;

                    // Maximum negative vertical Gs
                    ft = Formatter();
                    ft.Add<int32_t>(_loadedTrackDesign->statistics.maxNegativeVerticalG);
                    DrawTextBasic(rt, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, ft);
                    screenPos.y += kListRowHeight;

                    // Maximum lateral Gs
                    ft = Formatter();
                    ft.Add<int32_t>(_loadedTrackDesign->statistics.maxLateralG);
                    DrawTextBasic(rt, screenPos, STR_MAX_LATERAL_G, ft);
                    screenPos.y += kListRowHeight;

                    if (_loadedTrackDesign->statistics.totalAirTime != 0)
                    {
                        // Total air time
                        ft = Formatter();
                        ft.Add<int32_t>(ToHumanReadableAirTime(_loadedTrackDesign->statistics.totalAirTime));
                        DrawTextBasic(rt, screenPos, STR_TOTAL_AIR_TIME, ft);
                        screenPos.y += kListRowHeight;
                    }
                }

                if (GetRideTypeDescriptor(_loadedTrackDesign->trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasDrops))
                {
                    // Drops
                    ft = Formatter();
                    ft.Add<uint16_t>(_loadedTrackDesign->statistics.drops);
                    DrawTextBasic(rt, screenPos, STR_DROPS, ft);
                    screenPos.y += kListRowHeight;

                    // Drop height is multiplied by 0.75
                    ft = Formatter();
                    ft.Add<uint16_t>((_loadedTrackDesign->statistics.highestDropHeight * 3) / 4);
                    DrawTextBasic(rt, screenPos, STR_HIGHEST_DROP_HEIGHT, ft);
                    screenPos.y += kListRowHeight;
                }

                if (_loadedTrackDesign->statistics.inversions != 0)
                {
                    ft = Formatter();
                    ft.Add<uint16_t>(_loadedTrackDesign->statistics.inversions);
                    DrawTextBasic(rt, screenPos, STR_INVERSIONS, ft);
                    screenPos.y += kListRowHeight;
                }

                screenPos.y += 4;
            }

            if (!_loadedTrackDesign->statistics.spaceRequired.IsNull())
            {
                // Space required
                ft = Formatter();
                ft.Add<uint16_t>(_loadedTrackDesign->statistics.spaceRequired.x);
                ft.Add<uint16_t>(_loadedTrackDesign->statistics.spaceRequired.y);
                DrawTextBasic(rt, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
                screenPos.y += kListRowHeight;
            }

            if (_loadedTrackDesign->gameStateData.cost != 0)
            {
                ft = Formatter();
                ft.Add<uint32_t>(_loadedTrackDesign->gameStateData.cost);
                DrawTextBasic(rt, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
            }
        }

        void onScrollDraw(const int32_t scrollIndex, RenderTarget& rt) override
        {
            auto paletteIndex = getColourMap(colours[0].colour).midLight;
            GfxClear(rt, paletteIndex);

            auto screenCoords = ScreenCoordsXY{ 0, 0 };
            size_t listIndex = 0;
            if (gLegacyScene == LegacyScene::trackDesignsManager)
            {
                if (_trackDesigns.empty())
                {
                    // No track designs
                    DrawTextBasic(rt, screenCoords - ScreenCoordsXY{ 0, 1 }, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE);
                    return;
                }
            }
            else
            {
                // Build custom track item
                StringId stringId;
                if (listIndex == static_cast<size_t>(selectedListItem))
                {
                    // Highlight
                    Rectangle::filter(
                        rt, { screenCoords, { width, screenCoords.y + kScrollableRowHeight - 1 } },
                        FilterPaletteID::paletteDarken1);
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                }
                else
                {
                    stringId = STR_BLACK_STRING;
                }

                auto ft = Formatter();
                ft.Add<StringId>(STR_BUILD_CUSTOM_DESIGN);
                DrawTextBasic(rt, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
                screenCoords.y += kScrollableRowHeight;
                listIndex++;
            }

            for (auto i : _filteredTrackIds)
            {
                if (screenCoords.y + kScrollableRowHeight >= rt.y && screenCoords.y < rt.y + rt.height)
                {
                    StringId stringId;
                    if (listIndex == static_cast<size_t>(selectedListItem))
                    {
                        // Highlight
                        Rectangle::filter(
                            rt, { screenCoords, { width, screenCoords.y + kScrollableRowHeight - 1 } },
                            FilterPaletteID::paletteDarken1);
                        stringId = STR_WINDOW_COLOUR_2_STRINGID;
                    }
                    else
                    {
                        stringId = STR_BLACK_STRING;
                    }

                    // Draw track name
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_TRACK_LIST_NAME_FORMAT);
                    ft.Add<const utf8*>(_trackDesigns[i].name.c_str());
                    DrawTextBasic(rt, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
                }

                screenCoords.y += kScrollableRowHeight;
                listIndex++;
            }
        }

        void SetIsBeingUpdated(const bool beingUpdated)
        {
            _selectedItemIsBeingUpdated = beingUpdated;
        }

        void ReloadTrackDesigns()
        {
            _reloadTrackDesigns = true;
        }
    };

    WindowBase* TrackListOpen(const RideSelection item)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseConstructionWindows();

        WindowFlags flags = {};
        ScreenCoordsXY screenPos{};
        if (gLegacyScene == LegacyScene::trackDesignsManager)
        {
            flags = { WindowFlag::autoPosition, WindowFlag::centreScreen };
        }
        else
        {
            screenPos = { 0, kTopToolbarHeight + 2 };
        }

        return windowMgr->Create<TrackListWindow>(WindowClass::trackDesignList, screenPos, kWindowSize, flags, item);
    }

    void WindowTrackDesignListReloadTracks()
    {
        auto* windowMgr = GetWindowManager();
        auto* trackListWindow = static_cast<TrackListWindow*>(windowMgr->FindByClass(WindowClass::trackDesignList));
        if (trackListWindow != nullptr)
        {
            trackListWindow->ReloadTrackDesigns();
        }
    }

    void WindowTrackDesignListSetBeingUpdated(const bool beingUpdated)
    {
        auto* windowMgr = GetWindowManager();
        auto* trackListWindow = static_cast<TrackListWindow*>(windowMgr->FindByClass(WindowClass::trackDesignList));
        if (trackListWindow != nullptr)
        {
            trackListWindow->SetIsBeingUpdated(beingUpdated);
        }
    }
} // namespace OpenRCT2::Ui::Windows
