/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/StaffFireAction.hpp>
#include <openrct2/actions/StaffSetColourAction.hpp>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/peep/Staff.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Sprite.h>
#include <vector>

// clang-format off
enum {
    WINDOW_STAFF_LIST_TAB_HANDYMEN,
    WINDOW_STAFF_LIST_TAB_MECHANICS,
    WINDOW_STAFF_LIST_TAB_SECURITY,
    WINDOW_STAFF_LIST_TAB_ENTERTAINERS
};

static std::vector<uint16_t> StaffList;
static bool _quick_fire_mode = false;

static void window_staff_list_close(rct_window *w);
static void window_staff_list_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_list_resize(rct_window *w);
static void window_staff_list_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_staff_list_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_staff_list_update(rct_window *w);
static void window_staff_list_tooldown(rct_window *w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords);
static void window_staff_list_toolabort(rct_window *w, rct_widgetindex widgetIndex);
static void window_staff_list_scrollgetsize(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void window_staff_list_scrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_staff_list_scrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void window_staff_list_invalidate(rct_window *w);
static void window_staff_list_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_staff_list_scrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static rct_window_event_list window_staff_list_events = {
    window_staff_list_close,
    window_staff_list_mouseup,
    window_staff_list_resize,
    window_staff_list_mousedown,
    window_staff_list_dropdown,
    nullptr,
    window_staff_list_update,
    nullptr,
    nullptr,
    nullptr,
    window_staff_list_tooldown,
    nullptr,
    nullptr,
    window_staff_list_toolabort,
    nullptr,
    window_staff_list_scrollgetsize,
    window_staff_list_scrollmousedown,
    nullptr,
    window_staff_list_scrollmouseover,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    nullptr,
    window_staff_list_invalidate,
    window_staff_list_paint,
    window_staff_list_scrollpaint,
};

enum WINDOW_STAFF_LIST_WIDGET_IDX {
    WIDX_STAFF_LIST_BACKGROUND,
    WIDX_STAFF_LIST_TITLE,
    WIDX_STAFF_LIST_CLOSE,
    WIDX_STAFF_LIST_TAB_CONTENT_PANEL,
    WIDX_STAFF_LIST_HANDYMEN_TAB,
    WIDX_STAFF_LIST_MECHANICS_TAB,
    WIDX_STAFF_LIST_SECURITY_TAB,
    WIDX_STAFF_LIST_ENTERTAINERS_TAB,
    WIDX_STAFF_LIST_LIST,
    WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER,
    WIDX_STAFF_LIST_HIRE_BUTTON,
    WIDX_STAFF_LIST_QUICK_FIRE,
    WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON,
    WIDX_STAFF_LIST_MAP,
};

static constexpr const rct_string_id WINDOW_TITLE = STR_STAFF;
static constexpr const int32_t WW = 320;
static constexpr const int32_t WH = 270;
constexpr int32_t MAX_WW = 500;
constexpr int32_t MAX_WH = 450;

static rct_widget window_staff_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0, 43}, {    WW, WH - 43}, WWT_RESIZE,    WindowColour::Secondary                                                 ), // tab content panel
    MakeTab   ({  3, 17},                                                                             STR_STAFF_HANDYMEN_TAB_TIP    ), // handymen tab
    MakeTab   ({ 34, 17},                                                                             STR_STAFF_MECHANICS_TAB_TIP   ), // mechanics tab
    MakeTab   ({ 65, 17},                                                                             STR_STAFF_SECURITY_TAB_TIP    ), // security guards tab
    MakeTab   ({ 96, 17},                                                                             STR_STAFF_ENTERTAINERS_TAB_TIP), // entertainers tab
    MakeWidget({  3, 72}, {WW - 6,     195}, WWT_SCROLL,    WindowColour::Secondary, SCROLL_VERTICAL                                ), // staff list
    MakeWidget({130, 58}, {    12,      12}, WWT_COLOURBTN, WindowColour::Secondary, STR_NONE,        STR_UNIFORM_COLOUR_TIP        ), // uniform colour picker
    MakeWidget({165, 17}, {   145,      13}, WWT_BUTTON,    WindowColour::Primary  , STR_NONE,        STR_HIRE_STAFF_TIP            ), // hire button
    MakeWidget({243, 46}, {    24,      24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_DEMOLISH,    STR_QUICK_FIRE_STAFF          ), // quick fire staff
    MakeWidget({267, 46}, {    24,      24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_PATROL_BTN,  STR_SHOW_PATROL_AREA_TIP      ), // show staff patrol area tool
    MakeWidget({291, 46}, {    24,      24}, WWT_FLATBTN,   WindowColour::Secondary, SPR_MAP,         STR_SHOW_STAFF_ON_MAP_TIP     ), // show staff on map button
    { WIDGETS_END },
};

static int32_t _windowStaffListHighlightedIndex;
static int32_t _windowStaffListSelectedTab = WINDOW_STAFF_LIST_TAB_HANDYMEN;

static uint8_t window_staff_list_get_random_entertainer_costume();

struct staff_naming_convention
{
    rct_string_id plural;
    rct_string_id singular;
    rct_string_id action_hire;
};

static constexpr const staff_naming_convention StaffNamingConvention[] = {
    { STR_HANDYMAN_PLURAL,          STR_HANDYMAN_SINGULAR,          STR_HIRE_HANDYMAN },
    { STR_MECHANIC_PLURAL,          STR_MECHANIC_SINGULAR,          STR_HIRE_MECHANIC },
    { STR_SECURITY_GUARD_PLURAL,    STR_SECURITY_GUARD_SINGULAR,    STR_HIRE_SECURITY_GUARD },
    { STR_ENTERTAINER_PLURAL,       STR_ENTERTAINER_SINGULAR,       STR_HIRE_ENTERTAINER },
};
// clang-format on

/*
 * rct2: 0x006BD3CC
 **/
rct_window* window_staff_list_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_bring_to_front_by_class(WC_STAFF_LIST);
    if (window != nullptr)
        return window;

    window = window_create_auto_pos(WW, WH, &window_staff_list_events, WC_STAFF_LIST, WF_10 | WF_RESIZABLE);
    window->widgets = window_staff_list_widgets;
    window->enabled_widgets = (1 << WIDX_STAFF_LIST_CLOSE) | (1 << WIDX_STAFF_LIST_HANDYMEN_TAB)
        | (1 << WIDX_STAFF_LIST_MECHANICS_TAB) | (1 << WIDX_STAFF_LIST_SECURITY_TAB) | (1 << WIDX_STAFF_LIST_ENTERTAINERS_TAB)
        | (1 << WIDX_STAFF_LIST_HIRE_BUTTON) | (1 << WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER)
        | (1 << WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON) | (1 << WIDX_STAFF_LIST_MAP) | (1 << WIDX_STAFF_LIST_QUICK_FIRE);

    window_init_scroll_widgets(window);
    _windowStaffListHighlightedIndex = -1;
    window->list_information_type = 0;

    window_staff_list_widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WWT_EMPTY;
    window->min_width = WW;
    window->min_height = WH;
    window->max_width = MAX_WW;
    window->max_height = MAX_WH;
    _quick_fire_mode = false;

    WindowStaffListRefresh();
    return window;
}

