/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <iterator>
#include <openrct2-ui/input/MouseInput.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Editor.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/interface/Cursors.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/management/Research.h>
#include <openrct2/object/DefaultObjects.h>
#include <openrct2/object/ObjectList.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ObjectRepository.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/RideManager.hpp>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Scenery.h>

namespace OpenRCT2::Ui::Windows
{
#pragma region Widgets

    static constexpr ScreenSize kWindowSize = { 600, 400 };
    static constexpr StringId kWindowTitle = STR_INVENTION_LIST;

    enum
    {
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

    // clang-format off
    static constexpr auto _inventionListWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({  0,  43}, {600, 357}, WidgetType::resize,  WindowColour::secondary                                             ),
        makeTab   ({  3,  17}                                                                                                       ),
        makeWidget({  4,  56}, {368, 161}, WidgetType::scroll,  WindowColour::secondary, SCROLL_VERTICAL                            ),
        makeWidget({  4, 231}, {368, 157}, WidgetType::scroll,  WindowColour::secondary, SCROLL_VERTICAL                            ),
        makeWidget({431, 106}, {114, 114}, WidgetType::flatBtn, WindowColour::secondary                                             ),
        makeWidget({375, 343}, {220,  14}, WidgetType::button,  WindowColour::secondary, STR_MOVE_ALL_TOP                           ),
        makeWidget({375, 358}, {220,  14}, WidgetType::button,  WindowColour::secondary, STR_MOVE_ALL_BOTTOM                        ),
        makeWidget({375, 373}, {220,  14}, WidgetType::button,  WindowColour::secondary, STR_RANDOM_SHUFFLE,  STR_RANDOM_SHUFFLE_TIP)
    );

    static constexpr auto _inventionListDragWidgets = makeWidgets(
        makeWidget({0, 0}, {150, 14}, WidgetType::imgBtn, WindowColour::primary)
    );
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
        for (auto objectType : getTransientObjectTypes())
        {
            auto maxObjects = getObjectEntryGroupCount(objectType);
            for (auto i = 0u; i < maxObjects; i++)
            {
                Editor::ClearSelectedObject(objectType, i, ObjectSelectionFlags::AllFlags);
            }
        }

