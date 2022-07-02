/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Editor.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/management/Research.h>
#include <openrct2/object/DefaultObjects.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/sprites.h>
#include <openrct2/util/Util.h>
#include <openrct2/world/Scenery.h>

#pragma region Widgets

static constexpr const int32_t WW = 600;
static constexpr const int32_t WH = 400;
static constexpr const rct_string_id WINDOW_TITLE = STR_INVENTION_LIST;

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_RESIZE,
    WIDX_TAB_1,
    WIDX_PRE_RESEARCHED_SCROLL,
    WIDX_RESEARCH_ORDER_SCROLL,
    WIDX_PREVIEW,
    WIDX_MOVE_ITEMS_TO_TOP,
    WIDX_MOVE_ITEMS_TO_BOTTOM,
    WIDX_RANDOM_SHUFFLE
};

static rct_widget window_editor_inventions_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0,  43}, {600, 357}, WindowWidgetType::Resize,  WindowColour::Secondary                                             ),
    MakeTab   ({  3,  17}                                                                                               ),
    MakeWidget({  4,  56}, {368, 161}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                            ),
    MakeWidget({  4, 231}, {368, 157}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                            ),
    MakeWidget({431, 106}, {114, 114}, WindowWidgetType::FlatBtn, WindowColour::Secondary                                             ),
    MakeWidget({375, 343}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MOVE_ALL_TOP                           ),
    MakeWidget({375, 358}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MOVE_ALL_BOTTOM                        ),
    MakeWidget({375, 373}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_RANDOM_SHUFFLE,  STR_RANDOM_SHUFFLE_TIP),
    WIDGETS_END,
};

