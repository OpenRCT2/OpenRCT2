/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/GuestSetFlagsAction.h>
#include <openrct2/actions/GuestSetNameAction.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/management/Marketing.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepSpriteIds.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ride/ShopItem.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_STRINGID;
    static constexpr ScreenSize kWindowSize = { 192, 157 };

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

        WIDX_HAPPINESS_LABEL = WIDX_TAB_CONTENT_START,
        WIDX_HAPPINESS_BAR,
        WIDX_ENERGY_LABEL,
        WIDX_ENERGY_BAR,
        WIDX_HUNGER_LABEL,
        WIDX_HUNGER_BAR,
        WIDX_THIRST_LABEL,
        WIDX_THIRST_BAR,
        WIDX_NAUSEA_LABEL,
        WIDX_NAUSEA_BAR,
        WIDX_TOILET_LABEL,
        WIDX_TOILET_BAR,
        WIDX_SEPARATOR,

        WIDX_RIDES_BEEN_ON_LABEL = WIDX_TAB_CONTENT_START,
        WIDX_RIDE_SCROLL,

        WIDX_RECENT_THOUGHTS_LABEL = WIDX_TAB_CONTENT_START,

        WIDX_CARRYING_LABEL = WIDX_TAB_CONTENT_START,
    };

    VALIDATE_GLOBAL_WIDX(WC_PEEP, WIDX_PICKUP);

    static constexpr int32_t kTabWidth = 30;

    // clang-format off
    static constexpr auto kMainGuestWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ 0, 43 }, { 192, 114 }, WidgetType::resize, WindowColour::secondary), /* Resize */
        makeTab({ 3, 17 }, STR_SHOW_GUEST_VIEW_TIP),                                      /* Tab 1 */
        makeTab({ 34, 17 }, STR_SHOW_GUEST_NEEDS_TIP),                                    /* Tab 2 */
        makeTab({ 65, 17 }, STR_SHOW_GUEST_VISITED_RIDES_TIP),                            /* Tab 3 */
        makeTab({ 96, 17 }, STR_SHOW_GUEST_FINANCE_TIP),                                  /* Tab 4 */
        makeTab({ 127, 17 }, STR_SHOW_GUEST_THOUGHTS_TIP),                                /* Tab 5 */
        makeTab({ 158, 17 }, STR_SHOW_GUEST_ITEMS_TIP),                                   /* Tab 6 */
        makeTab({ 189, 17 }, STR_DEBUG_TIP)                                               /* Tab 7 */
    );

    static constexpr auto _guestWindowWidgetsOverview = makeWidgets(
        kMainGuestWidgets,
        makeWidget({  3,  45}, {164, 12}, WidgetType::labelCentred, WindowColour::secondary                                                        ), // Label Thought marquee
        makeWidget({  3,  57}, {164, 87}, WidgetType::viewport,     WindowColour::secondary                                                        ), // Viewport
        makeWidget({  3, 144}, {164, 11}, WidgetType::labelCentred, WindowColour::secondary                                                        ), // Label Action
        makeWidget({167,  45}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_PICKUP_BTN), STR_PICKUP_TIP               ), // Pickup Button
        makeWidget({167,  69}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_RENAME),     STR_NAME_GUEST_TIP           ), // Rename Button
        makeWidget({167,  93}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_LOCATE),     STR_LOCATE_SUBJECT_TIP       ), // Locate Button
        makeWidget({167, 117}, { 24, 24}, WidgetType::flatBtn,      WindowColour::secondary, ImageId(SPR_TRACK_PEEP), STR_TOGGLE_GUEST_TRACKING_TIP)  // Track Button
    );

    static constexpr auto _guestWindowWidgetsStats = makeWidgets(
        kMainGuestWidgets,
        makeWidget     ({  3, (kListRowHeight * 0) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_HAPPINESS_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 0) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightGreen,             0,                       19),
        makeWidget     ({  3, (kListRowHeight * 1) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_ENERGY_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 1) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightGreen,             0,                       19),
        makeWidget     ({  3, (kListRowHeight * 2) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_HUNGER_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 2) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightRed,               67,                      100),
        makeWidget     ({  3, (kListRowHeight * 3) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_THIRST_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 3) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightRed,               67,                      100),
        makeWidget     ({  3, (kListRowHeight * 4) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_NAUSEA_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 4) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightRed,               47,                      100),
        makeWidget     ({  3, (kListRowHeight * 5) + 4 + 43 }, { 62,  10 }, WidgetType::label,               WindowColour::secondary, STR_GUEST_STAT_TOILET_LABEL),
        makeProgressBar({ 65, (kListRowHeight * 5) + 4 + 43 }, { 119, 10 }, Drawing::Colour::brightRed,               62,                      100),
        makeWidget     ({  3, (kListRowHeight * 7) + 9 + 43 }, { 180, 2  }, WidgetType::horizontalSeparator, WindowColour::secondary)
    );

    static constexpr auto _guestWindowWidgetsRides = makeWidgets(
        kMainGuestWidgets,
        makeWidget({ 3, 45 }, { 186, 10 }, WidgetType::label,  WindowColour::secondary, STR_GUEST_LABEL_RIDES_BEEN_ON),
        makeWidget({ 3, 57 }, { 186, 87 }, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL)
    );

    static constexpr auto _guestWindowWidgetsFinance = makeWidgets(
        kMainGuestWidgets
    );

    static constexpr auto _guestWindowWidgetsThoughts = makeWidgets(
        kMainGuestWidgets,
        makeWidget({ 3, 45 }, { 186, 10 }, WidgetType::label,  WindowColour::secondary, STR_GUEST_RECENT_THOUGHTS_LABEL)
    );

    static constexpr auto _guestWindowWidgetsInventory = makeWidgets(
        kMainGuestWidgets,
        makeWidget({ 3, 45 }, { 186, 10 }, WidgetType::label,  WindowColour::secondary, STR_CARRYING)
    );

    static constexpr auto _guestWindowWidgetsDebug = makeWidgets(
        kMainGuestWidgets
    );

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
        void onOpen() override
        {
            setWidgets(_guestWindowWidgetsOverview);
            page = WINDOW_GUEST_OVERVIEW;
            currentFrame = 0;
            _marqueePosition = 0;
            pickedPeepFrame = 0;

            WindowSetResize(*this, kWindowSize, { 500, 450 });

            selectedListItem = -1;
        }

        void init(EntityId id)
        {
            number = id.ToUnderlying();
            page = -1; // Set Page to something invalid so that SetPage doesn't set audio on viewport
            setPage(WINDOW_GUEST_OVERVIEW);
        }

        void onClose() override
        {
            if (isToolActive(classification, number))
                ToolCancel();
        }

        void onMouseUp(WidgetIndex widx) override
        {
            switch (widx)
            {
                case WIDX_CLOSE:
                    close();
                    return;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                case WIDX_TAB_4:
                case WIDX_TAB_5:
                case WIDX_TAB_6:
                case WIDX_TAB_7:
                    setPage(widx - WIDX_TAB_1);
                    return;
            }

            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    onMouseUpOverview(widx);
                    break;
            }
        }
        void onMouseDown(WidgetIndex widx) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onMouseDownOverview(widx);
            }
        }
        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onDropdownOverview(widgetIndex, selectedIndex);
            }
        }
        void onResize() override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    onResizeOverview();
                    break;
                case WINDOW_GUEST_STATS:
                case WINDOW_GUEST_RIDES:
                case WINDOW_GUEST_FINANCE:
                case WINDOW_GUEST_THOUGHTS:
                case WINDOW_GUEST_INVENTORY:
                case WINDOW_GUEST_DEBUG:
                    onResizeCommon();
                    break;
            }
        }
        void onUpdate() override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    onUpdateOverview();
                    break;
                case WINDOW_GUEST_STATS:
                    onUpdateStats();
                    break;
                case WINDOW_GUEST_RIDES:
                    onUpdateRides();
                    break;
                case WINDOW_GUEST_FINANCE:
                    onUpdateFinance();
                    break;
                case WINDOW_GUEST_THOUGHTS:
                    onUpdateThoughts();
                    break;
                case WINDOW_GUEST_INVENTORY:
                    onUpdateInventory();
                    break;
                case WINDOW_GUEST_DEBUG:
                    onUpdateDebug();
                    break;
            }
        }
        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onToolUpdateOverview(widgetIndex, screenCoords);
            }
        }
        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onToolDownOverview(widgetIndex, screenCoords);
            }
        }
        void onToolAbort(WidgetIndex widgetIndex) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onToolAbortOverview(widgetIndex);
            }
        }
        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onTextInputOverview(widgetIndex, text);
            }
        }
        void onViewportRotate() override
        {
            if (page == WINDOW_GUEST_OVERVIEW)
            {
                onViewportRotateOverview();
            }
        }
        void onDraw(RenderTarget& rt) override
        {
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    onDrawOverview(rt);
                    break;
                case WINDOW_GUEST_STATS:
                    onDrawStats(rt);
                    break;
                case WINDOW_GUEST_RIDES:
                    onDrawRides(rt);
                    break;
                case WINDOW_GUEST_FINANCE:
                    onDrawFinance(rt);
                    break;
                case WINDOW_GUEST_THOUGHTS:
                    onDrawThoughts(rt);
                    break;
                case WINDOW_GUEST_INVENTORY:
                    onDrawInventory(rt);
                    break;
                case WINDOW_GUEST_DEBUG:
                    onDrawDebug(rt);
                    break;
            }
        }
        void onPrepareDraw() override
        {
            onPrepareDrawCommon();
            switch (page)
            {
                case WINDOW_GUEST_OVERVIEW:
                    onPrepareDrawOverview();
                    break;
                case WINDOW_GUEST_RIDES:
                    onPrepareDrawRides();
                    break;
            }
        }
        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                return onScrollGetSizeRides(scrollIndex);
            }
            return {};
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                onScrollMouseOverRides(scrollIndex, screenCoords);
            }
        }
        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                onScrollMouseDownRides(scrollIndex, screenCoords);
            }
        }
        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (page == WINDOW_GUEST_RIDES)
            {
                onScrollDrawRides(scrollIndex, rt);
            }
        }

    private:
        Guest* GetGuest()
        {
            auto guest = getGameState().entities.GetEntity<Guest>(EntityId::FromUnderlying(number));
            if (guest == nullptr)
            {
                close();
                return nullptr;
            }
            return guest;
        }

        void onResizeCommon()
        {
            // Get page specific min and max size
            auto minSize = _guestWindowPageSizes[page][0];
            auto maxSize = _guestWindowPageSizes[page][1];

            // Ensure min size is large enough for all tabs to fit
            for (int32_t i = WIDX_TAB_1; i <= WIDX_TAB_7; i++)
            {
                if (!widgetIsDisabled(*this, i))
                {
                    minSize.width = std::max(minSize.width, widgets[i].right + 3);
                }
            }
            maxSize.width = std::max(minSize.width, maxSize.width);

            WindowSetResize(*this, minSize, maxSize);
        }

        void onPrepareDrawCommon()
        {
            pressedWidgets |= 1uLL << (page + WIDX_TAB_1);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            auto ft = Formatter::Common();
            peep->FormatNameTo(ft);

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
                if (widgetIsDisabled(*this, WIDX_PICKUP))
                    invalidate();
            }
            else
            {
                newDisabledWidgets = (1uLL << WIDX_PICKUP);
                if (!widgetIsDisabled(*this, WIDX_PICKUP))
                    invalidate();
            }
            if (getGameState().park.flags & PARK_FLAGS_NO_MONEY)
            {
                newDisabledWidgets |= (1uLL << WIDX_TAB_4); // Disable finance tab if no money
            }
            if (!Config::Get().general.debuggingTools)
            {
                newDisabledWidgets |= (1uLL << WIDX_TAB_7); // Disable debug tab when debug tools not turned on
            }
            disabledWidgets = newDisabledWidgets;
        }

        void setPage(int32_t newPage)
        {
            if (isToolActive(classification, number))
                ToolCancel();

            bool listen = false;
            if (newPage == WINDOW_GUEST_OVERVIEW && page == WINDOW_GUEST_OVERVIEW && viewport != nullptr)
            {
                viewport->flags ^= VIEWPORT_FLAG_SOUND_ON;
                listen = (viewport->flags & VIEWPORT_FLAG_SOUND_ON) != 0;
            }

            // Skip setting page if we're already on this page, unless we're initialising the window
            if (page == newPage && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            _riddenRides.clear();
            selectedListItem = -1;

            removeViewport();

            holdDownWidgets = 0;
            pressedWidgets = 0;
            setWidgets(_guestWindowPageWidgets[page]);
            DisableWidgets();
            invalidate();
            onResize();
            onPrepareDraw();
            initScrollWidgets();
            invalidate();

            if (listen && viewport != nullptr)
                viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }

#pragma region Overview

        void OverviewTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_1))
                return;

            const auto& widget = widgets[WIDX_TAB_1];
            int32_t widgWidth = widget.width() - 2;
            int32_t widgHeight = widget.height() - 2;
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
            if (page == WINDOW_GUEST_OVERVIEW)
                widgHeight++;

            RenderTarget clipRT;
            if (!ClipRenderTarget(clipRT, rt, screenCoords, widgWidth, widgHeight))
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

            int32_t animationFrame = animObj->GetPeepAnimation(peep->AnimationGroup).baseImage + 1;
            int32_t animationFrameOffset = 0;

            if (page == WINDOW_GUEST_OVERVIEW)
            {
                animationFrameOffset = _guestAnimationFrame;
                animationFrameOffset &= 0xFFFC;
            }
            animationFrame += animationFrameOffset;

            auto spriteId = ImageId(animationFrame, peep->TshirtColour, peep->TrousersColour);
            GfxDrawSprite(clipRT, spriteId, screenCoords);

            auto* guest = peep->As<Guest>();
            if (guest == nullptr)
                return;

            // There are only 6 walking frames available for each item.
            auto itemFrame = (_guestAnimationFrame / 4) % 6;

            if (guest->AnimationGroup == PeepAnimationGroup::hat)
            {
                auto itemOffset = kPeepSpriteHatItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->HatColour);
                GfxDrawSprite(clipRT, imageId, screenCoords);
            }

            if (guest->AnimationGroup == PeepAnimationGroup::balloon)
            {
                auto itemOffset = kPeepSpriteBalloonItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->BalloonColour);
                GfxDrawSprite(clipRT, imageId, screenCoords);
            }

            if (guest->AnimationGroup == PeepAnimationGroup::umbrella)
            {
                auto itemOffset = kPeepSpriteUmbrellaItemStart + 1;
                auto imageId = ImageId(itemOffset + itemFrame * 4, guest->UmbrellaColour);
                GfxDrawSprite(clipRT, imageId, screenCoords);
            }
        }

        void onResizeOverview()
        {
            DisableWidgets();
            onPrepareDraw();
            invalidateWidget(WIDX_MARQUEE);
            onResizeCommon();

            if (viewport != nullptr)
            {
                const auto& widget = widgets[WIDX_VIEWPORT];
                const auto reqViewportWidth = widget.width() - 2;
                const auto reqViewportHeight = widget.height() - 2;
                viewport->pos = windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
                if (viewport->width != reqViewportWidth || viewport->height != reqViewportHeight)
                {
                    viewport->width = reqViewportWidth;
                    viewport->height = reqViewportHeight;
                }
            }
            onViewportRotate();
        }

        void onMouseUpOverview(WidgetIndex widgetIndex)
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            auto& gameState = getGameState();
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
                    GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Pickup,
                                                                EntityId::FromUnderlying(number), nullLoc,
                                                                Network::GetCurrentPlayerId() };
                    pickupAction.SetCallback(
                        [peepnum = number](const GameActions::GameAction* ga, const GameActions::Result* result) {
                            if (result->error != GameActions::Status::ok)
                                return;
                            auto* windowMgr = GetWindowManager();
                            WindowBase* wind = windowMgr->FindByNumber(WindowClass::peep, peepnum);
                            if (wind != nullptr)
                            {
                                ToolSet(*wind, WC_PEEP__WIDX_PICKUP, Tool::picker);
                            }
                        });
                    GameActions::Execute(&pickupAction, gameState);
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

                    auto guestSetFlagsAction = GameActions::GuestSetFlagsAction(EntityId::FromUnderlying(number), guestFlags);
                    GameActions::Execute(&guestSetFlagsAction, gameState);
                }
                break;
            }
        }

        void onMouseDownOverview(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                    ShowLocateDropdown(widgets[widgetIndex]);
                    break;
            }
        }

        void onDropdownOverview(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                {
                    if (dropdownIndex == 0)
                    {
                        scrollToViewport();
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
            std::array<Dropdown::Item, 2> dropdownItems = {
                Dropdown::PlainMenuLabel(STR_LOCATE_SUBJECT_TIP),
                Dropdown::PlainMenuLabel(STR_FOLLOW_SUBJECT_TIP),
            };

            WindowDropdownShowText(
                { windowPos.x + widget.left, windowPos.y + widget.top }, widget.height(), colours[1], 0, dropdownItems);
            gDropdown.defaultIndex = 0;
        }

        void GuestFollow()
        {
            WindowBase* main = WindowGetMain();
            WindowFollowSprite(*main, EntityId::FromUnderlying(number));
        }

        void onViewportRotateOverview()
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
                removeViewport();
            }

            onPrepareDraw();

            if (peep->State != PeepState::picked && viewport == nullptr)
            {
                const auto& viewWidget = widgets[WIDX_VIEWPORT];
                auto screenPos = ScreenCoordsXY{ viewWidget.left + 1 + windowPos.x, viewWidget.top + 1 + windowPos.y };
                int32_t widgWidth = viewWidget.width() - 2;
                int32_t widgHeight = viewWidget.height() - 2;

                ViewportCreate(*this, screenPos, widgWidth, widgHeight, focus.value());
                if (viewport != nullptr && reCreateViewport)
                {
                    viewport->flags = origViewportFlags;
                }
                flags |= WindowFlag::noScrolling;
                invalidate();
            }
            invalidate();
        }

        void onDrawOverview(RenderTarget& rt)
        {
            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

            // Draw the viewport no sound sprite
            if (viewport != nullptr)
            {
                WindowDrawViewport(rt, *this);
                if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
                {
                    GfxDrawSprite(rt, ImageId(SPR_HEARING_VIEWPORT), WindowGetViewportSoundIconPos(*this));
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
                int32_t textWidth = actionLabelWidget.width() - 1;
                DrawTextEllipsised(rt, screenPos, textWidth, STR_BLACK_STRING, ft, { TextAlignment::centre });
            }

            // Draw the marquee thought
            const auto& marqueeWidget = widgets[WIDX_MARQUEE];
            auto marqWidth = marqueeWidget.width() - 4;
            int32_t left = marqueeWidget.left + 2 + windowPos.x;
            int32_t top = marqueeWidget.top + windowPos.y;
            int32_t marqHeight = marqueeWidget.height() - 1;
            RenderTarget rtMarquee;
            if (!ClipRenderTarget(rtMarquee, rt, { left, top }, marqWidth, marqHeight))
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

            screenPos.x = marqueeWidget.width() - 1 - _marqueePosition;
            {
                auto ft = Formatter();
                PeepThoughtSetFormatArgs(&peep->Thoughts[i], ft);
                DrawTextBasic(rtMarquee, { screenPos.x, 0 }, STR_WINDOW_COLOUR_2_STRINGID, ft, { FontStyle::small });
            }
        }

        void onPrepareDrawOverview()
        {
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            pressedWidgets &= ~(1uLL << WIDX_TRACK);
            if (peep->PeepFlags & PEEP_FLAGS_TRACKING)
            {
                pressedWidgets |= (1uLL << WIDX_TRACK);
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

        void onUpdateOverview()
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
            const auto& pickAnim = animObj->GetPeepAnimation(peep->AnimationGroup, PeepAnimationType::hanging);
            const auto pickAnimLength = pickAnim.frameOffsets.size();

            // Update pickup animation, can only happen in this tab.
            pickedPeepFrame++;
            pickedPeepFrame %= pickAnimLength * 4;

            invalidateWidget(WIDX_TAB_1);
            invalidateWidget(WIDX_TAB_2);

            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_ACTION)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_ACTION;
                invalidateWidget(WIDX_ACTION_LBL);
            }

            _marqueePosition += 2;

            _beingWatchedTimer++;

            // Disable peep watching thought for multiplayer as it's client specific
            if (Network::GetMode() == Network::Mode::none)
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

            const std::optional<Focus> currentFocus = peep->State != PeepState::picked ? std::optional(Focus(peep->Id))
                                                                                       : std::nullopt;
            // Check if guest is in a vehicle (on ride, entering, or leaving but still on vehicle)
            auto isGuestInVehicle = [&peep]() {
                return peep->State == PeepState::onRide || peep->State == PeepState::enteringRide
                    || (peep->State == PeepState::leavingRide && peep->x == kLocationNull);
            };

            // Also update when guest is on a ride but viewport still points to the guest (not the vehicle)
            bool viewportNeedsVehicleUpdate = isGuestInVehicle() && viewportTargetSprite == EntityId::FromUnderlying(number);

            if (focus != currentFocus || viewportNeedsVehicleUpdate)
            {
                onViewportRotate();
            }

            for (const auto& thought : peep->Thoughts)
            {
                if (thought.freshness == 1 || thought.freshness == 2)
                {
                    invalidateWidget(WIDX_MARQUEE);
                    break;
                }
            }
        }

        void onTextInputOverview(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex != WIDX_RENAME)
                return;

            if (text.empty())
                return;
            std::string sText(text);
            auto gameAction = GameActions::GuestSetNameAction(EntityId::FromUnderlying(number), sText);
            GameActions::Execute(&gameAction, getGameState());
        }

        void onToolUpdateOverview(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            gMapSelectFlags.unset(MapSelectFlag::enable);

            auto mapCoords = FootpathGetCoordinatesFromPos({ screenCoords.x, screenCoords.y + 16 }, nullptr, nullptr);
            if (!mapCoords.IsNull())
            {
                gMapSelectFlags.set(MapSelectFlag::enable);
                gMapSelectType = MapSelectType::full;
                gMapSelectPositionA = mapCoords;
                gMapSelectPositionB = mapCoords;
            }

            gPickupPeepImage = ImageId();

            auto info = GetMapCoordinatesFromPos(screenCoords, kViewportInteractionItemAll);
            if (info.interactionType == ViewportInteractionItem::none)
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

            auto baseImageId = animObj->GetPeepAnimation(peep->AnimationGroup, PeepAnimationType::hanging).baseImage;
            baseImageId += pickedPeepFrame >> 2;
            gPickupPeepImage = ImageId(baseImageId, peep->TshirtColour, peep->TrousersColour);
        }

        void onToolDownOverview(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            TileElement* tileElement;
            auto destCoords = FootpathGetCoordinatesFromPos({ screenCoords.x, screenCoords.y + 16 }, nullptr, &tileElement);

            if (destCoords.IsNull())
                return;

            GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Place,
                                                        EntityId::FromUnderlying(number),
                                                        { destCoords, tileElement->GetBaseZ() },
                                                        Network::GetCurrentPlayerId() };
            pickupAction.SetCallback([](const GameActions::GameAction* ga, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                    return;
                ToolCancel();
                gPickupPeepImage = ImageId();
            });
            GameActions::Execute(&pickupAction, getGameState());
        }

        void onToolAbortOverview(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Cancel,
                                                        EntityId::FromUnderlying(number),
                                                        { _pickedPeepX, 0, 0 },
                                                        Network::GetCurrentPlayerId() };
            GameActions::Execute(&pickupAction, getGameState());
        }

