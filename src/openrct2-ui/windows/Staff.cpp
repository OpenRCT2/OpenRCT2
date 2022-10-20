/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/PeepPickupAction.h>
#include <openrct2/actions/StaffSetCostumeAction.h>
#include <openrct2/actions/StaffSetNameAction.h>
#include <openrct2/actions/StaffSetOrdersAction.h>
#include <openrct2/actions/StaffSetPatrolAreaAction.h>
#include <openrct2/config/Config.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/PatrolArea.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Finance.h>
#include <openrct2/network/network.h>
#include <openrct2/sprites.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Park.h>

static constexpr const StringId WINDOW_TITLE = STR_STRINGID;

static constexpr const int32_t WW = 190;
static constexpr const int32_t WH = 180;

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

validate_global_widx(WC_PEEP, WIDX_PATROL);
validate_global_widx(WC_STAFF, WIDX_PICKUP);

// clang-format off
#define MAIN_STAFF_WIDGETS \
    WINDOW_SHIM(WINDOW_TITLE, WW, WH), \
    MakeWidget({ 0, 43}, {190, 137}, WindowWidgetType::Resize, WindowColour::Secondary), /* Resize */ \
    MakeTab   ({ 3, 17}, STR_STAFF_OVERVIEW_TIP                         ), /* Tab 1 */ \
    MakeTab   ({34, 17}, STR_STAFF_OPTIONS_TIP                          ), /* Tab 2 */ \
    MakeTab   ({65, 17}, STR_STAFF_STATS_TIP                            )  /* Tab 3 */

static rct_widget window_staff_overview_widgets[] = {
    MAIN_STAFF_WIDGETS,
    MakeWidget     ({      3,      47}, {162, 120}, WindowWidgetType::Viewport,      WindowColour::Secondary                                        ), // Viewport
    MakeWidget     ({      3, WH - 13}, {162,  11}, WindowWidgetType::LabelCentred, WindowColour::Secondary                                        ), // Label at bottom of viewport
    MakeWidget     ({WW - 25,      45}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_PICKUP_BTN, STR_PICKUP_TIP        ), // Pickup Button
    MakeWidget     ({WW - 25,      69}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_PATROL_BTN, STR_SET_PATROL_TIP    ), // Patrol Button
    MakeWidget     ({WW - 25,      93}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_RENAME,     STR_NAME_STAFF_TIP    ), // Rename Button
    MakeWidget     ({WW - 25,     117}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_LOCATE,     STR_LOCATE_SUBJECT_TIP), // Locate Button
    MakeWidget     ({WW - 25,     141}, { 24,  24}, WindowWidgetType::FlatBtn,       WindowColour::Secondary, SPR_DEMOLISH,   STR_FIRE_STAFF_TIP    ), // Fire Button
    WIDGETS_END,
};

