/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/LandTool.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/TerrainEdgeObject.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>

using namespace OpenRCT2;

static constexpr const rct_string_id WINDOW_TITLE = STR_LAND;
static constexpr const int32_t WH = 160;
static constexpr const int32_t WW = 98;

// clang-format off
enum WINDOW_LAND_WIDGET_IDX {
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

static rct_widget window_land_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget     ({19,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_RIDE_CONSTRUCTION_SLOPE_UP, STR_ENABLE_MOUNTAIN_TOOL_TIP), // mountain mode
    MakeWidget     ({55,  19}, {24, 24}, WindowWidgetType::FlatBtn, WindowColour::Secondary, SPR_PAINTBRUSH,                 STR_DISABLE_ELEVATION),        // paint mode
    MakeWidget     ({27,  48}, {44, 32}, WindowWidgetType::ImgBtn,  WindowColour::Primary  , SPR_LAND_TOOL_SIZE_0,           STR_NONE),                     // preview box
    MakeRemapWidget({28,  49}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_DECREASE,         STR_ADJUST_SMALLER_LAND_TIP),  // decrement size
    MakeRemapWidget({54,  63}, {16, 16}, WindowWidgetType::TrnBtn,  WindowColour::Secondary, SPR_LAND_TOOL_INCREASE,         STR_ADJUST_LARGER_LAND_TIP),   // increment size
    MakeWidget     ({ 2, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_BASE_LAND_TIP),     // floor texture
    MakeWidget     ({49, 106}, {47, 36}, WindowWidgetType::FlatBtn, WindowColour::Secondary, 0xFFFFFFFF,                     STR_CHANGE_VERTICAL_LAND_TIP), // wall texture
    { WIDGETS_END },
};

static void window_land_close(rct_window *w);
static void window_land_mouseup(rct_window *w, rct_widgetindex widgetIndex);
static void window_land_mousedown(rct_window *w, rct_widgetindex widgetIndex, rct_widget* widget);
static void window_land_dropdown(rct_window *w, rct_widgetindex widgetIndex, int32_t dropdownIndex);
static void window_land_update(rct_window *w);
static void window_land_invalidate(rct_window *w);
static void window_land_paint(rct_window *w, rct_drawpixelinfo *dpi);
static void window_land_textinput(rct_window *w, rct_widgetindex widgetIndex, char *text);
static void window_land_inputsize(rct_window *w);

static rct_window_event_list window_land_events([](auto& events)
{
    events.close = &window_land_close;
    events.mouse_up = &window_land_mouseup;
    events.mouse_down = &window_land_mousedown;
    events.dropdown = &window_land_dropdown;
    events.update = &window_land_update;
    events.text_input = &window_land_textinput;
    events.invalidate = &window_land_invalidate;
    events.paint = &window_land_paint;
});
// clang-format on

static int32_t _selectedFloorTexture;
static int32_t _selectedWallTexture;

/**
 *
 *  rct2: 0x00663E7D
 */
rct_window* window_land_open()
{
    rct_window* window;

    // Check if window is already open
    window = window_find_by_class(WC_LAND);
    if (window != nullptr)
        return window;

    window = WindowCreate(ScreenCoordsXY(context_get_width() - 98, 29), 98, 160, &window_land_events, WC_LAND, 0);
    window->widgets = window_land_widgets;
    window->enabled_widgets = (1 << WIDX_CLOSE) | (1 << WIDX_DECREMENT) | (1 << WIDX_INCREMENT) | (1 << WIDX_FLOOR)
        | (1 << WIDX_WALL) | (1 << WIDX_MOUNTAINMODE) | (1 << WIDX_PAINTMODE) | (1 << WIDX_PREVIEW);
    window->hold_down_widgets = (1 << WIDX_DECREMENT) | (1 << WIDX_INCREMENT);
    WindowInitScrollWidgets(window);
    window_push_others_below(window);

    gLandToolSize = 1;
    gLandToolTerrainSurface = OBJECT_ENTRY_INDEX_NULL;
    gLandToolTerrainEdge = OBJECT_ENTRY_INDEX_NULL;
    gLandMountainMode = false;
    gLandPaintMode = false;
    _selectedFloorTexture = TERRAIN_GRASS;
    _selectedWallTexture = TERRAIN_EDGE_ROCK;
    gLandToolRaiseCost = MONEY32_UNDEFINED;
    gLandToolLowerCost = MONEY32_UNDEFINED;

    return window;
}

/**
 *
 *  rct2: 0x006640A5
 */
static void window_land_close(rct_window* w)
{
    // If the tool wasn't changed, turn tool off
    if (land_tool_is_active())
        tool_cancel();
}

/**
 *
 *  rct2: 0x00664064
 */
static void window_land_mouseup(rct_window* w, rct_widgetindex widgetIndex)
{
    switch (widgetIndex)
    {
        case WIDX_CLOSE:
            window_close(w);
            break;
        case WIDX_MOUNTAINMODE:
            gLandMountainMode ^= 1;
            gLandPaintMode = 0;
            w->Invalidate();
            break;
        case WIDX_PAINTMODE:
            gLandMountainMode = 0;
            gLandPaintMode ^= 1;
            w->Invalidate();
            break;
        case WIDX_PREVIEW:
            window_land_inputsize(w);
            break;
    }
}

/**
 *
 *  rct2: 0x0066407B
 */
static void window_land_mousedown(rct_window* w, rct_widgetindex widgetIndex, rct_widget* widget)
{
    switch (widgetIndex)
    {
        case WIDX_FLOOR:
            LandTool::ShowSurfaceStyleDropdown(w, widget, _selectedFloorTexture);
            break;
        case WIDX_WALL:
            LandTool::ShowEdgeStyleDropdown(w, widget, _selectedWallTexture);
            break;
        case WIDX_PREVIEW:
            window_land_inputsize(w);
            break;
        case WIDX_DECREMENT:
            // Decrement land tool size
            gLandToolSize = std::max(MINIMUM_TOOL_SIZE, gLandToolSize - 1);

            // Invalidate the window
            w->Invalidate();
            break;
        case WIDX_INCREMENT:
            // Increment land tool size
            gLandToolSize = std::min(MAXIMUM_TOOL_SIZE, gLandToolSize + 1);

            // Invalidate the window
            w->Invalidate();
            break;
    }
}

/**
 *
 *  rct2: 0x00664090
 */
static void window_land_dropdown(rct_window* w, rct_widgetindex widgetIndex, int32_t dropdownIndex)
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
            w->Invalidate();
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
            w->Invalidate();
            break;
    }
}

