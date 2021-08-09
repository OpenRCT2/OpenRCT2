/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <iterator>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/actions/ParkSetDateAction.h>
#include <openrct2/actions/SetCheatAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Climate.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

// clang-format off
enum
{
    WINDOW_TRANSPARENCY_PAGE_MAIN,
    WINDOW_TRANSPARENCY_PAGE_RIDES,
    WINDOW_TRANSPARENCY_PAGE_MISC,
    WINDOW_TRANSPARENCY_PAGE_COUNT,
};

enum WINDOW_TRANSPARENCY_WIDGET_IDX
{
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_CONTENT,

    WIDX_TERRAIN_GROUP = WIDX_TAB_CONTENT,
    WIDX_UNDERGROUND_VIEW,
    WIDX_TRANSPARENT_WATER,
    WIDX_HIDE_BASE_LAND,
    WIDX_HIDE_VERTICAL_FACES,
    WIDX_SEE_THROUGH_GROUP,
    WIDX_SEE_THROUGH_RIDES,
    WIDX_SEE_THROUGH_VEHICLES,
    WIDX_SEE_THROUGH_SCENERY,
    WIDX_SEE_THROUGH_TREES,
    WIDX_SEE_THROUGH_PATHS,
    WIDX_INVISIBLE_GROUP,
    WIDX_INVISIBLE_RIDES,
    WIDX_INVISIBLE_SCENERY,
    WIDX_INVISIBLE_TREES,
    WIDX_INVISIBLE_PATHS,
    WIDX_INVISIBLE_SUPPORTS,
    WIDX_INVISIBLE_GUESTS,
    WIDX_INVISIBLE_STAFF,

    WIDX_LIST = WIDX_TAB_CONTENT,

    WIDX_PLACEHOLDER_MISC = WIDX_TAB_CONTENT,
};

#pragma region MEASUREMENTS

static constexpr const rct_string_id WINDOW_TITLE = STR_TRANSPARENCY_OPTIONS_TITLE;
static constexpr const int32_t WW = 249;
static constexpr const int32_t WH = 300;

static constexpr ScreenSize CHEAT_BUTTON = {110, 17};
static constexpr ScreenSize CHEAT_CHECK = {221, 12};
static constexpr ScreenSize CHEAT_SPINNER = {117, 14};
static constexpr ScreenSize MINMAX_BUTTON = {55, 17};

static constexpr const int32_t TAB_WIDTH = 31;
static constexpr const int32_t TAB_START = 3;


#pragma endregion

#define MAIN_TRANSPARENCY_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({ 0, 43}, {WW, 257}, WindowWidgetType::ImgBtn, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({ 3, 17}, STR_FINANCIAL_CHEATS_TIP                      ), /* tab 1 */ \
    MakeTab   ({34, 17}, STR_RIDE_CHEATS_TIP                           ), /* tab 2 */ \
    MakeTab   ({65, 17}, STR_PARK_CHEATS_TIP                           )  /* tab 3 */

