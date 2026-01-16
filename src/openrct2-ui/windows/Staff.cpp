/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Theme.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportQuery.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/actions/StaffSetCostumeAction.h>
#include <openrct2/actions/StaffSetNameAction.h>
#include <openrct2/actions/StaffSetOrdersAction.h>
#include <openrct2/actions/StaffSetPatrolAreaAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/PatrolArea.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Finance.h>
#include <openrct2/network/Network.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/peep/PeepAnimations.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_STRINGID;

    static constexpr ScreenSize kWindowSize = { 190, 180 };

    enum WindowStaffPage
    {
        WINDOW_STAFF_OVERVIEW,
        WINDOW_STAFF_OPTIONS,
        WINDOW_STAFF_STATISTICS,
        WINDOW_STAFF_PAGE_COUNT,
    };

    enum WindowStaffWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_RESIZE,
        WIDX_TAB_1,
        WIDX_TAB_2,
        WIDX_TAB_3,

        WIDX_VIEWPORT = 7,
        WIDX_BTM_LABEL,
        WIDX_PICKUP,
        WIDX_PATROL,
        WIDX_RENAME,
        WIDX_LOCATE,
        WIDX_FIRE,

        WIDX_CHECKBOX_1 = 7,
        WIDX_CHECKBOX_2,
        WIDX_CHECKBOX_3,
        WIDX_CHECKBOX_4,
        WIDX_COSTUME_BOX,
        WIDX_COSTUME_BTN,
    };

    VALIDATE_GLOBAL_WIDX(WC_PEEP, WIDX_PATROL);
    VALIDATE_GLOBAL_WIDX(WC_STAFF, WIDX_PICKUP);

    // clang-format off
    static constexpr auto kMainStaffWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ 0, 43 }, { 190, 137 }, WidgetType::resize, WindowColour::secondary),
        makeTab({ 3, 17 }, STR_STAFF_OVERVIEW_TIP),
        makeTab({ 34, 17 }, STR_STAFF_OPTIONS_TIP),
        makeTab({ 65, 17 }, STR_STAFF_STATS_TIP)
    );

    static constexpr auto _staffOverviewWidgets = makeWidgets(
        kMainStaffWidgets,
        makeWidget     ({ 3, 47},                      {162, 120}, WidgetType::viewport,      WindowColour::secondary                                                 ), // Viewport
        makeWidget     ({ 3, kWindowSize.height - 13}, {162,  11}, WidgetType::labelCentred, WindowColour::secondary                                                  ), // Label at bottom of viewport
        makeWidget     ({kWindowSize.width - 25,  45}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_PICKUP_BTN), STR_PICKUP_TIP        ), // Pickup Button
        makeWidget     ({kWindowSize.width - 25,  69}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_PATROL_BTN), STR_SET_PATROL_TIP    ), // Patrol Button
        makeWidget     ({kWindowSize.width - 25,  93}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_RENAME),     STR_NAME_STAFF_TIP    ), // Rename Button
        makeWidget     ({kWindowSize.width - 25, 117}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_LOCATE),     STR_LOCATE_SUBJECT_TIP), // Locate Button
        makeWidget     ({kWindowSize.width - 25, 141}, { 24,  24}, WidgetType::flatBtn,       WindowColour::secondary, ImageId(SPR_DEMOLISH),   STR_FIRE_STAFF_TIP    )  // Fire Button
    );

    //0x9AF910
    static constexpr auto _staffOptionsWidgets = makeWidgets(
        kMainStaffWidgets,
        makeWidget     ({      5,  50},                {180,  12}, WidgetType::checkbox,     WindowColour::secondary                                            ), // Checkbox 1
        makeWidget     ({      5,  67},                {180,  12}, WidgetType::checkbox,     WindowColour::secondary                                            ), // Checkbox 2
        makeWidget     ({      5,  84},                {180,  12}, WidgetType::checkbox,     WindowColour::secondary                                            ), // Checkbox 3
        makeWidget     ({      5, 101},                {180,  12}, WidgetType::checkbox,     WindowColour::secondary                                            ), // Checkbox 4
        makeWidget     ({      5,  50},                {180,  12}, WidgetType::dropdownMenu, WindowColour::secondary                                            ), // Costume Dropdown
        makeWidget     ({kWindowSize.width - 17,  51}, { 11,  10}, WidgetType::button,       WindowColour::secondary, STR_DROPDOWN_GLYPH, STR_SELECT_COSTUME_TIP) // Costume Dropdown Button
    );

    // 0x9AF9F4
    static constexpr auto _staffStatsWidgets = makeWidgets(
        kMainStaffWidgets
    );
    // clang-format on

    static constexpr std::span<const Widget> window_staff_page_widgets[] = {
        _staffOverviewWidgets,
        _staffOptionsWidgets,
        _staffStatsWidgets,
    };

    class StaffWindow final : public Window
    {
    private:
        std::vector<AvailableCostume> _availableCostumes;
        uint16_t _tabAnimationOffset = 0;
        int32_t _pickedPeepOldX = kLocationNull;

    public:
        void initialise(EntityId entityId)
        {
            number = entityId.ToUnderlying();
            auto* staff = GetStaff();
            if (staff == nullptr)
                return;

            if (staff->isEntertainer())
                _availableCostumes = getAvailableCostumeStrings(AnimationPeepType::entertainer);

            ViewportInit();
        }

        void onOpen() override
        {
            setPage(WINDOW_STAFF_OVERVIEW);
        }

        void onClose() override
        {
            CancelTools();
        }

        void onLanguageChange() override
        {
            auto* staff = GetStaff();
            if (staff == nullptr)
                return;

            if (staff->isEntertainer())
                _availableCostumes = getAvailableCostumeStrings(AnimationPeepType::entertainer);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            if (widgetIndex <= WIDX_TAB_3)
                CommonMouseUp(widgetIndex);
            else
            {
                switch (page)
                {
                    case WINDOW_STAFF_OVERVIEW:
                        OverviewMouseUp(widgetIndex);
                        break;
                    case WINDOW_STAFF_OPTIONS:
                        OptionsMouseUp(widgetIndex);
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewOnMouseDown(widgetIndex);
                    break;
                case WINDOW_STAFF_OPTIONS:
                    OptionsOnMouseDown(widgetIndex);
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewOnDropdown(widgetIndex, dropdownIndex);
                    break;
                case WINDOW_STAFF_OPTIONS:
                    OptionsOnDropdown(widgetIndex, dropdownIndex);
                    break;
            }
        }

        void onPrepareDraw() override
        {
            CommonPrepareDrawBefore();

            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewPrepareDraw();
                    break;
                case WINDOW_STAFF_OPTIONS:
                    OptionsPrepareDraw();
                    break;
            }

            CommonPrepareDrawAfter();
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewDraw(rt);
                    break;
                case WINDOW_STAFF_STATISTICS:
                    StatsDraw(rt);
                    break;
            }
        }

        void onResize() override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewResize();
                    break;
                case WINDOW_STAFF_OPTIONS:
                    OptionsResize();
                    break;
                case WINDOW_STAFF_STATISTICS:
                    StatsResize();
                    break;
            }
        }

        void onUpdate() override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewUpdate();
                    break;
                case WINDOW_STAFF_OPTIONS:
                    OptionsUpdate();
                    break;
                case WINDOW_STAFF_STATISTICS:
                    StatsUpdate();
                    break;
            }
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewToolUpdate(widgetIndex, screenCoords);
                    break;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewToolDown(widgetIndex, screenCoords);
                    break;
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewToolAbort(widgetIndex);
                    break;
            }
        }

        void onViewportRotate() override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewViewportRotate();
                    break;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            switch (page)
            {
                case WINDOW_STAFF_OVERVIEW:
                    OverviewTextInput(widgetIndex, text);
                    break;
            }
        }

    private:
