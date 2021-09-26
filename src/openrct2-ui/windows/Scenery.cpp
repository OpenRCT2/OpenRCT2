/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/audio/audio.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Research.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/sprites.h>
#include <openrct2/world/LargeScenery.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/SmallScenery.h>

static constexpr const rct_string_id WINDOW_TITLE = STR_NONE;
constexpr int32_t WINDOW_SCENERY_WIDTH = 634;
constexpr int32_t WINDOW_SCENERY_HEIGHT = 180;
constexpr int32_t SCENERY_BUTTON_WIDTH = 66;
constexpr int32_t SCENERY_BUTTON_HEIGHT = 80;

// clang-format off
static void window_scenery_close(rct_window *w);
static void window_scenery_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_scenery_resize(rct_window *w);
static void window_scenery_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_scenery_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_scenery_update(rct_window *w);
static void window_scenery_periodic_update(rct_window *w);
static void window_scenery_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_scenery_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_scenery_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static OpenRCT2String window_scenery_tooltip(rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback);
static void window_scenery_invalidate(rct_window *w);
static void window_scenery_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_scenery_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_scenery_events([](auto& events)
{
    events.close = &window_scenery_close;
    events.mouse_up = &window_scenery_mouseup;
    events.resize = &window_scenery_resize;
    events.mouse_down = &window_scenery_mousedown;
    events.dropdown = &window_scenery_dropdown;
    events.update = &window_scenery_update;
    events.periodic_update = &window_scenery_periodic_update;
    events.get_scroll_size = &window_scenery_scrollgetsize;
    events.scroll_mousedown = &window_scenery_scrollmousedown;
    events.scroll_mouseover = &window_scenery_scrollmouseover;
    events.tooltip = &window_scenery_tooltip;
    events.invalidate = &window_scenery_invalidate;
    events.paint = &window_scenery_paint;
    events.scroll_paint = &window_scenery_scrollpaint;
});


enum WINDOW_SCENERY_LIST_WIDGET_IDX {
    WIDX_SCENERY_BACKGROUND,
    WIDX_SCENERY_TITLE,
    WIDX_SCENERY_CLOSE,
    WIDX_SCENERY_TAB_CONTENT_PANEL,
    WIDX_SCENERY_LIST,
    WIDX_SCENERY_ROTATE_OBJECTS_BUTTON,
    WIDX_SCENERY_REPAINT_SCENERY_BUTTON,
    WIDX_SCENERY_PRIMARY_COLOUR_BUTTON,
    WIDX_SCENERY_SECONDARY_COLOUR_BUTTON,
    WIDX_SCENERY_TERTIARY_COLOUR_BUTTON,
    WIDX_SCENERY_EYEDROPPER_BUTTON,
    WIDX_SCENERY_BUILD_CLUSTER_BUTTON,
    WIDX_SCENERY_TAB_1,
};

validate_global_widx(WC_SCENERY, WIDX_SCENERY_TAB_1);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_EYEDROPPER_BUTTON);

static rct_widget window_scenery_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WINDOW_SCENERY_WIDTH, WINDOW_SCENERY_HEIGHT),
    MakeWidget     ({  0,  43}, {634, 99}, WindowWidgetType::Resize,    WindowColour::Secondary                                                  ), // 8         0x009DE2C8
    MakeWidget     ({  2,  47}, {607, 80}, WindowWidgetType::Scroll,    WindowColour::Secondary, SCROLL_VERTICAL                                 ), // 1000000   0x009DE418
    MakeWidget     ({609,  44}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_ROTATE_ARROW,    STR_ROTATE_OBJECTS_90      ), // 2000000   0x009DE428
    MakeWidget     ({609,  68}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_PAINTBRUSH,      STR_SCENERY_PAINTBRUSH_TIP ), // 4000000   0x009DE438
    MakeWidget     ({615,  93}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_COLOUR          ), // 8000000   0x009DE448
    MakeWidget     ({615, 105}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_SECONDARY_COLOUR), // 10000000  0x009DE458
    MakeWidget     ({615, 117}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_TERNARY_COLOUR  ), // 20000000  0x009DE468
    MakeWidget     ({609, 130}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_G2_EYEDROPPER,   STR_SCENERY_EYEDROPPER_TIP ), // 40000000  0x009DE478
    MakeWidget     ({609, 154}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, SPR_SCENERY_CLUSTER, STR_SCENERY_CLUSTER_TIP    ), // 40000000  0x009DE478
    WIDGETS_END,
};
// clang-format on

void window_scenery_update_scroll(rct_window* w);

struct SceneryTabInfo
{
    ObjectEntryIndex SceneryGroupIndex = OBJECT_ENTRY_INDEX_NULL;
    std::vector<ScenerySelection> Entries;

    bool IsMisc() const
    {
        return SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL;
    }

    bool Contains(ScenerySelection entry) const
    {
        auto it = std::find(Entries.begin(), Entries.end(), entry);
        return it != Entries.end();
    }

    void AddEntry(ScenerySelection entry)
    {
        if (!Contains(entry))
        {
            Entries.push_back(entry);
        }
    }

    const rct_scenery_group_entry* GetSceneryGroupEntry() const
    {
        return get_scenery_group_entry(SceneryGroupIndex);
    }
};

std::vector<ScenerySelection> gWindowSceneryTabSelections;
size_t gWindowSceneryActiveTabIndex;
uint8_t gWindowSceneryPaintEnabled;
uint8_t gWindowSceneryRotation;
colour_t gWindowSceneryPrimaryColour;
colour_t gWindowScenerySecondaryColour;
colour_t gWindowSceneryTertiaryColour;
bool gWindowSceneryEyedropperEnabled;

