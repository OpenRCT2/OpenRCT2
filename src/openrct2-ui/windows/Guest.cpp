/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/ViewportQuery.h"

#include <array>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/GuestSetFlagsAction.h>
#include <openrct2/actions/GuestSetNameAction.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/management/Marketing.h>
#include <openrct2/network/network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepSpriteIds.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_STRINGID;
    static constexpr int32_t WH = 157;
    static constexpr int32_t WW = 192;

    enum WindowGuestPage
    {
        WINDOW_GUEST_OVERVIEW,
        WINDOW_GUEST_STATS,
        WINDOW_GUEST_RIDES,
        WINDOW_GUEST_FINANCE,
        WINDOW_GUEST_THOUGHTS,
        WINDOW_GUEST_INVENTORY,
        WINDOW_GUEST_DEBUG,
        WINDOW_GUEST_PAGE_COUNT,
    };

    enum WindowGuestWidgetIdx
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

        WIDX_TAB_CONTENT_START,

        WIDX_MARQUEE = WIDX_TAB_CONTENT_START,
        WIDX_VIEWPORT,
        WIDX_ACTION_LBL,
        WIDX_PICKUP,
        WIDX_RENAME,
        WIDX_LOCATE,
        WIDX_TRACK,

        WIDX_HAPPINESS_BAR = WIDX_TAB_CONTENT_START,
        WIDX_ENERGY_BAR,
        WIDX_HUNGER_BAR,
        WIDX_THIRST_BAR,
        WIDX_NAUSEA_BAR,
        WIDX_TOILET_BAR,

        WIDX_RIDE_SCROLL = WIDX_TAB_CONTENT_START,
    };

    validate_global_widx(WC_PEEP, WIDX_PICKUP);

    static constexpr int32_t TabWidth = 30;