static void window_land_textinput(rct_window* w, rct_widgetindex widgetIndex, char* text)
{
    int32_t size;
    char* end;

    if (widgetIndex != WIDX_PREVIEW || text == nullptr)
        return;

    size = strtol(text, &end, 10);
    if (*end == '\0')
    {
        size = std::max(MINIMUM_TOOL_SIZE, size);
        size = std::min(MAXIMUM_TOOL_SIZE, size);
        gLandToolSize = size;

        w->Invalidate();
    }
}

static void window_land_inputsize(rct_window* w)
{
    TextInputDescriptionArgs[0] = MINIMUM_TOOL_SIZE;
    TextInputDescriptionArgs[1] = MAXIMUM_TOOL_SIZE;
    window_text_input_open(w, WIDX_PREVIEW, STR_SELECTION_SIZE, STR_ENTER_SELECTION_SIZE, STR_NONE, STR_NONE, 3);
}

/**
 *
 *  rct2: 0x00664272
 */
static void window_land_update(rct_window* w)
{
    if (!land_tool_is_active())
        window_close(w);
}

/**
 *
 *  rct2: 0x00663F20
 */
static void window_land_invalidate(rct_window* w)
{
    auto surfaceImage = static_cast<uint32_t>(SPR_NONE);
    auto edgeImage = static_cast<uint32_t>(SPR_NONE);

    auto& objManager = GetContext()->GetObjectManager();
    const auto surfaceObj = static_cast<TerrainSurfaceObject*>(
        objManager.GetLoadedObject(ObjectType::TerrainSurface, _selectedFloorTexture));
    if (surfaceObj != nullptr)
    {
        surfaceImage = surfaceObj->IconImageId;
        if (surfaceObj->Colour != 255)
        {
            surfaceImage |= SPRITE_ID_PALETTE_COLOUR_1(surfaceObj->Colour);
        }
    }
    const auto edgeObj = static_cast<TerrainEdgeObject*>(
        objManager.GetLoadedObject(ObjectType::TerrainEdge, _selectedWallTexture));
    if (edgeObj != nullptr)
    {
        edgeImage = edgeObj->IconImageId;
    }

    w->pressed_widgets = (1 << WIDX_PREVIEW);
    if (gLandToolTerrainSurface != OBJECT_ENTRY_INDEX_NULL)
        w->pressed_widgets |= (1 << WIDX_FLOOR);
    if (gLandToolTerrainEdge != OBJECT_ENTRY_INDEX_NULL)
        w->pressed_widgets |= (1 << WIDX_WALL);
    if (gLandMountainMode)
        w->pressed_widgets |= (1 << WIDX_MOUNTAINMODE);
    if (gLandPaintMode)
        w->pressed_widgets |= (1 << WIDX_PAINTMODE);

    window_land_widgets[WIDX_FLOOR].image = surfaceImage;
    window_land_widgets[WIDX_WALL].image = edgeImage;
    // Update the preview image (for tool sizes up to 7)
    window_land_widgets[WIDX_PREVIEW].image = LandTool::SizeToSpriteIndex(gLandToolSize);
}

