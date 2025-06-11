/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <array>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Graph.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Objective.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ParkSetEntranceFeeAction.h>
#include <openrct2/actions/ParkSetNameAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/localisation/Currency.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/management/Award.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_STRINGID;
    static constexpr int32_t kWindowHeight = 224;

    static constexpr ScreenCoordsXY kGraphTopLeftPadding{ 45, 20 };
    static constexpr ScreenCoordsXY kGraphBottomRightPadding{ 25, 10 };
    static constexpr uint8_t kGraphNumYLabels = 6;

    enum WindowParkPage
    {
        WINDOW_PARK_PAGE_ENTRANCE,
        WINDOW_PARK_PAGE_RATING,
        WINDOW_PARK_PAGE_GUESTS,
        WINDOW_PARK_PAGE_PRICE,
        WINDOW_PARK_PAGE_STATS,
        WINDOW_PARK_PAGE_OBJECTIVE,
        WINDOW_PARK_PAGE_AWARDS,
        WINDOW_PARK_PAGE_COUNT,
    };

    enum WindowParkWidgetIdx
    {
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

    // clang-format off
    static constexpr auto makeParkWidgets = [](int16_t width) {
        return makeWidgets(
            makeWindowShim(kWindowTitle, { width, kWindowHeight }),
            makeWidget({ 0, 43 }, { width, 131 }, WidgetType::resize, WindowColour::secondary),
            makeTab({ 3, 17 }, STR_PARK_ENTRANCE_TAB_TIP),
            makeTab({ 34, 17 }, STR_PARK_RATING_TAB_TIP),
            makeTab({ 65, 17 }, STR_PARK_GUESTS_TAB_TIP),
            makeTab({ 96, 17 }, STR_PARK_PRICE_TAB_TIP),
            makeTab({ 127, 17 }, STR_PARK_STATS_TAB_TIP),
            makeTab({ 158, 17 }, STR_PARK_OBJECTIVE_TAB_TIP),
            makeTab({ 189, 17 }, STR_PARK_AWARDS_TAB_TIP)
        );
    };

    static constexpr auto _entranceWidgets = makeWidgets(
        makeParkWidgets(230),
        makeWidget({  3,  46}, {202, 115}, WidgetType::viewport,      WindowColour::secondary                                                                      ), // viewport
        makeWidget({  3, 161}, {202,  11}, WidgetType::labelCentred,  WindowColour::secondary                                                                      ), // status
        makeWidget({205,  49}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, 0xFFFFFFFF,                 STR_OPEN_OR_CLOSE_PARK_TIP              ), // open / close
        makeWidget({205,  73}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_BUY_LAND_RIGHTS),        STR_BUY_LAND_AND_CONSTRUCTION_RIGHTS_TIP), // buy land rights
        makeWidget({205,  97}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_LOCATE),                 STR_LOCATE_SUBJECT_TIP                  ), // locate
        makeWidget({205, 121}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_RENAME),                 STR_NAME_PARK_TIP                       ), // rename
        makeWidget({210,  51}, { 14,  15}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_CLOSE_BUTTON_0), STR_CLOSE_PARK_TIP                      ),
        makeWidget({210,  66}, { 14,  14}, WidgetType::imgBtn,        WindowColour::secondary, ImageId(SPR_G2_RCT1_OPEN_BUTTON_0),  STR_OPEN_PARK_TIP                       )
    );

    static constexpr auto _ratingWidgets = makeWidgets(
        makeParkWidgets(255)
    );

    static constexpr auto _guestsWidgets = makeWidgets(
        makeParkWidgets(255)
    );

    static constexpr auto _priceWidgets = makeWidgets(
        makeParkWidgets(230),
        makeWidget        ({ 21, 50}, {126, 14}, WidgetType::label,   WindowColour::secondary, STR_ADMISSION_PRICE),
        makeSpinnerWidgets({147, 50}, { 76, 14}, WidgetType::spinner, WindowColour::secondary                     ) // Price (3 widgets)
    );

    static constexpr auto _statsWidgets = makeWidgets(
        makeParkWidgets(230)
    );

    static constexpr auto _objectiveWidgets = makeWidgets(
        makeParkWidgets(230),
        makeWidget({7, 207}, {216, 14}, WidgetType::button, WindowColour::secondary, STR_ENTER_NAME_INTO_SCENARIO_CHART) // enter name
    );

    static constexpr auto _awardsWidgets = makeWidgets(
        makeParkWidgets(230)
    );

    static std::span<const Widget> _pagedWidgets[] = {
        _entranceWidgets,
        _ratingWidgets,
        _guestsWidgets,
        _priceWidgets,
        _statsWidgets,
        _objectiveWidgets,
        _awardsWidgets,
    };
    // clang-format on

