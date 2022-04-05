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
    WIDX_RESEARCH_NEVER_SCROLL,
    WIDX_PREVIEW,
    WIDX_MOVE_ITEMS_TO_TOP,
    WIDX_MOVE_ITEMS_TO_BOTTOM,
    WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP,
    WIDX_RANDOM_SHUFFLE
};

static rct_widget window_editor_inventions_list_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({  0,  43}, {600, 372}, WindowWidgetType::Resize,  WindowColour::Secondary                                             ),
    MakeTab   ({  3,  17}                                                                                               ),
    MakeWidget({  4,  56}, {368, 161}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                            ),
    MakeWidget({  4, 231}, {368, 157}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                            ),
    MakeWidget({  375, 56}, {220, 100}, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                            ),
    MakeWidget({431, 166}, {114, 114}, WindowWidgetType::FlatBtn, WindowColour::Secondary                                             ),
    MakeWidget({375, 343}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MOVE_ALL_TOP                           ),
    MakeWidget({375, 358}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_MOVE_ALL_BOTTOM                        ),
    MakeWidget({375, 373}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_INVENTION_NEVER_INVENTED_ITEMS_BUTTON                        ),
    MakeWidget({375, 388}, {220,  14}, WindowWidgetType::Button,  WindowColour::Secondary, STR_RANDOM_SHUFFLE,  STR_RANDOM_SHUFFLE_TIP),
    WIDGETS_END,
};

static rct_widget window_editor_inventions_list_drag_widgets[] = {
    MakeWidget({0, 0}, {150, 14}, WindowWidgetType::ImgBtn, WindowColour::Primary),
    WIDGETS_END,
};

#pragma endregion

#pragma region Events