/**
 *
 *  rct2: 0x00663F7C
 */
static void window_land_paint(rct_window* w, rct_drawpixelinfo* dpi)
{
    ScreenCoordsXY screenCoords;
    int32_t numTiles;
    money32 price;
    rct_widget* previewWidget = &window_land_widgets[WIDX_PREVIEW];

    WindowDrawWidgets(w, dpi);

    // Draw number for tool sizes bigger than 7
    if (gLandToolSize > MAX_TOOL_SIZE_WITH_SPRITE)
    {
        screenCoords = { w->windowPos.x + previewWidget->midX(), w->windowPos.y + previewWidget->midY() };
        DrawTextBasic(
            dpi, screenCoords - ScreenCoordsXY{ 0, 2 }, STR_LAND_TOOL_SIZE_VALUE, &gLandToolSize, { TextAlignment::CENTRE });
    }
    else if (gLandMountainMode)
    {
        screenCoords = { w->windowPos.x + previewWidget->left, w->windowPos.y + previewWidget->top };
        int32_t sprite = gLandToolSize % 2 == 0 ? SPR_G2_MOUNTAIN_TOOL_EVEN : SPR_G2_MOUNTAIN_TOOL_ODD;
        gfx_draw_sprite(dpi, sprite, screenCoords, 0);
        WidgetDraw(dpi, w, WIDX_DECREMENT);
        WidgetDraw(dpi, w, WIDX_INCREMENT);
    }

    screenCoords = { w->windowPos.x + previewWidget->midX(), w->windowPos.y + previewWidget->bottom + 5 };

    if (!(gParkFlags & PARK_FLAGS_NO_MONEY))
    {
        // Draw raise cost amount
        if (gLandToolRaiseCost != MONEY32_UNDEFINED && gLandToolRaiseCost != 0)
            DrawTextBasic(dpi, screenCoords, STR_RAISE_COST_AMOUNT, &gLandToolRaiseCost, { TextAlignment::CENTRE });
        screenCoords.y += 10;

        // Draw lower cost amount
        if (gLandToolLowerCost != MONEY32_UNDEFINED && gLandToolLowerCost != 0)
            DrawTextBasic(dpi, screenCoords, STR_LOWER_COST_AMOUNT, &gLandToolLowerCost, { TextAlignment::CENTRE });
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
                price += numTiles * surfaceObj->Price;
            }
        }

        if (gLandToolTerrainEdge != OBJECT_ENTRY_INDEX_NULL)
            price += numTiles * 100;

        if (price != 0)
        {
            auto ft = Formatter();
            ft.Add<money32>(price);
            DrawTextBasic(dpi, screenCoords, STR_COST_AMOUNT, ft.Data(), { TextAlignment::CENTRE });
        }
    }
}
