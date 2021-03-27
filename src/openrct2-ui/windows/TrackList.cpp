/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <vector>

static constexpr const rct_string_id WINDOW_TITLE = STR_SELECT_DESIGN;
static constexpr const int32_t WH = 431;
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

static rct_widget window_track_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  4,  18}, {218,  13}, WindowWidgetType::TableHeader, WindowColour::Primary  , STR_SELECT_OTHER_RIDE                                       ),
    MakeWidget({  4,  32}, {124,  13}, WindowWidgetType::TextBox,     WindowColour::Secondary                                                              ),
    MakeWidget({130,  32}, { 92,  13}, WindowWidgetType::Button,       WindowColour::Primary  , STR_OBJECT_SEARCH_CLEAR                                     ),
    MakeWidget({  4,  46}, {218, 381}, WindowWidgetType::Scroll,       WindowColour::Primary  , SCROLL_VERTICAL,         STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP),
    MakeWidget({224,  18}, {372, 219}, WindowWidgetType::FlatBtn,      WindowColour::Primary                                                                ),
    MakeWidget({572, 405}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , SPR_ROTATE_ARROW,        STR_ROTATE_90_TIP                  ),
    MakeWidget({572, 381}, { ROTATE_AND_SCENERY_BUTTON_SIZE, ROTATE_AND_SCENERY_BUTTON_SIZE}, WindowWidgetType::FlatBtn,      WindowColour::Primary  , SPR_SCENERY,             STR_TOGGLE_SCENERY_TIP             ),
    { WIDGETS_END },
};

static void window_track_list_close(rct_window *w);
static void window_track_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_track_list_update(rct_window *w);
static void window_track_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_track_list_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_track_list_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_track_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_track_list_invalidate(rct_window *w);
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_track_list_events([](auto& events)
{
    events.close = &window_track_list_close;
    events.mouse_up = &window_track_list_mouseup;
    events.update = &window_track_list_update;
    events.get_scroll_size = &window_track_list_scrollgetsize;
    events.scroll_mousedown = &window_track_list_scrollmousedown;
    events.scroll_mouseover = &window_track_list_scrollmouseover;
    events.text_input = &window_track_list_textinput;
    events.invalidate = &window_track_list_invalidate;
    events.paint = &window_track_list_paint;
    events.scroll_paint = &window_track_list_scrollpaint;
});
// clang-format on

constexpr uint16_t TRACK_DESIGN_INDEX_UNLOADED = UINT16_MAX;

RideSelection _window_track_list_item;

static std::vector<track_design_file_ref> _trackDesigns;
static utf8 _filterString[USER_STRING_MAX_LENGTH];
static std::vector<uint16_t> _filteredTrackIds;
static uint16_t _loadedTrackDesignIndex;
static std::unique_ptr<TrackDesign> _loadedTrackDesign;
static std::vector<uint8_t> _trackDesignPreviewPixels;

static void track_list_load_designs(RideSelection item);
static bool track_list_load_design_for_preview(utf8* path);

/**
 *
 *  rct2: 0x006CF1A2
 */
rct_window* window_track_list_open(RideSelection item)
{
    window_close_construction_windows();
    _window_track_list_item = item;

    String::Set(_filterString, sizeof(_filterString), "");
    track_list_load_designs(item);

    ScreenCoordsXY screenPos{};
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        screenPos = { screenWidth / 2 - 300, std::max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200) };
    }
    else
    {
        screenPos = { 0, TOP_TOOLBAR_HEIGHT + 2 };
    }

    rct_window* w = WindowCreate(screenPos, 600, WH, &window_track_list_events, WC_TRACK_DESIGN_LIST, 0);

    window_track_list_widgets[WIDX_FILTER_STRING].string = _filterString;
    w->widgets = window_track_list_widgets;
    w->enabled_widgets = (1ULL << WIDX_CLOSE) | (1ULL << WIDX_BACK) | (1ULL << WIDX_FILTER_STRING) | (1ULL << WIDX_FILTER_CLEAR)
        | (1ULL << WIDX_ROTATE) | (1ULL << WIDX_TOGGLE_SCENERY);

    WindowInitScrollWidgets(w);
    w->track_list.track_list_being_updated = false;
    w->track_list.reload_track_designs = false;

    // Start with first track highlighted
    w->selected_list_item = 0;
    if (_trackDesigns.size() != 0 && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        w->selected_list_item = 1;
    }

    gTrackDesignSceneryToggle = false;
    window_push_others_right(w);
    _currentTrackPieceDirection = 2;

    _trackDesignPreviewPixels.resize(4 * TRACK_PREVIEW_IMAGE_SIZE);

    _loadedTrackDesign = nullptr;
    _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;

    return w;
}

