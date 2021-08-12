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
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
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
    WIDX_TAB_CONTENT,

    WIDX_SEE_THROUGH_RIDES = WIDX_TAB_CONTENT,
    WIDX_SEE_THROUGH_VEHICLES,
    WIDX_SEE_THROUGH_SCENERY,
    WIDX_SEE_THROUGH_TREES,
    WIDX_SEE_THROUGH_PATHS,
    WIDX_SEE_THROUGH_SUPPORTS,
    WIDX_INVISIBLE_GUESTS,
    WIDX_INVISIBLE_STAFF,
    WIDX_INVISIBLE_RIDES,
    WIDX_INVISIBLE_VEHICLES,
    WIDX_INVISIBLE_SCENERY,
    WIDX_INVISIBLE_TREES,
    WIDX_INVISIBLE_PATHS,
    WIDX_INVISIBLE_SUPPORTS,

    WIDX_LIST = WIDX_TAB_CONTENT,

    WIDX_MISC_GROUP = WIDX_TAB_CONTENT,
};

#pragma region MEASUREMENTS

static constexpr const rct_string_id WINDOW_TITLE = STR_TRANSPARENCY_OPTIONS_TITLE;
static constexpr const int32_t WW = 249;
static constexpr const int32_t WH = 300;

static constexpr ScreenSize ICON_BUTTON = {24, 24};
static constexpr ScreenSize FLAT_BUTTON = {24, 12};

static constexpr const int32_t TAB_WIDTH = 31;
static constexpr const int32_t TAB_START = 3;


#pragma endregion

#define MAIN_TRANSPARENCY_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({ 0, 81}, {WW, 257}, WindowWidgetType::ImgBtn, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({ 3, 55}, STR_TRANSPARENCY_OPTIONS_TITLE_MAIN), /* tab 1 */ \
    MakeTab   ({34, 55}, STR_TRANSPARENCY_OPTIONS_TITLE_RIDE)

