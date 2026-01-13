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
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/ParkEntrancePlaceAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/object/EntranceObject.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/tile_element/EntranceElement.h>
#include <openrct2/world/tile_element/PathElement.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SurfaceElement.h>

using namespace OpenRCT2::Drawing;
using OpenRCT2::GameActions::CommandFlag;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_OBJECT_SELECTION_PARK_ENTRANCE;
    static constexpr int32_t kImageSize = 116;
    static constexpr int32_t kNumColumns = 4;
    static constexpr int32_t kNumRows = 1;
    static constexpr int32_t kScrollPadding = 2;
    static constexpr int32_t kScrollWidth = (kImageSize * kNumColumns) + kScrollBarWidth + 4;
    static constexpr int32_t kScrollHeight = (kImageSize * kNumRows);
    static constexpr ScreenSize kWindowSize = { kScrollWidth + 28, kScrollHeight + 51 };
    static bool _placingEntrance = false;

    struct EntranceSelection
    {
        ObjectEntryIndex entryIndex = kObjectEntryIndexNull;
        StringId stringId = kStringIdNone;
        ImageIndex imageId = kImageIndexUndefined;
    };

    enum WindowEditorParkEntranceListWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_CONTENT_PANEL,
        WIDX_TAB,
        WIDX_LIST,
        WIDX_ROTATE_ENTRANCE_BUTTON,
    };

    VALIDATE_GLOBAL_WIDX(WC_EDITOR_PARK_ENTRANCE, WIDX_ROTATE_ENTRANCE_BUTTON);

    // clang-format off
    static constexpr auto _widgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget     ({                      0, 43 }, { kWindowSize.width, kWindowSize.height - 43 }, WidgetType::resize,  WindowColour::secondary                                                   ),
        makeTab        ({                      3, 17 },                                                                                               kStringIdNone                                    ),
        makeWidget     ({                      2, 45 }, {      kScrollWidth,           kScrollHeight }, WidgetType::scroll,  WindowColour::secondary, SCROLL_VERTICAL                                  ),
        makeWidget     ({ kWindowSize.width - 26, 59 }, {                24,                      24 }, WidgetType::flatBtn, WindowColour::secondary, ImageId(SPR_ROTATE_ARROW), STR_ROTATE_OBJECTS_90 )
    );
    // clang-format on

    class EditorParkEntrance final : public Window
    {
    private:
        ObjectEntryIndex _selectedEntranceType = 0;
        ObjectEntryIndex _highlightedEntranceType = 0;
        std::vector<EntranceSelection> _entranceTypes{};

        void initParkEntranceItems()
        {
            _entranceTypes.clear();
            for (ObjectEntryIndex objectIndex = 0; objectIndex < kMaxParkEntranceObjects; objectIndex++)
            {
                auto& objManager = GetContext()->GetObjectManager();
                auto* object = objManager.GetLoadedObject<EntranceObject>(objectIndex);
                if (object != nullptr)
                {
                    const auto* legacyData = reinterpret_cast<const EntranceEntry*>(object->GetLegacyData());
                    _entranceTypes.push_back({ objectIndex, legacyData->string_idx, legacyData->image_id });
                }
            }
        }

        size_t GetNumRows()
        {
            auto numRows = _entranceTypes.size() / kNumColumns;
            if (_entranceTypes.size() % kNumColumns > 0)
                numRows++;

            return numRows;
        }

        void PaintPreview(RenderTarget& rt, ImageIndex imageStart, ScreenCoordsXY screenCoords, Direction direction)
        {
            imageStart += (direction * 3);

            switch (direction)
            {
                case 0:
                    GfxDrawSprite(rt, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ -32, 14 });
                    GfxDrawSprite(rt, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(rt, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ 32, 44 });
                    break;
                case 1:
                    GfxDrawSprite(rt, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ 32, 14 });
                    GfxDrawSprite(rt, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(rt, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ -32, 44 });
                    break;
                case 2:
                    GfxDrawSprite(rt, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ -32, 14 });
                    GfxDrawSprite(rt, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(rt, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ 32, 44 });
                    break;
                case 3:
                    GfxDrawSprite(rt, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ 32, 14 });
                    GfxDrawSprite(rt, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(rt, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ -32, 44 });
                    break;
            }
        }

        CoordsXYZD PlaceParkEntranceGetMapPosition(const ScreenCoordsXY& screenCoords)
        {
            CoordsXYZD parkEntranceMapPosition{ 0, 0, 0, kInvalidDirection };
            const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
            parkEntranceMapPosition = { mapCoords.x, mapCoords.y, 0, kInvalidDirection };
            if (parkEntranceMapPosition.IsNull())
                return parkEntranceMapPosition;

            auto surfaceElement = MapGetSurfaceElementAt(mapCoords);
            if (surfaceElement == nullptr)
            {
                parkEntranceMapPosition.SetNull();
                return parkEntranceMapPosition;
            }

            parkEntranceMapPosition.z = surfaceElement->GetWaterHeight();
            if (parkEntranceMapPosition.z == 0)
            {
                parkEntranceMapPosition.z = surfaceElement->GetBaseZ();
                if ((surfaceElement->GetSlope() & kTileSlopeRaisedCornersMask) != 0)
                {
                    parkEntranceMapPosition.z += 16;
                    if (surfaceElement->GetSlope() & kTileSlopeDiagonalFlag)
                    {
                        parkEntranceMapPosition.z += 16;
                    }
                }
            }
            parkEntranceMapPosition.direction = (gWindowSceneryRotation - GetCurrentRotation()) & 3;
            return parkEntranceMapPosition;
        }

        void PlaceParkEntranceToolUpdate(const ScreenCoordsXY& screenCoords)
        {
            if (_placingEntrance)
            {
                return;
            }

            gMapSelectFlags.unset(MapSelectFlag::enable, MapSelectFlag::enableArrow, MapSelectFlag::enableConstruct);
            CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
            if (parkEntrancePosition.IsNull())
            {
                ParkEntranceRemoveGhost();
                return;
            }

            int32_t sideDirection = (parkEntrancePosition.direction + 1) & 3;
            MapSelection::clearSelectedTiles();
            MapSelection::addSelectedTile({ parkEntrancePosition.x, parkEntrancePosition.y });
            MapSelection::addSelectedTile(
                { parkEntrancePosition.x + CoordsDirectionDelta[sideDirection].x,
                  parkEntrancePosition.y + CoordsDirectionDelta[sideDirection].y });
            MapSelection::addSelectedTile(
                { parkEntrancePosition.x - CoordsDirectionDelta[sideDirection].x,
                  parkEntrancePosition.y - CoordsDirectionDelta[sideDirection].y });

            gMapSelectArrowPosition = parkEntrancePosition;
            gMapSelectArrowDirection = parkEntrancePosition.direction;

            gMapSelectFlags.set(MapSelectFlag::enableConstruct, MapSelectFlag::enableArrow);
            if (gParkEntranceGhostExists && parkEntrancePosition == gParkEntranceGhostPosition)
            {
                return;
            }

            ParkEntranceRemoveGhost();

            bool isLegacyPath = (gFootpathSelection.legacyPath != kObjectEntryIndexNull);
            auto pathIndex = isLegacyPath ? gFootpathSelection.legacyPath : gFootpathSelection.normalSurface;
            auto gameAction = GameActions::ParkEntrancePlaceAction(
                parkEntrancePosition, pathIndex, _selectedEntranceType, isLegacyPath);
            gameAction.SetFlags({ CommandFlag::ghost });

            auto result = GameActions::Execute(&gameAction, getGameState());
            if (result.error == GameActions::Status::ok)
            {
                gParkEntranceGhostPosition = parkEntrancePosition;
                gParkEntranceGhostExists = true;
            }
        }

        void PlaceParkEntranceToolDown(const ScreenCoordsXY& screenCoords)
        {
            _placingEntrance = true;
            gMapSelectFlags.unset(MapSelectFlag::enable, MapSelectFlag::enableArrow, MapSelectFlag::enableConstruct);
            ParkEntranceRemoveGhost();

            CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
            if (!parkEntrancePosition.IsNull())
            {
                bool isLegacyPath = (gFootpathSelection.legacyPath != kObjectEntryIndexNull);
                auto pathIndex = isLegacyPath ? gFootpathSelection.legacyPath : gFootpathSelection.normalSurface;
                auto gameAction = GameActions::ParkEntrancePlaceAction(
                    parkEntrancePosition, pathIndex, _selectedEntranceType, isLegacyPath);
                gameAction.SetCallback(
                    [&](const GameActions::GameAction*, const GameActions::Result* result) { _placingEntrance = false; });
                auto result = GameActions::Execute(&gameAction, getGameState());
                if (result.error == GameActions::Status::ok)
                {
                    Audio::Play3D(Audio::SoundId::placeItem, result.position);
                }
            }
        }

        ObjectEntryIndex ScrollGetEntranceListItemAt(const ScreenCoordsXY& screenCoords)
        {
            if (screenCoords.x <= 0 || screenCoords.y <= 0)
                return kObjectEntryIndexNull;

            size_t column = screenCoords.x / kImageSize;
            size_t row = screenCoords.y / kImageSize;
            if (column >= 5)
                return kObjectEntryIndexNull;

            size_t index = column + (row * kNumColumns);
            if (index >= _entranceTypes.size())
                return kObjectEntryIndexNull;

            return _entranceTypes[index].entryIndex;
        }

    public:
        void onOpen() override
        {
            setWidgets(_widgets);

            initScrollWidgets();
            initParkEntranceItems();

            listInformationType = 0;

            auto newMaxHeight = static_cast<int16_t>(kWindowSize.height + kImageSize * (GetNumRows() - 1));
            WindowSetResize(*this, kWindowSize, { kWindowSize.width, newMaxHeight });

            pressedWidgets |= 1LL << WIDX_TAB;

            ToolSet(*this, WIDX_LIST, Tool::entranceDown);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_ROTATE_ENTRANCE_BUTTON:
                    gWindowSceneryRotation = DirectionNext(gWindowSceneryRotation);
                    invalidate();
                    break;
            }
        }

        void onClose() override
        {
            if (gCurrentToolWidget.windowClassification == classification)
                ToolCancel();
        }

        void onUpdate() override
        {
            if (gCurrentToolWidget.windowClassification != classification)
                close();
        }

        void onPrepareDraw() override
        {
            widgets[WIDX_LIST].right = width - 30;
            widgets[WIDX_LIST].bottom = height - 5;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);
            GfxDrawSprite(
                rt, ImageId(SPR_TAB_PARK_ENTRANCE),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB].left, widgets[WIDX_TAB].top });
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            GfxClear(rt, getColourMap(colours[1].colour).midLight);

            ScreenCoordsXY coords{ 1, 1 };

            for (auto& entranceType : _entranceTypes)
            {
                // Draw flat button rectangle
                auto borderStyle = Rectangle::BorderStyle::outset;
                auto fillBrightness = Rectangle::FillBrightness::light;
                if (_selectedEntranceType == entranceType.entryIndex)
                {
                    borderStyle = Rectangle::BorderStyle::inset;
                    fillBrightness = Rectangle::FillBrightness::dark;
                }
                else if (_highlightedEntranceType == entranceType.entryIndex)
                {
                    fillBrightness = Rectangle::FillBrightness::dark;
                }

                if (fillBrightness != Rectangle::FillBrightness::light)
                    Rectangle::fillInset(
                        rt, { coords, coords + ScreenCoordsXY{ kImageSize - 1, kImageSize - 1 } }, colours[1], borderStyle,
                        fillBrightness);

                RenderTarget clipRT;
                auto screenPos = coords + ScreenCoordsXY{ kScrollPadding, kScrollPadding };
                if (ClipRenderTarget(
                        clipRT, rt, screenPos, kImageSize - (2 * kScrollPadding), kImageSize - (2 * kScrollPadding)))
                {
                    PaintPreview(
                        clipRT, entranceType.imageId, ScreenCoordsXY{ kImageSize / 2, kImageSize / 2 }, gWindowSceneryRotation);
                }

                // Next position
                coords.x += kImageSize;
                if (coords.x >= kImageSize * kNumColumns + 1)
                {
                    coords.x = 1;
                    coords.y += kImageSize;
                }
            }
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            PlaceParkEntranceToolDown(screenCoords);
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            PlaceParkEntranceToolUpdate(screenCoords);
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            ParkEntranceRemoveGhost();
            invalidate();
            HideGridlines();
            HideLandRights();
            HideConstructionRights();
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            auto scrollHeight = static_cast<int32_t>(GetNumRows() * kImageSize);

            return ScreenSize(kImageSize * kNumColumns, scrollHeight);
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto highlighted = ScrollGetEntranceListItemAt(screenCoords);
            if (highlighted != kObjectEntryIndexNull)
            {
                _highlightedEntranceType = highlighted;
                invalidate();
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto selected = ScrollGetEntranceListItemAt(screenCoords);
            if (selected == kObjectEntryIndexNull)
            {
                return;
            }

            _selectedEntranceType = selected;

            Audio::Play(Audio::SoundId::click1, 0, windowPos.x + (width / 2));
            invalidate();
        }
    };

    WindowBase* EditorParkEntranceOpen()
    {
        // Check if window is already open
        auto* windowMgr = GetWindowManager();
        auto* window = windowMgr->BringToFrontByClass(WindowClass::editorParkEntrance);
        if (window != nullptr)
            return window;

        window = windowMgr->Create<EditorParkEntrance>(
            WindowClass::editorParkEntrance, kWindowSize, { WindowFlag::higherContrastOnPress, WindowFlag::resizable });

        _placingEntrance = false;
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