#pragma endregion

#pragma region Stats
        void StatsTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_2))
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
                        imageId += (currentFrame / 4) & 0xF;
                        break;
                    case SPR_PEEP_LARGE_FACE_VERY_SICK_0:
                        imageId += (currentFrame / 8) & 0x3;
                        break;
                    case SPR_PEEP_LARGE_FACE_ANGRY_0:
                        imageId += (currentFrame / 8) & 0x3;
                        break;
                }
            }
            GfxDrawSprite(rt, ImageId(imageId), screenCoords);
        }

        void onUpdateStats()
        {
            currentFrame++;
            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_STATS;

            invalidate();
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

        void onDrawStats(RenderTarget& rt)
        {
            // ebx
            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            int32_t happinessPercentage = NormalizeGuestStatValue(peep->Happiness, kPeepMaxHappiness, 3);
            widgetProgressBarSetNewPercentage(widgets[WIDX_HAPPINESS_BAR], happinessPercentage);

            int32_t energyPercentage = NormalizeGuestStatValue(
                peep->Energy - kPeepMinEnergy, kPeepMaxEnergy - kPeepMinEnergy, 3);
            widgetProgressBarSetNewPercentage(widgets[WIDX_ENERGY_BAR], energyPercentage);

            int32_t hungerPercentage = NormalizeGuestStatValue(peep->Hunger - 32, 158, 0);
            hungerPercentage = 100 - hungerPercentage; // the bar should be longer when peep->Hunger is low
            widgetProgressBarSetNewPercentage(widgets[WIDX_HUNGER_BAR], hungerPercentage);

            int32_t thirstPercentage = NormalizeGuestStatValue(peep->Thirst - 32, 158, 0);
            thirstPercentage = 100 - thirstPercentage; // the bar should be longer when peep->Thirst is low
            widgetProgressBarSetNewPercentage(widgets[WIDX_THIRST_BAR], thirstPercentage);

            int32_t nauseaPercentage = NormalizeGuestStatValue(peep->Nausea - 32, 223, 0);
            widgetProgressBarSetNewPercentage(widgets[WIDX_NAUSEA_BAR], nauseaPercentage);

            int32_t toiletPercentage = NormalizeGuestStatValue(peep->Toilet - 64, 178, 0);
            widgetProgressBarSetNewPercentage(widgets[WIDX_TOILET_BAR], toiletPercentage);

            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4,
                                  widgets[WIDX_PAGE_BACKGROUND].top + (kListRowHeight * 6) + 5 };

            // Time in park
            int32_t guestEntryTime = peep->GetParkEntryTime();
            if (guestEntryTime != -1)
            {
                int32_t timeInPark = (getGameState().currentTicks - guestEntryTime) >> 11;
                auto ft = Formatter();
                ft.Add<uint16_t>(timeInPark & 0xFFFF);
                DrawTextBasic(rt, screenCoords, STR_GUEST_STAT_TIME_IN_PARK, ft);
            }

            screenCoords.y += kListRowHeight + 9;

            // Preferred Ride
            DrawTextBasic(rt, screenCoords, STR_GUEST_STAT_PREFERRED_RIDE);
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

                DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ 4, 0 }, string_id, ft);
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
                DrawTextBasic(rt, screenCoords, STR_GUEST_STAT_NAUSEA_TOLERANCE, ft);
            }
        }