static rct_widget _inventionListDragWidgets[] = {
    MakeWidget({0, 0}, {150, 14}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

#pragma endregion

static std::pair<rct_string_id, Formatter> WindowEditorInventionsListPrepareName(const ResearchItem * researchItem, bool withGap);

// clang-format on

static void WindowEditorInventionsListDragOpen(ResearchItem* researchItem);
static const ResearchItem* WindowEditorInventionsListDragGetItem();

/**
 *
 *  rct2: 0x0068596F
 * Sets rides that are in use to be always researched
 */
static void ResearchRidesSetup()
{
    // Reset all objects to not required
    for (auto objectType : TransientObjectTypes)
    {
        auto maxObjects = object_entry_group_counts[EnumValue(objectType)];
        for (int32_t i = 0; i < maxObjects; i++)
        {
            Editor::ClearSelectedObject(objectType, i, ObjectSelectionFlags::AllFlags);
        }
    }

    // Set research required for rides in use
    for (const auto& ride : GetRideManager())
    {
        Editor::SetSelectedObject(ObjectType::Ride, ride.subtype, ObjectSelectionFlags::Selected | ObjectSelectionFlags::InUse);
    }
}

/**
 *
 *  rct2: 0x006855E7
 */
static void MoveResearchItem(const ResearchItem& item, ResearchItem* beforeItem, int32_t scrollIndex)
{
    auto w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
    if (w != nullptr)
    {
        w->research_item = nullptr;
        w->Invalidate();
    }

    ResearchRemove(item);

    auto& researchList = scrollIndex == 0 ? gResearchItemsInvented : gResearchItemsUninvented;
    if (beforeItem != nullptr)
    {
        for (size_t i = 0; i < researchList.size(); i++)
        {
            if (researchList[i] == *beforeItem)
            {
                researchList.insert((researchList.begin() + i), item);
                return;
            }
        }
    }

    // Still not found? Append to end of list.
    researchList.push_back(item);
}

#pragma region Invention List Window

class InventionListWindow final : public Window
{
public:
    void OnOpen() override
    {
        ResearchRidesSetup();

        widgets = window_editor_inventions_list_widgets;
        InitScrollWidgets();
        selected_tab = 0;
        research_item = nullptr;

        min_width = WW;
        min_height = WH;
        max_width = WW * 2;
        max_height = WH * 2;
    }

    void OnClose() override
    {
        research_remove_flags();

        // When used in-game (as a cheat)
        if (!(gScreenFlags & SCREEN_FLAGS_EDITOR))
        {
            gSilentResearch = true;
            research_reset_current_item();
            gSilentResearch = false;
        }
    }

    void OnMouseUp(rct_widgetindex widx) override
    {
        switch (widx)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_RANDOM_SHUFFLE:
                research_items_shuffle();
                Invalidate();
                break;
            case WIDX_MOVE_ITEMS_TO_TOP:
                research_items_make_all_researched();
                InitScrollWidgets();
                Invalidate();
                break;
            case WIDX_MOVE_ITEMS_TO_BOTTOM:
                research_items_make_all_unresearched();
                InitScrollWidgets();
                Invalidate();
                break;
        }
    }

    void OnResize() override
    {
        if (width < min_width)
        {
            Invalidate();
            width = min_width;
        }
        if (height < min_height)
        {
            Invalidate();
            height = min_height;
        }
    }

    void OnUpdate() override
    {
        frame_no++;
        OnPrepareDraw();
        widget_invalidate(this, WIDX_TAB_1);

        if (WindowEditorInventionsListDragGetItem() != nullptr)
            return;

        Invalidate();
    }

    ScreenSize OnScrollGetSize(int32_t scrollIndex) override
    {
        ScreenSize size{};
        if (scrollIndex == 0)
        {
            size.height = static_cast<int32_t>(gResearchItemsInvented.size()) * SCROLLABLE_ROW_HEIGHT;
        }
        else
        {
            size.height = static_cast<int32_t>(gResearchItemsUninvented.size()) * SCROLLABLE_ROW_HEIGHT;
        }
        return size;
    }

    void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto* researchItem = GetItemFromScrollY(scrollIndex, screenCoords.y);
        if (researchItem != research_item)
        {
            research_item = researchItem;
            Invalidate();

            // Prevent always-researched items from being highlighted when hovered over
            if (researchItem != nullptr && researchItem->IsAlwaysResearched())
            {
                research_item = nullptr;
            }
        }
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto* researchItem = GetItemFromScrollY(scrollIndex, screenCoords.y);
        if (researchItem == nullptr)
            return;

        // Disallow picking up always-researched items
        if (researchItem->IsAlwaysResearched())
            return;

        Invalidate();
        WindowEditorInventionsListDragOpen(researchItem);
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        // Draw background
        uint8_t paletteIndex = ColourMapA[colours[1]].mid_light;
        gfx_clear(&dpi, paletteIndex);

        int16_t boxWidth = widgets[WIDX_RESEARCH_ORDER_SCROLL].width();
        int16_t columnSplitOffset = boxWidth / 2;
        int32_t itemY = -SCROLLABLE_ROW_HEIGHT;
        auto* dragItem = WindowEditorInventionsListDragGetItem();

        const auto& researchList = scrollIndex == 0 ? gResearchItemsInvented : gResearchItemsUninvented;
        for (const auto& researchItem : researchList)
        {
            itemY += SCROLLABLE_ROW_HEIGHT;
            if (itemY + SCROLLABLE_ROW_HEIGHT < dpi.y || itemY >= dpi.y + dpi.height)
                continue;

            if (research_item == &researchItem)
            {
                int32_t top, bottom;
                if (dragItem == nullptr)
                {
                    // Highlight
                    top = itemY;
                    bottom = itemY + SCROLLABLE_ROW_HEIGHT - 1;
                }
                else
                {
                    // Drop horizontal rule
                    top = itemY - 1;
                    bottom = itemY;
                }

                gfx_filter_rect(&dpi, { 0, top, boxWidth, bottom }, FilterPaletteID::PaletteDarken1);
            }

            if (dragItem != nullptr && researchItem == *dragItem)
                continue;

            // TODO: this parameter by itself produces very light text.
            // It needs a {BLACK} token in the string to work properly.
            colour_t colour = COLOUR_BLACK;
            FontSpriteBase fontSpriteBase = FontSpriteBase::MEDIUM;

            if (researchItem.IsAlwaysResearched())
            {
                if (research_item == &researchItem && dragItem == nullptr)
                    fontSpriteBase = FontSpriteBase::MEDIUM_EXTRA_DARK;
                else
                    fontSpriteBase = FontSpriteBase::MEDIUM_DARK;
                colour = colours[1] | COLOUR_FLAG_INSET;
            }

            const rct_string_id itemNameId = researchItem.GetName();

            if (researchItem.type == Research::EntryType::Ride
                && !GetRideTypeDescriptor(researchItem.baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
            {
                const auto rideEntry = get_ride_entry(researchItem.entryIndex);
                const rct_string_id rideTypeName = get_ride_naming(researchItem.baseRideType, rideEntry).Name;

                // Draw group name
                auto ft = Formatter();
                ft.Add<rct_string_id>(rideTypeName);
                DrawTextEllipsised(
                    &dpi, { 1, itemY }, columnSplitOffset - 11, STR_INVENTIONS_LIST_RIDE_AND_VEHICLE_NAME, ft,
                    { colour, fontSpriteBase });

                // Draw vehicle name
                ft = Formatter();
                ft.Add<rct_string_id>(itemNameId);
                DrawTextEllipsised(
                    &dpi, { columnSplitOffset + 1, itemY }, columnSplitOffset - 11, STR_BLACK_STRING, ft,
                    { colour, fontSpriteBase });
            }
            else
            {
                // Scenery group, flat ride or shop
                auto ft = Formatter();
                ft.Add<rct_string_id>(itemNameId);
                DrawTextEllipsised(&dpi, { 1, itemY }, boxWidth, STR_BLACK_STRING, ft, { colour, fontSpriteBase });
            }
        }
    }

    CursorID OnCursor(rct_widgetindex widx, const ScreenCoordsXY& screenCoords, CursorID fallback) override
    {
        int32_t scrollIndex = 0;

        switch (widx)
        {
            case WIDX_PRE_RESEARCHED_SCROLL:
                scrollIndex = 0;
                break;
            case WIDX_RESEARCH_ORDER_SCROLL:
                scrollIndex = 1;
                break;
            default:
                return fallback;
        }

        // Use the open hand as cursor for items that can be picked up
        auto* researchItem = GetItemFromScrollY(scrollIndex, screenCoords.y);
        if (researchItem != nullptr && !researchItem->IsAlwaysResearched())
        {
            return CursorID::HandOpen;
        }
        return fallback;
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        DrawWidgets(dpi);

        // Tab image
        auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top };
        gfx_draw_sprite(&dpi, ImageId(SPR_TAB_FINANCES_RESEARCH_0 + (frame_no / 2) % 8), screenPos);

        // Pre-researched items label
        screenPos = windowPos
            + ScreenCoordsXY{ widgets[WIDX_PRE_RESEARCHED_SCROLL].left, widgets[WIDX_PRE_RESEARCHED_SCROLL].top - 11 };
        DrawTextBasic(&dpi, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_PREINVENTED_ITEMS);

        // Research order label
        screenPos = windowPos
            + ScreenCoordsXY{ widgets[WIDX_RESEARCH_ORDER_SCROLL].left, widgets[WIDX_RESEARCH_ORDER_SCROLL].top - 11 };
        DrawTextBasic(&dpi, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_TO_BE_INVENTED_ITEMS);

        // Preview background
        auto& bkWidget = widgets[WIDX_PREVIEW];
        gfx_fill_rect(
            &dpi,
            { windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 },
              windowPos + ScreenCoordsXY{ bkWidget.right - 1, bkWidget.bottom - 1 } },
            ColourMapA[colours[1]].darkest);

        auto* researchItem = WindowEditorInventionsListDragGetItem();
        if (researchItem == nullptr || researchItem->IsNull())
            researchItem = research_item;
        // If the research item is null or a list separator.
        if (researchItem == nullptr || researchItem->IsNull())
            return;

        // Preview image
        ObjectType objectEntryType = ObjectType::SceneryGroup;
        if (researchItem->type == Research::EntryType::Ride)
            objectEntryType = ObjectType::Ride;

        auto chunk = object_entry_get_chunk(objectEntryType, researchItem->entryIndex);
        if (chunk == nullptr)
            return;

        // Draw preview
        const auto* object = object_entry_get_object(objectEntryType, researchItem->entryIndex);
        if (object != nullptr)
        {
            rct_drawpixelinfo clipDPI;
            screenPos = windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 };
            const auto clipWidth = bkWidget.width() - 1;
            const auto clipHeight = bkWidget.height() - 1;
            if (clip_drawpixelinfo(&clipDPI, &dpi, screenPos, clipWidth, clipHeight))
            {
                object->DrawPreview(&clipDPI, clipWidth, height);
            }
        }

        // Item name
        screenPos = windowPos + ScreenCoordsXY{ bkWidget.midX() + 1, bkWidget.bottom + 3 };
        const auto itemWidth = width - widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;

        auto [drawString, ft] = WindowEditorInventionsListPrepareName(researchItem, false);
        DrawTextEllipsised(&dpi, screenPos, itemWidth, drawString, ft, { TextAlignment::CENTRE });
        screenPos.y += 15;

        // Item category
        screenPos.x = windowPos.x + widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
        ft = Formatter();
        ft.Add<rct_string_id>(researchItem->GetCategoryInventionString());
        DrawTextBasic(&dpi, screenPos, STR_INVENTION_RESEARCH_GROUP, ft);
    }

    void OnPrepareDraw() override
    {
        pressed_widgets |= 1ULL << WIDX_PREVIEW;
        pressed_widgets |= 1ULL << WIDX_TAB_1;

        widgets[WIDX_CLOSE].type = gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR ? WindowWidgetType::Empty
                                                                               : WindowWidgetType::CloseBox;

        widgets[WIDX_BACKGROUND].right = width - 1;
        widgets[WIDX_BACKGROUND].bottom = height - 1;
        widgets[WIDX_TITLE].right = width - 2;
        widgets[WIDX_CLOSE].left = width - 13;
        widgets[WIDX_CLOSE].right = width - 3;
        widgets[WIDX_RESIZE].right = width - 1;
        widgets[WIDX_RESIZE].bottom = height - 1;

        int16_t scroll_list_height = (height - 88) / 2;

        widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom = 60 + scroll_list_height;
        widgets[WIDX_PRE_RESEARCHED_SCROLL].right = width - 229;

        widgets[WIDX_RESEARCH_ORDER_SCROLL].top = widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom + 15;
        widgets[WIDX_RESEARCH_ORDER_SCROLL].bottom = widgets[WIDX_RESEARCH_ORDER_SCROLL].top + scroll_list_height;
        widgets[WIDX_RESEARCH_ORDER_SCROLL].right = width - 229;

        widgets[WIDX_PREVIEW].left = width - 169;
        widgets[WIDX_PREVIEW].right = width - 56;

        widgets[WIDX_MOVE_ITEMS_TO_TOP].top = height - 57;
        widgets[WIDX_MOVE_ITEMS_TO_TOP].bottom = height - 44;
        widgets[WIDX_MOVE_ITEMS_TO_TOP].left = width - 225;
        widgets[WIDX_MOVE_ITEMS_TO_TOP].right = width - 6;

        widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].top = height - 42;
        widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].bottom = height - 29;
        widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].left = width - 225;
        widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].right = width - 6;

        widgets[WIDX_RANDOM_SHUFFLE].top = height - 27;
        widgets[WIDX_RANDOM_SHUFFLE].bottom = height - 14;
        widgets[WIDX_RANDOM_SHUFFLE].left = width - 225;
        widgets[WIDX_RANDOM_SHUFFLE].right = width - 6;
    }

    // Get Research Item and Scroll Id (scroll id represents invented(0)/not invented(1)/failure(-1)
    std::pair<ResearchItem*, int32_t> GetResearchItemAt(const ScreenCoordsXY& screenCoords)
    {
        if (windowPos.x <= screenCoords.x && windowPos.y < screenCoords.y && windowPos.x + width > screenCoords.x
            && windowPos.y + height > screenCoords.y)
        {
            rct_widgetindex widgetIndex = window_find_widget_from_point(this, screenCoords);
            auto& widget = widgets[widgetIndex];
            if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL)
            {
                gPressedWidget.widget_index = widgetIndex;
                int32_t outScrollArea{};
                ScreenCoordsXY outScrollCoords{};
                int32_t outScrollId{};
                WidgetScrollGetPart(this, &widget, screenCoords, outScrollCoords, &outScrollArea, &outScrollId);
                if (outScrollArea == SCROLL_PART_VIEW)
                {
                    outScrollId = outScrollId == 0 ? 0 : 1;

                    int32_t scrollY = outScrollCoords.y + 6;
                    return std::make_pair(GetItemFromScrollYIncludeSeps(outScrollId, scrollY), outScrollId);
                }
            }
        }

        return std::make_pair(nullptr, -1);
    }