#define MAIN_GUEST_WIDGETS                                                                                                     \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),                                                                                         \
        MakeWidget({ 0, 43 }, { 192, 114 }, WindowWidgetType::Resize, WindowColour::Secondary), /* Resize */                   \
        MakeTab({ 3, 17 }, STR_SHOW_GUEST_VIEW_TIP),                                            /* Tab 1 */                    \
        MakeTab({ 34, 17 }, STR_SHOW_GUEST_NEEDS_TIP),                                          /* Tab 2 */                    \
        MakeTab({ 65, 17 }, STR_SHOW_GUEST_VISITED_RIDES_TIP),                                  /* Tab 3 */                    \
        MakeTab({ 96, 17 }, STR_SHOW_GUEST_FINANCE_TIP),                                        /* Tab 4 */                    \
        MakeTab({ 127, 17 }, STR_SHOW_GUEST_THOUGHTS_TIP),                                      /* Tab 5 */                    \
        MakeTab({ 158, 17 }, STR_SHOW_GUEST_ITEMS_TIP),                                         /* Tab 6 */                    \
        MakeTab({ 189, 17 }, STR_DEBUG_TIP)                                                     /* Tab 7 */

    // clang-format off
    static constexpr Widget _guestWindowWidgetsOverview[] = {
        MAIN_GUEST_WIDGETS,
        MakeWidget({  3,  45}, {164, 12}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                               ), // Label Thought marquee
        MakeWidget({  3,  57}, {164, 87}, WindowWidgetType::Viewport,      WindowColour::Secondary                                               ), // Viewport
        MakeWidget({  3, 144}, {164, 11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                               ), // Label Action
        MakeWidget({167,  45}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_PICKUP_BTN), STR_PICKUP_TIP               ), // Pickup Button
        MakeWidget({167,  69}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_RENAME),     STR_NAME_GUEST_TIP           ), // Rename Button
        MakeWidget({167,  93}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_LOCATE),     STR_LOCATE_SUBJECT_TIP       ), // Locate Button
        MakeWidget({167, 117}, { 24, 24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, ImageId(SPR_TRACK_PEEP), STR_TOGGLE_GUEST_TRACKING_TIP), // Track Button
    };
    // clang-format on

    static constexpr Widget _guestWindowWidgetsStats[] = {
        MAIN_GUEST_WIDGETS,
        MakeProgressBar({ 65, (kListRowHeight * 0) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_GREEN, 0, 19), // Happiness
        MakeProgressBar({ 65, (kListRowHeight * 1) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_GREEN, 0, 19), // Energy
        MakeProgressBar({ 65, (kListRowHeight * 2) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_RED, 67, 100), // Hunger
        MakeProgressBar({ 65, (kListRowHeight * 3) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_RED, 67, 100), // Thirst
        MakeProgressBar({ 65, (kListRowHeight * 4) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_RED, 47, 100), // Nausea
        MakeProgressBar({ 65, (kListRowHeight * 5) + 4 + 43 }, { 119, 10 }, COLOUR_BRIGHT_RED, 62, 100), // Toilet
    };

    static constexpr Widget _guestWindowWidgetsRides[] = {
        MAIN_GUEST_WIDGETS,
        MakeWidget({ 3, 57 }, { 186, 87 }, WindowWidgetType::Scroll, WindowColour::Secondary, SCROLL_VERTICAL),
    };

    static constexpr Widget _guestWindowWidgetsFinance[] = {
        MAIN_GUEST_WIDGETS,
    };

    static constexpr Widget _guestWindowWidgetsThoughts[] = {
        MAIN_GUEST_WIDGETS,
    };

    static constexpr Widget _guestWindowWidgetsInventory[] = {
        MAIN_GUEST_WIDGETS,
    };

    static constexpr Widget _guestWindowWidgetsDebug[] = {
        MAIN_GUEST_WIDGETS,
    };

    // clang-format off
    static constexpr std::span<const Widget> _guestWindowPageWidgets[] = {
        _guestWindowWidgetsOverview,
        _guestWindowWidgetsStats,
        _guestWindowWidgetsRides,
        _guestWindowWidgetsFinance,
        _guestWindowWidgetsThoughts,
        _guestWindowWidgetsInventory,
        _guestWindowWidgetsDebug,
    };
    static_assert(std::size(_guestWindowPageWidgets) == WINDOW_GUEST_PAGE_COUNT);
    // clang-format on

    static constexpr std::array _guestWindowPageSizes = {
        std::array{ ScreenSize{ 192, 159 }, ScreenSize{ 500, 450 } }, // WINDOW_GUEST_OVERVIEW
        std::array{ ScreenSize{ 192, 180 }, ScreenSize{ 192, 180 } }, // WINDOW_GUEST_STATS
        std::array{ ScreenSize{ 192, 180 }, ScreenSize{ 500, 400 } }, // WINDOW_GUEST_RIDES
        std::array{ ScreenSize{ 210, 148 }, ScreenSize{ 210, 148 } }, // WINDOW_GUEST_FINANCE
        std::array{ ScreenSize{ 192, 159 }, ScreenSize{ 500, 450 } }, // WINDOW_GUEST_THOUGHTS
        std::array{ ScreenSize{ 192, 159 }, ScreenSize{ 500, 450 } }, // WINDOW_GUEST_INVENTORY
        std::array{ ScreenSize{ 192, 171 }, ScreenSize{ 192, 171 } }, // WINDOW_GUEST_DEBUG
    };
    static_assert(_guestWindowPageSizes.size() == WINDOW_GUEST_PAGE_COUNT);

    class GuestWindow final : public Window
    {
    private:
        uint16_t _marqueePosition = 0;
        uint16_t _beingWatchedTimer = 0;
        uint16_t _guestAnimationFrame = 0;
        int16_t _pickedPeepX = kLocationNull; // entity->x gets set to 0x8000 on pickup, this is the old value
        std::vector<RideId> _riddenRides;

    public:
        void OnOpen() override
        {
            SetWidgets(_guestWindowWidgetsOverview);
            page = WINDOW_GUEST_OVERVIEW;
            frame_no = 0;
            _marqueePosition = 0;
            picked_peep_frame = 0;
            min_width = width;
            min_height = 157;
            max_width = 500;
            max_height = 450;
            selected_list_item = -1;
        }

        void Init(EntityId id)
        {
            number = id.ToUnderlying();
            page = -1; // Set Page to something invalid so that SetPage doesn't set audio on viewport
            SetPage(WINDOW_GUEST_OVERVIEW);
        }

        void OnClose() override
        {
            if (isToolActive(classification, number))
                ToolCancel();
        }

        void OnMouseUp(WidgetIndex widx) override
        {
            switch (widx)
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
                    SetPage(widx - WIDX_TAB_1);
                    return;
            }

            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    OnMouseUpOverview(widx);
                    break;
            }
        }
        void OnMouseDown(WidgetIndex widx) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnMouseDownOverview(widx);
            }
        }
        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnDropdownOverview(widgetIndex, selectedIndex);
            }
        }
        void OnResize() override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    OnResizeOverview();
                    break;
                case WINDOW_GUEST_STATS:
                case WINDOW_GUEST_RIDES:
                case WINDOW_GUEST_FINANCE:
                case WINDOW_GUEST_THOUGHTS:
                case WINDOW_GUEST_INVENTORY:
                case WINDOW_GUEST_DEBUG:
                    OnResizeCommon();
                    break;
            }
        }
        void OnUpdate() override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    OnUpdateOverview();
                    break;
                case WINDOW_GUEST_STATS:
                    OnUpdateStats();
                    break;
                case WINDOW_GUEST_RIDES:
                    OnUpdateRides();
                    break;
                case WINDOW_GUEST_FINANCE:
                    OnUpdateFinance();
                    break;
                case WINDOW_GUEST_THOUGHTS:
                    OnUpdateThoughts();
                    break;
                case WINDOW_GUEST_INVENTORY:
                    OnUpdateInventory();
                    break;
                case WINDOW_GUEST_DEBUG:
                    OnUpdateDebug();
                    break;
            }
        }
        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnToolUpdateOverview(widgetIndex, screenCoords);
            }
        }
        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnToolDownOverview(widgetIndex, screenCoords);
            }
        }
        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnToolAbortOverview(widgetIndex);
            }
        }
        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnTextInputOverview(widgetIndex, text);
            }
        }
        void OnViewportRotate() override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                OnViewportRotateOverview();
            }
        }
        void OnDraw(DrawPixelInfo& dpi) override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    OnDrawOverview(dpi);
                    break;
                case WINDOW_GUEST_STATS:
                    OnDrawStats(dpi);
                    break;
                case WINDOW_GUEST_RIDES:
                    OnDrawRides(dpi);
                    break;
                case WINDOW_GUEST_FINANCE:
                    OnDrawFinance(dpi);
                    break;
                case WINDOW_GUEST_THOUGHTS:
                    OnDrawThoughts(dpi);
                    break;
                case WINDOW_GUEST_INVENTORY:
                    OnDrawInventory(dpi);
                    break;
                case WINDOW_GUEST_DEBUG:
                    OnDrawDebug(dpi);
                    break;
            }
        }
        void OnPrepareDraw() override
        {
            OnPrepareDrawCommon();
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    OnPrepareDrawOverview();
                    break;
                case WINDOW_GUEST_RIDES:
                    OnPrepareDrawRides();
                    break;
            }
        }
        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                return OnScrollGetSizeRides(scrollIndex);
            }
            return {};
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                OnScrollMouseOverRides(scrollIndex, screenCoords);
            }
        }
        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                OnScrollMouseDownRides(scrollIndex, screenCoords);
            }
        }
        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                OnScrollDrawRides(scrollIndex, dpi);
            }
        }

    private:
        Guest* GetGuest()
        {
            auto guest = GetEntity<Guest>(EntityId::FromUnderlying(number));
            if (guest == nullptr)
            {
                Close();
                return nullptr;
            }
            return guest;
        }

        void OnResizeCommon()
        {
            // Get page specific min and max size
            int32_t minWidth = _guestWindowPageSizes[page][0].width;
            int32_t minHeight = _guestWindowPageSizes[page][0].height;
            int32_t maxWidth = _guestWindowPageSizes[page][1].width;
            int32_t maxHeight = _guestWindowPageSizes[page][1].height;

            // Ensure min size is large enough for all tabs to fit
            for (int32_t i = WIDX_TAB_1; i <= WIDX_TAB_7; i++)
            {
                if (!WidgetIsDisabled(*this, i))
                {
                    minWidth = std::max(minWidth, widgets[i].right + 3);
                }
            }
            maxWidth = std::max(minWidth, maxWidth);

            WindowSetResize(*this, minWidth, minHeight, maxWidth, maxHeight);
        }

        void OnPrepareDrawCommon()
        {
            pressed_widgets |= 1uLL << (page + WIDX_TAB_1);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            auto ft = Formatter::Common();
            peep->FormatNameTo(ft);

            ResizeFrameWithPage();

            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_7);
        }

        void DisableWidgets()
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            uint64_t newDisabledWidgets = 0;

            if (peep->CanBePickedUp())
            {
                if (WidgetIsDisabled(*this, WIDX_PICKUP))
                    Invalidate();
            }
            else
            {
                newDisabledWidgets = (1uLL << WIDX_PICKUP);
                if (!WidgetIsDisabled(*this, WIDX_PICKUP))
                    Invalidate();
            }
            if (GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY)
            {
                newDisabledWidgets |= (1uLL << WIDX_TAB_4); // Disable finance tab if no money
            }
            if (!Config::Get().general.DebuggingTools)
            {
                newDisabledWidgets |= (1uLL << WIDX_TAB_7); // Disable debug tab when debug tools not turned on
            }
            disabled_widgets = newDisabledWidgets;
        }

        void SetPage(int32_t newPage)
        {
            if (isToolActive(classification, number))
                ToolCancel();

            int32_t listen = 0;
            if (newPage == WINDOW_GUEST_OVERVIEW && page == WINDOW_GUEST_OVERVIEW && viewport != nullptr)
            {
                if (!(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
                    listen = 1;
            }

            page = newPage;
            frame_no = 0;
            _riddenRides.clear();
            selected_list_item = -1;

            RemoveViewport();

            hold_down_widgets = 0;
            pressed_widgets = 0;
            SetWidgets(_guestWindowPageWidgets[page]);
            DisableWidgets();
            Invalidate();
            OnResize();
            OnPrepareDraw();
            InitScrollWidgets();
            Invalidate();

            if (listen && viewport != nullptr)
                viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }

#pragma region Overview

        void OverviewTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_1))
                return;

            const auto& widget = widgets[WIDX_TAB_1];
            int32_t widgWidth = widget.width() - 1;
            int32_t widgHeight = widget.height() - 1;
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
            if (page == WINDOW_GUEST_OVERVIEW)
                widgHeight++;

            DrawPixelInfo clipDpi;
            if (!ClipDrawPixelInfo(clipDpi, dpi, screenCoords, widgWidth, widgHeight))
            {
                return;
            }

            screenCoords = ScreenCoordsXY{ 14, 20 };

            const auto* peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

            int32_t animationFrame = animObj->GetPeepAnimation(peep->AnimationGroup).base_image + 1;
            int32_t animationFrameOffset = 0;

            if (page == WINDOW_GUEST_OVERVIEW)
            {
                animationFrameOffset = _guestAnimationFrame;
                animationFrameOffset &= 0xFFFC;
            }
            animationFrame += animationFrameOffset;

            auto spriteId = ImageId(animationFrame, peep->TshirtColour, peep->TrousersColour);
            GfxDrawSprite(clipDpi, spriteId, screenCoords);

            auto* guest = peep->As<Guest>();
            if (guest == nullptr)
                return;

            // There are only 6 walking frames available for each item.
            auto itemFrame = (_guestAnimationFrame / 4) % 6;

            if (guest->AnimationGroup == PeepAnimationGroup::Hat)
            {
                auto itemOffset = kPeepSpriteHatItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->HatColour);
                GfxDrawSprite(clipDpi, imageId, screenCoords);
            }

            if (guest->AnimationGroup == PeepAnimationGroup::Balloon)
            {
                auto itemOffset = kPeepSpriteBalloonItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->BalloonColour);
                GfxDrawSprite(clipDpi, imageId, screenCoords);
            }

            if (guest->AnimationGroup == PeepAnimationGroup::Umbrella)
            {
                auto itemOffset = kPeepSpriteUmbrellaItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->UmbrellaColour);
                GfxDrawSprite(clipDpi, imageId, screenCoords);
            }
        }

        void OnResizeOverview()
        {
            DisableWidgets();
            OnPrepareDraw();
            InvalidateWidget(WIDX_MARQUEE);
            OnResizeCommon();

            if (viewport != nullptr)
            {
                auto reqViewportWidth = width - 30;
                auto reqViewportHeight = height - 72;
                if (viewport->width != reqViewportWidth || viewport->height != reqViewportHeight)
                {
                    viewport->width = reqViewportWidth;
                    viewport->height = reqViewportHeight;
                }
            }
            OnViewportRotate();
        }

        void OnMouseUpOverview(WidgetIndex widgetIndex)
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            switch (widgetIndex)
            {
                case WIDX_PICKUP:
                {
                    if (!peep->CanBePickedUp())
                    {
                        return;
                    }
                    _pickedPeepX = peep->x;
                    CoordsXYZ nullLoc{};
                    nullLoc.SetNull();
                    PeepPickupAction pickupAction{ PeepPickupType::Pickup, EntityId::FromUnderlying(number), nullLoc,
                                                   NetworkGetCurrentPlayerId() };
                    pickupAction.SetCallback([peepnum = number](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error != GameActions::Status::Ok)
                            return;
                        auto* windowMgr = GetWindowManager();
                        WindowBase* wind = windowMgr->FindByNumber(WindowClass::Peep, peepnum);
                        if (wind != nullptr)
                        {
                            ToolSet(*wind, WC_PEEP__WIDX_PICKUP, Tool::Picker);
                        }
                    });
                    GameActions::Execute(&pickupAction);
                }
                break;
                case WIDX_RENAME:
                {
                    auto peepName = peep->GetName();
                    WindowTextInputRawOpen(
                        this, widgetIndex, STR_GUEST_RENAME_TITLE, STR_GUEST_RENAME_PROMPT, {}, peepName.c_str(), 32);
                    break;
                }
                case WIDX_TRACK:
                {
                    uint32_t guestFlags = peep->PeepFlags ^ PEEP_FLAGS_TRACKING;

                    auto guestSetFlagsAction = GuestSetFlagsAction(EntityId::FromUnderlying(number), guestFlags);
                    GameActions::Execute(&guestSetFlagsAction);
                }
                break;
            }
        }

        void OnMouseDownOverview(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                    ShowLocateDropdown(widgets[widgetIndex]);
                    break;
            }
        }

        void OnDropdownOverview(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                {
                    if (dropdownIndex == 0)
                    {
                        ScrollToViewport();
                    }
                    else if (dropdownIndex == 1)
                    {
                        GuestFollow();
                    }
                    break;
                }
            }
        }

        void ShowLocateDropdown(Widget& widget)
        {
            gDropdownItems[0].Format = STR_LOCATE_SUBJECT_TIP;
            gDropdownItems[1].Format = STR_FOLLOW_SUBJECT_TIP;

            WindowDropdownShowText(
                { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height() + 1, colours[1], 0, 2);
            gDropdownDefaultIndex = 0;
        }

        void GuestFollow()
        {
            WindowBase* main = WindowGetMain();
            WindowFollowSprite(*main, EntityId::FromUnderlying(number));
        }

        void OnViewportRotateOverview()
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            ViewportUpdateSmartFollowGuest(this, *peep);
            bool reCreateViewport = false;
            uint16_t origViewportFlags{};
            if (viewport != nullptr)
            {
                if (focus.has_value())
                    return;

                origViewportFlags = viewport->flags;

                reCreateViewport = true;
                RemoveViewport();
            }

            OnPrepareDraw();

            if (peep->State != PeepState::Picked && viewport == nullptr)
            {
                const auto& viewWidget = widgets[WIDX_VIEWPORT];
                auto screenPos = ScreenCoordsXY{ viewWidget.left + 1 + windowPos.x, viewWidget.top + 1 + windowPos.y };
                int32_t widgWidth = viewWidget.width() - 1;
                int32_t widgHeight = viewWidget.height() - 1;

                ViewportCreate(this, screenPos, widgWidth, widgHeight, focus.value());
                if (viewport != nullptr && reCreateViewport)
                {
                    viewport->flags = origViewportFlags;
                }
                flags |= WF_NO_SCROLLING;
                Invalidate();
            }
            Invalidate();
        }

        void OnDrawOverview(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            // Draw the viewport no sound sprite
            if (viewport != nullptr)
            {
                WindowDrawViewport(dpi, *this);
                if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                {
                    GfxDrawSprite(dpi, ImageId(SPR_HEARING_VIEWPORT), WindowGetViewportSoundIconPos(*this));
                }
            }

            // Draw the centred label
            const auto* peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            const auto& actionLabelWidget = widgets[WIDX_ACTION_LBL];
            auto screenPos = windowPos + ScreenCoordsXY{ actionLabelWidget.midX(), actionLabelWidget.top - 1 };

            {
                auto ft = Formatter();
                peep->FormatActionTo(ft);
                int32_t textWidth = actionLabelWidget.width();
                DrawTextEllipsised(dpi, screenPos, textWidth, STR_BLACK_STRING, ft, { TextAlignment::CENTRE });
            }

            // Draw the marquee thought
            const auto& marqueeWidget = widgets[WIDX_MARQUEE];
            auto marqWidth = marqueeWidget.width() - 3;
            int32_t left = marqueeWidget.left + 2 + windowPos.x;
            int32_t top = marqueeWidget.top + windowPos.y;
            int32_t marqHeight = marqueeWidget.height();
            DrawPixelInfo dpiMarquee;
            if (!ClipDrawPixelInfo(dpiMarquee, dpi, { left, top }, marqWidth, marqHeight))
            {
                return;
            }

            int32_t i = 0;
            for (; i < kPeepMaxThoughts; ++i)
            {
                if (peep->Thoughts[i].type == PeepThoughtType::None)
                {
                    _marqueePosition = 0;
                    return;
                }
                if (peep->Thoughts[i].freshness == 1)
                { // If a fresh thought
                    break;
                }
            }
            if (i == kPeepMaxThoughts)
            {
                _marqueePosition = 0;
                return;
            }

            screenPos.x = marqueeWidget.width() - _marqueePosition;
            {
                auto ft = Formatter();
                PeepThoughtSetFormatArgs(&peep->Thoughts[i], ft);
                DrawTextBasic(dpiMarquee, { screenPos.x, 0 }, STR_WINDOW_COLOUR_2_STRINGID, ft, { FontStyle::Small });
            }
        }

        void OnPrepareDrawOverview()
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            pressed_widgets &= ~(1uLL << WIDX_TRACK);
            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
            {
                pressed_widgets |= (1uLL << WIDX_TRACK);
            }

            widgets[WIDX_VIEWPORT].right = width - 26;
            widgets[WIDX_VIEWPORT].bottom = height - 14;

            widgets[WIDX_ACTION_LBL].top = height - 12;
            widgets[WIDX_ACTION_LBL].bottom = height - 3;
            widgets[WIDX_ACTION_LBL].right = width - 24;

            widgets[WIDX_MARQUEE].right = width - 24;

            widgets[WIDX_PICKUP].right = width - 2;
            widgets[WIDX_RENAME].right = width - 2;
            widgets[WIDX_LOCATE].right = width - 2;
            widgets[WIDX_TRACK].right = width - 2;

            widgets[WIDX_PICKUP].left = width - 25;
            widgets[WIDX_RENAME].left = width - 25;
            widgets[WIDX_LOCATE].left = width - 25;
            widgets[WIDX_TRACK].left = width - 25;
        }

        void OnUpdateOverview()
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

            // Overview tab animation offset
            _guestAnimationFrame++;
            _guestAnimationFrame %= 24;

            // Get pickup animation length
            const auto& pickAnim = animObj->GetPeepAnimation(peep->AnimationGroup, PeepAnimationType::Hanging);
            const auto pickAnimLength = pickAnim.frame_offsets.size();

            // Update pickup animation, can only happen in this tab.
            picked_peep_frame++;
            picked_peep_frame %= pickAnimLength * 4;

            InvalidateWidget(WIDX_TAB_1);
            InvalidateWidget(WIDX_TAB_2);

            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_ACTION)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_ACTION;
                InvalidateWidget(WIDX_ACTION_LBL);
            }

            _marqueePosition += 2;

            _beingWatchedTimer++;

            // Disable peep watching thought for multiplayer as it's client specific
            if (NetworkGetMode() == NETWORK_MODE_NONE)
            {
                // Create the "I have the strangest feeling I am being watched thought"
                if (_beingWatchedTimer >= 3840)
                {
                    if (!(_beingWatchedTimer & 0x3FF))
                    {
                        int32_t random = UtilRand() & 0xFFFF;
                        if (random <= 0x2AAA)
                        {
                            peep->InsertNewThought(PeepThoughtType::Watched);
                        }
                    }
                }
            }
        }

        void OnTextInputOverview(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex != WIDX_RENAME)
                return;

            if (text.empty())
                return;
            std::string sText(text);
            auto gameAction = GuestSetNameAction(EntityId::FromUnderlying(number), sText);
            GameActions::Execute(&gameAction);
        }

        void OnToolUpdateOverview(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            MapInvalidateSelectionRect();

            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;

            auto mapCoords = FootpathGetCoordinatesFromPos({ screenCoords.x, screenCoords.y + 16 }, nullptr, nullptr);
            if (!mapCoords.IsNull())
            {
                gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE;
                gMapSelectType = MAP_SELECT_TYPE_FULL;
                gMapSelectPositionA = mapCoords;
                gMapSelectPositionB = mapCoords;
                MapInvalidateSelectionRect();
            }

            gPickupPeepImage = ImageId();

            auto info = GetMapCoordinatesFromPos(screenCoords, kViewportInteractionItemAll);
            if (info.interactionType == ViewportInteractionItem::None)
                return;

            gPickupPeepX = screenCoords.x - 1;
            gPickupPeepY = screenCoords.y + 16;

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(peep->AnimationObjectIndex);

            auto baseImageId = animObj->GetPeepAnimation(peep->AnimationGroup, PeepAnimationType::Hanging).base_image;
            baseImageId += picked_peep_frame >> 2;
            gPickupPeepImage = ImageId(baseImageId, peep->TshirtColour, peep->TrousersColour);
        }

        void OnToolDownOverview(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            TileElement* tileElement;
            auto destCoords = FootpathGetCoordinatesFromPos({ screenCoords.x, screenCoords.y + 16 }, nullptr, &tileElement);

            if (destCoords.IsNull())
                return;

            PeepPickupAction pickupAction{ PeepPickupType::Place,
                                           EntityId::FromUnderlying(number),
                                           { destCoords, tileElement->GetBaseZ() },
                                           NetworkGetCurrentPlayerId() };
            pickupAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                    return;
                ToolCancel();
                gPickupPeepImage = ImageId();
            });
            GameActions::Execute(&pickupAction);
        }

        void OnToolAbortOverview(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            PeepPickupAction pickupAction{
                PeepPickupType::Cancel, EntityId::FromUnderlying(number), { _pickedPeepX, 0, 0 }, NetworkGetCurrentPlayerId()
            };
            GameActions::Execute(&pickupAction);
        }