// 15px group padding top, 17px margin between lines, 17px group padding bottom. 22px group margin bottom
static rct_widget window_transparency_main_widgets[] =
{
    MAIN_TRANSPARENCY_WIDGETS,
    
    MakeWidget({102, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_RIDE,                       STR_SEE_THROUGH_RIDES),
    MakeWidget({127, 15}, ICON_BUTTON,    WindowWidgetType::Button,   WindowColour::Secondary, STR_NONE,                       STR_SEE_THROUGH_VEHICLES),
    MakeWidget({ 27, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_LARGE_SCENERY,    STR_SEE_THROUGH_SCENERY),
    MakeWidget({  2, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_TREES,            STR_SEE_THROUGH_TREES),
    MakeWidget({ 52, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_G2_BUTTON_FOOTPATH,         STR_SEE_THROUGH_PATHS),
    MakeWidget({ 77, 15}, ICON_BUTTON,    WindowWidgetType::Button,   WindowColour::Secondary, STR_NONE,                       STR_SEE_THROUGH_SUPPORTS),
    MakeWidget({152, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_GUESTS,                     STR_INVISIBLE_GUESTS),
    MakeWidget({177, 15}, ICON_BUTTON,    WindowWidgetType::ImgBtn,   WindowColour::Secondary, SPR_MECHANIC,                   STR_INVISIBLE_STAFF),

    MakeWidget({102,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_RIDES),
    MakeWidget({127,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_VEHICLES),
    MakeWidget({ 27,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SCENERY),
    MakeWidget({  2,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_TREES),
    MakeWidget({ 52,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_PATHS),
    MakeWidget({ 77,  40}, FLAT_BUTTON,   WindowWidgetType::Button,   WindowColour::Tertiary,  STR_NONE,                       STR_INVISIBLE_SUPPORTS),

    { WIDGETS_END },
};

static rct_widget window_transparency_rides_widgets[] =
{
    MAIN_TRANSPARENCY_WIDGETS,
    MakeWidget({  4, 95}, {160, 120}, WindowWidgetType::Scroll,       WindowColour::Secondary, SCROLL_VERTICAL),
    { WIDGETS_END },
};

static rct_widget *window_transparency_page_widgets[] =
{
    window_transparency_main_widgets,
    window_transparency_rides_widgets,
};

#define MAIN_TRANSPARENCY_ENABLED_WIDGETS (1ULL << WIDX_CLOSE) | (1ULL << WIDX_TAB_1) | (1ULL << WIDX_TAB_2)

static uint64_t window_transparency_page_enabled_widgets[] = {
    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_SEE_THROUGH_RIDES) |
    (1ULL << WIDX_SEE_THROUGH_VEHICLES) |
    (1ULL << WIDX_SEE_THROUGH_SCENERY) |
    (1ULL << WIDX_SEE_THROUGH_TREES) |
    (1ULL << WIDX_SEE_THROUGH_PATHS) |
    (1ULL << WIDX_INVISIBLE_RIDES) |
    (1ULL << WIDX_INVISIBLE_VEHICLES) |
    (1ULL << WIDX_INVISIBLE_SCENERY) |
    (1ULL << WIDX_INVISIBLE_TREES) |
    (1ULL << WIDX_INVISIBLE_PATHS) |
    (1ULL << WIDX_INVISIBLE_SUPPORTS) |
    (1ULL << WIDX_INVISIBLE_GUESTS) |
    (1ULL << WIDX_INVISIBLE_STAFF) |
    (1ULL << WIDX_SEE_THROUGH_SUPPORTS),

    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_LIST),

    MAIN_TRANSPARENCY_ENABLED_WIDGETS |
    (1ULL << WIDX_MISC_GROUP)
};
static uint64_t window_transparency_page_disabled_widgets[] = {
    0,
};

static uint64_t window_transparency_page_hold_down_widgets[] = {
        0,
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
                    case WINDOW_TRANSPARENCY_PAGE_MISC:
                        OnMouseUpMisc(widgetIndex);
                        break;
                }
                break;
        }
    }

    void OnDropdown(rct_widgetindex widgetIndex, int32_t selectedIndex) override
    {
    }

    void OnMouseUpRides(rct_widgetindex widgetIndex)
    {
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto i = screenCoords.y / SCROLLABLE_ROW_HEIGHT;
        // i += static_cast<int32_t>(_selectedPage * GUESTS_PER_PAGE);
        for (const auto& rideItem : _rideList)
        {
            if (i == 0)
            {
                auto ridec = get_ride(rideItem);
                if (ridec != nullptr)
                {
                    ridec->is_visible = (ridec->is_visible == true ? false : true);
                }
                break;
            }
            i--;
        }
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
                SetWidgetPressed(WIDX_SEE_THROUGH_RIDES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_RIDES));
                SetWidgetPressed(WIDX_SEE_THROUGH_VEHICLES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_VEHICLES));
                SetWidgetPressed(WIDX_SEE_THROUGH_SCENERY, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_SCENERY));
                SetWidgetPressed(WIDX_SEE_THROUGH_TREES, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_TREES));
                SetWidgetPressed(WIDX_SEE_THROUGH_PATHS, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_PATHS));
                SetWidgetPressed(WIDX_SEE_THROUGH_SUPPORTS, (w->viewport->flags & VIEWPORT_FLAG_SEETHROUGH_SUPPORTS));
                SetWidgetPressed(WIDX_INVISIBLE_RIDES, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_RIDES));
                SetWidgetPressed(WIDX_INVISIBLE_VEHICLES, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_VEHICLES));
                SetWidgetPressed(WIDX_INVISIBLE_SCENERY, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_SCENERY));
                SetWidgetPressed(WIDX_INVISIBLE_TREES, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_TREES));
                SetWidgetPressed(WIDX_INVISIBLE_PATHS, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_PATHS));
                SetWidgetPressed(WIDX_INVISIBLE_SUPPORTS, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_SUPPORTS));
                SetWidgetPressed(WIDX_INVISIBLE_GUESTS, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_GUESTS));
                SetWidgetPressed(WIDX_INVISIBLE_STAFF, (w->viewport->flags & VIEWPORT_FLAG_INVISIBLE_STAFF));
                break;
            case WINDOW_TRANSPARENCY_PAGE_MISC:
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
        // Main tab
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
    }

    void OnMouseUpMain(rct_widgetindex widgetIndex)
    {
        rct_window* w = window_get_main();
        if (w == nullptr)
            return;

        switch (widgetIndex)
        {
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
            case WIDX_SEE_THROUGH_SUPPORTS:
                w->viewport->flags ^= VIEWPORT_FLAG_SEETHROUGH_SUPPORTS;
                break;
            case WIDX_INVISIBLE_RIDES:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_RIDES;
                break;
            case WIDX_INVISIBLE_VEHICLES:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_VEHICLES;
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
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_GUESTS;
                break;
            case WIDX_INVISIBLE_STAFF:
                w->viewport->flags ^= VIEWPORT_FLAG_INVISIBLE_STAFF;
                break;
        }
        w->Invalidate();
    }

    void OnMouseUpMisc(rct_widgetindex widgetIndex)
    {
    }

    void OnDropdownMisc(rct_widgetindex widgetIndex, int32_t dropdownIndex)
    {
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        ScreenCoordsXY screenCoords;
        auto bgColour = ThemeGetColour(WC_CUSTOM, 0);

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