#pragma endregion

    // clang-format off
    static std::array<uint32_t, WINDOW_PARK_PAGE_COUNT> _pagedHoldDownWidgets = {
        0,
        0,
        0,
        (1uLL << WIDX_INCREASE_PRICE) |
        (1uLL << WIDX_DECREASE_PRICE),
        0,
        0,
        0,
    };

    struct WindowParkAward {
        StringId text;
        uint32_t sprite;
    };

    static constexpr WindowParkAward _parkAwards[] = {
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
        int32_t _numberOfStaff = -1;
        int32_t _numberOfRides = -1;
        uint8_t _peepAnimationFrame = 0;

        Graph::GraphProperties<uint16_t> _ratingProps{};
        Graph::GraphProperties<uint32_t> _guestProps{};

        ScreenRect _ratingGraphBounds;
        ScreenRect _guestGraphBounds;

    public:
        void OnOpen() override
        {
            number = 0;
            frame_no = 0;
            _numberOfRides = -1;
            _numberOfStaff = -1;
            _peepAnimationFrame = 0;
            SetPage(0);

            _ratingProps.lineCol = colours[2];
            _guestProps.lineCol = colours[2];
            _ratingProps.hoverIdx = -1;
            _guestProps.hoverIdx = -1;
        }

        void OnClose() override
        {
            if (isToolActive(classification, number))
            {
                ToolCancel();
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
                case WINDOW_PARK_PAGE_PRICE:
                    OnTextInputPrice(widgetIndex, text);
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

        void OnDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_PARK_PAGE_ENTRANCE:
                    OnDrawEntrance(rt);
                    break;
                case WINDOW_PARK_PAGE_RATING:
                    OnDrawRating(rt);
                    break;
                case WINDOW_PARK_PAGE_GUESTS:
                    OnDrawGuests(rt);
                    break;
                case WINDOW_PARK_PAGE_PRICE:
                    OnDrawPrice(rt);
                    break;
                case WINDOW_PARK_PAGE_STATS:
                    OnDrawStats(rt);
                    break;
                case WINDOW_PARK_PAGE_OBJECTIVE:
                    OnDrawObjective(rt);
                    break;
                case WINDOW_PARK_PAGE_AWARDS:
                    OnDrawAwards(rt);
                    break;
            }
        }

        void OnViewportRotate() override
        {
            if (page == WINDOW_PARK_PAGE_ENTRANCE)
            {
                InitViewport();
            }
        }

    private:
        void SetDisabledTabs()
        {
            // Disable price tab if money is disabled
            disabled_widgets = (getGameState().park.Flags & PARK_FLAGS_NO_MONEY) ? (1uLL << WIDX_TAB_4) : 0;
        }

        void PrepareWindowTitleText()
        {
            auto parkName = getGameState().park.Name.c_str();

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
                    ContextOpenWindow(WindowClass::LandRights);
                    break;
                case WIDX_LOCATE:
                    ScrollToViewport();
                    break;
                case WIDX_RENAME:
                {
                    auto& park = getGameState().park;
                    WindowTextInputRawOpen(
                        this, WIDX_RENAME, STR_PARK_NAME, STR_ENTER_PARK_NAME, {}, park.Name.c_str(), kUserStringMaxLength);
                    break;
                }
                case WIDX_CLOSE_LIGHT:
                    Park::SetOpen(false);
                    break;
                case WIDX_OPEN_LIGHT:
                    Park::SetOpen(true);
                    break;
            }
        }

        void OnResizeEntrance()
        {
            flags |= WF_RESIZABLE;
            WindowSetResize(*this, { 230, 174 + 9 }, { 230 * 3, (274 + 9) * 3 });
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

                if (getGameState().park.IsOpen())
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
                    Park::SetOpen(true);
                }
                else
                {
                    Park::SetOpen(false);
                }
            }
        }

        void OnUpdateEntrance()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_1);
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
            const auto& gameState = getGameState();
            InitScrollWidgets();

            SetPressedTab();

            // Set open / close park button state
            {
                auto parkName = gameState.park.Name.c_str();

                auto ft = Formatter::Common();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(parkName);
            }
            const bool parkIsOpen = gameState.park.IsOpen();
            widgets[WIDX_OPEN_OR_CLOSE].image = ImageId(parkIsOpen ? SPR_OPEN : SPR_CLOSED);
            const auto closeLightImage = SPR_G2_RCT1_CLOSE_BUTTON_0 + !parkIsOpen * 2
                + widgetIsPressed(*this, WIDX_CLOSE_LIGHT);
            widgets[WIDX_CLOSE_LIGHT].image = ImageId(closeLightImage);
            const auto openLightImage = SPR_G2_RCT1_OPEN_BUTTON_0 + parkIsOpen * 2 + widgetIsPressed(*this, WIDX_OPEN_LIGHT);
            widgets[WIDX_OPEN_LIGHT].image = ImageId(openLightImage);

            // Only allow closing of park for guest / rating objective
            if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
                disabled_widgets |= (1uLL << WIDX_OPEN_OR_CLOSE) | (1uLL << WIDX_CLOSE_LIGHT) | (1uLL << WIDX_OPEN_LIGHT);
            else
                disabled_widgets &= ~((1uLL << WIDX_OPEN_OR_CLOSE) | (1uLL << WIDX_CLOSE_LIGHT) | (1uLL << WIDX_OPEN_LIGHT));

            // Only allow purchase of land when there is money
            if (gameState.park.Flags & PARK_FLAGS_NO_MONEY)
                widgets[WIDX_BUY_LAND_RIGHTS].type = WidgetType::empty;
            else
                widgets[WIDX_BUY_LAND_RIGHTS].type = WidgetType::flatBtn;

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);

            // Anchor entrance page specific widgets
            widgets[WIDX_VIEWPORT].right = width - 26;
            widgets[WIDX_VIEWPORT].bottom = height - 14;
            widgets[WIDX_STATUS].right = width - 26;
            widgets[WIDX_STATUS].top = height - 13;
            widgets[WIDX_STATUS].bottom = height - 3;

            auto y = 0;
            if (ThemeGetFlags() & UITHEME_FLAG_USE_LIGHTS_PARK)
            {
                widgets[WIDX_OPEN_OR_CLOSE].type = WidgetType::empty;
                if (gameState.scenarioObjective.Type == OBJECTIVE_GUESTS_AND_RATING)
                {
                    widgets[WIDX_CLOSE_LIGHT].type = WidgetType::flatBtn;
                    widgets[WIDX_OPEN_LIGHT].type = WidgetType::flatBtn;
                }
                else
                {
                    widgets[WIDX_CLOSE_LIGHT].type = WidgetType::imgBtn;
                    widgets[WIDX_OPEN_LIGHT].type = WidgetType::imgBtn;
                }
                y = widgets[WIDX_OPEN_LIGHT].bottom + 5;
            }
            else
            {
                widgets[WIDX_OPEN_OR_CLOSE].type = WidgetType::flatBtn;
                widgets[WIDX_CLOSE_LIGHT].type = WidgetType::empty;
                widgets[WIDX_OPEN_LIGHT].type = WidgetType::empty;
                y = widgets[WIDX_PAGE_BACKGROUND].top + 6;
            }

            for (int32_t i = WIDX_CLOSE_LIGHT; i <= WIDX_OPEN_LIGHT; i++)
            {
                widgets[i].left = width - 20;
                widgets[i].right = width - 7;
            }
            for (int32_t i = WIDX_OPEN_OR_CLOSE; i <= WIDX_RENAME; i++)
            {
                if (widgets[i].type == WidgetType::empty)
                    continue;

                widgets[i].left = width - 25;
                widgets[i].right = width - 2;
                widgets[i].top = y;
                widgets[i].bottom = y + 23;
                y += 24;
            }
        }

        void OnDrawEntrance(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            // Draw viewport
            if (viewport != nullptr)
            {
                WindowDrawViewport(rt, *this);
                if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                    GfxDrawSprite(rt, ImageId(SPR_HEARING_VIEWPORT), WindowGetViewportSoundIconPos(*this));
            }

            // Draw park closed / open label
            auto ft = Formatter();
            ft.Add<StringId>(getGameState().park.IsOpen() ? STR_PARK_OPEN : STR_PARK_CLOSED);

            auto* labelWidget = &widgets[WIDX_STATUS];
            DrawTextEllipsised(
                rt, windowPos + ScreenCoordsXY{ labelWidget->midX(), labelWidget->top }, labelWidget->width(), STR_BLACK_STRING,
                ft, { TextAlignment::CENTRE });
        }

        void InitViewport()
        {
            if (page != WINDOW_PARK_PAGE_ENTRANCE)
                return;

            const auto& gameState = getGameState();

            std::optional<Focus> newFocus = std::nullopt;
            if (!gameState.park.Entrances.empty())
            {
                const auto& entrance = gameState.park.Entrances[0];
                newFocus = Focus(CoordsXYZ{ entrance.x + 16, entrance.y + 16, entrance.z + 32 });
            }

            int32_t viewportFlags{};
            if (viewport == nullptr)
            {
                viewportFlags = Config::Get().general.AlwaysShowGridlines ? VIEWPORT_FLAG_GRIDLINES : VIEWPORT_FLAG_NONE;
            }
            else
            {
                viewportFlags = viewport->flags;
                RemoveViewport();
            }

            // Call invalidate event
            OnPrepareDraw();

            focus = newFocus;

            if (focus.has_value())
            {
                // Create viewport
                if (viewport == nullptr)
                {
                    Widget* viewportWidget = &widgets[WIDX_VIEWPORT];
                    ViewportCreate(
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
            flags |= WF_RESIZABLE;
            WindowSetResize(*this, { 268, 174 + 9 }, kMaxWindowSize);
        }

        void OnUpdateRating()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_2);
            if (_ratingProps.UpdateHoverIndex())
            {
                InvalidateWidget(WIDX_BACKGROUND);
            }
        }

        void OnPrepareDrawRating()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);

            _ratingProps.min = 0;
            _ratingProps.max = 1000;
            _ratingProps.series = getGameState().park.RatingHistory;
            const Widget* background = &widgets[WIDX_PAGE_BACKGROUND];
            _ratingGraphBounds = { windowPos + ScreenCoordsXY{ background->left + 4, background->top + 15 },
                                   windowPos + ScreenCoordsXY{ background->right - 4, background->bottom - 4 } };

            char buffer[64]{};
            FormatStringToBuffer(buffer, sizeof(buffer), "{BLACK}{COMMA32}", _ratingProps.max);
            int32_t maxWidth = GfxGetStringWidth(buffer, FontStyle::Small) + Graph::kYTickMarkPadding + 1;
            const ScreenCoordsXY dynamicPadding{ std::max(maxWidth, kGraphTopLeftPadding.x), kGraphTopLeftPadding.y };

            _ratingProps.RecalculateLayout(
                { _ratingGraphBounds.Point1 + dynamicPadding, _ratingGraphBounds.Point2 - kGraphBottomRightPadding },
                kGraphNumYLabels, kParkRatingHistorySize);
        }

        void OnDrawRating(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            Widget* widget = &widgets[WIDX_PAGE_BACKGROUND];

            // Current value
            Formatter ft;
            ft.Add<uint16_t>(getGameState().park.Rating);
            DrawTextBasic(rt, windowPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_PARK_RATING_LABEL, ft);

            // Graph border
            GfxFillRectInset(rt, _ratingGraphBounds, colours[1], INSET_RECT_F_30);
            // hide resize widget on graph area
            constexpr ScreenCoordsXY offset{ 1, 1 };
            constexpr ScreenCoordsXY bigOffset{ 5, 5 };
            GfxFillRectInset(
                rt, { _ratingGraphBounds.Point2 - bigOffset, _ratingGraphBounds.Point2 - offset }, colours[1],
                INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_BORDER_NONE);

            Graph::DrawRatingGraph(rt, _ratingProps);
        }