static std::vector<SceneryTabInfo> _tabEntries;
static std::vector<rct_widget> _widgets;
static ScenerySelection _selectedScenery;
static int16_t _hoverCounter;

static ScenerySelection GetSelectedScenery(size_t tabIndex)
{
    if (gWindowSceneryTabSelections.size() > tabIndex)
    {
        return gWindowSceneryTabSelections[tabIndex];
    }
    return {};
}

static void SetSelectedScenery(size_t tabIndex, ScenerySelection value)
{
    if (gWindowSceneryTabSelections.size() <= tabIndex)
    {
        gWindowSceneryTabSelections.resize(tabIndex + 1);
    }
    gWindowSceneryTabSelections[tabIndex] = value;
}

static SceneryTabInfo* GetSceneryTabInfoForGroup(ObjectEntryIndex sceneryGroupIndex)
{
    if (sceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
    {
        return &_tabEntries[_tabEntries.size() - 1];
    }

    for (auto& tabEntry : _tabEntries)
    {
        if (tabEntry.SceneryGroupIndex == sceneryGroupIndex)
            return &tabEntry;
    }

    return nullptr;
}

static std::optional<size_t> window_scenery_find_tab_with_scenery(const ScenerySelection& scenery)
{
    for (size_t i = 0; i < _tabEntries.size(); i++)
    {
        const auto& tabInfo = _tabEntries[i];
        for (const auto& entry : tabInfo.Entries)
        {
            if (entry == scenery)
            {
                return i;
            }
        }
    }
    return {};
}

static void init_scenery_entry(const ScenerySelection& selection, ObjectEntryIndex sceneryGroupIndex)
{
    Guard::ArgumentInRange<int32_t>(selection.EntryIndex, 0, OBJECT_ENTRY_INDEX_NULL);

    if (IsSceneryAvailableToBuild(selection))
    {
        // Get current tab
        auto tabIndex = window_scenery_find_tab_with_scenery(selection);

        // Add scenery to primary group (usually trees or path additions)
        if (sceneryGroupIndex != OBJECT_ENTRY_INDEX_NULL)
        {
            auto tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
            if (tabInfo != nullptr)
            {
                tabInfo->AddEntry(selection);
                return;
            }
        }

        // If scenery is no tab, add it to misc
        if (!tabIndex)
        {
            auto tabInfo = GetSceneryTabInfoForGroup(OBJECT_ENTRY_INDEX_NULL);
            if (tabInfo != nullptr)
            {
                tabInfo->AddEntry(selection);
            }
        }
    }
}

static void window_scenery_sort_tabs()
{
    std::sort(_tabEntries.begin(), _tabEntries.end(), [](const SceneryTabInfo& a, const SceneryTabInfo& b) {
        if (a.SceneryGroupIndex == b.SceneryGroupIndex)
            return false;

        if (a.SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
            return false;
        if (b.SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
            return true;

        auto entryA = a.GetSceneryGroupEntry();
        auto entryB = b.GetSceneryGroupEntry();
        return entryA->priority < entryB->priority;
    });
}

static void window_scenery_prepare_widgets(rct_window* w)
{
    _widgets.clear();
    for (const auto& widget : window_scenery_widgets)
    {
        _widgets.push_back(widget);
    }

    // Remove WWT_LAST
    auto lastWidget = _widgets.back();
    _widgets.pop_back();

    // Add tabs
    ScreenCoordsXY pos = { 3, 17 };
    for (const auto& tabInfo : _tabEntries)
    {
        auto widget = MakeTab(pos, STR_STRING_DEFINED_TOOLTIP);
        pos.x += 31;

        if (tabInfo.SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
        {
            widget.image = SPR_TAB_QUESTION | IMAGE_TYPE_REMAP;
        }
        else
        {
            auto sceneryGroupEntry = get_scenery_group_entry(tabInfo.SceneryGroupIndex);
            if (sceneryGroupEntry != nullptr)
            {
                widget.image = sceneryGroupEntry->image | IMAGE_TYPE_REMAP;
            }
        }

        _widgets.push_back(widget);
    }

    _widgets.push_back(lastWidget);

    w->widgets = _widgets.data();

    w->enabled_widgets = (1 << WIDX_SCENERY_CLOSE) | (1 << WIDX_SCENERY_ROTATE_OBJECTS_BUTTON)
        | (1 << WIDX_SCENERY_PRIMARY_COLOUR_BUTTON) | (1 << WIDX_SCENERY_SECONDARY_COLOUR_BUTTON)
        | (1 << WIDX_SCENERY_REPAINT_SCENERY_BUTTON) | (1 << WIDX_SCENERY_TERTIARY_COLOUR_BUTTON)
        | (1 << WIDX_SCENERY_EYEDROPPER_BUTTON) | (1ULL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);
    for (size_t i = 0; i < _tabEntries.size(); i++)
    {
        w->enabled_widgets |= (1ULL << (WIDX_SCENERY_TAB_1 + i));
    }
}

/**
 *
 *  rct2: 0x006DFA00
 */
static void window_scenery_init(rct_window* w)
{
    _tabEntries.clear();

    auto maxTabs = 32;
    for (ObjectEntryIndex scenerySetIndex = 0; scenerySetIndex < maxTabs - 1; scenerySetIndex++)
    {
        const auto* sceneryGroupEntry = get_scenery_group_entry(scenerySetIndex);
        if (sceneryGroupEntry != nullptr)
        {
            SceneryTabInfo tabInfo;
            tabInfo.SceneryGroupIndex = scenerySetIndex;
            for (size_t i = 0; i < sceneryGroupEntry->entry_count; i++)
            {
                auto sceneryEntry = sceneryGroupEntry->scenery_entries[i];
                if (IsSceneryAvailableToBuild(sceneryEntry))
                {
                    tabInfo.Entries.push_back(sceneryEntry);
                }
            }
            if (tabInfo.Entries.size() > 0)
            {
                _tabEntries.push_back(std::move(tabInfo));
            }
        }
    }

    // Add misc tab
    _tabEntries.emplace_back();

    // small scenery
    for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_SMALL_SCENERY_OBJECTS; sceneryId++)
    {
        const auto* sceneryEntry = get_small_scenery_entry(sceneryId);
        if (sceneryEntry != nullptr)
        {
            init_scenery_entry({ SCENERY_TYPE_SMALL, sceneryId }, sceneryEntry->scenery_tab_id);
        }
    }

    // large scenery
    for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_LARGE_SCENERY_OBJECTS; sceneryId++)
    {
        const auto* sceneryEntry = get_large_scenery_entry(sceneryId);
        if (sceneryEntry != nullptr)
        {
            init_scenery_entry({ SCENERY_TYPE_LARGE, sceneryId }, sceneryEntry->scenery_tab_id);
        }
    }

    // walls
    for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_WALL_SCENERY_OBJECTS; sceneryId++)
    {
        const auto* sceneryEntry = get_wall_entry(sceneryId);
        if (sceneryEntry != nullptr)
        {
            init_scenery_entry({ SCENERY_TYPE_WALL, sceneryId }, sceneryEntry->scenery_tab_id);
        }
    }

    // banners
    for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_BANNER_OBJECTS; sceneryId++)
    {
        const auto* sceneryEntry = get_banner_entry(sceneryId);
        if (sceneryEntry != nullptr)
        {
            init_scenery_entry({ SCENERY_TYPE_BANNER, sceneryId }, sceneryEntry->scenery_tab_id);
        }
    }

    // path bits
    for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_PATH_ADDITION_OBJECTS; sceneryId++)
    {
        const auto* sceneryEntry = get_footpath_item_entry(sceneryId);
        if (sceneryEntry != nullptr)
        {
            init_scenery_entry({ SCENERY_TYPE_PATH_ITEM, sceneryId }, sceneryEntry->scenery_tab_id);
        }
    }

    // Remove misc tab if empty
    if (_tabEntries.back().Entries.size() == 0)
    {
        _tabEntries.pop_back();
    }

    window_scenery_sort_tabs();
    window_scenery_prepare_widgets(w);
    window_invalidate_by_class(WC_SCENERY);
}