#pragma endregion

#pragma region Stats
        void StatsTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_2))
                return;

            const auto& widget = widgets[WIDX_TAB_2];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            int32_t imageId = GetPeepFaceSpriteLarge(peep);
            if (page == WINDOW_GUEST_STATS)
            {
                // If currently viewing this tab animate tab
                // if it is very sick or angry.
                switch (imageId)
                {
                    case SPR_PEEP_LARGE_FACE_VERY_VERY_SICK_0:
                        imageId += (frame_no / 4) & 0xF;
                        break;
                    case SPR_PEEP_LARGE_FACE_VERY_SICK_0:
                        imageId += (frame_no / 8) & 0x3;
                        break;
                    case SPR_PEEP_LARGE_FACE_ANGRY_0:
                        imageId += (frame_no / 8) & 0x3;
                        break;
                }
            }
            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
        }

        void OnUpdateStats()
        {
            frame_no++;
            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_STATS;

            Invalidate();
        }

        /**
         * Takes a guest stat value (scaled to currMax) and adjusts it to be scaled out of 100.
         * Then clamp the value to between newMin and 100.
         */
        int32_t NormalizeGuestStatValue(int32_t value, int32_t currMax, int32_t newMin) const
        {
            int32_t newValue = (value * 100) / currMax;
            return std::clamp(newValue, newMin, 100);
        }

        void OnDrawStats(DrawPixelInfo& dpi)
        {
            // ebx
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            int32_t happinessPercentage = NormalizeGuestStatValue(peep->Happiness, kPeepMaxHappiness, 10);
            WidgetProgressBarSetNewPercentage(widgets[WIDX_HAPPINESS_BAR], happinessPercentage);

            int32_t energyPercentage = NormalizeGuestStatValue(
                peep->Energy - kPeepMinEnergy, kPeepMaxEnergy - kPeepMinEnergy, 10);
            WidgetProgressBarSetNewPercentage(widgets[WIDX_ENERGY_BAR], energyPercentage);

            int32_t hungerPercentage = NormalizeGuestStatValue(peep->Hunger - 32, 158, 0);
            hungerPercentage = 100 - hungerPercentage; // the bar should be longer when peep->Hunger is low
            WidgetProgressBarSetNewPercentage(widgets[WIDX_HUNGER_BAR], hungerPercentage);

            int32_t thirstPercentage = NormalizeGuestStatValue(peep->Thirst - 32, 158, 0);
            thirstPercentage = 100 - thirstPercentage; // the bar should be longer when peep->Thirst is low
            WidgetProgressBarSetNewPercentage(widgets[WIDX_THIRST_BAR], thirstPercentage);

            int32_t nauseaPercentage = NormalizeGuestStatValue(peep->Nausea - 32, 223, 0);
            WidgetProgressBarSetNewPercentage(widgets[WIDX_NAUSEA_BAR], nauseaPercentage);

            int32_t toiletPercentage = NormalizeGuestStatValue(peep->Toilet - 64, 178, 0);
            WidgetProgressBarSetNewPercentage(widgets[WIDX_TOILET_BAR], toiletPercentage);

            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            // Not sure why this is not stats widgets
            // cx dx
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            // Happiness
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_HAPPINESS_LABEL);

            // Energy
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_ENERGY_LABEL);

            // Hunger
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_HUNGER_LABEL);

            // Thirst
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_THIRST_LABEL);

            // Nausea
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_NAUSEA_LABEL);

            // Toilet
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_TOILET_LABEL);

            // Time in park
            screenCoords.y += kListRowHeight + 1;
            int32_t guestEntryTime = peep->GetParkEntryTime();
            if (guestEntryTime != -1)
            {
                int32_t timeInPark = (GetGameState().CurrentTicks - guestEntryTime) >> 11;
                auto ft = Formatter();
                ft.Add<uint16_t>(timeInPark & 0xFFFF);
                DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_TIME_IN_PARK, ft);
            }

            screenCoords.y += kListRowHeight + 9;
            GfxFillRectInset(
                dpi, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 179, -5 } }, colours[1],
                INSET_RECT_FLAG_BORDER_INSET);

            // Preferred Ride
            DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_PREFERRED_RIDE);
            screenCoords.y += kListRowHeight;

            // Intensity
            {
                auto ft = Formatter();
                auto maxIntensity = peep->Intensity.GetMaximum();
                int32_t string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BELOW;
                if (peep->Intensity.GetMinimum() != 0)
                {
                    ft.Add<uint16_t>(peep->Intensity.GetMinimum());
                    ft.Add<uint16_t>(maxIntensity);
                    string_id = STR_GUEST_STAT_PREFERRED_INTESITY_BETWEEN;
                    if (maxIntensity == 15)
                        string_id = STR_GUEST_STAT_PREFERRED_INTESITY_ABOVE;
                }
                else
                {
                    ft.Add<uint16_t>(maxIntensity);
                }

                DrawTextBasic(dpi, screenCoords + ScreenCoordsXY{ 4, 0 }, string_id, ft);
            }

            // Nausea tolerance
            {
                static constexpr StringId _nauseaTolerances[] = {
                    STR_PEEP_STAT_NAUSEA_TOLERANCE_NONE,
                    STR_PEEP_STAT_NAUSEA_TOLERANCE_LOW,
                    STR_PEEP_STAT_NAUSEA_TOLERANCE_AVERAGE,
                    STR_PEEP_STAT_NAUSEA_TOLERANCE_HIGH,
                };
                screenCoords.y += kListRowHeight;
                auto nausea_tolerance = EnumValue(peep->NauseaTolerance) & 0x3;
                auto ft = Formatter();
                ft.Add<StringId>(_nauseaTolerances[nausea_tolerance]);
                DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_NAUSEA_TOLERANCE, ft);
            }
        }