// 15px group padding top, 17px margin between lines, 17px group padding bottom. 22px group margin bottom
static rct_widget window_transparency_main_widgets[] =
{
    MAIN_TRANSPARENCY_WIDGETS,
    MakeWidget({  5,  48}, {238,  83},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_SET_GUESTS_PARAMETERS), // General parameters group frame
    
    MakeWidget({ 11,  63}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_UNDERGROUND_VIEW,           STR_UNDERGROUND_VIEW),
    MakeWidget({ 11,  80}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_VIEWPORT_TRANSPARENT_WATER, STR_VIEWPORT_TRANSPARENT_WATER),
    MakeWidget({ 11,  97}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_REMOVE_BASE_LAND,           STR_REMOVE_BASE_LAND),
    MakeWidget({ 11, 114}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_REMOVE_VERTICAL_FACES,      STR_REMOVE_VERTICAL_FACES),
    
    MakeWidget({  5, 136}, {238,  100},    WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_SET_GUESTS_PARAMETERS), // See-through parameters group frame

    MakeWidget({ 11, 151}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SEE_THROUGH_RIDES,          STR_SEE_THROUGH_RIDES),
    MakeWidget({ 11, 168}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SEE_THROUGH_VEHICLES,       STR_SEE_THROUGH_VEHICLES),
    MakeWidget({ 11, 185}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SEE_THROUGH_SCENERY,        STR_SEE_THROUGH_SCENERY),
    MakeWidget({ 11, 202}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SEE_THROUGH_TREES,          STR_SEE_THROUGH_TREES), // Trees
    MakeWidget({ 11, 219}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_SEE_THROUGH_PATHS,          STR_SEE_THROUGH_PATHS),

    MakeWidget({  5, 241}, {238,  134},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_SET_GUESTS_PARAMETERS), // Invisible parameters group frame

    MakeWidget({ 11, 256}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_RIDES,            STR_INVISIBLE_RIDES),
    MakeWidget({ 11, 273}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_SCENERY,          STR_INVISIBLE_SCENERY),
    MakeWidget({ 11, 290}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_TREES,            STR_INVISIBLE_TREES), // Trees
    MakeWidget({ 11, 307}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_PATHS,            STR_INVISIBLE_PATHS),
    MakeWidget({ 11, 324}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_SUPPORTS,         STR_INVISIBLE_SUPPORTS),
    MakeWidget({ 11, 341}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_GUESTS,           STR_INVISIBLE_GUESTS), // Guests
    MakeWidget({ 11, 358}, CHEAT_CHECK,   WindowWidgetType::Checkbox, WindowColour::Secondary, STR_INVISIBLE_STAFF,            STR_INVISIBLE_STAFF), // Staff

    { WIDGETS_END },
};

static rct_widget window_transparency_rides_widgets[] =
{
    MAIN_TRANSPARENCY_WIDGETS,
    MakeWidget({  4, 60}, {288, 327}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL),
    { WIDGETS_END },
};

//Strings for following moved to window_cheats_paint()
static rct_widget window_transparency_misc_widgets[] =
{
    MAIN_TRANSPARENCY_WIDGETS,
    MakeWidget        ({  5,  48}, {238,  60},   WindowWidgetType::Groupbox, WindowColour::Secondary, STR_CHEAT_GENERAL_GROUP), // General group
    { WIDGETS_END },
};

static rct_widget *window_transparency_page_widgets[] =
{
    window_transparency_main_widgets,
    window_transparency_rides_widgets,
    window_transparency_misc_widgets,
};

#define MAIN_TRANSPARENCY_ENABLED_WIDGETS (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2) | (1ULL << WIDX_TAB_3)

static uint64_t window_transparency_page_enabled_widgets[] = {
    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_TERRAIN_GROUP) |
    (1ULL << WIDX_UNDERGROUND_VIEW) |
    (1ULL << WIDX_TRANSPARENT_WATER) |
    (1ULL << WIDX_HIDE_BASE_LAND) |
    (1ULL << WIDX_HIDE_VERTICAL_FACES) |
    (1ULL << WIDX_SEE_THROUGH_GROUP) |
    (1ULL << WIDX_SEE_THROUGH_RIDES) |
    (1ULL << WIDX_SEE_THROUGH_VEHICLES) |
    (1ULL << WIDX_SEE_THROUGH_SCENERY) |
    (1ULL << WIDX_SEE_THROUGH_TREES) |
    (1ULL << WIDX_SEE_THROUGH_PATHS) |
    (1ULL << WIDX_INVISIBLE_GROUP) |
    (1ULL << WIDX_INVISIBLE_RIDES) |
    (1ULL << WIDX_INVISIBLE_SCENERY) |
    (1ULL << WIDX_INVISIBLE_TREES) |
    (1ULL << WIDX_INVISIBLE_PATHS) |
    (1ULL << WIDX_INVISIBLE_SUPPORTS) |
    (1ULL << WIDX_INVISIBLE_GUESTS) |
    (1ULL << WIDX_INVISIBLE_STAFF),

    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_LIST),

    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_PLACEHOLDER_MISC)
};

