/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <deque>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/BannerPlaceAction.h>
#include <openrct2/actions/BannerSetColourAction.h>
#include <openrct2/actions/FootpathAdditionPlaceAction.h>
#include <openrct2/actions/LargeSceneryPlaceAction.h>
#include <openrct2/actions/LargeScenerySetColourAction.h>
#include <openrct2/actions/ScenerySetRestrictedAction.h>
#include <openrct2/actions/SmallSceneryPlaceAction.h>
#include <openrct2/actions/SmallScenerySetColourAction.h>
#include <openrct2/actions/WallPlaceAction.h>
#include <openrct2/actions/WallSetColourAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Research.h>
#include <openrct2/network/network.h>
#include <openrct2/object/BannerSceneryEntry.h>
#include <openrct2/object/LargeSceneryEntry.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/PathAdditionEntry.h>
#include <openrct2/object/SceneryGroupEntry.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/ConstructionClearance.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/BannerElement.h>
#include <openrct2/world/tile_element/LargeSceneryElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/SmallSceneryElement.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/WallElement.h>

using namespace OpenRCT2::Numerics;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = kStringIdNone;
    constexpr int32_t WINDOW_SCENERY_MIN_WIDTH = 634;
    constexpr int32_t WINDOW_SCENERY_MIN_HEIGHT = 195;
    constexpr int32_t SCENERY_BUTTON_WIDTH = 66;
    constexpr int32_t SCENERY_BUTTON_HEIGHT = 80;
    constexpr int32_t InitTabPosX = 3;
    constexpr int32_t InitTabPosY = 17;
    constexpr int32_t TabWidth = 31;
    constexpr int32_t TabHeight = 28;
    constexpr int32_t ReservedTabCount = 2;
    constexpr int32_t MaxTabs = 257; // 255 selected tabs + misc + all
    constexpr int32_t MaxTabsPerRow = 20;

    constexpr uint8_t SceneryContentScrollIndex = 0;

    enum WindowSceneryListWidgetIdx : WidgetIndex
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
        WIDX_RESTRICT_SCENERY,
        WIDX_SCENERY_TAB_1,
    };

    validate_global_widx(WC_SCENERY, WIDX_SCENERY_BACKGROUND);
    validate_global_widx(WC_SCENERY, WIDX_SCENERY_TAB_1);
    validate_global_widx(WC_SCENERY, WIDX_SCENERY_ROTATE_OBJECTS_BUTTON);
    validate_global_widx(WC_SCENERY, WIDX_SCENERY_EYEDROPPER_BUTTON);

    // clang-format off
    static constexpr Widget WindowSceneryBaseWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WINDOW_SCENERY_MIN_WIDTH, WINDOW_SCENERY_MIN_HEIGHT),
        MakeWidget     ({  0,  43}, {634, 99}, WindowWidgetType::Resize,    WindowColour::Secondary                                                  ), // 8         0x009DE2C8
        MakeWidget     ({  2,  62}, {607, 80}, WindowWidgetType::Scroll,    WindowColour::Secondary, SCROLL_VERTICAL                                 ), // 1000000   0x009DE418
        MakeWidget     ({609,  59}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_ROTATE_ARROW),    STR_ROTATE_OBJECTS_90      ), // 2000000   0x009DE428
        MakeWidget     ({609,  83}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_PAINTBRUSH),      STR_SCENERY_PAINTBRUSH_TIP ), // 4000000   0x009DE438
        MakeWidget     ({615,  108}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_COLOUR          ), // 8000000   0x009DE448
        MakeWidget     ({615, 120}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_SECONDARY_COLOUR), // 10000000  0x009DE458
        MakeWidget     ({615, 132}, { 12, 12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, 0xFFFFFFFF,          STR_SELECT_TERTIARY_COLOUR  ), // 20000000  0x009DE468
        MakeWidget     ({609, 145}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_G2_EYEDROPPER),   STR_SCENERY_EYEDROPPER_TIP ), // 40000000  0x009DE478
        MakeWidget     ({609, 169}, { 24, 24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_SCENERY_CLUSTER), STR_SCENERY_CLUSTER_TIP    ), // 40000000  0x009DE478
        MakeWidget     ({  4,  46}, {211, 14}, WindowWidgetType::TextBox,   WindowColour::Secondary                          ),
        MakeWidget     ({218,  46}, { 70, 14}, WindowWidgetType::Button,    WindowColour::Secondary, STR_OBJECT_SEARCH_CLEAR ),
        MakeWidget     ({539,  46}, { 70, 14}, WindowWidgetType::Button,    WindowColour::Secondary, STR_RESTRICT_SCENERY,   STR_RESTRICT_SCENERY_TIP ),
    };
    // clang-format on

    // Persistent between window instances
    static size_t _activeTabIndex;
    static std::vector<ScenerySelection> _tabSelections;

    static bool _sceneryPaintEnabled;
    static colour_t _sceneryPrimaryColour;
    static colour_t _scenerySecondaryColour;
    static colour_t _sceneryTertiaryColour;

    uint8_t gWindowSceneryRotation;
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
            ObjectEntryIndex SceneryGroupIndex = kObjectEntryIndexNull;
            std::deque<ScenerySelection> Entries{};
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
                    Entries.push_back(entry);
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
        int32_t _requiredWidth;
        int32_t _actualMinHeight;
        ScenerySelection _selectedScenery;
        int16_t _hoverCounter;
        SceneryTabInfo _filteredSceneryTab;

        uint8_t _unkF64F0E{ 0 };
        int16_t _unkF64F0A{ 0 };

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
            gSceneryPlaceCost = kMoney64Undefined;
            gSceneryPlaceRotation = 0;
            _sceneryPaintEnabled = false; // repaint coloured scenery tool state
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
        }

        void OnClose() override
        {
            SceneryRemoveGhostToolPlacement();
            HideGridlines();
            ViewportSetVisibility(ViewportVisibility::Default);

            if (gWindowSceneryScatterEnabled)
            {
                auto* windowMgr = Ui::GetWindowManager();
                windowMgr->CloseByClass(WindowClass::SceneryScatter);
            }

            if (isToolActive(WindowClass::Scenery))
                ToolCancel();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            auto* windowMgr = Ui::GetWindowManager();

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
                    _sceneryPaintEnabled ^= true;
                    gWindowSceneryEyedropperEnabled = false;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::SceneryScatter);
                    Invalidate();
                    break;
                case WIDX_SCENERY_EYEDROPPER_BUTTON:
                    _sceneryPaintEnabled = false;
                    gWindowSceneryEyedropperEnabled = !gWindowSceneryEyedropperEnabled;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::SceneryScatter);
                    SceneryRemoveGhostToolPlacement();
                    Invalidate();
                    break;
                case WIDX_SCENERY_BUILD_CLUSTER_BUTTON:
                    _sceneryPaintEnabled = false;
                    gWindowSceneryEyedropperEnabled = false;
                    if (gWindowSceneryScatterEnabled)
                        windowMgr->CloseByClass(WindowClass::SceneryScatter);
                    else if (
                        NetworkGetMode() != NETWORK_MODE_CLIENT
                        || NetworkCanPerformCommand(NetworkGetCurrentPlayerGroupIndex(), -2))
                    {
                        SceneryScatterOpen();
                    }
                    else
                    {
                        ContextShowError(STR_CANT_DO_THIS, STR_PERMISSION_DENIED, {});
                    }
                    Invalidate();
                    break;
                case WIDX_FILTER_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _filteredSceneryTab.Filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR_BUTTON:
                    _tabEntries[_activeTabIndex].Filter.clear();
                    ContentUpdateScroll();
                    scrolls->contentOffsetY = 0;
                    Invalidate();
                    break;
                case WIDX_RESTRICT_SCENERY:
                {
                    const auto tabIndex = _activeTabIndex;
                    const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
                    if (!tabSelectedScenery.IsUndefined())
                    {
                        const auto newStatus = !IsSceneryItemRestricted(tabSelectedScenery);
                        const auto objectType = GetObjectTypeFromSceneryType(tabSelectedScenery.SceneryType);
                        auto action = ScenerySetRestrictedAction(objectType, tabSelectedScenery.EntryIndex, newStatus);
                        GameActions::Execute(&action);
                    }
                    break;
                }
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
                OnPrepareDraw();
                ContentUpdateScroll();
            }

            if (height > max_height)
            {
                Invalidate();
                height = max_height;
                Invalidate();
                // HACK: For some reason invalidate has not been called
                OnPrepareDraw();
                ContentUpdateScroll();
            }

            ResizeFrameWithPage();
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCENERY_PRIMARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _sceneryPrimaryColour);
                    break;
                case WIDX_SCENERY_SECONDARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _scenerySecondaryColour);
                    break;
                case WIDX_SCENERY_TERTIARY_COLOUR_BUTTON:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], _sceneryTertiaryColour);
                    break;
            }

            if (widgetIndex >= WIDX_SCENERY_TAB_1)
            {
                _activeTabIndex = widgetIndex - WIDX_SCENERY_TAB_1;
                Invalidate();
                gSceneryPlaceCost = kMoney64Undefined;

                ContentUpdateScroll();
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            if (widgetIndex == WIDX_SCENERY_PRIMARY_COLOUR_BUTTON)
            {
                _sceneryPrimaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }
            else if (widgetIndex == WIDX_SCENERY_SECONDARY_COLOUR_BUTTON)
            {
                _scenerySecondaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }
            else if (widgetIndex == WIDX_SCENERY_TERTIARY_COLOUR_BUTTON)
            {
                _sceneryTertiaryColour = ColourDropDownIndexToColour(dropdownIndex);
            }

            Invalidate();
        }

        void OnPeriodicUpdate() override
        {
            if (!_selectedScenery.IsUndefined())
            {
                // Find out what scenery the cursor is over
                const CursorState* state = ContextGetCursorState();

                auto* windowMgr = GetWindowManager();
                WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*this, state->position);
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
                // This will happen when the mouse leaves the scroll window and is required so that the cost and description
                // switch to the tool scenery selection.
                _selectedScenery = {};
            }
        }

        void OnUpdate() override
        {
            const CursorState* state = ContextGetCursorState();

            auto* windowMgr = GetWindowManager();
            WindowBase* other = windowMgr->FindFromPoint(state->position);
            if (other == this)
            {
                ScreenCoordsXY window = state->position - ScreenCoordsXY{ windowPos.x - 26, windowPos.y };

                if (window.y < 44 || window.x <= width)
                {
                    WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*this, state->position);
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
                            const auto nonListHeight = height - listWidget.height() + 12;

                            const auto numRows = static_cast<int32_t>(CountRows());
                            const auto maxContentHeight = numRows * SCENERY_BUTTON_HEIGHT;
                            const auto maxWindowHeight = maxContentHeight + nonListHeight;
                            const auto windowHeight = std::clamp(maxWindowHeight, _actualMinHeight, 473);

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

            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_FILTER_TEXT_BOX);
            }

            Invalidate();

            if (!isToolActive(WindowClass::Scenery))
            {
                Close();
                return;
            }

            if (gWindowSceneryEyedropperEnabled)
            {
                gCurrentToolId = Tool::Crosshair;
            }
            else if (_sceneryPaintEnabled)
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
                    {
                        gCurrentToolId = static_cast<Tool>(
                            OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(tabSelectedScenery.EntryIndex)->tool_id);
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

            scrolls->contentOffsetY = 0;
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
                    const auto& tabInfo = _tabEntries[tabIndex];
                    if (tabInfo.IsMisc())
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_MISCELLANEOUS);
                        return { fallback, ft };
                    }

                    if (tabInfo.IsAll())
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_ALL_SCENERY);
                        return { fallback, ft };
                    }

                    const auto* sceneryEntry = tabInfo.GetSceneryGroupEntry();
                    if (sceneryEntry != nullptr)
                    {
                        auto ft = Formatter();
                        ft.Add<StringId>(sceneryEntry->name);
                        return { fallback, ft };
                    }
                }
            }
            return { kStringIdNone, Formatter() };
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
            if (_sceneryPaintEnabled)
                pressed_widgets |= (1uLL << WIDX_SCENERY_REPAINT_SCENERY_BUTTON);
            if (gWindowSceneryEyedropperEnabled)
                pressed_widgets |= (1uLL << WIDX_SCENERY_EYEDROPPER_BUTTON);
            if (gWindowSceneryScatterEnabled)
                pressed_widgets |= (1uLL << WIDX_SCENERY_BUILD_CLUSTER_BUTTON);

            widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_RESTRICT_SCENERY].type = WindowWidgetType::Empty;

            const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
            if (!tabSelectedScenery.IsUndefined())
            {
                if (tabSelectedScenery.SceneryType == SCENERY_TYPE_SMALL)
                {
                    widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].type = WindowWidgetType::FlatBtn;

                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(
                        tabSelectedScenery.EntryIndex);
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                    {
                        widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
                    }
                }
                else if (tabSelectedScenery.SceneryType >= SCENERY_TYPE_LARGE)
                {
                    widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].type = WindowWidgetType::FlatBtn;
                }

                if ((gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR) || GetGameState().Cheats.sandboxMode)
                {
                    widgets[WIDX_RESTRICT_SCENERY].type = WindowWidgetType::Button;
                    if (IsSceneryItemRestricted(tabSelectedScenery))
                        pressed_widgets |= (1uLL << WIDX_RESTRICT_SCENERY);
                    else
                        pressed_widgets &= ~(1uLL << WIDX_RESTRICT_SCENERY);
                }
            }

            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].image = GetColourButtonImage(_sceneryPrimaryColour);
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].image = GetColourButtonImage(_scenerySecondaryColour);
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].image = GetColourButtonImage(_sceneryTertiaryColour);

            widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;
            widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::Empty;

            if (_sceneryPaintEnabled)
            { // repaint coloured scenery tool is on
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
            }
            else if (!tabSelectedScenery.IsUndefined())
            {
                if (tabSelectedScenery.SceneryType == SCENERY_TYPE_BANNER)
                {
                    auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(
                        tabSelectedScenery.EntryIndex);
                    if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                    {
                        widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].type = WindowWidgetType::ColourBtn;
                    }
                }
                else if (tabSelectedScenery.SceneryType == SCENERY_TYPE_LARGE)
                {
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(
                        tabSelectedScenery.EntryIndex);

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
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(
                        tabSelectedScenery.EntryIndex);

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

                if (GetSceneryTabInfoForMisc() != nullptr)
                {
                    auto miscTabWidget = &widgets[WIDX_SCENERY_TAB_1 + _tabEntries.size() - 2];
                    miscTabWidget->left = windowWidth - 2 * TabWidth - 6;
                    miscTabWidget->right = windowWidth - TabWidth - 7;
                    miscTabWidget->top = InitTabPosY;
                    miscTabWidget->bottom = InitTabPosY + TabHeight;
                }

                if (_tabEntries.back().IsAll())
                {
                    auto allTabWidget = &widgets[WIDX_SCENERY_TAB_1 + _tabEntries.size() - 1];
                    allTabWidget->left = windowWidth - TabWidth - 6;
                    allTabWidget->right = windowWidth - 7;
                    allTabWidget->top = InitTabPosY;
                    allTabWidget->bottom = InitTabPosY + TabHeight;
                }
            }

            ResizeFrameWithPage();
            widgets[WIDX_SCENERY_LIST].right = windowWidth - 26;
            widgets[WIDX_SCENERY_LIST].bottom = height - 24;

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
                if (_sceneryPaintEnabled) // repaint coloured scenery tool is on
                    return;
                if (gWindowSceneryEyedropperEnabled)
                    return;

                selectedSceneryEntry = GetSelectedScenery(_activeTabIndex);
                if (selectedSceneryEntry.IsUndefined())
                    return;
            }

            auto [name, price] = GetNameAndPrice(selectedSceneryEntry);
            if (price != kMoney64Undefined && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(price);

                // -14
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ width - 0x1A, height - 13 }, STR_COST_LABEL, ft, { TextAlignment::RIGHT });
            }

            auto ft = Formatter();
            ft.Add<StringId>(name);
            DrawTextEllipsised(dpi, { windowPos.x + 3, windowPos.y + height - 23 }, width - 19, STR_BLACK_STRING, ft);

            // Draw object author(s) if debugging tools are active
            if (Config::Get().general.DebuggingTools)
            {
                auto sceneryObjectType = GetObjectTypeFromSceneryType(selectedSceneryEntry.SceneryType);
                auto& objManager = GetContext()->GetObjectManager();
                auto sceneryObject = objManager.GetLoadedObject(sceneryObjectType, selectedSceneryEntry.EntryIndex);
                if (sceneryObject != nullptr && sceneryObject->GetAuthors().size() > 0)
                {
                    std::string authorsString;
                    const auto& authors = sceneryObject->GetAuthors();
                    for (size_t i = 0; i < authors.size(); ++i)
                    {
                        if (i > 0)
                        {
                            authorsString.append(", ");
                        }
                        authorsString.append(authors[i]);
                    }
                    ft = Formatter();
                    ft.Add<const char*>(authorsString.c_str());
                    DrawTextEllipsised(
                        dpi, windowPos + ScreenCoordsXY{ 3, height - 13 }, width - 19,
                        (sceneryObject->GetAuthors().size() == 1 ? STR_SCENERY_AUTHOR : STR_SCENERY_AUTHOR_PLURAL), ft);
                }
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            if (scrollIndex == SceneryContentScrollIndex)
            {
                ContentScrollDraw(dpi);
            }
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCENERY_BACKGROUND:
                    ToolUpdateScenery(screenCoords);
                    break;
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCENERY_BACKGROUND:
                    SceneryToolDown(screenCoords, widgetIndex);
                    break;
            }
        }

        void OnToolDrag(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCENERY_BACKGROUND:
                    if (_sceneryPaintEnabled)
                        SceneryToolDown(screenCoords, widgetIndex);
                    if (gWindowSceneryEyedropperEnabled)
                        SceneryToolDown(screenCoords, widgetIndex);
                    break;
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
                _sceneryPrimaryColour = primary.value();
            }
            if (secondary.has_value())
            {
                _scenerySecondaryColour = secondary.value();
            }
            if (tertiary.has_value())
            {
                _sceneryTertiaryColour = tertiary.value();
            }
            if (rotation.has_value())
            {
                gWindowSceneryRotation = rotation.value();
            }
            gWindowSceneryEyedropperEnabled = false;
            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, ContextGetWidth() / 2);
            _hoverCounter = -16;
            gSceneryPlaceCost = kMoney64Undefined;
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

            // Sort scenery group tabs before adding other tabs
            SortTabs();

            // Add misc and all tab
            _tabEntries.emplace_back(SceneryWindow::SceneryTabInfo{ SCENERY_TAB_TYPE_MISC });
            _tabEntries.emplace_back(SceneryWindow::SceneryTabInfo{ SCENERY_TAB_TYPE_ALL });

            // small scenery
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxSmallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    InitSceneryEntry({ SCENERY_TYPE_SMALL, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // large scenery
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxLargeSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    InitSceneryEntry({ SCENERY_TYPE_LARGE, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // walls
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxWallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    InitSceneryEntry({ SCENERY_TYPE_WALL, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // banners
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxBannerObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    InitSceneryEntry({ SCENERY_TYPE_BANNER, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxPathAdditionObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    InitSceneryEntry({ SCENERY_TYPE_PATH_ITEM, sceneryId }, sceneryEntry->scenery_tab_id);
                }
            }

            // Remove empty tabs
            _tabEntries.erase(
                std::remove_if(
                    _tabEntries.begin(), _tabEntries.end(),
                    [](const SceneryTabInfo& tabInfo) { return tabInfo.Entries.empty(); }),
                _tabEntries.end());

            // Set required width
            _requiredWidth = std::min(static_cast<int32_t>(_tabEntries.size()), MaxTabsPerRow) * TabWidth + 5;

            PrepareWidgets();

            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->InvalidateByClass(WindowClass::Scenery);
        }

        int32_t GetRequiredWidth() const
        {
            return std::max(_requiredWidth, WINDOW_SCENERY_MIN_WIDTH);
        }

    private:
        int32_t GetNumColumns() const
        {
            const auto& listWidget = widgets[WIDX_SCENERY_LIST];
            const auto contentWidth = listWidget.width() - kScrollBarWidth;
            return contentWidth / SCENERY_BUTTON_WIDTH;
        }

        template<typename T>
        T CountRows(T items) const
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
            scrolls[SceneryContentScrollIndex].contentHeight = ContentRowsHeight(sceneryItem.allRows) + 1;

            const int32_t maxTop = std::max(0, scrolls[SceneryContentScrollIndex].contentHeight - listHeight);
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

            scrolls[SceneryContentScrollIndex].contentOffsetY = ContentRowsHeight(rowSelected);
            scrolls[SceneryContentScrollIndex].contentOffsetY = std::min<int32_t>(
                maxTop, scrolls[SceneryContentScrollIndex].contentOffsetY);

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
            if (_tabEntries.size() >= 2)
            {
                if (_tabEntries[_tabEntries.size() - 2].IsMisc())
                    return &_tabEntries[_tabEntries.size() - 2];
            }

            return nullptr;
        }

        SceneryTabInfo* GetSceneryTabInfoForAll()
        {
            if (!_tabEntries.empty())
            {
                if (_tabEntries.back().IsAll())
                    return &_tabEntries.back();
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
                if (tabInfo.IsAll())
                {
                    // The scenery will be always added here so exclude this one.
                    continue;
                }
                if (tabInfo.Contains(scenery))
                {
                    return i;
                }
            }
            return std::nullopt;
        }

        void InitSceneryEntry(const ScenerySelection& selection, const ObjectEntryIndex sceneryGroupIndex)
        {
            Guard::ArgumentInRange<int32_t>(selection.EntryIndex, 0, kObjectEntryIndexNull);

            if (IsSceneryAvailableToBuild(selection))
            {
                // Add scenery to all tab
                auto* allTabInfo = GetSceneryTabInfoForAll();
                if (allTabInfo != nullptr)
                {
                    allTabInfo->AddEntryToBack(selection);
                }

                // Add scenery to primary group (usually trees or path additions)
                if (sceneryGroupIndex != kObjectEntryIndexNull)
                {
                    auto* tabInfo = GetSceneryTabInfoForGroup(sceneryGroupIndex);
                    if (tabInfo != nullptr)
                    {
                        tabInfo->AddEntryToBack(selection);
                        return;
                    }
                }

                // If scenery has no tab, add it to misc
                const auto tabIndex = FindTabWithScenery(selection);
                if (!tabIndex.has_value())
                {
                    auto* tabInfo = GetSceneryTabInfoForMisc();
                    if (tabInfo != nullptr)
                    {
                        tabInfo->AddEntryToBack(selection);
                    }
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
            return String::contains(object.GetName(), _filteredSceneryTab.Filter, true);
        }

        bool IsFilterInAuthors(const Object& object)
        {
            for (auto author : object.GetAuthors())
                if (String::contains(author, _filteredSceneryTab.Filter, true))
                    return true;

            return false;
        }

        bool IsFilterInIdentifier(const Object& object)
        {
            return String::contains(object.GetIdentifier(), _filteredSceneryTab.Filter, true);
        }

        bool IsFilterInFilename(const Object& object)
        {
            auto repoItem = ObjectRepositoryFindObjectByEntry(&(object.GetObjectEntry()));
            return String::contains(repoItem->Path, _filteredSceneryTab.Filter, true);
        }

        void SortTabs()
        {
            std::sort(_tabEntries.begin(), _tabEntries.end(), [](const SceneryTabInfo& a, const SceneryTabInfo& b) {
                if (a.SceneryGroupIndex == b.SceneryGroupIndex)
                    return false;

                if (a.SceneryGroupIndex == kObjectEntryIndexNull)
                    return false;
                if (b.SceneryGroupIndex == kObjectEntryIndexNull)
                    return true;

                const auto* entryA = a.GetSceneryGroupEntry();
                const auto* entryB = b.GetSceneryGroupEntry();
                return entryA->priority < entryB->priority;
            });
        }

        int32_t GetTabRowCount()
        {
            return std::max<int32_t>((static_cast<int32_t>(_tabEntries.size()) + MaxTabsPerRow - 1) / MaxTabsPerRow, 0);
        }

        int32_t GetMaxTabCountInARow()
        {
            int32_t tabEntries = static_cast<int32_t>(_tabEntries.size());
            return std::min(tabEntries, MaxTabsPerRow);
        }

        void PrepareWidgets()
        {
            // Add the base widgets
            SetWidgets(WindowSceneryBaseWidgets);

            // Add tabs
            _actualMinHeight = WINDOW_SCENERY_MIN_HEIGHT;
            int32_t xInit = InitTabPosX;
            int32_t tabsInThisRow = 0;

            auto hasMisc = GetSceneryTabInfoForMisc() != nullptr;
            auto maxTabsInThisRow = MaxTabsPerRow - 1 - (hasMisc ? 1 : 0);

            ScreenCoordsXY pos = { xInit, InitTabPosY };
            for (const auto& tabInfo : _tabEntries)
            {
                auto widget = MakeTab(pos, STR_STRING_DEFINED_TOOLTIP);
                pos.x += TabWidth;

                if (tabInfo.IsMisc())
                {
                    widget.image = ImageId(SPR_TAB_QUESTION, FilterPaletteID::PaletteNull);
                }
                else if (tabInfo.IsAll())
                {
                    widget.image = ImageId(SPR_TAB, FilterPaletteID::PaletteNull);
                }
                else if (tabInfo.IsSceneryGroup())
                {
                    // Default tab image
                    widget.image = ImageId(SPR_TAB_QUESTION, FilterPaletteID::PaletteNull);

                    // Scenery Group image
                    auto scgEntry = tabInfo.GetSceneryGroupEntry();
                    if (scgEntry != nullptr)
                    {
                        widget.image = ImageId(scgEntry->image, colours[1].colour);
                    }
                }

                widgets.push_back(widget);

                tabsInThisRow++;
                if (tabsInThisRow >= maxTabsInThisRow)
                {
                    pos.x = xInit;
                    pos.y += TabHeight;
                    tabsInThisRow = 0;
                    _actualMinHeight += TabHeight;
                    maxTabsInThisRow = MaxTabsPerRow;
                }
            }

            // Shift base widgets based on number of tab rows
            int32_t shiftAmount = (GetTabRowCount() - 1) * TabHeight;
            if (shiftAmount > 0)
            {
                widgets[WIDX_SCENERY_LIST].top += shiftAmount;
                widgets[WIDX_SCENERY_LIST].bottom += shiftAmount;
                widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_ROTATE_OBJECTS_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_REPAINT_SCENERY_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_PRIMARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_SECONDARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_TERTIARY_COLOUR_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_EYEDROPPER_BUTTON].bottom += shiftAmount;
                widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].top += shiftAmount;
                widgets[WIDX_SCENERY_BUILD_CLUSTER_BUTTON].bottom += shiftAmount;
                widgets[WIDX_FILTER_TEXT_BOX].top += shiftAmount;
                widgets[WIDX_FILTER_TEXT_BOX].bottom += shiftAmount;
                widgets[WIDX_FILTER_CLEAR_BUTTON].top += shiftAmount;
                widgets[WIDX_FILTER_CLEAR_BUTTON].bottom += shiftAmount;
            }
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

            _sceneryPaintEnabled = false;
            gWindowSceneryEyedropperEnabled = false;

            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, windowPos.x + (width / 2));
            _hoverCounter = -16;
            gSceneryPlaceCost = kMoney64Undefined;
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

        std::pair<StringId, money64> GetNameAndPrice(ScenerySelection selectedScenery)
        {
            StringId name = STR_UNKNOWN_OBJECT_TYPE;
            money64 price = kMoney64Undefined;
            if (selectedScenery.IsUndefined() && gSceneryPlaceCost != kMoney64Undefined)
            {
                price = gSceneryPlaceCost;
            }
            else
            {
                switch (selectedScenery.SceneryType)
                {
                    case SCENERY_TYPE_SMALL:
                    {
                        auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(
                            selectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            price = sceneryEntry->price;
                            name = sceneryEntry->name;
                        }
                        break;
                    }
                    case SCENERY_TYPE_PATH_ITEM:
                    {
                        auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(
                            selectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            price = sceneryEntry->price;
                            name = sceneryEntry->name;
                        }
                        break;
                    }
                    case SCENERY_TYPE_WALL:
                    {
                        auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(
                            selectedScenery.EntryIndex);
                        if (sceneryEntry != nullptr)
                        {
                            price = sceneryEntry->price;
                            name = sceneryEntry->name;
                        }
                        break;
                    }
                    case SCENERY_TYPE_LARGE:
                    {
                        auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(
                            selectedScenery.EntryIndex);
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

                if (_tabEntries[tabIndex].IsAll())
                {
                    auto imageId = ImageId(SPR_G2_INFINITY, FilterPaletteID::PaletteNull);
                    GfxDrawSprite(dpi, imageId, offset + widgetCoordsXY);
                }
            }
        }

        void DrawSceneryItem(DrawPixelInfo& dpi, ScenerySelection scenerySelection)
        {
            if (scenerySelection.SceneryType == SCENERY_TYPE_BANNER)
            {
                auto bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(scenerySelection.EntryIndex);
                auto imageId = ImageId(bannerEntry->image + gWindowSceneryRotation * 2, _sceneryPrimaryColour);
                GfxDrawSprite(dpi, imageId, { 33, 40 });
                GfxDrawSprite(dpi, imageId.WithIndexOffset(1), { 33, 40 });
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_LARGE)
            {
                auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(scenerySelection.EntryIndex);
                auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR)
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                    imageId = imageId.WithSecondary(_scenerySecondaryColour);
                if (sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                    imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                GfxDrawSprite(dpi, imageId, { 33, 0 });
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_WALL)
            {
                auto wallEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(scenerySelection.EntryIndex);
                auto imageId = ImageId(wallEntry->image);
                auto spriteTop = (wallEntry->height * 2) + 0x32;
                if (wallEntry->flags & WALL_SCENERY_HAS_GLASS)
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                    }
                    GfxDrawSprite(dpi, imageId, { 47, spriteTop });

                    auto glassImageId = ImageId(wallEntry->image + 6).WithTransparency(_sceneryPrimaryColour);
                    GfxDrawSprite(dpi, glassImageId, { 47, spriteTop });
                }
                else
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (wallEntry->flags & WALL_SCENERY_HAS_SECONDARY_COLOUR)
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                        if (wallEntry->flags & WALL_SCENERY_HAS_TERTIARY_COLOUR)
                        {
                            imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                        }
                    }
                    GfxDrawSprite(dpi, imageId, { 47, spriteTop });

                    if (wallEntry->flags & WALL_SCENERY_IS_DOOR)
                    {
                        GfxDrawSprite(dpi, imageId.WithIndexOffset(1), { 47, spriteTop });
                    }
                }
            }
            else if (scenerySelection.SceneryType == SCENERY_TYPE_PATH_ITEM)
            {
                auto* pathAdditionEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(
                    scenerySelection.EntryIndex);
                auto imageId = ImageId(pathAdditionEntry->image);
                GfxDrawSprite(dpi, imageId, { 11, 16 });
            }
            else
            {
                auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(scenerySelection.EntryIndex);
                auto imageId = ImageId(sceneryEntry->image + gWindowSceneryRotation);
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                {
                    imageId = imageId.WithPrimary(_sceneryPrimaryColour);
                    if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                    {
                        imageId = imageId.WithSecondary(_scenerySecondaryColour);
                    }
                }
                if (sceneryEntry->flags & SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
                {
                    imageId = imageId.WithTertiary(_sceneryTertiaryColour);
                }

                auto spriteTop = (sceneryEntry->height / 4) + 43;
                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE)
                    && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
                {
                    spriteTop -= 12;
                }

                GfxDrawSprite(dpi, imageId, { 32, spriteTop });

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
                {
                    imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation).WithTransparency(_sceneryPrimaryColour);
                    GfxDrawSprite(dpi, imageId, { 32, spriteTop });
                }

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
                {
                    imageId = ImageId(sceneryEntry->image + 4 + gWindowSceneryRotation);
                    GfxDrawSprite(dpi, imageId, { 32, spriteTop });
                }
            }
        }

        void ContentScrollDraw(DrawPixelInfo& dpi)
        {
            GfxClear(dpi, ColourMapA[colours[1].colour].mid_light);

            auto numColumns = GetNumColumns();
            auto tabIndex = _activeTabIndex;
            if (tabIndex >= _tabEntries.size())
            {
                return;
            }

            ScreenCoordsXY topLeft{ 0, 0 };

            for (size_t sceneryTabItemIndex = 0; sceneryTabItemIndex < _filteredSceneryTab.Entries.size();
                 sceneryTabItemIndex++)
            {
                const auto& currentSceneryGlobal = _filteredSceneryTab.Entries[sceneryTabItemIndex];
                const auto tabSelectedScenery = GetSelectedScenery(tabIndex);
                if (_sceneryPaintEnabled == 1 || gWindowSceneryEyedropperEnabled)
                {
                    if (_selectedScenery == currentSceneryGlobal)
                    {
                        GfxFillRectInset(
                            dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                            colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                    }
                }
                else
                {
                    if (tabSelectedScenery == currentSceneryGlobal)
                    {
                        GfxFillRectInset(
                            dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                            colours[1], (INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT));
                    }
                    else if (_selectedScenery == currentSceneryGlobal)
                    {
                        GfxFillRectInset(
                            dpi, { topLeft, topLeft + ScreenCoordsXY{ SCENERY_BUTTON_WIDTH - 1, SCENERY_BUTTON_HEIGHT - 1 } },
                            colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                    }
                }

                DrawPixelInfo clipdpi;
                if (ClipDrawPixelInfo(
                        clipdpi, dpi, topLeft + ScreenCoordsXY{ 1, 1 }, SCENERY_BUTTON_WIDTH - 2, SCENERY_BUTTON_HEIGHT - 2))
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

        /**
         *
         *  rct2: 0x006E287B
         */
        void ToolUpdateScenery(const ScreenCoordsXY& screenPos)
        {
            MapInvalidateSelectionRect();
            MapInvalidateMapSelectionTiles();

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorInvalidate();
            }

            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;

            if (_sceneryPaintEnabled)
                return;
            if (gWindowSceneryEyedropperEnabled)
                return;

            const auto selection = WindowSceneryGetTabSelection();
            if (selection.IsUndefined())
            {
                SceneryRemoveGhostToolPlacement();
                return;
            }

            money64 cost = 0;

            switch (selection.SceneryType)
            {
                case SCENERY_TYPE_SMALL:
                {
                    CoordsXY mapTile = {};
                    uint8_t quadrant;
                    Direction rotation;

                    Sub6E1F34SmallScenery(screenPos, selection.EntryIndex, mapTile, &quadrant, &rotation);

                    if (mapTile.IsNull())
                    {
                        SceneryRemoveGhostToolPlacement();
                        return;
                    }

                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                    if (gWindowSceneryScatterEnabled)
                    {
                        uint16_t cluster_size = (gWindowSceneryScatterSize - 1) * kCoordsXYStep;
                        gMapSelectPositionA.x = mapTile.x - cluster_size / 2;
                        gMapSelectPositionA.y = mapTile.y - cluster_size / 2;
                        gMapSelectPositionB.x = mapTile.x + cluster_size / 2;
                        gMapSelectPositionB.y = mapTile.y + cluster_size / 2;
                        if (gWindowSceneryScatterSize % 2 == 0)
                        {
                            gMapSelectPositionB.x += kCoordsXYStep;
                            gMapSelectPositionB.y += kCoordsXYStep;
                        }
                    }
                    else
                    {
                        gMapSelectPositionA.x = mapTile.x;
                        gMapSelectPositionA.y = mapTile.y;
                        gMapSelectPositionB.x = mapTile.x;
                        gMapSelectPositionB.y = mapTile.y;
                    }

                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(selection.EntryIndex);

                    gMapSelectType = MAP_SELECT_TYPE_FULL;
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && !gWindowSceneryScatterEnabled)
                    {
                        gMapSelectType = MAP_SELECT_TYPE_QUARTER_0 + (quadrant ^ 2);
                    }

                    MapInvalidateSelectionRect();

                    // If no change in ghost placement
                    if ((gSceneryGhostType & SCENERY_GHOST_FLAG_0) && mapTile == gSceneryGhostPosition && quadrant == _unkF64F0E
                        && gSceneryPlaceZ == _unkF64F0A && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_SMALL
                        && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
                    {
                        return;
                    }

                    SceneryRemoveGhostToolPlacement();

                    _unkF64F0E = quadrant;
                    _unkF64F0A = gSceneryPlaceZ;

                    uint8_t attemptsLeft = 1;
                    if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                    {
                        attemptsLeft = 20;
                    }

                    for (; attemptsLeft != 0; attemptsLeft--)
                    {
                        cost = TryPlaceGhostSmallScenery(
                            { mapTile, gSceneryPlaceZ, rotation }, quadrant, selection.EntryIndex, _sceneryPrimaryColour,
                            _scenerySecondaryColour, _sceneryTertiaryColour);

                        if (cost != kMoney64Undefined)
                            break;
                        gSceneryPlaceZ += 8;
                    }

                    gSceneryPlaceCost = cost;
                    break;
                }
                case SCENERY_TYPE_PATH_ITEM:
                {
                    CoordsXY mapTile = {};
                    int32_t z;

                    Sub6E1F34PathItem(screenPos, selection.EntryIndex, mapTile, &z);

                    if (mapTile.IsNull())
                    {
                        SceneryRemoveGhostToolPlacement();
                        return;
                    }

                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                    gMapSelectPositionA.x = mapTile.x;
                    gMapSelectPositionA.y = mapTile.y;
                    gMapSelectPositionB.x = mapTile.x;
                    gMapSelectPositionB.y = mapTile.y;
                    gMapSelectType = MAP_SELECT_TYPE_FULL;

                    MapInvalidateSelectionRect();

                    // If no change in ghost placement
                    if ((gSceneryGhostType & SCENERY_GHOST_FLAG_1) && mapTile == gSceneryGhostPosition
                        && z == gSceneryGhostPosition.z)
                    {
                        return;
                    }

                    SceneryRemoveGhostToolPlacement();

                    cost = TryPlaceGhostPathAddition({ mapTile, z }, selection.EntryIndex);

                    gSceneryPlaceCost = cost;
                    break;
                }
                case SCENERY_TYPE_WALL:
                {
                    CoordsXY mapTile = {};
                    uint8_t edge;

                    Sub6E1F34Wall(screenPos, selection.EntryIndex, mapTile, &edge);

                    if (mapTile.IsNull())
                    {
                        SceneryRemoveGhostToolPlacement();
                        return;
                    }

                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                    gMapSelectPositionA.x = mapTile.x;
                    gMapSelectPositionA.y = mapTile.y;
                    gMapSelectPositionB.x = mapTile.x;
                    gMapSelectPositionB.y = mapTile.y;
                    gMapSelectType = MAP_SELECT_TYPE_EDGE_0 + edge;

                    MapInvalidateSelectionRect();

                    // If no change in ghost placement
                    if ((gSceneryGhostType & SCENERY_GHOST_FLAG_2) && mapTile == gSceneryGhostPosition
                        && edge == gSceneryGhostWallRotation && gSceneryPlaceZ == _unkF64F0A)
                    {
                        return;
                    }

                    SceneryRemoveGhostToolPlacement();

                    gSceneryGhostWallRotation = edge;
                    _unkF64F0A = gSceneryPlaceZ;

                    uint8_t attemptsLeft = 1;
                    if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                    {
                        attemptsLeft = 20;
                    }

                    cost = 0;
                    for (; attemptsLeft != 0; attemptsLeft--)
                    {
                        cost = TryPlaceGhostWall(
                            { mapTile, gSceneryPlaceZ }, edge, selection.EntryIndex, _sceneryPrimaryColour,
                            _scenerySecondaryColour, _sceneryTertiaryColour);

                        if (cost != kMoney64Undefined)
                            break;
                        gSceneryPlaceZ += 8;
                    }

                    gSceneryPlaceCost = cost;
                    break;
                }
                case SCENERY_TYPE_LARGE:
                {
                    CoordsXY mapTile = {};
                    Direction direction;

                    Sub6E1F34LargeScenery(screenPos, selection.EntryIndex, mapTile, &direction);

                    if (mapTile.IsNull())
                    {
                        SceneryRemoveGhostToolPlacement();
                        return;
                    }

                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(selection.EntryIndex);
                    gMapSelectionTiles.clear();

                    for (auto& tile : sceneryEntry->tiles)
                    {
                        CoordsXY tileLocation = { tile.offset };
                        auto rotatedTileCoords = tileLocation.Rotate(direction);

                        rotatedTileCoords.x += mapTile.x;
                        rotatedTileCoords.y += mapTile.y;

                        gMapSelectionTiles.push_back(rotatedTileCoords);
                    }

                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
                    MapInvalidateMapSelectionTiles();

                    // If no change in ghost placement
                    if ((gSceneryGhostType & SCENERY_GHOST_FLAG_3) && mapTile == gSceneryGhostPosition
                        && gSceneryPlaceZ == _unkF64F0A && gSceneryPlaceObject.SceneryType == SCENERY_TYPE_LARGE
                        && gSceneryPlaceObject.EntryIndex == selection.EntryIndex)
                    {
                        return;
                    }

                    SceneryRemoveGhostToolPlacement();

                    gSceneryPlaceObject.SceneryType = SCENERY_TYPE_LARGE;
                    gSceneryPlaceObject.EntryIndex = selection.EntryIndex;
                    _unkF64F0A = gSceneryPlaceZ;

                    uint8_t attemptsLeft = 1;
                    if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                    {
                        attemptsLeft = 20;
                    }

                    cost = 0;
                    for (; attemptsLeft != 0; attemptsLeft--)
                    {
                        cost = TryPlaceGhostLargeScenery(
                            { mapTile, gSceneryPlaceZ, direction }, selection.EntryIndex, _sceneryPrimaryColour,
                            _scenerySecondaryColour, _sceneryTertiaryColour);

                        if (cost != kMoney64Undefined)
                            break;
                        gSceneryPlaceZ += kCoordsZStep;
                    }

                    gSceneryPlaceCost = cost;
                    break;
                }
                case SCENERY_TYPE_BANNER:
                {
                    CoordsXY mapTile = {};
                    Direction direction;
                    int32_t z;

                    Sub6E1F34Banner(screenPos, selection.EntryIndex, mapTile, &z, &direction);

                    if (mapTile.IsNull())
                    {
                        SceneryRemoveGhostToolPlacement();
                        return;
                    }

                    gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                    gMapSelectPositionA.x = mapTile.x;
                    gMapSelectPositionA.y = mapTile.y;
                    gMapSelectPositionB.x = mapTile.x;
                    gMapSelectPositionB.y = mapTile.y;
                    gMapSelectType = MAP_SELECT_TYPE_FULL;

                    MapInvalidateSelectionRect();

                    // If no change in ghost placement
                    if ((gSceneryGhostType & SCENERY_GHOST_FLAG_4) && mapTile == gSceneryGhostPosition
                        && z == gSceneryGhostPosition.z && direction == gSceneryPlaceRotation)
                    {
                        return;
                    }

                    SceneryRemoveGhostToolPlacement();

                    cost = TryPlaceGhostBanner({ mapTile, z, direction }, selection.EntryIndex);

                    gSceneryPlaceCost = cost;
                    break;
                }
            }
        }

        /**
         *
         *  rct2: 0x006E24F6
         * On failure returns kMoney64Undefined
         * On success places ghost scenery and returns cost to place proper
         */
        money64 TryPlaceGhostSmallScenery(
            CoordsXYZD loc, uint8_t quadrant, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour,
            colour_t tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e252b
            auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                loc, quadrant, entryIndex, primaryColour, secondaryColour, tertiaryColour);
            smallSceneryPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
            auto res = GameActions::Execute(&smallSceneryPlaceAction);
            if (res.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            const auto placementData = res.GetData<SmallSceneryPlaceActionResult>();

            gSceneryPlaceRotation = loc.direction;
            gSceneryPlaceObject.SceneryType = SCENERY_TYPE_SMALL;
            gSceneryPlaceObject.EntryIndex = entryIndex;

            gSceneryGhostPosition = { loc, placementData.BaseHeight };
            gSceneryQuadrant = placementData.SceneryQuadrant;
            if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                ViewportSetVisibility(ViewportVisibility::UndergroundViewGhostOn);
            }
            else
            {
                // Set underground off
                ViewportSetVisibility(ViewportVisibility::UndergroundViewGhostOff);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_0;
            return res.Cost;
        }

        money64 TryPlaceGhostPathAddition(CoordsXYZ loc, ObjectEntryIndex entryIndex)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e265b
            auto footpathAdditionPlaceAction = FootpathAdditionPlaceAction(loc, entryIndex);
            footpathAdditionPlaceAction.SetFlags(GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED);
            footpathAdditionPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                {
                    return;
                }
                gSceneryGhostPosition = loc;
                gSceneryGhostType |= SCENERY_GHOST_FLAG_1;
            });
            auto res = GameActions::Execute(&footpathAdditionPlaceAction);
            if (res.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            return res.Cost;
        }

        money64 TryPlaceGhostWall(
            CoordsXYZ loc, uint8_t edge, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour,
            colour_t tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e26b0
            auto wallPlaceAction = WallPlaceAction(entryIndex, loc, edge, primaryColour, secondaryColour, tertiaryColour);
            wallPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            wallPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                    return;

                const auto placementData = result->GetData<WallPlaceActionResult>();
                gSceneryGhostPosition = { loc, placementData.BaseHeight };
                gSceneryGhostWallRotation = edge;

                gSceneryGhostType |= SCENERY_GHOST_FLAG_2;
            });

            auto res = GameActions::Execute(&wallPlaceAction);
            if (res.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            return res.Cost;
        }

        money64 TryPlaceGhostLargeScenery(
            CoordsXYZD loc, ObjectEntryIndex entryIndex, colour_t primaryColour, colour_t secondaryColour,
            colour_t tertiaryColour)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e25a7
            auto sceneryPlaceAction = LargeSceneryPlaceAction(loc, entryIndex, primaryColour, secondaryColour, tertiaryColour);
            sceneryPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            auto res = GameActions::Execute(&sceneryPlaceAction);
            if (res.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            const auto placementData = res.GetData<LargeSceneryPlaceActionResult>();

            gSceneryPlaceRotation = loc.direction;

            gSceneryGhostPosition = { loc, placementData.firstTileHeight };
            if (placementData.GroundFlags & ELEMENT_IS_UNDERGROUND)
            {
                // Set underground on
                ViewportSetVisibility(ViewportVisibility::UndergroundViewGhostOn);
            }
            else
            {
                // Set underground off
                ViewportSetVisibility(ViewportVisibility::UndergroundViewGhostOff);
            }

            gSceneryGhostType |= SCENERY_GHOST_FLAG_3;
            return res.Cost;
        }

        money64 TryPlaceGhostBanner(CoordsXYZD loc, ObjectEntryIndex entryIndex)
        {
            SceneryRemoveGhostToolPlacement();

            // 6e2612
            auto primaryColour = _sceneryPrimaryColour;
            auto bannerPlaceAction = BannerPlaceAction(loc, entryIndex, primaryColour);
            bannerPlaceAction.SetFlags(
                GAME_COMMAND_FLAG_GHOST | GAME_COMMAND_FLAG_ALLOW_DURING_PAUSED | GAME_COMMAND_FLAG_NO_SPEND);
            auto res = GameActions::Execute(&bannerPlaceAction);
            if (res.Error != GameActions::Status::Ok)
                return kMoney64Undefined;

            gSceneryGhostPosition = loc;
            gSceneryGhostPosition.z += kPathHeightStep;
            gSceneryPlaceRotation = loc.direction;
            gSceneryGhostType |= SCENERY_GHOST_FLAG_4;
            return res.Cost;
        }

        /**
         *
         *  rct2: 0x006E3158
         */
        void RepaintSceneryToolDown(const ScreenCoordsXY& screenCoords, WidgetIndex widgetIndex)
        {
            auto flag = EnumsToFlags(
                ViewportInteractionItem::Scenery, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery,
                ViewportInteractionItem::Banner);
            auto info = GetMapCoordinatesFromPos(screenCoords, flag);
            switch (info.interactionType)
            {
                case ViewportInteractionItem::Scenery:
                {
                    auto* sceneryEntry = info.Element->AsSmallScenery()->GetEntry();

                    // If can't repaint
                    if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR | SMALL_SCENERY_FLAG_HAS_GLASS))
                        return;

                    uint8_t quadrant = info.Element->AsSmallScenery()->GetSceneryQuadrant();
                    auto repaintScenery = SmallScenerySetColourAction(
                        { info.Loc, info.Element->GetBaseZ() }, quadrant, info.Element->AsSmallScenery()->GetEntryIndex(),
                        _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery);
                    break;
                }
                case ViewportInteractionItem::Wall:
                {
                    auto* scenery_entry = info.Element->AsWall()->GetEntry();

                    // If can't repaint
                    if (!(scenery_entry->flags & (WALL_SCENERY_HAS_PRIMARY_COLOUR | WALL_SCENERY_HAS_GLASS)))
                        return;

                    auto repaintScenery = WallSetColourAction(
                        { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() }, _sceneryPrimaryColour,
                        _scenerySecondaryColour, _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery);
                    break;
                }
                case ViewportInteractionItem::LargeScenery:
                {
                    auto* sceneryEntry = info.Element->AsLargeScenery()->GetEntry();

                    // If can't repaint
                    if (!(sceneryEntry->flags & LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                        return;

                    auto repaintScenery = LargeScenerySetColourAction(
                        { info.Loc, info.Element->GetBaseZ(), info.Element->GetDirection() },
                        info.Element->AsLargeScenery()->GetSequenceIndex(), _sceneryPrimaryColour, _scenerySecondaryColour,
                        _sceneryTertiaryColour);

                    GameActions::Execute(&repaintScenery);
                    break;
                }
                case ViewportInteractionItem::Banner:
                {
                    auto banner = info.Element->AsBanner()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto* bannerEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
                        if (bannerEntry->flags & BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR)
                        {
                            auto repaintScenery = BannerSetColourAction(
                                { info.Loc, info.Element->GetBaseZ(), info.Element->AsBanner()->GetPosition() },
                                _sceneryPrimaryColour);

                            GameActions::Execute(&repaintScenery);
                        }
                    }
                    break;
                }
                default:
                    return;
            }
        }

        void SceneryEyedropperToolDown(const ScreenCoordsXY& screenCoords, WidgetIndex widgetIndex)
        {
            auto flag = EnumsToFlags(
                ViewportInteractionItem::Scenery, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery,
                ViewportInteractionItem::Banner, ViewportInteractionItem::PathAddition);
            auto info = GetMapCoordinatesFromPos(screenCoords, flag);
            switch (info.interactionType)
            {
                case ViewportInteractionItem::Scenery:
                {
                    SmallSceneryElement* sceneryElement = info.Element->AsSmallScenery();
                    auto entryIndex = sceneryElement->GetEntryIndex();
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_SMALL, entryIndex }, sceneryElement->GetPrimaryColour(),
                            sceneryElement->GetSecondaryColour(), sceneryElement->GetTertiaryColour(),
                            sceneryElement->GetDirectionWithOffset(GetCurrentRotation()));
                    }
                    break;
                }
                case ViewportInteractionItem::Wall:
                {
                    auto entryIndex = info.Element->AsWall()->GetEntryIndex();
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_WALL, entryIndex }, info.Element->AsWall()->GetPrimaryColour(),
                            info.Element->AsWall()->GetSecondaryColour(), info.Element->AsWall()->GetTertiaryColour(),
                            std::nullopt);
                    }
                    break;
                }
                case ViewportInteractionItem::LargeScenery:
                {
                    auto entryIndex = info.Element->AsLargeScenery()->GetEntryIndex();
                    auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(entryIndex);
                    if (sceneryEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_LARGE, entryIndex }, info.Element->AsLargeScenery()->GetPrimaryColour(),
                            info.Element->AsLargeScenery()->GetSecondaryColour(), std::nullopt,
                            (GetCurrentRotation() + info.Element->GetDirection()) & 3);
                    }
                    break;
                }
                case ViewportInteractionItem::Banner:
                {
                    auto banner = info.Element->AsBanner()->GetBanner();
                    if (banner != nullptr)
                    {
                        auto sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<BannerSceneryEntry>(banner->type);
                        if (sceneryEntry != nullptr)
                        {
                            WindowScenerySetSelectedItem(
                                { SCENERY_TYPE_BANNER, banner->type }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                        }
                    }
                    break;
                }
                case ViewportInteractionItem::PathAddition:
                {
                    auto entryIndex = info.Element->AsPath()->GetAdditionEntryIndex();
                    auto* pathAdditionEntry = OpenRCT2::ObjectManager::GetObjectEntry<PathAdditionEntry>(entryIndex);
                    if (pathAdditionEntry != nullptr)
                    {
                        WindowScenerySetSelectedItem(
                            { SCENERY_TYPE_PATH_ITEM, entryIndex }, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
                    }
                    break;
                }
                default:
                    break;
            }
        }

        void Sub6E1F34UpdateScreenCoordsAndButtonsPressed(bool canRaiseItem, ScreenCoordsXY& screenPos)
        {
            if (!canRaiseItem && !GetGameState().Cheats.disableSupportLimits)
            {
                gSceneryCtrlPressed = false;
                gSceneryShiftPressed = false;
            }
            else
            {
                auto& im = GetInputManager();
                if (!gSceneryCtrlPressed)
                {
                    if (im.IsModifierKeyPressed(ModifierKey::ctrl))
                    {
                        // CTRL pressed
                        constexpr auto flag = EnumsToFlags(
                            ViewportInteractionItem::Terrain, ViewportInteractionItem::Ride, ViewportInteractionItem::Scenery,
                            ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall,
                            ViewportInteractionItem::LargeScenery);
                        auto info = GetMapCoordinatesFromPos(screenPos, flag);

                        if (info.interactionType != ViewportInteractionItem::None)
                        {
                            gSceneryCtrlPressed = true;
                            gSceneryCtrlPressZ = info.Element->GetBaseZ();
                        }
                    }
                }
                else
                {
                    if (!(im.IsModifierKeyPressed(ModifierKey::ctrl)))
                    {
                        // CTRL not pressed
                        gSceneryCtrlPressed = false;
                    }
                }

                if (!gSceneryShiftPressed)
                {
                    if (im.IsModifierKeyPressed(ModifierKey::shift))
                    {
                        // SHIFT pressed
                        gSceneryShiftPressed = true;
                        gSceneryShiftPressX = screenPos.x;
                        gSceneryShiftPressY = screenPos.y;
                        gSceneryShiftPressZOffset = 0;
                    }
                }
                else
                {
                    if (im.IsModifierKeyPressed(ModifierKey::shift))
                    {
                        // SHIFT pressed
                        gSceneryShiftPressZOffset = (gSceneryShiftPressY - screenPos.y + 4);
                        // Scale delta by zoom to match mouse position.
                        auto* mainWnd = WindowGetMain();
                        if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                        {
                            gSceneryShiftPressZOffset = mainWnd->viewport->zoom.ApplyTo(gSceneryShiftPressZOffset);
                        }
                        gSceneryShiftPressZOffset = floor2(gSceneryShiftPressZOffset, 8);

                        screenPos.x = gSceneryShiftPressX;
                        screenPos.y = gSceneryShiftPressY;
                    }
                    else
                    {
                        // SHIFT not pressed
                        gSceneryShiftPressed = false;
                    }
                }
            }
        }

        void Sub6E1F34SmallScenery(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outQuadrant,
            Direction* outRotation)
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FindByClass(WindowClass::Scenery);

            if (w == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);
            bool can_raise_item = false;

            const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryIndex);
            if (sceneryEntry == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            maxPossibleHeight -= sceneryEntry->height;
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_STACKABLE))
            {
                can_raise_item = true;
            }

            Sub6E1F34UpdateScreenCoordsAndButtonsPressed(can_raise_item, screenPos);

            // Small scenery
            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
            {
                uint8_t quadrant = 0;

                // If CTRL not pressed
                if (!gSceneryCtrlPressed)
                {
                    auto gridCoords = ScreenGetMapXYQuadrant(screenPos, &quadrant);
                    if (!gridCoords.has_value())
                    {
                        gridPos.SetNull();
                        return;
                    }
                    gridPos = gridCoords.value();

                    gSceneryPlaceZ = 0;

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                        if (surfaceElement == nullptr)
                        {
                            gridPos.SetNull();
                            return;
                        }

                        int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                        z += gSceneryShiftPressZOffset;

                        z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                        gSceneryPlaceZ = z;
                    }
                }
                else
                {
                    int16_t z = gSceneryCtrlPressZ;

                    auto mapCoords = ScreenGetMapXYQuadrantWithZ(screenPos, z, &quadrant);
                    if (!mapCoords.has_value())
                    {
                        gridPos.SetNull();
                        return;
                    }
                    gridPos = mapCoords.value();

                    // If SHIFT pressed
                    if (gSceneryShiftPressed)
                    {
                        z += gSceneryShiftPressZOffset;
                    }

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }

                if (gridPos.IsNull())
                    return;

                uint8_t rotation = gWindowSceneryRotation;

                if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                {
                    rotation = UtilRand() & 0xFF;
                }

                rotation -= GetCurrentRotation();
                rotation &= 0x3;

                if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
                {
                    VirtualFloorSetHeight(gSceneryPlaceZ);
                }

                *outQuadrant = quadrant ^ 2;
                *outRotation = rotation;

                return;
            }

            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                constexpr auto flag = EnumsToFlags(ViewportInteractionItem::Terrain, ViewportInteractionItem::Water);

                auto info = GetMapCoordinatesFromPos(screenPos, flag);
                gridPos = info.Loc;

                if (info.interactionType == ViewportInteractionItem::None)
                {
                    gridPos.SetNull();
                    return;
                }

                // If CTRL and SHIFT not pressed
                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto coords = ScreenGetMapXYWithZ(screenPos, z);
                if (coords.has_value())
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.SetNull();
                }
                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            gridPos = gridPos.ToTileStart();
            uint8_t rotation = gWindowSceneryRotation;

            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
            {
                rotation = UtilRand() & 0xFF;
            }

            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outQuadrant = 0;
            *outRotation = rotation;
        }

        void Sub6E1F34PathItem(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ)
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FindByClass(WindowClass::Scenery);

            if (w == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            auto screenPos = sourceScreenPos;
            Sub6E1F34UpdateScreenCoordsAndButtonsPressed(false, screenPos);

            // Path bits
            constexpr auto flag = EnumsToFlags(ViewportInteractionItem::Footpath, ViewportInteractionItem::PathAddition);
            auto info = GetMapCoordinatesFromPos(screenPos, flag);
            gridPos = info.Loc;

            if (info.interactionType == ViewportInteractionItem::None)
            {
                gridPos.SetNull();
                return;
            }

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outZ = info.Element->GetBaseZ();
        }

        void Sub6E1F34Wall(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, uint8_t* outEdges)
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FindByClass(WindowClass::Scenery);

            if (w == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

            auto* wallEntry = OpenRCT2::ObjectManager::GetObjectEntry<WallSceneryEntry>(sceneryIndex);
            if (wallEntry != nullptr)
            {
                maxPossibleHeight -= wallEntry->height;
            }

            Sub6E1F34UpdateScreenCoordsAndButtonsPressed(true, screenPos);

            // Walls
            uint8_t edge;
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                auto gridCoords = ScreenGetMapXYSide(screenPos, &edge);
                if (!gridCoords.has_value())
                {
                    gridPos.SetNull();
                    return;
                }
                gridPos = gridCoords.value();

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto mapCoords = ScreenGetMapXYSideWithZ(screenPos, z, &edge);
                if (!mapCoords.has_value())
                {
                    gridPos.SetNull();
                    return;
                }
                gridPos = mapCoords.value();

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outEdges = edge;
        }

        void Sub6E1F34LargeScenery(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, Direction* outDirection)
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FindByClass(WindowClass::Scenery);

            if (w == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            auto screenPos = sourceScreenPos;
            uint16_t maxPossibleHeight = ZoomLevel::max().ApplyTo(
                std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

            auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<LargeSceneryEntry>(sceneryIndex);
            if (sceneryEntry)
            {
                int16_t maxClearZ = 0;
                for (auto& tile : sceneryEntry->tiles)
                {
                    maxClearZ = std::max<int16_t>(maxClearZ, tile.zClearance);
                }
                maxPossibleHeight = std::max(0, maxPossibleHeight - maxClearZ);
            }

            Sub6E1F34UpdateScreenCoordsAndButtonsPressed(true, screenPos);

            // Large scenery
            // If CTRL not pressed
            if (!gSceneryCtrlPressed)
            {
                const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenPos);
                gridPos = mapCoords;

                if (gridPos.IsNull())
                    return;

                gSceneryPlaceZ = 0;

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    auto* surfaceElement = MapGetSurfaceElementAt(gridPos);

                    if (surfaceElement == nullptr)
                    {
                        gridPos.SetNull();
                        return;
                    }

                    int16_t z = (surfaceElement->GetBaseZ()) & 0xFFF0;
                    z += gSceneryShiftPressZOffset;

                    z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                    gSceneryPlaceZ = z;
                }
            }
            else
            {
                int16_t z = gSceneryCtrlPressZ;
                auto coords = ScreenGetMapXYWithZ(screenPos, z);
                if (coords.has_value())
                {
                    gridPos = *coords;
                }
                else
                {
                    gridPos.SetNull();
                }

                // If SHIFT pressed
                if (gSceneryShiftPressed)
                {
                    z += gSceneryShiftPressZOffset;
                }

                z = std::clamp<int16_t>(z, 16, maxPossibleHeight);

                gSceneryPlaceZ = z;
            }

            if (gridPos.IsNull())
                return;

            gridPos = gridPos.ToTileStart();

            Direction rotation = gWindowSceneryRotation;
            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outDirection = rotation;
        }

        void Sub6E1F34Banner(
            const ScreenCoordsXY& sourceScreenPos, ObjectEntryIndex sceneryIndex, CoordsXY& gridPos, int32_t* outZ,
            Direction* outDirection)
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FindByClass(WindowClass::Scenery);

            if (w == nullptr)
            {
                gridPos.SetNull();
                return;
            }

            auto screenPos = sourceScreenPos;
            Sub6E1F34UpdateScreenCoordsAndButtonsPressed(false, screenPos);

            // Banner
            constexpr auto flag = EnumsToFlags(ViewportInteractionItem::Footpath, ViewportInteractionItem::PathAddition);
            auto info = GetMapCoordinatesFromPos(screenPos, flag);
            gridPos = info.Loc;

            if (info.interactionType == ViewportInteractionItem::None)
            {
                gridPos.SetNull();
                return;
            }

            uint8_t rotation = gWindowSceneryRotation;
            rotation -= GetCurrentRotation();
            rotation &= 0x3;

            auto z = info.Element->GetBaseZ();

            if (info.Element->AsPath()->IsSloped())
            {
                if (rotation != DirectionReverse(info.Element->AsPath()->GetSlopeDirection()))
                {
                    z += (2 * kCoordsZStep);
                }
            }

            if (Config::Get().general.VirtualFloorStyle != VirtualFloorStyles::Off)
            {
                VirtualFloorSetHeight(gSceneryPlaceZ);
            }

            *outDirection = rotation;
            *outZ = z;
        }

        /**
         *
         *  rct2: 0x006E2CC6
         */
        void SceneryToolDown(const ScreenCoordsXY& screenCoords, WidgetIndex widgetIndex)
        {
            SceneryRemoveGhostToolPlacement();

            if (_sceneryPaintEnabled)
            {
                RepaintSceneryToolDown(screenCoords, widgetIndex);
                return;
            }

            if (gWindowSceneryEyedropperEnabled)
            {
                SceneryEyedropperToolDown(screenCoords, widgetIndex);
                return;
            }

            auto selectedTab = WindowSceneryGetTabSelection();
            uint8_t sceneryType = selectedTab.SceneryType;
            uint16_t selectedScenery = selectedTab.EntryIndex;
            CoordsXY gridPos;

            switch (sceneryType)
            {
                case SCENERY_TYPE_SMALL:
                {
                    uint8_t quadrant;
                    Direction rotation;
                    Sub6E1F34SmallScenery(screenCoords, selectedScenery, gridPos, &quadrant, &rotation);
                    if (gridPos.IsNull())
                        return;

                    int32_t quantity = 1;
                    bool isCluster = gWindowSceneryScatterEnabled
                        && (NetworkGetMode() != NETWORK_MODE_CLIENT
                            || NetworkCanPerformCommand(NetworkGetCurrentPlayerGroupIndex(), -2));

                    if (isCluster)
                    {
                        switch (gWindowSceneryScatterDensity)
                        {
                            case ScatterToolDensity::LowDensity:
                                quantity = gWindowSceneryScatterSize;
                                break;

                            case ScatterToolDensity::MediumDensity:
                                quantity = gWindowSceneryScatterSize * 2;
                                break;

                            case ScatterToolDensity::HighDensity:
                                quantity = gWindowSceneryScatterSize * 3;
                                break;
                        }
                    }

                    bool forceError = true;
                    for (int32_t q = 0; q < quantity; q++)
                    {
                        int32_t zCoordinate = gSceneryPlaceZ;
                        auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(selectedScenery);

                        int16_t cur_grid_x = gridPos.x;
                        int16_t cur_grid_y = gridPos.y;

                        if (isCluster)
                        {
                            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE))
                            {
                                quadrant = UtilRand() & 3;
                            }

                            int16_t grid_x_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                            int16_t grid_y_offset = (UtilRand() % gWindowSceneryScatterSize) - (gWindowSceneryScatterSize / 2);
                            if (gWindowSceneryScatterSize % 2 == 0)
                            {
                                grid_x_offset += 1;
                                grid_y_offset += 1;
                            }
                            cur_grid_x += grid_x_offset * kCoordsXYStep;
                            cur_grid_y += grid_y_offset * kCoordsXYStep;

                            if (!sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ROTATABLE))
                            {
                                gSceneryPlaceRotation = (gSceneryPlaceRotation + 1) & 3;
                            }
                        }

                        uint8_t zAttemptRange = 1;
                        if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                        {
                            zAttemptRange = 20;
                        }

                        auto success = GameActions::Status::Unknown;
                        // Try find a valid z coordinate
                        for (; zAttemptRange != 0; zAttemptRange--)
                        {
                            auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                                { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                                _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);
                            auto res = GameActions::Query(&smallSceneryPlaceAction);
                            success = res.Error;
                            if (res.Error == GameActions::Status::Ok)
                            {
                                break;
                            }

                            if (res.Error == GameActions::Status::InsufficientFunds)
                            {
                                break;
                            }
                            if (zAttemptRange != 1)
                            {
                                gSceneryPlaceZ += 8;
                            }
                        }

                        // Actually place
                        if (success == GameActions::Status::Ok || ((q + 1 == quantity) && forceError))
                        {
                            auto smallSceneryPlaceAction = SmallSceneryPlaceAction(
                                { cur_grid_x, cur_grid_y, gSceneryPlaceZ, gSceneryPlaceRotation }, quadrant, selectedScenery,
                                _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                            smallSceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                                if (result->Error == GameActions::Status::Ok)
                                {
                                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                                }
                            });
                            auto res = GameActions::Execute(&smallSceneryPlaceAction);
                            if (res.Error == GameActions::Status::Ok)
                            {
                                forceError = false;
                            }

                            if (res.Error == GameActions::Status::InsufficientFunds)
                            {
                                break;
                            }
                        }
                        gSceneryPlaceZ = zCoordinate;
                    }
                    break;
                }
                case SCENERY_TYPE_PATH_ITEM:
                {
                    int32_t z;
                    Sub6E1F34PathItem(screenCoords, selectedScenery, gridPos, &z);
                    if (gridPos.IsNull())
                        return;

                    auto footpathAdditionPlaceAction = FootpathAdditionPlaceAction({ gridPos, z }, selectedScenery);

                    footpathAdditionPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error != GameActions::Status::Ok)
                        {
                            return;
                        }
                        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                    });
                    auto res = GameActions::Execute(&footpathAdditionPlaceAction);
                    break;
                }
                case SCENERY_TYPE_WALL:
                {
                    uint8_t edges;
                    Sub6E1F34Wall(screenCoords, selectedScenery, gridPos, &edges);
                    if (gridPos.IsNull())
                        return;

                    uint8_t zAttemptRange = 1;
                    if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                    {
                        zAttemptRange = 20;
                    }

                    for (; zAttemptRange != 0; zAttemptRange--)
                    {
                        auto wallPlaceAction = WallPlaceAction(
                            selectedScenery, { gridPos, gSceneryPlaceZ }, edges, _sceneryPrimaryColour, _scenerySecondaryColour,
                            _sceneryTertiaryColour);

                        auto res = GameActions::Query(&wallPlaceAction);
                        if (res.Error == GameActions::Status::Ok)
                        {
                            break;
                        }

                        if (const auto* message = std::get_if<StringId>(&res.ErrorMessage))
                        {
                            if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                            {
                                break;
                            }
                        }

                        if (zAttemptRange != 1)
                        {
                            gSceneryPlaceZ += 8;
                        }
                    }

                    auto wallPlaceAction = WallPlaceAction(
                        selectedScenery, { gridPos, gSceneryPlaceZ }, edges, _sceneryPrimaryColour, _scenerySecondaryColour,
                        _sceneryTertiaryColour);

                    wallPlaceAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                        }
                    });
                    auto res = GameActions::Execute(&wallPlaceAction);
                    break;
                }
                case SCENERY_TYPE_LARGE:
                {
                    Direction direction;
                    Sub6E1F34LargeScenery(screenCoords, selectedScenery, gridPos, &direction);
                    if (gridPos.IsNull())
                        return;

                    uint8_t zAttemptRange = 1;
                    if (gSceneryPlaceZ != 0 && gSceneryShiftPressed)
                    {
                        zAttemptRange = 20;
                    }

                    for (; zAttemptRange != 0; zAttemptRange--)
                    {
                        CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

                        auto sceneryPlaceAction = LargeSceneryPlaceAction(
                            loc, selectedScenery, _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);

                        auto res = GameActions::Query(&sceneryPlaceAction);
                        if (res.Error == GameActions::Status::Ok)
                        {
                            break;
                        }

                        if (const auto* message = std::get_if<StringId>(&res.ErrorMessage))
                        {
                            if (*message == STR_NOT_ENOUGH_CASH_REQUIRES || *message == STR_CAN_ONLY_BUILD_THIS_ON_WATER)
                            {
                                break;
                            }
                        }

                        if (zAttemptRange != 1)
                        {
                            gSceneryPlaceZ += 8;
                        }
                    }

                    CoordsXYZD loc = { gridPos, gSceneryPlaceZ, direction };

                    auto sceneryPlaceAction = LargeSceneryPlaceAction(
                        loc, selectedScenery, _sceneryPrimaryColour, _scenerySecondaryColour, _sceneryTertiaryColour);
                    sceneryPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                        }
                        else
                        {
                            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Error, { loc.x, loc.y, gSceneryPlaceZ });
                        }
                    });
                    auto res = GameActions::Execute(&sceneryPlaceAction);
                    break;
                }
                case SCENERY_TYPE_BANNER:
                {
                    int32_t z;
                    Direction direction;
                    Sub6E1F34Banner(screenCoords, selectedScenery, gridPos, &z, &direction);
                    if (gridPos.IsNull())
                        return;

                    CoordsXYZD loc{ gridPos, z, direction };
                    auto primaryColour = _sceneryPrimaryColour;
                    auto bannerPlaceAction = BannerPlaceAction(loc, selectedScenery, primaryColour);
                    bannerPlaceAction.SetCallback([=](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            auto data = result->GetData<BannerPlaceActionResult>();
                            OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, result->Position);
                            ContextOpenDetailWindow(WD_BANNER, data.bannerId.ToUnderlying());
                        }
                    });
                    GameActions::Execute(&bannerPlaceAction);
                    break;
                }
            }
        }
    };

    WindowBase* SceneryOpen()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->BringToFrontByClass(WindowClass::Scenery));
        if (w == nullptr)
        {
            w = windowMgr->Create<SceneryWindow>(WindowClass::Scenery);
        }
        return w;
    }

    void WindowScenerySetSelectedItem(
        const ScenerySelection& scenery, const std::optional<colour_t> primary, const std::optional<colour_t> secondary,
        const std::optional<colour_t> tertiary, const std::optional<colour_t> rotation)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->BringToFrontByClass(WindowClass::Scenery));
        if (w != nullptr)
        {
            w->SetSelectedItem(scenery, primary, secondary, tertiary, rotation);
        }
    }

    void WindowScenerySetSelectedTab(const ObjectEntryIndex sceneryGroupIndex)
    {
        // Should this bring to front?
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::Scenery));
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
        _sceneryPrimaryColour = COLOUR_BORDEAUX_RED;
        _scenerySecondaryColour = COLOUR_YELLOW;
        _sceneryTertiaryColour = COLOUR_DARK_BROWN;

        WindowSceneryResetSelectedSceneryItems();
    }

    const ScenerySelection WindowSceneryGetTabSelection()
    {
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::Scenery));
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
        auto* windowMgr = GetWindowManager();
        auto* w = static_cast<SceneryWindow*>(windowMgr->FindByClass(WindowClass::Scenery));
        if (w != nullptr)
        {
            w->Init();
        }
    }

    void ToggleSceneryWindow()
    {
        if (isToolActive(WindowClass::Scenery, WIDX_SCENERY_BACKGROUND))
        {
            ToolCancel();
        }
        else
        {
            auto* toolWindow = ContextOpenWindow(WindowClass::Scenery);
            ToolSet(*toolWindow, WIDX_SCENERY_BACKGROUND, Tool::Arrow);
            InputSetFlag(INPUT_FLAG_6, true);
        }
    }
} // namespace OpenRCT2::Ui::Windows