#pragma endregion

#pragma region Rides
        void RidesTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_3))
                return;

            const auto& widget = widgets[WIDX_TAB_3];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_RIDE_0;

            if (page == WINDOW_GUEST_RIDES)
            {
                imageId += (currentFrame / 4) & 0xF;
            }

            GfxDrawSprite(rt, ImageId(imageId), screenCoords);
        }

        void onUpdateRides()
        {
            currentFrame++;

            invalidateWidget(WIDX_TAB_2);
            invalidateWidget(WIDX_TAB_3);

            const auto guest = GetGuest();
            if (guest == nullptr)
            {
                return;
            }

            // Every 2048 ticks do a full window_invalidate
            int32_t numTicks = getGameState().currentTicks - guest->GetParkEntryTime();
            if (!(numTicks & 0x7FF))
                invalidate();

            const auto oldSize = _riddenRides.size();
            _riddenRides.clear();

            const auto& gameState = getGameState();
            for (const auto& r : RideManager(gameState))
            {
                if (r.isRide() && guest->HasRidden(r))
                {
                    _riddenRides.push_back(r.id);
                }
            }

            // If there are new items
            if (oldSize != _riddenRides.size())
            {
                invalidate();
            }
        }

        ScreenSize onScrollGetSizeRides(int32_t scrollIndex)
        {
            ScreenSize newSize;
            newSize.height = static_cast<int32_t>(_riddenRides.size()) * 10;

            if (selectedListItem != -1)
            {
                selectedListItem = -1;
                invalidate();
            }

            int32_t visableHeight = newSize.height - widgets[WIDX_RIDE_SCROLL].bottom + widgets[WIDX_RIDE_SCROLL].top + 21;

            if (visableHeight < 0)
                visableHeight = 0;

            if (visableHeight < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = visableHeight;
                invalidate();
            }
            return newSize;
        }

        void onScrollMouseDownRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            auto index = screenCoords.y / 10;
            if (index >= static_cast<int32_t>(_riddenRides.size()))
                return;

            auto intent = Intent(WindowClass::ride);
            intent.PutExtra(INTENT_EXTRA_RIDE_ID, _riddenRides[index]);
            ContextOpenIntent(&intent);
        }

        void onScrollMouseOverRides(int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
        {
            auto index = screenCoords.y / 10;
            if (index >= static_cast<int32_t>(_riddenRides.size()))
                return;

            if (index == selectedListItem)
                return;
            selectedListItem = index;

            invalidate();
        }

        void onPrepareDrawRides()
        {
            widgets[WIDX_RIDE_SCROLL].right = width - 4;
            widgets[WIDX_RIDE_SCROLL].bottom = height - 15;
        }

        void onDrawRides(RenderTarget& rt)
        {
            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            // cx dx
            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 2, widgets[WIDX_PAGE_BACKGROUND].top + 2 };

            screenCoords.y = windowPos.y + widgets[WIDX_PAGE_BACKGROUND].bottom - 12;

            auto ft = Formatter();
            auto* r = GetRide(peep->FavouriteRide);
            if (r != nullptr)
            {
                r->formatNameTo(ft);
            }
            else
            {
                ft.Add<StringId>(STR_PEEP_FAVOURITE_RIDE_NOT_AVAILABLE);
            }

            DrawTextEllipsised(rt, screenCoords, width - 14, STR_FAVOURITE_RIDE, ft);
        }

        void onScrollDrawRides(int32_t scrollIndex, RenderTarget& rt)
        {
            auto colour = getColourMap(colours[1].colour).midLight;
            Rectangle::fill(rt, { { rt.x, rt.y }, { rt.x + rt.width - 1, rt.y + rt.height - 1 } }, colour);

            for (int32_t listIndex = 0; listIndex < static_cast<int32_t>(_riddenRides.size()); listIndex++)
            {
                int32_t y = listIndex * 10;
                StringId stringId = STR_BLACK_STRING;
                if (listIndex == selectedListItem)
                {
                    Rectangle::filter(rt, { 0, y, 800, y + 9 }, FilterPaletteID::paletteDarken1);
                    stringId = STR_WINDOW_COLOUR_2_STRINGID;
                }

                auto* r = GetRide(_riddenRides[listIndex]);
                if (r != nullptr)
                {
                    auto ft = Formatter();
                    r->formatNameTo(ft);
                    DrawTextBasic(rt, { 0, y - 1 }, stringId, ft);
                }
            }
        }