static void window_track_list_filter_list()
{
    _filteredTrackIds.clear();

    // Nothing to filter, so fill the list with all indices
    if (String::LengthOf(_filterString) == 0)
    {
        for (uint16_t i = 0; i < _trackDesigns.size(); i++)
            _filteredTrackIds.push_back(i);

        return;
    }

    // Convert filter to lowercase
    utf8 filterStringLower[sizeof(_filterString)];
    String::Set(filterStringLower, sizeof(filterStringLower), _filterString);
    for (int32_t i = 0; filterStringLower[i] != '\0'; i++)
        filterStringLower[i] = static_cast<utf8>(tolower(filterStringLower[i]));

    // Fill the set with indices for tracks that match the filter
    for (uint16_t i = 0; i < _trackDesigns.size(); i++)
    {
        utf8 trackNameLower[USER_STRING_MAX_LENGTH];
        String::Set(trackNameLower, sizeof(trackNameLower), _trackDesigns[i].name);
        for (int32_t j = 0; trackNameLower[j] != '\0'; j++)
            trackNameLower[j] = static_cast<utf8>(tolower(trackNameLower[j]));

        if (strstr(trackNameLower, filterStringLower) != nullptr)
        {
            _filteredTrackIds.push_back(i);
        }
    }
}

/**
 *
 *  rct2: 0x006CFD76
 */
static void window_track_list_close(rct_window* w)
{
    // Dispose track design and preview
    _loadedTrackDesign = nullptr;
    _trackDesignPreviewPixels.clear();
    _trackDesignPreviewPixels.shrink_to_fit();

    // Dispose track list
    for (auto& trackDesign : _trackDesigns)
    {
        free(trackDesign.name);
        free(trackDesign.path);
    }
    _trackDesigns.clear();

    // If gScreenAge is zero, we're already in the process
    // of loading the track manager, so we shouldn't try
    // to do it again. Otherwise, this window will get
    // another close signal from the track manager load function,
    // try to load the track manager again, and an infinite loop will result.
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && gScreenAge != 0)
    {
        window_close_by_number(WC_MANAGE_TRACK_DESIGN, w->number);
        window_close_by_number(WC_TRACK_DELETE_PROMPT, w->number);
        Editor::LoadTrackManager();
    }
}

/**
 *
 *  rct2: 0x006CFB82
 */
static void window_track_list_select(rct_window* w, int32_t listIndex)
{
    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, w->windowPos.x + (w->width / 2));
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        if (listIndex == 0)
        {
            window_close(w);
            ride_construct_new(_window_track_list_item);
            return;
        }
        listIndex--;
    }

    // Displays a message if the ride can't load, fix #4080
    if (_loadedTrackDesign == nullptr)
    {
        context_show_error(STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_TRACK_LOAD_FAILED_ERROR, {});
        return;
    }

    if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
    {
        gTrackDesignSceneryToggle = true;
    }

    uint16_t trackDesignIndex = _filteredTrackIds[listIndex];
    track_design_file_ref* tdRef = &_trackDesigns[trackDesignIndex];
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        auto intent = Intent(WC_MANAGE_TRACK_DESIGN);
        intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
        context_open_intent(&intent);
    }
    else
    {
        if (_loadedTrackDesignIndex != TRACK_DESIGN_INDEX_UNLOADED
            && (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE))
        {
            context_show_error(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE, {});
        }

        auto intent = Intent(WC_TRACK_DESIGN_PLACE);
        intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
        context_open_intent(&intent);
    }
}

static int32_t window_track_list_get_list_item_index_from_position(const ScreenCoordsXY& screenCoords)
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

/**
 *
 *  rct2: 0x006CFA31
 */
static void window_track_list_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_ROTATE:
            _currentTrackPieceDirection++;
            _currentTrackPieceDirection %= 4;
            w->Invalidate();
            break;
        case WIDX_TOGGLE_SCENERY:
            gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
            _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
            w->Invalidate();
            break;
        case WIDX_BACK:
            window_close(w);
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
            {
                context_open_window(WC_CONSTRUCT_RIDE);
            }
            break;
        case WIDX_FILTER_STRING:
            window_start_textbox(w, widgetIndex, STR_STRING, _filterString, sizeof(_filterString));
            break;
        case WIDX_FILTER_CLEAR:
            // Keep the highlighted item selected
            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
            {
                if (w->selected_list_item != -1 && _filteredTrackIds.size() > static_cast<size_t>(w->selected_list_item))
                    w->selected_list_item = _filteredTrackIds[w->selected_list_item];
                else
                    w->selected_list_item = -1;
            }
            else
            {
                if (w->selected_list_item != 0)
                    w->selected_list_item = _filteredTrackIds[w->selected_list_item - 1] + 1;
            }

            String::Set(_filterString, sizeof(_filterString), "");
            window_track_list_filter_list();
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x006CFAB0
 */
