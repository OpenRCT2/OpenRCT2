/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <deque>
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
#include <openrct2/object/BannerSceneryEntry.h>
#include <openrct2/object/FootpathItemEntry.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/SceneryGroupEntry.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/sprites.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>

using namespace OpenRCT2;

static constexpr const StringId WINDOW_TITLE = STR_NONE;
constexpr int32_t WINDOW_SCENERY_MIN_WIDTH = 634;
constexpr int32_t WINDOW_SCENERY_MIN_HEIGHT = 195;
constexpr int32_t SCENERY_BUTTON_WIDTH = 66;
constexpr int32_t SCENERY_BUTTON_HEIGHT = 80;
constexpr int32_t TabWidth = 31;
constexpr int32_t TabHeight = 28;
constexpr int32_t MaxTabs = 257; // 255 selected tabs + misc + search
constexpr int32_t MaxTabsPerRow = 20;

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
    WIDX_FILTER_TEXT_BOX,
    WIDX_FILTER_CLEAR_BUTTON,
    WIDX_SCENERY_TAB_1,
};

validate_global_widx(WC_SCENERY, WIDX_SCENERY_TAB_1);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
validate_global_widx(WC_SCENERY, WIDX_SCENERY_EYEDROPPER_BUTTON);

// clang-format off
static Widget WindowSceneryBaseWidgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WINDOW_SCENERY_MIN_WIDTH, WINDOW_SCENERY_MIN_HEIGHT),
    MakeWidget     ({  0,  43}, {634, 99}, WindowWidgetType::Resize,    WindowColour::Secondary                                                  ), // 8         0x009DE2C8
    MakeWidget     ({  2,  62}, {607, 80}, WindowWidgetType::Scroll,    WindowColour::Secondary, SCROLL_VERTICAL                                 ), // 1000000   0x009DE418
    MakeWidget     ({609,  59}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_ROTATE_ARROW),    STR_ROTATE_OBJECTS_90      ), // 2000000   0x009DE428
    MakeWidget     ({609,  83}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_PAINTBRUSH),      STR_SCENERY_PAINTBRUSH_TIP ), // 4000000   0x009DE438
    MakeWidget     ({615,  108}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_COLOUR          ), // 8000000   0x009DE448
    MakeWidget     ({615, 120}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_SECONDARY_COLOUR), // 10000000  0x009DE458
    MakeWidget     ({615, 132}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_TERNARY_COLOUR  ), // 20000000  0x009DE468
    MakeWidget     ({609, 145}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_G2_EYEDROPPER),   STR_SCENERY_EYEDROPPER_TIP ), // 40000000  0x009DE478
    MakeWidget     ({609, 169}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_SCENERY_CLUSTER), STR_SCENERY_CLUSTER_TIP    ), // 40000000  0x009DE478
    MakeWidget     ({  4,  46}, {211, 14}, WindowWidgetType::TextBox,   WindowColour::Secondary                          ),
    MakeWidget     ({218,  46}, { 70, 14}, WindowWidgetType::Button,    WindowColour::Secondary, STR_OBJECT_SEARCH_CLEAR ),
    WIDGETS_END,
};
// clang-format on

// Persistent between window instances
static size_t _activeTabIndex;
static std::vector<ScenerySelection> _tabSelections;

uint8_t gWindowSceneryPaintEnabled;
uint8_t gWindowSceneryRotation;
colour_t gWindowSceneryPrimaryColour;
colour_t gWindowScenerySecondaryColour;
colour_t gWindowSceneryTertiaryColour;
bool gWindowSceneryEyedropperEnabled;

class SceneryWindow final : public Window
{
private:
    enum SceneryTabType
    {
        SCENERY_TAB_TYPE_GROUP,
        SCENERY_TAB_TYPE_MISC,
        SCENERY_TAB_TYPE_ALL,
    };

    struct SceneryItem
    {
        int32_t allRows;
        int32_t selected_item;
        ScenerySelection scenerySelection;
    };

    struct SceneryTabInfo
    {
        SceneryTabType Type = SCENERY_TAB_TYPE_GROUP;
        ObjectEntryIndex SceneryGroupIndex = OBJECT_ENTRY_INDEX_NULL;
        std::deque<ScenerySelection> Entries;
        u8string Filter = "";

        bool IsMisc() const
        {
            return Type == SCENERY_TAB_TYPE_MISC;
        }

        bool IsAll() const
        {
            return Type == SCENERY_TAB_TYPE_ALL;
        }

        bool IsSceneryGroup() const
        {
            return Type == SCENERY_TAB_TYPE_GROUP;
        }

        bool Contains(const ScenerySelection& entry) const
        {
            return std::find(std::begin(Entries), std::end(Entries), entry) != std::end(Entries);
        }