static void WindowEditorInventionsListClose(rct_window *w);
static void WindowEditorInventionsListMouseup(rct_window *w, rct_widgetindex widgetIndex);
static void WindowEditorInventionsListResize(rct_window *w);
static void WindowEditorInventionsListUpdate(rct_window *w);
static void WindowEditorInventionsListScrollgetheight(rct_window *w, int32_t scrollIndex, int32_t *width, int32_t *height);
static void WindowEditorInventionsListScrollmousedown(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorInventionsListScrollmouseover(rct_window *w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords);
static void WindowEditorInventionsListCursor(rct_window *w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID *cursorId);
static void WindowEditorInventionsListInvalidate(rct_window *w);
static void WindowEditorInventionsListPaint(rct_window *w, rct_drawpixelinfo *dpi);
static void WindowEditorInventionsListScrollpaint(rct_window *w, rct_drawpixelinfo *dpi, int32_t scrollIndex);

static void WindowEditorInventionsListDragCursor(rct_window *w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID *cursorId);
static void WindowEditorInventionsListDragMoved(rct_window* w, const ScreenCoordsXY& screenCoords);
static void WindowEditorInventionsListDragPaint(rct_window *w, rct_drawpixelinfo *dpi);

static std::pair<rct_string_id, Formatter> WindowEditorInventionsListPrepareName(const ResearchItem * researchItem, bool withGap);

// 0x0098177C
static rct_window_event_list window_editor_inventions_list_events([](auto& events)
{
    events.close = &WindowEditorInventionsListClose;
    events.mouse_up = &WindowEditorInventionsListMouseup;
    events.resize = &WindowEditorInventionsListResize;
    events.update = &WindowEditorInventionsListUpdate;
    events.get_scroll_size = &WindowEditorInventionsListScrollgetheight;
    events.scroll_mousedown = &WindowEditorInventionsListScrollmousedown;
    events.scroll_mouseover = &WindowEditorInventionsListScrollmouseover;
    events.cursor = &WindowEditorInventionsListCursor;
    events.invalidate = &WindowEditorInventionsListInvalidate;
    events.paint = &WindowEditorInventionsListPaint;
    events.scroll_paint = &WindowEditorInventionsListScrollpaint;
});

// 0x009817EC
static rct_window_event_list window_editor_inventions_list_drag_events([](auto& events)
{
    events.cursor = &WindowEditorInventionsListDragCursor;
    events.moved = &WindowEditorInventionsListDragMoved;
    events.paint = &WindowEditorInventionsListDragPaint;
});

#pragma endregion

static ResearchItem _editorInventionsListDraggedItem;

// clang-format on

static void WindowEditorInventionsListDragOpen(ResearchItem* researchItem);
static void MoveResearchItem(ResearchItem* beforeItem, int32_t scrollIndexFrom, int32_t scrollIndexTo);

/**
 *
 *  rct2: 0x0068596F
 * Sets rides that are in use to be always researched
 */
static void ResearchRidesSetup()
{
    // Reset all objects to not required
    for (ObjectType objectType = ObjectType::Ride; objectType < ObjectType::Count; objectType++)
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
static void MoveResearchItem(ResearchItem* beforeItem, int32_t scrollIndex)
{
    auto w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
    if (w != nullptr)
    {
        w->research_item = nullptr;
        w->Invalidate();
    }

    ResearchRemove(_editorInventionsListDraggedItem);

    auto& researchList = scrollIndex == 0 ? gResearchItemsInvented
        : scrollIndex == 1                ? gResearchItemsUninvented
                                          : gResearchItemsNeverInvented;
    if (beforeItem != nullptr)
    {
        for (size_t i = 0; i < researchList.size(); i++)
        {
            if (researchList[i] == *beforeItem)
            {
                researchList.insert((researchList.begin() + i), _editorInventionsListDraggedItem);
                return;
            }
        }
    }

    // Still not found? Append to end of list.
    researchList.push_back(_editorInventionsListDraggedItem);
}

/**
 *
 *  rct2: 0x0068558E
 */
static ResearchItem* WindowEditorInventionsListGetItemFromScrollY(int32_t scrollIndex, int32_t y)
{
    auto& researchList = scrollIndex == 0 ? gResearchItemsInvented
        : scrollIndex == 1                ? gResearchItemsUninvented
                                          : gResearchItemsNeverInvented;
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

/**
 *
 *  rct2: 0x006855BB
 */
static ResearchItem* WindowEditorInventionsListGetItemFromScrollYIncludeSeps(int32_t scrollIndex, int32_t y)
{
    auto& researchList = scrollIndex == 0 ? gResearchItemsInvented
        : scrollIndex == 1                ? gResearchItemsUninvented
                                          : gResearchItemsNeverInvented;
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

static ResearchItem* GetResearchItemAt(const ScreenCoordsXY& screenCoords, int32_t* outScrollId)
{
    rct_window* w = window_find_by_class(WC_EDITOR_INVENTION_LIST);
    if (w != nullptr && w->windowPos.x <= screenCoords.x && w->windowPos.y < screenCoords.y
        && w->windowPos.x + w->width > screenCoords.x && w->windowPos.y + w->height > screenCoords.y)
    {
        rct_widgetindex widgetIndex = window_find_widget_from_point(w, screenCoords);
        rct_widget* widget = &w->widgets[widgetIndex];
        if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL
            || widgetIndex == WIDX_RESEARCH_NEVER_SCROLL)
        {
            gPressedWidget.widget_index = widgetIndex;
            int32_t outScrollArea;
            ScreenCoordsXY outScrollCoords;
            WidgetScrollGetPart(w, widget, screenCoords, outScrollCoords, &outScrollArea, outScrollId);
            if (outScrollArea == SCROLL_PART_VIEW)
            {
                *outScrollId = widgetIndex == WIDX_PRE_RESEARCHED_SCROLL ? 0
                    : widgetIndex == WIDX_RESEARCH_ORDER_SCROLL          ? 1
                                                                         : 2;

                int32_t scrollY = outScrollCoords.y + 6;
                return WindowEditorInventionsListGetItemFromScrollYIncludeSeps(*outScrollId, scrollY);
            }
        }
    }

    *outScrollId = -1;
    return nullptr;
}

/**
 *
 *  rct2: 0x00684E04
 */
rct_window* WindowEditorInventionsListOpen()
{
    rct_window* w;

    w = window_bring_to_front_by_class(WC_EDITOR_INVENTION_LIST);
    if (w != nullptr)
        return w;

    ResearchRidesSetup();

    w = WindowCreateCentred(
        WW, WH, &window_editor_inventions_list_events, WC_EDITOR_INVENTION_LIST, WF_NO_SCROLLING | WF_RESIZABLE);
    w->widgets = window_editor_inventions_list_widgets;
    WindowInitScrollWidgets(w);
    w->selected_tab = 0;
    w->research_item = nullptr;
    _editorInventionsListDraggedItem.SetNull();

    w->min_width = WW;
    w->min_height = WH;
    w->max_width = WW * 2;
    w->max_height = WH * 2;

    return w;
}

/**
 *
 *  rct2: 0x006853D2
 */
static void WindowEditorInventionsListClose(rct_window* w)
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

/**
 *
 *  rct2: 0x0068521B
 */
static void WindowEditorInventionsListMouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_RANDOM_SHUFFLE:
            research_items_shuffle();
            w->Invalidate();
            break;
        case WIDX_MOVE_ITEMS_TO_TOP:
            research_items_make_all_researched();
            WindowInitScrollWidgets(w);
            w->Invalidate();
            break;
        case WIDX_MOVE_ITEMS_TO_BOTTOM:
            research_items_make_all_unresearched();
            WindowInitScrollWidgets(w);
            w->Invalidate();
            break;
        case WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP:
            research_items_unlock_unavailable();
            WindowInitScrollWidgets(w);
            w->Invalidate();
            break;
    }
}

static void WindowEditorInventionsListResize(rct_window* w)
{
    if (w->width < w->min_width)
    {
        w->Invalidate();
        w->width = w->min_width;
    }
    if (w->height < w->min_height)
    {
        w->Invalidate();
        w->height = w->min_height;
    }
}

/**
 *
 *  rct2: 0x00685392
 */
static void WindowEditorInventionsListUpdate(rct_window* w)
{
    w->frame_no++;
    window_event_invalidate_call(w);
    widget_invalidate(w, WIDX_TAB_1);

    if (_editorInventionsListDraggedItem.IsNull())
        return;

    if (window_find_by_class(WC_EDITOR_INVENTION_LIST_DRAG) != nullptr)
        return;

    _editorInventionsListDraggedItem.SetNull();
    w->Invalidate();
}

/**
 *
 *  rct2: 0x00685239
 */
static void WindowEditorInventionsListScrollgetheight(rct_window* w, int32_t scrollIndex, int32_t* width, int32_t* height)
{
    *height = 0;
    if (scrollIndex == 0)
    {
        *height += static_cast<int32_t>(gResearchItemsInvented.size()) * SCROLLABLE_ROW_HEIGHT;
    }
    else if (scrollIndex == 1)
    {
        *height += static_cast<int32_t>(gResearchItemsUninvented.size()) * SCROLLABLE_ROW_HEIGHT;
    }
    else
    {
        *height += static_cast<int32_t>(gResearchItemsNeverInvented.size()) * SCROLLABLE_ROW_HEIGHT;
    }
}

/**
 *
 *  rct2: 0x006852D4
 */
static void WindowEditorInventionsListScrollmousedown(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    ResearchItem* researchItem;

    researchItem = WindowEditorInventionsListGetItemFromScrollY(scrollIndex, screenCoords.y);
    if (researchItem == nullptr)
        return;

    // Disallow picking up always-researched items
    if (researchItem->IsAlwaysResearched())
        return;

    w->Invalidate();
    WindowEditorInventionsListDragOpen(researchItem);
}

/**
 *
 *  rct2: 0x00685275
 */
static void WindowEditorInventionsListScrollmouseover(rct_window* w, int32_t scrollIndex, const ScreenCoordsXY& screenCoords)
{
    ResearchItem* researchItem;

    researchItem = WindowEditorInventionsListGetItemFromScrollY(scrollIndex, screenCoords.y);
    if (researchItem != w->research_item)
    {
        w->research_item = researchItem;
        w->Invalidate();

        // Prevent always-researched items from being highlighted when hovered over
        if (researchItem != nullptr && researchItem->IsAlwaysResearched())
        {
            w->research_item = nullptr;
        }
    }
}

/**
 *
 *  rct2: 0x00685291
 */
static void WindowEditorInventionsListCursor(
    rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID* cursorId)
{
    ResearchItem* researchItem;
    int32_t scrollIndex;

    switch (widgetIndex)
    {
        case WIDX_PRE_RESEARCHED_SCROLL:
            scrollIndex = 0;
            break;
        case WIDX_RESEARCH_ORDER_SCROLL:
            scrollIndex = 1;
            break;
        case WIDX_RESEARCH_NEVER_SCROLL:
            scrollIndex = 2;
            break;
        default:
            return;
    }

    // Use the open hand as cursor for items that can be picked up
    researchItem = WindowEditorInventionsListGetItemFromScrollY(scrollIndex, screenCoords.y);
    if (researchItem != nullptr && !researchItem->IsAlwaysResearched())
    {
        *cursorId = CursorID::HandOpen;
    }
}

/**
 *
 *  rct2: 0x00685392
 */
static void WindowEditorInventionsListInvalidate(rct_window* w)
{
    w->pressed_widgets |= 1ULL << WIDX_PREVIEW;
    w->pressed_widgets |= 1ULL << WIDX_TAB_1;

    w->widgets[WIDX_CLOSE].type = gScreenFlags & SCREEN_FLAGS_SCENARIO_EDITOR ? WindowWidgetType::Empty
                                                                              : WindowWidgetType::CloseBox;

    w->widgets[WIDX_BACKGROUND].right = w->width - 1;
    w->widgets[WIDX_BACKGROUND].bottom = w->height - 1;
    w->widgets[WIDX_TITLE].right = w->width - 2;
    w->widgets[WIDX_CLOSE].left = w->width - 13;
    w->widgets[WIDX_CLOSE].right = w->width - 3;
    w->widgets[WIDX_RESIZE].right = w->width - 1;
    w->widgets[WIDX_RESIZE].bottom = w->height - 1;

    int16_t scroll_list_height = (w->height - 88) / 2;

    w->widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom = 60 + scroll_list_height;
    w->widgets[WIDX_PRE_RESEARCHED_SCROLL].right = w->width - 229;

    w->widgets[WIDX_RESEARCH_ORDER_SCROLL].top = w->widgets[WIDX_PRE_RESEARCHED_SCROLL].bottom + 15;
    w->widgets[WIDX_RESEARCH_ORDER_SCROLL].bottom = w->widgets[WIDX_RESEARCH_ORDER_SCROLL].top + scroll_list_height;
    w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right = w->width - 229;

    w->widgets[WIDX_PREVIEW].left = w->width - 169;
    w->widgets[WIDX_PREVIEW].right = w->width - 56;
    w->widgets[WIDX_PREVIEW].top = w->height - 234;
    w->widgets[WIDX_PREVIEW].bottom = w->height - 120;

    w->widgets[WIDX_MOVE_ITEMS_TO_TOP].top = w->height - 72;
    w->widgets[WIDX_MOVE_ITEMS_TO_TOP].bottom = w->height - 59;
    w->widgets[WIDX_MOVE_ITEMS_TO_TOP].left = w->width - 225;
    w->widgets[WIDX_MOVE_ITEMS_TO_TOP].right = w->width - 6;

    w->widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].top = w->height - 57;
    w->widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].bottom = w->height - 44;
    w->widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].left = w->width - 225;
    w->widgets[WIDX_MOVE_ITEMS_TO_BOTTOM].right = w->width - 6;

    w->widgets[WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP].top = w->height - 42;
    w->widgets[WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP].bottom = w->height - 29;
    w->widgets[WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP].left = w->width - 225;
    w->widgets[WIDX_MOVE_ITEMS_FROM_NEVER_TO_TOP].right = w->width - 6;

    w->widgets[WIDX_RANDOM_SHUFFLE].top = w->height - 27;
    w->widgets[WIDX_RANDOM_SHUFFLE].bottom = w->height - 14;
    w->widgets[WIDX_RANDOM_SHUFFLE].left = w->width - 225;
    w->widgets[WIDX_RANDOM_SHUFFLE].right = w->width - 6;

    w->widgets[WIDX_RESEARCH_NEVER_SCROLL].left = w->width - 225;
    w->widgets[WIDX_RESEARCH_NEVER_SCROLL].bottom = w->widgets[WIDX_PREVIEW].top - 10;
    w->widgets[WIDX_RESEARCH_NEVER_SCROLL].right = w->width - 6;
}

