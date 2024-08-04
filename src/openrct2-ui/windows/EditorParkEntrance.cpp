/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Context.h>
#include <openrct2/Input.h>
#include <openrct2/actions/ParkEntrancePlaceAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/object/EntranceObject.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/paint/tile_element/Paint.TileElement.h>
#include <openrct2/sprites.h>
#include <openrct2/world/tile_element/Slope.h>

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_OBJECT_SELECTION_PARK_ENTRANCE;
    static constexpr int32_t kImageSize = 116;
    static constexpr int32_t kNumColumns = 4;
    static constexpr int32_t kNumRows = 1;
    static constexpr int32_t kScrollPadding = 2;
    static constexpr int32_t kScrollWidth = (kImageSize * kNumColumns) + kScrollBarWidth + 4;
    static constexpr int32_t kScrollHeight = (kImageSize * kNumRows);
    static constexpr int32_t kWindowWidth = kScrollWidth + 28;
    static constexpr int32_t kWindowHeight = kScrollHeight + 50;

    struct EntranceSelection
    {
        ObjectEntryIndex entryIndex = OBJECT_ENTRY_INDEX_NULL;
        StringId stringId = STR_NONE;
        ImageIndex imageId = static_cast<ImageIndex>(SPR_NONE);
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

    validate_global_widx(WC_EDITOR_PARK_ENTRANCE, WIDX_ROTATE_ENTRANCE_BUTTON);

    // clang-format off
    static Widget _widgets[] = {
        WINDOW_SHIM(kWindowTitle, kWindowWidth, kWindowHeight),
        MakeWidget     ({                 0, 43 }, { kWindowWidth, kWindowHeight - 43 }, WindowWidgetType::Resize,  WindowColour::Secondary                                                   ),
        MakeTab        ({                 3, 17 },                                                                                           STR_NONE                                         ),
        MakeWidget     ({                 2, 45 }, { kScrollWidth, kScrollHeight      }, WindowWidgetType::Scroll,  WindowColour::Secondary, SCROLL_VERTICAL                                  ),
        MakeWidget     ({ kWindowWidth - 26, 59 }, {           24,            24      }, WindowWidgetType::FlatBtn, WindowColour::Secondary, ImageId(SPR_ROTATE_ARROW), STR_ROTATE_OBJECTS_90 ),
        kWidgetsEnd,
    };
    // clang-format on

    class EditorParkEntrance final : public Window
    {
    private:
        ObjectEntryIndex _selectedEntranceType = 0;
        ObjectEntryIndex _highlightedEntranceType = 0;
        std::vector<EntranceSelection> _entranceTypes{};

        void InitParkEntranceItems()
        {
            _entranceTypes.clear();
            for (ObjectEntryIndex objectIndex = 0; objectIndex < MAX_PARK_ENTRANCE_OBJECTS; objectIndex++)
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

            size_t column = screenCoords.x / kImageSize;
            size_t row = screenCoords.y / kImageSize;
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
            min_width = kWindowWidth;
            min_height = kWindowHeight;
            max_width = kWindowWidth;
            max_height = kWindowHeight;

            InitParkEntranceItems();
            pressed_widgets |= 1LL << WIDX_TAB;

            ToolSet(*this, WIDX_LIST, Tool::EntranceDown);
            InputSetFlag(INPUT_FLAG_6, true);
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

        void OnClose() override
        {
            if (gCurrentToolWidget.window_classification == classification)
                ToolCancel();
        }

        void OnUpdate() override
        {
            if (gCurrentToolWidget.window_classification != classification)
                Close();
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
                else if (_highlightedEntranceType == entranceType.entryIndex)
                    buttonFlags |= INSET_RECT_FLAG_FILL_MID_LIGHT;

                if (buttonFlags != 0)
                    GfxFillRectInset(
                        dpi, { coords, coords + ScreenCoordsXY{ kImageSize - 1, kImageSize - 1 } }, colours[1],
                        INSET_RECT_FLAG_FILL_MID_LIGHT | buttonFlags);

                DrawPixelInfo clipDPI;
                auto screenPos = coords + ScreenCoordsXY{ kScrollPadding, kScrollPadding };
                if (ClipDrawPixelInfo(
                        clipDPI, dpi, screenPos, kImageSize - (2 * kScrollPadding), kImageSize - (2 * kScrollPadding)))
                {
                    PaintPreview(
                        clipDPI, entranceType.imageId, ScreenCoordsXY{ kImageSize / 2, kImageSize / 2 },
                        gWindowSceneryRotation);
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

        void ContentScrollMouseOver(const ScreenCoordsXY& screenCoords)
        {
            auto highlighted = ScrollGetEntranceListItemAt(screenCoords);
            if (highlighted != OBJECT_ENTRY_INDEX_NULL)
            {
                _highlightedEntranceType = highlighted;
                Invalidate();
            }
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
        }
    };

    WindowBase* EditorParkEntranceOpen()
    {
        WindowBase* window;

        // Check if window is already open
        window = WindowBringToFrontByClass(WindowClass::EditorParkEntrance);
        if (window != nullptr)
            return window;

        window = WindowCreate<EditorParkEntrance>(
            WindowClass::EditorParkEntrance, kWindowWidth, kWindowHeight, WF_10 | WF_RESIZABLE);

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
