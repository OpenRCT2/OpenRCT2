/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <algorithm>
#include <array>
#include <limits>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/actions/SetParkEntranceFeeAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/localisation/Date.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Award.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Entrance.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_STRINGID;
static constexpr const int32_t WH = 224;

// clang-format off
enum WindowParkPage {
    WINDOW_PARK_PAGE_ENTRANCE,
    WINDOW_PARK_PAGE_RATING,
    WINDOW_PARK_PAGE_GUESTS,
    WINDOW_PARK_PAGE_PRICE,
    WINDOW_PARK_PAGE_STATS,
    WINDOW_PARK_PAGE_OBJECTIVE,
    WINDOW_PARK_PAGE_AWARDS,
    WINDOW_PARK_PAGE_COUNT,
};

enum WindowParkWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_PAGE_BACKGROUND,
    WIDX_TAB_1,
    WIDX_TAB_2,
    WIDX_TAB_3,
    WIDX_TAB_4,
    WIDX_TAB_5,
    WIDX_TAB_6,
    WIDX_TAB_7,

    WIDX_VIEWPORT = 11,
    WIDX_STATUS,
    WIDX_OPEN_OR_CLOSE,
    WIDX_BUY_LAND_RIGHTS,
    WIDX_LOCATE,
    WIDX_RENAME,
    WIDX_CLOSE_LIGHT,
    WIDX_OPEN_LIGHT,

    WIDX_PRICE_LABEL = 11,
    WIDX_PRICE,
    WIDX_INCREASE_PRICE,
    WIDX_DECREASE_PRICE,

    WIDX_ENTER_NAME = 11
};

#pragma region Widgets

#define MAIN_PARK_WIDGETS(WW) \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({  0, 43}, {WW, 131}, WindowWidgetType::Resize, WindowColour::Secondary), /* tab content panel */ \
    MakeTab   ({  3, 17}, STR_PARK_ENTRANCE_TAB_TIP                     ), /* tab 1 */ \
    MakeTab   ({ 34, 17}, STR_PARK_RATING_TAB_TIP                       ), /* tab 2 */ \
    MakeTab   ({ 65, 17}, STR_PARK_GUESTS_TAB_TIP                       ), /* tab 3 */ \
    MakeTab   ({ 96, 17}, STR_PARK_PRICE_TAB_TIP                        ), /* tab 4 */ \
    MakeTab   ({127, 17}, STR_PARK_STATS_TAB_TIP                        ), /* tab 5 */ \
    MakeTab   ({158, 17}, STR_PARK_OBJECTIVE_TAB_TIP                    ), /* tab 6 */ \
    MakeTab   ({189, 17}, STR_PARK_AWARDS_TAB_TIP                       )  /* tab 7 */

static rct_widget _entranceWidgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget({  3,  46}, {202, 115}, WindowWidgetType::Viewport,      WindowColour::Secondary                                                                      ), // viewport
    MakeWidget({  3, 161}, {202,  11}, WindowWidgetType::LabelCentred,  WindowColour::Secondary                                                                      ), // status
    MakeWidget({205,  49}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, 0xFFFFFFFF,                 STR_OPEN_OR_CLOSE_PARK_TIP              ), // open / close
    MakeWidget({205,  73}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_BUY_LAND_RIGHTS,        STR_BUY_LAND_AND_CONSTRUCTION_RIGHTS_TIP), // buy land rights
    MakeWidget({205,  97}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,                 STR_LOCATE_SUBJECT_TIP                  ), // locate
    MakeWidget({205, 121}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,                 STR_NAME_PARK_TIP                       ), // rename
    MakeWidget({210,  51}, { 14,  15}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_CLOSE_BUTTON_0, STR_CLOSE_PARK_TIP                      ),
    MakeWidget({210,  66}, { 14,  14}, WindowWidgetType::ImgBtn,        WindowColour::Secondary, SPR_G2_RCT1_OPEN_BUTTON_0,  STR_OPEN_PARK_TIP                       ),
    WIDGETS_END,
};

static rct_widget _ratingWidgets[] = {
    MAIN_PARK_WIDGETS(255),
    WIDGETS_END,
};

static rct_widget _guestsWidgets[] = {
    MAIN_PARK_WIDGETS(255),
    WIDGETS_END,
};

static rct_widget _priceWidgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget        ({ 21, 50}, {126, 14}, WindowWidgetType::Label,   WindowColour::Secondary, STR_ADMISSION_PRICE),
    MakeSpinnerWidgets({147, 50}, { 76, 14}, WindowWidgetType::Spinner, WindowColour::Secondary                     ), // Price (3 widgets)
    WIDGETS_END,
};

static rct_widget _statsWidgets[] = {
    MAIN_PARK_WIDGETS(230),
    WIDGETS_END,
};

static rct_widget _objectiveWidgets[] = {
    MAIN_PARK_WIDGETS(230),
    MakeWidget({7, 207}, {216, 14}, WindowWidgetType::Button, WindowColour::Secondary, STR_ENTER_NAME_INTO_SCENARIO_CHART), // enter name
    WIDGETS_END,
};

