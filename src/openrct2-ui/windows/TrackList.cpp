/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <vector>
#include <openrct2/audio/audio.h>
#include <openrct2/Context.h>
#include <openrct2/core/Math.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/Editor.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/rct1/RCT1.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/ride/TrackDesign.h>

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
    { WWT_FRAME,            0,  0,      599,    0,      430,    0xFFFFFFFF,                 STR_NONE                            },
    { WWT_CAPTION,          0,  1,      598,    1,      14,     STR_SELECT_DESIGN,          STR_WINDOW_TITLE_TIP                },
    { WWT_CLOSEBOX,         0,  587,    597,    2,      13,     STR_CLOSE_X,                STR_CLOSE_WINDOW_TIP                },
    { WWT_TABLE_HEADER,     0,  4,      221,    18,     30,     STR_SELECT_OTHER_RIDE,      STR_NONE                            },
    { WWT_TEXT_BOX,         1,  4,      127,    32,     44,     STR_NONE,                   STR_NONE                            },
    { WWT_BUTTON,           0,  130,    221,    32,     44,     STR_OBJECT_SEARCH_CLEAR,    STR_NONE                            },
    { WWT_SCROLL,           0,  4,      221,    46,     426,    SCROLL_VERTICAL,            STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP },
    { WWT_FLATBTN,          0,  224,    595,    18,     236,    0xFFFFFFFF,                 STR_NONE                            },
    { WWT_FLATBTN,          0,  574,    597,    405,    428,    SPR_ROTATE_ARROW,           STR_ROTATE_90_TIP                   },
    { WWT_FLATBTN,          0,  574,    597,    381,    404,    SPR_SCENERY,                STR_TOGGLE_SCENERY_TIP              },
    { WIDGETS_END },
};

static void window_track_list_close(rct_window *w);
static void window_track_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_track_list_update(rct_window *w);
static void window_track_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_track_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_track_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y);
static void window_track_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_track_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId);
static void window_track_list_invalidate(rct_window *w);
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_track_list_events = {
    window_track_list_close,
    window_track_list_mouseup,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_track_list_update,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_track_list_scrollgetsize,
    window_track_list_scrollmousedown,
    nullptr,
    window_track_list_scrollmouseover,
    window_track_list_textinput,
    nullptr,
    nullptr,
    window_track_list_tooltip,
    nullptr,
    nullptr,
    window_track_list_invalidate,
    window_track_list_paint,
    window_track_list_scrollpaint
};
// clang-format on

#define TRACK_DESIGN_INDEX_UNLOADED UINT16_MAX

ride_list_item _window_track_list_item;

static std::vector<track_design_file_ref> _trackDesigns;
static utf8                    _filterString[USER_STRING_MAX_LENGTH];
static std::vector<uint16_t>     _filteredTrackIds;
static uint16_t                  _loadedTrackDesignIndex;
static rct_track_td6         * _loadedTrackDesign;
static std::vector<uint8_t>      _trackDesignPreviewPixels;

static void track_list_load_designs(ride_list_item item);
static bool track_list_load_design_for_preview(utf8 *path);

/**
 *
 *  rct2: 0x006CF1A2
 */