#pragma endregion

#pragma region Rides
        void RidesTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_3))
                return;

            const auto& widget = widgets[WIDX_TAB_3];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_RIDE_0;

            if (page == WINDOW_GUEST_RIDES)
            {
                imageId += (frame_no / 4) & 0xF;
            }

            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
        }

        void OnUpdateRides()
        {
            frame_no++;

            InvalidateWidget(WIDX_TAB_2);
            InvalidateWidget(WIDX_TAB_3);

            const auto guest = GetGuest();
            if (guest == nullptr)
            {
                return;
            }

            // Every 2048 ticks do a full window_invalidate
            int32_t numTicks = GetGameState().CurrentTicks - guest->GetParkEntryTime();
            if (!(numTicks & 0x7FF))
                Invalidate();

            const auto oldSize = _riddenRides.size();
            _riddenRides.clear();
            for (const auto& r : GetRideManager())
            {
                if (r.IsRide() && guest->HasRidden(r))
                {
                    _riddenRides.push_back(r.id);
                }
            }

            // If there are new items
            if (oldSize != _riddenRides.size())
            {
                Invalidate();
            }
        }

        ScreenSize OnScrollGetSizeRides(int32_t scrollIndex)
        {
            ScreenSize newSize;
            newSize.height = static_cast<int32_t>(_riddenRides.size()) * 10;

            if (selected_list_item != -1)
            {
                selected_list_item = -1;
                Invalidate();
            }

            int32_t visableHeight = newSize.height - widgets[WIDX_RIDE_SCROLL].bottom + widgets[WIDX_RIDE_SCROLL].top + 21;

            if (visableHeight < 0)
                visableHeight = 0;

            if (visableHeight < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = visableHeight;
                Invalidate();
            }
            return newSize;
        }

        void OnScrollMouseDownRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            auto index = screenCoords.y / 10;
            if (index >= static_cast<int32_t>(_riddenRides.size()))
                return;

            auto intent = Intent(WindowClass::Ride);
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, _riddenRides[index]);
            ContextOpenIntent(&intent);
        }

        void OnScrollMouseOverRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            auto index = screenCoords.y / 10;
            if (index >= static_cast<int32_t>(_riddenRides.size()))
                return;

            if (index == selected_list_item)
                return;
            selected_list_item = index;

            Invalidate();
        }

        void OnPrepareDrawRides()
        {
            widgets[WIDX_RIDE_SCROLL].right = width - 4;
            widgets[WIDX_RIDE_SCROLL].bottom = height - 15;
        }

        void OnDrawRides(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            // cx dx
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 2, widgets[WIDX_PAGE_BACKGROUND].top + 2 };

            DrawTextBasic(dpi, screenCoords, STR_GUEST_LABEL_RIDES_BEEN_ON);

            screenCoords.y = windowPos.y + widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

            auto ft = Formatter();
            auto* r = GetRide(peep->FavouriteRide);
            if (r != nullptr)
            {
                r->FormatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_PEEP_FAVOURITE_RIDE_NOT_AVAILABLE);
            }

            DrawTextEllipsised(dpi, screenCoords, width - 14, STR_FAVOURITE_RIDE, ft);
        }

        void OnScrollDrawRides(int32_t scrollIndex, DrawPixelInfo& dpi)
        {
            auto colour = ColourMapA[colours[1].colour].mid_light;
            GfxFillRect(dpi, { { dpi.x, dpi.y }, { dpi.x + dpi.width - 1, dpi.y + dpi.height - 1 } }, colour);

            for (int32_t listIndex = 0; listIndex < static_cast<int32_t>(_riddenRides.size()); listIndex++)
            {
                int32_t y = listIndex * 10;
                StringId stringId = STR_BLACK_STRING;
                if (listIndex == selected_list_item)
                {
                    GfxFilterRect(dpi, { 0, y, 800, y + 9 }, FilterPaletteID::PaletteDarken1);
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                }

                auto* r = GetRide(_riddenRides[listIndex]);
                if (r != nullptr)
                {
                    auto ft = Formatter();
                    r->FormatNameTo(ft);
                    DrawTextBasic(dpi, { 0, y - 1 }, stringId, ft);
                }
            }
        }
