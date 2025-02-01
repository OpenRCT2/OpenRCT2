/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <bit>
#include <cctype>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/object/SceneryGroupObject.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenes/title/TitleScene.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <span>
#include <string>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    enum : uint16_t
    {
        FILTER_NONE = 0,

        FILTER_RCT1 = (1 << 0),
        FILTER_AA = (1 << 1),
        FILTER_LL = (1 << 2),
        FILTER_RCT2 = (1 << 3),
        FILTER_WW = (1 << 4),
        FILTER_TT = (1 << 5),
        FILTER_OO = (1 << 6),
        FILTER_CUSTOM = (1 << 7),

        FILTER_RIDE_TRANSPORT = (1 << 8),
        FILTER_RIDE_GENTLE = (1 << 9),
        FILTER_RIDE_COASTER = (1 << 10),
        FILTER_RIDE_THRILL = (1 << 11),
        FILTER_RIDE_WATER = (1 << 12),
        FILTER_RIDE_STALL = (1 << 13),

        FILTER_SELECTED = (1 << 14),
        FILTER_NONSELECTED = (1 << 15),

        FILTER_RIDES_ALL = FILTER_RIDE_TRANSPORT | FILTER_RIDE_GENTLE | FILTER_RIDE_COASTER | FILTER_RIDE_THRILL
            | FILTER_RIDE_WATER | FILTER_RIDE_STALL,
        FILTER_SOURCES_ALL = FILTER_RCT1 | FILTER_AA | FILTER_LL | FILTER_RCT2 | FILTER_WW | FILTER_TT | FILTER_OO
            | FILTER_CUSTOM,
        FILTER_ALL = FILTER_RIDES_ALL | FILTER_SOURCES_ALL | FILTER_SELECTED | FILTER_NONSELECTED,
    };

    enum
    {
        RIDE_SORT_TYPE,
        RIDE_SORT_RIDE
    };

    enum : uint8_t
    {
        DDIX_FILTER_RCT1,
        DDIX_FILTER_AA,
        DDIX_FILTER_LL,
        DDIX_FILTER_RCT2,
        DDIX_FILTER_WW,
        DDIX_FILTER_TT,
        DDIX_FILTER_OO,
        DDIX_FILTER_CUSTOM,
        DDIX_FILTER_SEPARATOR,
        DDIX_FILTER_SELECTED,
        DDIX_FILTER_NONSELECTED,
    };

    struct ObjectListItem
    {
        const ObjectRepositoryItem* repositoryItem;
        std::unique_ptr<RideFilters> filter;
        uint8_t* flags;
    };

    static constexpr uint8_t _numSourceGameItems = 8;

    static uint32_t _filter_flags;
    static uint16_t _filter_object_counts[EnumValue(ObjectType::Count)];

    static char _filter_string[MAX_PATH];

    static bool isFilterActive(const uint16_t filter)
    {
        return (_filter_flags & filter) == filter;
    }

    static constexpr StringId WINDOW_TITLE = STR_OBJECT_SELECTION;
    static constexpr int32_t WH = 400;
    static constexpr int32_t WW = 600;
    static constexpr auto kFilterWidth = 150;
    static constexpr auto kPreviewSize = 113;

    struct ObjectSubTab
    {
        StringId tooltip;
        ObjectType subObjectType;
        uint16_t flagFilter;
        uint32_t baseImage;
        uint8_t animationLength;
        uint8_t animationDivisor;
    };

    struct ObjectPageDesc
    {
        StringId Caption;
        ObjectType mainObjectType;
        uint32_t Image;
        std::span<ObjectSubTab> subTabs;
    };

    // clang-format off
    static ObjectSubTab kRideObjectSubTabs[] = {
        { STR_OBJECT_FILTER_ALL_RIDES_TIP, ObjectType::Ride, FILTER_RIDES_ALL,      SPR_G2_INFINITY,                  1, 1 },
        { STR_TRANSPORT_RIDES_TIP,         ObjectType::Ride, FILTER_RIDE_TRANSPORT, SPR_TAB_RIDES_TRANSPORT_0,       20, 4 },
        { STR_GENTLE_RIDES_TIP,            ObjectType::Ride, FILTER_RIDE_GENTLE,    SPR_TAB_RIDES_GENTLE_0,          32, 8 },
        { STR_ROLLER_COASTERS_TIP,         ObjectType::Ride, FILTER_RIDE_COASTER,   SPR_TAB_RIDES_ROLLER_COASTERS_0, 10, 2 },
        { STR_THRILL_RIDES_TIP,            ObjectType::Ride, FILTER_RIDE_THRILL,    SPR_TAB_RIDES_THRILL_0,          72, 4 },
        { STR_WATER_RIDES_TIP,             ObjectType::Ride, FILTER_RIDE_WATER,     SPR_TAB_RIDES_WATER_0,           24, 4 },
        { STR_SHOPS_STALLS_TIP,            ObjectType::Ride, FILTER_RIDE_STALL,     SPR_TAB_RIDES_SHOP_0,            28, 4 },
    };

    static ObjectSubTab kSceneryObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_SCENERY_GROUPS,    ObjectType::SceneryGroup,     FILTER_NONE, SPR_TAB_SCENERY_STATUES,    1, 1 },
        { STR_OBJECT_SELECTION_SMALL_SCENERY,     ObjectType::SmallScenery,     FILTER_NONE, SPR_TAB_SCENERY_TREES,      1, 1 },
        { STR_OBJECT_SELECTION_LARGE_SCENERY,     ObjectType::LargeScenery,     FILTER_NONE, SPR_TAB_SCENERY_URBAN,      1, 1 },
        { STR_OBJECT_SELECTION_WALLS_FENCES,      ObjectType::Walls,            FILTER_NONE, SPR_TAB_SCENERY_WALLS,      1, 1 },
    };

    static ObjectSubTab kPathObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_FOOTPATH_SURFACES, ObjectType::FootpathSurface,  FILTER_NONE, SPR_G2_PATH_SURFACE_TAB,    1, 1 },
        { STR_OBJECT_SELECTION_FOOTPATH_RAILINGS, ObjectType::FootpathRailings, FILTER_NONE, SPR_G2_PATH_RAILINGS_TAB,   1, 1 },
        { STR_OBJECT_SELECTION_FOOTPATHS,         ObjectType::Paths,            FILTER_NONE, SPR_G2_LEGACY_PATH_TAB,     1, 1 },
        { STR_OBJECT_SELECTION_PATH_EXTRAS,       ObjectType::PathAdditions,    FILTER_NONE, SPR_TAB_SCENERY_PATH_ITEMS, 1, 1 },
        { STR_OBJECT_SELECTION_PATH_SIGNS,        ObjectType::Banners,          FILTER_NONE, SPR_TAB_SCENERY_SIGNAGE,    1, 1 },
    };

    static ObjectSubTab kEntrancesObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_PARK_ENTRANCE,     ObjectType::ParkEntrance,     FILTER_NONE, SPR_TAB_PARK,            1, 1 },
        { STR_OBJECT_SELECTION_STATIONS,          ObjectType::Station,          FILTER_NONE, SPR_G2_RIDE_STATION_TAB, 1, 1 },
    };

    static ObjectSubTab kTerrainObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_TERRAIN_SURFACES,  ObjectType::TerrainSurface,   FILTER_NONE, SPR_G2_TAB_LAND,         1, 1 },
        { STR_OBJECT_SELECTION_TERRAIN_EDGES,     ObjectType::TerrainEdge,      FILTER_NONE, SPR_G2_TERRAIN_EDGE_TAB, 1, 1 },
        { STR_OBJECT_SELECTION_WATER,             ObjectType::Water,            FILTER_NONE, SPR_TAB_WATER,           1, 1 },
    };

    static ObjectSubTab kPeepObjectSubTabs[] = {
        { STR_OBJECT_SELECTION_PEEP_ANIMATIONS,   ObjectType::PeepAnimations,   FILTER_NONE, SPR_G2_PEEP_ANIMATIONS, 1, 1 },
        { STR_OBJECT_SELECTION_PEEP_NAMES,        ObjectType::PeepNames,        FILTER_NONE, SPR_TAB_GUESTS_0,       1, 1 },
    };

    static constexpr ObjectPageDesc ObjectSelectionPages[] = {
        { STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS, ObjectType::Ride,            SPR_TAB_RIDE_16,         kRideObjectSubTabs },
        { STR_OBJECT_SELECTION_SCENERY_GROUPS,            ObjectType::SceneryGroup,    SPR_TAB_SCENERY_STATUES, kSceneryObjectSubTabs },
        { STR_OBJECT_SELECTION_FOOTPATH_SURFACES,         ObjectType::FootpathSurface, SPR_G2_LEGACY_PATH_TAB,  kPathObjectSubTabs },
        { STR_OBJECT_SELECTION_PARK_ENTRANCE,             ObjectType::ParkEntrance,    SPR_TAB_PARK,            kEntrancesObjectSubTabs },
        { STR_OBJECT_SELECTION_TERRAIN_SURFACES,          ObjectType::TerrainSurface,  SPR_G2_TAB_LAND,         kTerrainObjectSubTabs },
        { STR_OBJECT_SELECTION_MUSIC,                     ObjectType::Music,           SPR_TAB_MUSIC_0,         {} },
        { STR_OBJECT_SELECTION_PEEP_NAMES,                ObjectType::PeepNames,       SPR_TAB_GUESTS_0,        kPeepObjectSubTabs },
    };
    // clang-format on