        // Set research required for rides in use
        for (const auto& ride : GetRideManager())
        {
            Editor::SetSelectedObject(
                ObjectType::ride, ride.subtype, ObjectSelectionFlags::Selected | ObjectSelectionFlags::InUse);
        }
    }

    static void DrawResearchItem(
        RenderTarget& rt, const ResearchItem& researchItem, const int16_t& width, const ScreenCoordsXY& screenCoords,
        StringId format, TextPaint textPaint)
    {
        const StringId itemNameId = researchItem.GetName();
        int16_t columnSplitOffset = width / 2;

        if (researchItem.type == Research::EntryType::Ride
            && !GetRideTypeDescriptor(researchItem.baseRideType).HasFlag(RtdFlag::listVehiclesSeparately))
        {
            const StringId rideTypeName = GetRideNaming(
                                              researchItem.baseRideType, *GetRideEntryByIndex(researchItem.entryIndex))
                                              .Name;

            // Draw group name
            auto ft = Formatter();
            ft.Add<StringId>(rideTypeName);
            DrawTextEllipsised(rt, screenCoords, columnSplitOffset - 11, format, ft, textPaint);

            // Draw vehicle name
            ft = Formatter();
            ft.Add<StringId>(itemNameId);
            DrawTextEllipsised(
                rt, { screenCoords + ScreenCoordsXY{ columnSplitOffset, 0 } }, columnSplitOffset - 11, format, ft, textPaint);
        }
        else
        {
            // Scenery group, flat ride or shopdis
            auto ft = Formatter();
            ft.Add<StringId>(itemNameId);
            DrawTextEllipsised(rt, screenCoords, width, format, ft, textPaint);
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

            SetWidgets(_inventionListWidgets);
            InitScrollWidgets();
            selected_tab = 0;
            _selectedResearchItem = nullptr;

            WindowSetResize(*this, kWindowSize, { kWindowSize.width * 2, kWindowSize.height * 2 });
        }

        void OnClose() override
        {
            ResearchRemoveFlags();

            // When used in-game (as a cheat)
            if (!isInEditorMode())
            {
                gSilentResearch = true;
                ResearchResetCurrentItem();
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
                    ResearchItemsShuffle();
                    Invalidate();
                    break;
                case WIDX_MOVE_ITEMS_TO_TOP:
                    ResearchItemsMakeAllResearched();
                    InitScrollWidgets();
                    Invalidate();
                    break;
                case WIDX_MOVE_ITEMS_TO_BOTTOM:
                    ResearchItemsMakeAllUnresearched();
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

            InvalidateWidget(WIDX_TAB_1);

            if (WindowEditorInventionsListDragGetItem() != nullptr)
                return;
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            const auto& gameState = getGameState();
            ScreenSize size{};
            if (scrollIndex == 0)
            {
                size.height = static_cast<int32_t>(gameState.researchItemsInvented.size()) * kScrollableRowHeight;
            }
            else
            {
                size.height = static_cast<int32_t>(gameState.researchItemsUninvented.size()) * kScrollableRowHeight;
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

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            const auto& gameState = getGameState();

            // Draw background
            uint8_t paletteIndex = ColourMapA[colours[1].colour].mid_light;
            GfxClear(rt, paletteIndex);

            int16_t boxWidth = widgets[WIDX_RESEARCH_ORDER_SCROLL].width();
            int32_t itemY = -kScrollableRowHeight;
            auto* dragItem = WindowEditorInventionsListDragGetItem();

            const auto& researchList = scrollIndex == 0 ? gameState.researchItemsInvented : gameState.researchItemsUninvented;
            for (const auto& researchItem : researchList)
            {
                itemY += kScrollableRowHeight;
                if (itemY + kScrollableRowHeight < rt.y || itemY >= rt.y + rt.height)
                    continue;

                if (_selectedResearchItem == &researchItem)
                {
                    int32_t top, bottom;
                    if (dragItem == nullptr)
                    {
                        // Highlight
                        top = itemY;
                        bottom = itemY + kScrollableRowHeight - 1;
                    }
                    else
                    {
                        // Drop horizontal rule
                        top = itemY - 1;
                        bottom = itemY;
                    }

                    GfxFilterRect(rt, { 0, top, boxWidth, bottom }, FilterPaletteID::PaletteDarken1);
                }

                if (dragItem != nullptr && researchItem == *dragItem)
                    continue;

                // TODO: this parameter by itself produces very light text.
                // It needs a {BLACK} token in the string to work properly.
                ColourWithFlags colour = { COLOUR_BLACK };
                FontStyle fontStyle = FontStyle::Medium;
                auto darkness = TextDarkness::Regular;

                if (researchItem.IsAlwaysResearched())
                {
                    if (_selectedResearchItem == &researchItem && dragItem == nullptr)
                        darkness = TextDarkness::ExtraDark;
                    else
                        darkness = TextDarkness::Dark;
                    colour = colours[1].withFlag(ColourFlag::inset, true);
                }

                DrawResearchItem(rt, researchItem, boxWidth, { 1, itemY }, STR_BLACK_STRING, { colour, fontStyle, darkness });
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

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);

            // Tab image
            auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top };
            GfxDrawSprite(rt, ImageId(SPR_TAB_FINANCES_RESEARCH_0 + (frame_no / 2) % 8), screenPos);

            // Pre-researched items label
            screenPos = windowPos
                + ScreenCoordsXY{ widgets[WIDX_PRE_RESEARCHED_SCROLL].left, widgets[WIDX_PRE_RESEARCHED_SCROLL].top - 11 };
            DrawTextBasic(rt, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_PREINVENTED_ITEMS);

            // Research order label
            screenPos = windowPos
                + ScreenCoordsXY{ widgets[WIDX_RESEARCH_ORDER_SCROLL].left, widgets[WIDX_RESEARCH_ORDER_SCROLL].top - 11 };
            DrawTextBasic(rt, screenPos - ScreenCoordsXY{ 0, 1 }, STR_INVENTION_TO_BE_INVENTED_ITEMS);

            // Preview background
            auto& bkWidget = widgets[WIDX_PREVIEW];
            GfxFillRect(
                rt,
                { windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ bkWidget.right - 1, bkWidget.bottom - 1 } },
                ColourMapA[colours[1].colour].darkest);

            auto* researchItem = WindowEditorInventionsListDragGetItem();
            if (researchItem == nullptr || researchItem->IsNull())
                researchItem = _selectedResearchItem;
            // If the research item is null or a list separator.
            if (researchItem == nullptr || researchItem->IsNull())
                return;

            // Preview image
            ObjectType objectEntryType = ObjectType::sceneryGroup;
            if (researchItem->type == Research::EntryType::Ride)
                objectEntryType = ObjectType::ride;

            auto chunk = ObjectEntryGetChunk(objectEntryType, researchItem->entryIndex);
            if (chunk == nullptr)
                return;

            // Draw preview
            const auto* object = ObjectEntryGetObject(objectEntryType, researchItem->entryIndex);
            if (object != nullptr)
            {
                RenderTarget clipDPI;
                screenPos = windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 };
                const auto clipWidth = bkWidget.width() - 1;
                const auto clipHeight = bkWidget.height() - 1;
                if (ClipDrawPixelInfo(clipDPI, rt, screenPos, clipWidth, clipHeight))
                {
                    object->DrawPreview(clipDPI, clipWidth, clipHeight);
                }
            }

            // Item name
            screenPos = windowPos + ScreenCoordsXY{ bkWidget.midX() + 1, bkWidget.bottom + 3 };
            const auto itemWidth = width - widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;

            StringId drawString = STR_WINDOW_COLOUR_2_STRINGID;
            StringId stringId = researchItem->GetName();
            auto ft = Formatter();

            if (researchItem->type == Research::EntryType::Ride
                && !GetRideTypeDescriptor(researchItem->baseRideType).HasFlag(RtdFlag::listVehiclesSeparately))
            {
                drawString = STR_WINDOW_COLOUR_2_STRINGID_STRINGID;
                StringId rideTypeName = GetRideNaming(
                                            researchItem->baseRideType, *GetRideEntryByIndex(researchItem->entryIndex))
                                            .Name;
                ft.Add<StringId>(rideTypeName);
                ft.Add<StringId>(stringId);
            }
            else
            {
                ft.Add<StringId>(stringId);
            }

            DrawTextEllipsised(rt, screenPos, itemWidth, drawString, ft, { TextAlignment::CENTRE });
            screenPos.y += 15;

            // Item category
            screenPos.x = windowPos.x + widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
            ft = Formatter();
            ft.Add<StringId>(researchItem->GetCategoryInventionString());
            DrawTextBasic(rt, screenPos, STR_INVENTION_RESEARCH_GROUP, ft);
        }

        void OnPrepareDraw() override
        {
            pressed_widgets |= 1uLL << WIDX_PREVIEW;
            pressed_widgets |= 1uLL << WIDX_TAB_1;

            widgets[WIDX_CLOSE].type = gLegacyScene == LegacyScene::scenarioEditor ? WidgetType::empty : WidgetType::closeBox;

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
                auto* windowMgr = GetWindowManager();
                WidgetIndex widgetIndex = windowMgr->FindWidgetFromPoint(*this, screenCoords);

                auto& widget = widgets[widgetIndex];
                if (widgetIndex == WIDX_PRE_RESEARCHED_SCROLL || widgetIndex == WIDX_RESEARCH_ORDER_SCROLL)
                {
                    gPressedWidget.widget_index = widgetIndex;
                    int32_t outScrollArea{};
                    ScreenCoordsXY outScrollCoords{};
                    int32_t outScrollId{};
                    widgetScrollGetPart(*this, &widget, screenCoords, outScrollCoords, &outScrollArea, &outScrollId);
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
            auto& gameState = getGameState();
            _selectedResearchItem = nullptr;
            Invalidate();

            uint32_t beforeItemRawValue = 0;
            if (beforeItem != nullptr)
                beforeItemRawValue = beforeItem->rawValue;

            if (item.rawValue == beforeItemRawValue)
                return;

            ResearchRemove(item);

            auto& researchList = isInvented ? gameState.researchItemsInvented : gameState.researchItemsUninvented;
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
            auto& gameState = getGameState();
            auto& researchList = isInvented ? gameState.researchItemsInvented : gameState.researchItemsUninvented;
            for (auto& researchItem : researchList)
            {
                y -= kScrollableRowHeight;
                if (y < 0)
                {
                    return &researchItem;
                }
            }

            return nullptr;
        }

        ResearchItem* GetItemFromScrollYIncludeSeps(bool isInvented, int32_t y) const
        {
            auto& gameState = getGameState();
            auto& researchList = isInvented ? gameState.researchItemsInvented : gameState.researchItemsUninvented;
            for (auto& researchItem : researchList)
            {
                y -= kScrollableRowHeight;
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
    WindowBase* EditorInventionsListOpen()
    {
        auto* windowMgr = GetWindowManager();
        return windowMgr->FocusOrCreate<InventionListWindow>(
            WindowClass::EditorInventionList, kWindowSize, WF_NO_SCROLLING | WF_RESIZABLE | WF_CENTRE_SCREEN);
    }
#pragma endregion

#pragma region Drag item

    class InventionDragWindow final : public Window
    {
        ResearchItem _draggedItem;

    public:
        void OnOpen() override
        {
            SetWidgets(_inventionListDragWidgets);
            colours[1] = COLOUR_WHITE;
        }

        CursorID OnCursor(const WidgetIndex widx, const ScreenCoordsXY& screenCoords, const CursorID defaultCursor) override
        {
            auto* windowMgr = GetWindowManager();
            auto* inventionListWindow = static_cast<InventionListWindow*>(
                windowMgr->FindByClass(WindowClass::EditorInventionList));
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
            auto* windowMgr = GetWindowManager();
            auto* inventionListWindow = static_cast<InventionListWindow*>(
                windowMgr->FindByClass(WindowClass::EditorInventionList));
            if (inventionListWindow == nullptr)
            {
                Close();
                return;
            }
            std::optional<InventionListItem> res;
            // Skip always researched items, so that the dragged item gets placed underneath them
            auto newScreenCoords = screenCoords;
            do
            {
                res = inventionListWindow->GetResearchItemAt(newScreenCoords);
                newScreenCoords.y += kListRowHeight;
            } while (res.has_value() && res->research != nullptr && res->research->IsAlwaysResearched());

            if (res.has_value())
            {
                inventionListWindow->MoveResearchItem(_draggedItem, res->research, res->isInvented);
            }

            windowMgr->InvalidateByClass(WindowClass::EditorInventionList);
            Close();
        }

        void OnDraw(RenderTarget& rt) override
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 0, 2 };

            DrawResearchItem(
                rt, _draggedItem, width, screenCoords, STR_WINDOW_COLOUR_2_STRINGID,
                { ColourWithFlags{ COLOUR_BLACK }.withFlag(ColourFlag::withOutline, true) });
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
        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::EditorInventionListDrag);
        auto* wnd = windowMgr->Create<InventionDragWindow>(
            WindowClass::EditorInventionListDrag, { 10, 14 }, WF_STICK_TO_FRONT | WF_TRANSPARENT | WF_NO_SNAPPING);
        if (wnd != nullptr)
        {
            wnd->Init(*researchItem, editorPos, objectSelectionScrollWidth);
        }
    }

    static const ResearchItem* WindowEditorInventionsListDragGetItem()
    {
        auto* windowMgr = GetWindowManager();
        auto* wnd = static_cast<InventionDragWindow*>(windowMgr->FindByClass(WindowClass::EditorInventionListDrag));
        if (wnd == nullptr)
        {
            return nullptr;
        }
        return &wnd->GetItem();
    }

#pragma endregion
} // namespace OpenRCT2::Ui::Windows