static void window_track_list_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    size_t numItems = _filteredTrackIds.size();
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        // Extra item: custom design
        numItems++;
    }

    *height = static_cast<int32_t>(numItems * SCROLLABLE_ROW_HEIGHT);
}

/**
 *
 *  rct2: 0x006CFB39
 */
static void window_track_list_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (!w->track_list.track_list_being_updated)
    {
        int32_t i = window_track_list_get_list_item_index_from_position(screenCoords);
        if (i != -1)
        {
            window_track_list_select(w, i);
        }
    }
}

/**
 *
 *  rct2: 0x006CFAD7
 */
static void window_track_list_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    if (!w->track_list.track_list_being_updated)
    {
        int32_t i = window_track_list_get_list_item_index_from_position(screenCoords);
        if (i != -1 && w->selected_list_item != i)
        {
            w->selected_list_item = i;
            w->Invalidate();
        }
    }
}

static void window_track_list_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    if (widgetIndex != WIDX_FILTER_STRING || text == nullptr)
        return;

    if (String::Equals(_filterString, text))
        return;

    String::Set(_filterString, sizeof(_filterString), text);

    window_track_list_filter_list();

    w->scrolls->v_top = 0;

    w->Invalidate();
}

static void window_track_list_update(rct_window* w)
{
    if (gCurrentTextBox.window.classification == w->classification && gCurrentTextBox.window.number == w->number)
    {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_FILTER_STRING);
    }

    if (w->track_list.reload_track_designs)
    {
        track_list_load_designs(_window_track_list_item);
        w->selected_list_item = 0;
        w->Invalidate();
        w->track_list.reload_track_designs = false;
    }
}

/**
 *
 *  rct2: 0x006CF2D6
 */
static void window_track_list_invalidate(rct_window* w)
{
    rct_string_id stringId = STR_NONE;
    rct_ride_entry* entry = get_ride_entry(_window_track_list_item.EntryIndex);

    if (entry != nullptr)
    {
        RideNaming rideName = get_ride_naming(_window_track_list_item.Type, entry);
        stringId = rideName.Name;
    }

    Formatter::Common().Add<rct_string_id>(stringId);
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

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || w->selected_list_item != 0)
    {
        w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
        w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
        window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::FlatBtn;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::FlatBtn;
        if (gTrackDesignSceneryToggle)
        {
            w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
        }
        else
        {
            w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
        }
    }
    else
    {
        w->pressed_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
        w->disabled_widgets |= (1 << WIDX_TRACK_PREVIEW);
        window_track_list_widgets[WIDX_ROTATE].type = WindowWidgetType::Empty;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WindowWidgetType::Empty;
    }

    // When debugging tools are on, shift everything up a bit to make room for displaying the path.
    const int32_t bottomMargin = gConfigGeneral.debugging_tools ? (WINDOW_PADDING + DEBUG_PATH_HEIGHT) : WINDOW_PADDING;
    window_track_list_widgets[WIDX_TRACK_LIST].bottom = w->height - bottomMargin;
    window_track_list_widgets[WIDX_ROTATE].bottom = w->height - bottomMargin;
    window_track_list_widgets[WIDX_ROTATE].top = window_track_list_widgets[WIDX_ROTATE].bottom - ROTATE_AND_SCENERY_BUTTON_SIZE;
    window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom = window_track_list_widgets[WIDX_ROTATE].top;
    window_track_list_widgets[WIDX_TOGGLE_SCENERY].top = window_track_list_widgets[WIDX_TOGGLE_SCENERY].bottom
        - ROTATE_AND_SCENERY_BUTTON_SIZE;
}

/**
 *
 *  rct2: 0x006CF387
 */