#pragma region Common events
        void CommonMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_TAB_1:
                case WIDX_TAB_2:
                case WIDX_TAB_3:
                    setPage(widgetIndex - WIDX_TAB_1);
                    break;
            }
        }

        void CommonPrepareDrawBefore()
        {
            ColourSchemeUpdateByClass(this, static_cast<WindowClass>(WindowClass::staff));

            SetPressedTab();
            DisableWidgets();

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            auto ft = Formatter::Common();
            staff->FormatNameTo(ft);
        }

        void CommonPrepareDrawAfter()
        {
            WindowAlignTabs(this, WIDX_TAB_1, WIDX_TAB_3);
        }
#pragma endregion

#pragma region Overview tab events
        void OverviewMouseUp(WidgetIndex widgetIndex)
        {
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            auto& gameState = getGameState();
            switch (widgetIndex)
            {
                case WIDX_PICKUP:
                {
                    _pickedPeepOldX = staff->x;
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
                                ToolSet(*wind, WC_STAFF__WIDX_PICKUP, Tool::picker);
                            }
                        });
                    GameActions::Execute(&pickupAction, gameState);
                }
                break;
                case WIDX_FIRE:
                {
                    auto intent = Intent(WindowClass::firePrompt);
                    intent.PutExtra(INTENT_EXTRA_PEEP, staff);
                    ContextOpenIntent(&intent);
                    break;
                }
                case WIDX_RENAME:
                {
                    auto peepName = staff->GetName();
                    WindowTextInputRawOpen(
                        this, widgetIndex, STR_STAFF_TITLE_STAFF_MEMBER_NAME, STR_STAFF_PROMPT_ENTER_NAME, {}, peepName.c_str(),
                        32);
                    break;
                }
            }
        }

        void OverviewOnMouseDown(WidgetIndex widgetIndex)
        {
            Widget* widget = &widgets[widgetIndex];

            switch (widgetIndex)
            {
                case WIDX_LOCATE:
                    ShowLocateDropdown(widget);
                    break;
                case WIDX_PATROL:
                {
                    // Dropdown names
                    gDropdown.items[0] = Dropdown::PlainMenuLabel(STR_SET_PATROL_AREA);
                    gDropdown.items[1] = Dropdown::PlainMenuLabel(STR_CLEAR_PATROL_AREA);

                    auto ddPos = ScreenCoordsXY{ widget->left + windowPos.x, widget->top + windowPos.y };
                    int32_t extraHeight = widget->height();
                    WindowDropdownShowText(ddPos, extraHeight, colours[1], 0, 2);
                    gDropdown.defaultIndex = 0;

                    auto staff = GetStaff();
                    if (staff == nullptr)
                    {
                        return;
                    }

                    // Disable clear patrol area if no area is set.
                    if (!staff->HasPatrolArea())
                    {
                        gDropdown.items[1].setDisabled(true);
                    }
                }
            }
        }

        void OverviewOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            auto& gameState = getGameState();
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
                        FollowPeep();
                    }
                    break;
                }
                case WIDX_PATROL:
                {
                    // Clear patrol
                    if (dropdownIndex == 1)
                    {
                        auto staff = GetStaff();
                        if (staff == nullptr)
                        {
                            return;
                        }

                        auto* windowMgr = GetWindowManager();
                        windowMgr->CloseByClass(WindowClass::patrolArea);

                        auto staffSetPatrolAreaAction = GameActions::StaffSetPatrolAreaAction(
                            staff->Id, {}, GameActions::StaffSetPatrolAreaMode::ClearAll);
                        GameActions::Execute(&staffSetPatrolAreaAction, gameState);
                    }
                    else
                    {
                        auto staffId = EntityId::FromUnderlying(number);
                        if (WindowPatrolAreaGetCurrentStaffId() == staffId)
                        {
                            auto* windowMgr = GetWindowManager();
                            windowMgr->CloseByClass(WindowClass::patrolArea);
                        }
                        else
                        {
                            PatrolAreaOpen(staffId);
                        }
                    }
                    break;
                }
            }
        }

        void OverviewPrepareDraw()
        {
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            widgets[WIDX_VIEWPORT].right = width - 26;
            widgets[WIDX_VIEWPORT].bottom = height - 14;

            widgets[WIDX_BTM_LABEL].right = width - 26;
            widgets[WIDX_BTM_LABEL].top = height - 13;
            widgets[WIDX_BTM_LABEL].bottom = height - 3;

            widgets[WIDX_PICKUP].left = width - 25;
            widgets[WIDX_PICKUP].right = width - 2;

            setWidgetPressed(WIDX_PATROL, WindowPatrolAreaGetCurrentStaffId() == staff->Id);

            widgets[WIDX_PATROL].left = width - 25;
            widgets[WIDX_PATROL].right = width - 2;

            widgets[WIDX_RENAME].left = width - 25;
            widgets[WIDX_RENAME].right = width - 2;

            widgets[WIDX_LOCATE].left = width - 25;
            widgets[WIDX_LOCATE].right = width - 2;

            widgets[WIDX_FIRE].left = width - 25;
            widgets[WIDX_FIRE].right = width - 2;
        }

        void OverviewDraw(Drawing::RenderTarget& rt)
        {
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
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }
            auto ft = Formatter();
            staff->FormatActionTo(ft);
            const auto& widget = widgets[WIDX_BTM_LABEL];
            auto screenPos = windowPos + ScreenCoordsXY{ widget.midX(), widget.top };
            int32_t widgetWidth = widget.width() - 1;
            DrawTextEllipsised(rt, screenPos, widgetWidth, STR_BLACK_STRING, ft, { TextAlignment::centre });
        }

        void DrawOverviewTabImage(Drawing::RenderTarget& rt)
        {
            if (isWidgetDisabled(WIDX_TAB_1))
                return;

            const auto& widget = widgets[WIDX_TAB_1];
            int32_t widgetWidth = widget.width() - 2;
            int32_t widgetHeight = widget.height() - 2;
            auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
            if (page == WINDOW_STAFF_OVERVIEW)
                widgetHeight++;

            Drawing::RenderTarget clippedRT;
            if (!ClipRenderTarget(clippedRT, rt, screenCoords, widgetWidth, widgetHeight))
            {
                return;
            }

            screenCoords = ScreenCoordsXY{ 14, 20 };

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            if (staff->isEntertainer())
                screenCoords.y++;

            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(staff->AnimationObjectIndex);

            const auto& anim = animObj->GetPeepAnimation(staff->AnimationGroup);
            int32_t animFrame = 0;
            if (page == WINDOW_STAFF_OVERVIEW)
                animFrame = _tabAnimationOffset / 4;

            auto imageIndex = anim.baseImage + 1 + anim.frameOffsets[animFrame] * 4;
            GfxDrawSprite(clippedRT, ImageId(imageIndex, staff->TshirtColour, staff->TrousersColour), screenCoords);
        }

        void OverviewResize()
        {
            WindowSetResize(*this, kWindowSize, { 500, 450 });

            if (viewport != nullptr)
            {
                const Widget& viewportWidget = widgets[WIDX_VIEWPORT];
                const auto reqViewportWidth = viewportWidget.width() - 2;
                const auto reqViewportHeight = viewportWidget.height() - 2;

                viewport->pos = windowPos + ScreenCoordsXY{ viewportWidget.left + 1, viewportWidget.top + 1 };
                if (viewport->width != reqViewportWidth || viewport->height != reqViewportHeight)
                {
                    viewport->width = reqViewportWidth;
                    viewport->height = reqViewportHeight;
                }
            }

            ViewportInit();
        }

        void OverviewUpdate()
        {
            auto* staff = GetStaff();
            if (staff == nullptr)
                return;
            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(staff->AnimationObjectIndex);

            // Get walking animation length
            const auto& walkingAnim = animObj->GetPeepAnimation(staff->AnimationGroup, PeepAnimationType::walking);
            const auto walkingAnimLength = walkingAnim.frameOffsets.size();

            // Overview tab animation offset
            _tabAnimationOffset++;
            _tabAnimationOffset %= walkingAnimLength * 4;

            // Get pickup animation length
            const auto& pickAnim = animObj->GetPeepAnimation(staff->AnimationGroup, PeepAnimationType::hanging);
            const auto pickAnimLength = pickAnim.frameOffsets.size();

            // Update pickup animation frame
            pickedPeepFrame++;
            pickedPeepFrame %= pickAnimLength * 4;

            invalidateWidget(WIDX_TAB_1);

            const std::optional<Focus> tempFocus = staff->State != PeepState::picked ? std::optional(Focus(staff->Id))
                                                                                     : std::nullopt;
            if (focus != tempFocus)
            {
                ViewportInit();
            }
        }

        void OverviewToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
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

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(staff->AnimationObjectIndex);

            auto& pickupAnim = animObj->GetPeepAnimation(staff->AnimationGroup, PeepAnimationType::hanging);
            auto baseImageId = pickupAnim.baseImage + pickupAnim.frameOffsets[pickedPeepFrame >> 2];
            gPickupPeepImage = ImageId(baseImageId, staff->TshirtColour, staff->TrousersColour);
        }

        void OverviewToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            const auto staffEntityId = EntityId::FromUnderlying(number);
            TileElement* tileElement;
            auto destCoords = FootpathGetCoordinatesFromPos({ screenCoords.x, screenCoords.y + 16 }, nullptr, &tileElement);

            if (destCoords.IsNull())
                return;

            GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Place,
                                                        staffEntityId,
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

        void OverviewToolAbort(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_PICKUP)
                return;

            GameActions::PeepPickupAction pickupAction{ GameActions::PeepPickupType::Cancel,
                                                        EntityId::FromUnderlying(number),
                                                        { _pickedPeepOldX, 0, 0 },
                                                        Network::GetCurrentPlayerId() };
            GameActions::Execute(&pickupAction, getGameState());
        }

        void OverviewViewportRotate()
        {
            ViewportInit();
        }

        void OverviewTextInput(WidgetIndex widgetIndex, std::string_view text)
        {
            if (widgetIndex != WIDX_RENAME)
                return;

            if (text.empty())
                return;

            auto gameAction = GameActions::StaffSetNameAction(EntityId::FromUnderlying(number), std::string{ text });
            GameActions::Execute(&gameAction, getGameState());
        }