#pragma region Widgets

    enum WINDOW_EDITOR_OBJECT_SELECTION_WIDGET_IDX
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_CONTENT_PANEL,
        WIDX_LIST,
        WIDX_PREVIEW,
        WIDX_INSTALL_TRACK,
        WIDX_FILTER_DROPDOWN,
        WIDX_FILTER_DROPDOWN_BTN,
        WIDX_FILTER_TEXT_BOX,
        WIDX_FILTER_CLEAR_BUTTON,
        WIDX_FILTER_RIDE_TAB_FRAME,
        WIDX_SUB_TAB_0,
        WIDX_SUB_TAB_1,
        WIDX_SUB_TAB_2,
        WIDX_SUB_TAB_3,
        WIDX_SUB_TAB_4,
        WIDX_SUB_TAB_5,
        WIDX_SUB_TAB_6,
        WIDX_LIST_SORT_TYPE,
        WIDX_LIST_SORT_RIDE,
        WIDX_RELOAD_OBJECT,
        WIDX_TAB_1,
    };

    validate_global_widx(WC_EDITOR_OBJECT_SELECTION, WIDX_TAB_1);

    // clang-format off
    static constexpr Widget _window_editor_object_selection_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget         ({  0, 43}, {WW,  357}, WindowWidgetType::Resize,       WindowColour::Secondary                                                                  ),
        MakeWidget         ({  4, 60}, {288, 277}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL                                                 ),
        MakeWidget         ({391, 45}, {114, 114}, WindowWidgetType::FlatBtn,      WindowColour::Secondary                                                                  ),
        MakeWidget         ({340, 22}, {122,  14}, WindowWidgetType::Button,       WindowColour::Primary,   STR_INSTALL_NEW_TRACK_DESIGN,  STR_INSTALL_NEW_TRACK_DESIGN_TIP ),
        MakeDropdownWidgets({470, 22}, {114,  14}, WindowWidgetType::DropdownMenu, WindowColour::Primary,   STR_OBJECT_FILTER,             STR_OBJECT_FILTER_TIP            ),
        MakeWidget         ({  4, 45}, {211,  14}, WindowWidgetType::TextBox,      WindowColour::Secondary                                                                  ),
        MakeWidget         ({218, 45}, { 70,  14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_OBJECT_SEARCH_CLEAR                                         ),
        MakeWidget         ({  3, 73}, {285,   4}, WindowWidgetType::ImgBtn,       WindowColour::Secondary                                                                  ),
        MakeTab            ({  3, 47}),
        MakeTab            ({ 34, 47}),
        MakeTab            ({ 65, 47}),
        MakeTab            ({ 96, 47}),
        MakeTab            ({127, 47}),
        MakeTab            ({158, 47}),
        MakeTab            ({189, 47}),
        MakeWidget         ({  4, 80}, {145,  14}, WindowWidgetType::TableHeader, WindowColour::Secondary                                                                  ),
        MakeWidget         ({149, 80}, {143,  14}, WindowWidgetType::TableHeader, WindowColour::Secondary                                                                  ),
        MakeWidget         ({700, 50}, { 24,  24}, WindowWidgetType::ImgBtn,      WindowColour::Secondary,  SPR_G2_RELOAD,    STR_RELOAD_OBJECT_TIP ),
        MakeTab            ({  3, 17},                                                                                       STR_STRING_DEFINED_TOOLTIP       ),
        // Copied object type times...
    };
    // clang-format on