#pragma endregion

#pragma region Finance
        void FinanceTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_4))
                return;

            const auto& widget = widgets[WIDX_TAB_4];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_FINANCES_SUMMARY_0;

            if (page == WINDOW_GUEST_FINANCE)
            {
                imageId += (frame_no / 2) & 0x7;
            }

            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
        }

        void OnUpdateFinance()
        {
            frame_no++;

            InvalidateWidget(WIDX_TAB_2);
            InvalidateWidget(WIDX_TAB_4);
        }

        void OnDrawFinance(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            // cx dx
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            // Cash in pocket
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->CashInPocket);
                DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_CASH_IN_POCKET, ft);
                screenCoords.y += kListRowHeight;
            }

            // Cash spent
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->CashSpent);
                DrawTextBasic(dpi, screenCoords, STR_GUEST_STAT_CASH_SPENT, ft);
                screenCoords.y += kListRowHeight * 2;
            }

            GfxFillRectInset(
                dpi, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 179, -5 } }, colours[1],
                INSET_RECT_FLAG_BORDER_INSET);

            // Paid to enter
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidToEnter);
                DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_ENTRANCE_FEE, ft);
                screenCoords.y += kListRowHeight;
            }
            // Paid on rides
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidOnRides);
                ft.Add<uint16_t>(peep->GuestNumRides);
                if (peep->GuestNumRides != 1)
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_RIDE_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_RIDE, ft);
                }
                screenCoords.y += kListRowHeight;
            }
            // Paid on food
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidOnFood);
                ft.Add<uint16_t>(peep->AmountOfFood);
                if (peep->AmountOfFood != 1)
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_FOOD_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_FOOD, ft);
                }
                screenCoords.y += kListRowHeight;
            }

            // Paid on drinks
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidOnDrink);
                ft.Add<uint16_t>(peep->AmountOfDrinks);
                if (peep->AmountOfDrinks != 1)
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_DRINK_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_DRINK, ft);
                }
                screenCoords.y += kListRowHeight;
            }
            // Paid on souvenirs
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidOnSouvenirs);
                ft.Add<uint16_t>(peep->AmountOfSouvenirs);
                if (peep->AmountOfSouvenirs != 1)
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_SOUVENIR_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(dpi, screenCoords, STR_GUEST_EXPENSES_SOUVENIR, ft);
                }
            }
        }