void WindowStaffListRefresh()
{
    auto w = window_find_by_class(WC_STAFF_LIST);
    if (w == nullptr)
    {
        return;
    }
    StaffList.clear();

    for (auto peep : EntityList<Staff>(EntityListId::Peep))
    {
        sprite_set_flashing(peep, false);
        if (static_cast<uint8_t>(peep->AssignedStaffType) != _windowStaffListSelectedTab)
            continue;
        sprite_set_flashing(peep, true);

        StaffList.push_back(peep->sprite_index);
    }

    std::sort(StaffList.begin(), StaffList.end(), [](const uint16_t a, const uint16_t b) { return peep_compare(a, b) < 0; });
}

static void window_staff_list_cancel_tools(rct_window* w)
{
    if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        if (w->classification == gCurrentToolWidget.window_classification && w->number == gCurrentToolWidget.window_number)
            tool_cancel();
}

/*
 * rct2: 0x006BD9B1
 **/
void window_staff_list_close(rct_window* w)
{
    window_staff_list_cancel_tools(w);
}

/**
 *
 *  rct2: 0x006BD94C
 */
static void window_staff_list_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_STAFF_LIST_CLOSE:
            window_close(w);
            break;
        case WIDX_STAFF_LIST_HIRE_BUTTON:
        {
            StaffType staffType = static_cast<StaffType>(_windowStaffListSelectedTab);
            ENTERTAINER_COSTUME costume = ENTERTAINER_COSTUME_COUNT;
            if (staffType == StaffType::Entertainer)
            {
                costume = static_cast<ENTERTAINER_COSTUME>(window_staff_list_get_random_entertainer_costume());
            }
            staff_hire_new_member(staffType, costume);
            break;
        }
        case WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON:
            if (!tool_set(w, WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON, TOOL_CROSSHAIR))
            {
                show_gridlines();
                gStaffDrawPatrolAreas = _windowStaffListSelectedTab | 0x8000;
                gfx_invalidate_screen();
            }
            break;
        case WIDX_STAFF_LIST_MAP:
            context_open_window(WC_MAP);
            break;
        case WIDX_STAFF_LIST_QUICK_FIRE:
            _quick_fire_mode ^= 1;
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x006BDD5D
 */