//0x9AF910
static rct_widget window_staff_options_widgets[] = {
    MAIN_STAFF_WIDGETS,
    MakeWidget     ({      5,  50}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 1
    MakeWidget     ({      5,  67}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 2
    MakeWidget     ({      5,  84}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 3
    MakeWidget     ({      5, 101}, {180,  12}, WindowWidgetType::Checkbox, WindowColour::Secondary                                            ), // Checkbox 4
    MakeWidget     ({      5,  50}, {180,  12}, WindowWidgetType::DropdownMenu, WindowColour::Secondary                                            ), // Costume Dropdown
    MakeWidget     ({WW - 17,  51}, { 11,  10}, WindowWidgetType::Button,   WindowColour::Secondary, STR_DROPDOWN_GLYPH, STR_SELECT_COSTUME_TIP), // Costume Dropdown Button
    WIDGETS_END,
};
// clang-format on

// 0x9AF9F4
static rct_widget window_staff_stats_widgets[] = {
    MAIN_STAFF_WIDGETS,
    WIDGETS_END,
};

static rct_widget* window_staff_page_widgets[] = {
    window_staff_overview_widgets,
    window_staff_options_widgets,
    window_staff_stats_widgets,
};

class StaffWindow final : public Window
{
private:
    EntertainerCostume _availableCostumes[static_cast<uint8_t>(EntertainerCostume::Count)]{};
    uint16_t _tabAnimationOffset = 0;

public:
    void Initialise(EntityId entityId)
    {
        number = entityId.ToUnderlying();
    }

    void OnOpen() override
    {
        SetPage(WINDOW_STAFF_OVERVIEW);
    }

    void OnClose() override
    {
        CancelTools();
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
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

    void OnMouseDown(WidgetIndex widgetIndex) override
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

    void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
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

    void OnPrepareDraw() override
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

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);
        DrawTabImages(&dpi);

        switch (page)
        {
            case WINDOW_STAFF_OVERVIEW:
                OverviewDraw(&dpi);
                break;
            case WINDOW_STAFF_STATISTICS:
                StatsDraw(&dpi);
                break;
        }
    }

    void OnResize() override
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

    void OnUpdate() override
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

    void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_STAFF_OVERVIEW:
                OverviewToolUpdate(widgetIndex, screenCoords);
                break;
        }
    }

    void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        switch (page)
        {
            case WINDOW_STAFF_OVERVIEW:
                OverviewToolDown(widgetIndex, screenCoords);
                break;
        }
    }

    void OnToolAbort(WidgetIndex widgetIndex) override
    {
        switch (page)
        {
            case WINDOW_STAFF_OVERVIEW:
                OverviewToolAbort(widgetIndex);
                break;
        }
    }

    void OnViewportRotate() override
    {
        switch (page)
        {
            case WINDOW_STAFF_OVERVIEW:
                OverviewViewportRotate();
                break;
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
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
                Close();
                break;
            case WIDX_TAB_1:
            case WIDX_TAB_2:
            case WIDX_TAB_3:
                SetPage(widgetIndex - WIDX_TAB_1);
                break;
        }
    }

    void CommonPrepareDrawBefore()
    {
        ColourSchemeUpdateByClass(this, static_cast<WindowClass>(WindowClass::Staff));

        if (window_staff_page_widgets[page] != widgets)
        {
            widgets = window_staff_page_widgets[page];
            InitScrollWidgets();
        }
        SetPressedTab();
        DisableWidgets();

        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        auto ft = Formatter::Common();
        staff->FormatNameTo(ft);

        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;

        widgets[WIDX_RESIZE].right = width - 1;
        widgets[WIDX_RESIZE].bottom = height - 1;

        widgets[WIDX_TITLE].right = width - 2;

        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
    }

    void CommonPrepareDrawAfter()
    {
        window_align_tabs(this, WIDX_TAB_1, WIDX_TAB_3);
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

        switch (widgetIndex)
        {
            case WIDX_PICKUP:
            {
                picked_peep_old_x = staff->x;
                CoordsXYZ nullLoc{};
                nullLoc.SetNull();
                PeepPickupAction pickupAction{ PeepPickupType::Pickup, EntityId::FromUnderlying(number), nullLoc,
                                               network_get_current_player_id() };
                pickupAction.SetCallback([peepnum = number](const GameAction* ga, const GameActions::Result* result) {
                    if (result->Error != GameActions::Status::Ok)
                        return;
                    rct_window* wind = window_find_by_number(WindowClass::Peep, peepnum);
                    if (wind != nullptr)
                    {
                        tool_set(*wind, WC_STAFF__WIDX_PICKUP, Tool::Picker);
                    }
                });
                GameActions::Execute(&pickupAction);
            }
            break;
            case WIDX_FIRE:
            {
                auto intent = Intent(WindowClass::FirePrompt);
                intent.putExtra(INTENT_EXTRA_PEEP, staff);
                context_open_intent(&intent);
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
        rct_widget* widget = &widgets[widgetIndex];

        switch (widgetIndex)
        {
            case WIDX_LOCATE:
                ShowLocateDropdown(widget);
                break;
            case WIDX_PATROL:
            {
                // Dropdown names
                gDropdownItems[0].Format = STR_SET_PATROL_AREA;
                gDropdownItems[1].Format = STR_CLEAR_PATROL_AREA;

                auto ddPos = ScreenCoordsXY{ widget->left + windowPos.x, widget->top + windowPos.y };
                int32_t extraHeight = widget->height() + 1;
                WindowDropdownShowText(ddPos, extraHeight, colours[1], 0, 2);
                gDropdownDefaultIndex = 0;

                auto staff = GetStaff();
                if (staff == nullptr)
                {
                    return;
                }

                // Disable clear patrol area if no area is set.
                if (!staff->HasPatrolArea())
                {
                    Dropdown::SetDisabled(1, true);
                }
            }
        }
    }

    void OverviewOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
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

                    window_close_by_class(WindowClass::PatrolArea);

                    auto staffSetPatrolAreaAction = StaffSetPatrolAreaAction(
                        staff->sprite_index, {}, StaffSetPatrolAreaMode::ClearAll);
                    GameActions::Execute(&staffSetPatrolAreaAction);
                }
                else
                {
                    auto staffId = EntityId::FromUnderlying(number);
                    if (WindowPatrolAreaGetCurrentStaffId() == staffId)
                    {
                        window_close_by_class(WindowClass::PatrolArea);
                    }
                    else
                    {
                        WindowPatrolAreaOpen(staffId);
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

        SetWidgetPressed(WIDX_PATROL, WindowPatrolAreaGetCurrentStaffId() == staff->sprite_index);

        widgets[WIDX_PATROL].left = width - 25;
        widgets[WIDX_PATROL].right = width - 2;

        widgets[WIDX_RENAME].left = width - 25;
        widgets[WIDX_RENAME].right = width - 2;

        widgets[WIDX_LOCATE].left = width - 25;
        widgets[WIDX_LOCATE].right = width - 2;

        widgets[WIDX_FIRE].left = width - 25;
        widgets[WIDX_FIRE].right = width - 2;
    }

    void OverviewDraw(rct_drawpixelinfo* dpi)
    {
        // Draw the viewport no sound sprite
        if (viewport != nullptr)
        {
            window_draw_viewport(dpi, *this);

            if (viewport->flags & VIEWPORT_FLAG_SOUND_ON)
            {
                gfx_draw_sprite(dpi, ImageId(SPR_HEARING_VIEWPORT), windowPos + ScreenCoordsXY{ 2, 2 });
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
        int32_t widgetWidth = widget.width();
        DrawTextEllipsised(dpi, screenPos, widgetWidth, STR_BLACK_STRING, ft, { TextAlignment::CENTRE });
    }

    void DrawOverviewTabImage(rct_drawpixelinfo* dpi)
    {
        if (IsWidgetDisabled(WIDX_TAB_1))
            return;

        const auto& widget = widgets[WIDX_TAB_1];
        int32_t widgetWidth = widget.width() - 1;
        int32_t widgetHeight = widget.height() - 1;
        auto screenCoords = windowPos + ScreenCoordsXY{ widget.left + 1, widget.top + 1 };
        if (page == WINDOW_STAFF_OVERVIEW)
            widgetHeight++;

        rct_drawpixelinfo clip_dpi;
        if (!clip_drawpixelinfo(&clip_dpi, dpi, screenCoords, widgetWidth, widgetHeight))
        {
            return;
        }

        screenCoords = ScreenCoordsXY{ 14, 20 };

        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        if (staff->Is<Staff>() && staff->AssignedStaffType == StaffType::Entertainer)
            screenCoords.y++;

        int32_t imageIndex = GetPeepAnimation(staff->SpriteType).base_image + 1;

        int32_t offset = 0;

        if (page == WINDOW_STAFF_OVERVIEW)
        {
            offset = _tabAnimationOffset;
            offset = floor2(offset, 4);
        }
        imageIndex += offset;

        gfx_draw_sprite(&clip_dpi, ImageId(imageIndex, staff->TshirtColour, staff->TrousersColour), screenCoords);
    }

    void OverviewResize()
    {
        min_width = WW;
        max_width = 500;
        min_height = WH;
        max_height = 450;

        if (width < min_width)
        {
            width = min_width;
            Invalidate();
        }
        if (width > max_width)
        {
            Invalidate();
            width = max_width;
        }
        if (height < min_height)
        {
            height = min_height;
            Invalidate();
        }
        if (height > max_height)
        {
            Invalidate();
            height = max_height;
        }

        if (viewport != nullptr)
        {
            int32_t newWidth = width - 30;
            int32_t newHeight = height - 62;

            // Update the viewport size
            if (viewport->width != newWidth || viewport->height != newHeight)
            {
                viewport->width = newWidth;
                viewport->height = newHeight;
                viewport->view_width = viewport->zoom.ApplyTo(newWidth);
                viewport->view_height = viewport->zoom.ApplyTo(newHeight);
            }
        }

        ViewportInit();
    }

    void OverviewUpdate()
    {
        _tabAnimationOffset++;
        _tabAnimationOffset %= 24;
        InvalidateWidget(WIDX_TAB_1);
    }

    void OverviewToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords)
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

        auto info = get_map_coordinates_from_pos(screenCoords, ViewportInteractionItemAll);
        if (info.SpriteType == ViewportInteractionItem::None)
            return;

        gPickupPeepX = screenCoords.x - 1;
        gPickupPeepY = screenCoords.y + 16;

        picked_peep_frame++;
        picked_peep_frame %= 48;

        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        auto baseImageId = GetPeepAnimation(staff->SpriteType, PeepActionSpriteType::Ui).base_image;
        baseImageId += picked_peep_frame >> 2;
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

        PeepPickupAction pickupAction{
            PeepPickupType::Place, staffEntityId, { destCoords, tileElement->GetBaseZ() }, network_get_current_player_id()
        };
        pickupAction.SetCallback([](const GameAction* ga, const GameActions::Result* result) {
            if (result->Error != GameActions::Status::Ok)
                return;
            tool_cancel();
            gPickupPeepImage = ImageId();
        });
        GameActions::Execute(&pickupAction);
    }

    void OverviewToolAbort(WidgetIndex widgetIndex)
    {
        if (widgetIndex != WIDX_PICKUP)
            return;

        PeepPickupAction pickupAction{ PeepPickupType::Cancel,
                                       EntityId::FromUnderlying(number),
                                       { picked_peep_old_x, 0, 0 },
                                       network_get_current_player_id() };
        GameActions::Execute(&pickupAction);
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

        auto gameAction = StaffSetNameAction(EntityId::FromUnderlying(number), std::string{ text });
        GameActions::Execute(&gameAction);
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
        // This will be moved below where Items Checked is when all
        // of dropdown related functions are finished. This prevents
        // the dropdown from not working on first click.
        int32_t numCostumes = staff_get_available_entertainer_costume_list(_availableCostumes);
        for (int32_t i = 0; i < numCostumes; i++)
        {
            EntertainerCostume costume = _availableCostumes[i];
            if (staff->SpriteType == EntertainerCostumeToSprite(costume))
            {
                checkedIndex = i;
            }
            gDropdownItems[i].Args = StaffCostumeNames[static_cast<uint8_t>(costume)];
            gDropdownItems[i].Format = STR_DROPDOWN_MENU_LABEL;
        }

        auto ddPos = ScreenCoordsXY{ ddWidget->left + windowPos.x, ddWidget->top + windowPos.y };
        int32_t ddHeight = ddWidget->height() + 1;
        int32_t ddWidth = ddWidget->width() - 3;
        WindowDropdownShowTextCustomWidth(ddPos, ddHeight, colours[1], 0, Dropdown::Flag::StayOpen, numCostumes, ddWidth);

        // See above note.
        if (checkedIndex != -1)
        {
            Dropdown::SetChecked(checkedIndex, true);
        }
    }

    void OptionsOnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex)
    {
        if (widgetIndex != WIDX_COSTUME_BTN)
        {
            return;
        }

        if (dropdownIndex == -1)
            return;

        EntertainerCostume costume = _availableCostumes[dropdownIndex];
        auto staffSetCostumeAction = StaffSetCostumeAction(EntityId::FromUnderlying(number), costume);
        GameActions::Execute(&staffSetCostumeAction);
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
            case StaffType::Entertainer:
                widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Empty;
                widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Empty;
                widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Empty;
                widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Empty;
                widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::DropdownMenu;
                widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Button;
                widgets[WIDX_COSTUME_BOX].text = StaffCostumeNames[EnumValue(staff->SpriteType) - 4];
                break;
            case StaffType::Handyman:
                widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_1].text = STR_STAFF_OPTION_SWEEP_FOOTPATHS;
                widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_2].text = STR_STAFF_OPTION_WATER_GARDENS;
                widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_3].text = STR_STAFF_OPTION_EMPTY_LITTER;
                widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_4].text = STR_STAFF_OPTION_MOW_GRASS;
                widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::Empty;
                widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Empty;
                OptionsSetCheckboxValues();
                break;
            case StaffType::Mechanic:
                widgets[WIDX_CHECKBOX_1].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_1].text = STR_INSPECT_RIDES;
                widgets[WIDX_CHECKBOX_2].type = WindowWidgetType::Checkbox;
                widgets[WIDX_CHECKBOX_2].text = STR_FIX_RIDES;
                widgets[WIDX_CHECKBOX_3].type = WindowWidgetType::Empty;
                widgets[WIDX_CHECKBOX_4].type = WindowWidgetType::Empty;
                widgets[WIDX_COSTUME_BOX].type = WindowWidgetType::Empty;
                widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Empty;
                widgets[WIDX_COSTUME_BTN].type = WindowWidgetType::Empty;
                OptionsSetCheckboxValues();
                break;
            case StaffType::Security:
                // Security guards don't have an options screen.
                break;
            case StaffType::Count:
                break;
        }
    }

    void OptionsSetCheckboxValues()
    {
        SetCheckboxValue(WIDX_CHECKBOX_1, false);
        SetCheckboxValue(WIDX_CHECKBOX_2, false);
        SetCheckboxValue(WIDX_CHECKBOX_3, false);
        SetCheckboxValue(WIDX_CHECKBOX_4, false);

        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        auto staffOrders = staff->StaffOrders;

        for (auto index = bitscanforward(staffOrders); index != -1; index = bitscanforward(staffOrders))
        {
            staffOrders &= ~(1 << index);
            SetCheckboxValue(WIDX_CHECKBOX_1 + index, true);
        }
    }

    void OptionsResize()
    {
        min_width = 190;
        max_width = 190;
        min_height = 126;
        max_height = 126;

        if (width < min_width)
        {
            width = min_width;
            Invalidate();
        }
        if (width > max_width)
        {
            Invalidate();
            width = max_width;
        }
        if (height < min_height)
        {
            height = min_height;
            Invalidate();
        }
        if (height > max_height)
        {
            Invalidate();
            height = max_height;
        }
    }

    void OptionsUpdate()
    {
        frame_no++;
        InvalidateWidget(WIDX_TAB_2);
    }