#pragma endregion

#pragma region Thoughts
        void ThoughtsTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_5))
                return;

            const auto& widget = widgets[WIDX_TAB_5];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_THOUGHTS_0;

            if (page == WINDOW_GUEST_THOUGHTS)
            {
                imageId += (frame_no / 2) & 0x7;
            }

            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
        }

        void OnUpdateThoughts()
        {
            frame_no++;

            InvalidateWidget(WIDX_TAB_2);
            InvalidateWidget(WIDX_TAB_5);

            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_THOUGHTS)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_THOUGHTS;
                Invalidate();
            }
        }

        void OnDrawThoughts(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            // cx dx
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };

            DrawTextBasic(dpi, screenCoords, STR_GUEST_RECENT_THOUGHTS_LABEL);

            screenCoords.y += 10;
            for (const auto& thought : peep->Thoughts)
            {
                if (thought.type == PeepThoughtType::None)
                    return;
                if (thought.freshness == 0)
                    continue;

                int32_t widgWidth = widgets[WIDX_PAGE_BACKGROUND].right - widgets[WIDX_PAGE_BACKGROUND].left - 8;

                auto ft = Formatter();
                PeepThoughtSetFormatArgs(&thought, ft);
                screenCoords.y += DrawTextWrapped(dpi, screenCoords, widgWidth, STR_BLACK_STRING, ft, { FontStyle::Small });

                // If this is the last visible line end drawing.
                if (screenCoords.y > windowPos.y + widgets[WIDX_PAGE_BACKGROUND].bottom - 32)
                    return;
            }
        }