void window_scenery_init()
{
    auto w = window_find_by_class(WC_SCENERY);
    if (w != nullptr)
    {
        window_scenery_init(w);
    }
}

/**
 *
 *  rct2: 0x006DFEE4
 */
void window_scenery_set_default_placement_configuration()
{
    gWindowSceneryRotation = 3;
    gWindowSceneryPrimaryColour = COLOUR_BORDEAUX_RED;
    gWindowScenerySecondaryColour = COLOUR_YELLOW;
    gWindowSceneryTertiaryColour = COLOUR_DARK_BROWN;

    window_scenery_init();

    gWindowSceneryTabSelections.clear();
    gWindowSceneryActiveTabIndex = 0;
}

/**
 *
 *  rct2: 0x006E0FEF
 */
rct_window* window_scenery_open()
{
    // Check if window is already open
    auto window = window_bring_to_front_by_class(WC_SCENERY);
    if (window != nullptr)
        return window;

    window = WindowCreate(
        ScreenCoordsXY(context_get_width() - WINDOW_SCENERY_WIDTH, 0x1D), WINDOW_SCENERY_WIDTH, WINDOW_SCENERY_HEIGHT,
        &window_scenery_events, WC_SCENERY, WF_NO_SCROLLING);

    window_scenery_init(window);

    WindowInitScrollWidgets(window);
    window_scenery_update_scroll(window);
    show_gridlines();
    gWindowSceneryRotation = 3;
    gSceneryCtrlPressed = false;
    gSceneryShiftPressed = false;
    _selectedScenery = {};
    _hoverCounter = 0;
    window_push_others_below(window);
    gSceneryGhostType = 0;
    gSceneryPlaceCost = MONEY32_UNDEFINED;
    gSceneryPlaceRotation = 0;
    gWindowSceneryPaintEnabled = 0; // repaint coloured scenery tool state
    gWindowSceneryEyedropperEnabled = false;

    window->min_width = WINDOW_SCENERY_WIDTH;
    window->max_width = WINDOW_SCENERY_WIDTH;
    window->min_height = WINDOW_SCENERY_HEIGHT;
    window->max_height = WINDOW_SCENERY_HEIGHT;

    return window;
}

static int32_t window_scenery_get_num_columns(rct_window* w)
{
    const auto* listWidget = &w->widgets[WIDX_SCENERY_LIST];
    auto contentWidth = listWidget->width() - SCROLLBAR_WIDTH;
    return contentWidth / SCENERY_BUTTON_WIDTH;
}

/*
 *
 *  rct2: 0x006E1A73
 */
void window_scenery_close(rct_window* w)
{
    scenery_remove_ghost_tool_placement();
    hide_gridlines();
    viewport_set_visibility(0);

    if (gWindowSceneryScatterEnabled)
        window_close_by_class(WC_SCENERY_SCATTER);

    if (scenery_tool_is_active())
        tool_cancel();
}

template<typename T> constexpr static T window_scenery_count_rows(rct_window* w, T items)
{
    auto rows = items / window_scenery_get_num_columns(w);
    return rows;
}

static size_t window_scenery_count_rows(rct_window* w)
{
    auto tabIndex = gWindowSceneryActiveTabIndex;
    if (tabIndex >= _tabEntries.size())
    {
        return 0;
    }

    auto totalItems = _tabEntries[tabIndex].Entries.size();
    auto numColumns = window_scenery_get_num_columns(w);
    auto rows = window_scenery_count_rows(w, totalItems + numColumns - 1);
    return rows;
}