static uint64_t window_transparency_page_hold_down_widgets[] = {
        WIDX_UNDERGROUND_VIEW,
};

static rct_string_id window_transparency_page_titles[] = {
    STR_TRANSPARENCY_OPTIONS_TITLE_MAIN,
    STR_TRANSPARENCY_OPTIONS_TITLE_RIDE,
    STR_TRANSPARENCY_OPTIONS_TITLE_MISC,
};
// clang-format on

class TransparencyWindow final : public Window
{
private:
    int32_t _selected_list_item = -1;
    std::vector<ride_id_t> _rideList;

public:
    void OnOpen() override
    {
        SetPage(WINDOW_TRANSPARENCY_PAGE_MAIN);
        RefreshRideList();
    }

    void OnUpdate() override
    {
        frame_no++;
        InvalidateWidget(WIDX_TAB_1 + page);
    }

    void OnMouseDown(rct_widgetindex widgetIndex) override
    {
        // switch (page)
    }

    void OnMouseUp(rct_widgetindex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
            default:
                switch (page)
                {
                    case WINDOW_TRANSPARENCY_PAGE_MAIN:
                        OnMouseUpMain(widgetIndex);
                        break;
                    case WINDOW_TRANSPARENCY_PAGE_RIDES:
                        OnMouseUpRides(widgetIndex);
                        break;
                }
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
    }