#pragma endregion

#pragma region Finance
        void FinanceTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_4))
                return;

            const auto& widget = widgets[WIDX_TAB_4];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_FINANCES_SUMMARY_0;

            if (page == WINDOW_GUEST_FINANCE)
            {
                imageId += (currentFrame / 2) & 0x7;
            }

            GfxDrawSprite(rt, ImageId(imageId), screenCoords);
        }

        void onUpdateFinance()
        {
            currentFrame++;

            invalidateWidget(WIDX_TAB_2);
            invalidateWidget(WIDX_TAB_4);
        }

        void onDrawFinance(RenderTarget& rt)
        {
            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

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
                DrawTextBasic(rt, screenCoords, STR_GUEST_STAT_CASH_IN_POCKET, ft);
                screenCoords.y += kListRowHeight;
            }

            // Cash spent
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->CashSpent);
                DrawTextBasic(rt, screenCoords, STR_GUEST_STAT_CASH_SPENT, ft);
                screenCoords.y += kListRowHeight * 2;
            }

            Rectangle::fillInset(
                rt, { screenCoords - ScreenCoordsXY{ 0, 6 }, screenCoords + ScreenCoordsXY{ 179, -5 } }, colours[1],
                Rectangle::BorderStyle::inset);

            // Paid to enter
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidToEnter);
                DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_ENTRANCE_FEE, ft);
                screenCoords.y += kListRowHeight;
            }
            // Paid on rides
            {
                auto ft = Formatter();
                ft.Add<money64>(peep->PaidOnRides);
                ft.Add<uint16_t>(peep->GuestNumRides);
                if (peep->GuestNumRides != 1)
                {
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_RIDE_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_RIDE, ft);
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
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_FOOD_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_FOOD, ft);
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
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_DRINK_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_DRINK, ft);
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
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_SOUVENIR_PLURAL, ft);
                }
                else
                {
                    DrawTextBasic(rt, screenCoords, STR_GUEST_EXPENSES_SOUVENIR, ft);
                }
            }
        }
