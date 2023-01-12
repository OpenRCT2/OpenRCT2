/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/ride/Construction.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <vector>

static constexpr const StringId WINDOW_TITLE = STR_SELECT_DESIGN;
static constexpr const int32_t WH = 441;
static constexpr const int32_t WW = 600;
static constexpr const int32_t DEBUG_PATH_HEIGHT = 12;
static constexpr const int32_t ROTATE_AND_SCENERY_BUTTON_SIZE = 24;
static constexpr const int32_t WINDOW_PADDING = 5;

// clang-format off
enum {
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

validate_global_widx(WC_TRACK_DESIGN_LIST, WIDX_ROTATE);

static Widget window_track_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  4,  18}, {218,  13}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_SELECT_OTHER_RIDE                                       ),
    MakeWidget({  4,  32}, {124,  13}, WindowWidgetType::TextBox,     WindowColour::Secondary                                                              ),
    MakeWidget({130,  32}, { 92,  13}, WindowWidgetType::Button,       WindowColour::Primary  , STR_OBJECT_SEARCH_CLEAR                                     ),
    MakeWidget({  4,  46}, {218, 381}, WindowWidgetType::Scroll,       WindowColour::Primary  , SCROLL_VERTICAL,         STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP),
    MakeWidget({224,  18}, {372, 219}, WindowWidgetType::FlatBtn,      WindowColour::Primary                                                                ),
    MakeWidget({572, 405}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , ImageId(SPR_ROTATE_ARROW),        STR_ROTATE_90_TIP                  ),
    MakeWidget({572, 381}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , ImageId(SPR_SCENERY),             STR_TOGGLE_SCENERY_TIP             ),
    WIDGETS_END,
};

// clang-format on

constexpr uint16_t TRACK_DESIGN_INDEX_UNLOADED = UINT16_MAX;

RideSelection _window_track_list_item;

class TrackListWindow final : public Window
{
private:
    std::vector<TrackDesignFileRef> _trackDesigns;
    utf8 _filterString[USER_STRING_MAX_LENGTH];
    std::vector<uint16_t> _filteredTrackIds;
    uint16_t _loadedTrackDesignIndex;
    std::unique_ptr<TrackDesign> _loadedTrackDesign;
    std::vector<uint8_t> _trackDesignPreviewPixels;

    void FilterList()
    {
        _filteredTrackIds.clear();

        // Nothing to filter, so fill the list with all indices
        if (String::LengthOf(_filterString) == 0)
        {
            for (uint16_t i = 0; i < _trackDesigns.size(); i++)
                _filteredTrackIds.push_back(i);

            return;
        }

        // Convert filter to uppercase
        const auto filterStringUpper = String::ToUpper(_filterString);

        // Fill the set with indices for tracks that match the filter
        for (uint16_t i = 0; i < _trackDesigns.size(); i++)
        {
            const auto trackNameUpper = String::ToUpper(_trackDesigns[i].name);
            if (trackNameUpper.find(filterStringUpper) != std::string::npos)
            {
                _filteredTrackIds.push_back(i);
            }
        }
    }

