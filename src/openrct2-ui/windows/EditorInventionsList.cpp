/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
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
static constexpr const StringId WINDOW_TITLE = STR_INVENTION_LIST;

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

static Widget _inventionListWidgets[] = {
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

static Widget _inventionListDragWidgets[] = {
    MakeWidget({0, 0}, {150, 14}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};
// clang-format on

#pragma endregion

static void WindowEditorInventionsListDragOpen(
    ResearchItem* researchItem, const ScreenCoordsXY& editorPos, int objectSelectionScrollWidth);
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

static void DrawResearchItem(
    rct_drawpixelinfo& dpi, const ResearchItem& researchItem, const int16_t& width, const ScreenCoordsXY& screenCoords,
    StringId format, TextPaint textPaint)
{
    const StringId itemNameId = researchItem.GetName();
    int16_t columnSplitOffset = width / 2;

    if (researchItem.type == Research::EntryType::Ride
        && !GetRideTypeDescriptor(researchItem.baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        const StringId rideTypeName = get_ride_naming(researchItem.baseRideType, *get_ride_entry(researchItem.entryIndex)).Name;

        // Draw group name
        auto ft = Formatter();
        ft.Add<StringId>(rideTypeName);
        DrawTextEllipsised(&dpi, screenCoords, columnSplitOffset - 11, format, ft, textPaint);

        // Draw vehicle name
        ft = Formatter();
        ft.Add<StringId>(itemNameId);
        DrawTextEllipsised(
            &dpi, { screenCoords + ScreenCoordsXY{ columnSplitOffset, 0 } }, columnSplitOffset - 11, format, ft, textPaint);
    }
    else
    {
        // Scenery group, flat ride or shopdis
        auto ft = Formatter();
        ft.Add<StringId>(itemNameId);
        DrawTextEllipsised(&dpi, screenCoords, width, format, ft, textPaint);
    }
}

#pragma region Invention List Window
struct InventionListItem
{
    ResearchItem* research = nullptr;
    bool isInvented = true;
};

class InventionListWindow final : public Window
{
    ResearchItem* _selectedResearchItem;

public:
    void OnOpen() override
    {
        ResearchRidesSetup();

        widgets = _inventionListWidgets;
        InitScrollWidgets();
        selected_tab = 0;
        _selectedResearchItem = nullptr;

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

    void OnMouseUp(WidgetIndex widx) override
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
        WidgetInvalidate(*this, WIDX_TAB_1);

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
        auto* researchItem = GetItemFromScrollY(scrollIndex == 0, screenCoords.y);
        if (researchItem != _selectedResearchItem)
        {
            _selectedResearchItem = researchItem;
            Invalidate();

            // Prevent always-researched items from being highlighted when hovered over
            if (researchItem != nullptr && researchItem->IsAlwaysResearched())
            {
                _selectedResearchItem = nullptr;
            }
        }
    }

    void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
    {
        auto* researchItem = GetItemFromScrollY(scrollIndex == 0, screenCoords.y);
        if (researchItem == nullptr)
            return;

        // Disallow picking up always-researched items
        if (researchItem->IsAlwaysResearched())
            return;

        Invalidate();

        WindowEditorInventionsListDragOpen(researchItem, windowPos, widgets[WIDX_PRE_RESEARCHED_SCROLL].right);
    }

    void OnScrollDraw(int32_t scrollIndex, rct_drawpixelinfo& dpi) override
    {
        // Draw background
        uint8_t paletteIndex = ColourMapA[colours[1]].mid_light;
        GfxClear(&dpi, paletteIndex);

        int16_t boxWidth = widgets[WIDX_RESEARCH_ORDER_SCROLL].width();
        int32_t itemY = -SCROLLABLE_ROW_HEIGHT;
        auto* dragItem = WindowEditorInventionsListDragGetItem();

        const auto& researchList = scrollIndex == 0 ? gResearchItemsInvented : gResearchItemsUninvented;
        for (const auto& researchItem : researchList)
        {
            itemY += SCROLLABLE_ROW_HEIGHT;
            if (itemY + SCROLLABLE_ROW_HEIGHT < dpi.y || itemY >= dpi.y + dpi.height)
                continue;

            if (_selectedResearchItem == &researchItem)
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

                GfxFilterRect(&dpi, { 0, top, boxWidth, bottom }, FilterPaletteID::PaletteDarken1);
            }

            if (dragItem != nullptr && researchItem == *dragItem)
                continue;

            // TODO: this parameter by itself produces very light text.
            // It needs a {BLACK} token in the string to work properly.
            colour_t colour = COLOUR_BLACK;
            FontStyle fontStyle = FontStyle::Medium;
            auto darkness = TextDarkness::Regular;

            if (researchItem.IsAlwaysResearched())
            {
                if (_selectedResearchItem == &researchItem && dragItem == nullptr)
                    darkness = TextDarkness::ExtraDark;
                else
                    darkness = TextDarkness::Dark;
                colour = colours[1] | COLOUR_FLAG_INSET;
            }

            DrawResearchItem(dpi, researchItem, boxWidth, { 1, itemY }, STR_BLACK_STRING, { colour, fontStyle, darkness });
        }
    }

    CursorID OnCursor(WidgetIndex widx, const ScreenCoordsXY& screenCoords, CursorID fallback) override
    {
        bool isInvented = false;

        switch (widx)
        {
            case WIDX_PRE_RESEARCHED_SCROLL:
                isInvented = true;
                break;
            case WIDX_RESEARCH_ORDER_SCROLL:
                isInvented = false;
                break;
            default:
                return fallback;
        }

        // Use the open hand as cursor for items that can be picked up
        auto* researchItem = GetItemFromScrollY(isInvented, screenCoords.y);
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
        GfxDrawSprite(&dpi, ImageId(SPR_TAB_FINANCES_RESEARCH_0 + (frame_no / 2) % 8), screenPos);

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
        GfxFillRect(
            &dpi,
            { windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 },
              windowPos + ScreenCoordsXY{ bkWidget.right - 1, bkWidget.bottom - 1 } },
            ColourMapA[colours[1]].darkest);

        auto* researchItem = WindowEditorInventionsListDragGetItem();
        if (researchItem == nullptr || researchItem->IsNull())
            researchItem = _selectedResearchItem;
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
            if (ClipDrawPixelInfo(&clipDPI, &dpi, screenPos, clipWidth, clipHeight))
            {
                object->DrawPreview(&clipDPI, clipWidth, clipHeight);
            }
        }

        // Item name
        screenPos = windowPos + ScreenCoordsXY{ bkWidget.midX() + 1, bkWidget.bottom + 3 };
        const auto itemWidth = width - widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;

        StringId drawString = STR_WINDOW_COLOUR_2_STRINGID;
        StringId stringId = researchItem->GetName();
        auto ft = Formatter();

        if (researchItem->type == Research::EntryType::Ride
            && !GetRideTypeDescriptor(researchItem->baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
        {
            drawString = STR_WINDOW_COLOUR_2_STRINGID_STRINGID;
            StringId rideTypeName = get_ride_naming(researchItem->baseRideType, *get_ride_entry(researchItem->entryIndex)).Name;
            ft.Add<StringId>(rideTypeName);
            ft.Add<StringId>(stringId);
        }
        else
        {
            ft.Add<StringId>(stringId);
        }

        DrawTextEllipsised(&dpi, screenPos, itemWidth, drawString, ft, { TextAlignment::CENTRE });
        screenPos.y += 15;

        // Item category
        screenPos.x = windowPos.x + widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
        ft = Formatter();
        ft.Add<StringId>(researchItem->GetCategoryInventionString());
        DrawTextBasic(&dpi, screenPos, STR_INVENTION_RESEARCH_GROUP, ft);
    }

    void OnPrepareDraw() override
    {
        pressed_widgets |= 1uLL << WIDX_PREVIEW;
        pressed_widgets |= 1uLL << WIDX_TAB_1;

        widgets[WIDX_CLOSE].type = gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR ? WindowWidgetType::Empty
                                                                               : WindowWidgetType::CloseBox;

        ResizeFrameWithPage();

        int16_t scrollListHeight = (height - 88) / 2;

        widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom = 60 + scrollListHeight;
        widgets[WIDX_PRE_RESEARCHED_SCROLL].right = width - 229;

        widgets[WIDX_RESEARCH_ORDER_SCROLL].top = widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom + 15;
        widgets[WIDX_RESEARCH_ORDER_SCROLL].bottom = widgets[WIDX_RESEARCH_ORDER_SCROLL].top + scrollListHeight;
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
    std::optional<InventionListItem> GetResearchItemAt(const ScreenCoordsXY& screenCoords)
    {
        if (windowPos.x <= screenCoords.x && windowPos.y < screenCoords.y && windowPos.x + width > screenCoords.x
            && windowPos.y + height > screenCoords.y)
        {
            WidgetIndex widgetIndex = WindowFindWidgetFromPoint(*this, screenCoords);
            auto& widget = widgets[widgetIndex];
            if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL)
            {
                gPressedWidget.widget_index = widgetIndex;
                int32_t outScrollArea{};
                ScreenCoordsXY outScrollCoords{};
                int32_t outScrollId{};
                WidgetScrollGetPart(*this, &widget, screenCoords, outScrollCoords, &outScrollArea, &outScrollId);
                if (outScrollArea == SCROLL_PART_VIEW)
                {
                    const auto isInvented = outScrollId == 0;
                    int32_t scrollY = outScrollCoords.y + 6;
                    return InventionListItem{ GetItemFromScrollYIncludeSeps(isInvented, scrollY), isInvented };
                }
            }
        }

        return std::nullopt;
    }

    bool IsResearchItemSelected(ResearchItem* item) const
    {
        return item == _selectedResearchItem;
    }

    // hack to fix #17544: OnScrollMouseOver never gets called while dragging
    void SetSelectedResearchItem(ResearchItem* item)
    {
        _selectedResearchItem = item;
    }

    void MoveResearchItem(const ResearchItem& item, ResearchItem* beforeItem, bool isInvented)
    {
        _selectedResearchItem = nullptr;
        Invalidate();

        uint32_t beforeItemRawValue = 0;
        if (beforeItem != nullptr)
            beforeItemRawValue = beforeItem->rawValue;

        if (item.rawValue == beforeItemRawValue)
            return;

        ResearchRemove(item);

        auto& researchList = isInvented ? gResearchItemsInvented : gResearchItemsUninvented;
        if (beforeItem != nullptr)
        {
            for (size_t i = 0; i < researchList.size(); i++)
            {
                if (researchList[i].rawValue == beforeItemRawValue)
                {
                    researchList.insert((researchList.begin() + i), item);
                    return;
                }
            }
        }

        // Still not found? Append to end of list.
        researchList.push_back(item);
    }

private:
    ResearchItem* GetItemFromScrollY(bool isInvented, int32_t y) const
    {
        auto& researchList = isInvented ? gResearchItemsInvented : gResearchItemsUninvented;
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

    ResearchItem* GetItemFromScrollYIncludeSeps(bool isInvented, int32_t y) const
    {
        auto& researchList = isInvented ? gResearchItemsInvented : gResearchItemsUninvented;
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
        WindowClass::EditorInventionList, WW, WH, WF_NO_SCROLLING | WF_RESIZABLE | WF_CENTRE_SCREEN);
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

    CursorID OnCursor(const WidgetIndex widx, const ScreenCoordsXY& screenCoords, const CursorID defaultCursor) override
    {
        auto* inventionListWindow = static_cast<InventionListWindow*>(WindowFindByClass(WindowClass::EditorInventionList));
        if (inventionListWindow != nullptr)
        {
            auto res = inventionListWindow->GetResearchItemAt(screenCoords);
            auto* research = res.has_value() ? res->research : nullptr;
            if (!inventionListWindow->IsResearchItemSelected(research))
            {
                inventionListWindow->SetSelectedResearchItem(research);
                inventionListWindow->Invalidate();
            }
        }

        return CursorID::HandClosed;
    }

    void OnMoved(const ScreenCoordsXY& screenCoords) override
    {
        auto* inventionListWindow = static_cast<InventionListWindow*>(WindowFindByClass(WindowClass::EditorInventionList));
        if (inventionListWindow == nullptr)
        {
            Close();
        }
        std::optional<InventionListItem> res;
        // Skip always researched items, so that the dragged item gets placed underneath them
        auto newScreenCoords = screenCoords;
        do
        {
            res = inventionListWindow->GetResearchItemAt(newScreenCoords);
            newScreenCoords.y += LIST_ROW_HEIGHT;
        } while (res.has_value() && res->research != nullptr && res->research->IsAlwaysResearched());

        if (res.has_value())
        {
            inventionListWindow->MoveResearchItem(_draggedItem, res->research, res->isInvented);
        }

        WindowInvalidateByClass(WindowClass::EditorInventionList);
        Close();
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        auto screenCoords = windowPos + ScreenCoordsXY{ 0, 2 };

        DrawResearchItem(
            dpi, _draggedItem, width, screenCoords, STR_WINDOW_COLOUR_2_STRINGID,
            { COLOUR_BLACK | static_cast<uint8_t>(COLOUR_FLAG_OUTLINE) });
    }

    void Init(ResearchItem& researchItem, const ScreenCoordsXY& editorPos, int objectSelectionScrollWidth)
    {
        _draggedItem = researchItem;

        widgets[0].right = objectSelectionScrollWidth;
        Invalidate();
        windowPos = ScreenCoordsXY{ editorPos.x, gTooltipCursor.y - 7 };
        width = objectSelectionScrollWidth;
        Invalidate();

        InputWindowPositionBegin(*this, 0, gTooltipCursor);
    }

    const ResearchItem& GetItem() const
    {
        return _draggedItem;
    }
};

static void WindowEditorInventionsListDragOpen(
    ResearchItem* researchItem, const ScreenCoordsXY& editorPos, int objectSelectionScrollWidth)
{
    WindowCloseByClass(WindowClass::EditorInventionListDrag);
    auto* wnd = WindowCreate<InventionDragWindow>(
        WindowClass::EditorInventionListDrag, 10, 14, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING);
    if (wnd != nullptr)
    {
        wnd->Init(*researchItem, editorPos, objectSelectionScrollWidth);
    }
}

static const ResearchItem* WindowEditorInventionsListDragGetItem()
{
    auto* wnd = static_cast<InventionDragWindow*>(WindowFindByClass(WindowClass::EditorInventionListDrag));
    if (wnd == nullptr)
    {
        return nullptr;
    }
    return &wnd->GetItem();
}

#pragma endregion
