/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/interface/ColourWithFlags.h>
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

using namespace OpenRCT2::Drawing;

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
        const auto& gameState = getGameState();
        for (const auto& ride : RideManager(gameState))
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

        if (researchItem.type == Research::EntryType::ride
            && !GetRideTypeDescriptor(researchItem.baseRideType).HasFlag(RtdFlag::listVehiclesSeparately))
        {
            const StringId rideTypeName = GetRideNaming(researchItem.baseRideType, GetRideEntryByIndex(researchItem.entryIndex))
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
        void onOpen() override
        {
            ResearchRidesSetup();

            setWidgets(_inventionListWidgets);
            initScrollWidgets();
            selectedTab = 0;
            _selectedResearchItem = nullptr;

            WindowSetResize(*this, kWindowSize, { kWindowSize.width * 2, kWindowSize.height * 2 });
        }

        void onClose() override
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

        void onMouseUp(WidgetIndex widx) override
        {
            switch (widx)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_RANDOM_SHUFFLE:
                    ResearchItemsShuffle();
                    invalidate();
                    break;
                case WIDX_MOVE_ITEMS_TO_TOP:
                    ResearchItemsMakeAllResearched();
                    initScrollWidgets();
                    invalidate();
                    break;
                case WIDX_MOVE_ITEMS_TO_BOTTOM:
                    ResearchItemsMakeAllUnresearched();
                    initScrollWidgets();
                    invalidate();
                    break;
            }
        }

        void onResize() override
        {
            if (width < minWidth)
            {
                invalidate();
                width = minWidth;
            }
            if (height < minHeight)
            {
                invalidate();
                height = minHeight;
            }
        }

        void onUpdate() override
        {
            currentFrame++;
            onPrepareDraw();

            invalidateWidget(WIDX_TAB_1);

            if (WindowEditorInventionsListDragGetItem() != nullptr)
                return;
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
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

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto* researchItem = GetItemFromScrollY(scrollIndex == 0, screenCoords.y);
            if (researchItem != _selectedResearchItem)
            {
                _selectedResearchItem = researchItem;
                invalidate();

                // Prevent always-researched items from being highlighted when hovered over
                if (researchItem != nullptr && researchItem->IsAlwaysResearched())
                {
                    _selectedResearchItem = nullptr;
                }
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto* researchItem = GetItemFromScrollY(scrollIndex == 0, screenCoords.y);
            if (researchItem == nullptr)
                return;

            // Disallow picking up always-researched items
            if (researchItem->IsAlwaysResearched())
                return;

            invalidate();

            WindowEditorInventionsListDragOpen(researchItem, windowPos, widgets[WIDX_PRE_RESEARCHED_SCROLL].right);
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            const auto& gameState = getGameState();

            // Draw background
            auto paletteIndex = getColourMap(colours[1].colour).midLight;
            GfxClear(rt, paletteIndex);

            int16_t boxWidth = widgets[WIDX_RESEARCH_ORDER_SCROLL].width() - 1;
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

                    Rectangle::filter(rt, { 0, top, boxWidth, bottom }, FilterPaletteID::paletteDarken1);
                }

                if (dragItem != nullptr && researchItem == *dragItem)
                    continue;

                // TODO: this parameter by itself produces very light text.
                // It needs a {BLACK} token in the string to work properly.
                ColourWithFlags colour = { Drawing::Colour::black };
                FontStyle fontStyle = FontStyle::medium;
                auto darkness = TextDarkness::regular;

                if (researchItem.IsAlwaysResearched())
                {
                    if (_selectedResearchItem == &researchItem && dragItem == nullptr)
                        darkness = TextDarkness::extraDark;
                    else
                        darkness = TextDarkness::dark;
                    colour = colours[1].withFlag(ColourFlag::inset, true);
                }

                DrawResearchItem(rt, researchItem, boxWidth, { 1, itemY }, STR_BLACK_STRING, { colour, fontStyle, darkness });
            }
        }

        CursorID onCursor(WidgetIndex widx, const ScreenCoordsXY& screenCoords, CursorID fallback) override
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

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            // Tab image
            auto screenPos = windowPos + ScreenCoordsXY{ widgets[WIDX_TAB_1].left, widgets[WIDX_TAB_1].top };
            GfxDrawSprite(rt, ImageId(SPR_TAB_FINANCES_RESEARCH_0 + (currentFrame / 2) % 8), screenPos);

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
            Rectangle::fill(
                rt,
                { windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 },
                  windowPos + ScreenCoordsXY{ bkWidget.right - 1, bkWidget.bottom - 1 } },
                getColourMap(colours[1].colour).darkest);

            auto* researchItem = WindowEditorInventionsListDragGetItem();
            if (researchItem == nullptr || researchItem->IsNull())
                researchItem = _selectedResearchItem;
            // If the research item is null or a list separator.
            if (researchItem == nullptr || researchItem->IsNull())
                return;

            // Preview image
            ObjectType objectEntryType = ObjectType::sceneryGroup;
            if (researchItem->type == Research::EntryType::ride)
                objectEntryType = ObjectType::ride;

            auto chunk = ObjectEntryGetChunk(objectEntryType, researchItem->entryIndex);
            if (chunk == nullptr)
                return;

            // Draw preview
            const auto* object = ObjectEntryGetObject(objectEntryType, researchItem->entryIndex);
            if (object != nullptr)
            {
                RenderTarget clipRT;
                screenPos = windowPos + ScreenCoordsXY{ bkWidget.left + 1, bkWidget.top + 1 };
                const auto clipWidth = bkWidget.width() - 2;
                const auto clipHeight = bkWidget.height() - 2;
                if (ClipRenderTarget(clipRT, rt, screenPos, clipWidth, clipHeight))
                {
                    object->DrawPreview(clipRT, clipWidth, clipHeight);
                }
            }

            // Item name
            screenPos = windowPos + ScreenCoordsXY{ bkWidget.midX() + 1, bkWidget.bottom + 3 };
            const auto itemWidth = width - widgets[WIDX_RESEARCH_ORDER_SCROLL].right - 6;

            StringId drawString = STR_WINDOW_COLOUR_2_STRINGID;
            StringId stringId = researchItem->GetName();
            auto ft = Formatter();

            if (researchItem->type == Research::EntryType::ride
                && !GetRideTypeDescriptor(researchItem->baseRideType).HasFlag(RtdFlag::listVehiclesSeparately))
            {
                drawString = STR_WINDOW_COLOUR_2_STRINGID_STRINGID;
                StringId rideTypeName = GetRideNaming(researchItem->baseRideType, GetRideEntryByIndex(researchItem->entryIndex))
                                            .Name;
                ft.Add<StringId>(rideTypeName);
                ft.Add<StringId>(stringId);
            }
            else
            {
                ft.Add<StringId>(stringId);
            }

            DrawTextEllipsised(rt, screenPos, itemWidth, drawString, ft, { TextAlignment::centre });
            screenPos.y += 15;

            // Item category
            screenPos.x = windowPos.x + widgets[WIDX_RESEARCH_ORDER_SCROLL].right + 4;
            ft = Formatter();
            ft.Add<StringId>(researchItem->GetCategoryInventionString());
            DrawTextBasic(rt, screenPos, STR_INVENTION_RESEARCH_GROUP, ft);
        }

        void onPrepareDraw() override
        {
            pressedWidgets |= 1uLL << WIDX_PREVIEW;
            pressedWidgets |= 1uLL << WIDX_TAB_1;

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
                    gPressedWidget.widgetIndex = widgetIndex;
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

        // hack to fix #17544: onScrollMouseOver never gets called while dragging
        void SetSelectedResearchItem(ResearchItem* item)
        {
            _selectedResearchItem = item;
        }

        void MoveResearchItem(const ResearchItem& item, ResearchItem* beforeItem, bool isInvented)
        {
            auto& gameState = getGameState();
            _selectedResearchItem = nullptr;
            invalidate();

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
            WindowClass::editorInventionList, kWindowSize,
            { WindowFlag::noScrolling, WindowFlag::resizable, WindowFlag::centreScreen });
    }