        void AddEntryToBack(const ScenerySelection& entry)
        {
            if (!Contains(entry))
            {
                if (toBack)
                    Entries.push_back(entry);
                else
                    Entries.push_front(entry);
            }
        }

        void AddEntryToFront(const ScenerySelection& entry)
        {
            if (!Contains(entry))
            {
                Entries.push_front(entry);
            }
        }

        const SceneryGroupEntry* GetSceneryGroupEntry() const
        {
            return OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(SceneryGroupIndex);
        }
    };

    std::vector<SceneryTabInfo> _tabEntries;
    std::vector<Widget> _widgets;
    int32_t _requiredWidth;
    int32_t _actualMinHeight;
    ScenerySelection _selectedScenery;
    int16_t _hoverCounter;
    SceneryTabInfo _filteredSceneryTab;

public:
    void OnOpen() override
    {
        Init();

        InitScrollWidgets();
        ContentUpdateScroll();
        ShowGridlines();
        gWindowSceneryRotation = 3;
        gSceneryCtrlPressed = false;
        gSceneryShiftPressed = false;
        _selectedScenery = {};
        _hoverCounter = 0;
        gSceneryGhostType = 0;
        gSceneryPlaceCost = MONEY32_UNDEFINED;
        gSceneryPlaceRotation = 0;
        gWindowSceneryPaintEnabled = 0; // repaint coloured scenery tool state
        gWindowSceneryEyedropperEnabled = false;

        width = GetRequiredWidth();
        min_width = width;
        max_width = width;
        height = _actualMinHeight;
        min_height = height;
        max_height = height;
        if (_activeTabIndex > _tabSelections.size())
        {
            _activeTabIndex = 0;
        }

        WindowMovePosition(*this, { ContextGetWidth() - GetRequiredWidth(), 0x1D });
        WindowPushOthersBelow(*this);
        LOG_INFO("Column count: %d", GetNumColumns());
    }

    void OnClose() override
    {
        SceneryRemoveGhostToolPlacement();
        HideGridlines();
        ViewportSetVisibility(0);

        if (gWindowSceneryScatterEnabled)
            WindowCloseByClass(WindowClass::SceneryScatter);

        if (SceneryToolIsActive())
            ToolCancel();
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_SCENERY_CLOSE:
                Close();
                break;
            case WIDX_SCENERY_ROTATE_OBJECTS_BUTTON:
                gWindowSceneryRotation++;
                gWindowSceneryRotation = gWindowSceneryRotation % 4;
                SceneryRemoveGhostToolPlacement();
                Invalidate();
                break;
            case WIDX_SCENERY_REPAINT_SCENERY_BUTTON:
                gWindowSceneryPaintEnabled ^= 1;
                gWindowSceneryEyedropperEnabled = false;
                if (gWindowSceneryScatterEnabled)
                    WindowCloseByClass(WindowClass::SceneryScatter);
                Invalidate();
                break;
            case WIDX_SCENERY_EYEDROPPER_BUTTON:
                gWindowSceneryPaintEnabled = 0;
                gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
                if (gWindowSceneryScatterEnabled)
                    WindowCloseByClass(WindowClass::SceneryScatter);
                SceneryRemoveGhostToolPlacement();
                Invalidate();
                break;
            case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
                gWindowSceneryPaintEnabled = 0;
                gWindowSceneryEyedropperEnabled = false;
                if (gWindowSceneryScatterEnabled)
                    WindowCloseByClass(WindowClass::SceneryScatter);
                else if (
                    NetworkGetMode() != NETWORK_MODE_CLIENT
                    || NetworkCanPerformCommand(NetworkGetCurrentPlayerGroupIndex(), -2))
                {
                    WindowSceneryScatterOpen();
                }
                else
                {
                    ContextShowError(STR_CANT_DO_THIS, STR_PERMISSION_DENIED, {});
                }
                Invalidate();
                break;
            case WIDX_FILTER_TEXT_BOX:
                WindowStartTextbox(*this, widgetIndex, STR_STRING, _filteredSceneryTab.Filter.data(), MAX_PATH);
                break;
            case WIDX_FILTER_CLEAR_BUTTON:
                _tabEntries[_activeTabIndex].Filter.clear();
                ContentUpdateScroll();
                scrolls->v_top = 0;
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
            WindowEventInvalidateCall(this);
            ContentUpdateScroll();
        }