#pragma endregion

    static StringId GetRideTypeStringId(const ObjectRepositoryItem* item);
    static bool VisibleListSortRideType(const ObjectListItem& a, const ObjectListItem& b);
    static bool VisibleListSortRideName(const ObjectListItem& a, const ObjectListItem& b);
    void EditorLoadSelectedObjects();

    class EditorObjectSelectionWindow final : public Window
    {
        using sortFunc_t = bool (*)(const ObjectListItem&, const ObjectListItem&);

    private:
        std::vector<ObjectListItem> _listItems;
        int32_t _listSortType = RIDE_SORT_TYPE;
        bool _listSortDescending = false;
        std::unique_ptr<Object> _loadedObject;
        uint8_t _selectedSubTab = 0;

    public:
        /**
         *
         * rct2: 0x006AA64E
         */
        void OnOpen() override
        {
            InitWidgets();

            Sub6AB211();
            ResetSelectedObjectCountAndSize();

            widgets[WIDX_FILTER_TEXT_BOX].string = _filter_string;

            _filter_flags = FILTER_RIDES_ALL | Config::Get().interface.ObjectSelectionFilterFlags;
            std::fill_n(_filter_string, sizeof(_filter_string), 0x00);

            WindowInitScrollWidgets(*this);

            selected_tab = 0;
            selected_list_item = -1;
            min_width = WW;
            min_height = WH;
            max_width = 1200;
            max_height = 1000;

            _listSortType = RIDE_SORT_TYPE;
            _listSortDescending = false;

            disabled_widgets |= 1u << WIDX_FILTER_RIDE_TAB_FRAME;

            VisibleListRefresh();
        }

        bool CanClose() override
        {
            // Prevent window closure when selection is invalid
            return EditorObjectSelectionWindowCheck();
        }

        /**
         *
         * rct2: 0x006AB199
         */
        void OnClose() override
        {
            UnloadUnselectedObjects();
            EditorLoadSelectedObjects();
            EditorObjectFlagsFree();

            if (_loadedObject != nullptr)
                _loadedObject->Unload();

            if (gScreenFlags & SCREEN_FLAGS_EDITOR)
            {
                ResearchPopulateListRandom();
            }
            else
            {
                // Used for in-game object selection cheat
                // This resets the ride selection list and resets research to 0 on current item
                gSilentResearch = true;
                ResearchResetCurrentItem();
                gSilentResearch = false;
            }

            auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
            ContextBroadcastIntent(&intent);

            VisibleListDispose();

            intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
            ContextBroadcastIntent(&intent);
        }

        void OnUpdate() override
        {
            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_FILTER_TEXT_BOX);
            }

            auto& currentPage = ObjectSelectionPages[selected_tab];
            if (currentPage.subTabs.empty())
                return;

            if (_selectedSubTab == 0)
                return;

            auto& subTabDef = currentPage.subTabs[_selectedSubTab];

            frame_no++;
            if (frame_no >= subTabDef.animationLength)
                frame_no = 0;

            InvalidateWidget(WIDX_SUB_TAB_0 + _selectedSubTab);
        }

        /**
         *
         * rct2: 0x006AAFAB
         */
        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && !EditorObjectSelectionWindowCheck())
                        return;

                    if (gScreenFlags & SCREEN_FLAGS_EDITOR)
                    {
                        FinishObjectSelection();
                    }
                    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                    {
                        GameNotifyMapChange();
                        GameUnloadScripts();

                        auto* context = OpenRCT2::GetContext();
                        context->SetActiveScene(context->GetTitleScene());
                    }
                    break;

                case WIDX_SUB_TAB_0:
                case WIDX_SUB_TAB_1:
                case WIDX_SUB_TAB_2:
                case WIDX_SUB_TAB_3:
                case WIDX_SUB_TAB_4:
                case WIDX_SUB_TAB_5:
                case WIDX_SUB_TAB_6:
                {
                    _selectedSubTab = widgetIndex - WIDX_SUB_TAB_0;

                    auto& currentPage = ObjectSelectionPages[selected_tab];
                    auto& subTabDef = currentPage.subTabs[_selectedSubTab];
                    _filter_flags &= ~FILTER_RIDES_ALL;
                    _filter_flags |= subTabDef.flagFilter;

                    Config::Get().interface.ObjectSelectionFilterFlags = _filter_flags;
                    Config::Save();

                    FilterUpdateCounts();
                    VisibleListRefresh();

                    selected_list_item = -1;
                    scrolls[0].contentOffsetY = 0;
                    frame_no = 0;
                    Invalidate();
                    break;
                }

                case WIDX_INSTALL_TRACK:
                {
                    if (selected_list_item != -1)
                    {
                        selected_list_item = -1;
                    }
                    Invalidate();

                    auto intent = Intent(WindowClass::Loadsave);
                    intent.PutExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK);
                    ContextOpenIntent(&intent);
                    break;
                }
                case WIDX_FILTER_TEXT_BOX:
                    WindowStartTextbox(*this, widgetIndex, _filter_string, sizeof(_filter_string));
                    break;
                case WIDX_FILTER_CLEAR_BUTTON:
                    std::fill_n(_filter_string, sizeof(_filter_string), 0x00);
                    FilterUpdateCounts();
                    scrolls->contentOffsetY = 0;
                    VisibleListRefresh();
                    Invalidate();
                    break;
                case WIDX_LIST_SORT_TYPE:
                    if (_listSortType == RIDE_SORT_TYPE)
                    {
                        _listSortDescending = !_listSortDescending;
                    }
                    else
                    {
                        _listSortType = RIDE_SORT_TYPE;
                        _listSortDescending = false;
                    }
                    VisibleListRefresh();
                    break;
                case WIDX_LIST_SORT_RIDE:
                    if (_listSortType == RIDE_SORT_RIDE)
                    {
                        _listSortDescending = !_listSortDescending;
                    }
                    else
                    {
                        _listSortType = RIDE_SORT_RIDE;
                        _listSortDescending = false;
                    }
                    VisibleListRefresh();
                    break;
                case WIDX_RELOAD_OBJECT:
                    if (_loadedObject != nullptr)
                    {
                        auto descriptor = _loadedObject->GetDescriptor();
                        auto& objectManager = GetContext()->GetObjectManager();
                        auto entryIndex = objectManager.GetLoadedObjectEntryIndex(descriptor);
                        if (entryIndex != kObjectEntryIndexNull)
                        {
                            objectManager.UnloadObjects({ descriptor });
                            objectManager.LoadObject(descriptor, entryIndex);
                        }
                    }
                    break;
                default:
                    if (widgetIndex >= WIDX_TAB_1
                        && static_cast<size_t>(widgetIndex) < WIDX_TAB_1 + std::size(ObjectSelectionPages))
                    {
                        SetPage(widgetIndex - WIDX_TAB_1);
                    }
                    break;
            }
        }

        void OnResize() override
        {
            WindowSetResize(*this, WW, WH, 1200, 1000);
        }

        static constexpr StringId kSourceStringIds[] = {
            STR_SCENARIO_CATEGORY_RCT1,          STR_SCENARIO_CATEGORY_RCT1_AA, STR_SCENARIO_CATEGORY_RCT1_LL,
            STR_ROLLERCOASTER_TYCOON_2_DROPDOWN, STR_OBJECT_FILTER_WW,          STR_OBJECT_FILTER_TT,
            STR_OBJECT_FILTER_OPENRCT2_OFFICIAL, STR_OBJECT_FILTER_CUSTOM,
        };

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            int32_t numSelectionItems = 0;

            switch (widgetIndex)
            {
                case WIDX_FILTER_DROPDOWN_BTN:
                    for (auto ddIdx = EnumValue(DDIX_FILTER_RCT1); ddIdx <= EnumValue(DDIX_FILTER_CUSTOM); ddIdx++)
                    {
                        gDropdownItems[ddIdx].Args = kSourceStringIds[ddIdx];
                        gDropdownItems[ddIdx].Format = STR_TOGGLE_OPTION;
                    }

                    // Track manager cannot select multiple, so only show selection filters if not in track manager
                    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                    {
                        numSelectionItems = 3;
                        gDropdownItems[DDIX_FILTER_SEPARATOR].Format = 0;
                        gDropdownItems[DDIX_FILTER_SELECTED].Format = STR_TOGGLE_OPTION;
                        gDropdownItems[DDIX_FILTER_NONSELECTED].Format = STR_TOGGLE_OPTION;
                        gDropdownItems[DDIX_FILTER_SEPARATOR].Args = kStringIdNone;
                        gDropdownItems[DDIX_FILTER_SELECTED].Args = STR_SELECTED_ONLY;
                        gDropdownItems[DDIX_FILTER_NONSELECTED].Args = STR_NON_SELECTED_ONLY;
                    }

                    auto& ddWidget = widgets[WIDX_FILTER_DROPDOWN];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget.left, windowPos.y + ddWidget.top }, ddWidget.height() + 1,
                        colours[ddWidget.colour], Dropdown::Flag::StayOpen, _numSourceGameItems + numSelectionItems);

                    for (int32_t i = 0; i < _numSourceGameItems; i++)
                    {
                        if (_filter_flags & (1 << i))
                        {
                            Dropdown::SetChecked(i, true);
                        }
                    }

                    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                    {
                        Dropdown::SetChecked(DDIX_FILTER_SELECTED, isFilterActive(FILTER_SELECTED));
                        Dropdown::SetChecked(DDIX_FILTER_NONSELECTED, isFilterActive(FILTER_NONSELECTED));
                    }
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
                return;

            switch (widgetIndex)
            {
                case WIDX_FILTER_DROPDOWN_BTN:
                    if (dropdownIndex == DDIX_FILTER_SELECTED)
                    {
                        _filter_flags ^= FILTER_SELECTED;
                        _filter_flags &= ~FILTER_NONSELECTED;
                    }
                    else if (dropdownIndex == DDIX_FILTER_NONSELECTED)
                    {
                        _filter_flags ^= FILTER_NONSELECTED;
                        _filter_flags &= ~FILTER_SELECTED;
                    }
                    else
                    {
                        _filter_flags ^= (1 << dropdownIndex);
                    }
                    Config::Get().interface.ObjectSelectionFilterFlags = _filter_flags;
                    Config::Save();

                    FilterUpdateCounts();
                    scrolls->contentOffsetY = 0;

                    VisibleListRefresh();
                    Invalidate();
                    break;
            }
        }

        /**
         *
         * rct2: 0x006AB031
         */
        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            const auto newHeight = static_cast<int32_t>(_listItems.size() * kScrollableRowHeight);
            return { 0, newHeight };
        }

        /**
         *
         * rct2: 0x006AB0B6
         */
        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // Used for in-game object selection cheat to prevent crashing the game
            // when windows attempt to draw objects that don't exist any more
            auto* windowMgr = Ui::GetWindowManager();
            windowMgr->CloseAllExceptClass(WindowClass::EditorObjectSelection);

            int32_t selected_object = GetObjectFromObjectSelection(GetSelectedObjectType(), screenCoords.y);
            if (selected_object == -1)
                return;

            ObjectListItem* listItem = &_listItems[selected_object];
            uint8_t object_selection_flags = *listItem->flags;
            if (object_selection_flags & ObjectSelectionFlags::Flag6)
                return;

            Invalidate();

            const CursorState* state = ContextGetCursorState();
            Audio::Play(Audio::SoundId::Click1, 0, state->position.x);

            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
            {
                const auto objectSelectResult = WindowEditorObjectSelectionSelectObject(
                    0, INPUT_FLAG_EDITOR_OBJECT_SELECT, listItem->repositoryItem);
                if (!objectSelectResult.Successful)
                    return;

                if (_loadedObject != nullptr)
                {
                    _loadedObject->Unload();
                    _loadedObject = nullptr;
                }

                auto& objRepository = GetContext()->GetObjectRepository();
                _loadedObject = objRepository.LoadObject(listItem->repositoryItem);
                auto& objManager = GetContext()->GetObjectManager();
                objManager.LoadObject(_loadedObject.get()->GetIdentifier());

                // This function calls window_track_list_open
                ManageTracks();
                Close();
                return;
            }

            uint32_t inputFlags = INPUT_FLAG_EDITOR_OBJECT_1 | INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP;
            // If already selected
            if (!(object_selection_flags & ObjectSelectionFlags::Selected))
                inputFlags |= INPUT_FLAG_EDITOR_OBJECT_SELECT;

            _gSceneryGroupPartialSelectError = std::nullopt;
            const auto objectSelectResult = WindowEditorObjectSelectionSelectObject(0, inputFlags, listItem->repositoryItem);
            if (!objectSelectResult.Successful)
            {
                StringId error_title = (inputFlags & INPUT_FLAG_EDITOR_OBJECT_SELECT) ? STR_UNABLE_TO_SELECT_THIS_OBJECT
                                                                                      : STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

                ContextShowError(error_title, objectSelectResult.Message, {});
                return;
            }

            if (isFilterActive(FILTER_SELECTED) || isFilterActive(FILTER_NONSELECTED))
            {
                FilterUpdateCounts();
                VisibleListRefresh();
                Invalidate();
            }

            if (_gSceneryGroupPartialSelectError.has_value())
            {
                const auto errorMessage = _gSceneryGroupPartialSelectError.value();
                if (errorMessage == STR_OBJECT_SELECTION_ERR_TOO_MANY_OF_TYPE_SELECTED)
                {
                    ContextShowError(
                        STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED, {});
                }
                else
                {
                    ContextShowError(
                        errorMessage, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED, Formatter::Common());
                }
            }
        }

        /**
         *
         * rct2: 0x006AB079
         */
        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t selectedObject = GetObjectFromObjectSelection(GetSelectedObjectType(), screenCoords.y);
            if (selectedObject != -1)
            {
                ObjectListItem* listItem = &_listItems[selectedObject];
                uint8_t objectSelectionFlags = *listItem->flags;
                if (objectSelectionFlags & ObjectSelectionFlags::Flag6)
                {
                    selectedObject = -1;
                }
            }
            if (selectedObject != selected_list_item)
            {
                selected_list_item = selectedObject;

                if (_loadedObject != nullptr)
                {
                    _loadedObject->Unload();
                    _loadedObject = nullptr;
                }

                if (selectedObject != -1)
                {
                    auto listItem = &_listItems[selectedObject];
                    auto& objRepository = GetContext()->GetObjectRepository();
                    _loadedObject = objRepository.LoadObject(listItem->repositoryItem);
                    if (_loadedObject != nullptr)
                    {
                        _loadedObject->Load();
                    }
                }

                Invalidate();
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            // ScrollPaint
            ScreenCoordsXY screenCoords;
            bool ridePage = (GetSelectedObjectType() == ObjectType::Ride);

            uint8_t paletteIndex = ColourMapA[colours[1].colour].mid_light;
            GfxClear(dpi, paletteIndex);

            screenCoords.y = 0;
            for (size_t i = 0; i < _listItems.size(); i++)
            {
                const auto& listItem = _listItems[i];
                if (screenCoords.y + kScrollableRowHeight >= dpi.y && screenCoords.y <= dpi.y + dpi.height)
                {
                    // Draw checkbox
                    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && !(*listItem.flags & 0x20))
                        GfxFillRectInset(
                            dpi, { { 2, screenCoords.y }, { 11, screenCoords.y + 10 } }, colours[1], INSET_RECT_F_E0);

                    // Highlight background
                    auto highlighted = i == static_cast<size_t>(selected_list_item)
                        && !(*listItem.flags & ObjectSelectionFlags::Flag6);
                    if (highlighted)
                    {
                        auto bottom = screenCoords.y + (kScrollableRowHeight - 1);
                        GfxFilterRect(dpi, { 0, screenCoords.y, width, bottom }, FilterPaletteID::PaletteDarken1);
                    }

                    // Draw checkmark
                    if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && (*listItem.flags & ObjectSelectionFlags::Selected))
                    {
                        screenCoords.x = 2;
                        auto darkness = highlighted ? TextDarkness::ExtraDark : TextDarkness::Dark;
                        auto colour2 = colours[1].withFlag(ColourFlag::translucent, false);
                        if (*listItem.flags & (ObjectSelectionFlags::InUse | ObjectSelectionFlags::AlwaysRequired))
                            colour2.setFlag(ColourFlag::inset, true);

                        DrawText(dpi, screenCoords, { colour2, FontStyle::Medium, darkness }, kCheckMarkString);
                    }

                    screenCoords.x = gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;

                    utf8 itemBuffer[512]{};
                    auto bufferWithColour = strcpy(itemBuffer, highlighted ? "{WINDOW_COLOUR_2}" : "{BLACK}");
                    auto buffer = strchr(bufferWithColour, '\0');

                    colour_t colour = COLOUR_BLACK;
                    auto darkness = TextDarkness::Regular;
                    if (*listItem.flags & ObjectSelectionFlags::Flag6)
                    {
                        colour = colours[1].colour;
                        darkness = TextDarkness::Dark;
                    }

                    int32_t width_limit = widgets[WIDX_LIST].width() - screenCoords.x;

                    if (ridePage)
                    {
                        width_limit /= 2;
                        // Draw ride type
                        StringId rideTypeStringId = GetRideTypeStringId(listItem.repositoryItem);
                        String::safeUtf8Copy(buffer, LanguageGetString(rideTypeStringId), 256 - (buffer - bufferWithColour));
                        auto ft = Formatter();
                        ft.Add<const char*>(itemBuffer);
                        DrawTextEllipsised(
                            dpi, screenCoords, width_limit - 15, STR_STRING, ft, { colour, FontStyle::Medium, darkness });
                        screenCoords.x = widgets[WIDX_LIST_SORT_RIDE].left - widgets[WIDX_LIST].left;
                    }

                    // Draw text
                    String::safeUtf8Copy(buffer, listItem.repositoryItem->Name.c_str(), 256 - (buffer - bufferWithColour));
                    if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                    {
                        while (*buffer != 0 && *buffer != 9)
                            buffer++;

                        *buffer = 0;
                    }
                    auto ft = Formatter();
                    ft.Add<const char*>(itemBuffer);
                    DrawTextEllipsised(dpi, screenCoords, width_limit, STR_STRING, ft, { colour, FontStyle::Medium, darkness });
                }
                screenCoords.y += kScrollableRowHeight;
            }
        }

        /**
         *
         * rct2: 0x006AB058
         */
        OpenRCT2String OnTooltip(const WidgetIndex widgetIndex, const StringId fallback) override
        {
            if (widgetIndex >= WIDX_TAB_1 && static_cast<size_t>(widgetIndex) < WIDX_TAB_1 + std::size(ObjectSelectionPages))
            {
                auto ft = Formatter();
                ft.Add<StringId>(ObjectSelectionPages[(widgetIndex - WIDX_TAB_1)].Caption);
                return { fallback, ft };
            }
            return { fallback, {} };
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_FILTER_TEXT_BOX)
                return;

            std::string tempText = text.data();
            const char* c = tempText.c_str();

            if (strcmp(_filter_string, c) == 0)
                return;

            String::safeUtf8Copy(_filter_string, c, sizeof(_filter_string));

            FilterUpdateCounts();

            scrolls->contentOffsetY = 0;

            VisibleListRefresh();
            Invalidate();
        }

        void OnPrepareDraw() override
        {
            // Resize widgets
            ResizeFrameWithPage();
            widgets[WIDX_LIST].right = width - 309;
            widgets[WIDX_LIST].bottom = height - 14;
            widgets[WIDX_PREVIEW].left = width - 209;
            widgets[WIDX_PREVIEW].right = width - 96;
            widgets[WIDX_RELOAD_OBJECT].left = width - 9 - 24;
            widgets[WIDX_RELOAD_OBJECT].right = width - 9;

            auto& dropdownWidget = widgets[WIDX_FILTER_DROPDOWN];
            ResizeDropdown(WIDX_FILTER_DROPDOWN, { width - kFilterWidth - 10, dropdownWidget.top }, { kFilterWidth, 14 });
            auto& installTrackWidget = widgets[WIDX_INSTALL_TRACK];
            installTrackWidget.moveToX(dropdownWidget.left - installTrackWidget.width() - 10);

            // Set pressed widgets
            pressed_widgets |= 1uLL << WIDX_PREVIEW;
            SetPressedTab();

            // Set window title and buttons
            auto& titleWidget = widgets[WIDX_TITLE];
            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
            {
                titleWidget.text = STR_TRACK_DESIGNS_MANAGER_SELECT_RIDE_TYPE;
                installTrackWidget.type = WindowWidgetType::Button;
            }
            else if (gScreenFlags & SCREEN_FLAGS_TRACK_DESIGNER)
            {
                titleWidget.text = STR_ROLLER_COASTER_DESIGNER_SELECT_RIDE_TYPES_VEHICLES;
                installTrackWidget.type = WindowWidgetType::Empty;
            }
            else
            {
                titleWidget.text = STR_OBJECT_SELECTION;
                installTrackWidget.type = WindowWidgetType::Empty;
            }

            // Set title parameters for current page
            const auto& currentPage = ObjectSelectionPages[selected_tab];
            auto ft = Formatter::Common();
            if (!currentPage.subTabs.empty())
                ft.Add<StringId>(currentPage.subTabs[_selectedSubTab].tooltip);
            else
                ft.Add<StringId>(currentPage.Caption);

            // Set filter dropdown caption
            if (!isFilterActive(FILTER_SOURCES_ALL))
            {
                // Only one source active?
                uint32_t sources = _filter_flags & FILTER_SOURCES_ALL;
                auto numSourcesActive = std::popcount(sources);
                if (numSourcesActive == 1)
                {
                    widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_ONLY_STRINGID;
                    auto firstActiveSource = Numerics::bitScanForward(sources);
                    ft.Add<StringId>(kSourceStringIds[firstActiveSource]);
                }
                else
                {
                    widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_SHOWING_N_SOURCES;
                    ft.Add<uint16_t>(numSourcesActive);
                }
            }
            else
                widgets[WIDX_FILTER_DROPDOWN].text = STR_OBJECT_SELECTION_ALL_SOURCES_SHOWN;

            // Align main tabs
            int32_t x = 3;
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                auto& widget = widgets[WIDX_TAB_1 + i];
                if (ObjectSelectionPages[i].Image != kSpriteIdNull)
                {
                    widget.type = WindowWidgetType::Tab;
                    widget.left = x;
                    widget.right = x + 30;
                    x += 31;
                }
                else
                    widget.type = WindowWidgetType::Empty;
            }

            if (Config::Get().general.DebuggingTools)
                widgets[WIDX_RELOAD_OBJECT].type = WindowWidgetType::ImgBtn;
            else
                widgets[WIDX_RELOAD_OBJECT].type = WindowWidgetType::Empty;

            if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TRACK_DESIGNER))
            {
                for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
                {
                    widgets[WIDX_TAB_1 + i].type = WindowWidgetType::Empty;
                }
            }

            // Do we have any sub-tabs?
            const bool hasSubTabs = !currentPage.subTabs.empty();

            widgets[WIDX_LIST].right = width / 2 - 2;
            widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = widgets[WIDX_LIST].right;

            widgets[WIDX_FILTER_TEXT_BOX].right = widgets[WIDX_LIST].right - 77;
            widgets[WIDX_FILTER_TEXT_BOX].top = (hasSubTabs ? 79 : 48);
            widgets[WIDX_FILTER_TEXT_BOX].bottom = (hasSubTabs ? 92 : 61);

            widgets[WIDX_FILTER_CLEAR_BUTTON].left = widgets[WIDX_LIST].right - 73;
            widgets[WIDX_FILTER_CLEAR_BUTTON].right = widgets[WIDX_LIST].right;
            widgets[WIDX_FILTER_CLEAR_BUTTON].top = (hasSubTabs ? 79 : 48);
            widgets[WIDX_FILTER_CLEAR_BUTTON].bottom = (hasSubTabs ? 92 : 61);

            // Toggle sub-tab visibility
            const auto numSubTabs = static_cast<int8_t>(currentPage.subTabs.size());
            for (int8_t i = 0; i <= 6; i++)
            {
                widgets[WIDX_SUB_TAB_0 + i].tooltip = i < numSubTabs ? currentPage.subTabs[i].tooltip : kStringIdNone;
                widgets[WIDX_SUB_TAB_0 + i].type = i < numSubTabs ? WindowWidgetType::Tab : WindowWidgetType::Empty;
                pressed_widgets &= ~(1uLL << (WIDX_SUB_TAB_0 + i));
            }

            // Mark current sub-tab as active, and toggle tab frame
            if (hasSubTabs)
            {
                pressed_widgets |= (1uLL << (WIDX_SUB_TAB_0 + _selectedSubTab));
                widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WindowWidgetType::ImgBtn;
            }
            else
                widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WindowWidgetType::Empty;

            // The ride tab has two headers for the list
            bool isRideTab = GetSelectedObjectType() == ObjectType::Ride;
            if (isRideTab)
            {
                int32_t width_limit = (widgets[WIDX_LIST].width() - 15) / 2;

                widgets[WIDX_LIST_SORT_TYPE].type = WindowWidgetType::TableHeader;
                widgets[WIDX_LIST_SORT_TYPE].top = widgets[WIDX_FILTER_TEXT_BOX].bottom + 3;
                widgets[WIDX_LIST_SORT_TYPE].bottom = widgets[WIDX_LIST_SORT_TYPE].top + 13;
                widgets[WIDX_LIST_SORT_TYPE].left = 4;
                widgets[WIDX_LIST_SORT_TYPE].right = widgets[WIDX_LIST_SORT_TYPE].left + width_limit;

                widgets[WIDX_LIST_SORT_RIDE].type = WindowWidgetType::TableHeader;
                widgets[WIDX_LIST_SORT_RIDE].top = widgets[WIDX_LIST_SORT_TYPE].top;
                widgets[WIDX_LIST_SORT_RIDE].bottom = widgets[WIDX_LIST_SORT_TYPE].bottom;
                widgets[WIDX_LIST_SORT_RIDE].left = widgets[WIDX_LIST_SORT_TYPE].right + 1;
                widgets[WIDX_LIST_SORT_RIDE].right = widgets[WIDX_LIST].right;

                widgets[WIDX_LIST].top = widgets[WIDX_LIST_SORT_TYPE].bottom + 2;
            }
            else
            {
                widgets[WIDX_LIST_SORT_TYPE].type = WindowWidgetType::Empty;
                widgets[WIDX_LIST_SORT_RIDE].type = WindowWidgetType::Empty;

                widgets[WIDX_LIST].top = widgets[WIDX_FILTER_TEXT_BOX].bottom + 2;
            }

            widgets[WIDX_PREVIEW].top = widgets[WIDX_FILTER_TEXT_BOX].top;
            widgets[WIDX_PREVIEW].bottom = widgets[WIDX_PREVIEW].top + kPreviewSize;
            widgets[WIDX_PREVIEW].left = width * 3 / 4 - kPreviewSize / 2;
            widgets[WIDX_PREVIEW].right = widgets[WIDX_PREVIEW].left + kPreviewSize;
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            // Draw main tab images
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                const auto& widget = widgets[WIDX_TAB_1 + i];
                if (widget.type != WindowWidgetType::Empty)
                {
                    auto image = ImageId(ObjectSelectionPages[i].Image);
                    auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                    GfxDrawSprite(dpi, image, screenPos);
                }
            }

            constexpr int32_t kThrillRidesTabAnimationSequence[] = {
                5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0,
            };

            // Draw sub-tab images, if applicable
            auto& currentPage = ObjectSelectionPages[selected_tab];
            if (!currentPage.subTabs.empty())
            {
                for (auto i = 0u; i < currentPage.subTabs.size(); i++)
                {
                    const auto& widget = widgets[WIDX_SUB_TAB_0 + i];
                    if (widget.type == WindowWidgetType::Empty)
                        continue;

                    auto& subTabDef = currentPage.subTabs[i];
                    int32_t spriteIndex = subTabDef.baseImage;
                    int32_t frame = 0;
                    if (subTabDef.animationLength > 1 && _selectedSubTab == i)
                    {
                        frame = frame_no / subTabDef.animationDivisor;
                    }

                    // TODO: generalise this?
                    if (currentPage.Caption == STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS && i == 4)
                        spriteIndex += kThrillRidesTabAnimationSequence[frame];
                    else
                        spriteIndex += frame;

                    auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                    GfxDrawSprite(dpi, ImageId(spriteIndex, colours[1].colour), screenPos);
                }
            }

            // Preview background
            const auto& previewWidget = widgets[WIDX_PREVIEW];
            GfxFillRect(
                dpi,
                { windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ previewWidget.right - 1, previewWidget.bottom - 1 } },
                ColourMapA[colours[1].colour].darkest);

            // Draw number of selected items
            if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
            {
                auto screenPos = windowPos + ScreenCoordsXY{ 3, height - 13 };

                auto numSelected = _numSelectedObjectsForType[EnumValue(GetSelectedObjectType())];
                auto totalSelectable = getObjectEntryGroupCount(GetSelectedObjectType());

                auto ft = Formatter();
                ft.Add<uint16_t>(numSelected);
                ft.Add<uint16_t>(totalSelectable);
                DrawTextBasic(dpi, screenPos, STR_OBJECT_SELECTION_SELECTION_SIZE, ft);
            }

            // Draw sort button text
            const auto& listSortTypeWidget = widgets[WIDX_LIST_SORT_TYPE];
            if (listSortTypeWidget.type != WindowWidgetType::Empty)
            {
                auto ft = Formatter();
                auto stringId = _listSortType == RIDE_SORT_TYPE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                                : kStringIdNone;
                ft.Add<StringId>(stringId);
                auto screenPos = windowPos + ScreenCoordsXY{ listSortTypeWidget.left + 1, listSortTypeWidget.top + 1 };
                DrawTextEllipsised(dpi, screenPos, listSortTypeWidget.width(), STR_OBJECTS_SORT_TYPE, ft, { colours[1] });
            }
            const auto& listSortRideWidget = widgets[WIDX_LIST_SORT_RIDE];
            if (listSortRideWidget.type != WindowWidgetType::Empty)
            {
                auto ft = Formatter();
                auto stringId = _listSortType == RIDE_SORT_RIDE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                                : kStringIdNone;
                ft.Add<StringId>(stringId);
                auto screenPos = windowPos + ScreenCoordsXY{ listSortRideWidget.left + 1, listSortRideWidget.top + 1 };
                DrawTextEllipsised(dpi, screenPos, listSortRideWidget.width(), STR_OBJECTS_SORT_RIDE, ft, { colours[1] });
            }

            if (selected_list_item == -1 || _loadedObject == nullptr)
                return;

            // Draw preview
            {
                DrawPixelInfo clipDPI;
                auto screenPos = windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 };
                int32_t previewWidth = previewWidget.width() - 1;
                int32_t previewHeight = previewWidget.height() - 1;
                if (ClipDrawPixelInfo(clipDPI, dpi, screenPos, previewWidth, previewHeight))
                {
                    _loadedObject->DrawPreview(clipDPI, previewWidth, previewHeight);
                }
            }

            DrawDescriptions(dpi);
            DrawDebugData(dpi);
        }

        void GoToTab(ObjectType objectType)
        {
            for (auto offset = 0u; offset < std::size(ObjectSelectionPages); offset++)
            {
                if (ObjectSelectionPages[offset].mainObjectType == objectType)
                {
                    SetPage(offset);
                    return;
                }
            }
        }

    private:
        bool tabWidgetsInitialised = false;

        void InitWidgets()
        {
            SetWidgets(_window_editor_object_selection_widgets);
            if (!tabWidgetsInitialised)
            {
                tabWidgetsInitialised = true;
                auto tabWidget = widgets[WIDX_TAB_1];
                for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
                {
                    widgets.insert(widgets.end() - 1, tabWidget);
                }
            }
        }

        void SetPage(int32_t _page)
        {
            if (selected_tab == _page)
                return;

            selected_tab = _page;
            _selectedSubTab = 0;
            _filter_flags |= FILTER_RIDES_ALL;
            selected_list_item = -1;
            scrolls[0].contentOffsetY = 0;
            frame_no = 0;

            if (_page == EnumValue(ObjectType::Ride))
            {
                _listSortType = RIDE_SORT_TYPE;
                _listSortDescending = false;
            }
            else
            {
                _listSortType = RIDE_SORT_RIDE;
                _listSortDescending = false;
            }

            VisibleListRefresh();
            Invalidate();
        }

        void VisibleListRefresh()
        {
            int32_t numObjects = static_cast<int32_t>(ObjectRepositoryGetItemsCount());

            VisibleListDispose();
            selected_list_item = -1;

            const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
            for (int32_t i = 0; i < numObjects; i++)
            {
                uint8_t selectionFlags = _objectSelectionFlags[i];
                const ObjectRepositoryItem* item = &items[i];
                if (item->Type == GetSelectedObjectType() && !(selectionFlags & ObjectSelectionFlags::Flag6)
                    && FilterSource(item) && FilterString(*item) && FilterChunks(item) && FilterSelected(selectionFlags)
                    && FilterCompatibilityObject(*item, selectionFlags))
                {
                    auto filter = std::make_unique<RideFilters>();
                    filter->category[0] = 0;
                    filter->category[1] = 0;
                    filter->ride_type = 0;

                    ObjectListItem currentListItem;
                    currentListItem.repositoryItem = item;
                    currentListItem.filter = std::move(filter);
                    currentListItem.flags = &_objectSelectionFlags[i];
                    _listItems.push_back(std::move(currentListItem));
                }
            }

            if (_listItems.empty())
            {
                VisibleListDispose();
            }
            else
            {
                sortFunc_t sortFunc = nullptr;
                switch (_listSortType)
                {
                    case RIDE_SORT_TYPE:
                        sortFunc = VisibleListSortRideType;
                        break;
                    case RIDE_SORT_RIDE:
                        sortFunc = VisibleListSortRideName;
                        break;
                    default:
                        LOG_WARNING("Wrong sort type %d, leaving list as-is.", _listSortType);
                        break;
                }
                if (sortFunc != nullptr)
                {
                    std::sort(_listItems.begin(), _listItems.end(), sortFunc);
                    if (_listSortDescending)
                    {
                        std::reverse(_listItems.begin(), _listItems.end());
                    }
                }
            }
            Invalidate();
        }

        void VisibleListDispose()
        {
            _listItems.clear();
            _listItems.shrink_to_fit();
        }

        void DrawDescriptions(DrawPixelInfo& dpi)
        {
            auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_PREVIEW].midX(), widgets[WIDX_PREVIEW].bottom + 3 };
            auto descriptionWidth = width - widgets[WIDX_LIST].right - 12;

            // Draw name of object
            {
                ObjectListItem* listItem = &_listItems[selected_list_item];

                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(listItem->repositoryItem->Name.c_str());
                screenPos.y += DrawTextWrapped(
                    dpi, screenPos, descriptionWidth, STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE });

                // Leave some space between name and description
                screenPos.y += kListRowHeight;
            }

            screenPos.x = windowPos.x + widgets[WIDX_LIST].right + 6;

            // Compatibility object?
            if (_loadedObject->IsCompatibilityObject())
            {
                screenPos.y += DrawTextWrapped(
                                   dpi, screenPos, descriptionWidth, STR_OBJECT_SELECTION_COMPAT_OBJECT_DESCRIPTION, {},
                                   { COLOUR_BRIGHT_RED })
                    + kListRowHeight;
            }

            auto description = ObjectGetDescription(_loadedObject.get());
            if (!description.empty())
            {
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(description.c_str());

                screenPos.y += DrawTextWrapped(dpi, screenPos, descriptionWidth, STR_WINDOW_COLOUR_2_STRINGID, ft);
                screenPos.y += kListRowHeight;
            }
            if (GetSelectedObjectType() == ObjectType::Ride)
            {
                auto* rideObject = reinterpret_cast<RideObject*>(_loadedObject.get());
                const auto& rideEntry = rideObject->GetEntry();
                if (rideEntry.shop_item[0] != ShopItem::None)
                {
                    std::string sells = "";
                    for (size_t i = 0; i < std::size(rideEntry.shop_item); i++)
                    {
                        if (rideEntry.shop_item[i] == ShopItem::None)
                            continue;

                        if (!sells.empty())
                            sells += ", ";

                        sells += LanguageGetString(GetShopItemDescriptor(rideEntry.shop_item[i]).Naming.Plural);
                    }
                    auto ft = Formatter();
                    ft.Add<const char*>(sells.c_str());
                    screenPos.y += DrawTextWrapped(dpi, screenPos, descriptionWidth, STR_RIDE_OBJECT_SHOP_SELLS, ft) + 2;
                }
            }
            else if (GetSelectedObjectType() == ObjectType::SceneryGroup)
            {
                const auto* sceneryGroupObject = reinterpret_cast<SceneryGroupObject*>(_loadedObject.get());
                auto ft = Formatter();
                ft.Add<uint16_t>(sceneryGroupObject->GetNumIncludedObjects());
                screenPos.y += DrawTextWrapped(dpi, screenPos, descriptionWidth, STR_INCLUDES_X_OBJECTS, ft) + 2;
            }
            else if (GetSelectedObjectType() == ObjectType::Music)
            {
                screenPos.y += DrawTextWrapped(dpi, screenPos, descriptionWidth, STR_MUSIC_OBJECT_TRACK_HEADER) + 2;
                const auto* musicObject = reinterpret_cast<MusicObject*>(_loadedObject.get());
                for (size_t i = 0; i < musicObject->GetTrackCount(); i++)
                {
                    const auto* track = musicObject->GetTrack(i);
                    if (track->Name.empty())
                        continue;

                    auto stringId = track->Composer.empty() ? STR_MUSIC_OBJECT_TRACK_LIST_ITEM
                                                            : STR_MUSIC_OBJECT_TRACK_LIST_ITEM_WITH_COMPOSER;
                    auto ft = Formatter();
                    ft.Add<const char*>(track->Name.c_str());
                    ft.Add<const char*>(track->Composer.c_str());
                    screenPos.y += DrawTextWrapped(dpi, screenPos + ScreenCoordsXY{ 10, 0 }, descriptionWidth, stringId, ft);
                }
            }
        }

        StringId GetAnimationPeepTypeStringId(AnimationPeepType type)
        {
            switch (type)
            {
                case AnimationPeepType::Handyman:
                    return STR_HANDYMAN_PLURAL;
                case AnimationPeepType::Mechanic:
                    return STR_MECHANIC_PLURAL;
                case AnimationPeepType::Security:
                    return STR_SECURITY_GUARD_PLURAL;
                case AnimationPeepType::Entertainer:
                    return STR_ENTERTAINER_PLURAL;
                case AnimationPeepType::Guest:
                default:
                    return STR_GUESTS;
            }
        }

        void DrawDebugData(DrawPixelInfo& dpi)
        {
            ObjectListItem* listItem = &_listItems[selected_list_item];
            auto screenPos = windowPos + ScreenCoordsXY{ width - 5, height - (kListRowHeight * 6) };

            // Draw fallback image warning
            if (_loadedObject && _loadedObject->UsesFallbackImages())
            {
                DrawTextBasic(dpi, screenPos, STR_OBJECT_USES_FALLBACK_IMAGES, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
            }
            screenPos.y += kListRowHeight;

            // Draw ride type.
            if (GetSelectedObjectType() == ObjectType::Ride)
            {
                auto stringId = GetRideTypeStringId(listItem->repositoryItem);
                DrawTextBasic(dpi, screenPos, stringId, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
            }

            // Draw peep animation object type
            if (GetSelectedObjectType() == ObjectType::PeepAnimations)
            {
                auto* animObj = reinterpret_cast<PeepAnimationsObject*>(_loadedObject.get());
                auto stringId = GetAnimationPeepTypeStringId(animObj->GetPeepType());
                DrawTextBasic(dpi, screenPos, stringId, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
            }

            screenPos.y += kListRowHeight;

            // Draw object source
            auto stringId = ObjectManagerGetSourceGameString(listItem->repositoryItem->GetFirstSourceGame());
            DrawTextBasic(dpi, screenPos, stringId, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
            screenPos.y += kListRowHeight;

            // Draw object filename
            {
                auto path = Path::GetFileName(listItem->repositoryItem->Path);
                auto ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const utf8*>(path.c_str());
                DrawTextBasic(
                    dpi, { windowPos.x + this->width - 5, screenPos.y }, STR_WINDOW_COLOUR_2_STRINGID, ft,
                    { COLOUR_BLACK, TextAlignment::RIGHT });
                screenPos.y += kListRowHeight;
            }

            // Draw object author (will be blank space if no author in file or a non JSON object)
            {
                auto ft = Formatter();
                std::string authorsString;
                for (size_t i = 0; i < listItem->repositoryItem->Authors.size(); i++)
                {
                    if (i > 0)
                    {
                        authorsString.append(", ");
                    }
                    authorsString.append(listItem->repositoryItem->Authors[i]);
                }
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(authorsString.c_str());
                DrawTextEllipsised(
                    dpi, { windowPos.x + width - 5, screenPos.y }, width - widgets[WIDX_LIST].right - 4,
                    STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::RIGHT });
            }
        }

        bool FilterSelected(uint8_t objectFlag)
        {
            // Track Manager has no concept of selection filtering, so always return true
            if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
            {
                return true;
            }
            if (isFilterActive(FILTER_SELECTED) == isFilterActive(FILTER_NONSELECTED))
            {
                return true;
            }
            if (isFilterActive(FILTER_SELECTED) && (objectFlag & ObjectSelectionFlags::Selected))
            {
                return true;
            }
            if (isFilterActive(FILTER_NONSELECTED) && !(objectFlag & ObjectSelectionFlags::Selected))
            {
                return true;
            }

            return false;
        }

        bool FilterCompatibilityObject(const ObjectRepositoryItem& item, uint8_t objectFlag)
        {
            // Only show compat objects if they are selected already.
            return !(item.Flags & ObjectItemFlags::IsCompatibilityObject) || (objectFlag & ObjectSelectionFlags::Selected);
        }

        static bool IsFilterInName(const ObjectRepositoryItem& item, std::string_view filter)
        {
            return String::contains(item.Name, filter, true);
        }

        static bool IsFilterInRideType(const ObjectRepositoryItem& item, std::string_view filter)
        {
            if (item.Type == ObjectType::Ride)
            {
                auto rideTypeName = LanguageGetString(GetRideTypeStringId(&item));
                if (String::contains(rideTypeName, filter, true))
                    return true;
            }
            return false;
        }

        static bool IsFilterInFilename(const ObjectRepositoryItem& item, std::string_view filter)
        {
            return String::contains(item.Path, filter, true);
        }

        static bool IsFilterInAuthor(const ObjectRepositoryItem& item, std::string_view filter)
        {
            for (auto& author : item.Authors)
            {
                if (String::contains(author, filter, true))
                {
                    return true;
                }
            }
            return false;
        }

        bool FilterString(const ObjectRepositoryItem& item)
        {
            // Nothing to search for
            std::string_view filter = _filter_string;
            if (filter.empty())
                return true;

            return IsFilterInName(item, filter) || IsFilterInRideType(item, filter) || IsFilterInFilename(item, filter)
                || IsFilterInAuthor(item, filter);
        }

        bool SourcesMatch(ObjectSourceGame source)
        {
            // clang-format off
            return (isFilterActive(FILTER_RCT1) && source == ObjectSourceGame::RCT1) ||
                   (isFilterActive(FILTER_AA)   && source == ObjectSourceGame::AddedAttractions) ||
                   (isFilterActive(FILTER_LL)   && source == ObjectSourceGame::LoopyLandscapes) ||
                   (isFilterActive(FILTER_RCT2) && source == ObjectSourceGame::RCT2) ||
                   (isFilterActive(FILTER_WW)   && source == ObjectSourceGame::WackyWorlds) ||
                   (isFilterActive(FILTER_TT)   && source == ObjectSourceGame::TimeTwister) ||
                   (isFilterActive(FILTER_OO)   && source == ObjectSourceGame::OpenRCT2Official) ||
                   (isFilterActive(FILTER_CUSTOM) &&
                        source != ObjectSourceGame::RCT1 &&
                        source != ObjectSourceGame::AddedAttractions &&
                        source != ObjectSourceGame::LoopyLandscapes &&
                        source != ObjectSourceGame::RCT2 &&
                        source != ObjectSourceGame::WackyWorlds &&
                        source != ObjectSourceGame::TimeTwister &&
                        source != ObjectSourceGame::OpenRCT2Official);
            // clang-format on
        }

        bool FilterSource(const ObjectRepositoryItem* item)
        {
            if (isFilterActive(FILTER_ALL))
                return true;

            for (auto source : item->Sources)
            {
                if (SourcesMatch(source))
                    return true;
            }

            return false;
        }

        bool FilterChunks(const ObjectRepositoryItem* item)
        {
            if (item->Type == ObjectType::Ride)
            {
                ride_type_t rideType = 0;
                for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
                {
                    if (item->RideInfo.RideType[i] != kRideTypeNull)
                    {
                        rideType = item->RideInfo.RideType[i];
                        break;
                    }
                }
                return (_filter_flags & (1 << (GetRideTypeDescriptor(rideType).Category + _numSourceGameItems))) != 0;
            }
            return true;
        }

        void FilterUpdateCounts()
        {
            if (!isFilterActive(FILTER_ALL) || _filter_string[0] != '\0')
            {
                const auto& selectionFlags = _objectSelectionFlags;
                std::fill(std::begin(_filter_object_counts), std::end(_filter_object_counts), 0);

                size_t numObjects = ObjectRepositoryGetItemsCount();
                const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
                for (size_t i = 0; i < numObjects; i++)
                {
                    const ObjectRepositoryItem* item = &items[i];
                    if (FilterSource(item) && FilterString(*item) && FilterChunks(item) && FilterSelected(selectionFlags[i])
                        && FilterCompatibilityObject(*item, selectionFlags[i]))
                    {
                        _filter_object_counts[EnumValue(item->Type)]++;
                    }
                }
            }
        }

        std::string ObjectGetDescription(const Object* object)
        {
            switch (object->GetObjectType())
            {
                case ObjectType::Ride:
                {
                    const RideObject* rideObject = static_cast<const RideObject*>(object);
                    return rideObject->GetDescription();
                }
                default:
                    return "";
            }
        }

        ObjectType GetSelectedObjectType()
        {
            auto& currentPage = ObjectSelectionPages[selected_tab];
            auto& subTabs = currentPage.subTabs;
            if (!subTabs.empty())
                return subTabs[_selectedSubTab].subObjectType;
            else
                return currentPage.mainObjectType;
        }

        /**
         * Takes the y coordinate of the clicked on scroll list
         * and converts this into an object selection.
         * Returns the position in the list.
         * Object_selection_flags, installed_entry also populated
         *
         * rct2: 0x006AA703
         */
        int32_t GetObjectFromObjectSelection(ObjectType object_type, int32_t y)
        {
            int32_t listItemIndex = y / kScrollableRowHeight;
            if (listItemIndex < 0 || static_cast<size_t>(listItemIndex) >= _listItems.size())
                return -1;

            return listItemIndex;
        }

        void SetPressedTab()
        {
            for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
            {
                pressed_widgets &= ~(1ull << (WIDX_TAB_1 + i));
            }
            pressed_widgets |= 1LL << (WIDX_TAB_1 + selected_tab);
        }

        /**
         *
         * rct2: 0x006D33E2
         */
        void ManageTracks()
        {
            SetEveryRideTypeInvented();
            SetEveryRideEntryInvented();

            GetGameState().EditorStep = EditorStep::DesignsManager;

            int32_t entry_index = 0;
            for (; ObjectEntryGetChunk(ObjectType::Ride, entry_index) == nullptr; entry_index++)
                ;

            const auto* rideEntry = GetRideEntryByIndex(entry_index);
            auto rideType = rideEntry->GetFirstNonNullRideType();

            auto intent = Intent(WindowClass::TrackDesignList);
            intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, rideType);
            intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, entry_index);
            ContextOpenIntent(&intent);
        }
    };

    /**
     *
     * rct2: 0x006AA64E
     */
    WindowBase* EditorObjectSelectionOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<EditorObjectSelectionWindow>(
            WindowClass::EditorObjectSelection, 755, 400, WF_10 | WF_RESIZABLE | WF_CENTRE_SCREEN);
    }

    static bool VisibleListSortRideName(const ObjectListItem& a, const ObjectListItem& b)
    {
        auto nameA = a.repositoryItem->Name.c_str();
        auto nameB = b.repositoryItem->Name.c_str();
        return strcmp(nameA, nameB) < 0;
    }

    static bool VisibleListSortRideType(const ObjectListItem& a, const ObjectListItem& b)
    {
        auto rideTypeA = LanguageGetString(GetRideTypeStringId(a.repositoryItem));
        auto rideTypeB = LanguageGetString(GetRideTypeStringId(b.repositoryItem));
        int32_t result = String::compare(rideTypeA, rideTypeB);
        return result != 0 ? result < 0 : VisibleListSortRideName(a, b);
    }

    static StringId GetRideTypeStringId(const ObjectRepositoryItem* item)
    {
        StringId result = kStringIdNone;
        for (int32_t i = 0; i < RCT2::ObjectLimits::kMaxRideTypesPerRideEntry; i++)
        {
            auto rideType = item->RideInfo.RideType[i];
            if (rideType != kRideTypeNull)
            {
                result = GetRideTypeDescriptor(rideType).Naming.Name;
                break;
            }
        }
        return result;
    }

    /**
     *
     * rct2: 0x006ABBBE
     */
    void EditorLoadSelectedObjects()
    {
        auto& objManager = GetContext()->GetObjectManager();
        int32_t numItems = static_cast<int32_t>(ObjectRepositoryGetItemsCount());
        const ObjectRepositoryItem* items = ObjectRepositoryGetItems();
        bool showFallbackWarning = false;
        for (int32_t i = 0; i < numItems; i++)
        {
            if (_objectSelectionFlags[i] & ObjectSelectionFlags::Selected)
            {
                const auto* item = &items[i];
                auto descriptor = ObjectEntryDescriptor(*item);
                const auto* loadedObject = objManager.GetLoadedObject(descriptor);
                if (loadedObject == nullptr)
                {
                    loadedObject = objManager.LoadObject(descriptor);
                    if (loadedObject == nullptr)
                    {
                        LOG_ERROR("Failed to load entry %s", std::string(descriptor.GetName()).c_str());
                    }
                    else if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
                    {
                        // Defaults selected items to researched (if in-game)
                        auto objectType = loadedObject->GetObjectType();
                        auto entryIndex = ObjectManagerGetLoadedObjectEntryIndex(loadedObject);
                        if (objectType == ObjectType::Ride)
                        {
                            const auto* rideEntry = GetRideEntryByIndex(entryIndex);
                            auto rideType = rideEntry->GetFirstNonNullRideType();
                            ResearchCategory category = static_cast<ResearchCategory>(GetRideTypeDescriptor(rideType).Category);
                            ResearchInsertRideEntry(rideType, entryIndex, category, true);
                        }
                        else if (objectType == ObjectType::SceneryGroup)
                        {
                            ResearchInsertSceneryGroupEntry(entryIndex, true);
                        }
                        if (loadedObject->UsesFallbackImages())
                        {
                            showFallbackWarning = true;
                        }
                    }
                }
            }
        }
        if (_numSelectedObjectsForType[EnumValue(ObjectType::Water)] == 0)
        {
            // Reloads the default cyan water palette if no palette was selected.
            LoadPalette();
        }
        if (showFallbackWarning)
            ContextShowError(STR_OBJECT_SELECTION_FALLBACK_IMAGES_WARNING, kStringIdEmpty, Formatter::Common());
    }

    bool EditorObjectSelectionWindowCheck()
    {
        auto* windowMgr = Ui::GetWindowManager();

        auto [missingObjectType, errorString] = Editor::CheckObjectSelection();
        if (missingObjectType == ObjectType::None)
        {
            windowMgr->CloseByClass(WindowClass::EditorObjectSelection);
            return true;
        }

        ContextShowError(STR_INVALID_SELECTION_OF_OBJECTS, errorString, {});

        WindowBase* w = windowMgr->FindByClass(WindowClass::EditorObjectSelection);
        if (w != nullptr)
        {
            // Click tab with missing object
            static_cast<EditorObjectSelectionWindow*>(w)->GoToTab(missingObjectType);
        }
        return false;
    }
} // namespace OpenRCT2::Ui::Windows