#pragma endregion

#pragma region Drag item

    class InventionDragWindow final : public Window
    {
        ResearchItem _draggedItem;

    public:
        void onOpen() override
        {
            setWidgets(_inventionListDragWidgets);
            colours[1] = Drawing::Colour::white;
        }

        CursorID onCursor(const WidgetIndex widx, const ScreenCoordsXY& screenCoords, const CursorID defaultCursor) override
        {
            auto* windowMgr = GetWindowManager();
            auto* inventionListWindow = static_cast<InventionListWindow*>(
                windowMgr->FindByClass(WindowClass::editorInventionList));
            if (inventionListWindow != nullptr)
            {
                auto res = inventionListWindow->GetResearchItemAt(screenCoords);
                auto* research = res.has_value() ? res->research : nullptr;
                if (!inventionListWindow->IsResearchItemSelected(research))
                {
                    inventionListWindow->SetSelectedResearchItem(research);
                    inventionListWindow->invalidate();
                }
            }

            return CursorID::HandClosed;
        }

        void onMoved(const ScreenCoordsXY& screenCoords) override
        {
            auto* windowMgr = GetWindowManager();
            auto* inventionListWindow = static_cast<InventionListWindow*>(
                windowMgr->FindByClass(WindowClass::editorInventionList));
            if (inventionListWindow == nullptr)
            {
                close();
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

            windowMgr->InvalidateByClass(WindowClass::editorInventionList);
            close();
        }

        void onDraw(RenderTarget& rt) override
        {
            auto screenCoords = windowPos + ScreenCoordsXY{ 0, 2 };

            DrawResearchItem(
                rt, _draggedItem, width, screenCoords, STR_WINDOW_COLOUR_2_STRINGID,
                { ColourWithFlags{ Drawing::Colour::black }.withFlag(ColourFlag::withOutline, true) });
        }

        void init(ResearchItem& researchItem, const ScreenCoordsXY& editorPos, int objectSelectionScrollWidth)
        {
            _draggedItem = researchItem;

            widgets[0].right = objectSelectionScrollWidth;
            invalidate();
            windowPos = ScreenCoordsXY{ editorPos.x, gTooltipCursor.y - 7 };
            width = objectSelectionScrollWidth;
            invalidate();

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
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::editorInventionListDrag);
        auto* wnd = windowMgr->Create<InventionDragWindow>(
            WindowClass::editorInventionListDrag, { 10, 14 },
            { WindowFlag::stickToFront, WindowFlag::transparent, WindowFlag::noSnapping });
        if (wnd != nullptr)
        {
            wnd->init(*researchItem, editorPos, objectSelectionScrollWidth);
        }
    }

    static const ResearchItem* WindowEditorInventionsListDragGetItem()
    {
        auto* windowMgr = GetWindowManager();
        auto* wnd = static_cast<InventionDragWindow*>(windowMgr->FindByClass(WindowClass::editorInventionListDrag));
        if (wnd == nullptr)
        {
            return nullptr;
        }
        return &wnd->GetItem();
    }

#pragma endregion
} // namespace OpenRCT2::Ui::Windows