struct scenery_item
{
    int32_t allRows;
    int32_t selected_item;
    ScenerySelection scenerySelection;
};

static scenery_item window_scenery_count_rows_with_selected_item(rct_window* w, size_t tabIndex)
{
    scenery_item sceneryItem = { 0, 0, ScenerySelection() };
    const auto& scenerySelection = GetSelectedScenery(tabIndex);
    const auto& tabInfo = _tabEntries[tabIndex];
    for (size_t i = 0; i < tabInfo.Entries.size(); i++)
    {
        const auto& currentEntry = tabInfo.Entries[i];
        if (currentEntry == scenerySelection)
        {
            sceneryItem.selected_item = static_cast<int32_t>(i);
            sceneryItem.scenerySelection = scenerySelection;
        }
    }
    sceneryItem.allRows = static_cast<int32_t>(window_scenery_count_rows(w, tabInfo.Entries.size() + 8));
    return sceneryItem;
}

static int32_t window_scenery_rows_height(size_t rows)
{
    return static_cast<int32_t>(rows * SCENERY_BUTTON_HEIGHT);
}

/**
 *
 *  rct2: 0x006BD94C
 */
static void window_scenery_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_SCENERY_CLOSE:
            if (gWindowSceneryScatterEnabled)
                window_close_by_class(WC_SCENERY_SCATTER);
            window_close(w);
            break;
        case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
            gWindowSceneryRotation++;
            gWindowSceneryRotation = gWindowSceneryRotation % 4;
            scenery_remove_ghost_tool_placement();
            w->Invalidate();
            break;
        case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
            gWindowSceneryPaintEnabled ^= 1;
            gWindowSceneryEyedropperEnabled = false;
            if (gWindowSceneryScatterEnabled)
                window_close_by_class(WC_SCENERY_SCATTER);
            w->Invalidate();
            break;
        case WIDX_SCENERY_EYEDROPPER_BUTTON:
            gWindowSceneryPaintEnabled = 0;
            gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
            if (gWindowSceneryScatterEnabled)
                window_close_by_class(WC_SCENERY_SCATTER);
            scenery_remove_ghost_tool_placement();
            w->Invalidate();
            break;
        case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
            gWindowSceneryPaintEnabled = 0;
            gWindowSceneryEyedropperEnabled = false;
            if (gWindowSceneryScatterEnabled)
                window_close_by_class(WC_SCENERY_SCATTER);
            else if (
                network_get_mode() != NETWORK_MODE_CLIENT
                || network_can_perform_command(network_get_current_player_group_index(), -2))
            {
                window_scenery_scatter_open();
            }
            else
            {
                context_show_error(STR_CANT_DO_THIS, STR_PERMISSION_DENIED, {});
            }
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x006E1EB4
 */
void window_scenery_update_scroll(rct_window* w)
{
    auto tabIndex = gWindowSceneryActiveTabIndex;
    if (tabIndex >= _tabEntries.size())
    {
        return;
    }

    int32_t listHeight = w->height - 14 - w->widgets[WIDX_SCENERY_LIST].top - 1;

    scenery_item sceneryItem = window_scenery_count_rows_with_selected_item(w, tabIndex);
    w->scrolls[0].v_bottom = window_scenery_rows_height(sceneryItem.allRows) + 1;

    int32_t maxTop = std::max(0, w->scrolls[0].v_bottom - listHeight);
    auto rowSelected = window_scenery_count_rows(w, sceneryItem.selected_item);
    if (sceneryItem.scenerySelection.IsUndefined())
    {
        rowSelected = 0;
        auto& scenery = _tabEntries[tabIndex].Entries[0];
        if (!scenery.IsUndefined())
        {
            SetSelectedScenery(tabIndex, scenery);
        }
    }

    w->scrolls[0].v_top = window_scenery_rows_height(rowSelected);
    w->scrolls[0].v_top = std::min<int32_t>(maxTop, w->scrolls[0].v_top);

    WidgetScrollUpdateThumbs(w, WIDX_SCENERY_LIST);
}

/**
 *
 *  rct2: 0x006E1E48
 */
static void window_scenery_resize(rct_window* w)
{
    if (w->width < w->min_width)
    {
        w->Invalidate();
        w->width = w->min_width;
        w->Invalidate();
    }

    if (w->width > w->max_width)
    {
        w->Invalidate();
        w->width = w->max_width;
        w->Invalidate();
    }

    if (w->height < w->min_height)
    {
        w->Invalidate();
        w->height = w->min_height;
        w->Invalidate();
        // HACK: For some reason invalidate has not been called
        window_event_invalidate_call(w);
        window_scenery_update_scroll(w);
    }

    if (w->height > w->max_height)
    {
        w->Invalidate();
        w->height = w->max_height;
        w->Invalidate();
        // HACK: For some reason invalidate has not been called
        window_event_invalidate_call(w);
        window_scenery_update_scroll(w);
    }
}

/**
 *
 *  rct2: 0x006E1A25
 */
static void window_scenery_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
            WindowDropdownShowColour(w, widget, w->colours[1], gWindowSceneryPrimaryColour);
            break;
        case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
            WindowDropdownShowColour(w, widget, w->colours[1], gWindowScenerySecondaryColour);
            break;
        case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
            WindowDropdownShowColour(w, widget, w->colours[1], gWindowSceneryTertiaryColour);
            break;
    }

    if (widgetIndex >= WIDX_SCENERY_TAB_1)
    {
        gWindowSceneryActiveTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
        w->Invalidate();
        gSceneryPlaceCost = MONEY32_UNDEFINED;

        window_scenery_update_scroll(w);
    }
}