static rct_widget _awardsWidgets[] = {
    MAIN_PARK_WIDGETS(230),
    WIDGETS_END,
};

static std::array<rct_widget*, WINDOW_PARK_PAGE_COUNT> _pagedWidgets = {
    _entranceWidgets,
    _ratingWidgets,
    _guestsWidgets,
    _priceWidgets,
    _statsWidgets,
    _objectiveWidgets,
    _awardsWidgets,
};

#pragma endregion

static std::array<uint32_t, WINDOW_PARK_PAGE_COUNT> _pagedHoldDownWidgets = {
    0,
    0,
    0,
    (1ULL << WIDX_INCREASE_PRICE) |
    (1ULL << WIDX_DECREASE_PRICE),
    0,
    0,
    0,
};

struct WindowParkAward {
    StringId text;
    uint32_t sprite;
};

static constexpr const WindowParkAward _parkAwards[] = {
    { STR_AWARD_MOST_UNTIDY,                SPR_AWARD_MOST_UNTIDY },
    { STR_AWARD_MOST_TIDY,                  SPR_AWARD_MOST_TIDY },
    { STR_AWARD_BEST_ROLLERCOASTERS,        SPR_AWARD_BEST_ROLLERCOASTERS },
    { STR_AWARD_BEST_VALUE,                 SPR_AWARD_BEST_VALUE },
    { STR_AWARD_MOST_BEAUTIFUL,             SPR_AWARD_MOST_BEAUTIFUL },
    { STR_AWARD_WORST_VALUE,                SPR_AWARD_WORST_VALUE },
    { STR_AWARD_SAFEST,                     SPR_AWARD_SAFEST },
    { STR_AWARD_BEST_STAFF,                 SPR_AWARD_BEST_STAFF },
    { STR_AWARD_BEST_FOOD,                  SPR_AWARD_BEST_FOOD },
    { STR_AWARD_WORST_FOOD,                 SPR_AWARD_WORST_FOOD },
    { STR_AWARD_BEST_TOILETS,               SPR_AWARD_BEST_TOILETS },
    { STR_AWARD_MOST_DISAPPOINTING,         SPR_AWARD_MOST_DISAPPOINTING },
    { STR_AWARD_BEST_WATER_RIDES,           SPR_AWARD_BEST_WATER_RIDES },
    { STR_AWARD_BEST_CUSTOM_DESIGNED_RIDES, SPR_AWARD_BEST_CUSTOM_DESIGNED_RIDES },
    { STR_AWARD_MOST_DAZZLING_RIDE_COLOURS, SPR_AWARD_MOST_DAZZLING_RIDE_COLOURS },
    { STR_AWARD_MOST_CONFUSING_LAYOUT,      SPR_AWARD_MOST_CONFUSING_LAYOUT },
    { STR_AWARD_BEST_GENTLE_RIDES,          SPR_AWARD_BEST_GENTLE_RIDES },
};
// clang-format on

class ParkWindow final : public Window
{
    int16_t _numberOfStaff = -1;
    int16_t _numberOfRides = -1;
    uint8_t _peepAnimationFrame = 0;

public:
    void OnOpen() override
    {
        number = 0;
        frame_no = 0;
        _numberOfRides = -1;
        _numberOfStaff = -1;
        _peepAnimationFrame = 0;
        SetPage(0);
    }

    void OnClose() override
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE) && classification == gCurrentToolWidget.window_classification
            && number == gCurrentToolWidget.window_number)
        {
            tool_cancel();
        }
    }

    void OnMouseUp(WidgetIndex idx) override
    {
        switch (idx)
        {
            case WIDX_CLOSE:
                Close();
                return;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
            case WIDX_TAB_4:
            case WIDX_TAB_5:
            case WIDX_TAB_6:
            case WIDX_TAB_7:
                SetPage(idx - WIDX_TAB_1);
                return;
        }
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnMouseUpEntrance(idx);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnMouseUpObjective(idx);
                break;
        }
    }

    void OnResize() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnResizeEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnResizeRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnResizeGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnResizePrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnResizeStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnResizeObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnResizeAwards();
                break;
        }
    }

    void OnMouseDown(WidgetIndex idx) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnMouseDownEntrance(idx);
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnMouseDownPrice(idx);
                break;
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnDropdownEntrance(widgetIndex, selectedIndex);
                break;
        }
    }

    void OnUpdate() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnUpdateEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnUpdateRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnUpdateGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnUpdatePrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnUpdateStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnUpdateObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnUpdateAwards();
                break;
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnTextInputEntrance(widgetIndex, text);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnTextInputObjective(widgetIndex, text);
                break;
        }
    }

    void OnPrepareDraw() override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnPrepareDrawEntrance();
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnPrepareDrawRating();
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnPrepareDrawGuests();
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnPrepareDrawPrice();
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnPrepareDrawStats();
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnPrepareDrawObjective();
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnPrepareDrawAwards();
                break;
        }
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        switch (page)
        {
            case WINDOW_PARK_PAGE_ENTRANCE:
                OnDrawEntrance(dpi);
                break;
            case WINDOW_PARK_PAGE_RATING:
                OnDrawRating(dpi);
                break;
            case WINDOW_PARK_PAGE_GUESTS:
                OnDrawGuests(dpi);
                break;
            case WINDOW_PARK_PAGE_PRICE:
                OnDrawPrice(dpi);
                break;
            case WINDOW_PARK_PAGE_STATS:
                OnDrawStats(dpi);
                break;
            case WINDOW_PARK_PAGE_OBJECTIVE:
                OnDrawObjective(dpi);
                break;
            case WINDOW_PARK_PAGE_AWARDS:
                OnDrawAwards(dpi);
                break;
        }
    }