static void window_staff_list_resize(rct_window* w)
{
    w->min_width = WW;
    w->min_height = WH;
    if (w->width < w->min_width)
    {
        w->width = w->min_width;
        w->Invalidate();
    }
    if (w->height < w->min_height)
    {
        w->height = w->min_height;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006BD971
 */
static void window_staff_list_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    int16_t newSelectedTab;

    switch (widgetIndex)
    {
        case WIDX_STAFF_LIST_HANDYMEN_TAB:
        case WIDX_STAFF_LIST_MECHANICS_TAB:
        case WIDX_STAFF_LIST_SECURITY_TAB:
        case WIDX_STAFF_LIST_ENTERTAINERS_TAB:
            newSelectedTab = widgetIndex - WIDX_STAFF_LIST_HANDYMEN_TAB;
            if (_windowStaffListSelectedTab == newSelectedTab)
                break;

            _windowStaffListSelectedTab = static_cast<uint8_t>(newSelectedTab);
            w->Invalidate();
            w->scrolls[0].v_top = 0;
            window_staff_list_cancel_tools(w);
            break;
        case WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER:
            window_dropdown_show_colour(
                w, widget, w->colours[1], staff_get_colour(static_cast<StaffType>(_windowStaffListSelectedTab)));
            break;
    }
}

/**
 *
 *  rct2: 0x006BD9A6
 */
static void window_staff_list_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
{
    if (widgetIndex == WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER && dropdownIndex != -1)
    {
        auto action = StaffSetColourAction(static_cast<StaffType>(_windowStaffListSelectedTab), dropdownIndex);
        GameActions::Execute(&action);
    }
}

/**
 *
 *  rct2: 0x006BDCEA
 */
void window_staff_list_update(rct_window* w)
{
    w->list_information_type++;
    if (w->list_information_type >= 24)
    {
        w->list_information_type = 0;
    }
    else
    {
        widget_invalidate(w, WIDX_STAFF_LIST_HANDYMEN_TAB + _windowStaffListSelectedTab);

        // Enable highlighting of these staff members in map window
        if (window_find_by_class(WC_MAP) != nullptr)
        {
            gWindowMapFlashingFlags |= (1 << 2);
            for (auto peep : EntityList<Staff>(EntityListId::Peep))
            {
                sprite_set_flashing(peep, false);

                if (static_cast<uint8_t>(peep->AssignedStaffType) == _windowStaffListSelectedTab)
                {
                    sprite_set_flashing(peep, true);
                }
            }
        }
    }

    // Note this may be slow if number of staff increases a large amount.
    // See GuestList for fix (more intents) if required.
    WindowStaffListRefresh();
}

/**
 *
 *  rct2: 0x006BD990
 */
static void window_staff_list_tooldown(rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords)
{
    if (widgetIndex == WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON)
    {
        int32_t selectedPeepType = _windowStaffListSelectedTab;

        int32_t direction;
        TileElement* tileElement;
        auto footpathCoords = footpath_get_coordinates_from_pos(screenCoords, &direction, &tileElement);
        if (footpathCoords.isNull())
            return;

        bool isPatrolAreaSet = staff_is_patrol_area_set_for_type(static_cast<StaffType>(selectedPeepType), footpathCoords);

        Peep* closestPeep = nullptr;
        int32_t closestPeepDistance = std::numeric_limits<int32_t>::max();

        for (auto peep : EntityList<Staff>(EntityListId::Peep))
        {
            if (static_cast<uint8_t>(peep->AssignedStaffType) != selectedPeepType)
                continue;

            if (isPatrolAreaSet)
            {
                if (gStaffModes[peep->StaffId] != StaffMode::Patrol)
                {
                    continue;
                }
                if (!peep->AsStaff()->IsLocationInPatrol(footpathCoords))
                {
                    continue;
                }
            }

            if (peep->x == LOCATION_NULL)
            {
                continue;
            }

            int32_t distance = abs(footpathCoords.x - peep->x) + abs(footpathCoords.y - peep->y);
            if (distance < closestPeepDistance)
            {
                closestPeepDistance = distance;
                closestPeep = peep;
            }
        }

        if (closestPeep != nullptr)
        {
            tool_cancel();
            rct_window* staffWindow = window_staff_open(closestPeep);
            window_event_dropdown_call(staffWindow, WC_PEEP__WIDX_PATROL, 0);
        }
        else
        {
            Formatter::Common().Add<rct_string_id>(StaffNamingConvention[selectedPeepType].plural);
            context_show_error(STR_NO_THING_IN_PARK_YET, STR_NONE);
        }
    }
}

/**
 *
 *  rct2: 0x006BD99B
 */
void window_staff_list_toolabort(rct_window* w, rct_widgetindex widgetIndex)
{
    if (widgetIndex == WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON)
    {
        hide_gridlines();
        tool_cancel();
        gStaffDrawPatrolAreas = 0xFFFF;
        gfx_invalidate_screen();
    }
}

/**
 *
 *  rct2: 0x006BDBE6
 */
void window_staff_list_scrollgetsize(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    if (_windowStaffListHighlightedIndex != -1)
    {
        _windowStaffListHighlightedIndex = -1;
        w->Invalidate();
    }

    *height = static_cast<int16_t>(StaffList.size()) * SCROLLABLE_ROW_HEIGHT;
    auto i = *height - window_staff_list_widgets[WIDX_STAFF_LIST_LIST].bottom
        + window_staff_list_widgets[WIDX_STAFF_LIST_LIST].top + 21;
    if (i < 0)
        i = 0;
    if (i < w->scrolls[0].v_top)
    {
        w->scrolls[0].v_top = i;
        w->Invalidate();
    }

    *width = w->widgets[WIDX_STAFF_LIST_LIST].width() - 15;
}

/**
 *
 *  rct2: 0x006BDC9A
 */
void window_staff_list_scrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t i = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    for (auto spriteIndex : StaffList)
    {
        if (i == 0)
        {
            if (_quick_fire_mode)
            {
                auto staffFireAction = StaffFireAction(spriteIndex);
                GameActions::Execute(&staffFireAction);
            }
            else
            {
                auto peep = GetEntity<Staff>(spriteIndex);
                if (peep != nullptr)
                {
                    auto intent = Intent(WC_PEEP);
                    intent.putExtra(INTENT_EXTRA_PEEP, peep);
                    context_open_intent(&intent);
                }
            }
            break;
        }

        i--;
    }
}

