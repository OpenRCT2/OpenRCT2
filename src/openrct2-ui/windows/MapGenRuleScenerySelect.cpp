/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/object/StationObject.h"

#include <openrct2-ui/UiStringIds.h>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/String.hpp>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/drawing/RenderTarget.h>
#include <openrct2/drawing/Text.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectEntryManager.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/SmallSceneryEntry.h>
#include <openrct2/object/SmallSceneryObject.h>
#include <openrct2/object/WallObject.h>
#include <openrct2/object/WallSceneryEntry.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/util/Hash.hpp>
#include <openrct2/world/map_generator/rule/Rule.h>
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

    struct SceneryItem
    {
        RuleSceneryType type;
        ObjectEntryIndex index;

        friend bool operator==(const SceneryItem& lhs, const SceneryItem& rhs)
        {
            return lhs.type == rhs.type && lhs.index == rhs.index;
        }
    };

    struct SceneryItemHash
    {
        size_t operator()(const SceneryItem& si) const noexcept
        {
            size_t hash = 0;
            Util::Hash::update(hash, si.type);
            Util::Hash::update(hash, si.index);
            return hash;
        }
    };

    enum class ColourSlot
    {
        Primary,
        Secondary,
        Tertiary,
    };

    class MapGenRuleScenerySelectWindow final : public Window
    {
    private:
        WidgetIdentifier callWidget = {};
        SceneryEffect effect = {};
        std::function<void(SceneryEffect&)> callback;

        u8string filter = "";
        bool filterSelectedOnly = false;

        std::vector<SceneryItem> availableItems;
        std::vector<SceneryItem> filteredItems;
        std::unordered_map<SceneryItem, SceneryEffectItem, SceneryItemHash> selectedItems;

        std::optional<SceneryItem> highlightedItem = std::nullopt;
        std::optional<std::pair<SceneryItem, uint8_t>> colourBtnPressed = std::nullopt;
        std::optional<std::pair<SceneryItem, uint8_t>> weightBtnPressed = std::nullopt;
        std::optional<SceneryItem> rotationBtnPressed = std::nullopt;

        bool HasParentWindow() const
        {
            return callWidget.window.classification != WindowClass::null;
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
        void onOpen() override
        {
            setWidgets(_widgets);
            WindowSetResize(*this, kWindowSize, kWindowSize);
            WindowInitScrollWidgets(*this);

            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxSmallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    availableItems.emplace_back(Small, sceneryId);
                }
            }
            for (ObjectEntryIndex sceneryId = 0; sceneryId < kMaxWallSceneryObjects; sceneryId++)
            {
                const auto* sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(sceneryId);
                if (sceneryEntry != nullptr)
                {
                    availableItems.emplace_back(Wall, sceneryId);
                }
            }

            applyFilter();
        }

        void onUpdate() override
        {
            if (HasParentWindow())
            {
                // If the calling window is closed then close this window
                auto parentWindow = GetParentWindow();
                if (parentWindow == nullptr)
                {
                    close();
                    return;
                }
            }

            if (GetCurrentTextBox().window.classification == classification && GetCurrentTextBox().window.number == number)
            {
                WindowUpdateTextboxCaret();
                invalidateWidget(WIDX_FILTER);
            }

            if (!widgetIsHighlighted(*this, WIDX_SCROLL))
            {
                highlightedItem = std::nullopt;
                invalidate();
            }
        }

        bool matchesFilter(const SceneryItem& si) const
        {
            auto object = GetContext()->GetObjectManager().GetLoadedObject(objectTypeOf(si.type), si.index);
            auto matchesSelectedOnlyFilter = !filterSelectedOnly || selectedItems.contains(si);
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
            for (const SceneryItem& item : availableItems)
            {
                if (matchesFilter(item))
                {
                    filteredItems.push_back(item);
                }
            }
            invalidate();
        }

        void onPrepareDraw() override
        {
            widgets[WIDX_COLOUR_BTN_DUMMY].type = WidgetType::empty;
            widgets[WIDX_FILTER].string = filter.data();

            setWidgetPressed(WIDX_FILTER_BY_SELECTION, filterSelectedOnly);
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            if (highlightedItem.has_value())
            {
                auto hi = highlightedItem.value();
                auto* obj = GetContext()->GetObjectManager().GetLoadedObject(objectTypeOf(hi.type), hi.index);
                Guard::Assert(obj != nullptr);

                auto coordsName = windowPos + ScreenCoordsXY{ 5, 365 };
                auto coordsId = windowPos + ScreenCoordsXY{ 613, 365 };
                auto labelWidth = (kWindowSize.width - 10) / 2;
                auto colour = colours[1];

                std::string name = obj->GetName();
                std::string identifier = std::string(obj->GetIdentifier());

                Formatter ftName;
                ftName.Add<char*>(name.c_str());
                drawTextEllipsised(rt, coordsName, labelWidth, STR_STRING, ftName, { colour, TextAlignment::left });

                Formatter ftIdentifier;
                ftIdentifier.Add<char*>(identifier.c_str());
                drawTextEllipsised(rt, coordsId, labelWidth, STR_STRING, ftIdentifier, { colour, TextAlignment::right });
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
            return static_cast<int32_t>(std::ceil(items / columns));
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, GetNumRows() * kItemSize.height };
        }

        static bool isRotatable(const SceneryItem& si)
        {
            switch (si.type)
            {
                case Small:
                {
                    auto sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(si.index);
                    return sceneryEntry->flags.has(SmallSceneryFlag::isRotatable);
                }
                case Large:
                {
                    // TODO
                    break;
                }
                case Wall:
                {
                    return true;
                }
            }
            return false;
        }

        static bool hasColourSlot(const SceneryItem& si, const ColourSlot& colourSlot)
        {
            switch (si.type)
            {
                case Small:
                {
                    auto sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(si.index);
                    switch (colourSlot)
                    {
                        case ColourSlot::Primary:
                            return sceneryEntry->flags.has(SmallSceneryFlag::hasPrimaryColour);
                        case ColourSlot::Secondary:
                            return sceneryEntry->flags.has(SmallSceneryFlag::hasSecondaryColour);
                        case ColourSlot::Tertiary:
                            return sceneryEntry->flags.has(SmallSceneryFlag::hasTertiaryColour);
                    }
                }
                case Large:
                {
                    // TODO
                    break;
                }
                case Wall:
                {
                    auto wallEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(si.index);
                    switch (colourSlot)
                    {
                        case ColourSlot::Primary:
                            return wallEntry->flags.has(WallSceneryFlag::hasPrimaryColour);
                        case ColourSlot::Secondary:
                            return wallEntry->flags.has(WallSceneryFlag::hasSecondaryColour);
                        case ColourSlot::Tertiary:
                            return wallEntry->flags.has(WallSceneryFlag::hasTertiaryColour);
                    }
                }
            }
            return false;
        }

        void SceneryDrawItem(Drawing::RenderTarget& rt, const SceneryItem& si)
        {
            std::array<Drawing::Colour, 3> itemColours = { Drawing::Colour::bordeauxRed, Drawing::Colour::yellow,
                                                           Drawing::Colour::darkBrown };
            uint8_t direction = 0;
            int32_t weight = 1;

            if (selectedItems.contains(si))
            {
                auto& effectItem = selectedItems[si];
                itemColours = effectItem.colours;
                direction = effectItem.direction.value_or(0);
                weight = effectItem.weight;
            }

            switch (si.type)
            {
                case Small:
                {
                    auto sceneryEntry = OpenRCT2::ObjectEntryManager::GetObjectEntry<SmallSceneryEntry>(si.index);
                    auto imageId = ImageId(sceneryEntry->image + direction);

                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasPrimaryColour))
                    {
                        imageId = imageId.WithPrimary(itemColours[0]);
                    }
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasSecondaryColour))
                    {
                        imageId = imageId.WithSecondary(itemColours[1]);
                    }
                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasTertiaryColour))
                    {
                        imageId = imageId.WithTertiary(itemColours[2]);
                    }

                    auto spriteTop = (kItemSize.height / 2) + (sceneryEntry->height / 2);
                    if (sceneryEntry->flags.hasAll(SmallSceneryFlag::occupiesFullTile, SmallSceneryFlag::vOffsetCentre))
                    {
                        spriteTop -= 12;
                    }

                    if (weight == 0)
                    {
                        imageId = imageId.WithRemap(Drawing::FilterPaletteID::paletteGhost);
                    }

                    auto spritePosition = ScreenCoordsXY{ kItemSize.width / 2, spriteTop };

                    GfxDrawSprite(rt, imageId, spritePosition);

                    if (sceneryEntry->flags.has(SmallSceneryFlag::hasGlass))
                    {
                        imageId = ImageId(sceneryEntry->image + 4 + direction).WithTransparency(itemColours[0]);
                        GfxDrawSprite(rt, imageId, spritePosition);
                    }

                    if (sceneryEntry->flags.has(SmallSceneryFlag::isAnimated))
                    {
                        imageId = ImageId(sceneryEntry->image + 4 + direction);
                        GfxDrawSprite(rt, imageId, spritePosition);
                    }
                    break;
                }
                case Large:
                {
                    // TODO
                    break;
                }
                case Wall:
                {
                    auto wallEntry = ObjectEntryManager::GetObjectEntry<WallSceneryEntry>(si.index);
                    if (wallEntry == nullptr)
                        return;

                    auto directionImgOffset = 0;
                    auto directionPosOffset = 0;
                    switch (direction)
                    {
                        case 0 /* NE */:
                            directionImgOffset = 1;
                            directionPosOffset = -16;
                            break;
                        case 1 /* SE */:
                            directionImgOffset = 0;
                            directionPosOffset = 16;
                            break;
                        case 2 /* SW */:
                            directionImgOffset = 1;
                            directionPosOffset = -16;
                            break;
                        case 3 /* NW */:
                            directionImgOffset = 0;
                            directionPosOffset = 16;
                            break;
                    }

                    auto imageId = ImageId(wallEntry->image + directionImgOffset);

                    if (wallEntry->flags.has(WallSceneryFlag::hasPrimaryColour))
                    {
                        imageId = imageId.WithPrimary(itemColours[0]);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasSecondaryColour))
                    {
                        imageId = imageId.WithSecondary(itemColours[1]);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasTertiaryColour))
                    {
                        imageId = imageId.WithTertiary(itemColours[2]);
                    }

                    if (weight == 0)
                    {
                        imageId = imageId.WithRemap(Drawing::FilterPaletteID::paletteGhost);
                    }

                    auto spriteTop = (kItemSize.height / 2) + (wallEntry->height / 2);
                    auto spritePosition = ScreenCoordsXY{ (kItemSize.width / 2) + directionPosOffset, spriteTop };

                    GfxDrawSprite(rt, imageId, spritePosition);

                    if (wallEntry->flags.has(WallSceneryFlag::isDoor))
                    {
                        GfxDrawSprite(rt, imageId.WithIndexOffset(1), spritePosition);
                    }
                    if (wallEntry->flags.has(WallSceneryFlag::hasGlass))
                    {
                        auto glassImageId = ImageId(wallEntry->image + 6).WithTransparency(itemColours[1]);
                        GfxDrawSprite(rt, glassImageId, spritePosition);
                    }

                    break;
                }
            }
        }

        ScreenRect itemRectToColourRect(const ScreenRect& itemRect, const uint8_t nthColour)
        {
            auto point1 = ScreenCoordsXY{ itemRect.GetRight() - 14, itemRect.GetTop() + 4 + (12 * nthColour) };
            auto point2 = point1 + ScreenCoordsXY{ 12, 12 };
            return { point1, point2 };
        }

        void drawColourButton(Drawing::RenderTarget& rt, const SceneryItem si, const uint8_t nthColour, const ScreenRect& rect)
        {
            ImageId btnImage = getColourButtonImage(selectedItems[si].colours[nthColour]);
            if (colourBtnPressed.has_value() && colourBtnPressed.value().first == si
                && colourBtnPressed.value().second == nthColour)
            {
                btnImage = btnImage.WithIndexOffset(1);
            }
            auto colourBtnRect = itemRectToColourRect(rect, nthColour);
            GfxDrawSprite(rt, btnImage.WithBlended(false), colourBtnRect.Point1);
        }

        void drawRotationButton(Drawing::RenderTarget& rt, const SceneryItem si, const ScreenRect& rect)
        {
            // draw btn
            bool pressed = rotationBtnPressed.has_value() && rotationBtnPressed.value() == si;
            ScreenRect btnRect = itemRectToRotationBtnRect(rect);
            Drawing::Rectangle::fillInset(
                rt, btnRect, colours[2],
                pressed ? Drawing::Rectangle::BorderStyle::inset : Drawing::Rectangle::BorderStyle::outset);

            // draw text
            auto& direction = selectedItems[si].direction;

            auto btnWidth = btnRect.GetWidth() - 2;
            auto coords = btnRect.Point1 + ScreenCoordsXY{ 1 + btnWidth / 2, 0 };
            auto stringId = STR_MAPGEN_SCENERY_EFFECT_ROTATION_RND;
            if (direction.has_value())
            {
                switch (direction.value())
                {
                    case 0 /* NE */:
                        stringId = STR_MAPGEN_SCENERY_EFFECT_ROTATION_NE;
                        break;
                    case 1 /* SE */:
                        stringId = STR_MAPGEN_SCENERY_EFFECT_ROTATION_SE;
                        break;
                    case 2 /* SW */:
                        stringId = STR_MAPGEN_SCENERY_EFFECT_ROTATION_SW;
                        break;
                    case 3 /* NW */:
                        stringId = STR_MAPGEN_SCENERY_EFFECT_ROTATION_NW;
                        break;
                }
            }
            Formatter ft;
            auto textPaint = TextPaint{ colours[2], TextAlignment::centre };
            drawTextEllipsised(rt, coords, btnWidth, stringId, ft, textPaint);
        }

        ScreenRect itemRectToWeightBtnRect(const ScreenRect& rect, const uint8_t nthButton)
        {
            auto point2 = rect.Point2 - ScreenCoordsXY{ 5, 5 } - ScreenCoordsXY{ 10 * nthButton, 0 };
            auto point1 = point2 - ScreenCoordsXY{ 10, 10 };
            return { point1, point2 };
        }

        ScreenRect itemRectToRotationBtnRect(const ScreenRect& rect)
        {
            auto point1 = rect.Point1 + ScreenCoordsXY{ 5, 5 };
            auto point2 = point1 + ScreenCoordsXY{ 20, 10 };
            return { point1, point2 };
        }

        void drawWeightButton(Drawing::RenderTarget& rt, const SceneryItem si, const uint8_t nthButton, const ScreenRect& rect)
        {
            // draw btn
            bool pressed = weightBtnPressed.has_value() && weightBtnPressed.value().first == si
                && weightBtnPressed->second == nthButton;
            ScreenRect btnRect = itemRectToWeightBtnRect(rect, nthButton);
            Drawing::Rectangle::fillInset(
                rt, btnRect, colours[2],
                pressed ? Drawing::Rectangle::BorderStyle::inset : Drawing::Rectangle::BorderStyle::outset);

            // draw text
            auto btnWidth = btnRect.GetWidth() - 2;
            auto coords = btnRect.Point1 + ScreenCoordsXY{ 1 + btnWidth / 2, 0 };
            auto stringId = nthButton == 0 ? STR_NUMERIC_UP : STR_NUMERIC_DOWN;
            Formatter ft;
            auto textPaint = TextPaint{ colours[2], TextAlignment::centre };
            drawTextEllipsised(rt, coords, btnWidth, stringId, ft, textPaint);
        }

        void SceneryDrawItems(Drawing::RenderTarget& rt)
        {
            GfxClear(rt, Drawing::getColourMap(colours[1].colour).midLight);

            auto numColumns = GetNumColumns();

            ScreenCoordsXY pos{ 0, 0 };

            for (auto& item : filteredItems)
            {
                auto itemRect = ScreenRect{ pos, pos + ScreenCoordsXY{ kItemSize.width - 1, kItemSize.height - 1 } };

                bool selected = selectedItems.contains(item);
                if (selected)
                {
                    Drawing::Rectangle::fillInset(
                        rt, itemRect, colours[1], Drawing::Rectangle::BorderStyle::inset,
                        Drawing::Rectangle::FillBrightness::light);
                }
                else if (highlightedItem.has_value() && highlightedItem.value() == item)
                {
                    Drawing::Rectangle::fillInset(
                        rt, itemRect, colours[1], Drawing::Rectangle::BorderStyle::outset,
                        Drawing::Rectangle::FillBrightness::dark);
                }

                // draw sprite
                Drawing::RenderTarget clippedRT;
                if (ClipRenderTarget(clippedRT, rt, pos + ScreenCoordsXY{ 1, 1 }, kItemSize.width - 2, kItemSize.height - 2))
                {
                    SceneryDrawItem(clippedRT, item);
                }

                // draw colour buttons
                if (selected && hasColourSlot(item, ColourSlot::Primary))
                {
                    drawColourButton(rt, item, 0, itemRect);
                }
                if (selected && hasColourSlot(item, ColourSlot::Secondary))
                {
                    drawColourButton(rt, item, 1, itemRect);
                }
                if (selected && hasColourSlot(item, ColourSlot::Tertiary))
                {
                    drawColourButton(rt, item, 2, itemRect);
                }

                // draw rotation button
                if (selected && isRotatable(item))
                {
                    drawRotationButton(rt, item, itemRect);
                }

                // draw weight + buttons
                if (selected)
                {
                    auto weightRect = ScreenRect{ { itemRect.GetLeft() + 4, itemRect.GetBottom() - 4 - 12 },
                                                  { itemRect.GetRight() - 4, itemRect.GetBottom() - 4 } };
                    Drawing::Rectangle::fillInset(rt, weightRect, colours[2], Drawing::Rectangle::BorderStyle::inset);
                    drawWeightButton(rt, item, 0, itemRect);
                    drawWeightButton(rt, item, 1, itemRect);
                    Formatter ft;
                    ft.Add<uint16_t>(static_cast<uint16_t>(selectedItems[item].weight));
                    drawText(
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
            const SceneryItem si, const uint8_t nthColour, const Drawing::Colour selectedColour, const ScreenRect& btnRect)
        {
            widgets[WIDX_COLOUR_BTN_DUMMY].type = WidgetType::colourBtn;
            widgets[WIDX_COLOUR_BTN_DUMMY].left = widgets[WIDX_SCROLL].left + btnRect.GetLeft();
            widgets[WIDX_COLOUR_BTN_DUMMY].top = widgets[WIDX_SCROLL].top + btnRect.GetTop() - scrolls[0].contentOffsetY;
            widgets[WIDX_COLOUR_BTN_DUMMY].right = widgets[WIDX_COLOUR_BTN_DUMMY].left + 12;
            widgets[WIDX_COLOUR_BTN_DUMMY].bottom = widgets[WIDX_COLOUR_BTN_DUMMY].top + 12;

            colourBtnPressed = std::make_optional(std::make_pair(si, nthColour));
            WindowDropdownShowColour(this, &widgets[WIDX_COLOUR_BTN_DUMMY], colours[1], selectedColour);
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            weightBtnPressed = std::nullopt;
            rotationBtnPressed = std::nullopt;

            ScreenRect box;
            auto maybeObjectIdx = getItemIdxAt(screenCoords, box);

            if (!maybeObjectIdx.has_value())
            {
                return;
            }

            auto sceneryItem = maybeObjectIdx.value();

            if (selectedItems.contains(sceneryItem))
            {
                auto& selectedItem = selectedItems[sceneryItem];

                auto rectPrimaryColour = itemRectToColourRect(box, 0);
                auto rectSecondaryColour = itemRectToColourRect(box, 1);
                auto rectTertiaryColour = itemRectToColourRect(box, 2);

                auto rectWeightUp = itemRectToWeightBtnRect(box, 0);
                auto rectWeightDown = itemRectToWeightBtnRect(box, 1);

                auto rectRotation = itemRectToRotationBtnRect(box);

                if (hasColourSlot(sceneryItem, ColourSlot::Primary) && rectPrimaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(sceneryItem, 0, selectedItem.colours[0], rectPrimaryColour);
                }
                else if (hasColourSlot(sceneryItem, ColourSlot::Secondary) && rectSecondaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(sceneryItem, 1, selectedItem.colours[1], rectSecondaryColour);
                }
                else if (hasColourSlot(sceneryItem, ColourSlot::Tertiary) && rectTertiaryColour.Contains(screenCoords))
                {
                    ShowColourDropdown(sceneryItem, 2, selectedItem.colours[2], rectTertiaryColour);
                }
                else if (isRotatable(sceneryItem) && rectRotation.Contains(screenCoords))
                {
                    rotationBtnPressed = std::make_optional(sceneryItem);
                    if (selectedItem.direction.has_value())
                    {
                        switch (selectedItem.direction.value())
                        {
                            case 0 /* NE */:
                                selectedItem.direction = 1;
                                break;
                            case 1 /* SE */:
                                selectedItem.direction = 2;
                                break;
                            case 2 /* SW */:
                                selectedItem.direction = 3;
                                break;
                            case 3 /* NW */:
                                selectedItem.direction = std::nullopt;
                                break;
                        }
                    }
                    else
                    {
                        selectedItem.direction = 0;
                    }
                }
                else if (rectWeightUp.Contains(screenCoords))
                {
                    weightBtnPressed = std::make_optional(std::make_pair(sceneryItem, 0));
                    selectedItem.weight = std::min(selectedItem.weight + 1, 100);
                }
                else if (rectWeightDown.Contains(screenCoords))
                {
                    weightBtnPressed = std::make_optional(std::make_pair(sceneryItem, 1));
                    selectedItem.weight = std::max(selectedItem.weight - 1, 0);
                }
                else
                {
                    selectedItems.erase(sceneryItem);
                }
            }
            else
            {
                selectedItems[sceneryItem] = SceneryEffectItem{
                    .type = sceneryItem.type,
                    .index = sceneryItem.index,
                    .weight = 1,
                    .direction = std::nullopt,
                    .colours = { Drawing::Colour::bordeauxRed, Drawing::Colour::yellow, Drawing::Colour::darkBrown }
                };
            }
            applyFilter();
        }

        void onScrollMouseUp(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (weightBtnPressed.has_value())
            {
                weightBtnPressed = std::nullopt;
            }
            if (rotationBtnPressed.has_value())
            {
                rotationBtnPressed = std::nullopt;
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            ScreenRect box;
            auto itemAndBoxAt = getItemIdxAt(screenCoords, box);
            highlightedItem = itemAndBoxAt;
            invalidate();
        }

        std::optional<SceneryItem> getItemIdxAt(const ScreenCoordsXY& screenCoords, ScreenRect& box) const
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

        void onScrollDraw(int32_t scrollIndex, Drawing::RenderTarget& rt) override
        {
            SceneryDrawItems(rt);
        }

        void SetCallingContext(const WindowBase* _callWindow, const uint16_t _callWidget)
        {
            callWidget.window.classification = _callWindow->classification;
            callWidget.window.number = _callWindow->number;
            callWidget.widgetIndex = _callWidget;

            colours[0] = _callWindow->colours[1];
            colours[1] = _callWindow->colours[1];
            colours[2] = _callWindow->colours[0];
        }

        void SetEffect(const SceneryEffect& _effect)
        {
            effect = _effect;
            for (auto& item : effect.objects)
            {
                selectedItems[{ item.type, item.index }] = item;
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

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CANCEL:
                    close();
                    break;
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_OKAY:
                    invokeCallback();
                    close();
                    break;
                case WIDX_FILTER:
                    WindowStartTextbox(*this, widgetIndex, filter, kTextInputSize);
                    break;
                case WIDX_FILTER_CLEAR:
                    filter.clear();
                    applyFilter();
                    scrolls->contentOffsetY = 0;
                    invalidateWidget(WIDX_FILTER);
                    break;
                case WIDX_FILTER_BY_SELECTION:
                    filterSelectedOnly = !filterSelectedOnly;
                    applyFilter();
                    scrolls->contentOffsetY = 0;
                    invalidateWidget(WIDX_FILTER_BY_SELECTION);
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

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
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

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_SCROLL:
                {
                    if (selectedIndex == -1 || !colourBtnPressed.has_value())
                    {
                        break;
                    }
                    Drawing::Colour selectedColour = ColourDropDownIndexToColour(selectedIndex);
                    auto objectIdx = colourBtnPressed.value().first;
                    auto nthColour = colourBtnPressed.value().second;
                    selectedItems[objectIdx].colours[nthColour] = selectedColour;
                    colourBtnPressed.reset();
                    break;
                }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex) {}
        }

        bool WasCalledFrom(const WindowBase* call_w, const WidgetIndex call_widget) const
        {
            if (call_w == nullptr)
            {
                return false;
            }
            return callWidget.window.classification == call_w->classification && callWidget.window.number == call_w->number
                && callWidget.widgetIndex == call_widget;
        }
    };

    WindowBase* MapGenRuleScenerySelectOpen(
        WindowBase* callWindow, WidgetIndex callWidget, SceneryEffect& effect,
        const std::function<void(SceneryEffect&)>& callback)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::mapgenRuleScenerySelect);

        auto w = windowMgr->Create<MapGenRuleScenerySelectWindow>(
            WindowClass::mapgenRuleScenerySelect, { kWindowSize.width, kWindowSize.height + 10 },
            { WindowFlag::centreScreen, WindowFlag::stickToFront });
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
            windowMgr->FindByClass(WindowClass::mapgenRuleScenerySelect));
        if (w == nullptr || !w->WasCalledFrom(call_w, call_widget))
        {
            return;
        }

        w->close();
    }

} // namespace OpenRCT2::Ui::Windows