        if (height > max_height)
        {
            Invalidate();
            height = max_height;
            Invalidate();
            // HACK: For some reason invalidate has not been called
            WindowEventInvalidateCall(this);
            ContentUpdateScroll();
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
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
            _activeTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
            Invalidate();
            gSceneryPlaceCost = MONEY32_UNDEFINED;

            ContentUpdateScroll();
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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
            const CursorState* state = ContextGetCursorState();
            WidgetIndex widgetIndex = WindowFindWidgetFromPoint(*this, state->position);
            if (widgetIndex == WIDX_SCENERY_LIST)
            {
                ScreenCoordsXY scrollPos = {};
                int32_t scrollArea = 0;
                int32_t scrollId = 0;
                WidgetScrollGetPart(*this, &widgets[WIDX_SCENERY_LIST], state->position, scrollPos, &scrollArea, &scrollId);
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
        const CursorState* state = ContextGetCursorState();
        WindowBase* other = WindowFindFromPoint(state->position);
        if (other == this)
        {
            ScreenCoordsXY window = state->position - ScreenCoordsXY{ windowPos.x - 26, windowPos.y };

            if (window.y < 44 || window.x <= width)
            {
                WidgetIndex widgetIndex = WindowFindWidgetFromPoint(*this, state->position);
                if (widgetIndex >= WIDX_SCENERY_TAB_CONTENT_PANEL)
                {
                    _hoverCounter++;
                    if (_hoverCounter < 8)
                    {
                        if (InputGetState() != InputState::ScrollLeft)
                        {
                            min_height = _actualMinHeight;
                            max_height = _actualMinHeight;
                        }
                    }
                    else
                    {
                        const auto& listWidget = widgets[WIDX_SCENERY_LIST];
                        const auto nonListHeight = height - listWidget.height() + 2;

                        const auto numRows = static_cast<int32_t>(CountRows());
                        const auto maxContentHeight = numRows * SCENERY_BUTTON_HEIGHT;
                        const auto maxWindowHeight = maxContentHeight + nonListHeight;
                        const auto windowHeight = std::clamp(maxWindowHeight, _actualMinHeight, 463);

                        min_height = windowHeight;
                        max_height = windowHeight;
                    }
                }
            }
        }
        else
        {
            _hoverCounter = 0;
            if (InputGetState() != InputState::ScrollLeft)
            {
                min_height = _actualMinHeight;
                max_height = _actualMinHeight;
            }
        }

        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            WindowUpdateTextboxCaret();
            WidgetInvalidate(*this, WIDX_FILTER_TEXT_BOX);
        }

        Invalidate();

        if (!SceneryToolIsActive())
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
            const auto tabIndex = _activeTabIndex;
            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
            if (!tabSelectedScenery.IsUndefined())
            {
                if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
                {
                    gCurrentToolId = Tool::EntranceDown;
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
                {
                    gCurrentToolId = static_cast<Tool>(
                        OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(tabSelectedScenery.EntryIndex)->tool_id);
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
                {
                    gCurrentToolId = static_cast<Tool>(
                        OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(tabSelectedScenery.EntryIndex)->tool_id);
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_PATH_ITEM)
                { // path bit
                    gCurrentToolId = static_cast<Tool>(
                        OpenRCT2::ObjectManager::GetObjectEntry<PathBitEntry>(tabSelectedScenery.EntryIndex)->tool_id);
                }
                else
                { // small scenery
                    gCurrentToolId = static_cast<Tool>(
                        OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(tabSelectedScenery.EntryIndex)->tool_id);
                }
            }
            else
            {
                gCurrentToolId = Tool::Arrow;
            }
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (widgetIndex != WIDX_FILTER_TEXT_BOX)
            return;

        if (text == _tabEntries[_activeTabIndex].Filter)
            return;

        _tabEntries[_activeTabIndex].Filter.assign(text);
        ContentUpdateScroll();

        scrolls->v_top = 0;
        Invalidate();
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

    OpenRCT2String OnTooltip(const WidgetIndex widgetIndex, const StringId fallback) override
    {
        if (widgetIndex >= WIDX_SCENERY_TAB_1)
        {
            const auto tabIndex = static_cast<size_t>(widgetIndex - WIDX_SCENERY_TAB_1);
            if (_tabEntries.size() > tabIndex)
            {
                auto ft = Formatter();
                const auto& tabInfo = _tabEntries[tabIndex];
                if (tabInfo.IsMisc())
                {
                    ft.Add<StringId>(STR_MISCELLANEOUS);
                    return { fallback, ft };
                }

                if (tabInfo.IsAll())
                {
                    ft.Add<StringId>(STR_ALL_SCENERY);
                    return { fallback, ft };
                }

                const auto* sceneryEntry = tabInfo.GetSceneryGroupEntry();
                if (sceneryEntry != nullptr)
                {
                    ft.Add<StringId>(sceneryEntry->name);
                    return { fallback, ft };
                }
            }
        }
        return { STR_NONE, Formatter() };
    }

    void OnPrepareDraw() override
    {
        // Set the window title
        StringId titleStringId = STR_MISCELLANEOUS;
        const auto tabIndex = _activeTabIndex;
        if (tabIndex < _tabEntries.size())
        {
            const auto& tabInfo = _tabEntries[tabIndex];
            if (tabInfo.IsAll())
            {
                titleStringId = STR_ALL_SCENERY;
            }
            else
            {
                const auto* sgEntry = tabInfo.GetSceneryGroupEntry();
                if (sgEntry != nullptr)
                {
                    titleStringId = sgEntry->name;
                }
            }
        }
        widgets[WIDX_SCENERY_TITLE].text = titleStringId;
        widgets[WIDX_FILTER_TEXT_BOX].string = _filteredSceneryTab.Filter.data();

        pressed_widgets = 0;
        pressed_widgets |= 1uLL << (tabIndex + WIDX_SCENERY_TAB_1);
        if (gWindowSceneryPaintEnabled == 1)
            pressed_widgets |= (1uLL << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
        if (gWindowSceneryEyedropperEnabled)
            pressed_widgets |= (1uLL << WIDX_SCENERY_EYEDROPPER_BUTTON);
        if (gWindowSceneryScatterEnabled)
            pressed_widgets |= (1uLL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

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

                auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(tabSelectedScenery.EntryIndex);
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

        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image = GetColourButtonImage(gWindowSceneryPrimaryColour);
        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image = GetColourButtonImage(gWindowScenerySecondaryColour);
        widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image = GetColourButtonImage(gWindowSceneryTertiaryColour);

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
                auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(tabSelectedScenery.EntryIndex);
                if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                {
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                }
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
            {
                auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(tabSelectedScenery.EntryIndex);

                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR
                    && !(sceneryEntry->flags & LARGE_SCENERY_FLAG_HIDE_PRIMARY_REMAP_BUTTON))
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR
                    && !(sceneryEntry->flags & LARGE_SCENERY_FLAG_HIDE_SECONDARY_REMAP_BUTTON))
                    widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                    widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_WALL)
            {
                auto* wallEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(tabSelectedScenery.EntryIndex);
                if (wallEntry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS))
                {
                    widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;

                        if (wallEntry->flags2 & WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR)
                            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
                        if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
                            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                    }
                }
            }
            else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
            {
                auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(tabSelectedScenery.EntryIndex);

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
            const auto lastTabIndex = GetMaxTabCountInARow() == MaxTabsPerRow ? MaxTabsPerRow - 1 : _tabEntries.size() - 1;
            const auto lastTabWidget = &widgets[WIDX_SCENERY_TAB_1 + lastTabIndex];
            windowWidth = std::max<int32_t>(windowWidth, lastTabWidget->right + 3);

            for (size_t index = 0; index < _tabEntries.size(); index++)
            {
                if (!_tabEntries[index].IsAll())
                    continue;

                auto searchTabWidget = &widgets[WIDX_SCENERY_TAB_1 + index];
                searchTabWidget->left = windowWidth - TabWidth - 2;
                searchTabWidget->right = windowWidth - 3;
                break;
            }
        }

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

    void OnDraw(DrawPixelInfo& dpi) override
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

            selectedSceneryEntry = GetSelectedScenery(_activeTabIndex);
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
        ft.Add<StringId>(name);
        DrawTextEllipsised(&dpi, { windowPos.x + 3, windowPos.y + height - 13 }, width - 19, STR_BLACK_STRING, ft);
    }

    void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
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

        _activeTabIndex = tabIndex.value();
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
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, ContextGetWidth() / 2);
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