/**
 *
 *  rct2: 0x00684EE0
 */
static void WindowEditorInventionsListPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    rct_widget* widget;
    ResearchItem* researchItem;
    int32_t width;

    WindowDrawWidgets(w, dpi);

    // Tab image
    auto screenPos = w->windowPos + ScreenCoordsXY{ w->widgets[WIDX_TAB_1].left, w->widgets[WIDX_TAB_1].top };
    gfx_draw_sprite(dpi, ImageId(SPR_TAB_FINANCES_RESEARCH_0 + (w->frame_no / 2) % 8), screenPos);

    // Pre-researched items label
    screenPos = w->windowPos
        + ScreenCoordsXY{ w->widgets[WIDX_PRE_RESEARCHED_SCROLL].left, w->widgets[WIDX_PRE_RESEARCHED_SCROLL].top - 11 };
    DrawTextBasic(dpi, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_PREINVENTED_ITEMS);

    // Research order label
    screenPos = w->windowPos
        + ScreenCoordsXY{ w->widgets[WIDX_RESEARCH_ORDER_SCROLL].left, w->widgets[WIDX_RESEARCH_ORDER_SCROLL].top - 11 };
    DrawTextBasic(dpi, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_TO_BE_INVENTED_ITEMS);

    // Never-researched items label
    screenPos = w->windowPos
        + ScreenCoordsXY{ w->widgets[WIDX_RESEARCH_NEVER_SCROLL].left, w->widgets[WIDX_RESEARCH_NEVER_SCROLL].top - 11 };
    DrawTextBasic(dpi, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_NEVER_INVENTED_ITEMS);

    // Preview background
    widget = &w->widgets[WIDX_PREVIEW];
    gfx_fill_rect(
        dpi,
        { w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 },
          w->windowPos + ScreenCoordsXY{ widget->right - 1, widget->bottom - 1 } },
        ColourMapA[w->colours[1]].darkest);

    researchItem = &_editorInventionsListDraggedItem;
    if (researchItem->IsNull())
        researchItem = w->research_item;
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
    widget = &w->widgets[WIDX_PREVIEW];

    const auto* object = object_entry_get_object(objectEntryType, researchItem->entryIndex);
    if (object != nullptr)
    {
        rct_drawpixelinfo clipDPI;
        screenPos = w->windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 };
        width = widget->width() - 1;
        int32_t height = widget->height() - 1;
        if (clip_drawpixelinfo(&clipDPI, dpi, screenPos, width, height))
        {
            object->DrawPreview(&clipDPI, width, height);
        }
    }

    // Item name
    screenPos = w->windowPos + ScreenCoordsXY{ widget->midX() + 1, widget->bottom + 3 };
    width = w->width - w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;

    auto [drawString, ft] = WindowEditorInventionsListPrepareName(researchItem, false);
    DrawTextEllipsised(dpi, screenPos, width, drawString, ft, { TextAlignment::CENTRE });
    screenPos.y += 15;

    // Item category
    screenPos.x = w->windowPos.x + w->widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
    ft = Formatter();
    ft.Add<rct_string_id>(researchItem->GetCategoryInventionString());
    DrawTextBasic(dpi, screenPos, STR_INVENTION_RESEARCH_GROUP, ft);
}