/**
 *
 *  rct2: 0x006BDC6B
 */
void window_staff_list_scrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    int32_t i;

    i = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
    if (i != _windowStaffListHighlightedIndex)
    {
        _windowStaffListHighlightedIndex = i;
        w->Invalidate();
    }
}

/**
 *
 *  rct2: 0x006BD477
 */
void window_staff_list_invalidate(rct_window* w)
{
    int32_t pressed_widgets = w->pressed_widgets
        & ~((1LL << WIDX_STAFF_LIST_HANDYMEN_TAB) | (1LL << WIDX_STAFF_LIST_MECHANICS_TAB)
            | (1LL << WIDX_STAFF_LIST_SECURITY_TAB) | (1LL << WIDX_STAFF_LIST_ENTERTAINERS_TAB));
    uint8_t tabIndex = _windowStaffListSelectedTab;
    uint8_t widgetIndex = tabIndex + WIDX_STAFF_LIST_HANDYMEN_TAB;

    w->pressed_widgets = pressed_widgets | (1ULL << widgetIndex);
    window_staff_list_widgets[WIDX_STAFF_LIST_HIRE_BUTTON].text = StaffNamingConvention[tabIndex].action_hire;
    window_staff_list_widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WWT_EMPTY;

    if (tabIndex < 3)
    {
        window_staff_list_widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WWT_COLOURBTN;
        auto spriteIdPalette = SPRITE_ID_PALETTE_COLOUR_1(
            static_cast<uint32_t>(staff_get_colour(static_cast<StaffType>(tabIndex))));
        window_staff_list_widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].image = spriteIdPalette | IMAGE_TYPE_TRANSPARENT
            | SPR_PALETTE_BTN;
    }
    if (_quick_fire_mode)
        w->pressed_widgets |= (1ULL << WIDX_STAFF_LIST_QUICK_FIRE);
    else
        w->pressed_widgets &= ~(1ULL << WIDX_STAFF_LIST_QUICK_FIRE);

    window_staff_list_widgets[WIDX_STAFF_LIST_BACKGROUND].right = w->width - 1;
    window_staff_list_widgets[WIDX_STAFF_LIST_BACKGROUND].bottom = w->height - 1;
    window_staff_list_widgets[WIDX_STAFF_LIST_TAB_CONTENT_PANEL].right = w->width - 1;
    window_staff_list_widgets[WIDX_STAFF_LIST_TAB_CONTENT_PANEL].bottom = w->height - 1;
    window_staff_list_widgets[WIDX_STAFF_LIST_TITLE].right = w->width - 2;
    window_staff_list_widgets[WIDX_STAFF_LIST_CLOSE].left = w->width - 2 - 11;
    window_staff_list_widgets[WIDX_STAFF_LIST_CLOSE].right = w->width - 2 - 11 + 10;
    window_staff_list_widgets[WIDX_STAFF_LIST_LIST].right = w->width - 4;
    window_staff_list_widgets[WIDX_STAFF_LIST_LIST].bottom = w->height - 15;
    window_staff_list_widgets[WIDX_STAFF_LIST_QUICK_FIRE].left = w->width - 77;
    window_staff_list_widgets[WIDX_STAFF_LIST_QUICK_FIRE].right = w->width - 54;
    window_staff_list_widgets[WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON].left = w->width - 53;
    window_staff_list_widgets[WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON].right = w->width - 30;
    window_staff_list_widgets[WIDX_STAFF_LIST_MAP].left = w->width - 29;
    window_staff_list_widgets[WIDX_STAFF_LIST_MAP].right = w->width - 6;
    window_staff_list_widgets[WIDX_STAFF_LIST_HIRE_BUTTON].left = w->width - 155;
    window_staff_list_widgets[WIDX_STAFF_LIST_HIRE_BUTTON].right = w->width - 11;
}