    const ScenerySelection GetTabSelection()
    {
        return GetSelectedScenery(_activeTabIndex);
    }

    void Init()
    {
        _tabEntries.clear();

        for (ObjectEntryIndex scenerySetIndex = 0; scenerySetIndex < MaxTabs - ReservedTabCount; scenerySetIndex++)
        {
            const auto* sceneryGroupEntry = OpenRCT2::ObjectManager::GetObjectEntry<SceneryGroupEntry>(scenerySetIndex);
            if (sceneryGroupEntry != nullptr && SceneryGroupIsInvented(scenerySetIndex))
            {
                SceneryTabInfo tabInfo;
                tabInfo.SceneryGroupIndex = scenerySetIndex;
                for (const auto& sceneryEntry : sceneryGroupEntry->SceneryEntries)
                {
                    if (IsSceneryAvailableToBuild(sceneryEntry))
                    {
                        tabInfo.AddEntryToBack(sceneryEntry);
                    }
                }
                if (tabInfo.Entries.size() > 0)
                {
                    _tabEntries.push_back(std::move(tabInfo));
                }
            }
        }

        // Sort tabs before adding search and misc tabs to front/back
        SortTabs();

        // Add misc tab
        _tabEntries.emplace_back();
        _tabEntries.back().Type = SCENERY_TAB_TYPE_MISC;

        // Add search tab
        _tabEntries.emplace_back();
        _tabEntries.back().Type = SCENERY_TAB_TYPE_ALL;

        // small scenery
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_SMALL_SCENERY_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_SMALL, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // large scenery
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_LARGE_SCENERY_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_LARGE, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // walls
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_WALL_SCENERY_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_WALL, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // banners
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_BANNER_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_BANNER, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // path bits
        for (ObjectEntryIndex sceneryId = 0; sceneryId < MAX_PATH_ADDITION_OBJECTS; sceneryId++)
        {
            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathBitEntry>(sceneryId);
            if (sceneryEntry != nullptr)
            {
                InitSceneryEntry({ SCENERY_TYPE_PATH_ITEM, sceneryId }, sceneryEntry->scenery_tab_id);
            }
        }

        // Remove empty tabs
        for (auto it = _tabEntries.begin(); it != _tabEntries.end();)
        {
            if (it->Entries.size() == 0)
                it = _tabEntries.erase(it);
            else
                it++;
        }

        // Move all scenery tab to end of first row
        if (_tabEntries.size() > MaxTabsPerRow)
        {
            std::rotate(_tabEntries.begin() + MaxTabsPerRow - 1, _tabEntries.end() - 1, _tabEntries.end());
        }

        // Set required width
        _requiredWidth = std::min(static_cast<int32_t>(_tabEntries.size()), MaxTabsPerRow) * TabWidth + 5;

        PrepareWidgets();
        WindowInvalidateByClass(WindowClass::Scenery);
    }