#pragma endregion

#pragma region Thoughts
        void ThoughtsTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_5))
                return;

            const auto& widget = widgets[WIDX_TAB_5];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_THOUGHTS_0;

            if (page == WINDOW_GUEST_THOUGHTS)
            {
                imageId += (currentFrame / 2) & 0x7;
            }

            GfxDrawSprite(rt, ImageId(imageId), screenCoords);
        }

        void onUpdateThoughts()
        {
            currentFrame++;

            invalidateWidget(WIDX_TAB_2);
            invalidateWidget(WIDX_TAB_5);

            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_THOUGHTS)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_THOUGHTS;
                invalidate();
            }
        }

        void onDrawThoughts(RenderTarget& rt)
        {
            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

            const auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }

            auto screenCoords = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PAGE_BACKGROUND].left + 4, widgets[WIDX_PAGE_BACKGROUND].top + 14 };

            for (const auto& thought : peep->Thoughts)
            {
                if (thought.type == PeepThoughtType::None)
                    return;
                if (thought.freshness == 0)
                    continue;

                int32_t widgWidth = widgets[WIDX_PAGE_BACKGROUND].right - widgets[WIDX_PAGE_BACKGROUND].left - 8;

                auto ft = Formatter();
                PeepThoughtSetFormatArgs(&thought, ft);
                screenCoords.y += DrawTextWrapped(rt, screenCoords, widgWidth, STR_BLACK_STRING, ft, { FontStyle::small });

                // If this is the last visible line end drawing.
                if (screenCoords.y > windowPos.y + widgets[WIDX_PAGE_BACKGROUND].bottom - 32)
                    return;
            }
        }