/**
 *
 *  rct2: 0x006850BD
 */
static void WindowEditorInventionsListScrollpaint(rct_window* w, rct_drawpixelinfo* dpi, int32_t scrollIndex)
{
    // Draw background
    uint8_t paletteIndex = ColourMapA[w->colours[1]].mid_light;
    gfx_clear(dpi, paletteIndex);

    int16_t boxWidth = w->widgets[WIDX_RESEARCH_ORDER_SCROLL].width();
    if (scrollIndex == 2)
        boxWidth = w->widgets[WIDX_RESEARCH_NEVER_SCROLL].width();
    int16_t columnSplitOffset = boxWidth / 2;
    int32_t itemY = -SCROLLABLE_ROW_HEIGHT;

    auto& researchList = scrollIndex == 0 ? gResearchItemsInvented
        : scrollIndex == 1                ? gResearchItemsUninvented
                                          : gResearchItemsNeverInvented;
    for (const auto& researchItem : researchList)
    {
        itemY += SCROLLABLE_ROW_HEIGHT;
        if (itemY + SCROLLABLE_ROW_HEIGHT < dpi->y || itemY >= dpi->y + dpi->height)
            continue;

        if (w->research_item == &researchItem)
        {
            int32_t top, bottom;
            if (_editorInventionsListDraggedItem.IsNull())
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

            gfx_filter_rect(dpi, { 0, top, boxWidth, bottom }, FilterPaletteID::PaletteDarken1);
        }

        if (researchItem == _editorInventionsListDraggedItem)
            continue;

        // TODO: this parameter by itself produces very light text.
        // It needs a {BLACK} token in the string to work properly.
        colour_t colour = COLOUR_BLACK;
        FontSpriteBase fontSpriteBase = FontSpriteBase::MEDIUM;

        if (researchItem.IsAlwaysResearched())
        {
            if (w->research_item == &researchItem && _editorInventionsListDraggedItem.IsNull())
                fontSpriteBase = FontSpriteBase::MEDIUM_EXTRA_DARK;
            else
                fontSpriteBase = FontSpriteBase::MEDIUM_DARK;
            colour = w->colours[1] | COLOUR_FLAG_INSET;
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
                dpi, { 1, itemY }, columnSplitOffset - 11, STR_INVENTIONS_LIST_RIDE_AND_VEHICLE_NAME, ft,
                { colour, fontSpriteBase });

            // Draw vehicle name
            ft = Formatter();
            ft.Add<rct_string_id>(itemNameId);
            DrawTextEllipsised(
                dpi, { columnSplitOffset + 1, itemY }, columnSplitOffset - 11, STR_BLACK_STRING, ft,
                { colour, fontSpriteBase });
        }
        else
        {
            // Scenery group, flat ride or shop
            auto ft = Formatter();
            ft.Add<rct_string_id>(itemNameId);
            DrawTextEllipsised(dpi, { 1, itemY }, boxWidth, STR_BLACK_STRING, ft, { colour, fontSpriteBase });
        }
    }
}