private:
    void SetDisabledTabs()
    {
        // Disable price tab if money is disabled
        disabled_widgets = (gParkFlags & PARK_FLAGS_NO_MONEY) ? (1ULL << WIDX_TAB_4) : 0;
    }

    void PrepareWindowTitleText()
    {
        auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
        auto parkName = park.Name.c_str();

        auto ft = Formatter::Common();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(parkName);
    }

#pragma region Entrance page
    void OnMouseUpEntrance(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_BUY_LAND_RIGHTS:
                context_open_window(WindowClass::LandRights);
                break;
            case WIDX_LOCATE:
                ScrollToViewport();
                break;
            case WIDX_RENAME:
            {
                auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
                WindowTextInputRawOpen(
                    this, WIDX_RENAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, park.Name.c_str(), USER_STRING_MAX_LENGTH);
                break;
            }
            case WIDX_CLOSE_LIGHT:
                ParkSetOpen(false);
                break;
            case WIDX_OPEN_LIGHT:
                ParkSetOpen(true);
                break;
        }
    }

    void OnResizeEntrance()
    {
        flags |= WF_RESIZABLE;
        window_set_resize(*this, 230, 174 + 9, 230 * 3, (274 + 9) * 3);
        InitViewport();
    }

    void OnMouseDownEntrance(WidgetIndex widgetIndex)
    {
        if (widgetIndex == WIDX_OPEN_OR_CLOSE)
        {
            auto& widget = widgets[widgetIndex];
            gDropdownItems[0].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[1].Format = STR_DROPDOWN_MENU_LABEL;
            gDropdownItems[0].Args = STR_CLOSE_PARK;
            gDropdownItems[1].Args = STR_OPEN_PARK;
            WindowDropdownShowText(
                { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1, colours[1], 0, 2);

            if (ParkIsOpen())
            {
                gDropdownDefaultIndex = 0;
                Dropdown::SetChecked(1, true);
            }
            else
            {
                gDropdownDefaultIndex = 1;
                Dropdown::SetChecked(0, true);
            }
        }
    }

    void OnDropdownEntrance(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex == WIDX_OPEN_OR_CLOSE)
        {
            if (dropdownIndex == -1)
                dropdownIndex = gDropdownHighlightedIndex;

            if (dropdownIndex != 0)
            {
                ParkSetOpen(true);
            }
            else
            {
                ParkSetOpen(false);
            }
        }
    }

    void OnUpdateEntrance()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_1);
    }

    void OnTextInputEntrance(WidgetIndex widgetIndex, std::string_view text)
    {
        if (widgetIndex == WIDX_RENAME && !text.empty())
        {
            auto action = ParkSetNameAction(std::string(text));
            GameActions::Execute(&action);
        }
    }

    void OnPrepareDrawEntrance()
    {
        widgets = _pagedWidgets[page];
        InitScrollWidgets();

        SetPressedTab();

        // Set open / close park button state
        {
            auto& park = OpenRCT2::GetContext()->GetGameState()->GetPark();
            auto parkName = park.Name.c_str();

            auto ft = Formatter::Common();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(parkName);
        }
        widgets[WIDX_OPEN_OR_CLOSE].image = ParkIsOpen() ? SPR_OPEN : SPR_CLOSED;
        widgets[WIDX_CLOSE_LIGHT].image = SPR_G2_RCT1_CLOSE_BUTTON_0 + !ParkIsOpen() * 2
            + WidgetIsPressed(*this, WIDX_CLOSE_LIGHT);
        widgets[WIDX_OPEN_LIGHT].image = SPR_G2_RCT1_OPEN_BUTTON_0 + ParkIsOpen() * 2 + WidgetIsPressed(*this, WIDX_OPEN_LIGHT);

        // Only allow closing of park for guest / rating objective
        if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            disabled_widgets |= (1ULL << WIDX_OPEN_OR_CLOSE) | (1ULL << WIDX_CLOSE_LIGHT) | (1ULL << WIDX_OPEN_LIGHT);
        else
            disabled_widgets &= ~((1ULL << WIDX_OPEN_OR_CLOSE) | (1ULL << WIDX_CLOSE_LIGHT) | (1ULL << WIDX_OPEN_LIGHT));

        // Only allow purchase of land when there is money
        if (gParkFlags & PARK_FLAGS_NO_MONEY)
            widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::Empty;
        else
            widgets[WIDX_BUY_LAND_RIGHTS].type = WindowWidgetType::FlatBtn;

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();

        // Anchor entrance page specific widgets
        widgets[WIDX_VIEWPORT].right = width - 26;
        widgets[WIDX_VIEWPORT].bottom = height - 14;
        widgets[WIDX_STATUS].right = width - 26;
        widgets[WIDX_STATUS].top = height - 13;
        widgets[WIDX_STATUS].bottom = height - 3;

        auto y = 0;
        if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_PARK)
        {
            widgets[WIDX_OPEN_OR_CLOSE].type = WindowWidgetType::Empty;
            if (gScenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
            {
                widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::FlatBtn;
                widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::FlatBtn;
            }
            else
            {
                widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::ImgBtn;
                widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::ImgBtn;
            }
            y = widgets[WIDX_OPEN_LIGHT].bottom + 5;
        }
        else
        {
            widgets[WIDX_OPEN_OR_CLOSE].type = WindowWidgetType::FlatBtn;
            widgets[WIDX_CLOSE_LIGHT].type = WindowWidgetType::Empty;
            widgets[WIDX_OPEN_LIGHT].type = WindowWidgetType::Empty;
            y = 49;
        }

        for (int32_t i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
        {
            widgets[i].left = width - 20;
            widgets[i].right = width - 7;
        }
        for (int32_t i = WIDX_OPEN_OR_CLOSE; i <= WIDX_RENAME; i++)
        {
            if (widgets[i].type == WindowWidgetType::Empty)
                continue;

            widgets[i].left = width - 25;
            widgets[i].right = width - 2;
            widgets[i].top = y;
            widgets[i].bottom = y + 23;
            y += 24;
        }
    }

    void OnDrawEntrance(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        // Draw viewport
        if (viewport != nullptr)
        {
            window_draw_viewport(&dpi, *this);
            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                gfx_draw_sprite(&dpi, ImageId(SPR_HEARING_VIEWPORT), windowPos + ScreenCoordsXY{ 2, 2 });
        }

        // Draw park closed / open label
        auto ft = Formatter();
        ft.Add<StringId>(ParkIsOpen() ? STR_PARK_OPEN : STR_PARK_CLOSED);

        auto* labelWidget = &widgets[WIDX_STATUS];
        DrawTextEllipsised(
            &dpi, windowPos + ScreenCoordsXY{ labelWidget->midX(), labelWidget->top }, labelWidget->width(), STR_BLACK_STRING,
            ft, { TextAlignment::CENTRE });
    }

    void InitViewport()
    {
        if (page != WINDOW_PARK_PAGE_ENTRANCE)
            return;

        std::optional<Focus> newFocus = std::nullopt;
        if (!gParkEntrances.empty())
        {
            const auto& entrance = gParkEntrances[0];
            newFocus = Focus(CoordsXYZ{ entrance.x + 16, entrance.y + 16, entrance.z + 32 });
        }

        int32_t viewportFlags{};
        if (viewport == nullptr)
        {
            viewportFlags = gConfigGeneral.AlwaysShowGridlines ? VIEWPORT_FLAG_GRIDLINES : 0;
        }
        else
        {
            viewportFlags = viewport->flags;
            RemoveViewport();
        }

        // Call invalidate event
        window_event_invalidate_call(this);

        focus = newFocus;

        if (focus.has_value())
        {
            // Create viewport
            if (viewport == nullptr)
            {
                rct_widget* viewportWidget = &widgets[WIDX_VIEWPORT];
                viewport_create(
                    this, windowPos + ScreenCoordsXY{ viewportWidget->left + 1, viewportWidget->top + 1 },
                    viewportWidget->width() - 1, viewportWidget->height() - 1, focus.value());
                flags |= WF_NO_SCROLLING;
                Invalidate();
            }
        }

        if (viewport != nullptr)
            viewport->flags = viewportFlags;
        Invalidate();
    }

#pragma endregion

#pragma region Rating page
    void OnResizeRating()
    {
        window_set_resize(*this, 255, 182, 255, 182);
    }

    void OnUpdateRating()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_2);
    }

    void OnPrepareDrawRating()
    {
        auto* ratingWidgets = _pagedWidgets[page];
        if (ratingWidgets != widgets)
        {
            widgets = ratingWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();
        PrepareWindowTitleText();

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawRating(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto screenPos = windowPos;
        rct_widget* widget = &widgets[WIDX_PAGE_BACKGROUND];

        // Current value
        auto ft = Formatter();
        ft.Add<uint16_t>(gParkRating);
        DrawTextBasic(&dpi, screenPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_PARK_RATING_LABEL, ft);

        // Graph border
        gfx_fill_rect_inset(
            &dpi,
            { screenPos + ScreenCoordsXY{ widget->left + 4, widget->top + 15 },
              screenPos + ScreenCoordsXY{ widget->right - 4, widget->bottom - 4 } },
            colours[1], INSET_RECT_F_30);

        // Y axis labels
        screenPos = screenPos + ScreenCoordsXY{ widget->left + 27, widget->top + 23 };
        for (int i = 5; i >= 0; i--)
        {
            uint32_t axisValue = i * 200;
            ft = Formatter();
            ft.Add<uint32_t>(axisValue);
            DrawTextBasic(
                &dpi, screenPos + ScreenCoordsXY{ 10, 0 }, STR_GRAPH_AXIS_LABEL, ft,
                { FontStyle::Small, TextAlignment::RIGHT });
            gfx_fill_rect_inset(
                &dpi, { screenPos + ScreenCoordsXY{ 15, 5 }, screenPos + ScreenCoordsXY{ width - 32, 5 } }, colours[2],
                INSET_RECT_FLAG_BORDER_INSET);
            screenPos.y += 20;
        }

        // Graph
        screenPos = windowPos + ScreenCoordsXY{ widget->left + 47, widget->top + 26 };

        Graph::Draw(&dpi, gParkRatingHistory, 32, screenPos);
    }

#pragma endregion

#pragma region Guests page
    void OnResizeGuests()
    {
        window_set_resize(*this, 255, 182, 255, 182);
    }

    void OnUpdateGuests()
    {
        frame_no++;
        _peepAnimationFrame = (_peepAnimationFrame + 1) % 24;
        widget_invalidate(*this, WIDX_TAB_3);
    }

    void OnPrepareDrawGuests()
    {
        auto* guestsWidgets = _pagedWidgets[page];
        if (widgets != guestsWidgets)
        {
            widgets = guestsWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();
        PrepareWindowTitleText();

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawGuests(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto screenPos = windowPos;
        rct_widget* widget = &widgets[WIDX_PAGE_BACKGROUND];

        // Current value
        auto ft = Formatter();
        ft.Add<uint32_t>(gNumGuestsInPark);
        DrawTextBasic(&dpi, screenPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_GUESTS_IN_PARK_LABEL, ft);

        // Graph border
        gfx_fill_rect_inset(
            &dpi,
            { screenPos + ScreenCoordsXY{ widget->left + 4, widget->top + 15 },
              screenPos + ScreenCoordsXY{ widget->right - 4, widget->bottom - 4 } },
            colours[1], INSET_RECT_F_30);

        // Y axis labels
        screenPos = screenPos + ScreenCoordsXY{ widget->left + 27, widget->top + 23 };
        for (int i = 5; i >= 0; i--)
        {
            uint32_t axisValue = i * 1000;
            ft = Formatter();
            ft.Add<uint32_t>(axisValue);
            DrawTextBasic(
                &dpi, screenPos + ScreenCoordsXY{ 10, 0 }, STR_GRAPH_AXIS_LABEL, ft,
                { FontStyle::Small, TextAlignment::RIGHT });
            gfx_fill_rect_inset(
                &dpi, { screenPos + ScreenCoordsXY{ 15, 5 }, screenPos + ScreenCoordsXY{ width - 32, 5 } }, colours[2],
                INSET_RECT_FLAG_BORDER_INSET);
            screenPos.y += 20;
        }

        // Graph
        screenPos = windowPos + ScreenCoordsXY{ widget->left + 47, widget->top + 26 };

        uint8_t cappedHistory[32];
        for (size_t i = 0; i < std::size(cappedHistory); i++)
        {
            auto value = gGuestsInParkHistory[i];
            if (value != std::numeric_limits<uint32_t>::max())
            {
                cappedHistory[i] = static_cast<uint8_t>(std::min<uint32_t>(value, 5000) / 20);
            }
            else
            {
                cappedHistory[i] = std::numeric_limits<uint8_t>::max();
            }
        }
        Graph::Draw(&dpi, cappedHistory, static_cast<int32_t>(std::size(cappedHistory)), screenPos);
    }

#pragma endregion

#pragma region Price page
    void OnResizePrice()
    {
        window_set_resize(*this, 230, 124, 230, 124);
    }

    void OnMouseDownPrice(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_INCREASE_PRICE:
            {
                const auto newFee = std::min(MAX_ENTRANCE_FEE, gParkEntranceFee + 1.00_GBP);
                auto gameAction = SetParkEntranceFeeAction(static_cast<money16>(newFee));
                GameActions::Execute(&gameAction);
                break;
            }
            case WIDX_DECREASE_PRICE:
            {
                const auto newFee = std::max(0.00_GBP, gParkEntranceFee - 1.00_GBP);
                auto gameAction = SetParkEntranceFeeAction(static_cast<money16>(newFee));
                GameActions::Execute(&gameAction);
                break;
            }
        }
    }

    void OnUpdatePrice()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_4);
    }

    void OnPrepareDrawPrice()
    {
        auto* priceWidgets = _pagedWidgets[page];
        if (widgets != priceWidgets)
        {
            widgets = priceWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();
        PrepareWindowTitleText();

        // Show a tooltip if the park is pay per ride.
        widgets[WIDX_PRICE_LABEL].tooltip = STR_NONE;
        widgets[WIDX_PRICE].tooltip = STR_NONE;

        if (!ParkEntranceFeeUnlocked())
        {
            widgets[WIDX_PRICE_LABEL].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
            widgets[WIDX_PRICE].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
        }

        // If the entry price is locked at free, disable the widget, unless the unlock_all_prices cheat is active.
        if ((gParkFlags & PARK_FLAGS_NO_MONEY) || !ParkEntranceFeeUnlocked())
        {
            widgets[WIDX_PRICE].type = WindowWidgetType::LabelCentred;
            widgets[WIDX_INCREASE_PRICE].type = WindowWidgetType::Empty;
            widgets[WIDX_DECREASE_PRICE].type = WindowWidgetType::Empty;
        }
        else
        {
            widgets[WIDX_PRICE].type = WindowWidgetType::Spinner;
            widgets[WIDX_INCREASE_PRICE].type = WindowWidgetType::Button;
            widgets[WIDX_DECREASE_PRICE].type = WindowWidgetType::Button;
        }

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawPrice(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto screenCoords = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 30 };
        auto ft = Formatter();
        ft.Add<money64>(gTotalIncomeFromAdmissions);
        DrawTextBasic(&dpi, screenCoords, STR_INCOME_FROM_ADMISSIONS, ft);

        money64 parkEntranceFee = ParkGetEntranceFee();
        auto stringId = parkEntranceFee == 0 ? STR_FREE : STR_BOTTOM_TOOLBAR_CASH;
        screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_PRICE].left + 1, widgets[WIDX_PRICE].top + 1 };
        ft = Formatter();
        ft.Add<money64>(parkEntranceFee);
        DrawTextBasic(&dpi, screenCoords, stringId, ft, { colours[1] });
    }