#pragma endregion

#pragma region Inventory
        void InventoryTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_6))
                return;

            const auto& widget = widgets[WIDX_TAB_6];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            GfxDrawSprite(rt, ImageId(SPR_TAB_GUEST_INVENTORY), screenCoords);
        }

        void onUpdateInventory()
        {
            currentFrame++;

            invalidateWidget(WIDX_TAB_2);
            invalidateWidget(WIDX_TAB_6);

            auto peep = GetGuest();
            if (peep == nullptr)
            {
                return;
            }
            if (peep->WindowInvalidateFlags & PEEP_INVALIDATE_PEEP_INVENTORY)
            {
                peep->WindowInvalidateFlags &= ~PEEP_INVALIDATE_PEEP_INVENTORY;
                invalidate();
            }
        }

        std::pair<ImageId, Formatter> InventoryFormatItem(Guest& guest, ShopItem item) const
        {
            auto parkName = getGameState().park.name.c_str();

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
                case ShopItem::balloon:
                    itemImage = ImageId(itemDesc.Image, guest.BalloonColour);
                    break;
                case ShopItem::photo:
                    invRide = GetRide(guest.Photo1RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->formatNameTo(ft);
                    }

                    break;
                case ShopItem::umbrella:
                    itemImage = ImageId(itemDesc.Image, guest.UmbrellaColour);
                    break;
                case ShopItem::voucher:
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
                                invRide->formatNameTo(ft);
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
                case ShopItem::hat:
                    itemImage = ImageId(itemDesc.Image, guest.HatColour);
                    break;
                case ShopItem::tShirt:
                    itemImage = ImageId(itemDesc.Image, guest.TshirtColour);
                    break;
                case ShopItem::photo2:
                    invRide = GetRide(guest.Photo2RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->formatNameTo(ft);
                    }
                    break;
                case ShopItem::photo3:
                    invRide = GetRide(guest.Photo3RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->formatNameTo(ft);
                    }
                    break;
                case ShopItem::photo4:
                    invRide = GetRide(guest.Photo4RideRef);
                    if (invRide != nullptr)
                    {
                        ft.Rewind();
                        ft.Increment(2);
                        invRide->formatNameTo(ft);
                    }
                    break;
                default:
                    // This switch handles only items visible in peep window
                    break;
            }

            return std::make_pair(itemImage, ft);
        }

        void onDrawInventory(RenderTarget& rt)
        {
            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

            const auto guest = GetGuest();
            if (guest == nullptr)
            {
                return;
            }

            auto& widget = widgets[WIDX_PAGE_BACKGROUND];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 4, widget.top + 12 };
            int32_t itemNameWidth = widget.width() - 25;

            int32_t maxY = windowPos.y + height - 22;
            int32_t numItems = 0;

            for (ShopItem item = ShopItem::balloon; item < ShopItem::count; item++)
            {
                if (screenCoords.y >= maxY)
                    break;
                if (!guest->HasItem(item))
                    continue;

                auto [imageId, ft] = InventoryFormatItem(*guest, item);
                GfxDrawSprite(rt, imageId, screenCoords);

                screenCoords.x += 16;
                screenCoords.y += 1;
                screenCoords.y += DrawTextWrapped(rt, screenCoords, itemNameWidth, STR_BLACK_STRING, ft);

                screenCoords.x -= 16;
                numItems++;
            }

            if (numItems == 0)
            {
                DrawTextBasic(rt, screenCoords, STR_NOTHING);
            }
        }