/**
 *
 *  rct2: 0x006E1A54
 */
static void window_scenery_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (dropdownIndex == -1)
        return;

    if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON)
    {
        gWindowSceneryPrimaryColour = static_cast<colour_t>(dropdownIndex);
    }
    else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON)
    {
        gWindowScenerySecondaryColour = static_cast<colour_t>(dropdownIndex);
    }
    else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON)
    {
        gWindowSceneryTertiaryColour = static_cast<colour_t>(dropdownIndex);
    }

    w->Invalidate();
}

/**
 *
 *  rct2: 0x006E1B9F
 */
static void window_scenery_periodic_update(rct_window* w)
{
    if (!_selectedScenery.IsUndefined())
    {
        _selectedScenery = {};
    }
}

/**
 *
 *  rct2: 0x006E1CD3
 */
static void window_scenery_update(rct_window* w)
{
    const CursorState* state = context_get_cursor_state();
    rct_window* other = window_find_from_point(state->position);
    if (other == w)
    {
        ScreenCoordsXY window = state->position - ScreenCoordsXY{ w->windowPos.x - 26, w->windowPos.y };

        if (window.y < 44 || window.x <= w->width)
        {
            rct_widgetindex widgetIndex = window_find_widget_from_point(w, state->position);
            if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL)
            {
                _hoverCounter++;
                if (_hoverCounter < 8)
                {
                    if (input_get_state() != InputState::ScrollLeft)
                    {
                        w->min_width = WINDOW_SCENERY_WIDTH;
                        w->max_width = WINDOW_SCENERY_WIDTH;
                        w->min_height = WINDOW_SCENERY_HEIGHT;
                        w->max_height = WINDOW_SCENERY_HEIGHT;
                    }
                }
                else
                {
                    const auto& listWidget = w->widgets[WIDX_SCENERY_LIST];
                    auto nonListHeight = w->height - listWidget.height() + 2;

                    auto numRows = static_cast<int32_t>(window_scenery_count_rows(w));
                    auto maxContentHeight = numRows * SCENERY_BUTTON_HEIGHT;
                    auto maxWindowHeight = maxContentHeight + nonListHeight;
                    auto windowHeight = std::clamp(maxWindowHeight, WINDOW_SCENERY_HEIGHT, 463);

                    w->min_width = WINDOW_SCENERY_WIDTH;
                    w->max_width = WINDOW_SCENERY_WIDTH;
                    w->min_height = windowHeight;
                    w->max_height = windowHeight;
                }
            }
        }
    }
    else
    {
        _hoverCounter = 0;
        if (input_get_state() != InputState::ScrollLeft)
        {
            w->min_width = WINDOW_SCENERY_WIDTH;
            w->max_width = WINDOW_SCENERY_WIDTH;
            w->min_height = WINDOW_SCENERY_HEIGHT;
            w->max_height = WINDOW_SCENERY_HEIGHT;
        }
    }

    w->Invalidate();

    if (!scenery_tool_is_active())
    {
        window_close(w);
        return;
    }

    if (gWindowSceneryEyedropperEnabled)
    {
        gCurrentToolId = Tool::Crosshair;
    }
    else if (gWindowSceneryPaintEnabled == 1)
    {
        gCurrentToolId = Tool::PaintDown;
    }
    else
    {
        auto tabIndex = gWindowSceneryActiveTabIndex;
        auto tabSelectedScenery = GetSelectedScenery(tabIndex);
        if (!tabSelectedScenery.IsUndefined())
        {
            if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
            {
                gCurrentToolId = Tool::EntranceDown;
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
            {
                gCurrentToolId = static_cast<Tool>(get_large_scenery_entry(tabSelectedScenery.EntryIndex)->tool_id);
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
            {
                gCurrentToolId = static_cast<Tool>(get_wall_entry(tabSelectedScenery.EntryIndex)->tool_id);
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_PATH_ITEM)
            { // path bit
                gCurrentToolId = static_cast<Tool>(get_footpath_item_entry(tabSelectedScenery.EntryIndex)->tool_id);
            }
            else
            { // small scenery
                gCurrentToolId = static_cast<Tool>(get_small_scenery_entry(tabSelectedScenery.EntryIndex)->tool_id);
            }
        }
    }
}

/**
 *
 *  rct2: 0x006E1A91
 */
void window_scenery_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    auto rows = window_scenery_count_rows(w);
    *height = window_scenery_rows_height(rows);
}

static ScenerySelection get_scenery_id_by_cursor_pos(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    ScenerySelection scenery{};

    auto numColumns = window_scenery_get_num_columns(w);
    auto colIndex = screenCoords.x / SCENERY_BUTTON_WIDTH;
    auto rowIndex = screenCoords.y / SCENERY_BUTTON_HEIGHT;
    if (colIndex >= 0 && colIndex < numColumns && rowIndex >= 0)
    {
        auto tabSceneryIndex = static_cast<size_t>((rowIndex * numColumns) + colIndex);
        auto tabIndex = gWindowSceneryActiveTabIndex;
        if (tabIndex < _tabEntries.size())
        {
            auto& tabInfo = _tabEntries[tabIndex];
            if (tabSceneryIndex < tabInfo.Entries.size())
            {
                return tabInfo.Entries[tabSceneryIndex];
            }
        }
    }
    return scenery;
}

/**
 *
 *  rct2: 0x006E1C4A
 */
void window_scenery_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    ScenerySelection scenery = get_scenery_id_by_cursor_pos(w, screenCoords);
    if (scenery.IsUndefined())
        return;

    SetSelectedScenery(gWindowSceneryActiveTabIndex, scenery);

    gWindowSceneryPaintEnabled &= 0xFE;
    gWindowSceneryEyedropperEnabled = false;
    OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, w->windowPos.x + (w->width / 2));
    _hoverCounter = -16;
    gSceneryPlaceCost = MONEY32_UNDEFINED;
    w->Invalidate();
}