    int32_t GetRequiredWidth() const
    {
        return std::max(_requiredWidth, WINDOW_SCENERY_MIN_WIDTH);
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
        const auto tabIndex = _activeTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return 0;
        }

        const auto totalItems = _filteredSceneryTab.Entries.size();
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
        const auto tabIndex = _activeTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return;
        }

        SetFilteredScenery(tabIndex);

        const int32_t listHeight = height - 14 - widgets[WIDX_SCENERY_LIST].top - 1;

        const auto sceneryItem = ContentCountRowsWithSelectedItem(tabIndex);
        scrolls[SceneryContentScrollIndex].v_bottom = ContentRowsHeight(sceneryItem.allRows) + 1;

        const int32_t maxTop = std::max(0, scrolls[SceneryContentScrollIndex].v_bottom - listHeight);
        auto rowSelected = CountRows(sceneryItem.selected_item);
        if (sceneryItem.scenerySelection.IsUndefined())
        {
            auto& oldScenery = GetSelectedScenery(tabIndex);
            if (!oldScenery.IsUndefined())
            {
                // Make sure last selected scenery for tab is always present
                _filteredSceneryTab.AddEntryToFront(oldScenery);
            }
            else
            {
                // No last selection was made, use blank ScenerySelection
                SetSelectedScenery(tabIndex, ScenerySelection());
                rowSelected = 0;
                if (!_filteredSceneryTab.Entries.empty())
                {
                    // Select first item in current filtered tab if available
                    const auto& scenery = _filteredSceneryTab.Entries.front();
                    if (!scenery.IsUndefined())
                    {
                        SetSelectedScenery(tabIndex, scenery);
                    }
                }
            }
        }

        scrolls[SceneryContentScrollIndex].v_top = ContentRowsHeight(rowSelected);
        scrolls[SceneryContentScrollIndex].v_top = std::min<int32_t>(maxTop, scrolls[SceneryContentScrollIndex].v_top);

        WidgetScrollUpdateThumbs(*this, WIDX_SCENERY_LIST);
    }

    SceneryItem ContentCountRowsWithSelectedItem(const size_t tabIndex)
    {
        SceneryItem sceneryItem = { 0, 0, ScenerySelection() };
        const auto scenerySelection = GetSelectedScenery(tabIndex);
        for (size_t i = 0; i < _filteredSceneryTab.Entries.size(); i++)
        {
            const auto& currentEntry = _filteredSceneryTab.Entries[i];
            if (currentEntry == scenerySelection)
            {
                sceneryItem.selected_item = static_cast<int32_t>(i);
                sceneryItem.scenerySelection = scenerySelection;
            }
        }
        sceneryItem.allRows = static_cast<int32_t>(CountRows(_filteredSceneryTab.Entries.size() + 8));
        return sceneryItem;
    }

    const ScenerySelection GetSelectedScenery(const size_t tabIndex)
    {
        if (_tabSelections.size() > tabIndex)
        {
            return _tabSelections[tabIndex];
        }
        return {};
    }

    void SetSelectedScenery(const size_t tabIndex, const ScenerySelection& value)
    {
        if (_tabSelections.size() <= tabIndex)
        {
            _tabSelections.resize(tabIndex + 1);
        }
        _tabSelections[tabIndex] = value;
    }

    SceneryTabInfo* GetSceneryTabInfoForMisc()
    {
        for (auto& tabEntry : _tabEntries)
        {
            if (tabEntry.IsMisc())
                return &tabEntry;
        }

        return nullptr;
    }

    SceneryTabInfo* GetSceneryTabInfoForSearch()
    {
        for (auto& tabEntry : _tabEntries)
        {
            if (tabEntry.IsAll())
                return &tabEntry;
        }

        return nullptr;
    }

    SceneryTabInfo* GetSceneryTabInfoForGroup(const ObjectEntryIndex sceneryGroupIndex)
    {
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
                    tabInfo->AddEntryToBack(selection);
                    return;
                }
            }

            // If scenery is no tab, add it to misc
            if (!tabIndex.has_value())
            {
                auto* tabInfo = GetSceneryTabInfoForMisc();
                if (tabInfo != nullptr)
                {
                    tabInfo->AddEntryToBack(selection);
                }
            }

            // Add all scenery to search tab
            auto tabInfo = GetSceneryTabInfoForSearch();
            if (tabInfo != nullptr)
            {
                tabInfo->AddEntryToBack(selection);
            }
        }
    }

    void SetFilteredScenery(const size_t tabIndex)
    {
        auto currentTab = _tabEntries[tabIndex];

        _filteredSceneryTab.Entries.clear();
        _filteredSceneryTab.Filter = currentTab.Filter;

        for (auto selection : currentTab.Entries)
        {
            if (MatchFilter(selection))
                _filteredSceneryTab.AddEntryToBack(selection);
        }
    }

    bool MatchFilter(const ScenerySelection& selection)
    {
        if (_filteredSceneryTab.Filter.empty())
            return true;

        auto& objManager = GetContext()->GetObjectManager();
        auto sceneryObjectType = GetObjectTypeFromSceneryType(selection.SceneryType);
        auto sceneryObject = objManager.GetLoadedObject(sceneryObjectType, selection.EntryIndex);

        return IsFilterInName(*sceneryObject) || IsFilterInAuthors(*sceneryObject) || IsFilterInIdentifier(*sceneryObject)
            || IsFilterInFilename(*sceneryObject);
    }

    bool IsFilterInName(const Object& object)
    {
        return String::Contains(object.GetName(), _filteredSceneryTab.Filter, true);
    }

    bool IsFilterInAuthors(const Object& object)
    {
        for (auto author : object.GetAuthors())
            if (String::Contains(author, _filteredSceneryTab.Filter, true))
                return true;

        return false;
    }

    bool IsFilterInIdentifier(const Object& object)
    {
        return String::Contains(object.GetIdentifier(), _filteredSceneryTab.Filter, true);
    }

    bool IsFilterInFilename(const Object& object)
    {
        auto repoItem = ObjectRepositoryFindObjectByEntry(&(object.GetObjectEntry()));
        return String::Contains(repoItem->Path, _filteredSceneryTab.Filter, true);
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

    int32_t GetTabRowCount()
    {
        int32_t tabEntries = static_cast<int32_t>(_tabEntries.size());
        return std::max<int32_t>((tabEntries + MaxTabsPerRow - 1) / MaxTabsPerRow, 0);
    }

    int32_t GetMaxTabCountInARow()
    {
        int32_t tabEntries = static_cast<int32_t>(_tabEntries.size());
        return std::min(tabEntries, MaxTabsPerRow);
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
        _actualMinHeight = WINDOW_SCENERY_MIN_HEIGHT;
        int32_t xInit = 3;
        int32_t tabsInThisRow = 0;

        ScreenCoordsXY pos = { xInit, 17 };
        for (const auto& tabInfo : _tabEntries)
        {
            auto widget = MakeTab(pos, STR_STRING_DEFINED_TOOLTIP);
            pos.x += TabWidth;

            if (tabInfo.IsMisc())
            {
                widget.image = ImageId(SPR_TAB_QUESTION, FilterPaletteID::PaletteNull);
            }

            _widgets.push_back(widget);

            tabsInThisRow++;
            if (tabsInThisRow >= MaxTabsPerRow)
            {
                pos.x = xInit;
                pos.y += TabHeight;
                tabsInThisRow = 0;
                _actualMinHeight += TabHeight;
            }
        }

        _widgets.push_back(lastWidget);

        // Shift base widgets based on number of tab rows
        int32_t shiftAmount = (GetTabRowCount() - 1) * TabHeight;
        if (shiftAmount > 0)
        {
            _widgets[WIDX_SCENERY_LIST].top += shiftAmount;
            _widgets[WIDX_SCENERY_LIST].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].top += shiftAmount;
            _widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].bottom += shiftAmount;
            _widgets[WIDX_FILTER_TEXT_BOX].top += shiftAmount;
            _widgets[WIDX_FILTER_TEXT_BOX].bottom += shiftAmount;
            _widgets[WIDX_FILTER_CLEAR_BUTTON].top += shiftAmount;
            _widgets[WIDX_FILTER_CLEAR_BUTTON].bottom += shiftAmount;
        }

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
            if (tabSceneryIndex < _filteredSceneryTab.Entries.size())
            {
                return _filteredSceneryTab.Entries[tabSceneryIndex];
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

        auto lastScenery = GetSelectedScenery(_activeTabIndex);
        if (lastScenery != scenery && !MatchFilter(lastScenery))
        {
            _filteredSceneryTab.Entries.pop_front();
        }
        SetSelectedScenery(_activeTabIndex, scenery);

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

    std::pair<StringId, money32> GetNameAndPrice(ScenerySelection selectedScenery)
    {
        StringId name = STR_UNKNOWN_OBJECT_TYPE;
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
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(selectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        price = sceneryEntry->price;
                        name = sceneryEntry->name;
                    }
                    break;
                }
                case SCENERY_TYPE_PATH_ITEM:
                {
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathBitEntry>(selectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        price = sceneryEntry->price;
                        name = sceneryEntry->name;
                    }
                    break;
                }
                case SCENERY_TYPE_WALL:
                {
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(selectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        price = sceneryEntry->price;
                        name = sceneryEntry->name;
                    }
                    break;
                }
                case SCENERY_TYPE_LARGE:
                {
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(selectedScenery.EntryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        price = sceneryEntry->price;
                        name = sceneryEntry->name;
                    }
                    break;
                }
                case SCENERY_TYPE_BANNER:
                {
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(
                        selectedScenery.EntryIndex);
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

    void DrawTabs(DrawPixelInfo& dpi, const ScreenCoordsXY& offset)
    {
        for (size_t tabIndex = 0; tabIndex < _tabEntries.size(); tabIndex++)
        {
            auto widgetIndex = static_cast<WidgetIndex>(WIDX_SCENERY_TAB_1 + tabIndex);
            auto widgetCoordsXY = ScreenCoordsXY(widgets[widgetIndex].left, widgets[widgetIndex].top);
            auto scgEntry = _tabEntries[tabIndex].GetSceneryGroupEntry();
            std::optional<ImageId> imageId = std::nullopt;
            auto imageOffset = tabIndex == _activeTabIndex ? TabImageOffsetSelected : TabImageOffsetUnselected;

            if (_tabEntries[tabIndex].IsAll())
            {
                imageId = ImageId(SPR_G2_ALL_UNSELECTED + imageOffset, colours[1]);
            }
            else if (scgEntry != nullptr)
            {
                imageId = ImageId(scgEntry->image + imageOffset, colours[1]);
            }

            if (imageId.has_value())
            {
                GfxDrawSprite(&dpi, imageId.value(), offset + widgetCoordsXY);
            }
        }
    }

    void DrawSceneryItem(DrawPixelInfo& dpi, ScenerySelection scenerySelection)
    {
        if (scenerySelection.SceneryType == SCENERY_TYPE_BANNER)
        {
            auto bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(scenerySelection.EntryIndex);
            auto imageId = ImageId(bannerEntry->image + gWindowSceneryRotation * 2, gWindowSceneryPrimaryColour);
            GfxDrawSprite(&dpi, imageId, { 33, 40 });
            GfxDrawSprite(&dpi, imageId.WithIndexOffset(1), { 33, 40 });
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_LARGE)
        {
            auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(scenerySelection.EntryIndex);
            auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
            if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                imageId = imageId.WithTertiary(gWindowSceneryTertiaryColour);
            GfxDrawSprite(&dpi, imageId, { 33, 0 });
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_WALL)
        {
            auto wallEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(scenerySelection.EntryIndex);
            auto imageId = ImageId(wallEntry->image);
            auto spriteTop = (wallEntry->height * 2) + 0x32;
            if (wallEntry->flags & WALL_SCENERY_HAS_GLASS)
            {
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
                }
                GfxDrawSprite(&dpi, imageId, { 47, spriteTop });

                auto glassImageId = ImageId(wallEntry->image + 6).WithTransparency(gWindowSceneryPrimaryColour);
                GfxDrawSprite(&dpi, glassImageId, { 47, spriteTop });
            }
            else
            {
                imageId = imageId.WithPrimary(gWindowSceneryPrimaryColour);
                if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                {
                    imageId = imageId.WithSecondary(gWindowScenerySecondaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
                    {
                        imageId = imageId.WithTertiary(gWindowSceneryTertiaryColour);
                    }
                }
                GfxDrawSprite(&dpi, imageId, { 47, spriteTop });

                if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
                {
                    GfxDrawSprite(&dpi, imageId.WithIndexOffset(1), { 47, spriteTop });
                }
            }
        }
        else if (scenerySelection.SceneryType == SCENERY_TYPE_PATH_ITEM)
        {
            auto* pathBitEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathBitEntry>(scenerySelection.EntryIndex);
            auto imageId = ImageId(pathBitEntry->image);
            GfxDrawSprite(&dpi, imageId, { 11, 16 });
        }
        else
        {
            auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(scenerySelection.EntryIndex);
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

            GfxDrawSprite(&dpi, imageId, { 32, spriteTop });

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation)
                              .WithTransparency(gWindowSceneryPrimaryColour);
                GfxDrawSprite(&dpi, imageId, { 32, spriteTop });
            }

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
            {
                imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation);
                GfxDrawSprite(&dpi, imageId, { 32, spriteTop });
            }
        }
    }

    void ContentScrollDraw(DrawPixelInfo& dpi)
    {
        GfxClear(&dpi, ColourMapA[colours[1]].mid_light);

        auto numColumns = GetNumColumns();
        auto tabIndex = _activeTabIndex;
        if (tabIndex >= _tabEntries.size())
        {
            return;
        }

        ScreenCoordsXY topLeft{ 0, 0 };

        for (size_t sceneryTabItemIndex = 0; sceneryTabItemIndex < _filteredSceneryTab.Entries.size(); sceneryTabItemIndex++)
        {
            const auto& currentSceneryGlobal = _filteredSceneryTab.Entries[sceneryTabItemIndex];
            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
            if (gWindowSceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled)
            {
                if (_selectedScenery == currentSceneryGlobal)
                {
                    GfxFillRectInset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
            }
            else
            {
                if (tabSelectedScenery == currentSceneryGlobal)
                {
                    GfxFillRectInset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
                }
                else if (_selectedScenery == currentSceneryGlobal)
                {
                    GfxFillRectInset(
                        &dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                        colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
            }

            DrawPixelInfo clipdpi;
            if (ClipDrawPixelInfo(
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

WindowBase* WindowSceneryOpen()
{
    auto* w = static_cast<SceneryWindow*>(WindowBringToFrontByClass(WindowClass::Scenery));
    if (w == nullptr)
    {
        w = WindowCreate<SceneryWindow>(WindowClass::Scenery);
    }
    return w;
}

void WindowScenerySetSelectedItem(
    const ScenerySelection& scenery, const std::optional<colour_t> primary, const std::optional<colour_t> secondary,
    const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation)
{
    auto* w = static_cast<SceneryWindow*>(WindowBringToFrontByClass(WindowClass::Scenery));
    if (w != nullptr)
    {
        w->SetSelectedItem(scenery, primary, secondary, tertiary, rotation);
    }
}

void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
{
    // Should this bring to front?
    auto* w = static_cast<SceneryWindow*>(WindowFindByClass(WindowClass::Scenery));
    if (w != nullptr)
    {
        return w->SetSelectedTab(sceneryGroupIndex);
    }
}

// Used after removing objects, in order to avoid crashes.
void WindowSceneryResetSelectedSceneryItems()
{
    _tabSelections.clear();
    _activeTabIndex = 0;
}

void WindowScenerySetDefaultPlacementConfiguration()
{
    gWindowSceneryRotation = 3;
    gWindowSceneryPrimaryColour = COLOUR_BORDEAUX_RED;
    gWindowScenerySecondaryColour = COLOUR_YELLOW;
    gWindowSceneryTertiaryColour = COLOUR_DARK_BROWN;

    WindowSceneryResetSelectedSceneryItems();
}

const ScenerySelection WindowSceneryGetTabSelection()
{
    auto* w = static_cast<SceneryWindow*>(WindowFindByClass(WindowClass::Scenery));
    if (w != nullptr)
    {
        return w->GetTabSelection();
    }
    else
    {
        return {};
    }
}

void WindowSceneryInit()
{
    auto* w = static_cast<SceneryWindow*>(WindowFindByClass(WindowClass::Scenery));
    if (w != nullptr)
    {
        w->Init();
    }
}