#pragma endregion

#pragma region Debug
        void DebugTabDraw(RenderTarget& rt)
        {
            if (widgetIsDisabled(*this, WIDX_TAB_7))
                return;

            const auto& widget = widgets[WIDX_TAB_7];
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left, widget.top };

            int32_t imageId = SPR_TAB_GEARS_0;
            if (page == WINDOW_GUEST_DEBUG)
            {
                imageId += (currentFrame / 2) & 0x3;
            }

            GfxDrawSprite(rt, ImageId(imageId), screenCoords);
        }

        void onUpdateDebug()
        {
            currentFrame++;
            invalidate();
        }

        void onDrawDebug(RenderTarget& rt)
        {
            char buffer[512]{};
            char buffer2[512]{};

            drawWidgets(rt);
            OverviewTabDraw(rt);
            StatsTabDraw(rt);
            RidesTabDraw(rt);
            FinanceTabDraw(rt);
            ThoughtsTabDraw(rt);
            InventoryTabDraw(rt);
            DebugTabDraw(rt);

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
                DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_SPRITE_INDEX, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->x);
                ft.Add<int32_t>(peep->y);
                ft.Add<int32_t>(peep->z);
                DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_POSITION, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->NextLoc.x);
                ft.Add<int32_t>(peep->NextLoc.y);
                ft.Add<int32_t>(peep->NextLoc.z);
                FormatStringLegacy(buffer, sizeof(buffer), STR_PEEP_DEBUG_NEXT, ft.Data());
                if (peep->GetNextIsSurface())
                {
                    FormatStringLegacy(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SURFACE, nullptr);
                    String::safeConcat(buffer, buffer2, sizeof(buffer));
                }
                if (peep->GetNextIsSloped())
                {
                    auto ft2 = Formatter();
                    ft2.Add<int32_t>(peep->GetNextDirection());
                    FormatStringLegacy(buffer2, sizeof(buffer2), STR_PEEP_DEBUG_NEXT_SLOPE, ft2.Data());
                    String::safeConcat(buffer, buffer2, sizeof(buffer));
                }
                DrawText(rt, screenCoords, {}, buffer);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->DestinationX);
                ft.Add<int32_t>(peep->DestinationY);
                ft.Add<int32_t>(peep->DestinationTolerance);
                DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_DEST, ft);
            }
            screenCoords.y += kListRowHeight;
            {
                auto ft = Formatter();
                ft.Add<int32_t>(peep->PathfindGoal.x);
                ft.Add<int32_t>(peep->PathfindGoal.y);
                ft.Add<int32_t>(peep->PathfindGoal.z);
                ft.Add<int32_t>(peep->PathfindGoal.direction);
                DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_PATHFIND_GOAL, ft);
            }
            screenCoords.y += kListRowHeight;
            DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_PATHFIND_HISTORY);
            screenCoords.y += kListRowHeight;

            screenCoords.x += 10;
            for (auto& point : peep->PathfindHistory)
            {
                auto ft = Formatter();
                ft.Add<int32_t>(point.x);
                ft.Add<int32_t>(point.y);
                ft.Add<int32_t>(point.z);
                ft.Add<int32_t>(point.direction);
                DrawTextBasic(rt, screenCoords, STR_PEEP_DEBUG_PATHFIND_HISTORY_ITEM, ft);
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
        auto* window = static_cast<GuestWindow*>(windowMgr->BringToFrontByNumber(WindowClass::peep, peep->Id.ToUnderlying()));
        if (window == nullptr)
        {
            auto windowSize = kWindowSize;
            if (Config::Get().general.debuggingTools)
                windowSize.width += kTabWidth;

            window = windowMgr->Create<GuestWindow>(WindowClass::peep, windowSize, WindowFlag::resizable);
            if (window == nullptr)
            {
                return nullptr;
            }
        }

        window->init(peep->Id);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