/**
 *
 *  rct2: 0x006E1BB8
 */
void window_scenery_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    ScenerySelection scenery = get_scenery_id_by_cursor_pos(w, screenCoords);
    if (!scenery.IsUndefined())
    {
        _selectedScenery = scenery;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006E1C05
 */
OpenRCT2String window_scenery_tooltip(rct_window* w, const rct_widgetindex widgetIndex, const rct_string_id fallback)
{
    if (widgetIndex >= WIDX_SCENERY_TAB_1)
    {
        auto tabIndex = static_cast<size_t>(widgetIndex - WIDX_SCENERY_TAB_1);
        if (_tabEntries.size() > tabIndex)
        {
            const auto& tabInfo = _tabEntries[tabIndex];
            if (tabInfo.IsMisc())
            {
                auto ft = Formatter();
                ft.Add<rct_string_id>(STR_MISCELLANEOUS);
                return { fallback, ft };
            }

            auto sceneryEntry = tabInfo.GetSceneryGroupEntry();
            if (sceneryEntry != nullptr)
            {
                auto ft = Formatter();
                ft.Add<rct_string_id>(sceneryEntry->name);
                return { fallback, ft };
            }
        }
    }
    return { STR_NONE, Formatter() };
}

/**
 *
 *  rct2: 0x006E118B
 */
void window_scenery_invalidate(rct_window* w)
{
    // Set the window title
    rct_string_id titleStringId = STR_MISCELLANEOUS;
    auto tabIndex = gWindowSceneryActiveTabIndex;
    if (tabIndex < _tabEntries.size())
    {
        const auto& tabInfo = _tabEntries[tabIndex];
        const auto* sgEntry = tabInfo.GetSceneryGroupEntry();
        if (sgEntry != nullptr)
        {
            titleStringId = sgEntry->name;
        }
    }
    w->widgets[WIDX_SCENERY_TITLE].text = titleStringId;

    w->pressed_widgets = 0;
    w->pressed_widgets |= 1ULL << (tabIndex + WIDX_SCENERY_TAB_1);
    if (gWindowSceneryPaintEnabled == 1)
        w->pressed_widgets |= (1ULL << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
    if (gWindowSceneryEyedropperEnabled)
        w->pressed_widgets |= (1ULL << WIDX_SCENERY_EYEDROPPER_BUTTON);
    if (gWindowSceneryScatterEnabled)
        w->pressed_widgets |= (1ULL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

    w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::Empty;
    w->widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WindowWidgetType::Empty;
    w->widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::Empty;

    if (!(gWindowSceneryPaintEnabled & 1))
    {
        w->widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WindowWidgetType::FlatBtn;
    }

    auto tabSelectedScenery = GetSelectedScenery(tabIndex);
    if (!tabSelectedScenery.IsUndefined())
    {
        if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
        {
            if (!(gWindowSceneryPaintEnabled & 1))
            {
                w->widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::FlatBtn;
            }

            auto* sceneryEntry = get_small_scenery_entry(tabSelectedScenery.EntryIndex);
            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ROTATABLE))
            {
                w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
            }
        }
        else if (tabSelectedScenery.SceneryType >= SCENERY_TYPE_LARGE)
        {
            w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
        }
    }

    w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryPrimaryColour)
        | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowScenerySecondaryColour)
        | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
    w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryTertiaryColour)
        | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;

    w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
    w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;

    if (gWindowSceneryPaintEnabled & 1)
    { // repaint coloured scenery tool is on
        w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
        w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
        w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
        w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::Empty;
    }
    else if (!tabSelectedScenery.IsUndefined())
    {
        if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
        {
            auto* bannerEntry = get_banner_entry(tabSelectedScenery.EntryIndex);
            if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
            {
                w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            }
        }
        else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
        {
            auto* sceneryEntry = get_large_scenery_entry(tabSelectedScenery.EntryIndex);

            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
        }
        else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
        {
            auto* wallEntry = get_wall_entry(tabSelectedScenery.EntryIndex);
            if (wallEntry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS))
            {
                w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                    if (wallEntry->flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
                        w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
                    if (wallEntry->flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
                        w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                }
            }
        }
        else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
        {
            auto* sceneryEntry = get_small_scenery_entry(tabSelectedScenery.EntryIndex);

            if (scenery_small_entry_has_flag(
                    sceneryEntry, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            }
        }
    }

    auto windowWidth = w->width;
    if (_tabEntries.size() > 0)
    {
        auto lastTabIndex = _tabEntries.size() - 1;
        const auto lastTabWidget = &w->widgets[WIDX_SCENERY_TAB_1 + lastTabIndex];
        windowWidth = std::max<int32_t>(windowWidth, lastTabWidget->right + 3);
    }
    w->min_width = windowWidth;
    w->max_width = windowWidth;

    w->widgets[WIDX_SCENERY_BACKGROUND].right = windowWidth - 1;
    w->widgets[WIDX_SCENERY_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].right = windowWidth - 1;
    w->widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].bottom = w->height - 1;
    w->widgets[WIDX_SCENERY_TITLE].right = windowWidth - 2;
    w->widgets[WIDX_SCENERY_CLOSE].left = windowWidth - 13;
    w->widgets[WIDX_SCENERY_CLOSE].right = w->widgets[WIDX_SCENERY_CLOSE].left + 10;
    w->widgets[WIDX_SCENERY_LIST].right = windowWidth - 26;
    w->widgets[WIDX_SCENERY_LIST].bottom = w->height - 14;

    w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].left = windowWidth - 25;
    w->widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].left = windowWidth - 25;
    w->widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].left = windowWidth - 25;
    w->widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = windowWidth - 25;
    w->widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = windowWidth - 2;
    w->widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = windowWidth - 2;
    w->widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].right = windowWidth - 2;
    w->widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].right = windowWidth - 2;

    w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].left = windowWidth - 19;
    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].left = windowWidth - 19;
    w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].left = windowWidth - 19;
    w->widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].right = windowWidth - 8;
    w->widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].right = windowWidth - 8;
    w->widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].right = windowWidth - 8;
}

