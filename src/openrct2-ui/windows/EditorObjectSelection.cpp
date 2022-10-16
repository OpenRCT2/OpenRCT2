/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <cctype>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Editor.h>
#include <openrct2/EditorObjectSelectionSession.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/LoadOrQuitAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/MusicObject.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/object/RideObject.h>
#include <openrct2/object/SceneryGroupObject.h>
#include <openrct2/platform/Platform.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/title/TitleScreen.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <string>
#include <vector>

enum
{
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

    FILTER_RIDES = FILTER_RIDE_TRANSPORT | FILTER_RIDE_GENTLE | FILTER_RIDE_COASTER | FILTER_RIDE_THRILL | FILTER_RIDE_WATER
        | FILTER_RIDE_STALL,
    FILTER_ALL = FILTER_RIDES | FILTER_RCT1 | FILTER_AA | FILTER_LL | FILTER_RCT2 | FILTER_WW | FILTER_TT | FILTER_OO
        | FILTER_CUSTOM | FILTER_SELECTED | FILTER_NONSELECTED,
};

enum
{
    RIDE_SORT_TYPE,
    RIDE_SORT_RIDE
};

enum
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
    std::unique_ptr<rct_object_filters> filter;
    uint8_t* flags;
};

static constexpr uint8_t _numSourceGameItems = 8;

static uint32_t _filter_flags;
static uint16_t _filter_object_counts[EnumValue(ObjectType::Count)];

static char _filter_string[MAX_PATH];

#define _FILTER_ALL ((_filter_flags & FILTER_ALL) == FILTER_ALL)
#define _FILTER_RCT1 (_filter_flags & FILTER_RCT1)
#define _FILTER_AA (_filter_flags & FILTER_AA)
#define _FILTER_LL (_filter_flags & FILTER_LL)
#define _FILTER_RCT2 (_filter_flags & FILTER_RCT2)
#define _FILTER_WW (_filter_flags & FILTER_WW)
#define _FILTER_TT (_filter_flags & FILTER_TT)
#define _FILTER_OO (_filter_flags & FILTER_OO)
#define _FILTER_CUSTOM (_filter_flags & FILTER_CUSTOM)
#define _FILTER_SELECTED (_filter_flags & FILTER_SELECTED)
#define _FILTER_NONSELECTED (_filter_flags & FILTER_NONSELECTED)

static constexpr const StringId WINDOW_TITLE = STR_OBJECT_SELECTION;
static constexpr const int32_t WH = 400;
static constexpr const int32_t WW = 755;

struct ObjectPageDesc
{
    StringId Caption;
    uint32_t Image;
    bool IsAdvanced;
};

// clang-format off
// Order of which the object tabs are displayed.
static constexpr const ObjectPageDesc ObjectSelectionPages[] = {
    { STR_OBJECT_SELECTION_RIDE_VEHICLES_ATTRACTIONS, SPR_TAB_RIDE_16,            false },
    { STR_OBJECT_SELECTION_STATIONS,                  SPR_G2_RIDE_STATION_TAB,    true  },
    { STR_OBJECT_SELECTION_MUSIC,                     SPR_TAB_MUSIC_0,            true  },
    { STR_OBJECT_SELECTION_SCENERY_GROUPS,            SPR_TAB_SCENERY_STATUES,    false },
    { STR_OBJECT_SELECTION_SMALL_SCENERY,             SPR_TAB_SCENERY_TREES,      true  },
    { STR_OBJECT_SELECTION_LARGE_SCENERY,             SPR_TAB_SCENERY_URBAN,      true  },
    { STR_OBJECT_SELECTION_WALLS_FENCES,              SPR_TAB_SCENERY_WALLS,      true  },
    { STR_OBJECT_SELECTION_FOOTPATH_SURFACES,         SPR_G2_PATH_SURFACE_TAB,    false },
    { STR_OBJECT_SELECTION_FOOTPATH_RAILINGS,         SPR_G2_PATH_RAILINGS_TAB,   false },
    { STR_OBJECT_SELECTION_FOOTPATHS,                 SPR_G2_LEGACY_PATH_TAB,     true  },
    { STR_OBJECT_SELECTION_PATH_EXTRAS,               SPR_TAB_SCENERY_PATH_ITEMS, false },
    { STR_OBJECT_SELECTION_PATH_SIGNS,                SPR_TAB_SCENERY_SIGNAGE,    true  },
    { STR_OBJECT_SELECTION_PARK_ENTRANCE,             SPR_TAB_PARK,               false },
    { STR_OBJECT_SELECTION_TERRAIN_SURFACES,          SPR_G2_TAB_LAND,            true  },
    { STR_OBJECT_SELECTION_TERRAIN_EDGES,             SPR_G2_TERRAIN_EDGE_TAB,    true  },
    { STR_OBJECT_SELECTION_WATER,                     SPR_TAB_WATER,              false },
};
// clang-format on

// Order of which the contents of each tab is displayed.
ObjectType static TabOrder[] = {
    ObjectType::Ride,         ObjectType::Station,         ObjectType::Music,
    ObjectType::SceneryGroup, ObjectType::SmallScenery,    ObjectType::LargeScenery,
    ObjectType::Walls,        ObjectType::FootpathSurface, ObjectType::FootpathRailings,
    ObjectType::Paths,        ObjectType::PathBits,        ObjectType::Banners,
    ObjectType::ParkEntrance, ObjectType::TerrainSurface,  ObjectType::TerrainEdge,
    ObjectType::Water,
};

#pragma region Widgets