#pragma endregion

#pragma region Inventory
        void InventoryTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_6))
                return;

            const auto& widget = widgets[WIDX_TAB_6];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            GfxDrawSprite(dpi, ImageId(SPR_TAB_GUEST_INVENTORY), screenCoords);
        }

        void OnUpdateInventory()
        {
            frame_no++;

            InvalidateWidget(WIDX_TAB_2);
            InvalidateWidget(WIDX_TAB_6);

            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_INVENTORY)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_INVENTORY;
                Invalidate();
            }
        }

        std::pair<ImageId, Formatter> InventoryFormatItem(Guest& guest, ShopItem item) const
        {
            auto parkName = GetGameState().Park.Name.c_str();

            // Default item image
            auto& itemDesc = GetShopItemDescriptor(item);
            auto itemImage = ImageId(itemDesc.Image);

            // Default arguments
            auto ft = Formatter();
            ft.Add<StringId>(itemDesc.Naming.Display);
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(parkName);

            // Special overrides
            Ride* invRide{};
            switch (item)
            {
                case ShopItem::Balloon:
                    itemImage = ImageId(itemDesc.Image, guest.BalloonColour);
                    break;
                case ShopItem::Photo:
                    invRide = GetRide(guest.Photo1RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->FormatNameTo(ft);
                    }

                    break;
                case ShopItem::Umbrella:
                    itemImage = ImageId(itemDesc.Image, guest.UmbrellaColour);
                    break;
                case ShopItem::Voucher:
                    switch (guest.VoucherType)
                    {
                        case VOUCHER_TYPE_PARK_ENTRY_FREE:
                            ft.Rewind();
                            ft.Increment(2);
                            ft.Add<StringId>(STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_FREE);
                            ft.Add<StringId>(STR_STRING);
                            ft.Add<const char*>(parkName);
                            break;
                        case VOUCHER_TYPE_RIDE_FREE:
                            invRide = GetRide(guest.VoucherRideId);
                            if (invRide != nullptr)
                            {
                                ft.Rewind();
                                ft.Increment(2);
                                ft.Add<StringId>(STR_PEEP_INVENTORY_VOUCHER_RIDE_FREE);
                                invRide->FormatNameTo(ft);
                            }
                            break;
                        case VOUCHER_TYPE_PARK_ENTRY_HALF_PRICE:
                            ft.Rewind();
                            ft.Increment(2);
                            ft.Add<StringId>(STR_PEEP_INVENTORY_VOUCHER_PARK_ENTRY_HALF_PRICE);
                            ft.Add<StringId>(STR_STRING);
                            ft.Add<const char*>(parkName);
                            break;
                        case VOUCHER_TYPE_FOOD_OR_DRINK_FREE:
                            ft.Rewind();
                            ft.Increment(2);
                            ft.Add<StringId>(STR_PEEP_INVENTORY_VOUCHER_FOOD_OR_DRINK_FREE);
                            ft.Add<StringId>(GetShopItemDescriptor(guest.VoucherShopItem).Naming.Singular);
                            break;
                    }
                    break;
                case ShopItem::Hat:
                    itemImage = ImageId(itemDesc.Image, guest.HatColour);
                    break;
                case ShopItem::TShirt:
                    itemImage = ImageId(itemDesc.Image, guest.TshirtColour);
                    break;
                case ShopItem::Photo2:
                    invRide = GetRide(guest.Photo2RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->FormatNameTo(ft);
                    }
                    break;
                case ShopItem::Photo3:
                    invRide = GetRide(guest.Photo3RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->FormatNameTo(ft);
                    }
                    break;
                case ShopItem::Photo4:
                    invRide = GetRide(guest.Photo4RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->FormatNameTo(ft);
                    }
                    break;
                default:
                    // This switch handles only items visible in peep window
                    break;
            }

            return std::make_pair(itemImage, ft);
        }

        void OnDrawInventory(DrawPixelInfo& dpi)
        {
            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            const auto guest = GetGuest();
            if (guest == nullptr)
            {
                return;
            }

            auto& widget = widgets[WIDX_PAGE_BACKGROUND];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 4, widget.top + 2 };
            int32_t itemNameWidth = widget.width() - 24;

            int32_t maxY = windowPos.y + height - 22;
            int32_t numItems = 0;

            DrawTextBasic(dpi, screenCoords, STR_CARRYING);
            screenCoords.y += 10;

            for (ShopItem item = ShopItem::Balloon; item < ShopItem::Count; item++)
            {
                if (screenCoords.y >= maxY)
                    break;
                if (!guest->HasItem(item))
                    continue;

                auto [imageId, ft] = InventoryFormatItem(*guest, item);
                GfxDrawSprite(dpi, imageId, screenCoords);

                screenCoords.x += 16;
                screenCoords.y += 1;
                screenCoords.y += DrawTextWrapped(dpi, screenCoords, itemNameWidth, STR_BLACK_STRING, ft);

                screenCoords.x -= 16;
                numItems++;
            }

            if (numItems == 0)
            {
                DrawTextBasic(dpi, screenCoords, STR_NOTHING);
            }
        }