#pragma endregion

#pragma region Stats page
    void OnResizeStats()
    {
        window_set_resize(*this, 230, 119, 230, 119);
    }

    void OnUpdateStats()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_5);

        // Invalidate ride count if changed
        const auto rideCount = ride_get_count();
        if (_numberOfRides != rideCount)
        {
            _numberOfRides = rideCount;
            widget_invalidate(*this, WIDX_PAGE_BACKGROUND);
        }

        // Invalidate number of staff if changed
        const auto staffCount = peep_get_staff_count();
        if (_numberOfStaff != staffCount)
        {
            _numberOfStaff = staffCount;
            widget_invalidate(*this, WIDX_PAGE_BACKGROUND);
        }
    }

    void OnPrepareDrawStats()
    {
        auto* statsWidgets = _pagedWidgets[page];
        if (widgets != statsWidgets)
        {
            widgets = statsWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();
        PrepareWindowTitleText();

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawStats(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto screenCoords = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

        // Draw park size
        auto parkSize = gParkSize * 10;
        auto stringIndex = STR_PARK_SIZE_METRIC_LABEL;
        if (gConfigGeneral.MeasurementFormat == MeasurementFormat::Imperial)
        {
            stringIndex = STR_PARK_SIZE_IMPERIAL_LABEL;
            parkSize = squaredmetres_to_squaredfeet(parkSize);
        }
        auto ft = Formatter();
        ft.Add<uint32_t>(parkSize);
        DrawTextBasic(&dpi, screenCoords, stringIndex, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Draw number of rides / attractions
        if (_numberOfRides != -1)
        {
            ft = Formatter();
            ft.Add<uint32_t>(_numberOfRides);
            DrawTextBasic(&dpi, screenCoords, STR_NUMBER_OF_RIDES_LABEL, ft);
        }
        screenCoords.y += LIST_ROW_HEIGHT;

        // Draw number of staff
        if (_numberOfStaff != -1)
        {
            ft = Formatter();
            ft.Add<uint32_t>(_numberOfStaff);
            DrawTextBasic(&dpi, screenCoords, STR_STAFF_LABEL, ft);
        }
        screenCoords.y += LIST_ROW_HEIGHT;

        // Draw number of guests in park
        ft = Formatter();
        ft.Add<uint32_t>(gNumGuestsInPark);
        DrawTextBasic(&dpi, screenCoords, STR_GUESTS_IN_PARK_LABEL, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        ft = Formatter();
        ft.Add<uint32_t>(gTotalAdmissions);
        DrawTextBasic(&dpi, screenCoords, STR_TOTAL_ADMISSIONS, ft);
    }
#pragma endregion

#pragma region Objective page
    void OnMouseUpObjective(WidgetIndex widgetIndex)
    {
        switch (widgetIndex)
        {
            case WIDX_ENTER_NAME:
                WindowTextInputOpen(
                    this, WIDX_ENTER_NAME, STR_ENTER_NAME, STR_PLEASE_ENTER_YOUR_NAME_FOR_THE_SCENARIO_CHART, {}, 0, 0,
                    ParkNameMaxLength);
                break;
        }
    }

    void OnResizeObjective()
    {
#ifndef NO_TTF
        if (gCurrentTTFFontSet != nullptr)
            window_set_resize(*this, 230, 270, 230, 270);
        else
#endif
            window_set_resize(*this, 230, 226, 230, 226);
    }

    void OnUpdateObjective()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_6);
    }

    void OnTextInputObjective(WidgetIndex widgetIndex, std::string_view text)
    {
        if (widgetIndex == WIDX_ENTER_NAME && !text.empty())
        {
            std::string strText(text);
            scenario_success_submit_name(strText.c_str());
            Invalidate();
        }
    }

    void OnPrepareDrawObjective()
    {
        SetPressedTab();
        PrepareWindowTitleText();

        // Show name input button on scenario completion.
        if (gParkFlags & PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT)
        {
            widgets[WIDX_ENTER_NAME].type = WindowWidgetType::Button;
            widgets[WIDX_ENTER_NAME].top = height - 19;
            widgets[WIDX_ENTER_NAME].bottom = height - 6;
        }
        else
            widgets[WIDX_ENTER_NAME].type = WindowWidgetType::Empty;

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawObjective(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        // Scenario description
        auto screenCoords = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 7 };
        auto ft = Formatter();
        ft.Add<StringId>(STR_STRING);
        ft.Add<const char*>(gScenarioDetails.c_str());
        screenCoords.y += DrawTextWrapped(&dpi, screenCoords, 222, STR_BLACK_STRING, ft);
        screenCoords.y += 5;

        // Your objective:
        DrawTextBasic(&dpi, screenCoords, STR_OBJECTIVE_LABEL);
        screenCoords.y += LIST_ROW_HEIGHT;

        // Objective
        ft = Formatter();
        if (gScenarioObjective.Type == OBJECTIVE_BUILD_THE_BEST)
        {
            StringId rideTypeString = STR_NONE;
            auto rideTypeId = gScenarioObjective.RideId;
            if (rideTypeId != RIDE_TYPE_NULL && rideTypeId < RIDE_TYPE_COUNT)
            {
                rideTypeString = GetRideTypeDescriptor(rideTypeId).Naming.Name;
            }
            ft.Add<StringId>(rideTypeString);
        }
        else
        {
            ft.Add<uint16_t>(gScenarioObjective.NumGuests);
            ft.Add<int16_t>(date_get_total_months(MONTH_OCTOBER, gScenarioObjective.Year));
            if (gScenarioObjective.Type == OBJECTIVE_FINISH_5_ROLLERCOASTERS)
                ft.Add<uint16_t>(gScenarioObjective.MinimumExcitement);
            else
                ft.Add<money64>(gScenarioObjective.Currency);
        }

        screenCoords.y += DrawTextWrapped(&dpi, screenCoords, 221, ObjectiveNames[gScenarioObjective.Type], ft);
        screenCoords.y += 5;

        // Objective outcome
        if (gScenarioCompletedCompanyValue != MONEY64_UNDEFINED)
        {
            if (gScenarioCompletedCompanyValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
            {
                // Objective failed
                DrawTextWrapped(&dpi, screenCoords, 222, STR_OBJECTIVE_FAILED);
            }
            else
            {
                // Objective completed
                ft = Formatter();
                ft.Add<money64>(gScenarioCompletedCompanyValue);
                DrawTextWrapped(&dpi, screenCoords, 222, STR_OBJECTIVE_ACHIEVED, ft);
            }
        }
    }
#pragma endregion

#pragma region Awards page
    void OnResizeAwards()
    {
        window_set_resize(*this, 230, 182, 230, 182);
    }

    void OnUpdateAwards()
    {
        frame_no++;
        widget_invalidate(*this, WIDX_TAB_7);
    }

    void OnPrepareDrawAwards()
    {
        auto* awardsWidgets = _pagedWidgets[page];
        if (widgets != awardsWidgets)
        {
            widgets = awardsWidgets;
            InitScrollWidgets();
        }

        SetPressedTab();
        PrepareWindowTitleText();

        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_7);
        AnchorBorderWidgets();
    }

    void OnDrawAwards(rct_drawpixelinfo& dpi)
    {
        DrawWidgets(dpi);
        DrawTabImages(dpi);

        auto screenCoords = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

        for (const auto& award : GetAwards())
        {
            gfx_draw_sprite(&dpi, ImageId(_parkAwards[EnumValue(award.Type)].sprite), screenCoords);
            DrawTextWrapped(&dpi, screenCoords + ScreenCoordsXY{ 34, 6 }, 180, _parkAwards[EnumValue(award.Type)].text);

            screenCoords.y += 32;
        }

        if (GetAwards().empty())
            DrawTextBasic(&dpi, screenCoords + ScreenCoordsXY{ 6, 6 }, STR_NO_RECENT_AWARDS);
    }
#pragma endregion

#pragma region Common
    void SetPage(int32_t newPage)
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
            if (classification == gCurrentToolWidget.window_classification && number == gCurrentToolWidget.window_number)
                tool_cancel();

        // Set listen only to viewport
        bool listen = false;
        if (newPage == WINDOW_PARK_PAGE_ENTRANCE && viewport != nullptr && !(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
            listen = true;

        page = newPage;
        frame_no = 0;
        _peepAnimationFrame = 0;
        RemoveViewport();

        hold_down_widgets = _pagedHoldDownWidgets[newPage];
        widgets = _pagedWidgets[newPage];
        SetDisabledTabs();
        Invalidate();

        window_event_resize_call(this);
        window_event_invalidate_call(this);
        window_event_update_call(this);
        if (listen && viewport != nullptr)
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
    }

    void AnchorBorderWidgets()
    {
        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_PAGE_BACKGROUND].right = width - 1;
        widgets[WIDX_PAGE_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
    }

    void SetPressedTab()
    {
        for (int32_t i = WIDX_TAB_1; i <= WIDX_TAB_7; i++)
            pressed_widgets &= ~(1 << i);
        pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
    }

    void DrawTabImages(rct_drawpixelinfo& dpi)
    {
        // Entrance tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_1))
        {
            gfx_draw_sprite(
                &dpi, ImageId(SPR_TAB_PARK_ENTRANCE),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });
        }

        // Rating tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_2))
        {
            ImageId spriteIdx(SPR_TAB_GRAPH_0);
            if (page == WINDOW_PARK_PAGE_RATING)
                spriteIdx = spriteIdx.WithIndexOffset((frame_no / 8) % 8);
            gfx_draw_sprite(&dpi, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
            gfx_draw_sprite(
                &dpi, ImageId(SPR_RATING_HIGH),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left + 7, widgets[WIDX_TAB_2].top + 1 });
            gfx_draw_sprite(
                &dpi, ImageId(SPR_RATING_LOW),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left + 16, widgets[WIDX_TAB_2].top + 12 });
        }

        // Guests tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_3))
        {
            ImageId spriteIdx(SPR_TAB_GRAPH_0);
            if (page == WINDOW_PARK_PAGE_GUESTS)
                spriteIdx = spriteIdx.WithIndexOffset((frame_no / 8) % 8);
            gfx_draw_sprite(&dpi, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });

            ImageId peepImage(GetPeepAnimation(PeepSpriteType::Normal).base_image + 1, COLOUR_BRIGHT_RED, COLOUR_TEAL);
            if (page == WINDOW_PARK_PAGE_GUESTS)
                peepImage = peepImage.WithIndexOffset(_peepAnimationFrame & 0xFFFFFFFC);

            gfx_draw_sprite(
                &dpi, peepImage, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].midX(), widgets[WIDX_TAB_3].bottom - 9 });
        }

        // Price tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_4))
        {
            ImageId spriteIdx(SPR_TAB_ADMISSION_0);
            if (page == WINDOW_PARK_PAGE_PRICE)
                spriteIdx = spriteIdx.WithIndexOffset((frame_no / 2) % 8);
            gfx_draw_sprite(&dpi, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_4].left, widgets[WIDX_TAB_4].top });
        }

        // Statistics tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_5))
        {
            ImageId spriteIdx(SPR_TAB_STATS_0);
            if (page == WINDOW_PARK_PAGE_STATS)
                spriteIdx = spriteIdx.WithIndexOffset((frame_no / 4) % 7);
            gfx_draw_sprite(&dpi, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_5].left, widgets[WIDX_TAB_5].top });
        }

        // Objective tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_6))
        {
            ImageId spriteIdx(SPR_TAB_OBJECTIVE_0);
            if (page == WINDOW_PARK_PAGE_OBJECTIVE)
                spriteIdx = spriteIdx.WithIndexOffset((frame_no / 4) % 16);
            gfx_draw_sprite(&dpi, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_6].left, widgets[WIDX_TAB_6].top });
        }

        // Awards tab
        if (!WidgetIsDisabled(*this, WIDX_TAB_7))
        {
            gfx_draw_sprite(
                &dpi, ImageId(SPR_TAB_AWARDS), windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_7].left, widgets[WIDX_TAB_7].top });
        }
    }
};