    void OnMouseUpMain(rct_widgetindex widgetIndex)
    {
        rct_window* w = window_get_main();
        if (w == nullptr)
            return;

        switch (widgetIndex)
        {
            case WIDX_UNDERGROUND_VIEW:
                w->viewport->flags ^= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                break;
            case WIDX_TRANSPARENT_WATER:
                gConfigGeneral.transparent_water ^= 1;
                config_save_default();
                break;
            case WIDX_HIDE_BASE_LAND:
                w->viewport->flags ^= VIEWPORT_FLAG_HIDE_BASE;
                break;
            case WIDX_HIDE_VERTICAL_FACES:
                w->viewport->flags ^= VIEWPORT_FLAG_HIDE_VERTICAL;
                break;
            case WIDX_SEE_THROUGH_RIDES:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_RIDES;
                break;
            case WIDX_SEE_THROUGH_VEHICLES:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_VEHICLES;
                break;
            case WIDX_SEE_THROUGH_SCENERY:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SCENERY;
                break;
            case WIDX_SEE_THROUGH_TREES:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_TREES;
                break;
            case WIDX_SEE_THROUGH_PATHS:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_PATHS;
                break;
            case WIDX_INVISIBLE_RIDES:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_RIDES;
                break;
            case WIDX_INVISIBLE_SCENERY:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SCENERY;
                break;
            case WIDX_INVISIBLE_TREES:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_TREES;
                break;
            case WIDX_INVISIBLE_PATHS:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PATHS;
                break;
            case WIDX_INVISIBLE_SUPPORTS:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_SUPPORTS;
                break;
            case WIDX_INVISIBLE_GUESTS:
                if (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
                    w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_GUESTS;
                break;
            case WIDX_INVISIBLE_STAFF:
                if (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS)
                    w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_PEEPS;
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_STAFF;
                break;
        }
    }

    void OnMouseUpRides(rct_widgetindex widgetIndex)
    {
        // TODO: Get the clicked on ride id, get the ride, set toggle is_visible
    }

    void OnPrepareDraw() override
    {
        auto* targetWidgets = window_transparency_page_widgets[page];
        if (widgets != targetWidgets)
        {
            widgets = targetWidgets;
            WindowInitScrollWidgets(this);
        }

        pressed_widgets = 0;
        disabled_widgets = 0;

        // Set correct active tab
        for (auto i = 0; i < WINDOW_TRANSPARENCY_PAGE_COUNT; i++)
            SetWidgetPressed(WIDX_TAB_1 + i, false);
        SetWidgetPressed(WIDX_TAB_1 + page, true);

        // Set title
        widgets[WIDX_TITLE].text = window_transparency_page_titles[page];

        rct_window* w = window_get_main();
        if (w == nullptr)
            return;

        switch (page)
        {
            case WINDOW_TRANSPARENCY_PAGE_MAIN:
                SetCheckboxValue(WIDX_UNDERGROUND_VIEW, (w->viewport->flags & VIEWPORT_FLAG_UNDERGROUND_INSIDE));
                SetCheckboxValue(WIDX_TRANSPARENT_WATER, (gConfigGeneral.transparent_water == 1));
                SetCheckboxValue(WIDX_HIDE_BASE_LAND, (w->viewport->flags & VIEWPORT_FLAG_HIDE_BASE));
                SetCheckboxValue(WIDX_HIDE_VERTICAL_FACES, (w->viewport->flags & VIEWPORT_FLAG_HIDE_VERTICAL));
                SetCheckboxValue(WIDX_SEE_THROUGH_RIDES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_RIDES));
                SetCheckboxValue(WIDX_SEE_THROUGH_VEHICLES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_VEHICLES));
                SetCheckboxValue(WIDX_SEE_THROUGH_SCENERY, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_SCENERY));
                SetCheckboxValue(WIDX_SEE_THROUGH_TREES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_TREES));
                SetCheckboxValue(WIDX_SEE_THROUGH_PATHS, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_PATHS));
                SetCheckboxValue(WIDX_INVISIBLE_RIDES, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_RIDES));
                SetCheckboxValue(WIDX_INVISIBLE_SCENERY, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_SCENERY));
                SetCheckboxValue(WIDX_INVISIBLE_TREES, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_TREES));
                SetCheckboxValue(WIDX_INVISIBLE_PATHS, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PATHS));
                SetCheckboxValue(WIDX_INVISIBLE_SUPPORTS, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS));
                SetCheckboxValue(
                    WIDX_INVISIBLE_GUESTS,
                    (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_GUESTS)
                        || (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS));
                SetCheckboxValue(
                    WIDX_INVISIBLE_STAFF,
                    (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_STAFF)
                        || (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PEEPS));
            break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        UpdateTabPositions();
        DrawWidgets(dpi);
        DrawTabImages(dpi);
    }

    void OnTextInput(rct_widgetindex widgetIndex, std::string_view text) override
    {
    }

    OpenRCT2String OnTooltip(rct_widgetindex widgetIndex, rct_string_id fallback) override
    {
        return { fallback, {} };
    }

