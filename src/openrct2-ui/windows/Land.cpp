/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/world/Park.h>

using namespace OpenRCT2;

static constexpr const StringId WINDOW_TITLE = STR_LAND;
static constexpr const int32_t WH = 160;
static constexpr const int32_t WW = 98;

// clang-format off
enum WindowLandWidgetIdx {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_MOUNTAINMODE,
    WIDX_PAINTMODE,
    WIDX_PREVIEW,
    WIDX_DECREMENT,
    WIDX_INCREMENT,
    WIDX_FLOOR,
    WIDX_WALL,
};

static Widget window_land_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({19,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_RIDE_CONSTRUCTION_SLOPE_UP), STR_ENABLE_MOUNTAIN_TOOL_TIP), // mountain mode
    MakeWidget     ({55,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_PAINTBRUSH),                 STR_DISABLE_ELEVATION),        // paint mode
    MakeWidget     ({27,  48}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary  , ImageId(SPR_LAND_TOOL_SIZE_0),           STR_NONE),                     // preview box
    MakeRemapWidget({28,  49}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,         STR_ADJUST_SMALLER_LAND_TIP),  // decrement size
    MakeRemapWidget({54,  63}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,         STR_ADJUST_LARGER_LAND_TIP),   // increment size
    MakeWidget     ({ 2, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_BASE_LAND_TIP),     // floor texture
    MakeWidget     ({49, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_VERTICAL_LAND_TIP), // wall texture
    WIDGETS_END,
};
// clang-format on

class LandWindow final : public Window
{
private:
    ObjectEntryIndex _selectedFloorTexture = 0;
    ObjectEntryIndex _selectedWallTexture = 0;

    void InputSize()
    {
        Formatter ft;
        ft.Add<int16_t>(MINIMUM_TOOL_SIZE);
        ft.Add<int16_t>(MAXIMUM_TOOL_SIZE);
        WindowTextInputOpen(this, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, ft, STR_NONE, STR_NONE, 3);
    }

public:
    void OnOpen() override
    {
        widgets = window_land_widgets;
        hold_down_widgets = (1uLL << WIDX_DECREMENT) | (1uLL << WIDX_INCREMENT);
        WindowInitScrollWidgets(*this);
        WindowPushOthersBelow(*this);

        gLandToolSize = 1;
        gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
        gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
        gLandMountainMode = false;
        gLandPaintMode = false;
        _selectedFloorTexture = 0;
        _selectedWallTexture = 0;
        gLandToolRaiseCost = MONEY64_UNDEFINED;
        gLandToolLowerCost = MONEY64_UNDEFINED;
    }

    void OnClose() override
    {
        // If the tool wasn't changed, turn tool off
        if (LandToolIsActive())
            ToolCancel();
    }

    void OnMouseUp(WidgetIndex widgetIndex) override
    {
        switch (widgetIndex)
        {
            case WIDX_CLOSE:
                Close();
                break;
            case WIDX_MOUNTAINMODE:
                gLandMountainMode ^= 1;
                gLandPaintMode = 0;
                Invalidate();
                break;
            case WIDX_PAINTMODE:
                gLandMountainMode = 0;
                gLandPaintMode ^= 1;
                Invalidate();
                break;
            case WIDX_PREVIEW:
                InputSize();
                break;
        }
    }

    void OnMouseDown(WidgetIndex widgetIndex) override
    {
        Widget* widget = &widgets[widgetIndex];
        switch (widgetIndex)
        {
            case WIDX_FLOOR:
                LandTool::ShowSurfaceStyleDropdown(this, widget, _selectedFloorTexture);
                break;
            case WIDX_WALL:
                LandTool::ShowEdgeStyleDropdown(this, widget, _selectedWallTexture);
                break;
            case WIDX_PREVIEW:
                InputSize();
                break;
            case WIDX_DECREMENT:
                // Decrement land tool size
                gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

                // Invalidate the window
                Invalidate();
                break;
            case WIDX_INCREMENT:
                // Increment land tool size
                gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

                // Invalidate the window
                Invalidate();
                break;
        }
    }

    void OnDropdown(WidgetIndex widgetIndex, int32_t dropdownIndex) override
    {
        int32_t type;

        switch (widgetIndex)
        {
            case WIDX_FLOOR:
                if (dropdownIndex == -1)
                    dropdownIndex = gDropdownHighlightedIndex;

                type = (dropdownIndex == -1) ? _selectedFloorTexture : dropdownIndex;

                if (gLandToolTerrainSurface == type)
                {
                    gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
                }
                else
                {
                    gLandToolTerrainSurface = type;
                    _selectedFloorTexture = type;
                }
                Invalidate();
                break;
            case WIDX_WALL:
                if (dropdownIndex == -1)
                    dropdownIndex = gDropdownHighlightedIndex;

                type = (dropdownIndex == -1) ? _selectedWallTexture : dropdownIndex;

                if (gLandToolTerrainEdge == type)
                {
                    gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
                }
                else
                {
                    gLandToolTerrainEdge = type;
                    _selectedWallTexture = type;
                }
                Invalidate();
                break;
        }
    }

    void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
    {
        if (widgetIndex != WIDX_PREVIEW)
            return;

        char* end;
        std::string textStr = std::string(text);
        int32_t size = strtol(textStr.c_str(), &end, 10);
        if (*end == '\0')
        {
            size = std::max(MINIMUM_TOOL_SIZE, size);
            size = std::min(MAXIMUM_TOOL_SIZE, size);
            gLandToolSize = size;

            Invalidate();
        }
    }

    void OnUpdate() override
    {
        if (!LandToolIsActive())
            Close();
    }

    void OnPrepareDraw() override
    {
        pressed_widgets = 0;
        SetWidgetPressed(WIDX_PREVIEW, true);
        if (gLandToolTerrainSurface != OBJECT_ENTRY_INDEX_NULL)
            SetWidgetPressed(WIDX_FLOOR, true);
        if (gLandToolTerrainEdge != OBJECT_ENTRY_INDEX_NULL)
            SetWidgetPressed(WIDX_WALL, true);
        if (gLandMountainMode)
            SetWidgetPressed(WIDX_MOUNTAINMODE, true);
        if (gLandPaintMode)
            SetWidgetPressed(WIDX_PAINTMODE, true);

        // Update the preview image (for tool sizes up to 7)
        widgets[WIDX_PREVIEW].image = ImageId(LandTool::SizeToSpriteIndex(gLandToolSize));
    }

    void OnDraw(DrawPixelInfo& dpi) override
    {
        ScreenCoordsXY screenCoords;
        int32_t numTiles;
        money64 price;
        Widget* previewWidget = &widgets[WIDX_PREVIEW];

        DrawWidgets(dpi);
        DrawDropdownButtons(dpi);

        // Draw number for tool sizes bigger than 7
        if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
        {
            auto ft = Formatter();
            ft.Add<uint16_t>(gLandToolSize);
            screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->midY() };
            DrawTextBasic(dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, ft, { TextAlignment::CENTRE });
        }
        else if (gLandMountainMode)
        {
            screenCoords = { windowPos.x + previewWidget->left, windowPos.y + previewWidget->top };
            auto sprite = ImageId(gLandToolSize % 2 == 0 ? SPR_G2_MOUNTAIN_TOOL_EVEN : SPR_G2_MOUNTAIN_TOOL_ODD);
            GfxDrawSprite(dpi, sprite, screenCoords);
            WidgetDraw(dpi, *this, WIDX_DECREMENT);
            WidgetDraw(dpi, *this, WIDX_INCREMENT);
        }

        screenCoords = { windowPos.x + previewWidget->midX(), windowPos.y + previewWidget->bottom + 5 };

        if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            // Draw raise cost amount
            if (gLandToolRaiseCost != MONEY64_UNDEFINED && gLandToolRaiseCost != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(gLandToolRaiseCost);
                DrawTextBasic(dpi, screenCoords, STR_RAISE_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
            screenCoords.y += 10;

            // Draw lower cost amount
            if (gLandToolLowerCost != MONEY64_UNDEFINED && gLandToolLowerCost != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(gLandToolLowerCost);
                DrawTextBasic(dpi, screenCoords, STR_LOWER_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
            screenCoords.y += 50;

            // Draw paint price
            numTiles = gLandToolSize * gLandToolSize;
            price = 0;
            if (gLandToolTerrainSurface != OBJECT_ENTRY_INDEX_NULL)
            {
                auto& objManager = GetContext()->GetObjectManager();
                const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
                    objManager.GetLoadedObject(ObjectType::TerrainSurface, gLandToolTerrainSurface));
                if (surfaceObj != nullptr)
                {
                    price += numTiles * static_cast<money64>(surfaceObj->Price);
                }
            }

            if (gLandToolTerrainEdge != OBJECT_ENTRY_INDEX_NULL)
                price += numTiles * 100LL;

            if (price != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(price);
                DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, ft, { TextAlignment::CENTRE });
            }
        }
    }

private:
    void DrawDropdownButtons(DrawPixelInfo& dpi)
    {
        auto& objManager = GetContext()->GetObjectManager();
        const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
            objManager.GetLoadedObject(ObjectType::TerrainSurface, _selectedFloorTexture));
        ImageId surfaceImage;
        if (surfaceObj != nullptr)
        {
            surfaceImage = ImageId(surfaceObj->IconImageId);
            if (surfaceObj->Colour != 255)
                surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
        }

        const auto edgeObj = static_cast<TerrainEdgeObject*>(
            objManager.GetLoadedObject(ObjectType::TerrainEdge, _selectedWallTexture));
        ImageId edgeImage;
        if (edgeObj != nullptr)
        {
            edgeImage = ImageId(edgeObj->IconImageId);
        }

        DrawDropdownButton(dpi, WIDX_FLOOR, surfaceImage);
        DrawDropdownButton(dpi, WIDX_WALL, edgeImage);
    }

    void DrawDropdownButton(DrawPixelInfo& dpi, WidgetIndex widgetIndex, ImageId image)
    {
        const auto& widget = widgets[widgetIndex];
        GfxDrawSprite(dpi, image, { windowPos.x + widget.left, windowPos.y + widget.top });
    }
};

WindowBase* WindowLandOpen()
{
    return WindowFocusOrCreate<LandWindow>(WindowClass::Land, ScreenCoordsXY(ContextGetWidth() - WW, 29), WW, WH, 0);
}