static void window_track_list_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    int32_t listItemIndex = w->selected_list_item;
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
    int32_t colour;
    rct_widget* widget = &window_track_list_widgets[WIDX_TRACK_PREVIEW];
    colour = ColourMapA[w->colours[0]].darkest;
    utf8* path = _trackDesigns[trackIndex].path;

    // Show track file path (in debug mode)
    if (gConfigGeneral.debugging_tools)
    {
        utf8 pathBuffer[MAX_PATH];
        const utf8* pathPtr = pathBuffer;
        shorten_path(pathBuffer, sizeof(pathBuffer), path, w->width, FontSpriteBase::MEDIUM);
        auto ft = Formatter();
        ft.Add<utf8*>(pathPtr);
        DrawTextBasic(
            dpi, w->windowPos + ScreenCoordsXY{ 0, w->height - DEBUG_PATH_HEIGHT - 3 }, STR_STRING, ft, { w->colours[1] });
    }

    auto screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 };
    gfx_fill_rect(dpi, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour);

    if (_loadedTrackDesignIndex != trackIndex)
    {
        if (track_list_load_design_for_preview(path))
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
    screenPos = w->windowPos + ScreenCoordsXY{ widget->midX(), widget->midY() };

    rct_g1_element g1temp = {};
    g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
    g1temp.width = 370;
    g1temp.height = 217;
    g1temp.flags = G1_FLAG_BMP;
    gfx_set_g1_element(SPR_TEMP, &g1temp);
    drawing_engine_invalidate_image(SPR_TEMP);
    gfx_draw_sprite(dpi, ImageId(SPR_TEMP), trackPreview);

    screenPos.y = w->windowPos.y + widget->bottom - 12;

    // Warnings
    if ((_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE)
        && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
    {
        // Vehicle design not available
        DrawTextEllipsised(dpi, screenPos, 368, STR_VEHICLE_DESIGN_UNAVAILABLE, {}, { TextAlignment::CENTRE });
        screenPos.y -= SCROLLABLE_ROW_HEIGHT;
    }

    if (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)
    {
        if (!gTrackDesignSceneryToggle)
        {
            // Scenery not available
            DrawTextEllipsised(
                dpi, screenPos, 368, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::CENTRE });
            screenPos.y -= SCROLLABLE_ROW_HEIGHT;
        }
    }

    // Track design name
    auto ft = Formatter();
    ft.Add<utf8*>(_trackDesigns[trackIndex].name);
    DrawTextEllipsised(dpi, screenPos, 368, STR_TRACK_PREVIEW_NAME_FORMAT, ft, { TextAlignment::CENTRE });

    // Information
    screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->bottom + 2 };

    // Stats
    fixed32_2dp rating = _loadedTrackDesign->excitement * 10;
    DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, &rating);
    screenPos.y += LIST_ROW_HEIGHT;

    rating = _loadedTrackDesign->intensity * 10;
    DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_INTENSITY_RATING, &rating);
    screenPos.y += LIST_ROW_HEIGHT;

    rating = _loadedTrackDesign->nausea * 10;
    DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_NAUSEA_RATING, &rating);
    screenPos.y += LIST_ROW_HEIGHT + 4;

    // Information for tracked rides.
    if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_TRACK))
    {
        if (_loadedTrackDesign->type != RIDE_TYPE_MAZE)
        {
            if (_loadedTrackDesign->type == RIDE_TYPE_MINI_GOLF)
            {
                // Holes
                uint16_t holes = _loadedTrackDesign->holes & 0x1F;
                DrawTextBasic(dpi, screenPos, STR_HOLES, &holes);
                screenPos.y += LIST_ROW_HEIGHT;
            }
            else
            {
                // Maximum speed
                uint16_t speed = ((_loadedTrackDesign->max_speed << 16) * 9) >> 18;
                DrawTextBasic(dpi, screenPos, STR_MAX_SPEED, &speed);
                screenPos.y += LIST_ROW_HEIGHT;

                // Average speed
                speed = ((_loadedTrackDesign->average_speed << 16) * 9) >> 18;
                DrawTextBasic(dpi, screenPos, STR_AVERAGE_SPEED, &speed);
                screenPos.y += LIST_ROW_HEIGHT;
            }

            // Ride length
            ft = Formatter();
            ft.Add<rct_string_id>(STR_RIDE_LENGTH_ENTRY);
            ft.Add<uint16_t>(_loadedTrackDesign->ride_length);
            DrawTextEllipsised(dpi, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
            screenPos.y += LIST_ROW_HEIGHT;
        }

        if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            // Maximum positive vertical Gs
            int32_t gForces = _loadedTrackDesign->max_positive_vertical_g * 32;
            DrawTextBasic(dpi, screenPos, STR_MAX_POSITIVE_VERTICAL_G, &gForces);
            screenPos.y += LIST_ROW_HEIGHT;

            // Maximum negative vertical Gs
            gForces = _loadedTrackDesign->max_negative_vertical_g * 32;
            DrawTextBasic(dpi, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, &gForces);
            screenPos.y += LIST_ROW_HEIGHT;

            // Maximum lateral Gs
            gForces = _loadedTrackDesign->max_lateral_g * 32;
            DrawTextBasic(dpi, screenPos, STR_MAX_LATERAL_G, &gForces);
            screenPos.y += LIST_ROW_HEIGHT;

            if (_loadedTrackDesign->total_air_time != 0)
            {
                // Total air time
                int32_t airTime = _loadedTrackDesign->total_air_time * 25;
                DrawTextBasic(dpi, screenPos, STR_TOTAL_AIR_TIME, &airTime);
                screenPos.y += LIST_ROW_HEIGHT;
            }
        }

        if (GetRideTypeDescriptor(_loadedTrackDesign->type).HasFlag(RIDE_TYPE_FLAG_HAS_DROPS))
        {
            // Drops
            uint16_t drops = _loadedTrackDesign->drops & 0x3F;
            DrawTextBasic(dpi, screenPos, STR_DROPS, &drops);
            screenPos.y += LIST_ROW_HEIGHT;

            // Drop height is multiplied by 0.75
            uint16_t highestDropHeight = (_loadedTrackDesign->highest_drop_height * 3) / 4;
            DrawTextBasic(dpi, screenPos, STR_HIGHEST_DROP_HEIGHT, &highestDropHeight);
            screenPos.y += LIST_ROW_HEIGHT;
        }

        if (_loadedTrackDesign->type != RIDE_TYPE_MINI_GOLF)
        {
            uint16_t inversions = _loadedTrackDesign->inversions & 0x1F;
            if (inversions != 0)
            {
                // Inversions
                DrawTextBasic(dpi, screenPos, STR_INVERSIONS, &inversions);
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
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
        screenPos.y += LIST_ROW_HEIGHT;
    }

    if (_loadedTrackDesign->cost != 0)
    {
        ft = Formatter();
        ft.Add<uint32_t>(_loadedTrackDesign->cost);
        DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
    }
}

/**
 *
 *  rct2: 0x006CF8CD
 */
static void window_track_list_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    uint8_t paletteIndex = ColourMapA[w->colours[0]].mid_light;
    gfx_clear(dpi, paletteIndex);

    auto screenCoords = ScreenCoordsXY{ 0, 0 };
    size_t listIndex = 0;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        if (_trackDesigns.empty())
        {
            // No track designs
            DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE);
            return;
        }
    }
    else
    {
        // Build custom track item
        rct_string_id stringId;
        if (listIndex == static_cast<size_t>(w->selected_list_item))
        {
            // Highlight
            gfx_filter_rect(
                dpi, screenCoords.x, screenCoords.y, w->width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1,
                FilterPaletteID::PaletteDarken1);
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
        }
        else
        {
            stringId = STR_BLACK_STRING;
        }

        rct_string_id stringId2 = STR_BUILD_CUSTOM_DESIGN;
        DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, &stringId2);
        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
        listIndex++;
    }

    for (auto i : _filteredTrackIds)
    {
        if (screenCoords.y + SCROLLABLE_ROW_HEIGHT >= dpi->y && screenCoords.y < dpi->y + dpi->height)
        {
            rct_string_id stringId;
            if (listIndex == static_cast<size_t>(w->selected_list_item))
            {
                // Highlight
                gfx_filter_rect(
                    dpi, screenCoords.x, screenCoords.y, w->width, screenCoords.y + SCROLLABLE_ROW_HEIGHT - 1,
                    FilterPaletteID::PaletteDarken1);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else
            {
                stringId = STR_BLACK_STRING;
            }

            // Draw track name
            auto ft = Formatter();
            ft.Add<rct_string_id>(STR_TRACK_LIST_NAME_FORMAT);
            ft.Add<utf8*>(_trackDesigns[i].name);
            DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 1 }, stringId, ft);
        }

        screenCoords.y += SCROLLABLE_ROW_HEIGHT;
        listIndex++;
    }
}

static void track_list_load_designs(RideSelection item)
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

    window_track_list_filter_list();
}

static bool track_list_load_design_for_preview(utf8* path)
{
    _loadedTrackDesign = track_design_open(path);
    if (_loadedTrackDesign != nullptr)
    {
        track_design_draw_preview(_loadedTrackDesign.get(), _trackDesignPreviewPixels.data());
        return true;
    }
    return false;
}