#pragma endregion

#pragma region Guests page
        void OnResizeGuests()
        {
            flags |= WF_RESIZABLE;
            WindowSetResize(*this, { 268, 174 + 9 }, kMaxWindowSize);
        }

        void OnUpdateGuests()
        {
            frame_no++;
            _peepAnimationFrame = (_peepAnimationFrame + 1) % 24;
            InvalidateWidget(WIDX_TAB_3);
            if (_guestProps.UpdateHoverIndex())
            {
                InvalidateWidget(WIDX_BACKGROUND);
            }
        }

        void OnPrepareDrawGuests()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);

            const auto& gameState = getGameState();
            _guestProps.series = gameState.guestsInParkHistory;
            const Widget* background = &widgets[WIDX_PAGE_BACKGROUND];
            _guestGraphBounds = { windowPos + ScreenCoordsXY{ background->left + 4, background->top + 15 },
                                  windowPos + ScreenCoordsXY{ background->right - 4, background->bottom - 4 } };

            // Calculate Y axis max and min
            _guestProps.min = 0;
            _guestProps.max = 5000;
            for (size_t i = 0; i < std::size(gameState.guestsInParkHistory); i++)
            {
                auto value = gameState.guestsInParkHistory[i];
                if (value == kGuestsInParkHistoryUndefined)
                    continue;
                while (value > _guestProps.max)
                    _guestProps.max += 5000;
            }

            char buffer[64]{};
            FormatStringToBuffer(buffer, sizeof(buffer), "{BLACK}{COMMA32}", _guestProps.max);
            int32_t maxWidth = GfxGetStringWidth(buffer, FontStyle::Small) + Graph::kYTickMarkPadding + 1;
            const ScreenCoordsXY dynamicPadding{ std::max(maxWidth, kGraphTopLeftPadding.x), kGraphTopLeftPadding.y };

            _guestProps.RecalculateLayout(
                { _guestGraphBounds.Point1 + dynamicPadding, _guestGraphBounds.Point2 - kGraphBottomRightPadding },
                kGraphNumYLabels, kGuestsInParkHistorySize);
        }

        void OnDrawGuests(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            Widget* widget = &widgets[WIDX_PAGE_BACKGROUND];

            // Current value
            Formatter ft;
            ft.Add<uint32_t>(getGameState().numGuestsInPark);
            DrawTextBasic(rt, windowPos + ScreenCoordsXY{ widget->left + 3, widget->top + 2 }, STR_GUESTS_IN_PARK_LABEL, ft);

            // Graph border
            GfxFillRectInset(rt, _guestGraphBounds, colours[1], INSET_RECT_F_30);
            // hide resize widget on graph area
            constexpr ScreenCoordsXY offset{ 1, 1 };
            constexpr ScreenCoordsXY bigOffset{ 5, 5 };
            GfxFillRectInset(
                rt, { _guestGraphBounds.Point2 - bigOffset, _guestGraphBounds.Point2 - offset }, colours[1],
                INSET_RECT_FLAG_FILL_DONT_LIGHTEN | INSET_RECT_FLAG_BORDER_NONE);

            Graph::DrawGuestGraph(rt, _guestProps);
        }

