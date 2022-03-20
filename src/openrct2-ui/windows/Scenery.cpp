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
#include <openrct2/localisation/Formatter.h>
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

constexpr uint8_t SceneryContentScrollIndex = 0;

enum WindowSceneryListWidgetIdx
{
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

// clang-format off
static rct_widget WindowSceneryBaseWidgets[] = {
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

std::vector<ScenerySelection> gWindowSceneryTabSelections;
size_t gWindowSceneryActiveTabIndex;
uint8_t gWindowSceneryPaintEnabled;
uint8_t gWindowSceneryRotation;
colour_t gWindowSceneryPrimaryColour;
colour_t gWindowScenerySecondaryColour;
colour_t gWindowSceneryTertiaryColour;
bool gWindowSceneryEyedropperEnabled;

class SceneryWindow final : public Window
{
private:
    struct SceneryItem
    {
        int32_t allRows;
        int32_t selected_item;
        ScenerySelection scenerySelection;
    };

    struct SceneryTabInfo
    {
        ObjectEntryIndex SceneryGroupIndex = OBJECT_ENTRY_INDEX_NULL;
        std::vector<ScenerySelection> Entries;

        bool IsMisc() const
        {
            return SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL;
        }

        bool Contains(const ScenerySelection& entry) const
        {
            return std::find(std::begin(Entries), std::end(Entries), entry) != std::end(Entries);
        }

        void AddEntry(const ScenerySelection& entry)
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

    std::vector<SceneryTabInfo> _tabEntries;
    std::vector<rct_widget> _widgets;
    ScenerySelection _selectedScenery;
    int16_t _hoverCounter;

public:
    void OnOpen() override
    {
        Init();

        InitScrollWidgets();
        ContentUpdateScroll();
        show_gridlines();
        gWindowSceneryRotation = 3;
        gSceneryCtrlPressed = false;
        gSceneryShiftPressed = false;
        _selectedScenery = {};
        _hoverCounter = 0;
        window_push_others_below(this);
        gSceneryGhostType = 0;
        gSceneryPlaceCost = MONEY32_UNDEFINED;
        gSceneryPlaceRotation = 0;
        gWindowSceneryPaintEnabled = 0; // repaint coloured scenery tool state
        gWindowSceneryEyedropperEnabled = false;

        min_width = WINDOW_SCENERY_WIDTH;
        max_width = WINDOW_SCENERY_WIDTH;
        min_height = WINDOW_SCENERY_HEIGHT;
        max_height = WINDOW_SCENERY_HEIGHT;
    }

    void OnClose() override
    {
        scenery_remove_ghost_tool_placement();
        hide_gridlines();
        viewport_set_visibility(0);

        if (gWindowSceneryScatterEnabled)
            window_close_by_class(WC_SCENERY_SCATTER);

        if (scenery_tool_is_active())
            tool_cancel();
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_SCENERY_CLOSE:
                if (gWindowSceneryScatterEnabled)
                    window_close_by_class(WC_SCENERY_SCATTER);
                Close();
                break;
            case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
                gWindowSceneryRotation++;
                gWindowSceneryRotation = gWindowSceneryRotation % 4;
                scenery_remove_ghost_tool_placement();
                Invalidate();
                break;
            case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
                gWindowSceneryPaintEnabled ^= 1;
                gWindowSceneryEyedropperEnabled = false;
                if (gWindowSceneryScatterEnabled)
                    window_close_by_class(WC_SCENERY_SCATTER);
                Invalidate();
                break;
            case WIDX_SCENERY_EYEDROPPER_BUTTON:
                gWindowSceneryPaintEnabled = 0;
                gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
                if (gWindowSceneryScatterEnabled)
                    window_close_by_class(WC_SCENERY_SCATTER);
                scenery_remove_ghost_tool_placement();
                Invalidate();
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
                    WindowSceneryScatterOpen();
                }
                else
                {
                    context_show_error(STR_CANT_DO_THIS, STR_PERMISSION_DENIED, {});
                }
                Invalidate();
                break;
        }
    }

    void OnResize() override
    {
        if (width < min_width)
        {
            Invalidate();
            width = min_width;
            Invalidate();
        }

        if (width > max_width)
        {
            Invalidate();
            width = max_width;
            Invalidate();
        }

        if (height < min_height)
        {
            Invalidate();
            height = min_height;
            Invalidate();
            // HACK: For some reason invalidate has not been called
            window_event_invalidate_call(this);
            ContentUpdateScroll();
        }

        if (height > max_height)
        {
            Invalidate();
            height = max_height;
            Invalidate();
            // HACK: For some reason invalidate has not been called
            window_event_invalidate_call(this);
            ContentUpdateScroll();
        }
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], gWindowSceneryPrimaryColour);
                break;
            case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], gWindowScenerySecondaryColour);
                break;
            case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
                WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], gWindowSceneryTertiaryColour);
                break;
        }

        if (widgetIndex >= WIDX_SCENERY_TAB_1)
        {
            gWindowSceneryActiveTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
            Invalidate();
            gSceneryPlaceCost = MONEY32_UNDEFINED;

            ContentUpdateScroll();
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t dropdownIndex) override
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

        Invalidate();
    }

    void OnPeriodicUpdate() override
    {
        if (!_selectedScenery.IsUndefined())
        {
            // Find out what scenery the cursor is over
            const CursorState* state = context_get_cursor_state();
            rct_widgetindex widgetIndex = window_find_widget_from_point(this, state->position);
            if (widgetIndex == WIDX_SCENERY_LIST)
            {
                ScreenCoordsXY scrollPos = {};
                int32_t scrollArea = 0;
                int32_t scrollId = 0;
                WidgetScrollGetPart(this, &widgets[WIDX_SCENERY_LIST], state->position, scrollPos, &scrollArea, &scrollId);
                if (scrollArea == SCROLL_PART_VIEW)
                {
                    const ScenerySelection scenery = GetSceneryIdByCursorPos(scrollPos);
                    if (scenery == _selectedScenery)
                    {
                        return;
                    }
                }
            }

            // Cursor was not over the currently hover selected scenery so reset hover selection.
            // This will happen when the mouse leaves the scroll window and is required so that the cost and description switch
            // to the tool scenery selection.
            _selectedScenery = {};
        }
    }

    void OnUpdate() override
    {
        const CursorState* state = context_get_cursor_state();
        rct_window* other = window_find_from_point(state->position);
        if (other == this)
        {
            ScreenCoordsXY window = state->position - ScreenCoordsXY{ windowPos.x - 26, windowPos.y };

            if (window.y < 44 || window.x <= width)
            {
                rct_widgetindex widgetIndex = window_find_widget_from_point(this, state->position);
                if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL)
                {
                    _hoverCounter++;
                    if (_hoverCounter < 8)
                    {
                        if (input_get_state() != InputState::ScrollLeft)
                        {
                            min_width = WINDOW_SCENERY_WIDTH;
                            max_width = WINDOW_SCENERY_WIDTH;
                            min_height = WINDOW_SCENERY_HEIGHT;
                            max_height = WINDOW_SCENERY_HEIGHT;
                        }
                    }
                    else
                    {
                        const auto& listWidget = widgets[WIDX_SCENERY_LIST];
                        const auto nonListHeight = height - listWidget.height() + 2;

                        const auto numRows = static_cast<int32_t>(CountRows());
                        const auto maxContentHeight = numRows * SCENERY_BUTTON_HEIGHT;
                        const auto maxWindowHeight = maxContentHeight + nonListHeight;
                        const auto windowHeight = std::clamp(maxWindowHeight, WINDOW_SCENERY_HEIGHT, 463);

                        min_width = WINDOW_SCENERY_WIDTH;
                        max_width = WINDOW_SCENERY_WIDTH;
                        min_height = windowHeight;
                        max_height = windowHeight;
                    }
                }
            }
        }
        else
        {
            _hoverCounter = 0;
            if (input_get_state() != InputState::ScrollLeft)
            {
                min_width = WINDOW_SCENERY_WIDTH;
                max_width = WINDOW_SCENERY_WIDTH;
                min_height = WINDOW_SCENERY_HEIGHT;
                max_height = WINDOW_SCENERY_HEIGHT;
            }
        }

        Invalidate();

        if (!scenery_tool_is_active())
        {
            Close();
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
            const auto tabIndex = gWindowSceneryActiveTabIndex;
            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
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

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        if (scrollIndex == SceneryContentScrollIndex)
        {
            return ContentScrollGetSize();
        }
        return {};
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (scrollIndex == SceneryContentScrollIndex)
        {
            ContentScrollMouseDown(screenCoords);
        }
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        if (scrollIndex == SceneryContentScrollIndex)
        {
            ContentScrollMouseOver(screenCoords);
        }
    }

    OpenRCT2String OnTooltip(const rct_widgetindex widgetIndex, const rct_string_id fallback) override
    {
        if (widgetIndex >= WIDX_SCENERY_TAB_1)
        {
            const auto tabIndex = static_cast<size_t>(widgetIndex - WIDX_SCENERY_TAB_1);
            if (_tabEntries.size() > tabIndex)
            {
                const auto& tabInfo = _tabEntries[tabIndex];
                if (tabInfo.IsMisc())
                {
                    auto ft = Formatter();
                    ft.Add<rct_string_id>(STR_MISCELLANEOUS);
                    return { fallback, ft };
                }

                const auto* sceneryEntry = tabInfo.GetSceneryGroupEntry();
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

    void OnPrepareDraw() override
    {
        // Set the window title
        rct_string_id titleStringId = STR_MISCELLANEOUS;
        const auto tabIndex = gWindowSceneryActiveTabIndex;
        if (tabIndex < _tabEntries.size())
        {
            const auto& tabInfo = _tabEntries[tabIndex];
            const auto* sgEntry = tabInfo.GetSceneryGroupEntry();
            if (sgEntry != nullptr)
            {
                titleStringId = sgEntry->name;
            }
        }
        widgets[WIDX_SCENERY_TITLE].text = titleStringId;

        pressed_widgets = 0;
        pressed_widgets |= 1ULL << (tabIndex + WIDX_SCENERY_TAB_1);
        if (gWindowSceneryPaintEnabled == 1)
            pressed_widgets |= (1ULL << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
        if (gWindowSceneryEyedropperEnabled)
            pressed_widgets |= (1ULL << WIDX_SCENERY_EYEDROPPER_BUTTON);
        if (gWindowSceneryScatterEnabled)
            pressed_widgets |= (1ULL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

        widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::Empty;

        if (!(gWindowSceneryPaintEnabled & 1))
        {
            widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].type = WindowWidgetType::FlatBtn;
        }

        const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
        if (!tabSelectedScenery.IsUndefined())
        {
            if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
            {
                if (!(gWindowSceneryPaintEnabled & 1))
                {
                    widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::FlatBtn;
                }

                auto* sceneryEntry = get_small_scenery_entry(tabSelectedScenery.EntryIndex);
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                {
                    widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
                }
            }
            else if (tabSelectedScenery.SceneryType >= SCENERY_TYPE_LARGE)
            {
                widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
            }
        }

        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryPrimaryColour)
            | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowScenerySecondaryColour)
            | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;
        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image = SPRITE_ID_PALETTE_COLOUR_1(gWindowSceneryTertiaryColour)
            | IMAGE_TYPE_TRANSPARENT | SPR_PALETTE_BTN;

        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;

        if (gWindowSceneryPaintEnabled & 1)
        { // repaint coloured scenery tool is on
            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::Empty;
        }
        else if (!tabSelectedScenery.IsUndefined())
        {
            if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
            {
                auto* bannerEntry = get_banner_entry(tabSelectedScenery.EntryIndex);
                if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                {
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                }
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
            {
                auto* sceneryEntry = get_large_scenery_entry(tabSelectedScenery.EntryIndex);

                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                    widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                    widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
            {
                auto* wallEntry = get_wall_entry(tabSelectedScenery.EntryIndex);
                if (wallEntry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS))
                {
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                        if (wallEntry->flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
                            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
                        if (wallEntry->flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
                            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                    }
                }
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
            {
                auto* sceneryEntry = get_small_scenery_entry(tabSelectedScenery.EntryIndex);

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                {
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
                        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                }
            }
        }

        auto windowWidth = width;
        if (_tabEntries.size() > 0)
        {
            const auto lastTabIndex = _tabEntries.size() - 1;
            const auto lastTabWidget = &widgets[WIDX_SCENERY_TAB_1 + lastTabIndex];
            windowWidth = std::max<int32_t>(windowWidth, lastTabWidget->right + 3);
        }
        min_width = windowWidth;
        max_width = windowWidth;

        widgets[WIDX_SCENERY_BACKGROUND].right = windowWidth - 1;
        widgets[WIDX_SCENERY_BACKGROUND].bottom = height - 1;
        widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].right = windowWidth - 1;
        widgets[WIDX_SCENERY_TAB_CONTENT_PANEL].bottom = height - 1;
        widgets[WIDX_SCENERY_TITLE].right = windowWidth - 2;
        widgets[WIDX_SCENERY_CLOSE].left = windowWidth - 13;
        widgets[WIDX_SCENERY_CLOSE].right = widgets[WIDX_SCENERY_CLOSE].left + 10;
        widgets[WIDX_SCENERY_LIST].right = windowWidth - 26;
        widgets[WIDX_SCENERY_LIST].bottom = height - 14;

        widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].left = windowWidth - 25;
        widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].left = windowWidth - 25;
        widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].left = windowWidth - 25;
        widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].left = windowWidth - 25;
        widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].right = windowWidth - 2;
        widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].right = windowWidth - 2;
        widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].right = windowWidth - 2;
        widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].right = windowWidth - 2;

        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].left = windowWidth - 19;
        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].left = windowWidth - 19;
        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].left = windowWidth - 19;
        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].right = windowWidth - 8;
        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].right = windowWidth - 8;
        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].right = windowWidth - 8;
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabs(dpi, windowPos);

        auto selectedSceneryEntry = _selectedScenery;
        if (selectedSceneryEntry.IsUndefined())
        {
            if (gWindowSceneryPaintEnabled & 1) // repaint coloured scenery tool is on
                return;
            if (gWindowSceneryEyedropperEnabled)
                return;

            selectedSceneryEntry = GetSelectedScenery(gWindowSceneryActiveTabIndex);
            if (selectedSceneryEntry.IsUndefined())
                return;
        }

        auto [name, price] = GetNameAndPrice(selectedSceneryEntry);
        if (price != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            auto ft = Formatter();
            ft.Add<money64>(price);

            // -14
            DrawTextBasic(
                &dpi, windowPos + ScreenCoordsXY{ width - 0x1A, height - 13 }, STR_COST_LABEL, ft, { TextAlignment::RIGHT });
        }

        auto ft = Formatter();
        ft.Add<rct_string_id>(name);
        DrawTextEllipsised(&dpi, { windowPos.x + 3, windowPos.y + height - 13 }, width - 19, STR_BLACK_STRING, ft);
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        if (scrollIndex == SceneryContentScrollIndex)
        {
            ContentScrollDraw(dpi);
        }
    }

    void SetSelectedItem(
        const ScenerySelection& scenery, const std::optional<colour_t> primary, const std::optional<colour_t> secondary,
        const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation)
    {
        auto tabIndex = FindTabWithScenery(scenery);
        if (!tabIndex.has_value())
        {
            return;
        }

        gWindowSceneryActiveTabIndex = tabIndex.value();
        SetSelectedScenery(tabIndex.value(), scenery);
        if (primary.has_value())
        {
            gWindowSceneryPrimaryColour = primary.value();
        }
        if (secondary.has_value())
        {
            gWindowScenerySecondaryColour = secondary.value();
        }
        if (tertiary.has_value())
        {
            gWindowSceneryTertiaryColour = tertiary.value();
        }
        if (rotation.has_value())
        {
            gWindowSceneryRotation = rotation.value();
        }
        gWindowSceneryEyedropperEnabled = false;
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, context_get_width() / 2);
        _hoverCounter = -16;
        gSceneryPlaceCost = MONEY32_UNDEFINED;
        Invalidate();
    }

    void SetSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
    {
        const auto* tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
        if (tabInfo == nullptr)
        {
            tabInfo = &_tabEntries.back();
        }
        const auto tabId = std::distance(&*_tabEntries.cbegin(), tabInfo);

        OnMouseDown(WIDX_SCENERY_TAB_1 + tabId);
    }

    // Used after removing objects, in order to avoid crashes.
    void ResetSelectedSceneryItems()
    {
        gWindowSceneryTabSelections.clear();
    }

    void Init()
    {
        _tabEntries.clear();

        auto maxTabs = 32;
        for (ObjectEntryIndex scenerySetIndex = 0; scenerySetIndex < maxTabs - 1; scenerySetIndex++)
        {
            const auto* sceneryGroupEntry = get_scenery_group_entry(scenerySetIndex);
            if (sceneryGroupEntry != nullptr && scenery_group_is_invented(scenerySetIndex))
            {
                SceneryTabInfo tabInfo;
                tabInfo.SceneryGroupIndex = scenerySetIndex;
                for (size_t i = 0; i < sceneryGroupEntry->entry_count; i++)
                {
                    const auto& sceneryEntry = sceneryGroupEntry->scenery_entries[i];
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
                InitSceneryEntry({ SCENERY_TYPE_SMALL, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // large scenery
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_LARGE_SCENERY_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = get_large_scenery_entry(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_LARGE, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // walls
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_WALL_SCENERY_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = get_wall_entry(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_WALL, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // banners
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_BANNER_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = get_banner_entry(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_BANNER, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // path bits
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_PATH_ADDITION_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = get_footpath_item_entry(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_PATH_ITEM, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // Remove misc tab if empty
        if (_tabEntries.back().Entries.size() == 0)
        {
            _tabEntries.pop_back();
        }

        SortTabs();
        PrepareWidgets();
        window_invalidate_by_class(WC_SCENERY);
    }

    void SetDefaultPlacementConfiguration()
    {
        gWindowSceneryRotation = 3;
        gWindowSceneryPrimaryColour = COLOUR_BORDEAUX_RED;
        gWindowScenerySecondaryColour = COLOUR_YELLOW;
        gWindowSceneryTertiaryColour = COLOUR_DARK_BROWN;

        Init();

        gWindowSceneryTabSelections.clear();
        gWindowSceneryActiveTabIndex = 0;
    }

private:
    int32_t GetNumColumns() const
    {
        const auto& listWidget = widgets[WIDX_SCENERY_LIST];
        const auto contentWidth = listWidget.width() - SCROLLBAR_WIDTH;
        return contentWidth / SCENERY_BUTTON_WIDTH;
    }

    template<typename T> T CountRows(T items) const
    {
        const auto rows = items / GetNumColumns();
        return rows;
    }

    size_t CountRows() const
    {
        const auto tabIndex = gWindowSceneryActiveTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return 0;
        }

        const auto totalItems = _tabEntries[tabIndex].Entries.size();
        const auto numColumns = GetNumColumns();
        const auto rows = CountRows(totalItems + numColumns - 1);
        return rows;
    }

    int32_t constexpr ContentRowsHeight(const size_t rows) const
    {
        return static_cast<int32_t>(rows * SCENERY_BUTTON_HEIGHT);
    }

    void ContentUpdateScroll()
    {
        const auto tabIndex = gWindowSceneryActiveTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return;
        }

        const int32_t listHeight = height - 14 - widgets[WIDX_SCENERY_LIST].top - 1;

        const auto sceneryItem = ContentCountRowsWithSelectedItem(tabIndex);
        scrolls[SceneryContentScrollIndex].v_bottom = ContentRowsHeight(sceneryItem.allRows) + 1;

        const int32_t maxTop = std::max(0, scrolls[SceneryContentScrollIndex].v_bottom - listHeight);
        auto rowSelected = CountRows(sceneryItem.selected_item);
        if (sceneryItem.scenerySelection.IsUndefined())
        {
            rowSelected = 0;
            const auto& scenery = _tabEntries[tabIndex].Entries[0];
            if (!scenery.IsUndefined())
            {
                SetSelectedScenery(tabIndex, scenery);
            }
        }

        scrolls[SceneryContentScrollIndex].v_top = ContentRowsHeight(rowSelected);
        scrolls[SceneryContentScrollIndex].v_top = std::min<int32_t>(maxTop, scrolls[SceneryContentScrollIndex].v_top);

        WidgetScrollUpdateThumbs(this, WIDX_SCENERY_LIST);
    }

    SceneryItem ContentCountRowsWithSelectedItem(const size_t tabIndex)
    {
        SceneryItem sceneryItem = { 0, 0, ScenerySelection() };
        const auto scenerySelection = GetSelectedScenery(tabIndex);
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
        sceneryItem.allRows = static_cast<int32_t>(CountRows(tabInfo.Entries.size() + 8));
        return sceneryItem;
    }

    const ScenerySelection GetSelectedScenery(const size_t tabIndex)
    {
        if (gWindowSceneryTabSelections.size() > tabIndex)
        {
            return gWindowSceneryTabSelections[tabIndex];
        }
        return {};
    }

    void SetSelectedScenery(const size_t tabIndex, const ScenerySelection& value)
    {
        if (gWindowSceneryTabSelections.size() <= tabIndex)
        {
            gWindowSceneryTabSelections.resize(tabIndex + 1);
        }
        gWindowSceneryTabSelections[tabIndex] = value;
    }

    SceneryTabInfo* GetSceneryTabInfoForGroup(const ObjectEntryIndex sceneryGroupIndex)
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

    std::optional<size_t> FindTabWithScenery(const ScenerySelection& scenery)
    {
        for (size_t i = 0; i < _tabEntries.size(); i++)
        {
            const auto& tabInfo = _tabEntries[i];
            if (tabInfo.Contains(scenery))
            {
                return i;
            }
        }
        return std::nullopt;
    }

    void InitSceneryEntry(const ScenerySelection& selection, const ObjectEntryIndex sceneryGroupIndex)
    {
        Guard::ArgumentInRange<int32_t>(selection.EntryIndex, 0, OBJECT_ENTRY_INDEX_NULL);

        if (IsSceneryAvailableToBuild(selection))
        {
            // Get current tab
            const auto tabIndex = FindTabWithScenery(selection);

            // Add scenery to primary group (usually trees or path additions)
            if (sceneryGroupIndex != OBJECT_ENTRY_INDEX_NULL)
            {
                auto* tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
                if (tabInfo != nullptr)
                {
                    tabInfo->AddEntry(selection);
                    return;
                }
            }

            // If scenery is no tab, add it to misc
            if (!tabIndex.has_value())
            {
                auto* tabInfo = GetSceneryTabInfoForGroup(OBJECT_ENTRY_INDEX_NULL);
                if (tabInfo != nullptr)
                {
                    tabInfo->AddEntry(selection);
                }
            }
        }
    }

    void SortTabs()
    {
        std::sort(_tabEntries.begin(), _tabEntries.end(), [](const SceneryTabInfo& a, const SceneryTabInfo& b) {
            if (a.SceneryGroupIndex == b.SceneryGroupIndex)
                return false;

            if (a.SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
                return false;
            if (b.SceneryGroupIndex == OBJECT_ENTRY_INDEX_NULL)
                return true;

            const auto* entryA = a.GetSceneryGroupEntry();
            const auto* entryB = b.GetSceneryGroupEntry();
            return entryA->priority < entryB->priority;
        });
    }

    void PrepareWidgets()
    {
        // Add the base widgets
        _widgets.clear();
        for (const auto& widget : WindowSceneryBaseWidgets)
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

            _widgets.push_back(widget);
        }

        _widgets.push_back(lastWidget);

        widgets = _widgets.data();
    }

    ScenerySelection GetSceneryIdByCursorPos(const ScreenCoordsXY& screenCoords) const
    {
        ScenerySelection scenery{};

        const auto numColumns = GetNumColumns();
        const auto colIndex = screenCoords.x / SCENERY_BUTTON_WIDTH;
        const auto rowIndex = screenCoords.y / SCENERY_BUTTON_HEIGHT;
        if (colIndex >= 0 && colIndex < numColumns && rowIndex >= 0)
        {
            const auto tabSceneryIndex = static_cast<size_t>((rowIndex * numColumns) + colIndex);
            const auto tabIndex = gWindowSceneryActiveTabIndex;
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

    ScreenSize ContentScrollGetSize() const
    {
        auto rows = CountRows();
        return { 0, ContentRowsHeight(rows) };
    }

    void ContentScrollMouseDown(const ScreenCoordsXY& screenCoords)
    {
        const auto scenery = GetSceneryIdByCursorPos(screenCoords);
        if (scenery.IsUndefined())
            return;

        SetSelectedScenery(gWindowSceneryActiveTabIndex, scenery);

        gWindowSceneryPaintEnabled &= 0xFE;
        gWindowSceneryEyedropperEnabled = false;
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, windowPos.x + (width / 2));
        _hoverCounter = -16;
        gSceneryPlaceCost = MONEY32_UNDEFINED;
        Invalidate();
    }

    void ContentScrollMouseOver(const ScreenCoordsXY& screenCoords)
    {
        ScenerySelection scenery = GetSceneryIdByCursorPos(screenCoords);
        if (!scenery.IsUndefined())
        {
            _selectedScenery = scenery;
            Invalidate();
        }
    }

    std::pair<rct_string_id, money32> GetNameAndPrice(ScenerySelection selectedScenery)
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

    void DrawTabs(rct_drawpixelinfo& dpi, const ScreenCoordsXY& offset)
    {
        for (size_t tabIndex = 0; tabIndex < _tabEntries.size(); tabIndex++)
        {
            auto widgetIndex = static_cast<rct_widgetindex>(WIDX_SCENERY_TAB_1 + tabIndex);
            auto scgEntry = _tabEntries[tabIndex].GetSceneryGroupEntry();
            if (scgEntry != nullptr)
            {
                auto imageOffset = tabIndex == gWindowSceneryActiveTabIndex ? 1 : 0;
                auto imageId = ImageId(scgEntry->image + imageOffset, colours[1]);
                gfx_draw_sprite(&dpi, imageId, offset + ScreenCoordsXY{ widgets[widgetIndex].left, widgets[widgetIndex].top });
            }
        }
    }

    void DrawSceneryItem(rct_drawpixelinfo& dpi, ScenerySelection scenerySelection)
    {
        if (scenerySelection.SceneryType == SCENERY_TYPE_BANNER)
        {
            auto bannerEntry = get_banner_entry(scenerySelection.EntryIndex);
            auto imageId = ImageId(bannerEntry->image + gWindowSceneryRotation * 2, gWindowSceneryPrimaryColour);
            gfx_draw_sprite(&dpi, imageId, { 33, 40 });
            gfx_draw_sprite(&dpi, imageId.WithIndexOffset(1), { 33, 40 });
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_LARGE)
        {
            auto sceneryEntry = get_large_scenery_entry(scenerySelection.EntryIndex);
            auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                imageId = imageId.WithTertiary(gWindowSceneryTertiaryColour);
            gfx_draw_sprite(&dpi, imageId, { 33, 0 });
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_WALL)
        {
            auto wallEntry = get_wall_entry(scenerySelection.EntryIndex);
            auto imageId = ImageId(wallEntry->image);
            auto spriteTop = (wallEntry->height * 2) + 0x32;
            if (wallEntry->flags & WALL_SCENERY_HAS_GLASS)
            {
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
                }
                gfx_draw_sprite(&dpi, imageId, { 47, spriteTop });

                auto glassImageId = ImageId(wallEntry->image + 6).WithTransparancy(gWindowSceneryPrimaryColour);
                gfx_draw_sprite(&dpi, glassImageId, { 47, spriteTop });
            }
            else
            {
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_TERNARY_COLOUR)
                    {
                        imageId = imageId.WithTertiary(gWindowSceneryTertiaryColour);
                    }
                }
                gfx_draw_sprite(&dpi, imageId, { 47, spriteTop });

                if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
                {
                    gfx_draw_sprite(&dpi, imageId.WithIndexOffset(1), { 47, spriteTop });
                }
            }
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_PATH_ITEM)
        {
            auto* pathBitEntry = get_footpath_item_entry(scenerySelection.EntryIndex);
            auto imageId = ImageId(pathBitEntry->image);
            gfx_draw_sprite(&dpi, imageId, { 11, 16 });
        }
        else
        {
            auto sceneryEntry = get_small_scenery_entry(scenerySelection.EntryIndex);
            auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
            {
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                {
                    imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
                }
            }
            if (sceneryEntry->flags & SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
            {
                imageId = imageId.WithTertiary(gWindowSceneryTertiaryColour);
            }

            auto spriteTop = (sceneryEntry->height / 4) + 43;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                spriteTop -= 12;
            }

            gfx_draw_sprite(&dpi, imageId, { 32, spriteTop });

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation)
                              .WithTransparancy(gWindowSceneryPrimaryColour);
                gfx_draw_sprite(&dpi, imageId, { 32, spriteTop });
            }

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
            {
                imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation);
                gfx_draw_sprite(&dpi, imageId, { 32, spriteTop });
            }
        }
    }

    void ContentScrollDraw(rct_drawpixelinfo& dpi)
    {
        gfx_clear(&dpi, ColourMapA[colours[1]].mid_light);

        auto numColumns = GetNumColumns();
        auto tabIndex = gWindowSceneryActiveTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return;
        }

        ScreenCoordsXY topLeft{ 0, 0 };

        const auto& tabInfo = _tabEntries[tabIndex];
        for (size_t sceneryTabItemIndex = 0; sceneryTabItemIndex < tabInfo.Entries.size(); sceneryTabItemIndex++)
        {
            const auto& currentSceneryGlobal = tabInfo.Entries[sceneryTabItemIndex];
            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
            if (gWindowSceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled)
            {
                if (_selectedScenery == currentSceneryGlobal)
                {
                    gfx_fill_rect_inset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
            }
            else
            {
                if (tabSelectedScenery == currentSceneryGlobal)
                {
                    gfx_fill_rect_inset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
                }
                else if (_selectedScenery == currentSceneryGlobal)
                {
                    gfx_fill_rect_inset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
            }

            rct_drawpixelinfo clipdpi;
            if (clip_drawpixelinfo(
                    &clipdpi, &dpi, topLeft + ScreenCoordsXY{ 1, 1 }, SCENERY_BUTTON_WIDTH - 2, SCENERY_BUTTON_HEIGHT - 2))
            {
                DrawSceneryItem(clipdpi, currentSceneryGlobal);
            }

            topLeft.x += SCENERY_BUTTON_WIDTH;
            if (topLeft.x >= numColumns * SCENERY_BUTTON_WIDTH)
            {
                topLeft.y += SCENERY_BUTTON_HEIGHT;
                topLeft.x = 0;
            }
        }
    }
};

rct_window* WindowSceneryOpen()
{
    return WindowFocusOrCreate<SceneryWindow>(
        WC_SCENERY, ScreenCoordsXY(context_get_width() - WINDOW_SCENERY_WIDTH, 0x1D), WINDOW_SCENERY_WIDTH,
        WINDOW_SCENERY_HEIGHT, WF_NO_SCROLLING);
}

void WindowScenerySetSelectedItem(
    const ScenerySelection& scenery, const std::optional<colour_t> primary, const std::optional<colour_t> secondary,
    const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation)
{
    auto* w = static_cast<SceneryWindow*>(window_bring_to_front_by_class(WC_SCENERY));
    if (w != nullptr)
    {
        w->SetSelectedItem(scenery, primary, secondary, tertiary, rotation);
    }
}

void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
{
    // Should this bring to front?
    auto* w = static_cast<SceneryWindow*>(window_find_by_class(WC_SCENERY));
    if (w != nullptr)
    {
        return w->SetSelectedTab(sceneryGroupIndex);
    }
}

// Used after removing objects, in order to avoid crashes.
void WindowSceneryResetSelectedSceneryItems()
{
    auto* w = static_cast<SceneryWindow*>(window_find_by_class(WC_SCENERY));
    if (w != nullptr)
    {
        w->ResetSelectedSceneryItems();
    }
}

void WindowScenerySetDefaultPlacementConfiguration()
{
    auto* w = static_cast<SceneryWindow*>(window_find_by_class(WC_SCENERY));
    if (w != nullptr)
    {
        w->SetDefaultPlacementConfiguration();
    }
}

void WindowSceneryInit()
{
    auto* w = static_cast<SceneryWindow*>(window_find_by_class(WC_SCENERY));
    if (w != nullptr)
    {
        w->Init();
    }
}