#pragma region Drag item

/**
 *
 *  rct2: 0x006852F4
 */
static void WindowEditorInventionsListDragOpen(ResearchItem* researchItem)
{
    char buffer[256], *ptr;
    window_close_by_class(WC_EDITOR_INVENTION_LIST_DRAG);
    _editorInventionsListDraggedItem = *researchItem;
    rct_string_id stringId = researchItem->GetName();

    ptr = buffer;
    if (researchItem->type == Research::EntryType::Ride
        && !GetRideTypeDescriptor(researchItem->baseRideType).HasFlag(RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY))
    {
        const auto rideEntry = get_ride_entry(researchItem->entryIndex);
        const rct_string_id rideTypeName = get_ride_naming(researchItem->baseRideType, rideEntry).Name;
        rct_string_id args[] = {
            rideTypeName,
            stringId,
        };
        format_string(ptr, 256, STR_INVENTIONS_LIST_RIDE_AND_VEHICLE_NAME, &args);
    }
    else
    {
        format_string(ptr, 256, stringId, nullptr);
    }

    auto stringWidth = gfx_get_string_width(buffer, FontSpriteBase::MEDIUM);
    window_editor_inventions_list_drag_widgets[0].right = stringWidth;

    auto* w = WindowCreate(
        gTooltipCursor - ScreenCoordsXY{ stringWidth / 2, 7 }, stringWidth, 14, &window_editor_inventions_list_drag_events,
        WC_EDITOR_INVENTION_LIST_DRAG, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING);
    w->widgets = window_editor_inventions_list_drag_widgets;
    w->colours[1] = COLOUR_WHITE;
    InputWindowPositionBegin(w, 0, gTooltipCursor);
}