rct_window * window_track_list_open(ride_list_item item)
{
    window_close_construction_windows();
    _window_track_list_item = item;

    String::Set(_filterString, sizeof(_filterString), "");
    track_list_load_designs(item);

    int32_t x, y;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        int32_t screenWidth = context_get_width();
        int32_t screenHeight = context_get_height();
        x = screenWidth / 2 - 300;
        y = std::max(TOP_TOOLBAR_HEIGHT + 1, screenHeight / 2 - 200);
    }
    else
    {
        x = 0;
        y = TOP_TOOLBAR_HEIGHT + 2;
    }

    rct_window *w = window_create(
        x,
        y,
        600,
        432,
        &window_track_list_events,
        WC_TRACK_DESIGN_LIST,
        0
    );

    window_track_list_widgets[WIDX_FILTER_STRING].string = _filterString;
    w->widgets = window_track_list_widgets;
    w->enabled_widgets =
        (1ULL << WIDX_CLOSE) |
        (1ULL << WIDX_BACK) |
        (1ULL << WIDX_FILTER_STRING) |
        (1ULL << WIDX_FILTER_CLEAR) |
        (1ULL << WIDX_ROTATE) |
        (1ULL << WIDX_TOGGLE_SCENERY);

    window_init_scroll_widgets(w);
    w->track_list.track_list_being_updated = false;
    w->track_list.reload_track_designs = false;

    // Start with first track highlighted
    w->selected_list_item = 0;
    if (_trackDesigns.size() != 0 && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
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
        filterStringLower[i] = (utf8)tolower(filterStringLower[i]);

    // Fill the set with indices for tracks that match the filter
    for (uint16_t i = 0; i < _trackDesigns.size(); i++)
    {
        utf8 trackNameLower[USER_STRING_MAX_LENGTH];
        String::Set(trackNameLower, sizeof(trackNameLower), _trackDesigns[i].name);
        for (int32_t j = 0; trackNameLower[j] != '\0'; j++)
            trackNameLower[j] = (utf8)tolower(trackNameLower[j]);

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
static void window_track_list_close(rct_window *w)
{
    // Dispose track design and preview
    track_design_dispose(_loadedTrackDesign);
    _loadedTrackDesign = nullptr;
    _trackDesignPreviewPixels.clear();
    _trackDesignPreviewPixels.shrink_to_fit();

    // Dispose track list
    for (auto &trackDesign : _trackDesigns)
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
    if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && gScreenAge != 0){
        window_close_by_number(WC_MANAGE_TRACK_DESIGN, w->number);
        window_close_by_number(WC_TRACK_DELETE_PROMPT, w->number);
        Editor::LoadTrackManager();
    }
}

/**
 *
 *  rct2: 0x006CFB82
 */
static void window_track_list_select(rct_window *w, int32_t listIndex)
{
    // Displays a message if the ride can't load, fix #4080
    if (_loadedTrackDesign == nullptr) {
        context_show_error(STR_CANT_BUILD_PARK_ENTRANCE_HERE, STR_TRACK_LOAD_FAILED_ERROR);
        return;
    }

    audio_play_sound(SOUND_CLICK_1, 0, w->x + (w->width / 2));
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
        if (listIndex == 0) {
            window_close(w);
            ride_construct_new(_window_track_list_item);
            return;
        }
        listIndex--;
    }

    rct_track_td6 *td6 = _loadedTrackDesign;
    if (td6 != nullptr && (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE)) {
        gTrackDesignSceneryToggle = true;
    }

    uint16_t trackDesignIndex = _filteredTrackIds[listIndex];
    track_design_file_ref *tdRef = &_trackDesigns[trackDesignIndex];
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        auto intent = Intent(WC_MANAGE_TRACK_DESIGN);
        intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
        context_open_intent(&intent);
    } else {
        if (_loadedTrackDesignIndex != TRACK_DESIGN_INDEX_UNLOADED && (_loadedTrackDesign->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE)) {
            context_show_error(STR_THIS_DESIGN_WILL_BE_BUILT_WITH_AN_ALTERNATIVE_VEHICLE_TYPE, STR_NONE);
        }

        auto intent = Intent(WC_TRACK_DESIGN_PLACE);
        intent.putExtra(INTENT_EXTRA_TRACK_DESIGN, tdRef);
        context_open_intent(&intent);
    }
}

static int32_t window_track_list_get_list_item_index_from_position(int32_t x, int32_t y)
{
    size_t maxItems = _filteredTrackIds.size();
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
        // Extra item: custom design
        maxItems++;
    }

    int32_t index = y / SCROLLABLE_ROW_HEIGHT;
    if (index < 0 || (uint32_t)index >= maxItems) {
        index = -1;
    }
    return index;
}

/**
 *
 *  rct2: 0x006CFA31
 */