private:
    ResearchItem* GetItemFromScrollY(int32_t scrollIndex, int32_t y) const
    {
        auto& researchList = scrollIndex == 0 ? gResearchItemsInvented : gResearchItemsUninvented;
        for (auto& researchItem : researchList)
        {
            y -= SCROLLABLE_ROW_HEIGHT;
            if (y < 0)
            {
                return &researchItem;
            }
        }

        return nullptr;
    }

    ResearchItem* GetItemFromScrollYIncludeSeps(int32_t scrollIndex, int32_t y) const
    {
        auto& researchList = scrollIndex == 0 ? gResearchItemsInvented : gResearchItemsUninvented;
        for (auto& researchItem : researchList)
        {
            y -= SCROLLABLE_ROW_HEIGHT;
            if (y < 0)
            {
                return &researchItem;
            }
        }
        return nullptr;
    }
};

/**
 *
 *  rct2: 0x00684E04
 */
rct_window* WindowEditorInventionsListOpen()
{
    return WindowFocusOrCreate<InventionListWindow>(
        WC_EDITOR_INVENTION_LIST, WW, WH, WF_NO_SCROLLING | WF_RESIZABLE | WF_CENTRE_SCREEN);
}

static std::pair<ResearchItem*, int32_t> GetResearchItemAt(const ScreenCoordsXY& screenCoords)
{
    auto* w = static_cast<InventionListWindow*>(window_find_by_class(WC_EDITOR_INVENTION_LIST));
    if (w != nullptr)
    {
        return w->GetResearchItemAt(screenCoords);
    }

    return std::make_pair(nullptr, -1);
}
#pragma endregion