private:
    void SetPage(int32_t p)
    {
        page = p;
        frame_no = 0;

        enabled_widgets = window_transparency_page_enabled_widgets[p];
        hold_down_widgets = window_transparency_page_hold_down_widgets[p];
        pressed_widgets = 0;
        widgets = window_transparency_page_widgets[p];

        auto maxY = 0;
        auto* widget = &widgets[WIDX_TAB_CONTENT];
        while (widget->type != WindowWidgetType::Last)
        {
            maxY = std::max<int32_t>(maxY, widget->bottom);
            widget++;
        }
        maxY += 6;

        Invalidate();
        height = maxY;
        widgets[WIDX_BACKGROUND].bottom = maxY - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = maxY - 1;
        Invalidate();
    }

    void UpdateTabPositions()
    {
        constexpr const uint16_t tabs[] = {
            WIDX_TAB_1,
            WIDX_TAB_2,
            WIDX_TAB_3,
        };

        auto left = TAB_START;
        for (auto tab : tabs)
        {
            widgets[tab].left = left;
            if (!IsWidgetDisabled(tab))
            {
                left += TAB_WIDTH;
            }
        }
    }

    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        // Money tab
        if (!IsWidgetDisabled(WIDX_TAB_1))
        {
            uint32_t sprite_idx = SPR_TAB_FINANCES_SUMMARY_0;
            if (page == WINDOW_TRANSPARENCY_PAGE_MAIN)
                sprite_idx += (frame_no / 2) % 8;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });
        }

        // Rides tab
        if (!IsWidgetDisabled(WIDX_TAB_2))
        {
            uint32_t sprite_idx = SPR_TAB_RIDE_0;
            if (page == WINDOW_TRANSPARENCY_PAGE_RIDES)
                sprite_idx += (frame_no / 4) % 16;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
        }

        // Guests tab
        if (!IsWidgetDisabled(WIDX_TAB_3))
        {
            uint32_t sprite_idx = SPR_TAB_GUESTS_0;
            if (page == WINDOW_TRANSPARENCY_PAGE_MISC)
                sprite_idx += (frame_no / 3) % 8;
            gfx_draw_sprite(
                &dpi, ImageId(sprite_idx), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });
        }
    }

    void OnMouseDownMisc(rct_widgetindex widgetIndex)
    {
        // auto* widget = &widgets[widgetIndex];
    }

    void OnMouseUpMisc(rct_widgetindex widgetIndex)
    {
    }

    void OnDropdownMisc(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
    }

    void OnMouseDownRides(rct_widgetindex widgetIndex)
    {
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        ScreenCoordsXY screenCoords;
        auto bgColour = ThemeGetColour(WC_CUSTOM, 0);

        //uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
        //gfx_clear(dpi, paletteIndex);

        screenCoords.y = 2;
        for (size_t i = 0; i < _rideList.size(); i++)
        {
            auto ridec = get_ride(_rideList[i]);
            if (ridec == nullptr)
                continue;

            if (screenCoords.y + SCROLLABLE_ROW_HEIGHT >= dpi.y && screenCoords.y <= dpi.y + dpi.height)
            {
                gfx_fill_rect_inset(&dpi, { { 2, screenCoords.y }, { 11, screenCoords.y + 10 } }, bgColour, INSET_RECT_F_E0);

                // Draw checkmark
                if (ridec->is_visible == true)
                {
                    screenCoords.x = 2;
                    FontSpriteBase fontSpriteBase = FontSpriteBase::MEDIUM_DARK;
                    colour_t colour2 = NOT_TRANSLUCENT(bgColour);
                    gfx_draw_string(
                        &dpi, screenCoords, static_cast<const char*>(CheckBoxMarkString),
                        { static_cast<colour_t>(colour2), fontSpriteBase });
                }

                screenCoords.x = 13;

                int32_t width_limit = widgets[WIDX_LIST].width() - screenCoords.x;

                // Ride name
                auto ft = Formatter();
                ridec->FormatNameTo(ft);
                DrawTextEllipsised(&dpi, { screenCoords.x, screenCoords.y }, width_limit, STR_WINDOW_COLOUR_2_STRINGID, ft);
            }
            screenCoords.y += SCROLLABLE_ROW_HEIGHT;
        }
    }

    void RefreshRideList()
    {
        _rideList.clear();

        size_t list_index = 0;
        for (auto& ridec : GetRideManager())
        {
            auto rided = &ridec;
            _rideList.push_back(rided->id);
            list_index++;
        }

        _selected_list_item = -1;
        Invalidate();
    }
};

rct_window* WindowTransparencyOpen()
{
    auto* window = window_bring_to_front_by_class(WC_TRANSPARENCY);
    if (window == nullptr)
    {
        window = WindowCreate<TransparencyWindow>(WC_TRANSPARENCY, ScreenCoordsXY(32, 32), WW, WH);
    }
    return window;
}