/**
 *
 *  rct2: 0x006BD533
 */
void window_staff_list_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    int32_t i;
    uint8_t selectedTab;

    // Widgets
    window_draw_widgets(w, dpi);

    selectedTab = _windowStaffListSelectedTab;

    // Handymen tab image
    i = (selectedTab == 0 ? (w->list_information_type & ~3) : 0);
    i += g_peep_animation_entries[PEEP_SPRITE_TYPE_HANDYMAN].sprite_animation->base_image + 1;
    i |= SPRITE_ID_PALETTE_COLOUR_1(gStaffHandymanColour);
    gfx_draw_sprite(
        dpi, i,
        w->windowPos
            + ScreenCoordsXY{ (window_staff_list_widgets[WIDX_STAFF_LIST_HANDYMEN_TAB].left
                               + window_staff_list_widgets[WIDX_STAFF_LIST_HANDYMEN_TAB].right)
                                  / 2,
                              window_staff_list_widgets[WIDX_STAFF_LIST_HANDYMEN_TAB].bottom - 6 },
        0);

    // Mechanic tab image
    i = (selectedTab == 1 ? (w->list_information_type & ~3) : 0);
    i += g_peep_animation_entries[PEEP_SPRITE_TYPE_MECHANIC].sprite_animation->base_image + 1;
    i |= SPRITE_ID_PALETTE_COLOUR_1(gStaffMechanicColour);
    gfx_draw_sprite(
        dpi, i,
        w->windowPos
            + ScreenCoordsXY{ (window_staff_list_widgets[WIDX_STAFF_LIST_MECHANICS_TAB].left
                               + window_staff_list_widgets[WIDX_STAFF_LIST_MECHANICS_TAB].right)
                                  / 2,
                              window_staff_list_widgets[WIDX_STAFF_LIST_MECHANICS_TAB].bottom - 6 },
        0);

    // Security tab image
    i = (selectedTab == 2 ? (w->list_information_type & ~3) : 0);
    i += g_peep_animation_entries[PEEP_SPRITE_TYPE_SECURITY].sprite_animation->base_image + 1;
    i |= SPRITE_ID_PALETTE_COLOUR_1(gStaffSecurityColour);
    gfx_draw_sprite(
        dpi, i,
        w->windowPos
            + ScreenCoordsXY{ (window_staff_list_widgets[WIDX_STAFF_LIST_SECURITY_TAB].left
                               + window_staff_list_widgets[WIDX_STAFF_LIST_SECURITY_TAB].right)
                                  / 2,
                              window_staff_list_widgets[WIDX_STAFF_LIST_SECURITY_TAB].bottom - 6 },
        0);

    rct_drawpixelinfo sprite_dpi;
    if (clip_drawpixelinfo(
            &sprite_dpi, dpi,
            w->windowPos
                + ScreenCoordsXY{ window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].left + 1,
                                  window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].top + 1 },
            window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].right
                - window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].left - 1,
            window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].bottom
                - window_staff_list_widgets[WIDX_STAFF_LIST_ENTERTAINERS_TAB].top - 1))
    {
        // Entertainers tab image
        i = (selectedTab == 3 ? (w->list_information_type & ~3) : 0);
        i += g_peep_animation_entries[PEEP_SPRITE_TYPE_ENTERTAINER_ELEPHANT].sprite_animation->base_image + 1;
        gfx_draw_sprite(&sprite_dpi, i, { 0x0F, 0x17 }, 0);
    }

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        Formatter::Common().Add<money32>(gStaffWageTable[selectedTab]);
        gfx_draw_string_left(
            dpi, STR_COST_PER_MONTH, gCommonFormatArgs, COLOUR_BLACK, w->windowPos + ScreenCoordsXY{ w->width - 155, 0x20 });
    }

    if (selectedTab < 3)
    {
        gfx_draw_string_left(
            dpi, STR_UNIFORM_COLOUR, w, COLOUR_BLACK,
            w->windowPos + ScreenCoordsXY{ 6, window_staff_list_widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].top + 1 });
    }

    int32_t staffTypeStringId = StaffNamingConvention[selectedTab].plural;
    // If the number of staff for a given type is 1, we use the singular forms of the names
    if (StaffList.size() == 1)
    {
        staffTypeStringId = StaffNamingConvention[selectedTab].singular;
    }

    auto ft = Formatter::Common();
    ft.Add<uint16_t>(StaffList.size());
    ft.Add<rct_string_id>(staffTypeStringId);

    gfx_draw_string_left(
        dpi, STR_STAFF_LIST_COUNTER, gCommonFormatArgs, COLOUR_BLACK,
        w->windowPos + ScreenCoordsXY{ 4, window_staff_list_widgets[WIDX_STAFF_LIST_LIST].bottom + 2 });
}