static ParkWindow* ParkWindowOpen(uint8_t page)
{
    auto* wnd = WindowFocusOrCreate<ParkWindow>(WindowClass::ParkInformation, 230, 174 + 9, WF_10);
    if (wnd != nullptr && page != WINDOW_PARK_PAGE_ENTRANCE)
    {
        wnd->OnMouseUp(WIDX_TAB_1 + page);
    }
    return wnd;
}

/**
 *
 *  rct2: 0x00667C48
 */
rct_window* WindowParkEntranceOpen()
{
    return ParkWindowOpen(WINDOW_PARK_PAGE_ENTRANCE);
}

/**
 *
 *  rct2: 0x00667CA4
 */
rct_window* WindowParkRatingOpen()
{
    return ParkWindowOpen(WINDOW_PARK_PAGE_RATING);
}

/**
 *
 *  rct2: 0x00667D35
 */
rct_window* WindowParkGuestsOpen()
{
    return ParkWindowOpen(WINDOW_PARK_PAGE_GUESTS);
}

/**
 *
 *  rct2: 0x00667E57
 */
rct_window* WindowParkObjectiveOpen()
{
    auto* wnd = ParkWindowOpen(WINDOW_PARK_PAGE_OBJECTIVE);
    if (wnd != nullptr)
    {
        wnd->Invalidate();
        wnd->windowPos.x = context_get_width() / 2 - 115;
        wnd->windowPos.y = context_get_height() / 2 - 87;
        wnd->Invalidate();
    }
    return wnd;
}

/**
 *
 *  rct2: 0x00667DC6
 */
rct_window* WindowParkAwardsOpen()
{
    return ParkWindowOpen(WINDOW_PARK_PAGE_AWARDS);
}