#pragma endregion

#pragma region Price page
        void OnResizePrice()
        {
            WindowSetResize(*this, { 230, 124 }, { 230, 124 });
        }

        void OnMouseDownPrice(WidgetIndex widgetIndex)
        {
            const auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_INCREASE_PRICE:
                {
                    const auto newFee = std::min(kMaxEntranceFee, gameState.park.EntranceFee + 1.00_GBP);
                    auto gameAction = ParkSetEntranceFeeAction(newFee);
                    GameActions::Execute(&gameAction);
                    break;
                }
                case WIDX_DECREASE_PRICE:
                {
                    const auto newFee = std::max(0.00_GBP, gameState.park.EntranceFee - 1.00_GBP);
                    auto gameAction = ParkSetEntranceFeeAction(newFee);
                    GameActions::Execute(&gameAction);
                    break;
                }
                case WIDX_PRICE:
                {
                    utf8 _moneyInputText[kMoneyStringMaxlength] = {};
                    MoneyToString(Park::GetEntranceFee(), _moneyInputText, kMoneyStringMaxlength, false);
                    WindowTextInputRawOpen(
                        this, WIDX_PRICE, STR_ENTER_NEW_VALUE, STR_ENTER_NEW_VALUE, {}, _moneyInputText, kMoneyStringMaxlength);
                }
            }
        }

        void OnUpdatePrice()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_4);
        }

        void OnPrepareDrawPrice()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            // Show a tooltip if the park is pay per ride.
            widgets[WIDX_PRICE_LABEL].tooltip = kStringIdNone;
            widgets[WIDX_PRICE].tooltip = kStringIdNone;

            if (!Park::EntranceFeeUnlocked())
            {
                widgets[WIDX_PRICE_LABEL].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
                widgets[WIDX_PRICE].tooltip = STR_ADMISSION_PRICE_PAY_PER_RIDE_TIP;
            }

            // If the entry price is locked at free, disable the widget, unless the unlock_all_prices cheat is active.
            if ((getGameState().park.Flags & PARK_FLAGS_NO_MONEY) || !Park::EntranceFeeUnlocked())
            {
                widgets[WIDX_PRICE].type = WidgetType::labelCentred;
                widgets[WIDX_INCREASE_PRICE].type = WidgetType::empty;
                widgets[WIDX_DECREASE_PRICE].type = WidgetType::empty;
            }
            else
            {
                widgets[WIDX_PRICE].type = WidgetType::spinner;
                widgets[WIDX_INCREASE_PRICE].type = WidgetType::button;
                widgets[WIDX_DECREASE_PRICE].type = WidgetType::button;
            }

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);
        }

        void OnDrawPrice(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 30 };
            auto ft = Formatter();
            ft.Add<money64>(getGameState().totalIncomeFromAdmissions);
            DrawTextBasic(rt, screenCoords, STR_INCOME_FROM_ADMISSIONS, ft);

            money64 parkEntranceFee = Park::GetEntranceFee();
            ft = Formatter();
            ft.Add<money64>(parkEntranceFee);

            StringId stringId = STR_BOTTOM_TOOLBAR_CASH;
            if (parkEntranceFee == 0)
                stringId = STR_FREE;

            screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_PRICE].left + 1, widgets[WIDX_PRICE].top + 1 };
            DrawTextBasic(rt, screenCoords, stringId, ft, { colours[1] });
        }