#pragma endregion

#pragma region Options tab events
        void OptionsMouseUp(WidgetIndex widgetIndex)
        {
            switch (widgetIndex)
            {
                case WIDX_CHECKBOX_1:
                case WIDX_CHECKBOX_2:
                case WIDX_CHECKBOX_3:
                case WIDX_CHECKBOX_4:
                    SetOrder(widgetIndex - WIDX_CHECKBOX_1);
                    break;
            }
        }

        void OptionsOnMouseDown(WidgetIndex widgetIndex)
        {
            if (widgetIndex != WIDX_COSTUME_BTN)
            {
                return;
            }

            auto* ddWidget = &widgets[WIDX_COSTUME_BOX];

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            int32_t checkedIndex = -1;
            auto numCostumes = _availableCostumes.size();
            for (auto i = 0u; i < numCostumes; i++)
            {
                gDropdown.items[i] = Dropdown::MenuLabel(_availableCostumes[i].friendlyName.c_str());

                // Remember what item to check for the end of this event function
                auto costumeIndex = _availableCostumes[i].index;
                if (staff->AnimationObjectIndex == costumeIndex)
                    checkedIndex = i;
            }

            auto ddPos = ScreenCoordsXY{ ddWidget->left + windowPos.x, ddWidget->top + windowPos.y };
            int32_t ddHeight = ddWidget->height();
            int32_t ddWidth = ddWidget->width() - 4;
            WindowDropdownShowTextCustomWidth(ddPos, ddHeight, colours[1], 0, Dropdown::Flag::StayOpen, numCostumes, ddWidth);

            // Set selection
            if (checkedIndex != -1)
                gDropdown.items[checkedIndex].setChecked(true);
        }

        void OptionsOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
        {
            if (widgetIndex != WIDX_COSTUME_BTN)
            {
                return;
            }

            if (dropdownIndex == -1)
                return;

            ObjectEntryIndex costume = _availableCostumes[dropdownIndex].index;
            auto staffSetCostumeAction = GameActions::StaffSetCostumeAction(EntityId::FromUnderlying(number), costume);
            GameActions::Execute(&staffSetCostumeAction, getGameState());
        }

        void OptionsPrepareDraw()
        {
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            switch (staff->AssignedStaffType)
            {
                case StaffType::entertainer:
                {
                    widgets[WIDX_CHECKBOX_1].type = WidgetType::empty;
                    widgets[WIDX_CHECKBOX_2].type = WidgetType::empty;
                    widgets[WIDX_CHECKBOX_3].type = WidgetType::empty;
                    widgets[WIDX_CHECKBOX_4].type = WidgetType::empty;
                    widgets[WIDX_COSTUME_BOX].type = WidgetType::dropdownMenu;
                    widgets[WIDX_COSTUME_BTN].type = WidgetType::button;

                    auto pos = std::find_if(_availableCostumes.begin(), _availableCostumes.end(), [staff](auto costume) {
                        return costume.index == staff->AnimationObjectIndex;
                    });

                    if (pos != _availableCostumes.end())
                    {
                        auto index = std::distance(_availableCostumes.begin(), pos);
                        auto name = _availableCostumes[index].friendlyName.c_str();
                        widgets[WIDX_COSTUME_BOX].string = const_cast<utf8*>(name);
                        widgets[WIDX_COSTUME_BOX].flags.set(WidgetFlag::textIsString);
                    }
                    else
                    {
                        widgets[WIDX_COSTUME_BOX].text = kStringIdEmpty;
                        widgets[WIDX_COSTUME_BOX].flags.unset(WidgetFlag::textIsString);
                    }

                    break;
                }
                case StaffType::handyman:
                    widgets[WIDX_CHECKBOX_1].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_1].text = STR_STAFF_OPTION_SWEEP_FOOTPATHS;
                    widgets[WIDX_CHECKBOX_2].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_2].text = STR_STAFF_OPTION_WATER_GARDENS;
                    widgets[WIDX_CHECKBOX_3].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_3].text = STR_STAFF_OPTION_EMPTY_LITTER;
                    widgets[WIDX_CHECKBOX_4].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_4].text = STR_STAFF_OPTION_MOW_GRASS;
                    widgets[WIDX_COSTUME_BOX].type = WidgetType::empty;
                    widgets[WIDX_COSTUME_BTN].type = WidgetType::empty;
                    OptionsSetCheckboxValues();
                    break;
                case StaffType::mechanic:
                    widgets[WIDX_CHECKBOX_1].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_1].text = STR_INSPECT_RIDES;
                    widgets[WIDX_CHECKBOX_2].type = WidgetType::checkbox;
                    widgets[WIDX_CHECKBOX_2].text = STR_FIX_RIDES;
                    widgets[WIDX_CHECKBOX_3].type = WidgetType::empty;
                    widgets[WIDX_CHECKBOX_4].type = WidgetType::empty;
                    widgets[WIDX_COSTUME_BOX].type = WidgetType::empty;
                    widgets[WIDX_COSTUME_BTN].type = WidgetType::empty;
                    widgets[WIDX_COSTUME_BTN].type = WidgetType::empty;
                    OptionsSetCheckboxValues();
                    break;
                case StaffType::security:
                    // Security guards don't have an options screen.
                    break;
                case StaffType::count:
                    break;
            }
        }

        void OptionsSetCheckboxValues()
        {
            setCheckboxValue(WIDX_CHECKBOX_1, false);
            setCheckboxValue(WIDX_CHECKBOX_2, false);
            setCheckboxValue(WIDX_CHECKBOX_3, false);
            setCheckboxValue(WIDX_CHECKBOX_4, false);

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            uint32_t staffOrders = staff->StaffOrders;
            for (auto index = Numerics::bitScanForward(staffOrders); index != -1; index = Numerics::bitScanForward(staffOrders))
            {
                staffOrders &= ~(1 << index);
                setCheckboxValue(WIDX_CHECKBOX_1 + index, true);
            }
        }

        void OptionsResize()
        {
            WindowSetResize(*this, { 190, 126 }, { 190, 126 });
        }

        void OptionsUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_2);
        }