/**
 *
 *  rct2: 0x0068549C
 */
static void WindowEditorInventionsListDragCursor(
    rct_window* w, rct_widgetindex widgetIndex, const ScreenCoordsXY& screenCoords, CursorID* cursorId)
{
    rct_window* inventionListWindow = window_find_by_class(WC_EDITOR_INVENTION_LIST);
    if (inventionListWindow != nullptr)
    {
        int32_t scrollId;
        ResearchItem* researchItem = GetResearchItemAt(screenCoords, &scrollId);
        if (researchItem != inventionListWindow->research_item)
        {
            inventionListWindow->Invalidate();
        }
    }

    *cursorId = CursorID::HandClosed;
}

/**
 *
 *  rct2: 0x00685412
 */
static void WindowEditorInventionsListDragMoved(rct_window* w, const ScreenCoordsXY& screenCoords)
{
    ResearchItem* researchItem;

    int32_t scrollId;
    // Skip always researched items, so that the dragged item gets placed underneath them
    auto newScreenCoords = screenCoords;
    do
    {
        researchItem = GetResearchItemAt(newScreenCoords, &scrollId);
        newScreenCoords.y += LIST_ROW_HEIGHT;
    } while (researchItem != nullptr && researchItem->IsAlwaysResearched());

    if (scrollId != -1)
    {
        MoveResearchItem(researchItem, scrollId);
    }

    window_close(w);
    _editorInventionsListDraggedItem.SetNull();
    window_invalidate_by_class(WC_EDITOR_INVENTION_LIST);
}

/**
 *
 *  rct2: 0x006853D9
 */
static void WindowEditorInventionsListDragPaint(rct_window* w, rct_drawpixelinfo* dpi)
{
    auto screenCoords = w->windowPos + ScreenCoordsXY{ 0, 2 };

    auto [drawString, ft] = WindowEditorInventionsListPrepareName(&_editorInventionsListDraggedItem, true);
    DrawTextBasic(dpi, screenCoords, drawString, ft, { COLOUR_BLACK | COLOUR_FLAG_OUTLINE });
}

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

#pragma endregion