#pragma endregion

#pragma region Statistics tab events
    void StatsDraw(rct_drawpixelinfo* dpi)
    {
        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        auto screenCoords = windowPos + ScreenCoordsXY{ widgets[WIDX_RESIZE].left + 4, widgets[WIDX_RESIZE].top + 4 };

        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            auto ft = Formatter();
            ft.Add<money64>(GetStaffWage(staff->AssignedStaffType));
            DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_WAGES, ft);
            screenCoords.y += LIST_ROW_HEIGHT;
        }

        auto ft = Formatter();
        ft.Add<int32_t>(staff->GetHireDate());
        DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_EMPLOYED_FOR, ft);
        screenCoords.y += LIST_ROW_HEIGHT;

        switch (staff->AssignedStaffType)
        {
            case StaffType::Handyman:
                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffLawnsMown);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_LAWNS_MOWN, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffGardensWatered);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_GARDENS_WATERED, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffLitterSwept);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_LITTER_SWEPT, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffBinsEmptied);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_BINS_EMPTIED, ft);
                break;
            case StaffType::Mechanic:
                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffRidesInspected);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_RIDES_INSPECTED, ft);
                screenCoords.y += LIST_ROW_HEIGHT;

                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffRidesFixed);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_RIDES_FIXED, ft);
                break;
            case StaffType::Security:
                ft = Formatter();
                ft.Add<uint32_t>(staff->StaffVandalsStopped);
                DrawTextBasic(dpi, screenCoords, STR_STAFF_STAT_VANDALS_STOPPED, ft);
                break;
            case StaffType::Entertainer:
            case StaffType::Count:
                break;
        }
    }

    void StatsResize()
    {
        min_width = 190;
        max_width = 190;
        min_height = 126;
        max_height = 126;

        if (width < min_width)
        {
            width = min_width;
            Invalidate();
        }
        if (width > max_width)
        {
            Invalidate();
            width = max_width;
        }
        if (height < min_height)
        {
            height = min_height;
            Invalidate();
        }
        if (height > max_height)
        {
            Invalidate();
            height = max_height;
        }
    }

    void StatsUpdate()
    {
        frame_no++;
        InvalidateWidget(WIDX_TAB_3);

        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        if (staff->WindowInvalidateFlags & PEEP_INVALIDATE_STAFF_STATS)
        {
            staff->WindowInvalidateFlags &= ~PEEP_INVALIDATE_STAFF_STATS;
            Invalidate();
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

        for (WidgetIndex widgetIndex = WIDX_TAB_1; widgets[widgetIndex].type != WIDGETS_END.type; widgetIndex++)
        {
            SetWidgetDisabled(widgetIndex, false);
        }

        if (staff->AssignedStaffType == StaffType::Security)
        {
            SetWidgetDisabled(WIDX_TAB_2, true);
        }

        if (page == WINDOW_STAFF_OVERVIEW)
        {
            if (staff->CanBePickedUp())
            {
                SetWidgetDisabled(WIDX_PICKUP, false);
            }
            else
            {
                SetWidgetDisabled(WIDX_PICKUP, true);
            }
        }
    }

    void CancelTools()
    {
        if (input_test_flag(INPUT_FLAG_TOOL_ACTIVE))
        {
            if (number == gCurrentToolWidget.window_number && classification == gCurrentToolWidget.window_classification)
                tool_cancel();
        }
    }

    void SetPage(int32_t pageNum)
    {
        CancelTools();

        int32_t listen = 0;
        if (page == WINDOW_STAFF_OVERVIEW && viewport != nullptr)
        {
            if (!(viewport->flags & VIEWPORT_FLAG_SOUND_ON))
                listen = 1;
        }

        page = pageNum;
        frame_no = 0;
        pressed_widgets = 0;
        hold_down_widgets = 0;
        widgets = window_staff_page_widgets[page];

        RemoveViewport();

        Invalidate();
        window_event_resize_call(this);
        window_event_invalidate_call(this);
        InitScrollWidgets();
        ViewportInit();
        Invalidate();

        if (listen && viewport != nullptr)
            viewport->flags |= VIEWPORT_FLAG_SOUND_ON;
    }

    void SetPressedTab()
    {
        for (int32_t i = 0; i < WINDOW_STAFF_PAGE_COUNT; i++)
            SetWidgetPressed((WIDX_TAB_1 + i), false);
        SetWidgetPressed(WIDX_TAB_1 + page, true);
    }

    void SetOrder(int32_t orderId)
    {
        auto staff = GetStaff();
        if (staff == nullptr)
        {
            return;
        }

        uint8_t newOrders = staff->StaffOrders ^ (1 << orderId);
        auto staffSetOrdersAction = StaffSetOrdersAction(EntityId::FromUnderlying(number), newOrders);
        GameActions::Execute(&staffSetOrdersAction);
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
        if (staff->State != PeepState::Picked)
        {
            tempFocus = Focus(staff->sprite_index);
        }

        uint16_t viewport_flags;

        if (viewport != nullptr)
        {
            if (tempFocus == focus)
                return;

            viewport_flags = viewport->flags;
            RemoveViewport();
        }
        else
        {
            viewport_flags = 0;
            if (gConfigGeneral.AlwaysShowGridlines)
                viewport_flags |= VIEWPORT_FLAG_GRIDLINES;
        }

        window_event_invalidate_call(this);

        focus = tempFocus;

        if (staff->State != PeepState::Picked)
        {
            if (viewport == nullptr)
            {
                const auto& viewWidget = widgets[WIDX_VIEWPORT];

                auto screenPos = ScreenCoordsXY{ viewWidget.left + 1 + windowPos.x, viewWidget.top + 1 + windowPos.y };
                int32_t viewportWidth = viewWidget.width() - 1;
                int32_t viewportHeight = viewWidget.height() - 1;

                viewport_create(this, screenPos, viewportWidth, viewportHeight, focus.value());
                flags |= WF_NO_SCROLLING;
                Invalidate();
            }
        }

        if (viewport != nullptr)
            viewport->flags = viewport_flags;
        Invalidate();
    }

    void ShowLocateDropdown(rct_widget* widget)
    {
        gDropdownItems[0].Format = STR_LOCATE_SUBJECT_TIP;
        gDropdownItems[1].Format = STR_FOLLOW_SUBJECT_TIP;

        WindowDropdownShowText(
            { windowPos.x + widget->left, windowPos.y + widget->top }, widget->height() + 1, colours[1], 0, 2);
        gDropdownDefaultIndex = 0;
    }

    void FollowPeep()
    {
        rct_window* main = window_get_main();
        window_follow_sprite(*main, EntityId::FromUnderlying(number));
    }

    void DrawTabImages(rct_drawpixelinfo* dpi)
    {
        DrawOverviewTabImage(dpi);
        DrawTabImage(dpi, WINDOW_STAFF_OPTIONS, SPR_TAB_STAFF_OPTIONS_0);
        DrawTabImage(dpi, WINDOW_STAFF_STATISTICS, SPR_TAB_STATS_0);
    }

    void DrawTabImage(rct_drawpixelinfo* dpi, int32_t p, int32_t baseImageId)
    {
        WidgetIndex widgetIndex = WIDX_TAB_1 + p;
        rct_widget* widget = &widgets[widgetIndex];

        auto screenCoords = windowPos + ScreenCoordsXY{ widget->left, widget->top };

        if (!IsWidgetDisabled(widgetIndex))
        {
            if (page == p)
            {
                int32_t frame = frame_no / TabAnimationDivisor[page - 1];
                baseImageId += (frame % TabAnimationFrames);
            }

            // Draw normal, enabled sprite.
            gfx_draw_sprite(dpi, ImageId(baseImageId), screenCoords);
        }
    }

    Staff* GetStaff()
    {
        return GetEntity<Staff>(EntityId::FromUnderlying(number));
    }

    static constexpr int32_t TabAnimationDivisor[] = {
        2, // WINDOW_STAFF_OPTIONS,
        4, // WINDOW_STAFF_STATISTICS,
    };

    static constexpr int32_t TabAnimationFrames = 7;
};

rct_window* WindowStaffOpen(Peep* peep)
{
    auto w = static_cast<StaffWindow*>(window_bring_to_front_by_number(WindowClass::Peep, peep->sprite_index.ToUnderlying()));

    if (w != nullptr)
        return w;

    w = WindowCreate<StaffWindow>(WindowClass::Peep, WW, WH, WF_10 | WF_RESIZABLE);

    if (w == nullptr)
        return nullptr;

    if (w != nullptr)
        w->Initialise(peep->sprite_index);

    return w;
}
