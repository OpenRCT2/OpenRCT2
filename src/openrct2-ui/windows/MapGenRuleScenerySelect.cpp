/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/String.hpp>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/SmallSceneryObject.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/map_generator/rule/Rule.h>
#include <openrct2/world/tile_element/SmallSceneryElement.h>
#include <optional>
#include <ranges>
#include <unordered_map>
#include <vector>

using namespace OpenRCT2::World::MapGenerator::Rule;

namespace OpenRCT2::Ui::Windows
{
    class MapGenWindow;
    enum WindowMapGenRuleSceneryWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CANCEL,
        WIDX_OKAY,

        WIDX_FILTER,
        WIDX_FILTER_CLEAR,
        WIDX_FILTER_BY_SELECTION,

        WIDX_COLOUR_BTN_DUMMY,
        WIDX_SCROLL,
    };

    static constexpr ScreenSize kWindowSize = { 618, 400 };
    static constexpr ScreenSize kItemSize = { 66, 80 };
    static constexpr auto _widgets = makeWidgets(
        // clang-format off
        makeWindowShim(STR_MAPGEN_RULE_SCENERY_SELECT_WINDOW_TITLE, kWindowSize),
        makeWidget({ 504, 381 }, { 109,  14 }, WidgetType::button,    WindowColour::secondary, STR_CANCEL),
        makeWidget({   5, 381 }, { 109,  14 }, WidgetType::button,    WindowColour::secondary, STR_OK),
        makeWidget({   5,  19 }, { 380,  14 }, WidgetType::textBox,   WindowColour::secondary),
        makeWidget({ 390,  19 }, { 109,  14 }, WidgetType::button,    WindowColour::secondary, STR_CLEAR_BUTTON),
        makeWidget({ 504,  19 }, { 109,  14 }, WidgetType::button,    WindowColour::secondary, STR_SELECTED_ONLY),
        makeWidget({   0,   0 }, {   1,   1 }, WidgetType::colourBtn, WindowColour::secondary),
        makeWidget({   5,  38 }, { 608, 319 }, WidgetType::scroll,    WindowColour::secondary, SCROLL_VERTICAL)

        // clang-format on
    );

    class MapGenRuleScenerySelectWindow final : public Window
    {
    private:
        WidgetIdentifier callWidget = {};
        SceneryEffect effect = {};
        std::function<void(SceneryEffect&)> callback;

        u8string filter = "";
        bool filterSelectedOnly = false;

        std::vector<ObjectEntryIndex> availableItems;
        std::vector<ObjectEntryIndex> filteredItems;
        std::unordered_map<ObjectEntryIndex, SceneryEffectItem> selectedItems;

        std::optional<ObjectEntryIndex> highlightedItem = std::nullopt;
        std::optional<std::pair<ObjectEntryIndex, uint8_t>> colourBtnPressed = std::nullopt;
        std::optional<std::pair<ObjectEntryIndex, uint8_t>> weightBtnPressed = std::nullopt;

        bool HasParentWindow() const
        {
            return callWidget.window.classification != WindowClass::Null;
        }

        MapGenWindow* GetParentWindow() const
        {
            auto* windowMgr = GetWindowManager();
            if (HasParentWindow())
            {
                auto* parent = windowMgr->FindByNumber(callWidget.window.classification, callWidget.window.number);
                return reinterpret_cast<MapGenWindow*>(parent);
            }

            return nullptr;
        }

    public:
        void OnOpen() override
        {
            SetWidgets(_widgets);
            WindowSetResize(*this, kWindowSize, kWindowSize);
            WindowInitScrollWidgets(*this);

            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxSmallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectManager::GetObjectEntry<SmallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    availableItems.push_back(sceneryId);
                }
            }

            applyFilter();
        }

        void OnUpdate() override
        {
            if (HasParentWindow())
            {
                // If the calling window is closed then close this window
                auto parentWindow = GetParentWindow();
                if (parentWindow == nullptr)
                {
                    Close();
                    return;
                }
            }

            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                InvalidateWidget(WIDX_FILTER);
            }

            if (!widgetIsHighlighted(*this, WIDX_SCROLL))
            {
                highlightedItem = std::nullopt;
                Invalidate();
            }
        }

        bool matchesFilter(const ObjectEntryIndex& idx) const
        {
            auto& objManager = GetContext()->GetObjectManager();
            auto object = objManager.GetLoadedObject(ObjectType::smallScenery, idx);

            auto matchesSelectedOnlyFilter = !filterSelectedOnly || selectedItems.contains(idx);
            auto matchesTextFilter = String::contains(object->GetName(), filter, true)
                || std::ranges::any_of(object->GetAuthors(),
                                       [&](const std::string& a) { return String::contains(a, filter, true); })
                || String::contains(object->GetFileName(), filter, true)
                || String::contains(object->GetIdentifier(), filter, true);

            return matchesSelectedOnlyFilter && matchesTextFilter;
        }

        void applyFilter()
        {
            filteredItems.clear();
            for (ObjectEntryIndex& item : availableItems)
            {
                if (matchesFilter(item))
                {
                    filteredItems.push_back(item);
                }
            }
            Invalidate();
        }

        void OnPrepareDraw() override
        {
            widgets[WIDX_COLOUR_BTN_DUMMY].type = WidgetType::empty;
            widgets[WIDX_FILTER].string = filter.data();

            SetWidgetPressed(WIDX_FILTER_BY_SELECTION, filterSelectedOnly);
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);

            if (highlightedItem.has_value())
            {
                auto* obj = GetContext()->GetObjectManager().GetLoadedObject<SmallSceneryObject>(highlightedItem.value());
                Guard::Assert(obj != nullptr);

                auto coordsName = windowPos + ScreenCoordsXY{ 5, 365 };
                auto coordsId = windowPos + ScreenCoordsXY{ 613, 365 };
                auto labelWidth = (kWindowSize.width - 10) / 2;
                auto colour = colours[1];

                std::string name = obj->GetName();
                std::string identifier = std::string(obj->GetIdentifier());

                Formatter ftName;
                ftName.Add<char*>(name.c_str());
                DrawTextEllipsised(rt, coordsName, labelWidth, STR_STRING, ftName, { colour, TextAlignment::LEFT });

                Formatter ftIdentifier;
                ftIdentifier.Add<char*>(identifier.c_str());
                DrawTextEllipsised(rt, coordsId, labelWidth, STR_STRING, ftIdentifier, { colour, TextAlignment::RIGHT });
            }
        }

        int32_t GetNumColumns() const
        {
            return (widgets[WIDX_SCROLL].width() - kScrollBarWidth) / kItemSize.width;
        }

        int32_t GetNumRows() const
        {
            float items = static_cast<float>(filteredItems.size());
            float columns = static_cast<float>(GetNumColumns());
            return static_cast<int32_t>(std::ceil<float>(items / columns));
        }

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, GetNumRows() * kItemSize.height };
        }

        void SceneryDrawItem(RenderTarget& rt, const ObjectEntryIndex& idx)
        {
            std::array<colour_t, 3> colours = { COLOUR_BORDEAUX_RED, COLOUR_YELLOW, COLOUR_DARK_BROWN };
            uint8_t direction = 0;
            int32_t weight = 1;

            if (selectedItems.contains(idx))
            {
                auto& effectItem = selectedItems[idx];
                colours = effectItem.colours;
                direction = effectItem.direction.value_or(0);
                weight = effectItem.weight;
            }

            auto sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(idx);
            auto imageId = ImageId(sceneryEntry->image + direction);

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
            {
                imageId = imageId.WithPrimary(colours[0]);
            }
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
            {
                imageId = imageId.WithSecondary(colours[1]);
            }
            if (sceneryEntry->flags & SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR)
            {
                imageId = imageId.WithTertiary(colours[2]);
            }

            auto spriteTop = (kItemSize.height / 2) + (sceneryEntry->height / 2);
            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_FULL_TILE) && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_VOFFSET_CENTRE))
            {
                spriteTop -= 12;
            }

            if (weight == 0)
            {
                imageId = imageId.WithRemap(FilterPaletteID::PaletteGhost);
            }

            auto spritePosition = ScreenCoordsXY{ kItemSize.width / 2, spriteTop };

            GfxDrawSprite(rt, imageId, spritePosition);

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_GLASS))
            {
                imageId = ImageId(sceneryEntry->image + 4 + direction).WithTransparency(colours[0]);
                GfxDrawSprite(rt, imageId, spritePosition);
            }

            if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_ANIMATED_FG))
            {
                imageId = ImageId(sceneryEntry->image + 4 + direction);
                GfxDrawSprite(rt, imageId, spritePosition);
            }
        }

        ScreenRect itemRectToColourRect(const ScreenRect& itemRect, const uint8_t nthColour)
        {
            auto point1 = ScreenCoordsXY{ itemRect.GetRight() - 14, itemRect.GetTop() + 4 + (12 * nthColour) };
            auto point2 = point1 + ScreenCoordsXY{ 12, 12 };
            return { point1, point2 };
        }

        void drawColourButton(
            RenderTarget& rt, const ObjectEntryIndex objectIdx, const uint8_t nthColour, const ScreenRect& rect)
        {
            ImageId btnImage = getColourButtonImage(selectedItems[objectIdx].colours[nthColour]);
            if (colourBtnPressed.has_value() && colourBtnPressed.value().first == objectIdx
                && colourBtnPressed.value().second == nthColour)
            {
                btnImage = btnImage.WithIndexOffset(1);
            }
            auto colourBtnRect = itemRectToColourRect(rect, nthColour);
            GfxDrawSprite(rt, btnImage.WithBlended(false), colourBtnRect.Point1);
        }

        ScreenRect itemRectToWeightBtnRect(const ScreenRect& rect, const uint8_t nthButton)
        {
            auto point2 = rect.Point2 - ScreenCoordsXY{ 5, 5 } - ScreenCoordsXY{ 10 * nthButton, 0 };
            auto point1 = point2 - ScreenCoordsXY{ 10, 10 };
            return { point1, point2 };
        }

        void drawWeightButton(
            RenderTarget& rt, const ObjectEntryIndex objectIdx, const uint8_t nthButton, const ScreenRect& rect)
        {
            // draw btn
            bool pressed = weightBtnPressed.has_value() && weightBtnPressed.value().first == objectIdx
                && weightBtnPressed->second == nthButton;
            ScreenRect btnRect = itemRectToWeightBtnRect(rect, nthButton);
            GfxFillRectInset(rt, btnRect, colours[2], pressed ? INSET_RECT_FLAG_BORDER_INSET : 0);

            // draw text
            auto width = btnRect.GetWidth() - 2;
            auto coords = btnRect.Point1 + ScreenCoordsXY{ 1 + width / 2, 0 };
            auto stringId = nthButton == 0 ? STR_NUMERIC_UP : STR_NUMERIC_DOWN;
            Formatter ft;
            auto textPaint = TextPaint{ colours[2], TextAlignment::CENTRE };
            DrawTextEllipsised(rt, coords, width, stringId, ft, textPaint);
        }

        void SceneryDrawItems(RenderTarget& rt)
        {
            GfxClear(rt, ColourMapA[colours[1].colour].mid_light);

            auto numColumns = GetNumColumns();

            ScreenCoordsXY pos{ 0, 0 };

            for (auto& itemIdx : filteredItems)
            {
                auto itemRect = ScreenRect{ pos, pos + ScreenCoordsXY{ kItemSize.width - 1, kItemSize.height - 1 } };

                bool selected = selectedItems.contains(itemIdx);
                if (selected)
                {
                    GfxFillRectInset(rt, itemRect, colours[1], INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
                else if (highlightedItem.has_value() && highlightedItem.value() == itemIdx)
                {
                    GfxFillRectInset(rt, itemRect, colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
                }

                // draw sprite
                RenderTarget clippedRT;
                if (ClipDrawPixelInfo(clippedRT, rt, pos + ScreenCoordsXY{ 1, 1 }, kItemSize.width - 2, kItemSize.height - 2))
                {
                    SceneryDrawItem(clippedRT, itemIdx);
                }

                // draw colour buttons
                auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(itemIdx);
                if (selected && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR))
                {
                    drawColourButton(rt, itemIdx, 0, itemRect);
                }
                if (selected && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR))
                {
                    drawColourButton(rt, itemIdx, 1, itemRect);
                }
                if (selected && sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR))
                {
                    drawColourButton(rt, itemIdx, 2, itemRect);
                }

                // draw weight + buttons
                if (selected)
                {
                    auto weightRect = ScreenRect{ { itemRect.GetLeft() + 4, itemRect.GetBottom() - 4 - 12 },
                                                  { itemRect.GetRight() - 4, itemRect.GetBottom() - 4 } };
                    GfxFillRectInset(rt, weightRect, colours[2], INSET_RECT_FLAG_BORDER_INSET);
                    drawWeightButton(rt, itemIdx, 0, itemRect);
                    drawWeightButton(rt, itemIdx, 1, itemRect);
                    Formatter ft;
                    ft.Add<uint16_t>(static_cast<uint16_t>(selectedItems[itemIdx].weight));
                    DrawTextBasic(
                        rt, { weightRect.GetLeft() + 2, weightRect.GetTop() + 2 }, STR_MAPGEN_RULE_SCENERY_ITEM_WEIGHT_TIMES_X,
                        ft, { colours[2] });
                }

                pos.x += kItemSize.width;
                if (pos.x >= numColumns * kItemSize.width)
                {
                    pos.y += kItemSize.height;
                    pos.x = 0;
                }
            }
        }

        void ShowColourDropdown(
            const ObjectEntryIndex idx, const uint8_t nthColour, const colour_t selectedColour, const ScreenRect& btnRect)
        {
            widgets[WIDX_COLOUR_BTN_DUMMY].type = WidgetType::colourBtn;
            widgets[WIDX_COLOUR_BTN_DUMMY].left = widgets[WIDX_SCROLL].left + btnRect.GetLeft();
            widgets[WIDX_COLOUR_BTN_DUMMY].top = widgets[WIDX_SCROLL].top + btnRect.GetTop() - scrolls[0].contentOffsetY;
            widgets[WIDX_COLOUR_BTN_DUMMY].right = widgets[WIDX_COLOUR_BTN_DUMMY].left + 12;
            widgets[WIDX_COLOUR_BTN_DUMMY].bottom = widgets[WIDX_COLOUR_BTN_DUMMY].top + 12;

            colourBtnPressed = std::make_optional(std::make_pair(idx, nthColour));
            WindowDropdownShowColour(this, &widgets[WIDX_COLOUR_BTN_DUMMY], colours[1], selectedColour);
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            weightBtnPressed = std::nullopt;

            ScreenRect box;
            auto maybeObjectIdx = getItemIdxAt(screenCoords, box);

            if (!maybeObjectIdx.has_value())
            {
                return;
            }

            auto objectIdx = maybeObjectIdx.value();
            auto* sceneryEntry = ObjectManager::GetObjectEntry<SmallSceneryEntry>(objectIdx);

            if (selectedItems.contains(objectIdx))
            {
                auto& selectedItem = selectedItems[objectIdx];

                auto rectPrimaryColour = itemRectToColourRect(box, 0);
                auto rectSecondaryColour = itemRectToColourRect(box, 1);
                auto rectTertiaryColour = itemRectToColourRect(box, 2);

                auto rectWeightUp = itemRectToWeightBtnRect(box, 0);
                auto rectWeightDown = itemRectToWeightBtnRect(box, 1);

                if (sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_PRIMARY_COLOUR) && rectPrimaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(objectIdx, 0, selectedItem.colours[0], rectPrimaryColour);
                }
                else if (
                    sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_SECONDARY_COLOUR)
                    && rectSecondaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(objectIdx, 1, selectedItem.colours[1], rectSecondaryColour);
                }
                else if (
                    sceneryEntry->HasFlag(SMALL_SCENERY_FLAG_HAS_TERTIARY_COLOUR) && rectTertiaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(objectIdx, 2, selectedItem.colours[2], rectTertiaryColour);
                }
                else if (rectWeightUp.Contains(screenCoords))
                {
                    weightBtnPressed = std::make_optional(std::make_pair(objectIdx, 0));
                    selectedItem.weight = std::min(selectedItem.weight + 1, 100);
                }
                else if (rectWeightDown.Contains(screenCoords))
                {
                    weightBtnPressed = std::make_optional(std::make_pair(objectIdx, 1));
                    selectedItem.weight = std::max(selectedItem.weight - 1, 0);
                }
                else
                {
                    selectedItems.erase(objectIdx);
                }
            }
            else
            {
                selectedItems[objectIdx] = SceneryEffectItem{ .index = objectIdx,
                                                              .weight = 1,
                                                              .direction = std::nullopt,
                                                              .colours = { COLOUR_BORDEAUX_RED, COLOUR_YELLOW,
                                                                           COLOUR_DARK_BROWN } };
            }
            applyFilter();
        }

        void OnScrollMouseUp(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (weightBtnPressed.has_value())
            {
                weightBtnPressed = std::nullopt;
            }
        }

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            ScreenRect box;
            auto itemAndBoxAt = getItemIdxAt(screenCoords, box);
            highlightedItem = itemAndBoxAt;
            Invalidate();
        }

        std::optional<ObjectEntryIndex> getItemIdxAt(const ScreenCoordsXY& screenCoords, ScreenRect& box) const
        {
            const auto columns = GetNumColumns();
            const auto col = screenCoords.x / kItemSize.width;
            const auto row = screenCoords.y / kItemSize.height;

            if (col >= 0 && col < columns && row >= 0)
            {
                const auto idx = static_cast<size_t>(row * columns + col);
                if (idx < filteredItems.size())
                {
                    auto point1 = ScreenCoordsXY{ col * kItemSize.width, row * kItemSize.height };
                    auto point2 = point1 + kItemSize;
                    box = ScreenRect{ point1, point2 };
                    return std::make_optional(filteredItems[idx]);
                }
            }
            return std::nullopt;
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            SceneryDrawItems(rt);
        }

        void SetCallingContext(const WindowBase* _callWindow, const uint16_t _callWidget)
        {
            callWidget.window.classification = _callWindow->classification;
            callWidget.window.number = _callWindow->number;
            callWidget.widget_index = _callWidget;

            colours[0] = _callWindow->colours[1];
            colours[1] = _callWindow->colours[1];
            colours[2] = _callWindow->colours[0];
        }

        void SetEffect(const SceneryEffect& _effect)
        {
            effect = _effect;
            for (auto& item : effect.objects)
            {
                selectedItems[item.index] = item;
            }
        }

        void SetCallback(const std::function<void(SceneryEffect&)>& _callback)
        {
            callback = _callback;
        }

        void invokeCallback()
        {
            if (!callback)
            {
                return;
            }

            effect.objects.clear();
            for (auto& item : selectedItems | std::views::values)
            {
                effect.objects.push_back(item);
            }
            std::ranges::sort(effect.objects, [](SceneryEffectItem& a, SceneryEffectItem& b) { return a.index < b.index; });
            callback(effect);
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CANCEL:
                    Close();
                    break;
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_OKAY:
                    invokeCallback();
                    Close();
                    break;
                case WIDX_FILTER:
                    WindowStartTextbox(*this, widgetIndex, filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR:
                    filter.clear();
                    applyFilter();
                    scrolls->contentOffsetY = 0;
                    InvalidateWidget(WIDX_FILTER);
                    break;
                case WIDX_FILTER_BY_SELECTION:
                    filterSelectedOnly = !filterSelectedOnly;
                    applyFilter();
                    scrolls->contentOffsetY = 0;
                    InvalidateWidget(WIDX_FILTER_BY_SELECTION);
                    break;
            }
        }

        std::optional<int32_t> parseInt(std::string_view& text)
        {
            auto textStr = std::string(text);
            try
            {
                return std::make_optional(std::stoi(textStr));
            }
            catch (const std::invalid_argument& e)
            {
                LOG_VERBOSE("failed parsing int from %s (%s)", textStr.c_str(), e.what());
                return std::nullopt;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            switch (widgetIndex)
            {
                case WIDX_FILTER:
                {
                    filter.assign(text);
                    applyFilter();
                    scrolls->contentOffsetY = 0;
                    break;
                }
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCROLL:
                {
                    if (selectedIndex == -1 || !colourBtnPressed.has_value())
                    {
                        break;
                    }
                    colour_t selectedColour = ColourDropDownIndexToColour(selectedIndex);
                    auto objectIdx = colourBtnPressed.value().first;
                    auto nthColour = colourBtnPressed.value().second;
                    selectedItems[objectIdx].colours[nthColour] = selectedColour;
                    colourBtnPressed.reset();
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
            }
        }

        bool WasCalledFrom(const WindowBase* call_w, const WidgetIndex call_widget) const
        {
            if (call_w == nullptr)
            {
                return false;
            }
            return callWidget.window.classification == call_w->classification && callWidget.window.number == call_w->number
                && callWidget.widget_index == call_widget;
        }
    };

    WindowBase* MapGenRuleScenerySelectOpen(
        WindowBase* callWindow, WidgetIndex callWidget, SceneryEffect& effect, std::function<void(SceneryEffect&)> callback)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::MapgenRuleScenerySelect);

        auto w = windowMgr->Create<MapGenRuleScenerySelectWindow>(
            WindowClass::MapgenRuleScenerySelect, { kWindowSize.width, kWindowSize.height + 10 },
            WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
        if (w != nullptr)
        {
            w->SetCallingContext(callWindow, callWidget);
            w->SetEffect(effect);
            w->SetCallback(callback);
        }

        return w;
    }

    void MapGenRuleScenerySelectCloseByCalling(WindowBase* call_w, WidgetIndex call_widget)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = reinterpret_cast<MapGenRuleScenerySelectWindow*>(
            windowMgr->FindByClass(WindowClass::MapgenRuleScenerySelect));
        if (w == nullptr || !w->WasCalledFrom(call_w, call_widget))
        {
            return;
        }

        w->Close();
    }

} // namespace OpenRCT2::Ui::Windows