static std::pair<rct_string_id, money32> window_scenery_get_name_and_price(ScenerySelection selectedScenery)
{
    rct_string_id name = STR_UNKNOWN_OBJECT_TYPE;
    money32 price = MONEY32_UNDEFINED;
    if (selectedScenery.IsUndefined() && gSceneryPlaceCost != MONEY32_UNDEFINED)
    {
        price = gSceneryPlaceCost;
    }
    else
    {
        switch (selectedScenery.SceneryType)
        {
            case SCENERY_TYPE_SMALL:
            {
                auto* sceneryEntry = get_small_scenery_entry(selectedScenery.EntryIndex);
                if (sceneryEntry != nullptr)
                {
                    price = sceneryEntry->price * 10;
                    name = sceneryEntry->name;
                }
                break;
            }
            case SCENERY_TYPE_PATH_ITEM:
            {
                auto* sceneryEntry = get_footpath_item_entry(selectedScenery.EntryIndex);
                if (sceneryEntry != nullptr)
                {
                    price = sceneryEntry->price;
                    name = sceneryEntry->name;
                }
                break;
            }
            case SCENERY_TYPE_WALL:
            {
                auto* sceneryEntry = get_wall_entry(selectedScenery.EntryIndex);
                if (sceneryEntry != nullptr)
                {
                    price = sceneryEntry->price;
                    name = sceneryEntry->name;
                }
                break;
            }
            case SCENERY_TYPE_LARGE:
            {
                auto* sceneryEntry = get_large_scenery_entry(selectedScenery.EntryIndex);
                if (sceneryEntry != nullptr)
                {
                    price = sceneryEntry->price * 10;
                    name = sceneryEntry->name;
                }
                break;
            }
            case SCENERY_TYPE_BANNER:
            {
                auto* sceneryEntry = get_banner_entry(selectedScenery.EntryIndex);
                if (sceneryEntry != nullptr)
                {
                    price = sceneryEntry->price;
                    name = sceneryEntry->name;
                }
                break;
            }
        }
    }
    return { name, price };
}

/**
 *
 *  rct2: 0x006E1462
 */
void window_scenery_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    WindowDrawWidgets(w, dpi);

    auto tabIndex = gWindowSceneryActiveTabIndex;
    if (tabIndex < _tabEntries.size())
    {
        auto selectedWidgetId = static_cast<rct_widgetindex>(WIDX_SCENERY_TAB_1 + tabIndex);
        uint32_t imageId = ((w->colours[1] << 19) | w->widgets[selectedWidgetId].image) + 1ul;

        gfx_draw_sprite(
            dpi, imageId, w->windowPos + ScreenCoordsXY{ w->widgets[selectedWidgetId].left, w->widgets[selectedWidgetId].top },
            selectedWidgetId);
    }

    auto selectedSceneryEntry = _selectedScenery;
    if (selectedSceneryEntry.IsUndefined())
    {
        if (gWindowSceneryPaintEnabled & 1) // repaint coloured scenery tool is on
            return;
        if (gWindowSceneryEyedropperEnabled)
            return;

        selectedSceneryEntry = GetSelectedScenery(tabIndex);
        if (selectedSceneryEntry.IsUndefined())
            return;
    }

    auto [name, price] = window_scenery_get_name_and_price(selectedSceneryEntry);
    if (price != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        auto ft = Formatter();
        ft.Add<money64>(price);

        // -14
        DrawTextBasic(
            dpi, w->windowPos + ScreenCoordsXY{ w->width - 0x1A, w->height - 13 }, STR_COST_LABEL, ft,
            { TextAlignment::RIGHT });
    }

    auto ft = Formatter();
    ft.Add<rct_string_id>(name);
    DrawTextEllipsised(dpi, { w->windowPos.x + 3, w->windowPos.y + w->height - 13 }, w->width - 19, STR_BLACK_STRING, ft);
}

