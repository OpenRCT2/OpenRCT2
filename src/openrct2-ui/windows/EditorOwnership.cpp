/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/audio/audio.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/actions/ParkEntrancePlaceAction.h>
#include <openrct2/object/EntranceObject.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/sprites.h>
#include <openrct2/world/tile_element/Slope.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId _windowTitle = STR_OBJECT_SELECTION_PARK_ENTRANCE;
    static constexpr int32_t _imageSize = 116;
    static constexpr int32_t _numColumns = 4;
    static constexpr int32_t _numRows = 3;
    // static constexpr int32_t _scrollPadding = 2;
    static constexpr int32_t _scrollWidth = (_imageSize * _numColumns) + kScrollBarWidth + 4;
    static constexpr int32_t _scrollHeight = (_imageSize * _numRows);
    static constexpr int32_t _windowWidth = _scrollWidth + 21;
    static constexpr int32_t _windowHeight = _scrollHeight + 50;

    struct EntranceSelection
    {
        ObjectEntryIndex entryIndex = OBJECT_ENTRY_INDEX_NULL;
        StringId stringId = STR_NONE;
        ImageIndex imageId = SPR_NONE;
    };

    enum WindowEditorOwnershipListWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TAB_CONTENT_PANEL,
        WIDX_TAB,
        WIDX_LIST,
        WIDX_ROTATE_ENTRANCE_BUTTON,
    };

    validate_global_widx(WC_EDITOR_OWNERSHIP, WIDX_ROTATE_ENTRANCE_BUTTON);

    // clang-format off
    static Widget _widgets[] = {
        WINDOW_SHIM(_windowTitle, _windowWidth, _windowHeight),
        MakeWidget     ({                 0, 43 }, { _windowWidth, _windowHeight - 43 }, WindowWidgetType::Resize,  WindowColour::Secondary                                                   ),
        MakeTab        ({                 3, 17 },                                                                                           STR_NONE                                         ),
        MakeWidget     ({                 2, 45 }, { _scrollWidth, _scrollHeight      }, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                                  ),
        MakeWidget     ({ _windowWidth - 26, 59 }, {           24,            24      }, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_ROTATE_ARROW), STR_ROTATE_OBJECTS_90 ),
        kWidgetsEnd,
    };
    // clang-format on

    class EditorOwnership final : public Window
    {
    private:
        ObjectEntryIndex _selectedEntranceType = 0;
        // ObjectEntryIndex _highlightedEntranceType = 0;
        std::vector<EntranceSelection> _entranceTypes{};

        void InitParkEntranceItems()
        {
            _entranceTypes.clear();
            for (ObjectEntryIndex objectIndex = 0; objectIndex < OBJECT_ENTRY_INDEX_NULL; objectIndex++)
            {
                auto& objManager = GetContext()->GetObjectManager();
                auto* object = static_cast<EntranceObject*>(objManager.GetLoadedObject(ObjectType::ParkEntrance, objectIndex));
                if (object != nullptr)
                {
                    const auto* legacyData = reinterpret_cast<const EntranceEntry*>(object->GetLegacyData());
                    _entranceTypes.push_back({ objectIndex, legacyData->string_idx, legacyData->image_id });
                }
            }
        }

        void PaintPreview(DrawPixelInfo& dpi, ImageIndex imageStart, ScreenCoordsXY screenCoords, Direction direction)
        {
            imageStart += (direction * 3);

            switch (direction)
            {
                case 0:
                    GfxDrawSprite(dpi, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ -32, 14 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ 32, 44 });
                    break;
                case 1:
                    GfxDrawSprite(dpi, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ 32, 14 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ -32, 44 });
                    break;
                case 2:
                    GfxDrawSprite(dpi, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ -32, 14 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ 32, 44 });
                    break;
                case 3:
                    GfxDrawSprite(dpi, ImageId(imageStart + 2), screenCoords + ScreenCoordsXY{ 32, 14 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 0), screenCoords + ScreenCoordsXY{ 0, 28 });
                    GfxDrawSprite(dpi, ImageId(imageStart + 1), screenCoords + ScreenCoordsXY{ -32, 44 });
                    break;
            }
        }

        CoordsXYZD PlaceParkEntranceGetMapPosition(const ScreenCoordsXY& screenCoords)
        {
            CoordsXYZD parkEntranceMapPosition{ 0, 0, 0, INVALID_DIRECTION };
            const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
            parkEntranceMapPosition = { mapCoords.x, mapCoords.y, 0, INVALID_DIRECTION };
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
            MapInvalidateSelectionRect();
            MapInvalidateMapSelectionTiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
            if (parkEntrancePosition.IsNull())
            {
                ParkEntranceRemoveGhost();
                return;
            }

            int32_t sideDirection = (parkEntrancePosition.direction + 1) & 3;
            gMapSelectionTiles.clear();
            gMapSelectionTiles.push_back({ parkEntrancePosition.x, parkEntrancePosition.y });
            gMapSelectionTiles.push_back({ parkEntrancePosition.x + CoordsDirectionDelta[sideDirection].x,
                                           parkEntrancePosition.y + CoordsDirectionDelta[sideDirection].y });
            gMapSelectionTiles.push_back({ parkEntrancePosition.x - CoordsDirectionDelta[sideDirection].x,
                                           parkEntrancePosition.y - CoordsDirectionDelta[sideDirection].y });

            gMapSelectArrowPosition = parkEntrancePosition;
            gMapSelectArrowDirection = parkEntrancePosition.direction;

            gMapSelectFlags |= MAP_SELECT_FLAG_ENABLE_CONSTRUCT | MAP_SELECT_FLAG_ENABLE_ARROW;
            MapInvalidateMapSelectionTiles();
            if (gParkEntranceGhostExists && parkEntrancePosition == gParkEntranceGhostPosition)
            {
                return;
            }

            ParkEntranceRemoveGhost();

            auto gameAction = ParkEntrancePlaceAction(parkEntrancePosition, gFootpathSelectedId, _selectedEntranceType);
            gameAction.SetFlags(GAME_COMMAND_FLAG_GHOST);

            auto result = GameActions::Execute(&gameAction);
            if (result.Error == GameActions::Status::Ok)
            {
                gParkEntranceGhostPosition = parkEntrancePosition;
                gParkEntranceGhostExists = true;
            }
        }

        void PlaceParkEntranceToolDown(const ScreenCoordsXY& screenCoords)
        {
            ParkEntranceRemoveGhost();

            CoordsXYZD parkEntrancePosition = PlaceParkEntranceGetMapPosition(screenCoords);
            if (!parkEntrancePosition.IsNull())
            {
                auto gameAction = ParkEntrancePlaceAction(parkEntrancePosition, gFootpathSelectedId, _selectedEntranceType);
                auto result = GameActions::Execute(&gameAction);
                if (result.Error == GameActions::Status::Ok)
                {
                    Audio::Play3D(Audio::SoundId::PlaceItem, result.Position);
                }
            }
        }

        ObjectEntryIndex ScrollGetEntranceListItemAt(const ScreenCoordsXY& screenCoords)
        {
            if (screenCoords.x <= 0 || screenCoords.y <= 0)
                return OBJECT_ENTRY_INDEX_NULL;

            size_t column = screenCoords.x / 116;
            size_t row = screenCoords.y / 116;
            if (column >= 5)
                return OBJECT_ENTRY_INDEX_NULL;

            size_t index = column + (row * 5);
            if (index >= _entranceTypes.size())
                return OBJECT_ENTRY_INDEX_NULL;

            return _entranceTypes[index].entryIndex;
        }

    public:
        void OnOpen() override
        {
            widgets = _widgets;

            InitScrollWidgets();

            list_information_type = 0;
            min_width = _windowWidth;
            min_height = _windowHeight;
            max_width = _windowWidth;
            max_height = _windowHeight;

            InitParkEntranceItems();
            pressed_widgets |= 1LL << WIDX_TAB;
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_ROTATE_ENTRANCE_BUTTON:
                    gWindowSceneryRotation = DirectionNext(gWindowSceneryRotation);
                    Invalidate();
                    break;
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);
            GfxDrawSprite(
                dpi, ImageId(SPR_TAB_PARK_ENTRANCE),
                windowPos + ScreenCoordsXY{ widgets[WIDX_TAB].left, widgets[WIDX_TAB].top });
        }

        void OnScrollDraw(int32_t scrollIndex, DrawPixelInfo& dpi) override
        {
            GfxClear(dpi, ColourMapA[colours[1].colour].mid_light);

            ScreenCoordsXY coords{ 1, 1 };

            for (auto& entranceType : _entranceTypes)
            {
                // Draw flat button rectangle
                int32_t buttonFlags = 0;
                if (_selectedEntranceType == entranceType.entryIndex)
                    buttonFlags |= INSET_RECT_FLAG_BORDER_INSET;

                if (buttonFlags != 0)
                    GfxFillRectInset(
                        dpi, { coords, coords + ScreenCoordsXY{ _imageSize - 1, _imageSize - 1 } }, colours[1],
                        INSET_RECT_FLAG_FILL_MID_LIGHT | buttonFlags);

                DrawPixelInfo clipDPI;
                auto screenPos = coords + ScreenCoordsXY{ 2, 2 };
                if (ClipDrawPixelInfo(clipDPI, dpi, screenPos, _imageSize - 4, _imageSize - 4))
                {
                    PaintPreview(
                        clipDPI, entranceType.imageId, ScreenCoordsXY{ _imageSize / 2, _imageSize / 2 },
                        gWindowSceneryRotation);
                    //_loadedObject->DrawPreview(clipDPI, _width, _height);
                }

                // Draw ride image with feathered border
                //                auto mask = ImageId(SPR_NEW_RIDE_MASK);
                //                auto entranceImage = ImageId(entranceType.imageId);
                //                GfxDrawSpriteRawMasked(dpi, coords + ScreenCoordsXY{ 2, 2 }, mask, entranceImage);

                // Next position
                coords.x += _imageSize;
                if (coords.x >= _imageSize * _numColumns + 1)
                {
                    coords.x = 1;
                    coords.y += _imageSize;
                }
            }
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            PlaceParkEntranceToolDown(screenCoords);
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            PlaceParkEntranceToolUpdate(screenCoords);
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            ParkEntranceRemoveGhost();
            Invalidate();
            HideGridlines();
            HideLandRights();
            HideConstructionRights();
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto selected = ScrollGetEntranceListItemAt(screenCoords);
            if (selected == OBJECT_ENTRY_INDEX_NULL)
            {
                return;
            }

            _selectedEntranceType = selected;

            Audio::Play(Audio::SoundId::Click1, 0, windowPos.x + (width / 2));
            Invalidate();

            ToolSet(*this, WIDX_LIST, Tool::UpArrow);
            InputSetFlag(INPUT_FLAG_6, true);
        }
    };

    WindowBase* EditorOwnershipOpen()
    {
        WindowBase* window;

        // Check if window is already open
        window = WindowBringToFrontByClass(WindowClass::EditorOwnership);
        if (window != nullptr)
            return window;

        window = WindowCreate<EditorOwnership>(WindowClass::EditorOwnership, _windowWidth, _windowHeight, WF_10 | WF_RESIZABLE);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