#pragma region Drag item

class InventionDragWindow final : public Window
{
    ResearchItem _draggedItem;

public:
    void OnOpen() override
    {
        widgets = _inventionListDragWidgets;
        colours[1] = COLOUR_WHITE;
    }

    CursorID OnCursor(const rct_widgetindex widx, const ScreenCoordsXY& screenCoords, const CursorID defaultCursor) override
    {
        auto* inventionListWindow = window_find_by_class(WC_EDITOR_INVENTION_LIST);
        if (inventionListWindow != nullptr)
        {
            auto res = GetResearchItemAt(screenCoords);
            if (res.first != inventionListWindow->research_item)
            {
                inventionListWindow->Invalidate();
            }
        }

        return CursorID::HandClosed;
    }

    void OnMoved(const ScreenCoordsXY& screenCoords) override
    {
        std::pair<ResearchItem*, int32_t> res = std::make_pair(nullptr, -1);
        // Skip always researched items, so that the dragged item gets placed underneath them
        auto newScreenCoords = screenCoords;
        do
        {
            res = GetResearchItemAt(newScreenCoords);
            newScreenCoords.y += LIST_ROW_HEIGHT;
        } while (res.first != nullptr && res.first->IsAlwaysResearched());

        if (res.second != -1)
        {
            MoveResearchItem(_draggedItem, res.first, res.second);
        }

        window_invalidate_by_class(WC_EDITOR_INVENTION_LIST);
        Close();
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        auto screenCoords = windowPos + ScreenCoordsXY{ 0, 2 };

        auto [drawString, ft] = WindowEditorInventionsListPrepareName(&_draggedItem, true);
        DrawTextBasic(&dpi, screenCoords, drawString, ft, { COLOUR_BLACK | COLOUR_FLAG_OUTLINE });
    }