static void window_scenery_scrollpaint_item(rct_window* w, rct_drawpixelinfo* dpi, ScenerySelection scenerySelection)
{
    if (scenerySelection.SceneryType == SCENERY_TYPE_BANNER)
    {
        auto bannerEntry = get_banner_entry(scenerySelection.EntryIndex);
        uint32_t imageId = bannerEntry->image + gWindowSceneryRotation * 2;
        imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

        gfx_draw_sprite(dpi, imageId, { 0x21, 0x28 }, w->colours[1]);
        gfx_draw_sprite(dpi, imageId + 1, { 0x21, 0x28 }, w->colours[1]);
    }
    else if (scenerySelection.SceneryType == SCENERY_TYPE_LARGE)
    {
        auto sceneryEntry = get_large_scenery_entry(scenerySelection.EntryIndex);
        uint32_t imageId = sceneryEntry->image + gWindowSceneryRotation;
        imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;
        imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;

        gfx_draw_sprite(dpi, imageId, { 0x21, 0 }, w->colours[1]);
    }
    else if (scenerySelection.SceneryType == SCENERY_TYPE_WALL)
    {
        auto wallEntry = get_wall_entry(scenerySelection.EntryIndex);
        uint32_t imageId = wallEntry->image;
        uint8_t tertiaryColour = w->colours[1];
        uint16_t spriteTop = (wallEntry->height * 2) + 0x32;

        if (wallEntry->flags & WALL_SCENERY_HAS_GLASS)
        {
            imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

            if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
            {
                imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;
            }
            gfx_draw_sprite(dpi, imageId, { 0x2F, spriteTop }, tertiaryColour);

            imageId = (wallEntry->image + 0x40000006) | (EnumValue(GlassPaletteIds[gWindowSceneryPrimaryColour]) << 19);
            gfx_draw_sprite(dpi, imageId, { 0x2F, spriteTop }, tertiaryColour);
        }
        else
        {
            imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

            if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
            {
                imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;

                if (wallEntry->flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
                {
                    imageId &= 0xDFFFFFFF;
                    tertiaryColour = gWindowSceneryTertiaryColour;
                }
            }
            gfx_draw_sprite(dpi, imageId, { 0x2F, spriteTop }, tertiaryColour);

            if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
            {
                gfx_draw_sprite(dpi, imageId + 1, { 0x2F, spriteTop }, tertiaryColour);
            }
        }
    }
    else if (scenerySelection.SceneryType == SCENERY_TYPE_PATH_ITEM)
    {
        auto* pathBitEntry = get_footpath_item_entry(scenerySelection.EntryIndex);
        uint32_t imageId = pathBitEntry->image;

        gfx_draw_sprite(dpi, imageId, { 0x0B, 0x10 }, w->colours[1]);
    }
    else
    {
        auto sceneryEntry = get_small_scenery_entry(scenerySelection.EntryIndex);
        uint32_t imageId = sceneryEntry->image + gWindowSceneryRotation;

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
        {
            imageId |= (gWindowSceneryPrimaryColour << 19) | IMAGE_TYPE_REMAP;

            if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
            {
                imageId |= (gWindowScenerySecondaryColour << 24) | IMAGE_TYPE_REMAP_2_PLUS;
            }
        }

        uint16_t spriteTop = (sceneryEntry->height / 4) + 0x2B;

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_FULL_TILE)
            && scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
        {
            spriteTop -= 0x0C;
        }

        gfx_draw_sprite(dpi, imageId, { 0x20, spriteTop }, w->colours[1]);

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_HAS_GLASS))
        {
            imageId = ((sceneryEntry->image + gWindowSceneryRotation) + 0x40000004)
                + (EnumValue(GlassPaletteIds[gWindowSceneryPrimaryColour]) << 19);

            gfx_draw_sprite(dpi, imageId, { 0x20, spriteTop }, w->colours[1]);
        }

        if (scenery_small_entry_has_flag(sceneryEntry, SMALL_SCENERY_FLAG_ANIMATED_FG))
        {
            imageId = (sceneryEntry->image + gWindowSceneryRotation) + 4;
            gfx_draw_sprite(dpi, imageId, { 0x20, spriteTop }, w->colours[1]);
        }
    }
}

void window_scenery_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    gfx_clear(dpi, ColourMapA[w->colours[1]].mid_light);

    auto numColumns = window_scenery_get_num_columns(w);
    auto tabIndex = gWindowSceneryActiveTabIndex;
    if (tabIndex > _tabEntries.size())
    {
        return;
    }

    ScreenCoordsXY topLeft{ 0, 0 };

    const auto& tabInfo = _tabEntries[tabIndex];
    for (size_t sceneryTabItemIndex = 0; sceneryTabItemIndex < tabInfo.Entries.size(); sceneryTabItemIndex++)
    {
        const auto& currentSceneryGlobal = tabInfo.Entries[sceneryTabItemIndex];
        auto tabSelectedScenery = GetSelectedScenery(tabIndex);
        if (gWindowSceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled)
        {
            if (_selectedScenery == currentSceneryGlobal)
            {
                gfx_fill_rect_inset(
                    dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                    w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
            }
        }
        else
        {
            if (tabSelectedScenery == currentSceneryGlobal)
            {
                gfx_fill_rect_inset(
                    dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                    w->colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
            }
            else if (_selectedScenery == currentSceneryGlobal)
            {
                gfx_fill_rect_inset(
                    dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                    w->colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
            }
        }

        rct_drawpixelinfo clipdpi;
        if (clip_drawpixelinfo(
                &clipdpi, dpi, topLeft + ScreenCoordsXY{ 1, 1 }, SCENERY_BUTTON_WIDTH - 2, SCENERY_BUTTON_HEIGHT - 2))
        {
            window_scenery_scrollpaint_item(w, &clipdpi, currentSceneryGlobal);
        }

        topLeft.x += SCENERY_BUTTON_WIDTH;
        if (topLeft.x >= numColumns * SCENERY_BUTTON_WIDTH)
        {
            topLeft.y += SCENERY_BUTTON_HEIGHT;
            topLeft.x = 0;
        }
    }
}

bool window_scenery_set_selected_item(const ScenerySelection& scenery)
{
    bool result = false;
    rct_window* w = window_bring_to_front_by_class(WC_SCENERY);
    if (w != nullptr)
    {
        auto tabIndex = window_scenery_find_tab_with_scenery(scenery);
        if (tabIndex)
        {
            gWindowSceneryActiveTabIndex = *tabIndex;
            SetSelectedScenery(*tabIndex, scenery);

            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, context_get_width() / 2);
            _hoverCounter = -16;
            gSceneryPlaceCost = MONEY32_UNDEFINED;
            w->Invalidate();
            result = true;
        }
    }
    return result;
}

// Used after removing objects, in order to avoid crashes.
void window_scenery_reset_selected_scenery_items()
{
    gWindowSceneryTabSelections.clear();
}