#pragma endregion

#pragma region Statistics tab events
        void StatsDraw(Drawing::RenderTarget& rt)
        {
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            auto screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_RESIZE].left + 4, widgets[WIDX_RESIZE].top + 4 };

            if (!(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(GetStaffWage(staff->AssignedStaffType));
                DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_WAGES, ft);
                screenCoords.y += kListRowHeight;
            }

            auto ft = Formatter();
            ft.Add<int32_t>(staff->GetHireDate());
            DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_EMPLOYED_FOR, ft);
            screenCoords.y += kListRowHeight;

            switch (staff->AssignedStaffType)
            {
                case StaffType::handyman:
                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffLawnsMown);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_LAWNS_MOWN, ft);
                    screenCoords.y += kListRowHeight;

                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffGardensWatered);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_GARDENS_WATERED, ft);
                    screenCoords.y += kListRowHeight;

                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffLitterSwept);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_LITTER_SWEPT, ft);
                    screenCoords.y += kListRowHeight;

                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffBinsEmptied);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_BINS_EMPTIED, ft);
                    break;
                case StaffType::mechanic:
                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffRidesInspected);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_RIDES_INSPECTED, ft);
                    screenCoords.y += kListRowHeight;

                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffRidesFixed);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_RIDES_FIXED, ft);
                    break;
                case StaffType::security:
                    ft = Formatter();
                    ft.Add<uint32_t>(staff->StaffVandalsStopped);
                    DrawTextBasic(rt, screenCoords, STR_STAFF_STAT_VANDALS_STOPPED, ft);
                    break;
                case StaffType::entertainer:
                case StaffType::count:
                    break;
            }
        }

        void StatsResize()
        {
            WindowSetResize(*this, { 190, 126 }, { 190, 126 });
        }

        void StatsUpdate()
        {
            currentFrame++;
            invalidateWidget(WIDX_TAB_3);

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            if (staff->WindowInvalidateFlags & PEEP_INVALIDATE_STAFF_STATS)
            {
                staff->WindowInvalidateFlags &= ~PEEP_INVALIDATE_STAFF_STATS;
                invalidate();
            }
        }