/** rct2: 0x00992A08 */
static constexpr const uint32_t staffOrderBaseSprites[] = {
    SPR_STAFF_ORDERS_SWEEPING,
    SPR_STAFF_ORDERS_INSPECT_RIDES,
    0,
    0,
};

static constexpr const uint32_t staffCostumeSprites[] = {
    SPR_STAFF_COSTUME_PANDA,   SPR_STAFF_COSTUME_TIGER,   SPR_STAFF_COSTUME_ELEPHANT, SPR_STAFF_COSTUME_ROMAN,
    SPR_STAFF_COSTUME_GORILLA, SPR_STAFF_COSTUME_SNOWMAN, SPR_STAFF_COSTUME_KNIGHT,   SPR_STAFF_COSTUME_ASTRONAUT,
    SPR_STAFF_COSTUME_BANDIT,  SPR_STAFF_COSTUME_SHERIFF, SPR_STAFF_COSTUME_PIRATE,
};

/**
 *
 *  rct2: 0x006BD785
 */
void window_staff_list_scrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    auto dpiCoords = ScreenCoordsXY{ dpi->x, dpi->y };
    gfx_fill_rect(
        dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi->width - 1, dpi->height - 1 } }, ColourMapA[w->colours[1]].mid_light);

    // How much space do we have for the name and action columns? (Discount scroll area and icons.)
    const int32_t nonIconSpace = w->widgets[WIDX_STAFF_LIST_LIST].width() - 15 - 68;
    const int32_t nameColumnSize = nonIconSpace * 0.42;
    const int32_t actionColumnSize = nonIconSpace * 0.58;
    const int32_t actionOffset = w->widgets[WIDX_STAFF_LIST_LIST].right - actionColumnSize - 15;

    auto y = 0;
    auto i = 0;
    for (auto spriteIndex : StaffList)
    {
        if (y > dpi->y + dpi->height)
        {
            break;
        }

        if (y + 11 >= dpi->y)
        {
            auto peep = GetEntity<Staff>(spriteIndex);
            if (peep == nullptr)
            {
                continue;
            }
            int32_t format = (_quick_fire_mode ? STR_RED_STRINGID : STR_BLACK_STRING);

            if (i == _windowStaffListHighlightedIndex)
            {
                gfx_filter_rect(dpi, 0, y, 800, y + (SCROLLABLE_ROW_HEIGHT - 1), PALETTE_DARKEN_1);
                format = (_quick_fire_mode ? STR_LIGHTPINK_STRINGID : STR_WINDOW_COLOUR_2_STRINGID);
            }

            auto ft = Formatter::Common();
            peep->FormatNameTo(ft);
            gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { 0, y }, nameColumnSize);

            ft = Formatter::Common();
            peep->FormatActionTo(ft);
            gfx_draw_string_left_clipped(dpi, format, gCommonFormatArgs, COLOUR_BLACK, { actionOffset, y }, actionColumnSize);

            // True if a patrol path is set for the worker
            if (gStaffModes[peep->StaffId] == StaffMode::Patrol)
            {
                gfx_draw_sprite(dpi, SPR_STAFF_PATROL_PATH, { nameColumnSize + 5, y }, 0);
            }

            auto staffOrderIcon_x = nameColumnSize + 20;
            if (peep->AssignedStaffType != StaffType::Entertainer)
            {
                auto staffOrders = peep->StaffOrders;
                auto staffOrderSprite = staffOrderBaseSprites[_windowStaffListSelectedTab];

                while (staffOrders != 0)
                {
                    if (staffOrders & 1)
                    {
                        gfx_draw_sprite(dpi, staffOrderSprite, { staffOrderIcon_x, y }, 0);
                    }
                    staffOrders = staffOrders >> 1;
                    staffOrderIcon_x += 9;
                    // TODO: Remove sprite ID addition
                    staffOrderSprite++;
                }
            }
            else
            {
                gfx_draw_sprite(dpi, staffCostumeSprites[peep->SpriteType - 4], { staffOrderIcon_x, y }, 0);
            }
        }

        y += SCROLLABLE_ROW_HEIGHT;
        i++;
    }
}

static uint8_t window_staff_list_get_random_entertainer_costume()
{
    uint8_t result = ENTERTAINER_COSTUME_PANDA;
    uint8_t costumeList[ENTERTAINER_COSTUME_COUNT];
    int32_t numCostumes = staff_get_available_entertainer_costume_list(costumeList);
    if (numCostumes > 0)
    {
        int32_t index = util_rand() % numCostumes;
        result = costumeList[index];
    }
    return result;
}