#pragma endregion

#pragma region Stats page
        void OnResizeStats()
        {
            WindowSetResize(*this, { 230, 119 }, { 230, 119 });
        }

        void OnUpdateStats()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_5);

            // Invalidate ride count if changed
            const auto rideCount = RideGetCount();
            if (_numberOfRides != rideCount)
            {
                _numberOfRides = rideCount;
                InvalidateWidget(WIDX_PAGE_BACKGROUND);
            }

            // Invalidate number of staff if changed
            const auto staffCount = PeepGetStaffCount();
            if (_numberOfStaff != staffCount)
            {
                _numberOfStaff = staffCount;
                InvalidateWidget(WIDX_PAGE_BACKGROUND);
            }
        }

        void OnPrepareDrawStats()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);
        }

        void OnDrawStats(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            auto& gameState = getGameState();
            // Draw park size
            auto parkSize = gameState.park.Size * 10;
            auto stringIndex = STR_PARK_SIZE_METRIC_LABEL;
            if (Config::Get().general.MeasurementFormat == MeasurementFormat::Imperial)
            {
                stringIndex = STR_PARK_SIZE_IMPERIAL_LABEL;
                parkSize = SquaredMetresToSquaredFeet(parkSize);
            }
            auto ft = Formatter();
            ft.Add<uint32_t>(parkSize);
            DrawTextBasic(rt, screenCoords, stringIndex, ft);
            screenCoords.y += kListRowHeight;

            // Draw number of rides / attractions
            if (_numberOfRides != -1)
            {
                ft = Formatter();
                ft.Add<uint32_t>(_numberOfRides);
                DrawTextBasic(rt, screenCoords, STR_NUMBER_OF_RIDES_LABEL, ft);
            }
            screenCoords.y += kListRowHeight;

            // Draw number of staff
            if (_numberOfStaff != -1)
            {
                ft = Formatter();
                ft.Add<uint32_t>(_numberOfStaff);
                DrawTextBasic(rt, screenCoords, STR_STAFF_LABEL, ft);
            }
            screenCoords.y += kListRowHeight;

            // Draw number of guests in park
            ft = Formatter();
            ft.Add<uint32_t>(gameState.numGuestsInPark);
            DrawTextBasic(rt, screenCoords, STR_GUESTS_IN_PARK_LABEL, ft);
            screenCoords.y += kListRowHeight;

            ft = Formatter();
            ft.Add<uint32_t>(gameState.totalAdmissions);
            DrawTextBasic(rt, screenCoords, STR_TOTAL_ADMISSIONS, ft);
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
                        kParkNameMaxLength);
                    break;
            }
        }

        void OnResizeObjective()
        {
#ifndef DISABLE_TTF
            if (gCurrentTTFFontSet != nullptr)
                WindowSetResize(*this, { 230, 270 }, { 230, 270 });
            else
#endif
                WindowSetResize(*this, { 230, 226 }, { 230, 226 });
        }

        void OnUpdateObjective()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_6);
        }

        void OnTextInputObjective(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex == WIDX_ENTER_NAME && !text.empty())
            {
                std::string strText(text);
                ScenarioSuccessSubmitName(getGameState(), strText.c_str());
                Invalidate();
            }
        }

        void OnTextInputPrice(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex == WIDX_PRICE && !text.empty())
            {
                std::string strText(text);
                auto money = StringToMoney(strText.c_str());
                if (money == kMoney64Undefined)
                {
                    return;
                }

                money = std::clamp(money, 0.00_GBP, kMaxEntranceFee);
                auto gameAction = ParkSetEntranceFeeAction(money);
                GameActions::Execute(&gameAction);
            }
        }

        void OnPrepareDrawObjective()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            // Show name input button on scenario completion.
            if (getGameState().park.Flags & PARK_FLAGS_SCENARIO_COMPLETE_NAME_INPUT)
            {
                widgets[WIDX_ENTER_NAME].type = WidgetType::button;
                widgets[WIDX_ENTER_NAME].top = height - 19;
                widgets[WIDX_ENTER_NAME].bottom = height - 6;
            }
            else
                widgets[WIDX_ENTER_NAME].type = WidgetType::empty;

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);
        }

        void OnDrawObjective(RenderTarget& rt)
        {
            auto& gameState = getGameState();
            DrawWidgets(rt);
            DrawTabImages(rt);

            // Scenario description
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 7 };
            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(gameState.scenarioDetails.c_str());
            screenCoords.y += DrawTextWrapped(rt, screenCoords, 222, STR_BLACK_STRING, ft);
            screenCoords.y += 5;

            // Your objective:
            DrawTextBasic(rt, screenCoords, STR_OBJECTIVE_LABEL);
            screenCoords.y += kListRowHeight;

            // Objective
            ft = Formatter();
            formatObjective(ft, gameState.scenarioObjective);

            screenCoords.y += DrawTextWrapped(rt, screenCoords, 221, kObjectiveNames[gameState.scenarioObjective.Type], ft);
            screenCoords.y += 5;

            // Objective outcome
            if (gameState.scenarioCompletedCompanyValue != kMoney64Undefined)
            {
                if (gameState.scenarioCompletedCompanyValue == kCompanyValueOnFailedObjective)
                {
                    // Objective failed
                    DrawTextWrapped(rt, screenCoords, 222, STR_OBJECTIVE_FAILED);
                }
                else
                {
                    // Objective completed
                    ft = Formatter();
                    ft.Add<money64>(gameState.scenarioCompletedCompanyValue);
                    DrawTextWrapped(rt, screenCoords, 222, STR_OBJECTIVE_ACHIEVED, ft);
                }
            }
        }