    void SelectFromList(int32_t listIndex)
    {
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, this->windowPos.x + (this->width / 2));
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            if (listIndex == 0)
            {
                Close();
                ride_construct_new(_window_track_list_item);
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

        if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
        {
            gTrackDesignSceneryToggle = true;
        }

        uint16_t trackDesignIndex = _filteredTrackIds[listIndex];
        TrackDesignFileRef* tdRef = &_trackDesigns[trackDesignIndex];
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            auto intent = Intent(WindowClass::ManageTrackDesign);
            intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
            ContextOpenIntent(&intent);
        }
        else
        {
            if (_loadedTrackDesignIndex != TRACK_DESIGN_INDEX_UNLOADED
                && (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE))
            {
                ContextShowError(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE, {});
            }

            auto intent = Intent(WindowClass::TrackDesignPlace);
            intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
            ContextOpenIntent(&intent);
        }
    }

    int32_t GetListItemFromPosition(const ScreenCoordsXY& screenCoords)
    {
        size_t maxItems = _filteredTrackIds.size();
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Extra item: custom design
            maxItems++;
        }

        int32_t index = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        if (index < 0 || static_cast<uint32_t>(index) >= maxItems)
        {
            index = -1;
        }
        return index;
    }

    void LoadDesignsList(RideSelection item)
    {
        auto repo = OpenRCT2::GetContext()->GetTrackDesignRepository();
        std::string entryName;
        if (item.Type < 0x80)
        {
            if (GetRideTypeDescriptor(item.Type).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                entryName = get_ride_entry_name(item.EntryIndex);
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
            TrackDesignDrawPreview(_loadedTrackDesign.get(), _trackDesignPreviewPixels.data());
            return true;
        }
        return false;
    }

public:
    void OnOpen() override
    {
        String::Set(_filterString, sizeof(_filterString), "");
        window_track_list_widgets[WIDX_FILTER_STRING].string = _filterString;
        widgets = window_track_list_widgets;

        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            widgets[WIDX_BACK].type = WindowWidgetType::Empty;
        }
        else
        {
            widgets[WIDX_BACK].type = WindowWidgetType::TableHeader;
        }

        WindowInitScrollWidgets(*this);
        track_list.track_list_being_updated = false;
        track_list.reload_track_designs = false;
        // Start with first track highlighted
        selected_list_item = 0;
        if (_trackDesigns.size() != 0 && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            selected_list_item = 1;
        }
        gTrackDesignSceneryToggle = false;
        window_push_others_right(*this);
        _currentTrackPieceDirection = 2;
        _trackDesignPreviewPixels.resize(4 * TRACK_PREVIEW_IMAGE_SIZE);

        _loadedTrackDesign = nullptr;
        _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
    }

    void OnClose() override
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
        if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && gScreenAge != 0)
        {
            window_close_by_number(WindowClass::ManageTrackDesign, number);
            window_close_by_number(WindowClass::TrackDeletePrompt, number);
            Editor::LoadTrackManager();
        }
    }

    void OnMouseUp(const WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_ROTATE:
                _currentTrackPieceDirection++;
                _currentTrackPieceDirection %= 4;
                Invalidate();
                break;
            case WIDX_TOGGLE_SCENERY:
                gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
                _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
                Invalidate();
                break;
            case WIDX_BACK:
                Close();
                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                {
                    ContextOpenWindow(WindowClass::ConstructRide);
                }
                break;
            case WIDX_FILTER_STRING:
                window_start_textbox(
                    *this, widgetIndex, STR_STRING, _filterString, sizeof(_filterString)); // TODO check this out
                break;
            case WIDX_FILTER_CLEAR:
                // Keep the highlighted item selected
                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    if (selected_list_item != -1 && _filteredTrackIds.size() > static_cast<size_t>(selected_list_item))
                        selected_list_item = _filteredTrackIds[selected_list_item];
                    else
                        selected_list_item = -1;
                }
                else
                {
                    if (selected_list_item != 0)
                        selected_list_item = _filteredTrackIds[selected_list_item - 1] + 1;
                }

                String::Set(_filterString, sizeof(_filterString), "");
                FilterList();
                Invalidate();
                break;
        }
    }

    ScreenSize OnScrollGetSize(const int32_t scrollIndex) override
    {
        size_t numItems = _filteredTrackIds.size();
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Extra item: custom design
            numItems++;
        }
        int32_t scrollHeight = static_cast<int32_t>(numItems * SCROLLABLE_ROW_HEIGHT);

        return { width, scrollHeight };
    }

    void OnScrollMouseDown(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (!track_list.track_list_being_updated)
        {
            int32_t i = GetListItemFromPosition(screenCoords);
            if (i != -1)
            {
                SelectFromList(i);
            }
        }
    }

    void OnScrollMouseOver(const int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (!track_list.track_list_being_updated)
        {
            int32_t i = GetListItemFromPosition(screenCoords);
            if (i != -1 && selected_list_item != i)
            {
                selected_list_item = i;
                Invalidate();
            }
        }
    }

    void OnTextInput(const WidgetIndex widgetIndex, std::string_view text) override
    {
        if (widgetIndex != WIDX_FILTER_STRING || text.empty())
            return;

        if (String::Equals(_filterString, std::string(text).c_str()))
            return;

        String::Set(_filterString, sizeof(_filterString), std::string(text).c_str());

        FilterList();

        scrolls->v_top = 0;

        Invalidate();
    }

    void OnPrepareDraw() override
    {
        StringId stringId = STR_NONE;
        rct_ride_entry* entry = get_ride_entry(_window_track_list_item.EntryIndex);

        if (entry != nullptr)
        {
            RideNaming rideName = get_ride_naming(_window_track_list_item.Type, *entry);
            stringId = rideName.Name;
        }

        Formatter::Common().Add<StringId>(stringId);
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            window_track_list_widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS;
            window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
        }
        else
        {
            window_track_list_widgets[WIDX_TITLE].text = STR_SELECT_DESIGN;
            window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
        }

        if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || selected_list_item != 0)
        {
            pressed_widgets |= 1uLL << WIDX_TRACK_PREVIEW;
            disabled_widgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
            window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::FlatBtn;
            window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::FlatBtn;
            if (gTrackDesignSceneryToggle)
            {
                pressed_widgets &= ~(1uLL << WIDX_TOGGLE_SCENERY);
            }
            else
            {
                pressed_widgets |= (1uLL << WIDX_TOGGLE_SCENERY);
            }
        }
        else
        {
            pressed_widgets &= ~(1uLL << WIDX_TRACK_PREVIEW);
            disabled_widgets |= (1uLL << WIDX_TRACK_PREVIEW);
            window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
            window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::Empty;
        }

        // When debugging tools are on, shift everything up a bit to make room for displaying the path.
        const int32_t bottomMargin = gConfigGeneral.DebuggingTools ? (WINDOW_PADDING + DEBUG_PATH_HEIGHT) : WINDOW_PADDING;
        window_track_list_widgets[WIDX_TRACK_LIST].bottom = height - bottomMargin;
        window_track_list_widgets[WIDX_ROTATE].bottom = height - bottomMargin;
        window_track_list_widgets[WIDX_ROTATE].top = window_track_list_widgets[WIDX_ROTATE].bottom
            - ROTATE_AND_SCENERY_BUTTON_SIZE;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom = window_track_list_widgets[WIDX_ROTATE].top;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].top = window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom
            - ROTATE_AND_SCENERY_BUTTON_SIZE;
    }

    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            window_update_textbox_caret();
            widget_invalidate(*this, WIDX_FILTER_STRING); // TODO Check this
        }

        if (track_list.reload_track_designs)
        {
            LoadDesignsList(_window_track_list_item);
            selected_list_item = 0;
            Invalidate();
            track_list.reload_track_designs = false;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        int32_t listItemIndex = selected_list_item;
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            if (_trackDesigns.empty() || listItemIndex == -1)
                return;
        }
        else
        {
            if (listItemIndex == 0)
                return;

            // Because the first item in the list is "Build a custom design", lower the index by one
            listItemIndex--;
        }

        int32_t trackIndex = _filteredTrackIds[listItemIndex];

        // Track preview
        auto& tdWidget = widgets[WIDX_TRACK_PREVIEW];
        int32_t colour = ColourMapA[colours[0]].darkest;
        u8string path = _trackDesigns[trackIndex].path;

        // Show track file path (in debug mode)
        if (gConfigGeneral.DebuggingTools)
        {
            utf8 pathBuffer[MAX_PATH];
            const utf8* pathPtr = pathBuffer;
            shorten_path(pathBuffer, sizeof(pathBuffer), path.c_str(), width, FontStyle::Medium);
            auto ft = Formatter();
            ft.Add<utf8*>(pathPtr);
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ 0, height - DEBUG_PATH_HEIGHT - 3 }, STR_STRING, ft,
                { colours[1] }); // TODO Check dpi
        }

        auto screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.top + 1 };
        gfx_fill_rect(&dpi, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour); // TODO Check dpi

        if (_loadedTrackDesignIndex != trackIndex)
        {
            if (LoadDesignPreview(path))
            {
                _loadedTrackDesignIndex = trackIndex;
            }
            else
            {
                _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
            }
        }

        if (!_loadedTrackDesign)
        {
            return;
        }

        auto trackPreview = screenPos;
        screenPos = windowPos + ScreenCoordsXY{ tdWidget.midX(), tdWidget.midY() };

        rct_g1_element g1temp = {};
        g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
        g1temp.width = 370;
        g1temp.height = 217;
        g1temp.flags = G1_FLAG_HAS_TRANSPARENCY;
        gfx_set_g1_element(SPR_TEMP, &g1temp);
        drawing_engine_invalidate_image(SPR_TEMP);
        gfx_draw_sprite(&dpi, ImageId(SPR_TEMP), trackPreview);

        screenPos.y = windowPos.y + tdWidget.bottom - 12;

        // Warnings
        if ((_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE)
            && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            // Vehicle design not available
            DrawTextEllipsised(&dpi, screenPos, 368, STR_VEHICLE_DESIGN_UNAVAILABLE, {}, { TextAlignment::CENTRE });
            screenPos.y -= SCROLLABLE_ROW_HEIGHT;
        }

        if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
        {
            if (!gTrackDesignSceneryToggle)
            {
                // Scenery not available
                DrawTextEllipsised(
                    &dpi, screenPos, 368, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::CENTRE });
                screenPos.y -= SCROLLABLE_ROW_HEIGHT;
            }
        }

        // Track design name
        auto ft = Formatter();
        ft.Add<const utf8*>(_trackDesigns[trackIndex].name.c_str());
        DrawTextEllipsised(&dpi, screenPos, 368, STR_TRACK_PREVIEW_NAME_FORMAT, ft, { TextAlignment::CENTRE });

        // Information
        screenPos = windowPos + ScreenCoordsXY{ tdWidget.left + 1, tdWidget.bottom + 2 };

        // Stats
        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->excitement * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->intensity * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_INTENSITY_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<fixed32_2dp>(_loadedTrackDesign->nausea * 10);
        DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_NAUSEA_RATING, ft);
        screenPos.y += LIST_ROW_HEIGHT + 4;

        // Information for tracked rides.
        if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
        {
            const auto& rtd = GetRideTypeDescriptor(_loadedTrackDesign->type);
            if (!rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                if (_loadedTrackDesign->type == RIDE_TYPE_MINI_GOLF)
                {
                    // Holes
                    ft = Formatter();
                    ft.Add<uint16_t>(_loadedTrackDesign->holes & 0x1F);
                    DrawTextBasic(&dpi, screenPos, STR_HOLES, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
                else
                {
                    // Maximum speed
                    ft = Formatter();
                    ft.Add<uint16_t>(((_loadedTrackDesign->max_speed << 16) * 9) >> 18);
                    DrawTextBasic(&dpi, screenPos, STR_MAX_SPEED, ft);
                    screenPos.y += LIST_ROW_HEIGHT;

                    // Average speed
                    ft = Formatter();
                    ft.Add<uint16_t>(((_loadedTrackDesign->average_speed << 16) * 9) >> 18);
                    DrawTextBasic(&dpi, screenPos, STR_AVERAGE_SPEED, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }

                // Ride length
                ft = Formatter();
                ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                ft.Add<uint16_t>(_loadedTrackDesign->ride_length);
                DrawTextEllipsised(&dpi, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }

            if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
            {
                // Maximum positive vertical Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_positive_vertical_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_POSITIVE_VERTICAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Maximum negative vertical Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_negative_vertical_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Maximum lateral Gs
                ft = Formatter();
                ft.Add<int32_t>(_loadedTrackDesign->max_lateral_g * 32);
                DrawTextBasic(&dpi, screenPos, STR_MAX_LATERAL_G, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                if (_loadedTrackDesign->total_air_time != 0)
                {
                    // Total air time
                    ft = Formatter();
                    ft.Add<int32_t>(_loadedTrackDesign->total_air_time * 25);
                    DrawTextBasic(&dpi, screenPos, STR_TOTAL_AIR_TIME, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
            }

            if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
            {
                // Drops
                ft = Formatter();
                ft.Add<uint16_t>(_loadedTrackDesign->drops & 0x3F);
                DrawTextBasic(&dpi, screenPos, STR_DROPS, ft);
                screenPos.y += LIST_ROW_HEIGHT;

                // Drop height is multiplied by 0.75
                ft = Formatter();
                ft.Add<uint16_t>((_loadedTrackDesign->highest_drop_height * 3) / 4);
                DrawTextBasic(&dpi, screenPos, STR_HIGHEST_DROP_HEIGHT, ft);
                screenPos.y += LIST_ROW_HEIGHT;
            }

            if (_loadedTrackDesign->type != RIDE_TYPE_MINI_GOLF)
            {
                uint16_t inversions = _loadedTrackDesign->inversions & 0x1F;
                if (inversions != 0)
                {
                    ft = Formatter();
                    ft.Add<uint16_t>(inversions);
                    // Inversions
                    DrawTextBasic(&dpi, screenPos, STR_INVERSIONS, ft);
                    screenPos.y += LIST_ROW_HEIGHT;
                }
            }
            screenPos.y += 4;
        }

        if (_loadedTrackDesign->space_required_x != 0xFF)
        {
            // Space required
            ft = Formatter();
            ft.Add<uint16_t>(_loadedTrackDesign->space_required_x);
            ft.Add<uint16_t>(_loadedTrackDesign->space_required_y);
            DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }

        if (_loadedTrackDesign->cost != 0)
        {
            ft = Formatter();
            ft.Add<uint32_t>(_loadedTrackDesign->cost);
            DrawTextBasic(&dpi, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
        }
    }

    void OnScrollDraw(const int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        uint8_t paletteIndex = ColourMapA[colours[0]].mid_light;
        gfx_clear(&dpi, paletteIndex);

        auto screenCoords = ScreenCoordsXY{ 0, 0 };
        size_t listIndex = 0;
        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            if (_trackDesigns.empty())
            {
                // No track designs
                DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE);
                return;
            }
        }
        else
        {
            // Build custom track item
            StringId stringId;
            if (listIndex == static_cast<size_t>(selected_list_item))
            {
                // Highlight
                gfx_filter_rect(
                    &dpi, { screenCoords, { width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } },
                    FilterPaletteID::PaletteDarken1);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else
            {
                stringId = STR_BLACK_STRING;
            }

            auto ft = Formatter();
            ft.Add<StringId>(STR_BUILD_CUSTOM_DESIGN);
            DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
            listIndex++;
        }

        for (auto i : _filteredTrackIds)
        {
            if (screenCoords.y + SCROLLABLE_ROW_HEIGHT >= dpi.y && screenCoords.y < dpi.y + dpi.height)
            {
                StringId stringId;
                if (listIndex == static_cast<size_t>(selected_list_item))
                {
                    // Highlight
                    gfx_filter_rect(
                        &dpi, { screenCoords, { width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1 } },
                        FilterPaletteID::PaletteDarken1);
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
                DrawTextBasic(&dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
            }

            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
            listIndex++;
        }
    }

    bool SetRideSelection(const RideSelection item)
    {
        _window_track_list_item = item;
        LoadDesignsList(item);
        return true;
    }
};

rct_window* WindowTrackListOpen(const RideSelection item)
{
    window_close_construction_windows();
    ScreenCoordsXY screenPos{};
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        screenPos = { screenWidth / 2 - 300, std::max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200) };
    }
    else
    {
        screenPos = { 0, TOP_TOOLBAR_HEIGHT + 2 };
    }
    auto* w = WindowCreate<TrackListWindow>(WindowClass::TrackDesignList, WW, WH, 0);
    w->SetRideSelection(item);
    return w;
}