enum WINDOW_EDITOR_OBJECT_SELECTION_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_TAB_CONTENT_PANEL,
    WIDX_ADVANCED,
    WIDX_LIST,
    WIDX_PREVIEW,
    WIDX_INSTALL_TRACK,
    WIDX_FILTER_DROPDOWN,
    WIDX_FILTER_TEXT_BOX,
    WIDX_FILTER_CLEAR_BUTTON,
    WIDX_FILTER_RIDE_TAB_FRAME,
    WIDX_FILTER_RIDE_TAB_ALL,
    WIDX_FILTER_RIDE_TAB_TRANSPORT,
    WIDX_FILTER_RIDE_TAB_GENTLE,
    WIDX_FILTER_RIDE_TAB_COASTER,
    WIDX_FILTER_RIDE_TAB_THRILL,
    WIDX_FILTER_RIDE_TAB_WATER,
    WIDX_FILTER_RIDE_TAB_STALL,
    WIDX_LIST_SORT_TYPE,
    WIDX_LIST_SORT_RIDE,
    WIDX_TAB_1,
};

validate_global_widx(WC_EDITOR_OBJECT_SELECTION, WIDX_TAB_1);

static bool _window_editor_object_selection_widgets_initialised;

// clang-format off
static std::vector<rct_widget> _window_editor_object_selection_widgets = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {WW,  357}, WindowWidgetType::Resize,       WindowColour::Secondary                                                                  ),
    MakeWidget({470, 22}, {122,  14}, WindowWidgetType::Button,       WindowColour::Primary,   STR_OBJECT_SELECTION_ADVANCED, STR_OBJECT_SELECTION_ADVANCED_TIP),
    MakeWidget({  4, 60}, {288, 327}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL                                                 ),
    MakeWidget({391, 45}, {114, 114}, WindowWidgetType::FlatBtn,      WindowColour::Secondary                                                                  ),
    MakeWidget({470, 22}, {122,  14}, WindowWidgetType::Button,       WindowColour::Primary,   STR_INSTALL_NEW_TRACK_DESIGN,  STR_INSTALL_NEW_TRACK_DESIGN_TIP ),
    MakeWidget({350, 22}, {114,  14}, WindowWidgetType::Button,       WindowColour::Primary,   STR_OBJECT_FILTER,             STR_OBJECT_FILTER_TIP            ),
    MakeWidget({  4, 45}, {211,  14}, WindowWidgetType::TextBox,     WindowColour::Secondary                                                                  ),
    MakeWidget({218, 45}, { 70,  14}, WindowWidgetType::Button,       WindowColour::Secondary, STR_OBJECT_SEARCH_CLEAR                                         ),
    MakeWidget({  3, 73}, {285,   4}, WindowWidgetType::ImgBtn,       WindowColour::Secondary                                                                  ),
    MakeTab   ({  3, 47},                                                                                       STR_OBJECT_FILTER_ALL_RIDES_TIP  ),
    MakeTab   ({ 34, 47},                                                                                       STR_TRANSPORT_RIDES_TIP          ),
    MakeTab   ({ 65, 47},                                                                                       STR_GENTLE_RIDES_TIP             ),
    MakeTab   ({ 96, 47},                                                                                       STR_ROLLER_COASTERS_TIP          ),
    MakeTab   ({127, 47},                                                                                       STR_THRILL_RIDES_TIP             ),
    MakeTab   ({158, 47},                                                                                       STR_WATER_RIDES_TIP              ),
    MakeTab   ({189, 47},                                                                                       STR_SHOPS_STALLS_TIP             ),
    MakeWidget({  4, 80}, {145,  14}, WindowWidgetType::TableHeader, WindowColour::Secondary                                                                  ),
    MakeWidget({149, 80}, {143,  14}, WindowWidgetType::TableHeader, WindowColour::Secondary                                                                  ),

    MakeTab   ({  3, 17},                                                                                       STR_STRING_DEFINED_TOOLTIP       ),
    // Copied object type times...

    WIDGETS_END,
};
// clang-format on

#pragma endregion

static constexpr const int32_t window_editor_object_selection_animation_loops[] = {
    20, // All
    32, // Transport
    10, // Gentle
    72, // Coaster
    24, // Thrill
    28, // Water
    16, // Stall
};
static constexpr const int32_t window_editor_object_selection_animation_divisor[] = {
    4, // All
    8, // Transport
    2, // Gentle
    4, // Coaster
    4, // Thrill
    4, // Water
    2, // Stall
};

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