static void window_track_list_mouseup(rct_window *w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex) {
    case WIDX_CLOSE:
        window_close(w);
        break;
    case WIDX_ROTATE:
        _currentTrackPieceDirection++;
        _currentTrackPieceDirection %= 4;
        window_invalidate(w);
        break;
    case WIDX_TOGGLE_SCENERY:
        gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
        _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
        window_invalidate(w);
        break;
    case WIDX_BACK:
        window_close(w);
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
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
            w->selected_list_item = _filteredTrackIds[w->selected_list_item];
        }
        else
        {
            if (w->selected_list_item != 0)
                w->selected_list_item = _filteredTrackIds[w->selected_list_item - 1] + 1;
        }

        String::Set(_filterString, sizeof(_filterString), "");
        window_track_list_filter_list();
        window_invalidate(w);
        break;
    }
}

/**
 *
 *  rct2: 0x006CFAB0
 */
static void window_track_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height)
{
    size_t numItems = _filteredTrackIds.size();
    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
        // Extra item: custom design
        numItems++;
    }

    *height = (int32_t)(numItems * SCROLLABLE_ROW_HEIGHT);
}

/**
 *
 *  rct2: 0x006CFB39
 */
static void window_track_list_scrollmousedown(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    if (!w->track_list.track_list_being_updated) {
        int32_t i = window_track_list_get_list_item_index_from_position(x, y);
        if (i != -1) {
            window_track_list_select(w, i);
        }
    }
}

/**
 *
 *  rct2: 0x006CFAD7
 */
static void window_track_list_scrollmouseover(rct_window *w, int32_t scrollIndex, int32_t x, int32_t y)
{
    if (!w->track_list.track_list_being_updated)
    {
        int32_t i = window_track_list_get_list_item_index_from_position(x, y);
        if (i != -1 && w->selected_list_item != i)
        {
            w->selected_list_item = i;
            window_invalidate(w);
        }
    }
}

static void window_track_list_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text)
{
    if (widgetIndex != WIDX_FILTER_STRING || text == nullptr)
        return;

    if (String::Equals(_filterString, text))
        return;

    String::Set(_filterString, sizeof(_filterString), text);

    window_track_list_filter_list();

    w->scrolls->v_top = 0;

    window_invalidate(w);
}

/**
 *
 *  rct2: 0x006CFD6C
 */
static void window_track_list_tooltip(rct_window* w, rct_widgetindex widgetIndex, rct_string_id *stringId)
{
    set_format_arg(0, rct_string_id, STR_LIST);
}

static void window_track_list_update(rct_window *w)
{
    if (gCurrentTextBox.window.classification == w->classification &&
        gCurrentTextBox.window.number == w->number)
    {
        window_update_textbox_caret();
        widget_invalidate(w, WIDX_FILTER_STRING);
    }

    if (w->track_list.reload_track_designs) {
        track_list_load_designs(_window_track_list_item);
        w->selected_list_item = 0;
        window_invalidate(w);
        w->track_list.reload_track_designs = false;
    }
}

/**
 *
 *  rct2: 0x006CF2D6
 */
static void window_track_list_invalidate(rct_window *w)
{
    rct_string_id stringId = STR_NONE;
    rct_ride_entry * entry = get_ride_entry(_window_track_list_item.entry_index);

    if (entry != nullptr)
    {
        rct_ride_name rideName = get_ride_naming(_window_track_list_item.type, entry);
        stringId = rideName.name;
    }

    set_format_arg(0, rct_string_id, stringId);
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        window_track_list_widgets[WIDX_TITLE].text = STR_TRACK_DESIGNS;
        window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_RENAME_OR_DELETE_IT;
    } else {
        window_track_list_widgets[WIDX_TITLE].text = STR_SELECT_DESIGN;
        window_track_list_widgets[WIDX_TRACK_LIST].tooltip = STR_CLICK_ON_DESIGN_TO_BUILD_IT_TIP;
    }

    if ((gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) || w->selected_list_item != 0) {
        w->pressed_widgets |= 1 << WIDX_TRACK_PREVIEW;
        w->disabled_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
        window_track_list_widgets[WIDX_ROTATE].type = WWT_FLATBTN;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_FLATBTN;
        if (gTrackDesignSceneryToggle) {
            w->pressed_widgets &= ~(1 << WIDX_TOGGLE_SCENERY);
        } else {
            w->pressed_widgets |= (1 << WIDX_TOGGLE_SCENERY);
        }
    } else {
        w->pressed_widgets &= ~(1 << WIDX_TRACK_PREVIEW);
        w->disabled_widgets |= (1 << WIDX_TRACK_PREVIEW);
        window_track_list_widgets[WIDX_ROTATE].type = WWT_EMPTY;
        window_track_list_widgets[WIDX_TOGGLE_SCENERY].type = WWT_EMPTY;
    }
}