#pragma endregion

#pragma region Debug
        void DebugTabDraw(DrawPixelInfo& dpi)
        {
            if (WidgetIsDisabled(*this, WIDX_TAB_7))
                return;

            const auto& widget = widgets[WIDX_TAB_7];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_GEARS_0;
            if (page == WINDOW_GUEST_DEBUG)
            {
                imageId += (frame_no / 2) & 0x3;
            }

            GfxDrawSprite(dpi, ImageId(imageId), screenCoords);
        }

        void OnUpdateDebug()
        {
            frame_no++;
            Invalidate();
        }

        void OnDrawDebug(DrawPixelInfo& dpi)
        {
            char buffer[512]{};
            char buffer2[512]{};

            DrawWidgets(dpi);
            OverviewTabDraw(dpi);
            StatsTabDraw(dpi);
            RidesTabDraw(dpi);
            FinanceTabDraw(dpi);
            ThoughtsTabDraw(dpi);
            InventoryTabDraw(dpi);
            DebugTabDraw(dpi);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 4 };
            {
                auto ft = Formatter();
                ft.Add<uint32_t>(peep->Id);
                DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_SPRITE_INDEX, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->x);
                ft.Add<int32_t>(peep->y);
                ft.Add<int32_t>(peep->z);
                DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_POSITION, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->NextLoc.x);
                ft.Add<int32_t>(peep->NextLoc.y);
                ft.Add<int32_t>(peep->NextLoc.z);
                OpenRCT2::FormatStringLegacy(buffer, sizeof(buffer), STR_PEEP_DEBUG_NEXT, ft.Data());
                if (peep->GetNextIsSurface())
                {
                    OpenRCT2::FormatStringLegacy(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SURFACE, nullptr);
                    String::safeConcat(buffer, buffer2, sizeof(buffer));
                }
                if (peep->GetNextIsSloped())
                {
                    auto ft2 = Formatter();
                    ft2.Add<int32_t>(peep->GetNextDirection());
                    OpenRCT2::FormatStringLegacy(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SLOPE, ft2.Data());
                    String::safeConcat(buffer, buffer2, sizeof(buffer));
                }
                DrawText(dpi, screenCoords, {}, buffer);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->DestinationX);
                ft.Add<int32_t>(peep->DestinationY);
                ft.Add<int32_t>(peep->DestinationTolerance);
                DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_DEST, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->PathfindGoal.x);
                ft.Add<int32_t>(peep->PathfindGoal.y);
                ft.Add<int32_t>(peep->PathfindGoal.z);
                ft.Add<int32_t>(peep->PathfindGoal.direction);
                DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_PATHFIND_GOAL, ft);
            }
            screenCoords.y += kListRowHeight;
            DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_PATHFIND_HISTORY);
            screenCoords.y += kListRowHeight;

            screenCoords.x += 10;
            for (auto& point : peep->PathfindHistory)
            {
                auto ft = Formatter();
                ft.Add<int32_t>(point.x);
                ft.Add<int32_t>(point.y);
                ft.Add<int32_t>(point.z);
                ft.Add<int32_t>(point.direction);
                DrawTextBasic(dpi, screenCoords, STR_PEEP_DEBUG_PATHFIND_HISTORY_ITEM, ft);
                screenCoords.y += kListRowHeight;
            }
            screenCoords.x -= 10;
        }
#pragma endregion
    };

    /**
     *
     *  rct2: 0x006989E9
     *
     */
    WindowBase* GuestOpen(Peep* peep)
    {
        if (peep == nullptr)
        {
            return nullptr;
        }
        if (peep->Is<Staff>())
        {
            return StaffOpen(peep);
        }

        auto* windowMgr = GetWindowManager();
        auto* window = static_cast<GuestWindow*>(windowMgr->BringToFrontByNumber(WindowClass::Peep, peep->Id.ToUnderlying()));
        if (window == nullptr)
        {
            int32_t windowWidth = 192;
            if (Config::Get().general.DebuggingTools)
                windowWidth += TabWidth;

            window = windowMgr->Create<GuestWindow>(WindowClass::Peep, windowWidth, 157, WF_RESIZABLE);
            if (window == nullptr)
            {
                return nullptr;
            }
        }

        window->Init(peep->Id);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
