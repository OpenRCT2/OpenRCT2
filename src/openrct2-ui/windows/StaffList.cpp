/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <limits>
#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportQuery.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/actions/StaffFireAction.h>
#include <openrct2/actions/StaffHireNewAction.h>
#include <openrct2/actions/StaffSetColourAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/EnumUtils.hpp>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/PatrolArea.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Finance.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/PeepAnimationsObject.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Util.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WINDOW_STAFF_LIST_TAB_HANDYMEN,
        WINDOW_STAFF_LIST_TAB_MECHANICS,
        WINDOW_STAFF_LIST_TAB_SECURITY,
        WINDOW_STAFF_LIST_TAB_ENTERTAINERS
    };

    enum WindowStaffListWidgetIdx
    {
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

    static constexpr StringId WINDOW_TITLE = STR_STAFF;
    static constexpr int32_t WW = 320;
    static constexpr int32_t WH = 270;
    constexpr int32_t MAX_WW = 500;
    constexpr int32_t MAX_WH = 450;

    // clang-format off
    static constexpr Widget _staffListWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({  0, 43}, {    WW, WH - 43}, WindowWidgetType::Resize,    WindowColour::Secondary                                                     ), // tab content panel
        MakeTab   ({  3, 17},                                                                          STR_STAFF_HANDYMEN_TAB_TIP                         ), // handymen tab
        MakeTab   ({ 34, 17},                                                                          STR_STAFF_MECHANICS_TAB_TIP                        ), // mechanics tab
        MakeTab   ({ 65, 17},                                                                          STR_STAFF_SECURITY_TAB_TIP                         ), // security guards tab
        MakeTab   ({ 96, 17},                                                                          STR_STAFF_ENTERTAINERS_TAB_TIP                     ), // entertainers tab
        MakeWidget({  3, 72}, {WW - 6,     195}, WindowWidgetType::Scroll,    WindowColour::Secondary, SCROLL_VERTICAL                                    ), // staff list
        MakeWidget({130, 58}, {    12,      12}, WindowWidgetType::ColourBtn, WindowColour::Secondary, kStringIdNone,        STR_UNIFORM_COLOUR_TIP            ), // uniform colour picker
        MakeWidget({165, 17}, {   145,      13}, WindowWidgetType::Button,    WindowColour::Primary  , kStringIdNone,        STR_HIRE_STAFF_TIP                ), // hire button
        MakeWidget({243, 46}, {    24,      24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_DEMOLISH),    STR_QUICK_FIRE_STAFF     ), // quick fire staff
        MakeWidget({267, 46}, {    24,      24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_PATROL_BTN),  STR_SHOW_PATROL_AREA_TIP ), // show staff patrol area tool
        MakeWidget({291, 46}, {    24,      24}, WindowWidgetType::FlatBtn,   WindowColour::Secondary, ImageId(SPR_MAP),         STR_SHOW_STAFF_ON_MAP_TIP), // show staff on map button
    };
    // clang-format on

    class StaffListWindow final : public Window
    {
    private:
        struct StaffNamingConvention
        {
            StringId Plural;
            StringId Singular;
            StringId ActionHire;
        };

        struct StaffEntry
        {
            EntityId Id;
            u8string Name;
        };

        std::vector<StaffEntry> _staffList;
        bool _quickFireMode{};
        std::optional<size_t> _highlightedIndex{};
        int32_t _selectedTab{};
        uint32_t _tabAnimationIndex{};

    public:
        void OnOpen() override
        {
            SetWidgets(_staffListWidgets);
            WindowInitScrollWidgets(*this);

            widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WindowWidgetType::Empty;
            min_width = WW;
            min_height = WH;
            max_width = MAX_WW;
            max_height = MAX_WH;

            RefreshList();
        }

        void OnClose() override
        {
            CancelTools();
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_STAFF_LIST_CLOSE:
                    Close();
                    break;
                case WIDX_STAFF_LIST_HIRE_BUTTON:
                {
                    auto staffType = GetSelectedStaffType();
                    HireNewMember(staffType);
                    break;
                }
                case WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON:
                    if (!ToolSet(*this, WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON, Tool::Crosshair))
                    {
                        ShowGridlines();
                        SetPatrolAreaToRender(GetSelectedStaffType());
                        GfxInvalidateScreen();
                    }
                    break;
                case WIDX_STAFF_LIST_MAP:
                    ContextOpenWindow(WindowClass::Map);
                    break;
                case WIDX_STAFF_LIST_QUICK_FIRE:
                    _quickFireMode = !_quickFireMode;
                    Invalidate();
                    break;
            }
        }

        void OnResize() override
        {
            min_width = WW;
            min_height = WH;
            if (width < min_width)
            {
                width = min_width;
                Invalidate();
            }
            if (height < min_height)
            {
                height = min_height;
                Invalidate();
            }
            ResizeFrameWithPage();
        }

        void OnUpdate() override
        {
            auto animPeepType = AnimationPeepType(static_cast<uint8_t>(_selectedTab) + 1);
            auto* animObj = findPeepAnimationsObjectForType(animPeepType);
            if (animObj != nullptr)
            {
                auto& anim = animObj->GetPeepAnimation(PeepAnimationGroup::Normal);
                _tabAnimationIndex++;

                if (_tabAnimationIndex >= anim.frame_offsets.size() * 4)
                    _tabAnimationIndex = 0;

                InvalidateWidget(WIDX_STAFF_LIST_HANDYMEN_TAB + _selectedTab);
            }

            // Enable highlighting of these staff members in map window
            auto* windowMgr = GetWindowManager();
            if (windowMgr->FindByClass(WindowClass::Map) != nullptr)
            {
                for (auto peep : EntityList<Staff>())
                {
                    EntitySetFlashing(peep, false);
                    if (peep->AssignedStaffType == GetSelectedStaffType())
                    {
                        EntitySetFlashing(peep, true);
                    }
                }
            }

            // Note this may be slow if number of staff increases a large amount.
            // See GuestList for fix (more intents) if required.
            RefreshList();
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_STAFF_LIST_HANDYMEN_TAB:
                case WIDX_STAFF_LIST_MECHANICS_TAB:
                case WIDX_STAFF_LIST_SECURITY_TAB:
                case WIDX_STAFF_LIST_ENTERTAINERS_TAB:
                {
                    auto newSelectedTab = widgetIndex - WIDX_STAFF_LIST_HANDYMEN_TAB;
                    if (_selectedTab != newSelectedTab)
                    {
                        _selectedTab = static_cast<uint8_t>(newSelectedTab);
                        RefreshList();
                        Invalidate();
                        scrolls[0].contentOffsetY = 0;
                        CancelTools();
                    }
                    break;
                }
                case WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER:
                    WindowDropdownShowColour(this, &widgets[widgetIndex], colours[1], StaffGetColour(GetSelectedStaffType()));
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
        {
            if (dropdownIndex == -1)
            {
                return;
            }
            if (widgetIndex == WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER)
            {
                auto action = StaffSetColourAction(GetSelectedStaffType(), ColourDropDownIndexToColour(dropdownIndex));
                GameActions::Execute(&action);
            }
        }

        void OnPrepareDraw() override
        {
            // Set selected tab
            SetWidgetPressed(WIDX_STAFF_LIST_HANDYMEN_TAB, false);
            SetWidgetPressed(WIDX_STAFF_LIST_MECHANICS_TAB, false);
            SetWidgetPressed(WIDX_STAFF_LIST_SECURITY_TAB, false);
            SetWidgetPressed(WIDX_STAFF_LIST_ENTERTAINERS_TAB, false);
            SetWidgetPressed(_selectedTab + WIDX_STAFF_LIST_HANDYMEN_TAB, true);

            widgets[WIDX_STAFF_LIST_HIRE_BUTTON].text = GetStaffNamingConvention(GetSelectedStaffType()).ActionHire;
            widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WindowWidgetType::Empty;

            if (GetSelectedStaffType() != StaffType::Entertainer)
            {
                widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].type = WindowWidgetType::ColourBtn;
                widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].image = GetColourButtonImage(
                    StaffGetColour(GetSelectedStaffType()));
            }
            SetWidgetPressed(WIDX_STAFF_LIST_QUICK_FIRE, _quickFireMode);

            ResizeFrameWithPage();
            widgets[WIDX_STAFF_LIST_LIST].right = width - 4;
            widgets[WIDX_STAFF_LIST_LIST].bottom = height - 15;
            widgets[WIDX_STAFF_LIST_QUICK_FIRE].left = width - 77;
            widgets[WIDX_STAFF_LIST_QUICK_FIRE].right = width - 54;
            widgets[WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON].left = width - 53;
            widgets[WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON].right = width - 30;
            widgets[WIDX_STAFF_LIST_MAP].left = width - 29;
            widgets[WIDX_STAFF_LIST_MAP].right = width - 6;
            widgets[WIDX_STAFF_LIST_HIRE_BUTTON].left = width - 155;
            widgets[WIDX_STAFF_LIST_HIRE_BUTTON].right = width - 11;
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
            DrawTabImages(dpi);

            if (!(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                auto ft = Formatter();
                ft.Add<money64>(GetStaffWage(GetSelectedStaffType()));
                DrawTextBasic(dpi, windowPos + ScreenCoordsXY{ width - 155, 32 }, STR_COST_PER_MONTH, ft);
            }

            if (GetSelectedStaffType() != StaffType::Entertainer)
            {
                DrawTextBasic(
                    dpi, windowPos + ScreenCoordsXY{ 6, widgets[WIDX_STAFF_LIST_UNIFORM_COLOUR_PICKER].top + 1 },
                    STR_UNIFORM_COLOUR);
            }

            auto namingConvention = GetStaffNamingConvention(GetSelectedStaffType());
            auto staffTypeStringId = _staffList.size() == 1 ? namingConvention.Singular : namingConvention.Plural;

            auto ft = Formatter();
            ft.Add<uint32_t>(_staffList.size());
            ft.Add<StringId>(staffTypeStringId);

            DrawTextBasic(
                dpi, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_STAFF_LIST_LIST].bottom + 2 }, STR_STAFF_LIST_COUNTER, ft);
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            if (_highlightedIndex)
            {
                _highlightedIndex = {};
                Invalidate();
            }

            auto scrollHeight = static_cast<int32_t>(_staffList.size()) * kScrollableRowHeight;
            auto i = scrollHeight - widgets[WIDX_STAFF_LIST_LIST].bottom + widgets[WIDX_STAFF_LIST_LIST].top + 21;
            if (i < 0)
                i = 0;
            if (i < scrolls[0].contentOffsetY)
            {
                scrolls[0].contentOffsetY = i;
                Invalidate();
            }

            auto scrollWidth = widgets[WIDX_STAFF_LIST_LIST].width() - 15;
            return { scrollWidth, scrollHeight };
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto i = static_cast<size_t>(screenCoords.y / kScrollableRowHeight);
            if (i != _highlightedIndex)
            {
                _highlightedIndex = static_cast<size_t>(i);
                Invalidate();
            }
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            int32_t i = screenCoords.y / kScrollableRowHeight;
            for (const auto& entry : _staffList)
            {
                if (i == 0)
                {
                    if (_quickFireMode)
                    {
                        auto staffFireAction = StaffFireAction(entry.Id);
                        GameActions::Execute(&staffFireAction);
                    }
                    else
                    {
                        auto peep = GetEntity<Staff>(entry.Id);
                        if (peep != nullptr)
                        {
                            auto intent = Intent(WindowClass::Peep);
                            intent.PutExtra(INTENT_EXTRA_PEEP, peep);
                            ContextOpenIntent(&intent);
                        }
                    }
                    break;
                }

                i--;
            }
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            auto dpiCoords = ScreenCoordsXY{ dpi.x, dpi.y };
            GfxFillRect(
                dpi, { dpiCoords, dpiCoords + ScreenCoordsXY{ dpi.width - 1, dpi.height - 1 } },
                ColourMapA[colours[1].colour].mid_light);

            // How much space do we have for the name and action columns? (Discount scroll area and icons.)
            const int32_t nonIconSpace = widgets[WIDX_STAFF_LIST_LIST].width() - 15 - 68;
            const int32_t nameColumnSize = nonIconSpace * 0.42;
            const int32_t actionColumnSize = nonIconSpace * 0.58;
            const int32_t actionOffset = widgets[WIDX_STAFF_LIST_LIST].right - actionColumnSize - 15;

            auto y = 0;
            size_t i = 0;
            for (const auto& entry : _staffList)
            {
                if (y > dpi.y + dpi.height)
                {
                    break;
                }

                if (y + 11 >= dpi.y)
                {
                    const auto* peep = GetEntity<Staff>(entry.Id);
                    if (peep == nullptr)
                    {
                        continue;
                    }

                    StringId format = STR_BLACK_STRING;
                    if (_quickFireMode)
                        format = STR_RED_STRINGID;

                    if (i == _highlightedIndex)
                    {
                        GfxFilterRect(dpi, { 0, y, 800, y + (kScrollableRowHeight - 1) }, FilterPaletteID::PaletteDarken1);

                        format = STR_WINDOW_COLOUR_2_STRINGID;
                        if (_quickFireMode)
                            format = STR_LIGHTPINK_STRINGID;
                    }

                    auto ft = Formatter();
                    peep->FormatNameTo(ft);
                    DrawTextEllipsised(dpi, { 0, y }, nameColumnSize, format, ft);

                    ft = Formatter();
                    peep->FormatActionTo(ft);
                    DrawTextEllipsised(dpi, { actionOffset, y }, actionColumnSize, format, ft);

                    // True if a patrol path is set for the worker
                    if (peep->HasPatrolArea())
                    {
                        GfxDrawSprite(dpi, ImageId(SPR_STAFF_PATROL_PATH), { nameColumnSize + 5, y });
                    }

                    auto staffOrderIcon_x = nameColumnSize + 20;
                    if (peep->AssignedStaffType != StaffType::Entertainer)
                    {
                        auto staffOrders = peep->StaffOrders;
                        auto staffOrderSprite = GetStaffOrderBaseSprite(GetSelectedStaffType());

                        while (staffOrders != 0)
                        {
                            if (staffOrders & 1)
                            {
                                GfxDrawSprite(dpi, ImageId(staffOrderSprite), { staffOrderIcon_x, y });
                            }
                            staffOrders = staffOrders >> 1;
                            staffOrderIcon_x += 9;
                            // TODO: Remove sprite ID addition
                            staffOrderSprite++;
                        }
                    }
                    else
                    {
                        GfxDrawSprite(dpi, GetCostumeInlineSprite(peep->AnimationObjectIndex), { staffOrderIcon_x, y });
                    }
                }

                y += kScrollableRowHeight;
                i++;
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (widgetIndex == WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON)
            {
                auto closestStaffMember = GetClosestStaffMemberTo(screenCoords);
                if (closestStaffMember != nullptr)
                {
                    ToolCancel();
                    auto* staffWindow = StaffOpen(closestStaffMember);
                    staffWindow->OnDropdown(WC_PEEP__WIDX_PATROL, 0);
                }
                else
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(GetStaffNamingConvention(GetSelectedStaffType()).Plural);
                    ContextShowError(STR_NO_THING_IN_PARK_YET, kStringIdNone, ft);
                }
            }
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == WIDX_STAFF_LIST_SHOW_PATROL_AREA_BUTTON)
            {
                HideGridlines();
                ToolCancel();
                ClearPatrolAreaToRender();
                GfxInvalidateScreen();
            }
        }

        void RefreshList()
        {
            _staffList.clear();

            for (auto* peep : EntityList<Staff>())
            {
                EntitySetFlashing(peep, false);
                if (peep->AssignedStaffType == GetSelectedStaffType())
                {
                    EntitySetFlashing(peep, true);

                    StaffEntry entry;
                    entry.Id = peep->Id;
                    entry.Name = peep->GetName();

                    _staffList.push_back(std::move(entry));
                }
            }

            std::sort(_staffList.begin(), _staffList.end(), [](const auto& a, const auto& b) {
                return String::logicalCmp(a.Name.c_str(), b.Name.c_str()) < 0;
            });
        }

    private:
        /**
         * Hires a new staff member of the given type.
         */
        void HireNewMember(StaffType staffType)
        {
            bool autoPosition = Config::Get().general.AutoStaffPlacement;
            if (GetInputManager().IsModifierKeyPressed(ModifierKey::shift))
            {
                autoPosition = autoPosition ^ 1;
            }

            uint32_t staffOrders = 0;

            if (staffType == StaffType::Handyman)
            {
                staffOrders = STAFF_ORDERS_SWEEPING | STAFF_ORDERS_WATER_FLOWERS | STAFF_ORDERS_EMPTY_BINS;
                if (Config::Get().general.HandymenMowByDefault)
                {
                    staffOrders |= STAFF_ORDERS_MOWING;
                }
            }
            else if (staffType == StaffType::Mechanic)
            {
                staffOrders = STAFF_ORDERS_INSPECT_RIDES | STAFF_ORDERS_FIX_RIDES;
            }

            auto animPeepType = AnimationPeepType(static_cast<uint8_t>(staffType) + 1);
            ObjectEntryIndex costume;
            if (staffType == StaffType::Entertainer)
                costume = findRandomPeepAnimationsIndexForType(animPeepType);
            else
                costume = findPeepAnimationsIndexForType(animPeepType);

            auto hireStaffAction = StaffHireNewAction(autoPosition, staffType, costume, staffOrders);
            hireStaffAction.SetCallback([=](const GameAction*, const GameActions::Result* res) -> void {
                if (res->Error != GameActions::Status::Ok)
                    return;

                auto actionResult = res->GetData<StaffHireNewActionResult>();
                auto* staff = GetEntity<Staff>(actionResult.StaffEntityId);
                if (staff == nullptr)
                    return;

                // If autoposition of staff is disabled, pickup peep and then open the staff window
                if (staff->State == PeepState::Picked)
                {
                    CoordsXYZ nullLoc{};
                    nullLoc.SetNull();

                    PeepPickupAction pickupAction{ PeepPickupType::Pickup, staff->Id, nullLoc, NetworkGetCurrentPlayerId() };
                    pickupAction.SetCallback([staffId = staff->Id](const GameAction* ga, const GameActions::Result* result) {
                        if (result->Error != GameActions::Status::Ok)
                            return;

                        auto* staff2 = GetEntity<Staff>(staffId);
                        auto intent = Intent(WindowClass::Peep);
                        intent.PutExtra(INTENT_EXTRA_PEEP, staff2);
                        auto* wind = ContextOpenIntent(&intent);
                        if (wind != nullptr)
                        {
                            ToolSet(*wind, WC_STAFF__WIDX_PICKUP, Tool::Picker);
                        }
                    });
                    GameActions::Execute(&pickupAction);
                }
                else
                {
                    // Open window for new staff.
                    auto intent = Intent(WindowClass::Peep);
                    intent.PutExtra(INTENT_EXTRA_PEEP, staff);
                    ContextOpenIntent(&intent);
                }
            });

            GameActions::Execute(&hireStaffAction);
        }

        StaffType GetSelectedStaffType() const
        {
            return static_cast<StaffType>(_selectedTab);
        }

        void DrawTabImages(DrawPixelInfo& dpi) const
        {
            const auto& gameState = GetGameState();
            DrawTabImage(dpi, WINDOW_STAFF_LIST_TAB_HANDYMEN, AnimationPeepType::Handyman, gameState.StaffHandymanColour);
            DrawTabImage(dpi, WINDOW_STAFF_LIST_TAB_MECHANICS, AnimationPeepType::Mechanic, gameState.StaffMechanicColour);
            DrawTabImage(dpi, WINDOW_STAFF_LIST_TAB_SECURITY, AnimationPeepType::Security, gameState.StaffSecurityColour);
            DrawTabImage(dpi, WINDOW_STAFF_LIST_TAB_ENTERTAINERS, AnimationPeepType::Entertainer);
        }

        void DrawTabImage(DrawPixelInfo& dpi, int32_t tabIndex, AnimationPeepType type, colour_t colour) const
        {
            PeepAnimationsObject* animObj = findPeepAnimationsObjectForType(type);
            if (animObj == nullptr)
                return;

            auto widgetIndex = WIDX_STAFF_LIST_HANDYMEN_TAB + tabIndex;
            const auto& widget = widgets[widgetIndex];

            auto frame = _selectedTab == tabIndex ? _tabAnimationIndex / 4 : 0;
            auto& anim = animObj->GetPeepAnimation(PeepAnimationGroup::Normal);
            auto imageId = anim.base_image + 1 + anim.frame_offsets[frame] * 4;

            GfxDrawSprite(
                dpi, ImageId(imageId, colour),
                windowPos + ScreenCoordsXY{ (widget.left + widget.right) / 2, widget.bottom - 6 });
        }

        void DrawTabImage(DrawPixelInfo& dpi, int32_t tabIndex, AnimationPeepType type) const
        {
            PeepAnimationsObject* animObj = findPeepAnimationsObjectForType(type);
            if (animObj == nullptr)
                return;

            auto widgetIndex = WIDX_STAFF_LIST_HANDYMEN_TAB + tabIndex;
            const auto& widget = widgets[widgetIndex];

            DrawPixelInfo clippedDpi;
            if (ClipDrawPixelInfo(
                    clippedDpi, dpi, windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 },
                    widget.right - widget.left - 1, widget.bottom - widget.top - 1))
            {
                auto frame = _selectedTab == tabIndex ? _tabAnimationIndex / 4 : 0;
                auto& anim = animObj->GetPeepAnimation(PeepAnimationGroup::Normal);
                auto imageId = anim.base_image + 1 + anim.frame_offsets[frame] * 4;

                GfxDrawSprite(clippedDpi, ImageId(imageId), { 15, 23 });
            }
        }

        void CancelTools()
        {
            if (isToolActive(classification, number))
                ToolCancel();
        }

        Peep* GetClosestStaffMemberTo(const ScreenCoordsXY& screenCoords)
        {
            int32_t direction{};
            TileElement* tileElement{};
            auto footpathCoords = FootpathGetCoordinatesFromPos(screenCoords, &direction, &tileElement);
            if (footpathCoords.IsNull())
                return nullptr;

            auto isPatrolAreaSet = IsPatrolAreaSetForStaffType(GetSelectedStaffType(), footpathCoords);

            Peep* closestPeep = nullptr;
            auto closestPeepDistance = std::numeric_limits<int32_t>::max();
            for (auto peep : EntityList<Staff>())
            {
                if (peep->AssignedStaffType != GetSelectedStaffType())
                    continue;

                if (isPatrolAreaSet)
                {
                    if (!peep->HasPatrolArea())
                    {
                        continue;
                    }
                    if (!peep->IsLocationInPatrol(footpathCoords))
                    {
                        continue;
                    }
                }

                if (peep->x == kLocationNull)
                {
                    continue;
                }

                auto distance = std::abs(footpathCoords.x - peep->x) + std::abs(footpathCoords.y - peep->y);
                if (distance < closestPeepDistance)
                {
                    closestPeepDistance = distance;
                    closestPeep = peep;
                }
            }
            return closestPeep;
        }

        static constexpr StaffNamingConvention GetStaffNamingConvention(StaffType type)
        {
            switch (type)
            {
                default:
                case StaffType::Handyman:
                    return { STR_HANDYMAN_PLURAL, STR_HANDYMAN_SINGULAR, STR_HIRE_HANDYMAN };
                case StaffType::Mechanic:
                    return { STR_MECHANIC_PLURAL, STR_MECHANIC_SINGULAR, STR_HIRE_MECHANIC };
                case StaffType::Security:
                    return { STR_SECURITY_GUARD_PLURAL, STR_SECURITY_GUARD_SINGULAR, STR_HIRE_SECURITY_GUARD };
                case StaffType::Entertainer:
                    return { STR_ENTERTAINER_PLURAL, STR_ENTERTAINER_SINGULAR, STR_HIRE_ENTERTAINER };
            }
        }

        static uint32_t GetStaffOrderBaseSprite(StaffType type)
        {
            switch (type)
            {
                case StaffType::Handyman:
                    return SPR_STAFF_ORDERS_SWEEPING;
                case StaffType::Mechanic:
                    return SPR_STAFF_ORDERS_INSPECT_RIDES;
                default:
                    return 0;
            }
        }

        static ImageId GetCostumeInlineSprite(ObjectEntryIndex index)
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto* animObj = objManager.GetLoadedObject<PeepAnimationsObject>(index);
            return ImageId(animObj->GetInlineImageId());
        }
    };

    WindowBase* StaffListOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<StaffListWindow>(WindowClass::StaffList, WW, WH, WF_10 | WF_RESIZABLE);
    }

    void WindowStaffListRefresh()
    {
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->FindByClass(WindowClass::StaffList);
        if (window != nullptr)
        {
            static_cast<StaffListWindow*>(window)->RefreshList();
        }
    }
} // namespace OpenRCT2::Ui::Windows