#pragma endregion

#pragma region Awards page
        void OnResizeAwards()
        {
            WindowSetResize(*this, { 230, 182 }, { 230, 182 });
        }

        void OnUpdateAwards()
        {
            frame_no++;
            InvalidateWidget(WIDX_TAB_7);
        }

        void OnPrepareDrawAwards()
        {
            SetPressedTab();
            PrepareWindowTitleText();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);
        }

        void OnDrawAwards(RenderTarget& rt)
        {
            DrawWidgets(rt);
            DrawTabImages(rt);

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            auto& currentAwards = getGameState().currentAwards;

            for (const auto& award : currentAwards)
            {
                GfxDrawSprite(rt, ImageId(_parkAwards[EnumValue(award.Type)].sprite), screenCoords);
                DrawTextWrapped(rt, screenCoords + ScreenCoordsXY{ 34, 6 }, 180, _parkAwards[EnumValue(award.Type)].text);

                screenCoords.y += 32;
            }

            if (currentAwards.empty())
                DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 6, 6 }, STR_NO_RECENT_AWARDS);
        }
#pragma endregion

#pragma region Common
        void SetPage(int32_t newPage)
        {
            if (isToolActive(classification, number))
                ToolCancel();

            // Set listen only to viewport
            bool listen = false;
            if (newPage == WINDOW_PARK_PAGE_ENTRANCE && page == WINDOW_PARK_PAGE_ENTRANCE && viewport != nullptr)
            {
                viewport->flags ^= VIEWPORT_FLAG_SOUND_ON;
                listen = (viewport->flags & VIEWPORT_FLAG_SOUND_ON) != 0;
            }

            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            frame_no = 0;
            _peepAnimationFrame = 0;
            RemoveViewport();

            hold_down_widgets = _pagedHoldDownWidgets[newPage];
            SetWidgets(_pagedWidgets[newPage]);
            SetDisabledTabs();
            Invalidate();
            InitScrollWidgets();

            if (page == WINDOW_PARK_PAGE_GUESTS || WINDOW_PARK_PAGE_RATING)
            {
                // We need to compensate for the enlarged title bar for windows that do not
                // constrain the window height between tabs (e.g. chart tabs)
                height -= getTitleBarDiffNormal();
            }

            OnResize();
            OnUpdate();
            ResizeFrame();

            if (listen && viewport != nullptr)
                viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }

        void SetPressedTab()
        {
            for (int32_t i = WIDX_TAB_1; i <= WIDX_TAB_7; i++)
                pressed_widgets &= ~(1 << i);
            pressed_widgets |= 1LL << (WIDX_TAB_1 + page);
        }

        void DrawTabImages(RenderTarget& rt)
        {
            // Entrance tab
            if (!widgetIsDisabled(*this, WIDX_TAB_1))
            {
                GfxDrawSprite(
                    rt, ImageId(SPR_TAB_PARK_ENTRANCE),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top });
            }

            // Rating tab
            if (!widgetIsDisabled(*this, WIDX_TAB_2))
            {
                ImageId spriteIdx(SPR_TAB_GRAPH_0);
                if (page == WINDOW_PARK_PAGE_RATING)
                    spriteIdx = spriteIdx.WithIndexOffset((frame_no / 8) % 8);
                GfxDrawSprite(rt, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left, widgets[WIDX_TAB_2].top });
                GfxDrawSprite(
                    rt, ImageId(SPR_RATING_HIGH),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left + 7, widgets[WIDX_TAB_2].top + 1 });
                GfxDrawSprite(
                    rt, ImageId(SPR_RATING_LOW),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_2].left + 16, widgets[WIDX_TAB_2].top + 12 });
            }

            // Guests tab
            if (!widgetIsDisabled(*this, WIDX_TAB_3))
            {
                ImageId spriteIdx(SPR_TAB_GRAPH_0);
                if (page == WINDOW_PARK_PAGE_GUESTS)
                    spriteIdx = spriteIdx.WithIndexOffset((frame_no / 8) % 8);
                GfxDrawSprite(rt, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].left, widgets[WIDX_TAB_3].top });

                auto* animObj = findPeepAnimationsObjectForType(AnimationPeepType::Guest);
                ImageId peepImage(
                    animObj->GetPeepAnimation(PeepAnimationGroup::Normal).base_image + 1, COLOUR_BRIGHT_RED, COLOUR_TEAL);
                if (page == WINDOW_PARK_PAGE_GUESTS)
                    peepImage = peepImage.WithIndexOffset(_peepAnimationFrame & 0xFFFFFFFC);

                GfxDrawSprite(
                    rt, peepImage, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_3].midX(), widgets[WIDX_TAB_3].bottom - 9 });
            }

            // Price tab
            if (!widgetIsDisabled(*this, WIDX_TAB_4))
            {
                ImageId spriteIdx(SPR_TAB_ADMISSION_0);
                if (page == WINDOW_PARK_PAGE_PRICE)
                    spriteIdx = spriteIdx.WithIndexOffset((frame_no / 2) % 8);
                GfxDrawSprite(rt, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_4].left, widgets[WIDX_TAB_4].top });
            }

            // Statistics tab
            if (!widgetIsDisabled(*this, WIDX_TAB_5))
            {
                ImageId spriteIdx(SPR_TAB_STATS_0);
                if (page == WINDOW_PARK_PAGE_STATS)
                    spriteIdx = spriteIdx.WithIndexOffset((frame_no / 4) % 7);
                GfxDrawSprite(rt, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_5].left, widgets[WIDX_TAB_5].top });
            }

            // Objective tab
            if (!widgetIsDisabled(*this, WIDX_TAB_6))
            {
                ImageId spriteIdx(SPR_TAB_OBJECTIVE_0);
                if (page == WINDOW_PARK_PAGE_OBJECTIVE)
                    spriteIdx = spriteIdx.WithIndexOffset((frame_no / 4) % 16);
                GfxDrawSprite(rt, spriteIdx, windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_6].left, widgets[WIDX_TAB_6].top });
            }

            // Awards tab
            if (!widgetIsDisabled(*this, WIDX_TAB_7))
            {
                GfxDrawSprite(
                    rt, ImageId(SPR_TAB_AWARDS),
                    windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_7].left, widgets[WIDX_TAB_7].top });
            }
        }