public:
    /**
     *
     * rct2: 0x006AA64E
     */
    void OnOpen() override
    {
        InitWidgets();

        sub_6AB211();
        reset_selected_object_count_and_size();

        widgets[WIDX_FILTER_TEXT_BOX].string = _filter_string;

        _filter_flags = gConfigInterface.ObjectSelectionFilterFlags;
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

        VisibleListRefresh();
    }

    /**
     *
     * rct2: 0x006AB199
     */
    void OnClose() override
    {
        unload_unselected_objects();
        EditorLoadSelectedObjects();
        editor_object_flags_free();

        if (_loadedObject != nullptr)
            _loadedObject->Unload();

        if (gScreenFlags & SCREEN_FLAGS_EDITOR)
        {
            research_populate_list_random();
        }
        else
        {
            // Used for in-game object selection cheat
            // This resets the ride selection list and resets research to 0 on current item
            gSilentResearch = true;
            research_reset_current_item();
            gSilentResearch = false;
        }

        auto intent = Intent(INTENT_ACTION_REFRESH_NEW_RIDES);
        context_broadcast_intent(&intent);

        VisibleListDispose();

        intent = Intent(INTENT_ACTION_REFRESH_SCENERY);
        context_broadcast_intent(&intent);
    }

    void OnUpdate() override
    {
        if (gCurrentTextBox.window.classification == classification && gCurrentTextBox.window.number == number)
        {
            window_update_textbox_caret();
            widget_invalidate(*this, WIDX_FILTER_TEXT_BOX);
        }

        for (WidgetIndex i = WIDX_FILTER_RIDE_TAB_TRANSPORT; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
        {
            if (!IsWidgetPressed(i))
                continue;

            frame_no++;
            if (frame_no >= window_editor_object_selection_animation_loops[i - WIDX_FILTER_RIDE_TAB_TRANSPORT])
                frame_no = 0;

            widget_invalidate(*this, i);
            break;
        }
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
                window_close(*this);
                if (gScreenFlags & SCREEN_FLAGS_EDITOR)
                {
                    finish_object_selection();
                }
                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    game_notify_map_change();
                    game_unload_scripts();
                    title_load();
                }
                break;
            case WIDX_FILTER_RIDE_TAB_ALL:
                _filter_flags |= FILTER_RIDES;
                gConfigInterface.ObjectSelectionFilterFlags = _filter_flags;
                ConfigSaveDefault();

                FilterUpdateCounts();
                VisibleListRefresh();

                selected_list_item = -1;
                scrolls[0].v_top = 0;
                Invalidate();
                break;
            case WIDX_FILTER_RIDE_TAB_TRANSPORT:
            case WIDX_FILTER_RIDE_TAB_GENTLE:
            case WIDX_FILTER_RIDE_TAB_COASTER:
            case WIDX_FILTER_RIDE_TAB_THRILL:
            case WIDX_FILTER_RIDE_TAB_WATER:
            case WIDX_FILTER_RIDE_TAB_STALL:
                _filter_flags &= ~FILTER_RIDES;
                _filter_flags |= (1 << (widgetIndex - WIDX_FILTER_RIDE_TAB_TRANSPORT + _numSourceGameItems));
                gConfigInterface.ObjectSelectionFilterFlags = _filter_flags;
                ConfigSaveDefault();

                FilterUpdateCounts();
                VisibleListRefresh();

                selected_list_item = -1;
                scrolls[0].v_top = 0;
                frame_no = 0;
                Invalidate();
                break;

            case WIDX_ADVANCED:
                list_information_type ^= 1;
                Invalidate();
                break;

            case WIDX_INSTALL_TRACK:
            {
                if (selected_list_item != -1)
                {
                    selected_list_item = -1;
                }
                Invalidate();

                auto intent = Intent(WindowClass::Loadsave);
                intent.putExtra(INTENT_EXTRA_LOADSAVE_TYPE, LOADSAVETYPE_LOAD | LOADSAVETYPE_TRACK);
                context_open_intent(&intent);
                break;
            }
            case WIDX_FILTER_TEXT_BOX:
                window_start_textbox(*this, widgetIndex, STR_STRING, _filter_string, sizeof(_filter_string));
                break;
            case WIDX_FILTER_CLEAR_BUTTON:
                std::fill_n(_filter_string, sizeof(_filter_string), 0x00);
                FilterUpdateCounts();
                scrolls->v_top = 0;
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
        window_set_resize(*this, WW, WH, 1200, 1000);
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        int32_t numSelectionItems = 0;

        switch (widgetIndex)
        {
            case WIDX_FILTER_DROPDOWN:

                gDropdownItems[DDIX_FILTER_RCT1].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_AA].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_LL].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_RCT2].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_WW].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_TT].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_OO].Format = STR_TOGGLE_OPTION;
                gDropdownItems[DDIX_FILTER_CUSTOM].Format = STR_TOGGLE_OPTION;

                gDropdownItems[DDIX_FILTER_RCT1].Args = STR_SCENARIO_CATEGORY_RCT1;
                gDropdownItems[DDIX_FILTER_AA].Args = STR_SCENARIO_CATEGORY_RCT1_AA;
                gDropdownItems[DDIX_FILTER_LL].Args = STR_SCENARIO_CATEGORY_RCT1_LL;
                gDropdownItems[DDIX_FILTER_RCT2].Args = STR_ROLLERCOASTER_TYCOON_2_DROPDOWN;
                gDropdownItems[DDIX_FILTER_WW].Args = STR_OBJECT_FILTER_WW;
                gDropdownItems[DDIX_FILTER_TT].Args = STR_OBJECT_FILTER_TT;
                gDropdownItems[DDIX_FILTER_OO].Args = STR_OBJECT_FILTER_OPENRCT2_OFFICIAL;
                gDropdownItems[DDIX_FILTER_CUSTOM].Args = STR_OBJECT_FILTER_CUSTOM;

                // Track manager cannot select multiple, so only show selection filters if not in track manager
                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                {
                    numSelectionItems = 3;
                    gDropdownItems[DDIX_FILTER_SEPARATOR].Format = 0;
                    gDropdownItems[DDIX_FILTER_SELECTED].Format = STR_TOGGLE_OPTION;
                    gDropdownItems[DDIX_FILTER_NONSELECTED].Format = STR_TOGGLE_OPTION;
                    gDropdownItems[DDIX_FILTER_SEPARATOR].Args = STR_NONE;
                    gDropdownItems[DDIX_FILTER_SELECTED].Args = STR_SELECTED_ONLY;
                    gDropdownItems[DDIX_FILTER_NONSELECTED].Args = STR_NON_SELECTED_ONLY;
                }

                WindowDropdownShowText(
                    { windowPos.x + widgets[widgetIndex].left, windowPos.y + widgets[widgetIndex].top },
                    widgets[widgetIndex].height() + 1, colours[widgets[widgetIndex].colour], Dropdown::Flag::StayOpen,
                    _numSourceGameItems + numSelectionItems);

                for (int32_t i = 0; i < _numSourceGameItems; i++)
                {
                    if (_filter_flags & (1 << i))
                    {
                        Dropdown::SetChecked(i, true);
                    }
                }

                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
                {
                    Dropdown::SetChecked(DDIX_FILTER_SELECTED, _FILTER_SELECTED != 0);
                    Dropdown::SetChecked(DDIX_FILTER_NONSELECTED, _FILTER_NONSELECTED != 0);
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
            case WIDX_FILTER_DROPDOWN:
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
                gConfigInterface.ObjectSelectionFilterFlags = _filter_flags;
                ConfigSaveDefault();

                FilterUpdateCounts();
                scrolls->v_top = 0;

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
        const auto newHeight = static_cast<int32_t>(_listItems.size() * SCROLLABLE_ROW_HEIGHT);
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
        window_close_all_except_class(WindowClass::EditorObjectSelection);

        int32_t selected_object = GetObjectFromObjectSelection(GetSelectedObjectType(), screenCoords.y);
        if (selected_object == -1)
            return;

        ObjectListItem* listItem = &_listItems[selected_object];
        uint8_t object_selection_flags = *listItem->flags;
        if (object_selection_flags & ObjectSelectionFlags::Flag6)
            return;

        Invalidate();

        const CursorState* state = context_get_cursor_state();
        OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::Click1, 0, state->position.x);

        if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
        {
            const auto objectSelectResult = window_editor_object_selection_select_object(
                0, INPUT_FLAG_EDITOR_OBJECT_SELECT, listItem->repositoryItem);
            if (!objectSelectResult.Successful)
                return;

            // Close any other open windows such as options/colour schemes to prevent a crash.
            window_close_all();
            // window_close(*w);

            // This function calls window_track_list_open
            ManageTracks();
            return;
        }

        uint32_t inputFlags = INPUT_FLAG_EDITOR_OBJECT_1 | INPUT_FLAG_EDITOR_OBJECT_SELECT_OBJECTS_IN_SCENERY_GROUP;
        // If already selected
        if (!(object_selection_flags & ObjectSelectionFlags::Selected))
            inputFlags |= INPUT_FLAG_EDITOR_OBJECT_SELECT;

        _gSceneryGroupPartialSelectError = std::nullopt;
        const auto objectSelectResult = window_editor_object_selection_select_object(0, inputFlags, listItem->repositoryItem);
        if (!objectSelectResult.Successful)
        {
            StringId error_title = (inputFlags & INPUT_FLAG_EDITOR_OBJECT_SELECT) ? STR_UNABLE_TO_SELECT_THIS_OBJECT
                                                                                  : STR_UNABLE_TO_DE_SELECT_THIS_OBJECT;

            context_show_error(error_title, objectSelectResult.Message, {});
            return;
        }

        if (_FILTER_SELECTED || _FILTER_NONSELECTED)
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
                context_show_error(
                    STR_WARNING_TOO_MANY_OBJECTS_SELECTED, STR_NOT_ALL_OBJECTS_IN_THIS_SCENERY_GROUP_COULD_BE_SELECTED, {});
            }
            else
            {
                context_show_error(
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
                auto& objRepository = OpenRCT2::GetContext()->GetObjectRepository();
                _loadedObject = objRepository.LoadObject(listItem->repositoryItem);
                if (_loadedObject != nullptr)
                {
                    _loadedObject->Load();
                }
            }

            Invalidate();
        }
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        // ScrollPaint
        ScreenCoordsXY screenCoords;
        bool ridePage = (GetSelectedObjectType() == ObjectType::Ride);

        uint8_t paletteIndex = ColourMapA[colours[1]].mid_light;
        gfx_clear(&dpi, paletteIndex);

        screenCoords.y = 0;
        for (size_t i = 0; i < _listItems.size(); i++)
        {
            const auto& listItem = _listItems[i];
            if (screenCoords.y + SCROLLABLE_ROW_HEIGHT >= dpi.y && screenCoords.y <= dpi.y + dpi.height)
            {
                // Draw checkbox
                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && !(*listItem.flags & 0x20))
                    gfx_fill_rect_inset(
                        &dpi, { { 2, screenCoords.y }, { 11, screenCoords.y + 10 } }, colours[1], INSET_RECT_F_E0);

                // Highlight background
                auto highlighted = i == static_cast<size_t>(selected_list_item)
                    && !(*listItem.flags & ObjectSelectionFlags::Flag6);
                if (highlighted)
                {
                    auto bottom = screenCoords.y + (SCROLLABLE_ROW_HEIGHT - 1);
                    gfx_filter_rect(&dpi, { 0, screenCoords.y, width, bottom }, FilterPaletteID::PaletteDarken1);
                }

                // Draw checkmark
                if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER) && (*listItem.flags & ObjectSelectionFlags::Selected))
                {
                    screenCoords.x = 2;
                    FontSpriteBase fontSpriteBase = highlighted ? FontSpriteBase::MEDIUM_EXTRA_DARK
                                                                : FontSpriteBase::MEDIUM_DARK;
                    colour_t colour2 = NOT_TRANSLUCENT(colours[1]);
                    if (*listItem.flags & (ObjectSelectionFlags::InUse | ObjectSelectionFlags::AlwaysRequired))
                        colour2 |= COLOUR_FLAG_INSET;

                    gfx_draw_string(
                        &dpi, screenCoords, static_cast<const char*>(CheckBoxMarkString),
                        { static_cast<colour_t>(colour2), fontSpriteBase });
                }

                screenCoords.x = gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER ? 0 : 15;

                auto bufferWithColour = strcpy(gCommonStringFormatBuffer, highlighted ? "{WINDOW_COLOUR_2}" : "{BLACK}");
                auto buffer = strchr(bufferWithColour, '\0');

                colour_t colour = COLOUR_BLACK;
                FontSpriteBase fontSpriteBase = FontSpriteBase::MEDIUM;
                if (*listItem.flags & ObjectSelectionFlags::Flag6)
                {
                    colour = colours[1] & 0x7F;
                    fontSpriteBase = FontSpriteBase::MEDIUM_DARK;
                }

                int32_t width_limit = widgets[WIDX_LIST].width() - screenCoords.x;

                if (ridePage)
                {
                    width_limit /= 2;
                    // Draw ride type
                    StringId rideTypeStringId = GetRideTypeStringId(listItem.repositoryItem);
                    safe_strcpy(buffer, language_get_string(rideTypeStringId), 256 - (buffer - bufferWithColour));
                    auto ft = Formatter();
                    ft.Add<const char*>(gCommonStringFormatBuffer);
                    DrawTextEllipsised(&dpi, screenCoords, width_limit - 15, STR_STRING, ft, { colour, fontSpriteBase });
                    screenCoords.x = widgets[WIDX_LIST_SORT_RIDE].left - widgets[WIDX_LIST].left;
                }

                // Draw text
                safe_strcpy(buffer, listItem.repositoryItem->Name.c_str(), 256 - (buffer - bufferWithColour));
                if (gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER)
                {
                    while (*buffer != 0 && *buffer != 9)
                        buffer++;

                    *buffer = 0;
                }
                auto ft = Formatter();
                ft.Add<const char*>(gCommonStringFormatBuffer);
                DrawTextEllipsised(&dpi, screenCoords, width_limit, STR_STRING, ft, { colour, fontSpriteBase });
            }
            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
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
        if (widgetIndex != WIDX_FILTER_TEXT_BOX || text.empty())
            return;

        std::string tempText = text.data();
        const char* c = tempText.c_str();

        if (strcmp(_filter_string, c) == 0)
            return;

        safe_strcpy(_filter_string, c, sizeof(_filter_string));

        FilterUpdateCounts();

        scrolls->v_top = 0;

        VisibleListRefresh();
        Invalidate();
    }

    void OnPrepareDraw() override
    {
        // Resize widgets
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
        widgets[WIDX_TAB_CONTENT_PANEL].right = width - 1;
        widgets[WIDX_TAB_CONTENT_PANEL].bottom = height - 1;
        widgets[WIDX_ADVANCED].left = width - 130;
        widgets[WIDX_ADVANCED].right = width - 9;
        widgets[WIDX_LIST].right = width - 309;
        widgets[WIDX_LIST].bottom = height - 14;
        widgets[WIDX_PREVIEW].left = width - 209;
        widgets[WIDX_PREVIEW].right = width - 96;
        widgets[WIDX_INSTALL_TRACK].left = width - 130;
        widgets[WIDX_INSTALL_TRACK].right = width - 9;
        widgets[WIDX_FILTER_DROPDOWN].left = width - 250;
        widgets[WIDX_FILTER_DROPDOWN].right = width - 137;

        // Set pressed widgets
        pressed_widgets |= 1ULL << WIDX_PREVIEW;
        SetPressedTab();
        if (list_information_type & 1)
            pressed_widgets |= (1ULL << WIDX_ADVANCED);
        else
            pressed_widgets &= ~(1ULL << WIDX_ADVANCED);

        // Set window title and buttons
        auto ft = Formatter::Common();
        ft.Add<StringId>(ObjectSelectionPages[selected_tab].Caption);
        auto& titleWidget = widgets[WIDX_TITLE];
        auto& installTrackWidget = widgets[WIDX_INSTALL_TRACK];
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

        // Align tabs, hide advanced ones
        bool advancedMode = (list_information_type & 1) != 0;
        int32_t x = 3;
        for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
        {
            auto& widget = widgets[WIDX_TAB_1 + i];
            if ((!advancedMode && ObjectSelectionPages[i].IsAdvanced)
                || ObjectSelectionPages[i].Image == static_cast<uint32_t>(SPR_NONE))
            {
                widget.type = WindowWidgetType::Empty;
            }
            else
            {
                widget.type = WindowWidgetType::Tab;
                widget.left = x;
                widget.right = x + 30;
                x += 31;
            }
        }

        if (gScreenFlags & (SCREEN_FLAGS_TRACK_MANAGER | SCREEN_FLAGS_TRACK_DESIGNER))
        {
            widgets[WIDX_ADVANCED].type = WindowWidgetType::Empty;
            for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
            {
                widgets[WIDX_TAB_1 + i].type = WindowWidgetType::Empty;
            }
            x = 150;
        }
        else
        {
            widgets[WIDX_ADVANCED].type = WindowWidgetType::Button;
            x = 300;
        }

        widgets[WIDX_FILTER_DROPDOWN].type = WindowWidgetType::Button;
        widgets[WIDX_LIST].right = width - (WW - 587) - x;
        widgets[WIDX_PREVIEW].left = width - (WW - 537) - (x / 2);
        widgets[WIDX_PREVIEW].right = widgets[WIDX_PREVIEW].left + 113;
        widgets[WIDX_FILTER_RIDE_TAB_FRAME].right = widgets[WIDX_LIST].right;

        bool ridePage = (GetSelectedObjectType() == ObjectType::Ride);
        widgets[WIDX_LIST].top = (ridePage ? 118 : 60);
        widgets[WIDX_FILTER_TEXT_BOX].right = widgets[WIDX_LIST].right - 77;
        widgets[WIDX_FILTER_TEXT_BOX].top = (ridePage ? 79 : 45);
        widgets[WIDX_FILTER_TEXT_BOX].bottom = (ridePage ? 92 : 58);
        widgets[WIDX_FILTER_CLEAR_BUTTON].left = widgets[WIDX_LIST].right - 73;
        widgets[WIDX_FILTER_CLEAR_BUTTON].right = widgets[WIDX_LIST].right;
        widgets[WIDX_FILTER_CLEAR_BUTTON].top = (ridePage ? 79 : 45);
        widgets[WIDX_FILTER_CLEAR_BUTTON].bottom = (ridePage ? 92 : 58);

        if (ridePage)
        {
            for (int32_t i = 0; i < 7; i++)
                pressed_widgets &= ~(1 << (WIDX_FILTER_RIDE_TAB_ALL + i));

            if ((_filter_flags & FILTER_RIDES) == FILTER_RIDES)
                pressed_widgets |= (1ULL << WIDX_FILTER_RIDE_TAB_ALL);
            else
            {
                for (int32_t i = 0; i < 6; i++)
                {
                    if (_filter_flags & (1 << (_numSourceGameItems + i)))
                        pressed_widgets |= 1ULL << (WIDX_FILTER_RIDE_TAB_TRANSPORT + i);
                }
            }

            widgets[WIDX_FILTER_RIDE_TAB_FRAME].type = WindowWidgetType::ImgBtn;
            for (int32_t i = WIDX_FILTER_RIDE_TAB_ALL; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
                widgets[i].type = WindowWidgetType::Tab;

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
            for (int32_t i = WIDX_FILTER_RIDE_TAB_FRAME; i <= WIDX_FILTER_RIDE_TAB_STALL; i++)
                widgets[i].type = WindowWidgetType::Empty;

            widgets[WIDX_LIST_SORT_TYPE].type = WindowWidgetType::Empty;
            widgets[WIDX_LIST_SORT_RIDE].type = WindowWidgetType::Empty;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        int32_t _width;

        DrawWidgets(dpi);

        // Draw tabs
        for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
        {
            const auto& widget = widgets[WIDX_TAB_1 + i];
            if (widget.type != WindowWidgetType::Empty)
            {
                auto image = ImageId(ObjectSelectionPages[i].Image);
                auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                gfx_draw_sprite(&dpi, image, screenPos);
            }
        }

        const int32_t ride_tabs[] = {
            SPR_TAB_RIDE_16,        SPR_TAB_RIDES_TRANSPORT_0, SPR_TAB_RIDES_GENTLE_0, SPR_TAB_RIDES_ROLLER_COASTERS_0,
            SPR_TAB_RIDES_THRILL_0, SPR_TAB_RIDES_WATER_0,     SPR_TAB_RIDES_SHOP_0,   SPR_TAB_FINANCES_RESEARCH_0,
        };
        const int32_t ThrillRidesTabAnimationSequence[] = {
            5, 6, 5, 4, 3, 2, 1, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 0, 0, 0,
        };

        // Draw ride tabs
        if (GetSelectedObjectType() == ObjectType::Ride)
        {
            for (int32_t i = 0; i < 7; i++)
            {
                const auto& widget = widgets[WIDX_FILTER_RIDE_TAB_ALL + i];
                if (widget.type == WindowWidgetType::Empty)
                    continue;

                int32_t spriteIndex = ride_tabs[i];
                int32_t frame = 0;
                if (i != 0 && IsWidgetPressed(WIDX_FILTER_RIDE_TAB_ALL + i))
                {
                    frame = frame_no / window_editor_object_selection_animation_divisor[i - 1];
                }
                spriteIndex += (i == 4 ? ThrillRidesTabAnimationSequence[frame] : frame);

                auto screenPos = windowPos + ScreenCoordsXY{ widget.left, widget.top };
                gfx_draw_sprite(&dpi, ImageId(spriteIndex, colours[1]), screenPos);
            }
        }

        // Preview background
        const auto& previewWidget = widgets[WIDX_PREVIEW];
        gfx_fill_rect(
            &dpi,
            { windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 },
              windowPos + ScreenCoordsXY{ previewWidget.right - 1, previewWidget.bottom - 1 } },
            ColourMapA[colours[1]].darkest);

        // Draw number of selected items
        if (!(gScreenFlags & SCREEN_FLAGS_TRACK_MANAGER))
        {
            auto screenPos = windowPos + ScreenCoordsXY{ 3, height - 13 };

            int32_t numSelected = _numSelectedObjectsForType[EnumValue(GetSelectedObjectType())];
            int32_t totalSelectable = object_entry_group_counts[EnumValue(GetSelectedObjectType())];

            auto ft = Formatter();
            ft.Add<uint16_t>(numSelected);
            ft.Add<uint16_t>(totalSelectable);
            DrawTextBasic(&dpi, screenPos, STR_OBJECT_SELECTION_SELECTION_SIZE, ft);
        }

        // Draw sort button text
        const auto& listSortTypeWidget = widgets[WIDX_LIST_SORT_TYPE];
        if (listSortTypeWidget.type != WindowWidgetType::Empty)
        {
            auto ft = Formatter();
            auto stringId = _listSortType == RIDE_SORT_TYPE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                            : STR_NONE;
            ft.Add<StringId>(stringId);
            auto screenPos = windowPos + ScreenCoordsXY{ listSortTypeWidget.left + 1, listSortTypeWidget.top + 1 };
            DrawTextEllipsised(&dpi, screenPos, listSortTypeWidget.width(), STR_OBJECTS_SORT_TYPE, ft, { colours[1] });
        }
        const auto& listSortRideWidget = widgets[WIDX_LIST_SORT_RIDE];
        if (listSortRideWidget.type != WindowWidgetType::Empty)
        {
            auto ft = Formatter();
            auto stringId = _listSortType == RIDE_SORT_RIDE ? static_cast<StringId>(_listSortDescending ? STR_DOWN : STR_UP)
                                                            : STR_NONE;
            ft.Add<StringId>(stringId);
            auto screenPos = windowPos + ScreenCoordsXY{ listSortRideWidget.left + 1, listSortRideWidget.top + 1 };
            DrawTextEllipsised(&dpi, screenPos, listSortRideWidget.width(), STR_OBJECTS_SORT_RIDE, ft, { colours[1] });
        }

        if (selected_list_item == -1 || _loadedObject == nullptr)
            return;

        ObjectListItem* listItem = &_listItems[selected_list_item];

        // Draw preview
        {
            rct_drawpixelinfo clipDPI;
            auto screenPos = windowPos + ScreenCoordsXY{ previewWidget.left + 1, previewWidget.top + 1 };
            _width = previewWidget.width() - 1;
            int32_t _height = previewWidget.height() - 1;
            if (clip_drawpixelinfo(&clipDPI, &dpi, screenPos, _width, _height))
            {
                _loadedObject->DrawPreview(&clipDPI, _width, _height);
            }
        }

        // Draw name of object
        {
            auto screenPos = windowPos + ScreenCoordsXY{ previewWidget.midX() + 1, previewWidget.bottom + 3 };
            _width = this->width - widgets[WIDX_LIST].right - 6;
            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(listItem->repositoryItem->Name.c_str());
            DrawTextEllipsised(&dpi, screenPos, _width, STR_WINDOW_COLOUR_2_STRINGID, ft, { TextAlignment::CENTRE });
        }

        DrawDescriptions(&dpi);
        DrawDebugData(&dpi);
    }

private:
    void InitWidgets()
    {
        auto& targetWidgets = _window_editor_object_selection_widgets;
        if (!_window_editor_object_selection_widgets_initialised)
        {
            _window_editor_object_selection_widgets_initialised = true;
            auto tabWidget = targetWidgets[targetWidgets.size() - 2];
            for (size_t i = 1; i < std::size(ObjectSelectionPages); i++)
            {
                targetWidgets.insert(targetWidgets.end() - 1, tabWidget);
            }
        }

        widgets = targetWidgets.data();
    }

    void SetPage(int32_t _page)
    {
        if (selected_tab == _page)
            return;

        selected_tab = _page;
        selected_list_item = -1;
        scrolls[0].v_top = 0;
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
        int32_t numObjects = static_cast<int32_t>(object_repository_get_items_count());

        VisibleListDispose();
        selected_list_item = -1;

        const ObjectRepositoryItem* items = object_repository_get_items();
        for (int32_t i = 0; i < numObjects; i++)
        {
            uint8_t selectionFlags = _objectSelectionFlags[i];
            const ObjectRepositoryItem* item = &items[i];
            if (item->Type == GetSelectedObjectType() && !(selectionFlags & ObjectSelectionFlags::Flag6) && FilterSource(item)
                && FilterString(*item) && FilterChunks(item) && FilterSelected(selectionFlags))
            {
                auto filter = std::make_unique<rct_object_filters>();
                filter->ride.category[0] = 0;
                filter->ride.category[1] = 0;
                filter->ride.ride_type = 0;

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
                    log_warning("Wrong sort type %d, leaving list as-is.", _listSortType);
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

    void DrawDescriptions(rct_drawpixelinfo* dpi)
    {
        const auto& widget = widgets[WIDX_PREVIEW];
        auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_LIST].right + 4, widget.bottom + 23 };
        auto _width2 = windowPos.x + this->width - screenPos.x - 4;

        auto description = ObjectGetDescription(_loadedObject.get());
        if (!description.empty())
        {
            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(description.c_str());

            screenPos.y += DrawTextWrapped(dpi, screenPos, _width2, STR_WINDOW_COLOUR_2_STRINGID, ft) + LIST_ROW_HEIGHT;
        }
        if (GetSelectedObjectType() == ObjectType::Ride)
        {
            auto* rideObject = reinterpret_cast<RideObject*>(_loadedObject.get());
            const auto* rideEntry = reinterpret_cast<rct_ride_entry*>(rideObject->GetLegacyData());
            if (rideEntry->shop_item[0] != ShopItem::None)
            {
                std::string sells = "";
                for (size_t i = 0; i < std::size(rideEntry->shop_item); i++)
                {
                    if (rideEntry->shop_item[i] == ShopItem::None)
                        continue;

                    if (!sells.empty())
                        sells += ", ";

                    sells += language_get_string(GetShopItemDescriptor(rideEntry->shop_item[i]).Naming.Plural);
                }
                auto ft = Formatter();
                ft.Add<const char*>(sells.c_str());
                screenPos.y += DrawTextWrapped(dpi, screenPos, _width2, STR_RIDE_OBJECT_SHOP_SELLS, ft) + 2;
            }
        }
        else if (GetSelectedObjectType() == ObjectType::SceneryGroup)
        {
            const auto* sceneryGroupObject = reinterpret_cast<SceneryGroupObject*>(_loadedObject.get());
            auto ft = Formatter();
            ft.Add<uint16_t>(sceneryGroupObject->GetNumIncludedObjects());
            screenPos.y += DrawTextWrapped(dpi, screenPos, _width2, STR_INCLUDES_X_OBJECTS, ft) + 2;
        }
        else if (GetSelectedObjectType() == ObjectType::Music)
        {
            screenPos.y += DrawTextWrapped(dpi, screenPos, _width2, STR_MUSIC_OBJECT_TRACK_HEADER) + 2;
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
                screenPos.y += DrawTextWrapped(dpi, screenPos + ScreenCoordsXY{ 10, 0 }, _width2, stringId, ft);
            }
        }
    }

    void DrawDebugData(rct_drawpixelinfo* dpi)
    {
        ObjectListItem* listItem = &_listItems[selected_list_item];
        auto screenPos = windowPos + ScreenCoordsXY{ width - 5, height - (LIST_ROW_HEIGHT * 6) };

        // Draw fallback image warning
        if (_loadedObject && _loadedObject->UsesFallbackImages())
        {
            DrawTextBasic(dpi, screenPos, STR_OBJECT_USES_FALLBACK_IMAGES, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
        }
        screenPos.y += LIST_ROW_HEIGHT;

        // Draw ride type.
        if (GetSelectedObjectType() == ObjectType::Ride)
        {
            auto stringId = GetRideTypeStringId(listItem->repositoryItem);
            DrawTextBasic(dpi, screenPos, stringId, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
        }

        screenPos.y += LIST_ROW_HEIGHT;

        // Draw object source
        auto stringId = object_manager_get_source_game_string(listItem->repositoryItem->GetFirstSourceGame());
        DrawTextBasic(dpi, screenPos, stringId, {}, { COLOUR_WHITE, TextAlignment::RIGHT });
        screenPos.y += LIST_ROW_HEIGHT;

        // Draw object filename
        {
            auto path = Path::GetFileName(listItem->repositoryItem->Path);
            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const utf8*>(path.c_str());
            DrawTextBasic(
                dpi, { windowPos.x + this->width - 5, screenPos.y }, STR_WINDOW_COLOUR_2_STRINGID, ft,
                { COLOUR_BLACK, TextAlignment::RIGHT });
            screenPos.y += LIST_ROW_HEIGHT;
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
        if (_FILTER_SELECTED == _FILTER_NONSELECTED)
        {
            return true;
        }
        if (_FILTER_SELECTED && objectFlag & ObjectSelectionFlags::Selected)
        {
            return true;
        }
        if (_FILTER_NONSELECTED && !(objectFlag & ObjectSelectionFlags::Selected))
        {
            return true;
        }

        return false;
    }

    static bool IsFilterInName(const ObjectRepositoryItem& item, std::string_view filter)
    {
        return String::Contains(item.Name, filter, true);
    }

    static bool IsFilterInRideType(const ObjectRepositoryItem& item, std::string_view filter)
    {
        if (item.Type == ObjectType::Ride)
        {
            auto rideTypeName = language_get_string(GetRideTypeStringId(&item));
            if (String::Contains(rideTypeName, filter, true))
                return true;
        }
        return false;
    }

    static bool IsFilterInFilename(const ObjectRepositoryItem& item, std::string_view filter)
    {
        return String::Contains(item.Path, filter, true);
    }

    static bool IsFilterInAuthor(const ObjectRepositoryItem& item, std::string_view filter)
    {
        for (auto& author : item.Authors)
        {
            if (String::Contains(author, filter, true))
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
        return (_FILTER_RCT1 && source == ObjectSourceGame::RCT1) ||
            (_FILTER_AA && source == ObjectSourceGame::AddedAttractions) ||
            (_FILTER_LL && source == ObjectSourceGame::LoopyLandscapes) ||
            (_FILTER_RCT2 && source == ObjectSourceGame::RCT2) ||
            (_FILTER_WW && source == ObjectSourceGame::WackyWorlds) ||
            (_FILTER_TT && source == ObjectSourceGame::TimeTwister) ||
            (_FILTER_OO && source == ObjectSourceGame::OpenRCT2Official) ||
            (_FILTER_CUSTOM &&
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
        if (_FILTER_ALL)
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
            for (int32_t i = 0; i < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; i++)
            {
                if (item->RideInfo.RideType[i] != RIDE_TYPE_NULL)
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
        if (!_FILTER_ALL || _filter_string[0] != '\0')
        {
            const auto& selectionFlags = _objectSelectionFlags;
            std::fill(std::begin(_filter_object_counts), std::end(_filter_object_counts), 0);

            size_t numObjects = object_repository_get_items_count();
            const ObjectRepositoryItem* items = object_repository_get_items();
            for (size_t i = 0; i < numObjects; i++)
            {
                const ObjectRepositoryItem* item = &items[i];
                if (FilterSource(item) && FilterString(*item) && FilterChunks(item) && FilterSelected(selectionFlags[i]))
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
        const bool inBounds = selected_tab >= 0 && selected_tab < static_cast<int16_t>(std::size(TabOrder));
        return inBounds ? TabOrder[selected_tab] : ObjectType::Ride;
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
        int32_t listItemIndex = y / SCROLLABLE_ROW_HEIGHT;
        if (listItemIndex < 0 || static_cast<size_t>(listItemIndex) >= _listItems.size())
            return -1;

        return listItemIndex;
    }

    void SetPressedTab()
    {
        for (size_t i = 0; i < std::size(ObjectSelectionPages); i++)
        {
            pressed_widgets &= ~(1 << (WIDX_TAB_1 + i));
        }
        pressed_widgets |= 1LL << (WIDX_TAB_1 + selected_tab);
    }

    /**
     *
     * rct2: 0x006D33E2
     */
    void ManageTracks()
    {
        set_every_ride_type_invented();
        set_every_ride_entry_invented();

        gEditorStep = EditorStep::DesignsManager;

        int32_t entry_index = 0;
        for (; object_entry_get_chunk(ObjectType::Ride, entry_index) == nullptr; entry_index++)
            ;

        rct_ride_entry* ride_entry = get_ride_entry(entry_index);
        auto rideType = ride_entry->GetFirstNonNullRideType();

        auto intent = Intent(WindowClass::TrackDesignList);
        intent.putExtra(INTENT_EXTRA_RIDE_TYPE, rideType);
        intent.putExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, entry_index);
        context_open_intent(&intent);
    }
};

/**
 *
 * rct2: 0x006AA64E
 */
rct_window* WindowEditorObjectSelectionOpen()
{
    return WindowFocusOrCreate<EditorObjectSelectionWindow>(
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
    auto rideTypeA = language_get_string(GetRideTypeStringId(a.repositoryItem));
    auto rideTypeB = language_get_string(GetRideTypeStringId(b.repositoryItem));
    int32_t result = String::Compare(rideTypeA, rideTypeB);
    return result != 0 ? result < 0 : VisibleListSortRideName(a, b);
}

static StringId GetRideTypeStringId(const ObjectRepositoryItem* item)
{
    StringId result = STR_NONE;
    for (int32_t i = 0; i < RCT2::ObjectLimits::MaxRideTypesPerRideEntry; i++)
    {
        auto rideType = item->RideInfo.RideType[i];
        if (rideType != RIDE_TYPE_NULL)
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
    auto& objManager = OpenRCT2::GetContext()->GetObjectManager();
    int32_t numItems = static_cast<int32_t>(object_repository_get_items_count());
    const ObjectRepositoryItem* items = object_repository_get_items();
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
                    log_error("Failed to load entry %s", std::string(descriptor.GetName()).c_str());
                }
                else if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
                {
                    // Defaults selected items to researched (if in-game)
                    auto objectType = loadedObject->GetObjectType();
                    auto entryIndex = object_manager_get_loaded_object_entry_index(loadedObject);
                    if (objectType == ObjectType::Ride)
                    {
                        rct_ride_entry* rideEntry = get_ride_entry(entryIndex);
                        auto rideType = rideEntry->GetFirstNonNullRideType();
                        ResearchCategory category = static_cast<ResearchCategory>(GetRideTypeDescriptor(rideType).Category);
                        research_insert_ride_entry(rideType, entryIndex, category, true);
                    }
                    else if (objectType == ObjectType::SceneryGroup)
                    {
                        research_insert_scenery_group_entry(entryIndex, true);
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
        load_palette();
    }
    if (showFallbackWarning)
        context_show_error(STR_OBJECT_SELECTION_FALLBACK_IMAGES_WARNING, STR_EMPTY, Formatter::Common());
}