#pragma endregion
        void DisableWidgets()
        {
            const auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            for (WidgetIndex widgetIndex = WIDX_TAB_1; widgetIndex < widgets.size(); widgetIndex++)
            {
                setWidgetDisabled(widgetIndex, false);
            }

            if (staff->AssignedStaffType == StaffType::security)
            {
                setWidgetDisabled(WIDX_TAB_2, true);
            }

            if (page == WINDOW_STAFF_OVERVIEW)
            {
                if (staff->CanBePickedUp())
                {
                    setWidgetDisabled(WIDX_PICKUP, false);
                }
                else
                {
                    setWidgetDisabled(WIDX_PICKUP, true);
                }

                setWidgetDisabled(WIDX_FIRE, staff->State == PeepState::fixing || staff->State == PeepState::inspecting);
            }
        }

        void CancelTools()
        {
            if (isToolActive(classification, number))
                ToolCancel();
        }

        void setPage(int32_t newPage)
        {
            CancelTools();

            bool listen = false;
            if (page == WINDOW_STAFF_OVERVIEW && newPage == WINDOW_STAFF_OVERVIEW && viewport != nullptr)
            {
                viewport->flags ^= VIEWPORT_FLAG_SOUND_ON;
                listen = (viewport->flags & VIEWPORT_FLAG_SOUND_ON) != 0;
            }

            // Skip setting page if we're already on this page, unless we're initialising the window
            if (newPage == page && !widgets.empty())
                return;

            page = newPage;
            currentFrame = 0;
            pressedWidgets = 0;
            holdDownWidgets = 0;
            setWidgets(window_staff_page_widgets[page]);

            removeViewport();

            invalidate();
            onResize();
            onPrepareDraw();
            initScrollWidgets();
            ViewportInit();
            invalidate();

            if (listen && viewport != nullptr)
                viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
        }

        void SetPressedTab()
        {
            for (int32_t i = 0; i < WINDOW_STAFF_PAGE_COUNT; i++)
                setWidgetPressed((WIDX_TAB_1 + i), false);
            setWidgetPressed(WIDX_TAB_1 + page, true);
        }

        void SetOrder(int32_t orderId)
        {
            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            uint8_t newOrders = staff->StaffOrders ^ (1 << orderId);
            auto staffSetOrdersAction = GameActions::StaffSetOrdersAction(EntityId::FromUnderlying(number), newOrders);
            GameActions::Execute(&staffSetOrdersAction, getGameState());
        }

        void ViewportInit()
        {
            if (page != WINDOW_STAFF_OVERVIEW)
                return;

            auto staff = GetStaff();
            if (staff == nullptr)
            {
                return;
            }

            std::optional<Focus> tempFocus;
            if (staff->State != PeepState::picked)
            {
                tempFocus = Focus(staff->Id);
            }

            uint16_t viewport_flags;

            if (viewport != nullptr)
            {
                if (tempFocus == focus)
                    return;

                viewport_flags = viewport->flags;
                removeViewport();
            }
            else
            {
                viewport_flags = 0;
                if (Config::Get().general.alwaysShowGridlines)
                    viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
            }

            onPrepareDraw();

            focus = tempFocus;

            if (staff->State != PeepState::picked)
            {
                if (viewport == nullptr)
                {
                    const auto& viewWidget = widgets[WIDX_VIEWPORT];

                    auto screenPos = ScreenCoordsXY{ viewWidget.left + 1 + windowPos.x, viewWidget.top + 1 + windowPos.y };
                    int32_t viewportWidth = viewWidget.width() - 2;
                    int32_t viewportHeight = viewWidget.height() - 2;

                    ViewportCreate(*this, screenPos, viewportWidth, viewportHeight, focus.value());
                    flags |= WindowFlag::noScrolling;
                    invalidate();
                }
            }

            if (viewport != nullptr)
                viewport->flags = viewport_flags;
            invalidate();
        }

        void ShowLocateDropdown(Widget* widget)
        {
            gDropdown.items[0] = Dropdown::PlainMenuLabel(STR_LOCATE_SUBJECT_TIP);
            gDropdown.items[1] = Dropdown::PlainMenuLabel(STR_FOLLOW_SUBJECT_TIP);

            WindowDropdownShowText(
                { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height(), colours[1], 0, 2);
            gDropdown.defaultIndex = 0;
        }

        void FollowPeep()
        {
            WindowBase* main = WindowGetMain();
            WindowFollowSprite(*main, EntityId::FromUnderlying(number));
        }

        void DrawTabImages(Drawing::RenderTarget& rt)
        {
            DrawOverviewTabImage(rt);
            DrawTabImage(rt, WINDOW_STAFF_OPTIONS, SPR_TAB_STAFF_OPTIONS_0);
            DrawTabImage(rt, WINDOW_STAFF_STATISTICS, SPR_TAB_STATS_0);
        }

        void DrawTabImage(Drawing::RenderTarget& rt, int32_t p, int32_t baseImageId)
        {
            WidgetIndex widgetIndex = WIDX_TAB_1 + p;
            Widget* widget = &widgets[widgetIndex];

            auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };

            if (!isWidgetDisabled(widgetIndex))
            {
                if (page == p)
                {
                    int32_t frame = currentFrame / TabAnimationDivisor[page - 1];
                    baseImageId += (frame % TabAnimationFrames);
                }

                // Draw normal, enabled sprite.
                GfxDrawSprite(rt, ImageId(baseImageId), screenCoords);
            }
        }

        Staff* GetStaff()
        {
            return getGameState().entities.GetEntity<Staff>(EntityId::FromUnderlying(number));
        }

        static constexpr int32_t TabAnimationDivisor[] = {
            2, // WINDOW_STAFF_OPTIONS,
            4, // WINDOW_STAFF_STATISTICS,
        };

        static constexpr int32_t TabAnimationFrames = 7;
    };

    WindowBase* StaffOpen(Peep* peep)
    {
        auto* windowMgr = GetWindowManager();

        auto w = static_cast<StaffWindow*>(windowMgr->BringToFrontByNumber(WindowClass::peep, peep->Id.ToUnderlying()));
        if (w != nullptr)
            return w;

        w = windowMgr->Create<StaffWindow>(
            WindowClass::peep, kWindowSize, { WindowFlag::higherContrastOnPress, WindowFlag::resizable });
        if (w == nullptr)
            return nullptr;

        if (w != nullptr)
            w->initialise(peep->Id);

        return w;
    }
} // namespace OpenRCT2::Ui::Windows
