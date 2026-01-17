/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/ViewportQuery.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/MapChangeSizeAction.h>
#include <openrct2/actions/PeepSpawnPlaceAction.h>
#include <openrct2/actions/SurfaceSetStyleAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/entity/EntityList.h>
#include <openrct2/entity/EntityRegistry.h>
#include <openrct2/entity/Staff.h>
#include <openrct2/object/TerrainSurfaceObject.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrainManager.h>
#include <openrct2/ride/Vehicle.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Footpath.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Scenery.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <openrct2/world/tile_element/TrackElement.h>
#include <vector>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    struct ColourPair
    {
        PaletteIndex a = PaletteIndex::transparent;
        PaletteIndex b = PaletteIndex::transparent;

        constexpr ColourPair() = default;

        constexpr ColourPair(PaletteIndex _a)
            : a(_a)
            , b(_a)
        {
        }

        constexpr ColourPair(PaletteIndex _a, PaletteIndex _b)
            : a(_a)
            , b(_b)
        {
        }

        constexpr bool operator==(const ColourPair& rhs) const
        {
            return a == rhs.a && b == rhs.b;
        }
    };

    static constexpr ColourPair MapColourUnowned(ColourPair colour)
    {
        return ColourPair(colour.a, PaletteIndex::pi10);
    }
    static int32_t getPracticalMapSize()
    {
        // Take non-square maps into account
        return std::max(getGameState().mapSize.x, getGameState().mapSize.y) - 2;
    }
    static int32_t getPracticalMapSizeBig()
    {
        return getPracticalMapSize() * kCoordsXYStep;
    }
    static int32_t getMaxTileStartXY()
    {
        return getPracticalMapSizeBig() - kCoordsXYStep;
    }
    static int32_t getMiniMapWidth()
    {
        return getPracticalMapSize() * 2;
    }

    static bool isEditorOrSandbox()
    {
        return gLegacyScene == LegacyScene::scenarioEditor || getGameState().cheats.sandboxMode;
    }

    static constexpr StringId kWindowTitle = STR_MAP_LABEL;
    static constexpr ScreenSize kWindowSize = { 245, 259 };

    static constexpr uint16_t kReservedHSpace = 4;
    static constexpr uint16_t kReservedTopSpace = 46;
    static constexpr uint16_t kEditorReservedHSpace = 26;
    static constexpr uint16_t kEditorReservedVSpace = 18;
    static constexpr uint16_t kRidesTabReservedVSpace = 4 * kListRowHeight + 4;
    static constexpr uint16_t kDefaultReservedVSpace = 14;

    static uint16_t GetReservedRightSpace()
    {
        return isEditorOrSandbox() ? kEditorReservedHSpace : kReservedHSpace;
    }

    static int32_t getMapOffset(int16_t width)
    {
        return (width - getMiniMapWidth() - GetReservedRightSpace() - kScrollBarWidth) / 2;
    }

    // Some functions manipulate coordinates on the map. These are the coordinates of the pixels in the
    // minimap. In order to distinguish those from actual coordinates, we use a separate name.
    using MapCoordsXY = TileCoordsXY;

    enum
    {
        PAGE_PEEPS,
        PAGE_RIDES
    };

    enum WindowMapWidgetIdx : WidgetIndex
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_RESIZE,
        WIDX_PEOPLE_TAB,
        WIDX_RIDES_TAB,
        WIDX_MAP,
        WIDX_MAP_SIZE_SPINNER_Y,
        WIDX_MAP_SIZE_SPINNER_Y_UP,
        WIDX_MAP_SIZE_SPINNER_Y_DOWN,
        WIDX_MAP_SIZE_LINK,
        WIDX_MAP_SIZE_SPINNER_X,
        WIDX_MAP_SIZE_SPINNER_X_UP,
        WIDX_MAP_SIZE_SPINNER_X_DOWN,
        WIDX_SET_LAND_RIGHTS,
        WIDX_PEOPLE_STARTING_POSITION,
        WIDX_BUILD_PARK_ENTRANCE,
        WIDX_MAP_GENERATOR
    };

    // clang-format off
    static constexpr auto window_map_widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget        ({  0,  43}, {245, 215}, WidgetType::resize,    WindowColour::secondary                                                                ),
        makeRemapWidget   ({  3,  17}, { 31,  27}, WidgetType::colourBtn, WindowColour::secondary, SPR_TAB,                      STR_SHOW_PEOPLE_ON_MAP_TIP      ),
        makeRemapWidget   ({ 34,  17}, { 31,  27}, WidgetType::colourBtn, WindowColour::secondary, SPR_TAB,                      STR_SHOW_RIDES_STALLS_ON_MAP_TIP),
        makeWidget        ({  3,  46}, {239, 180}, WidgetType::scroll,    WindowColour::secondary, SCROLL_BOTH                                                   ),
        makeSpinnerWidgets({102, 229}, { 50,  12}, WidgetType::spinner,   WindowColour::secondary, STR_COMMA16                                                   ), // NB: 3 widgets
        makeWidget        ({153, 230}, { 20,  12}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_G2_LINK_CHAIN),   STR_MAINTAIN_SQUARE_MAP_TOOLTIP ),
        makeSpinnerWidgets({174, 229}, { 50,  12}, WidgetType::spinner,   WindowColour::secondary, STR_POP16_COMMA16                                             ), // NB: 3 widgets
        makeWidget        ({  4,  46}, { 24,  24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_BUY_LAND_RIGHTS), STR_SELECT_PARK_OWNED_LAND_TIP  ),
        makeWidget        ({  4,  70}, { 24,  24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_G2_PEEP_SPAWN),   STR_SET_STARTING_POSITIONS_TIP  ),
        makeWidget        ({ 28,  94}, { 24,  24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_PARK_ENTRANCE),   STR_BUILD_PARK_ENTRANCE_TIP     ),
        makeWidget        ({110, 118}, { 24,  24}, WidgetType::flatBtn,   WindowColour::secondary, ImageId(SPR_G2_MAP_GEN_BTN),  STR_MAP_GENERATOR_TIP           )
    );
    // clang-format on

    // These represent a coefficient for the map size to be multiplied
    // Used in transforming viewport view coordinates to minimap coordinates
    // rct2: 0x00981BBC (analogous)
    static constexpr ScreenCoordsXY MiniMapOffsetFactors[] = {
        { 1, 0 },
        { 2, 1 },
        { 1, 2 },
        { 0, 1 },
    };

    static constexpr StringId MapLabels[] = {
        STR_MAP_RIDE,       STR_MAP_FOOD_STALL, STR_MAP_DRINK_STALL,  STR_MAP_SOUVENIR_STALL,
        STR_MAP_INFO_KIOSK, STR_MAP_FIRST_AID,  STR_MAP_CASH_MACHINE, STR_MAP_TOILET,
    };

    static constexpr ColourPair kRideKeyColours[] = {
        ColourPair(PaletteIndex::pi61),     // COLOUR_KEY_RIDE
        ColourPair(PaletteIndex::pi42),     // COLOUR_KEY_FOOD
        ColourPair(PaletteIndex::pi20),     // COLOUR_KEY_DRINK
        ColourPair(PaletteIndex::hotPink7), // COLOUR_KEY_SOUVENIR
        ColourPair(PaletteIndex::pi136),    // COLOUR_KEY_KIOSK
        ColourPair(PaletteIndex::pi102),    // COLOUR_KEY_FIRST_AID
        ColourPair(PaletteIndex::yellow9),  // COLOUR_KEY_CASH_MACHINE
        ColourPair(PaletteIndex::pi161),    // COLOUR_KEY_TOILETS
    };

    static constexpr PaletteIndex DefaultPeepMapColour = PaletteIndex::pi20;
    static constexpr PaletteIndex GuestMapColour = PaletteIndex::pi172;
    static constexpr PaletteIndex GuestMapColourAlternate = PaletteIndex::pi21;
    static constexpr PaletteIndex StaffMapColour = PaletteIndex::pi138;
    static constexpr PaletteIndex StaffMapColourAlternate = PaletteIndex::pi10;

    static constexpr auto kWaterColour = ColourPair(PaletteIndex::pi195);

    static constexpr ColourPair kElementTypeOverwriteColour[] = {
        ColourPair(PaletteIndex::transparent),                      // TILE_ELEMENT_TYPE_SURFACE
        ColourPair(PaletteIndex::pi17),                             // TILE_ELEMENT_TYPE_PATH
        ColourPair(PaletteIndex::pi183, PaletteIndex::transparent), // TILE_ELEMENT_TYPE_TRACK
        ColourPair(PaletteIndex::transparent, PaletteIndex::pi99),  // TILE_ELEMENT_TYPE_SMALL_SCENERY
        ColourPair(PaletteIndex::pi186),                            // TILE_ELEMENT_TYPE_ENTRANCE
        ColourPair(PaletteIndex::transparent),                      // TILE_ELEMENT_TYPE_WALL
        ColourPair(PaletteIndex::pi99),                             // TILE_ELEMENT_TYPE_LARGE_SCENERY
        ColourPair(PaletteIndex::transparent),                      // TILE_ELEMENT_TYPE_BANNER
    };

    namespace MapFlashingFlags
    {
        constexpr uint16_t FlashGuests = (1 << 1);
        constexpr uint16_t FlashStaff = (1 << 3);
        constexpr uint16_t SwitchColour = (1 << 15); // Every couple ticks the colour switches
    } // namespace MapFlashingFlags

    class MapWindow final : public Window
    {
        uint8_t _rotation;
        uint32_t _currentLine;
        uint16_t _landRightsToolSize;
        int32_t _firstColumnWidth;
        std::vector<PaletteIndex> _mapImageData;

        bool _mapWidthAndHeightLinked = true;
        bool _recalculateScrollbars = false;
        bool _adjustedForSandboxMode = false;

        enum class ResizeDirection
        {
            Both,
            X,
            Y,
        } _resizeDirection{ ResizeDirection::Both };
        uint16_t _flashingFlags = 0;

    public:
        void onOpen() override
        {
            setWidgets(window_map_widgets);

            holdDownWidgets = (1uLL << WIDX_MAP_SIZE_SPINNER_Y_UP) | (1uLL << WIDX_MAP_SIZE_SPINNER_Y_DOWN)
                | (1uLL << WIDX_MAP_SIZE_SPINNER_X_UP) | (1uLL << WIDX_MAP_SIZE_SPINNER_X_DOWN);

            flags |= WindowFlag::resizable;

            SetInitialWindowDimensions();
            ResetMaxWindowDimensions();
            ResizeMiniMap();
            initScrollWidgets();
            CalculateTextLayout();

            _rotation = GetCurrentRotation();

            initMap();
            gWindowSceneryRotation = 0;
            CentreMapOnViewPoint();
            WindowFootpathSelectDefault();

            auto& gameState = getGameState();
            _mapWidthAndHeightLinked = gameState.mapSize.x == gameState.mapSize.y;

            // Reset land rights tool size
            _landRightsToolSize = 1;
        }

        void onClose() override
        {
            _mapImageData.clear();
            _mapImageData.shrink_to_fit();

            if (isToolActive(classification, number))
            {
                ToolCancel();
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            auto* windowMgr = GetWindowManager();
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_SET_LAND_RIGHTS:
                {
                    if (!windowMgr->FindByClass(WindowClass::landRights))
                        ContextOpenWindow(WindowClass::landRights);
                    else
                        windowMgr->CloseByClass(WindowClass::landRights);
                    break;
                }
                case WIDX_BUILD_PARK_ENTRANCE:
                {
                    if (!windowMgr->FindByClass(WindowClass::editorParkEntrance))
                        ContextOpenWindow(WindowClass::editorParkEntrance);
                    else
                        windowMgr->CloseByClass(WindowClass::editorParkEntrance);
                    break;
                }
                case WIDX_PEOPLE_STARTING_POSITION:
                    if (ToolSet(*this, widgetIndex, Tool::upArrow))
                        break;

                    ShowGridlines();
                    ShowLandRights();
                    ShowConstructionRights();
                    break;
                case WIDX_MAP_SIZE_SPINNER_Y:
                case WIDX_MAP_SIZE_SPINNER_X:
                    InputMapSize(widgetIndex);
                    break;
                case WIDX_MAP_SIZE_LINK:
                    _mapWidthAndHeightLinked = !_mapWidthAndHeightLinked;
                    break;
                case WIDX_MAP_GENERATOR:
                    ContextOpenWindow(WindowClass::mapgen);
                    break;
                default:
                    if (widgetIndex >= WIDX_PEOPLE_TAB && widgetIndex <= WIDX_RIDES_TAB)
                    {
                        widgetIndex -= WIDX_PEOPLE_TAB;
                        if (widgetIndex == selectedTab)
                            break;

                        selectedTab = widgetIndex;
                        listInformationType = 0;
                        _recalculateScrollbars = true;
                        ResetMaxWindowDimensions();
                    }
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_SPINNER_Y_UP:
                    _resizeDirection = ResizeDirection::Y;
                    IncreaseMapSize();
                    break;
                case WIDX_MAP_SIZE_SPINNER_Y_DOWN:
                    _resizeDirection = ResizeDirection::Y;
                    DecreaseMapSize();
                    break;
                case WIDX_MAP_SIZE_SPINNER_X_UP:
                    _resizeDirection = ResizeDirection::X;
                    IncreaseMapSize();
                    break;
                case WIDX_MAP_SIZE_SPINNER_X_DOWN:
                    _resizeDirection = ResizeDirection::X;
                    DecreaseMapSize();
                    break;
            }
        }

        void onUpdate() override
        {
            auto* windowMgr = GetWindowManager();

            // the flickering frequency is reduced by 4, compared to the original
            // it was done due to inability to reproduce original frequency
            // and decision that the original one looks too fast
            if (gCurrentRealTimeTicks % 4 == 0)
                _flashingFlags ^= MapFlashingFlags::SwitchColour;

            // Handle guest map flashing
            _flashingFlags &= ~MapFlashingFlags::FlashGuests;
            if (windowMgr->FindByClass(WindowClass::guestList) != nullptr)
                _flashingFlags |= MapFlashingFlags::FlashGuests;

            // Handle staff map flashing
            _flashingFlags &= ~MapFlashingFlags::FlashStaff;
            if (windowMgr->FindByClass(WindowClass::staffList) != nullptr)
                _flashingFlags |= MapFlashingFlags::FlashStaff;

            if (GetCurrentRotation() != _rotation)
            {
                _rotation = GetCurrentRotation();
                initMap();
                CentreMapOnViewPoint();
            }

            for (int32_t i = 0; i < 16; i++)
                SetMapPixels();

            invalidate();

            if (_adjustedForSandboxMode != isEditorOrSandbox())
            {
                SetInitialWindowDimensions();
                ResetMaxWindowDimensions();
            }

            // Update tab animations
            listInformationType++;
            switch (selectedTab)
            {
                case PAGE_PEEPS:
                    if (listInformationType >= 32)
                    {
                        listInformationType = 0;
                    }
                    break;
                case PAGE_RIDES:
                    if (listInformationType >= 64)
                    {
                        listInformationType = 0;
                    }
                    break;
            }
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_PEOPLE_STARTING_POSITION:
                    SetPeepSpawnToolUpdate(screenCoords);
                    break;
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            switch (widgetIndex)
            {
                case WIDX_PEOPLE_STARTING_POSITION:
                    SetPeepSpawnToolDown(screenCoords);
                    break;
            }
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_PEOPLE_STARTING_POSITION:
                    invalidate();
                    HideGridlines();
                    HideLandRights();
                    HideConstructionRights();
                    break;
            }
        }

        void SetPeepSpawnToolUpdate(const ScreenCoordsXY& screenCoords)
        {
            int32_t direction;
            TileElement* tileElement;
            gMapSelectFlags.unset(MapSelectFlag::enable, MapSelectFlag::enableArrow);
            auto mapCoords = FootpathBridgeGetInfoFromPos(screenCoords, &direction, &tileElement);
            if (mapCoords.IsNull())
                return;

            int32_t mapZ = tileElement->GetBaseZ();
            if (tileElement->GetType() == TileElementType::Surface)
            {
                if ((tileElement->AsSurface()->GetSlope() & kTileSlopeRaisedCornersMask) != 0)
                    mapZ += 16;
                if (tileElement->AsSurface()->GetSlope() & kTileSlopeDiagonalFlag)
                    mapZ += 16;
            }

            gMapSelectFlags.set(MapSelectFlag::enable, MapSelectFlag::enableArrow);
            gMapSelectType = MapSelectType::full;
            gMapSelectPositionA = mapCoords;
            gMapSelectPositionB = mapCoords;
            gMapSelectArrowPosition = CoordsXYZ{ mapCoords, mapZ };
            gMapSelectArrowDirection = DirectionReverse(direction);
        }

        void SetPeepSpawnToolDown(const ScreenCoordsXY& screenCoords)
        {
            // Verify footpath exists at location, and retrieve coordinates
            TileElement* tileElement;
            int32_t direction;
            auto mapCoords = FootpathGetCoordinatesFromPos(screenCoords, &direction, &tileElement);
            if (mapCoords.IsNull())
                return;

            int32_t mapZ = tileElement->GetBaseZ();

            auto gameAction = GameActions::PeepSpawnPlaceAction({ mapCoords, mapZ, static_cast<Direction>(direction) });
            auto result = GameActions::Execute(&gameAction, getGameState());
            if (result.error == GameActions::Status::ok)
            {
                Audio::Play3D(Audio::SoundId::placeItem, result.position);
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (text.empty())
                return;

            switch (widgetIndex)
            {
                case WIDX_MAP_SIZE_SPINNER_Y:
                case WIDX_MAP_SIZE_SPINNER_X:
                {
                    char* end;
                    std::string textStr = std::string(text);
                    int32_t size = strtol(textStr.c_str(), &end, 10);
                    if (*end == '\0')
                    {
                        // The practical size is 2 lower than the technical size
                        size += 2;
                        size = std::clamp(
                            size, static_cast<int>(kMinimumMapSizeTechnical), static_cast<int>(kMaximumMapSizeTechnical));

                        TileCoordsXY newMapSize = getGameState().mapSize;
                        if (_resizeDirection != ResizeDirection::X)
                            newMapSize.y = size;
                        if (_resizeDirection != ResizeDirection::Y)
                            newMapSize.x = size;

                        auto mapChangeSizeAction = GameActions::MapChangeSizeAction(newMapSize);
                        GameActions::Execute(&mapChangeSizeAction, getGameState());
                        invalidate();
                    }
                    break;
                }
            }
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            auto size = ScreenSize(getMiniMapWidth(), getMiniMapWidth());

            // Adjust for hidden scrollbars if needed
            auto& mapArea = widgets[WIDX_MAP];
            if (size.width >= mapArea.width() - 1)
                size.width -= kScrollBarWidth;
            if (size.height >= mapArea.height() - 1)
                size.height -= kScrollBarWidth;

            return size;
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // Adjust coordinates for any map offset to centre
            auto adjCoords = screenCoords;
            auto mapOffset = getMapOffset(width);
            if (mapOffset > 0)
            {
                adjCoords -= ScreenCoordsXY(mapOffset, mapOffset - kScrollBarWidth);
            }

            CoordsXY c = ScreenToMap(adjCoords);
            auto mapCoords = CoordsXY{ std::clamp(c.x, 0, getPracticalMapSizeBig() - 1),
                                       std::clamp(c.y, 0, getPracticalMapSizeBig() - 1) };
            auto mapZ = TileElementHeight(mapCoords);

            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow != nullptr)
            {
                WindowScrollToLocation(*mainWindow, { mapCoords, mapZ });
            }
        }

        void onScrollMouseDrag(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            onScrollMouseDown(scrollIndex, screenCoords);
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            GfxClear(rt, PaletteIndex::pi10);

            // Ensure small maps are centred
            auto screenOffset = ScreenCoordsXY(0, 0);
            auto mapOffset = getMapOffset(width);
            if (mapOffset > 0)
                screenOffset += ScreenCoordsXY(mapOffset, mapOffset - kScrollBarWidth);

            G1Element g1temp = {};
            g1temp.offset = reinterpret_cast<uint8_t*>(_mapImageData.data());
            g1temp.width = getMiniMapWidth();
            g1temp.height = getMiniMapWidth();
            GfxSetG1Element(SPR_TEMP, &g1temp);
            DrawingEngineInvalidateImage(SPR_TEMP);
            GfxDrawSprite(rt, ImageId(SPR_TEMP), screenOffset);

            if (selectedTab == PAGE_PEEPS)
            {
                PaintPeepOverlay(rt, screenOffset);
            }
            else
            {
                PaintTrainOverlay(rt, screenOffset);
            }
            PaintHudRectangle(rt, screenOffset);
        }

        void onPrepareDraw() override
        {
            auto* windowMgr = GetWindowManager();

            // Set the pressed widgets
            pressedWidgets = 0;
            setWidgetPressed(WIDX_MAP_SIZE_LINK, _mapWidthAndHeightLinked);
            pressedWidgets |= (1uLL << (WIDX_PEOPLE_TAB + selectedTab));

            if (windowMgr->FindByClass(WindowClass::editorParkEntrance))
                pressedWidgets |= (1uLL << WIDX_BUILD_PARK_ENTRANCE);

            if (windowMgr->FindByClass(WindowClass::landRights))
                pressedWidgets |= (1uLL << WIDX_SET_LAND_RIGHTS);

            if (windowMgr->FindByClass(WindowClass::mapgen))
                pressedWidgets |= (1uLL << WIDX_MAP_GENERATOR);

            // Set disabled widgets
            auto& gameState = getGameState();
            setWidgetDisabled(WIDX_MAP_SIZE_LINK, gameState.mapSize.x != gameState.mapSize.y);

            // Resize widgets to window size
            ResizeMiniMap();

            widgets[WIDX_MAP_SIZE_SPINNER_Y].top = height - 15;
            widgets[WIDX_MAP_SIZE_SPINNER_Y].bottom = height - 4;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_UP].top = height - 14;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_UP].bottom = height - 5;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_DOWN].top = height - 14;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_DOWN].bottom = height - 5;
            widgets[WIDX_MAP_SIZE_LINK].top = height - 15;
            widgets[WIDX_MAP_SIZE_LINK].bottom = height - 4;
            widgets[WIDX_MAP_SIZE_SPINNER_X].top = height - 15;
            widgets[WIDX_MAP_SIZE_SPINNER_X].bottom = height - 4;
            widgets[WIDX_MAP_SIZE_SPINNER_X_UP].top = height - 14;
            widgets[WIDX_MAP_SIZE_SPINNER_X_UP].bottom = height - 5;
            widgets[WIDX_MAP_SIZE_SPINNER_X_DOWN].top = height - 14;
            widgets[WIDX_MAP_SIZE_SPINNER_X_DOWN].bottom = height - 5;

            widgets[WIDX_SET_LAND_RIGHTS].left = width - 26;
            widgets[WIDX_SET_LAND_RIGHTS].right = width - 2;
            widgets[WIDX_BUILD_PARK_ENTRANCE].left = width - 26;
            widgets[WIDX_BUILD_PARK_ENTRANCE].right = width - 2;
            widgets[WIDX_PEOPLE_STARTING_POSITION].left = width - 26;
            widgets[WIDX_PEOPLE_STARTING_POSITION].right = width - 2;
            widgets[WIDX_MAP_GENERATOR].left = width - 26;
            widgets[WIDX_MAP_GENERATOR].right = width - 2;

            // Disable all scenario editor related widgets
            for (int32_t i = WIDX_MAP_SIZE_SPINNER_Y; i <= WIDX_MAP_GENERATOR; i++)
            {
                widgets[i].type = WidgetType::empty;
            }

            if (isEditorOrSandbox())
            {
                ShowDefaultScenarioEditorButtons();
            }
            if (_recalculateScrollbars)
            {
                widgetScrollUpdateThumbs(*this, WIDX_MAP);
                _recalculateScrollbars = false;
            }
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            DrawTabImages(rt);

            if (!isEditorOrSandbox())
            {
                // Render the map legend
                if (selectedTab == PAGE_RIDES)
                {
                    auto screenCoords = windowPos + ScreenCoordsXY{ 4, widgets[WIDX_MAP].bottom + 2 };

                    static_assert(std::size(kRideKeyColours) == std::size(MapLabels));

                    for (uint32_t i = 0; i < std::size(kRideKeyColours); i++)
                    {
                        Rectangle::fill(
                            rt, { screenCoords + ScreenCoordsXY{ 0, 2 }, screenCoords + ScreenCoordsXY{ 6, 8 } },
                            kRideKeyColours[i].b);
                        DrawTextBasic(rt, screenCoords + ScreenCoordsXY{ kListRowHeight, 0 }, MapLabels[i], {});
                        screenCoords.y += kListRowHeight;
                        if (i == 3)
                        {
                            screenCoords += { _firstColumnWidth, -(kListRowHeight * 4) };
                        }
                    }
                }
            }
            else if (!isToolActive(*this, WIDX_SET_LAND_RIGHTS))
            {
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ 4, widgets[WIDX_MAP_SIZE_SPINNER_Y].top + 1 }, STR_MAP_SIZE, {},
                    { colours[1] });
            }
        }

        void onLanguageChange() override
        {
            CalculateTextLayout();
        }

        void ResetMap()
        {
            initMap();
            CentreMapOnViewPoint();
        }

    private:
        void initMap()
        {
            _mapImageData.resize(getMiniMapWidth() * getMiniMapWidth());
            std::fill(_mapImageData.begin(), _mapImageData.end(), PaletteIndex::pi10);
            _currentLine = 0;
        }

        void CentreMapOnViewPoint()
        {
            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow == nullptr)
                return;

            Viewport* vp = mainWindow->viewport;
            if (vp == nullptr)
                return;

            // calculate centre view point of viewport and transform it to minimap coordinates
            auto centreX = ((vp->ViewWidth() / 2) + vp->viewPos.x) / kCoordsXYStep;
            auto centreY = ((vp->ViewHeight() / 2) + vp->viewPos.y) / kCoordsXYHalfTile;

            auto& offset = MiniMapOffsetFactors[GetCurrentRotation()];
            centreX += offset.x * getPracticalMapSize();
            centreY += offset.y * getPracticalMapSize();

            // calculate width and height of minimap
            auto& widget = widgets[WIDX_MAP];
            auto mapWidth = widget.width() - 1 - kScrollBarWidth - 1;
            auto mapHeight = widget.height() - 1 - kScrollBarWidth - 1;

            centreX = std::max(centreX - (mapWidth >> 1), 0);
            centreY = std::max(centreY - (mapHeight >> 1), 0);

            mapWidth = scrolls[0].contentWidth - mapWidth;
            mapHeight = scrolls[0].contentHeight - mapHeight;

            if (mapWidth < 0 && (mapWidth - centreX) < 0)
                centreX = 0;

            if (mapHeight < 0 && (mapHeight - centreY) < 0)
                centreY = 0;

            scrolls[0].contentOffsetX = centreX;
            scrolls[0].contentOffsetY = centreY;
            widgetScrollUpdateThumbs(*this, WIDX_MAP);
        }

        void IncreaseMapSize()
        {
            auto newMapSize = getGameState().mapSize;
            if (isWidgetPressed(WIDX_MAP_SIZE_LINK) || _resizeDirection == ResizeDirection::Y)
                newMapSize.y++;
            if (isWidgetPressed(WIDX_MAP_SIZE_LINK) || _resizeDirection == ResizeDirection::X)
                newMapSize.x++;

            auto increaseMapSizeAction = GameActions::MapChangeSizeAction(newMapSize);
            GameActions::Execute(&increaseMapSizeAction, getGameState());
        }

        void DecreaseMapSize()
        {
            auto newMapSize = getGameState().mapSize;
            if (isWidgetPressed(WIDX_MAP_SIZE_LINK) || _resizeDirection == ResizeDirection::Y)
                newMapSize.y--;
            if (isWidgetPressed(WIDX_MAP_SIZE_LINK) || _resizeDirection == ResizeDirection::X)
                newMapSize.x--;

            auto decreaseMapSizeAction = GameActions::MapChangeSizeAction(newMapSize);
            GameActions::Execute(&decreaseMapSizeAction, getGameState());
        }

        void SetMapPixels()
        {
            int32_t x = 0, y = 0, dx = 0, dy = 0;

            int32_t pos = (_currentLine * (getMiniMapWidth() - 1)) + getPracticalMapSize() - 1;
            auto destinationPosition = ScreenCoordsXY{ pos % getMiniMapWidth(), pos / getMiniMapWidth() };
            auto destination = _mapImageData.data() + (destinationPosition.y * getMiniMapWidth()) + destinationPosition.x;
            switch (GetCurrentRotation())
            {
                case 0:
                    x = _currentLine * kCoordsXYStep;
                    y = 0;
                    dx = 0;
                    dy = kCoordsXYStep;
                    break;
                case 1:
                    x = getMaxTileStartXY();
                    y = _currentLine * kCoordsXYStep;
                    dx = -kCoordsXYStep;
                    dy = 0;
                    break;
                case 2:
                    x = getPracticalMapSizeBig() - ((_currentLine + 1) * kCoordsXYStep);
                    y = getMaxTileStartXY();
                    dx = 0;
                    dy = -kCoordsXYStep;
                    break;
                case 3:
                    x = 0;
                    y = getPracticalMapSizeBig() - ((_currentLine + 1) * kCoordsXYStep);
                    dx = kCoordsXYStep;
                    dy = 0;
                    break;
            }

            for (int32_t i = 0; i < getPracticalMapSize(); i++)
            {
                if (!MapIsEdge({ x, y }))
                {
                    ColourPair colour{};
                    switch (selectedTab)
                    {
                        case PAGE_PEEPS:
                            colour = GetPixelColourPeep({ x, y });
                            break;
                        case PAGE_RIDES:
                            colour = GetPixelColourRide({ x, y });
                            break;
                    }
                    destination[0] = colour.a;
                    destination[1] = colour.b;
                }
                x += dx;
                y += dy;

                destinationPosition.x++;
                destinationPosition.y++;
                destination = _mapImageData.data() + (destinationPosition.y * getMiniMapWidth()) + destinationPosition.x;
            }
            _currentLine++;
            if (_currentLine >= static_cast<uint32_t>(getPracticalMapSize()))
                _currentLine = 0;
        }

        ColourPair GetPixelColourPeep(const CoordsXY& c)
        {
            auto* surfaceElement = MapGetSurfaceElementAt(c);
            if (surfaceElement == nullptr)
                return { PaletteIndex::transparent, PaletteIndex::transparent };

            auto colour = ColourPair(PaletteIndex::transparent);
            const auto* surfaceObject = surfaceElement->GetSurfaceObject();
            if (surfaceObject != nullptr)
                colour = ColourPair(surfaceObject->MapColours[0], surfaceObject->MapColours[1]);

            if (surfaceElement->GetWaterHeight() > 0)
                colour = kWaterColour;

            if (!(surfaceElement->GetOwnership() & OWNERSHIP_OWNED))
                colour = MapColourUnowned(colour);

            const int32_t maxSupportedTileElementType = static_cast<int32_t>(std::size(kElementTypeOverwriteColour));
            auto tileElement = reinterpret_cast<TileElement*>(surfaceElement);
            while (!(tileElement++)->IsLastForTile())
            {
                if (tileElement->IsGhost())
                {
                    colour = ColourPair(PaletteIndex::pi21);
                    break;
                }

                auto tileElementType = tileElement->GetType();
                if (EnumValue(tileElementType) >= maxSupportedTileElementType)
                {
                    tileElementType = TileElementType::Surface;
                }
                const auto overwriteColours = kElementTypeOverwriteColour[EnumValue(tileElementType)];
                if (overwriteColours.a != PaletteIndex::transparent)
                {
                    colour.a = overwriteColours.a;
                }
                if (overwriteColours.b != PaletteIndex::transparent)
                {
                    colour.b = overwriteColours.b;
                }
            }

            return colour;
        }

        ColourPair GetPixelColourRide(const CoordsXY& c)
        {
            ColourPair colourA{};                                // highlight colour
            ColourPair colourB = ColourPair(PaletteIndex::pi13); // surface colour (dark grey)

            // as an improvement we could use first_element to show underground stuff?
            TileElement* tileElement = reinterpret_cast<TileElement*>(MapGetSurfaceElementAt(c));
            do
            {
                if (tileElement == nullptr)
                    break;

                if (tileElement->IsGhost())
                {
                    colourA = ColourPair(PaletteIndex::pi21);
                    break;
                }

                switch (tileElement->GetType())
                {
                    case TileElementType::Surface:
                        if (tileElement->AsSurface()->GetWaterHeight() > 0)
                            // Why is this a different water colour as above (195)?
                            colourB = ColourPair(PaletteIndex::pi194);
                        if (!(tileElement->AsSurface()->GetOwnership() & OWNERSHIP_OWNED))
                            colourB = MapColourUnowned(colourB);
                        break;
                    case TileElementType::Path:
                        colourA = ColourPair(PaletteIndex::pi14); // lighter grey
                        break;
                    case TileElementType::Entrance:
                    {
                        if (tileElement->AsEntrance()->GetEntranceType() == ENTRANCE_TYPE_PARK_ENTRANCE)
                            break;
                        Ride* targetRide = GetRide(tileElement->AsEntrance()->GetRideIndex());
                        if (targetRide != nullptr)
                        {
                            const auto& colourKey = targetRide->getRideTypeDescriptor().ColourKey;
                            colourA = kRideKeyColours[EnumValue(colourKey)];
                        }
                        break;
                    }
                    case TileElementType::Track:
                    {
                        Ride* targetRide = GetRide(tileElement->AsTrack()->GetRideIndex());
                        if (targetRide != nullptr)
                        {
                            const auto& colourKey = targetRide->getRideTypeDescriptor().ColourKey;
                            colourA = kRideKeyColours[EnumValue(colourKey)];
                        }

                        break;
                    }
                    default:
                        break;
                }
            } while (!(tileElement++)->IsLastForTile());

            if (colourA != ColourPair(PaletteIndex::transparent, PaletteIndex::transparent))
                return colourA;

            return colourB;
        }

        void PaintPeepOverlay(RenderTarget& rt, const ScreenCoordsXY& offset)
        {
            auto flashColour = GetGuestFlashColour();
            for (auto guest : EntityList<Guest>())
            {
                DrawMapPeepPixel(guest, flashColour, rt, offset);
            }
            flashColour = GetStaffFlashColour();
            for (auto staff : EntityList<Staff>())
            {
                DrawMapPeepPixel(staff, flashColour, rt, offset);
            }
        }

        void DrawMapPeepPixel(Peep* peep, const PaletteIndex flashColour, RenderTarget& rt, const ScreenCoordsXY& offset)
        {
            if (peep->x == kLocationNull)
                return;

            MapCoordsXY c = TransformToMapCoords({ peep->x, peep->y });
            auto leftTop = ScreenCoordsXY{ c.x, c.y } + offset;
            auto rightBottom = leftTop;
            auto colour = DefaultPeepMapColour;
            if (getGameState().entities.EntityGetFlashing(peep))
            {
                colour = flashColour;
                // If flashing then map peep pixel size is increased (by moving left top downwards)
                if (flashColour != DefaultPeepMapColour)
                {
                    leftTop.x--;
                }
            }

            Rectangle::fill(rt, { leftTop, rightBottom }, colour);
        }

        PaletteIndex GetGuestFlashColour() const
        {
            auto colour = DefaultPeepMapColour;
            if ((_flashingFlags & MapFlashingFlags::FlashGuests) != 0)
            {
                colour = GuestMapColour;
                if ((_flashingFlags & MapFlashingFlags::SwitchColour) == 0)
                    colour = GuestMapColourAlternate;
            }
            return colour;
        }

        PaletteIndex GetStaffFlashColour() const
        {
            auto colour = DefaultPeepMapColour;
            if ((_flashingFlags & MapFlashingFlags::FlashStaff) != 0)
            {
                colour = StaffMapColour;
                if ((_flashingFlags & MapFlashingFlags::SwitchColour) == 0)
                    colour = StaffMapColourAlternate;
            }
            return colour;
        }

        void PaintTrainOverlay(RenderTarget& rt, const ScreenCoordsXY& offset)
        {
            for (auto train : TrainManager::View())
            {
                for (Vehicle* vehicle = train; vehicle != nullptr;
                     vehicle = getGameState().entities.GetEntity<Vehicle>(vehicle->next_vehicle_on_train))
                {
                    if (vehicle->x == kLocationNull)
                        continue;

                    auto mapCoord = TransformToMapCoords({ vehicle->x, vehicle->y });
                    auto pixelCoord = ScreenCoordsXY{ mapCoord.x, mapCoord.y } + offset;

                    Rectangle::fill(rt, { pixelCoord, pixelCoord }, PaletteIndex::pi171);
                }
            }
        }

        /**
         * The call to Rectangle::fill was originally wrapped in Sub68DABD which made sure that arguments were ordered
         * correctly, but it doesn't look like it's ever necessary here so the call was removed.
         */
        void PaintHudRectangle(RenderTarget& rt, const ScreenCoordsXY& widgetOffset)
        {
            WindowBase* mainWindow = WindowGetMain();
            if (mainWindow == nullptr)
                return;

            Viewport* mainViewport = mainWindow->viewport;
            if (mainViewport == nullptr)
                return;

            auto mapOffset = MiniMapOffsetFactors[GetCurrentRotation()];
            mapOffset.x *= getPracticalMapSize();
            mapOffset.y *= getPracticalMapSize();

            auto leftTop = widgetOffset + mapOffset
                + ScreenCoordsXY{ (mainViewport->viewPos.x / kCoordsXYStep), (mainViewport->viewPos.y / kCoordsXYHalfTile) };
            auto rightBottom = leftTop
                + ScreenCoordsXY{ mainViewport->ViewWidth() / kCoordsXYStep, mainViewport->ViewHeight() / kCoordsXYHalfTile };
            auto rightTop = ScreenCoordsXY{ rightBottom.x, leftTop.y };
            auto leftBottom = ScreenCoordsXY{ leftTop.x, rightBottom.y };

            // top horizontal lines
            Rectangle::fill(rt, { leftTop, leftTop + ScreenCoordsXY{ 3, 0 } }, PaletteIndex::yellow10);
            Rectangle::fill(rt, { rightTop - ScreenCoordsXY{ 3, 0 }, rightTop }, PaletteIndex::yellow10);

            // left vertical lines
            Rectangle::fill(rt, { leftTop, leftTop + ScreenCoordsXY{ 0, 3 } }, PaletteIndex::yellow10);
            Rectangle::fill(rt, { leftBottom - ScreenCoordsXY{ 0, 3 }, leftBottom }, PaletteIndex::yellow10);

            // bottom horizontal lines
            Rectangle::fill(rt, { leftBottom, leftBottom + ScreenCoordsXY{ 3, 0 } }, PaletteIndex::yellow10);
            Rectangle::fill(rt, { rightBottom - ScreenCoordsXY{ 3, 0 }, rightBottom }, PaletteIndex::yellow10);

            // right vertical lines
            Rectangle::fill(rt, { rightTop, rightTop + ScreenCoordsXY{ 0, 3 } }, PaletteIndex::yellow10);
            Rectangle::fill(rt, { rightBottom - ScreenCoordsXY{ 0, 3 }, rightBottom }, PaletteIndex::yellow10);
        }

        void DrawTabImages(RenderTarget& rt)
        {
            // Guest tab image (animated)
            uint32_t guestTabImage = SPR_TAB_GUESTS_0;
            if (selectedTab == PAGE_PEEPS)
                guestTabImage += listInformationType / 4;

            GfxDrawSprite(
                rt, ImageId(guestTabImage),
                windowPos + ScreenCoordsXY{ widgets[WIDX_PEOPLE_TAB].left, widgets[WIDX_PEOPLE_TAB].top });

            // Ride/stall tab image (animated)
            uint32_t rideTabImage = SPR_TAB_RIDE_0;
            if (selectedTab == PAGE_RIDES)
                rideTabImage += listInformationType / 4;

            GfxDrawSprite(
                rt, ImageId(rideTabImage),
                windowPos + ScreenCoordsXY{ widgets[WIDX_RIDES_TAB].left, widgets[WIDX_RIDES_TAB].top });
        }

        void ShowDefaultScenarioEditorButtons()
        {
            widgets[WIDX_SET_LAND_RIGHTS].type = WidgetType::flatBtn;
            widgets[WIDX_BUILD_PARK_ENTRANCE].type = WidgetType::flatBtn;
            widgets[WIDX_PEOPLE_STARTING_POSITION].type = WidgetType::flatBtn;

            // only show this in the scenario editor, even when in sandbox mode.
            if (gLegacyScene == LegacyScene::scenarioEditor)
                widgets[WIDX_MAP_GENERATOR].type = WidgetType::flatBtn;

            widgets[WIDX_MAP_SIZE_SPINNER_Y].type = WidgetType::spinner;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_UP].type = WidgetType::button;
            widgets[WIDX_MAP_SIZE_SPINNER_Y_DOWN].type = WidgetType::button;
            widgets[WIDX_MAP_SIZE_LINK].type = WidgetType::flatBtn;
            widgets[WIDX_MAP_SIZE_SPINNER_X].type = WidgetType::spinner;
            widgets[WIDX_MAP_SIZE_SPINNER_X_UP].type = WidgetType::button;
            widgets[WIDX_MAP_SIZE_SPINNER_X_DOWN].type = WidgetType::button;

            // Push width (Y) and height (X) to the common formatter arguments for the map size spinners to use
            auto& gameState = getGameState();
            auto ft = Formatter::Common();
            ft.Add<uint16_t>(gameState.mapSize.y - 2);
            ft.Add<uint16_t>(gameState.mapSize.x - 2);
        }

        void InputMapSize(WidgetIndex callingWidget)
        {
            if (isWidgetPressed(WIDX_MAP_SIZE_LINK))
                _resizeDirection = ResizeDirection::Both;
            else
                _resizeDirection = (callingWidget == WIDX_MAP_SIZE_SPINNER_Y) ? ResizeDirection::Y : ResizeDirection::X;

            Formatter ft;
            ft.Add<int16_t>(kMinimumMapSizePractical);
            ft.Add<int16_t>(kMaximumMapSizePractical);
            textInputOpen(callingWidget, STR_MAP_SIZE_2, STR_ENTER_MAP_SIZE, ft, kStringIdNone, kStringIdNone, 4);
        }

        CoordsXY ScreenToMap(ScreenCoordsXY screenCoords)
        {
            screenCoords.x = ((screenCoords.x + 8) - getPracticalMapSize()) / 2;
            screenCoords.y = ((screenCoords.y + 8)) / 2;
            auto location = TileCoordsXY(screenCoords.y - screenCoords.x, screenCoords.x + screenCoords.y).ToCoordsXY();

            switch (GetCurrentRotation())
            {
                case 0:
                    return location;
                case 1:
                    return { getPracticalMapSizeBig() - 1 - location.y, location.x };
                case 2:
                    return { getPracticalMapSizeBig() - 1 - location.x, getPracticalMapSizeBig() - 1 - location.y };
                case 3:
                    return { location.y, getPracticalMapSizeBig() - 1 - location.x };
            }

            return { 0, 0 }; // unreachable
        }

        MapCoordsXY TransformToMapCoords(CoordsXY c)
        {
            int32_t x = c.x, y = c.y;

            switch (GetCurrentRotation())
            {
                case 3:
                    std::swap(x, y);
                    x = getPracticalMapSizeBig() - 1 - x;
                    break;
                case 2:
                    x = getPracticalMapSizeBig() - 1 - x;
                    y = getPracticalMapSizeBig() - 1 - y;
                    break;
                case 1:
                    std::swap(x, y);
                    y = getPracticalMapSizeBig() - 1 - y;
                    break;
                case 0:
                    break;
            }
            x /= 32;
            y /= 32;

            return { -x + y + getPracticalMapSize(), x + y };
        }

        uint16_t GetReservedBottomSpace()
        {
            if (isEditorOrSandbox())
                return kEditorReservedVSpace;
            else if (selectedTab == PAGE_RIDES)
                return kRidesTabReservedVSpace;
            else
                return kDefaultReservedVSpace;
        }

        void SetInitialWindowDimensions()
        {
            // The initial mini map size should be able to show a reasonably sized map
            auto initWidth = std::clamp(getPracticalMapSize(), 100, 254) * 2;
            width = initWidth + GetReservedRightSpace();

            auto initHeight = std::clamp(getMiniMapWidth(), 100, 254) * 2;
            height = initHeight + kReservedTopSpace + GetReservedBottomSpace();

            width += getPracticalMapSize() > initWidth ? kScrollBarWidth : 2;
            height += getMiniMapWidth() > initHeight ? kScrollBarWidth : 2;

            auto maxWindowHeight = ContextGetHeight() - 68;
            width = std::min<int16_t>(width, ContextGetWidth());
            height = std::min<int16_t>(height, maxWindowHeight);

            _adjustedForSandboxMode = isEditorOrSandbox();

            resizeFrame();
        }

        void ResetMaxWindowDimensions()
        {
            auto newMaxWidth = std::clamp(getMiniMapWidth() + GetReservedRightSpace(), kWindowSize.width, ContextGetWidth());
            auto newMaxHeight = std::clamp(
                getMiniMapWidth() + kReservedTopSpace + GetReservedBottomSpace(), kWindowSize.height, ContextGetHeight() - 68);

            auto scrollbarSize = getMiniMapWidth() + GetReservedRightSpace() > ContextGetWidth() ? kScrollBarWidth : 2;
            newMaxWidth += scrollbarSize;
            newMaxHeight += scrollbarSize;

            WindowSetResize(*this, kWindowSize, { newMaxWidth, newMaxHeight });
        }

        void ResizeMiniMap()
        {
            widgets[WIDX_MAP].right = width - GetReservedRightSpace();
            widgets[WIDX_MAP].bottom = height - 1 - GetReservedBottomSpace();
        }

        void CalculateTextLayout()
        {
            int32_t textOffset = 4 + kListRowHeight;
            _firstColumnWidth = 118;
            for (uint32_t i = 0; i < 4; i++)
            {
                const auto* labelStr = LanguageGetString(MapLabels[i]);
                _firstColumnWidth = std::max(textOffset + GfxGetStringWidth(labelStr, FontStyle::medium), _firstColumnWidth);
            }

            textOffset += _firstColumnWidth + 4;
            minWidth = kWindowSize.width;
            for (uint32_t i = 4; i < std::size(MapLabels); i++)
            {
                const auto* labelStr = LanguageGetString(MapLabels[i]);
                minWidth = std::max(
                    static_cast<int16_t>(textOffset + GfxGetStringWidth(labelStr, FontStyle::medium)), minWidth);
            }
            width = std::max(minWidth, width);
            _recalculateScrollbars = true;
        }
    };

    WindowBase* MapOpen()
    {
        try
        {
            auto* windowMgr = GetWindowManager();
            auto* w = windowMgr->FocusOrCreate<MapWindow>(WindowClass::map, kWindowSize, WindowFlag::higherContrastOnPress);
            w->selectedTab = 0;
            w->listInformationType = 0;
            return w;
        }
        catch (const std::bad_alloc&)
        {
            return nullptr;
        }
    }

    void WindowMapReset()
    {
        // Check if window is even opened
        auto* windowMgr = GetWindowManager();
        auto* w = windowMgr->BringToFrontByClass(WindowClass::map);
        if (w == nullptr)
        {
            return;
        }

        auto* mapWindow = static_cast<MapWindow*>(w);
        mapWindow->ResetMap();
    }
} // namespace OpenRCT2::Ui::Windows