/**
 *
 *  rct2: 0x006CF387
 */
static void window_track_list_paint(rct_window *w, rct_drawpixelinfo *dpi)
{
    window_draw_widgets(w, dpi);

    int32_t listItemIndex = w->selected_list_item;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
    {
        if (_trackDesigns.size() == 0 || listItemIndex == -1)
            return;
    }
    else {
        if (listItemIndex == 0)
            return;

        // Because the first item in the list is "Build a custom design", lower the index by one
        listItemIndex--;
    }

    int32_t trackIndex = _filteredTrackIds[listItemIndex];

    // Track preview
    int32_t x, y, colour;
    rct_widget *widget = &window_track_list_widgets[WIDX_TRACK_PREVIEW];
    x = w->x + widget->left + 1;
    y = w->y + widget->top + 1;
    colour = ColourMapA[w->colours[0]].darkest;
    gfx_fill_rect(dpi, x, y, x + 369, y + 216, colour);

    if (_loadedTrackDesignIndex != trackIndex) {
        utf8 *path = _trackDesigns[trackIndex].path;
        if (track_list_load_design_for_preview(path)) {
            _loadedTrackDesignIndex = trackIndex;
        } else {
            _loadedTrackDesignIndex = TRACK_DESIGN_INDEX_UNLOADED;
        }
    }

    rct_track_td6 *td6 = _loadedTrackDesign;
    if (td6 == nullptr) {
        return;
    }

    int32_t trackPreviewX = x, trackPreviewY = y;
    x = w->x + (widget->left + widget->right) / 2;
    y = w->y + (widget->top + widget->bottom) / 2;

    if (drawing_engine_get_type() != DRAWING_ENGINE_OPENGL) {
        rct_g1_element g1temp = {};
        g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * TRACK_PREVIEW_IMAGE_SIZE);
        g1temp.width = 370;
        g1temp.height = 217;
        g1temp.flags = G1_FLAG_BMP;
        gfx_set_g1_element(SPR_TEMP, &g1temp);
        gfx_draw_sprite(dpi, SPR_TEMP, trackPreviewX, trackPreviewY, 0);
    }
    else
    {
        gfx_draw_string_centred_clipped(dpi, STR_NOT_SUPPPORTED_IN_OPENGL, nullptr, COLOUR_BLACK, x, y, 368);
    }

    y = w->y + widget->bottom - 12;

    // Warnings
    if ((td6->track_flags & TRACK_DESIGN_FLAG_VEHICLE_UNAVAILABLE) && !(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)) {
        // Vehicle design not available
        gfx_draw_string_centred_clipped(dpi, STR_VEHICLE_DESIGN_UNAVAILABLE, nullptr, COLOUR_BLACK, x, y, 368);
        y -= SCROLLABLE_ROW_HEIGHT;
    }

    if (td6->track_flags & TRACK_DESIGN_FLAG_SCENERY_UNAVAILABLE) {
        if (!gTrackDesignSceneryToggle) {
            // Scenery not available
            gfx_draw_string_centred_clipped(dpi, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, nullptr, COLOUR_BLACK, x, y, 368);
            y -= SCROLLABLE_ROW_HEIGHT;
        }
    }

    // Track design name
    utf8 *trackName = _trackDesigns[trackIndex].name;
    gfx_draw_string_centred_clipped(dpi, STR_TRACK_PREVIEW_NAME_FORMAT, &trackName, COLOUR_BLACK, x, y, 368);

    // Information
    x = w->x + widget->left + 1;
    y = w->y + widget->bottom + 2;

    // Stats
    fixed32_2dp rating = td6->excitement * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_EXCITEMENT_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    rating = td6->intensity * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_INTENSITY_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT;

    rating = td6->nausea * 10;
    gfx_draw_string_left(dpi, STR_TRACK_LIST_NAUSEA_RATING, &rating, COLOUR_BLACK, x, y);
    y += LIST_ROW_HEIGHT + 4;

    // Information for tracked rides.
    if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_TRACK))
    {
        if (td6->type != RIDE_TYPE_MAZE)
        {
            if (td6->type == RIDE_TYPE_MINI_GOLF)
            {
                // Holes
                uint16_t holes = td6->holes & 0x1F;
                gfx_draw_string_left(dpi, STR_HOLES, &holes, COLOUR_BLACK, x, y);
                y += LIST_ROW_HEIGHT;
            }
            else
            {
                // Maximum speed
                uint16_t speed = ((td6->max_speed << 16) * 9) >> 18;
                gfx_draw_string_left(dpi, STR_MAX_SPEED, &speed, COLOUR_BLACK, x, y);
                y += LIST_ROW_HEIGHT;

                // Average speed
                speed = ((td6->average_speed << 16) * 9) >> 18;
                gfx_draw_string_left(dpi, STR_AVERAGE_SPEED, &speed, COLOUR_BLACK, x, y);
                y += LIST_ROW_HEIGHT;
            }

            // Ride length
            set_format_arg(0, rct_string_id, STR_RIDE_LENGTH_ENTRY);
            set_format_arg(2, uint16_t, td6->ride_length);
            gfx_draw_string_left_clipped(dpi, STR_TRACK_LIST_RIDE_LENGTH, gCommonFormatArgs, COLOUR_BLACK, x, y, 214);
            y += LIST_ROW_HEIGHT;
        }

        if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_G_FORCES))
        {
            // Maximum positive vertical Gs
            int32_t gForces = td6->max_positive_vertical_g * 32;
            gfx_draw_string_left(dpi, STR_MAX_POSITIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Maximum negative vertical Gs
            gForces = td6->max_negative_vertical_g * 32;
            gfx_draw_string_left(dpi, STR_MAX_NEGATIVE_VERTICAL_G, &gForces, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Maximum lateral Gs
            gForces = td6->max_lateral_g * 32;
            gfx_draw_string_left(dpi, STR_MAX_LATERAL_G, &gForces, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // If .TD6
            if (td6->version_and_colour_scheme / 4 >= 2)
            {
                if (td6->total_air_time != 0)
                {
                    // Total air time
                    int32_t airTime = td6->total_air_time * 25;
                    gfx_draw_string_left(dpi, STR_TOTAL_AIR_TIME, &airTime, COLOUR_BLACK, x, y);
                    y += LIST_ROW_HEIGHT;
                }
            }
        }

        if (ride_type_has_flag(td6->type, RIDE_TYPE_FLAG_HAS_DROPS))
        {
            // Drops
            uint16_t drops = td6->drops & 0x3F;
            gfx_draw_string_left(dpi, STR_DROPS, &drops, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;

            // Drop height is multiplied by 0.75
            uint16_t highestDropHeight = (td6->highest_drop_height * 3) / 4;
            gfx_draw_string_left(dpi, STR_HIGHEST_DROP_HEIGHT, &highestDropHeight, COLOUR_BLACK, x, y);
            y += LIST_ROW_HEIGHT;
        }

        if (td6->type != RIDE_TYPE_MINI_GOLF)
        {
            uint16_t inversions = td6->inversions & 0x1F;
            if (inversions != 0)
            {
                // Inversions
                gfx_draw_string_left(dpi, STR_INVERSIONS, &inversions, COLOUR_BLACK, x, y);
                y += LIST_ROW_HEIGHT;
            }
        }
        y += 4;
    }

    if (td6->space_required_x != 0xFF)
    {
        // Space required
        set_format_arg(0, uint16_t, td6->space_required_x);
        set_format_arg(2, uint16_t, td6->space_required_y);
        gfx_draw_string_left(dpi, STR_TRACK_LIST_SPACE_REQUIRED, gCommonFormatArgs, COLOUR_BLACK, x, y);
        y += LIST_ROW_HEIGHT;
    }

    if (td6->cost != 0)
    {
        gfx_draw_string_left(dpi, STR_TRACK_LIST_COST_AROUND, &td6->cost, COLOUR_BLACK, x, y);
    }
}

/**
 *
 *  rct2: 0x006CF8CD
 */
static void window_track_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex)
{
    uint8_t paletteIndex = ColourMapA[w->colours[0]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int32_t x = 0;
    int32_t y = 0;
    size_t listIndex = 0;
    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) {
        if (_trackDesigns.size() == 0) {
            // No track designs
            gfx_draw_string_left(dpi, STR_NO_TRACK_DESIGNS_OF_THIS_TYPE, nullptr, COLOUR_BLACK, x, y - 1);
            return;
        }
    } else {
        // Build custom track item
        rct_string_id stringId;
        if (listIndex == (size_t)w->selected_list_item) {
            // Highlight
            gfx_filter_rect(dpi, x, y, w->width, y + SCROLLABLE_ROW_HEIGHT - 1, PALETTE_DARKEN_1);
            stringId = STR_WINDOW_COLOUR_2_STRINGID;
        } else {
            stringId = STR_BLACK_STRING;
        }

        rct_string_id stringId2 = STR_BUILD_CUSTOM_DESIGN;
        gfx_draw_string_left(dpi, stringId, &stringId2, COLOUR_BLACK, x, y - 1);
        y += SCROLLABLE_ROW_HEIGHT;
        listIndex++;
    }

    for (auto i : _filteredTrackIds)
    {
        if (y + SCROLLABLE_ROW_HEIGHT >= dpi->y && y < dpi->y + dpi->height)
        {
            rct_string_id stringId;
            if (listIndex == (size_t)w->selected_list_item)
            {
                // Highlight
                gfx_filter_rect(dpi, x, y, w->width, y + SCROLLABLE_ROW_HEIGHT - 1, PALETTE_DARKEN_1);
                stringId = STR_WINDOW_COLOUR_2_STRINGID;
            }
            else
            {
                stringId = STR_BLACK_STRING;
            }

            // Draw track name
            set_format_arg(0, rct_string_id, STR_TRACK_LIST_NAME_FORMAT);
            set_format_arg(2, utf8*, _trackDesigns[i].name);
            gfx_draw_string_left(dpi, stringId, gCommonFormatArgs, COLOUR_BLACK, x, y - 1);
        }

        y += SCROLLABLE_ROW_HEIGHT;
        listIndex++;
    }
}

static void track_list_load_designs(ride_list_item item)
{
    auto repo = OpenRCT2::GetContext()->GetTrackDesignRepository();
    if (!RideGroupManager::RideTypeHasRideGroups(item.type))
    {
        char entry[9];
        const char * entryPtr = nullptr;
        if (item.type < 0x80)
        {
            if (RideGroupManager::RideTypeIsIndependent(item.type))
            {
                get_ride_entry_name(entry, item.entry_index);
                entryPtr = entry;
            }
        }

        _trackDesigns = repo->GetItemsForObjectEntry(item.type, String::ToStd(entryPtr));
    }
    else
    {
        auto rideEntry = get_ride_entry(item.entry_index);
        auto rideGroup = RideGroupManager::GetRideGroup(item.type, rideEntry);
        _trackDesigns = repo->GetItemsForRideGroup(item.type, rideGroup);
    }

    window_track_list_filter_list();
}

static bool track_list_load_design_for_preview(utf8 *path)
{
    // Dispose currently loaded track
    track_design_dispose(_loadedTrackDesign);
    _loadedTrackDesign = nullptr;

    _loadedTrackDesign = track_design_open(path);
    if (_loadedTrackDesign != nullptr) {
        if (drawing_engine_get_type() != DRAWING_ENGINE_OPENGL) {
            track_design_draw_preview(_loadedTrackDesign, _trackDesignPreviewPixels.data());
        }
        return true;
    }
    return false;
}