    void Init(ResearchItem& researchItem)
    {
        _draggedItem = researchItem;
        rct_string_id stringId = researchItem.GetName();
        char buffer[256] = {};
        if (researchItem.type == Research::EntryType::Ride
            && !GetRideTypeDescriptor(researchItem.baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        {
            const auto rideEntry = get_ride_entry(researchItem.entryIndex);
            const rct_string_id rideTypeName = get_ride_naming(researchItem.baseRideType, rideEntry).Name;
            Formatter ft;
            ft.Add<rct_string_id>(stringId);
            ft.Add<rct_string_id>(rideTypeName);
            format_string(buffer, 256, STR_INVENTIONS_LIST_RIDE_AND_VEHICLE_NAME, &ft);
        }
        else
        {
            format_string(buffer, 256, stringId, nullptr);
        }

        auto stringWidth = gfx_get_string_width(buffer, FontSpriteBase::MEDIUM);
        widgets[0].right = stringWidth;

        Invalidate();
        windowPos = gTooltipCursor - ScreenCoordsXY{ stringWidth / 2, 7 };
        width = stringWidth;
        Invalidate();

        InputWindowPositionBegin(this, 0, gTooltipCursor);
    }

    const ResearchItem& GetItem() const
    {
        return _draggedItem;
    }
};

static void WindowEditorInventionsListDragOpen(ResearchItem* researchItem)
{
    window_close_by_class(WC_EDITOR_INVENTION_LIST_DRAG);
    auto* wnd = WindowCreate<InventionDragWindow>(
        WC_EDITOR_INVENTION_LIST_DRAG, 10, 14, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING);
    if (wnd != nullptr)
    {
        wnd->Init(*researchItem);
    }
}

static const ResearchItem* WindowEditorInventionsListDragGetItem()
{
    auto* wnd = static_cast<InventionDragWindow*>(window_find_by_class(WC_EDITOR_INVENTION_LIST_DRAG));
    if (wnd == nullptr)
    {
        return nullptr;
    }
    return &wnd->GetItem();
}

#pragma endregion

static std::pair<rct_string_id, Formatter> WindowEditorInventionsListPrepareName(const ResearchItem* researchItem, bool withGap)
{
    rct_string_id drawString;
    rct_string_id stringId = researchItem->GetName();
    auto ft = Formatter();

    if (researchItem->type == Research::EntryType::Ride
        && !GetRideTypeDescriptor(researchItem->baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        drawString = withGap ? STR_INVENTIONS_LIST_RIDE_AND_VEHICLE_NAME_DRAG : STR_WINDOW_COLOUR_2_STRINGID_STRINGID;
        rct_string_id rideTypeName = get_ride_naming(researchItem->baseRideType, get_ride_entry(researchItem->entryIndex)).Name;
        ft.Add<rct_string_id>(rideTypeName);
        ft.Add<rct_string_id>(stringId);
    }
    else
    {
        drawString = STR_WINDOW_COLOUR_2_STRINGID;
        ft.Add<rct_string_id>(stringId);
    }

    return std::make_pair(drawString, ft);
}