#pragma endregion
    };

    static ParkWindow* ParkWindowOpen(uint8_t page)
    {
        auto* windowMgr = GetWindowManager();
        auto* wnd = windowMgr->FocusOrCreate<ParkWindow>(WindowClass::ParkInformation, { 230, 174 + 9 }, WF_10);
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
    WindowBase* ParkEntranceOpen()
    {
        return ParkWindowOpen(WINDOW_PARK_PAGE_ENTRANCE);
    }

    /**
     *
     *  rct2: 0x00667CA4
     */
    WindowBase* ParkRatingOpen()
    {
        return ParkWindowOpen(WINDOW_PARK_PAGE_RATING);
    }

    /**
     *
     *  rct2: 0x00667D35
     */
    WindowBase* ParkGuestsOpen()
    {
        return ParkWindowOpen(WINDOW_PARK_PAGE_GUESTS);
    }

    /**
     *
     *  rct2: 0x00667E57
     */
    WindowBase* ParkObjectiveOpen()
    {
        auto* wnd = ParkWindowOpen(WINDOW_PARK_PAGE_OBJECTIVE);
        if (wnd != nullptr)
        {
            wnd->Invalidate();
            wnd->windowPos.x = ContextGetWidth() / 2 - 115;
            wnd->windowPos.y = ContextGetHeight() / 2 - 87;
            wnd->Invalidate();
        }
        return wnd;
    }

    /**
     *
     *  rct2: 0x00667DC6
     */
    WindowBase* ParkAwardsOpen()
    {
        return ParkWindowOpen(WINDOW_PARK_PAGE_AWARDS);
    }
} // namespace OpenRCT2::Ui::Windows
