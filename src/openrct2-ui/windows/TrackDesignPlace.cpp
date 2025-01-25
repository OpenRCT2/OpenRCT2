/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/UiContext.h>
#include <openrct2-ui/input/InputManager.h>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/ViewportInteraction.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/GameState.h>
#include <openrct2/Input.h>
#include <openrct2/actions/TrackDesignAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <vector>

using namespace OpenRCT2::Numerics;
using namespace OpenRCT2::TrackMetaData;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId WINDOW_TITLE = STR_STRING;
    static constexpr int32_t WH = 124;
    static constexpr int32_t WW = 200;
    constexpr int16_t TRACK_MINI_PREVIEW_WIDTH = 168;
    constexpr int16_t TRACK_MINI_PREVIEW_HEIGHT = 78;
    constexpr uint16_t TRACK_MINI_PREVIEW_SIZE = TRACK_MINI_PREVIEW_WIDTH * TRACK_MINI_PREVIEW_HEIGHT;

    static constexpr uint8_t kPaletteIndexColourEntrance = PALETTE_INDEX_20; // White
    static constexpr uint8_t kPaletteIndexColourExit = PALETTE_INDEX_10;     // Black
    static constexpr uint8_t kPaletteIndexColourTrack = PALETTE_INDEX_248;   // Grey (dark)
    static constexpr uint8_t kPaletteIndexColourStation = PALETTE_INDEX_252; // Grey (light)

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_ROTATE,
        WIDX_MIRROR,
        WIDX_SELECT_DIFFERENT_DESIGN,
        WIDX_PRICE
    };

    validate_global_widx(WC_TRACK_DESIGN_PLACE, WIDX_ROTATE);

    // clang-format off
    static constexpr Widget _trackPlaceWidgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({173,  83}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_ROTATE_ARROW),              STR_ROTATE_90_TIP                         ),
        MakeWidget({173,  59}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_MIRROR_ARROW),              STR_MIRROR_IMAGE_TIP                      ),
        MakeWidget({  4, 109}, {192, 12}, WindowWidgetType::Button,  WindowColour::Primary, STR_SELECT_A_DIFFERENT_DESIGN, STR_GO_BACK_TO_DESIGN_SELECTION_WINDOW_TIP),
        MakeWidget({  0,   0}, {  1,  1}, WindowWidgetType::Empty,   WindowColour::Primary),
    };
    // clang-format on

    class TrackDesignPlaceWindow final : public Window
    {
    private:
        std::unique_ptr<TrackDesign> _trackDesign;

        CoordsXYZD _placementLoc;
        RideId _placementGhostRideId;
        bool _hasPlacementGhost;
        money64 _placementCost;
        CoordsXYZD _placementGhostLoc;

        std::vector<uint8_t> _miniPreview;

        bool _trackPlaceCtrlState = false;
        int32_t _trackPlaceCtrlZ;

        bool _trackPlaceShiftState = false;
        ScreenCoordsXY _trackPlaceShiftStart;
        int32_t _trackPlaceShiftZ;

        int32_t _trackPlaceZ;

    public:
        void OnOpen() override
        {
            SetWidgets(_trackPlaceWidgets);
            WindowInitScrollWidgets(*this);
            ToolSet(*this, WIDX_PRICE, Tool::Crosshair);
            InputSetFlag(INPUT_FLAG_6, true);
            WindowPushOthersRight(*this);
            ShowGridlines();
            _miniPreview.resize(TRACK_MINI_PREVIEW_SIZE);
            _placementCost = kMoney64Undefined;
            _placementLoc.SetNull();
            _currentTrackPieceDirection = (2 - GetCurrentRotation()) & 3;
        }

        void OnClose() override
        {
            ClearProvisional();
            ViewportSetVisibility(ViewportVisibility::Default);
            MapInvalidateMapSelectionTiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;
            HideGridlines();
            _miniPreview.clear();
            _miniPreview.shrink_to_fit();
            _trackDesign = nullptr;
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_ROTATE:
                    ClearProvisional();
                    _currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
                    Invalidate();
                    _placementLoc.SetNull();
                    DrawMiniPreview(*_trackDesign);
                    break;
                case WIDX_MIRROR:
                    TrackDesignMirror(*_trackDesign);
                    _currentTrackPieceDirection = (0 - _currentTrackPieceDirection) & 3;
                    Invalidate();
                    _placementLoc.SetNull();
                    DrawMiniPreview(*_trackDesign);
                    break;
                case WIDX_SELECT_DIFFERENT_DESIGN:
                    Close();

                    auto intent = Intent(WindowClass::TrackDesignList);
                    intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, _window_track_list_item.Type);
                    intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, _window_track_list_item.EntryIndex);
                    ContextOpenIntent(&intent);
                    break;
            }
        }

        void OnUpdate() override
        {
            if (!isToolActive(WindowClass::TrackDesignPlace))
                Close();
        }

        void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            TrackDesignState tds{};

            MapInvalidateMapSelectionTiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            // Take shift modifier into account
            ScreenCoordsXY targetScreenCoords = screenCoords;
            if (_trackPlaceShiftState)
                targetScreenCoords = _trackPlaceShiftStart;

            // Get the tool map position
            CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(targetScreenCoords);
            if (mapCoords.IsNull())
            {
                ClearProvisional();
                return;
            }

            // Get base Z position
            // NB: always use the actual screenCoords here, not the shifted ones
            auto maybeMapZ = GetBaseZ(mapCoords, screenCoords);
            if (!maybeMapZ.has_value())
            {
                ClearProvisional();
                return;
            }

            CoordsXYZD trackLoc = { mapCoords, *maybeMapZ, _currentTrackPieceDirection };

            // Check if tool map position has changed since last update
            if (trackLoc == _placementLoc)
            {
                TrackDesignPreviewDrawOutlines(
                    tds, *_trackDesign, RideGetTemporaryForPreview(), { mapCoords, 0, _currentTrackPieceDirection });
                return;
            }

            money64 cost = kMoney64Undefined;
            if (GameIsNotPaused() || GetGameState().Cheats.buildInPauseMode)
            {
                ClearProvisional();
                auto res = FindValidTrackDesignPlaceHeight(trackLoc, GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);

                if (res.Error == GameActions::Status::Ok)
                {
                    // Valid location found. Place the ghost at the location.
                    auto tdAction = TrackDesignAction(trackLoc, *_trackDesign);
                    tdAction.SetFlags(GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
                    tdAction.SetCallback([&](const GameAction*, const GameActions::Result* result) {
                        if (result->Error == GameActions::Status::Ok)
                        {
                            _placementGhostRideId = result->GetData<RideId>();
                            _placementGhostLoc = trackLoc;
                            _hasPlacementGhost = true;
                        }
                    });
                    res = GameActions::Execute(&tdAction);
                    cost = res.Error == GameActions::Status::Ok ? res.Cost : kMoney64Undefined;

                    VirtualFloorSetHeight(trackLoc.z);
                }
            }

            _placementLoc = trackLoc;
            if (cost != _placementCost)
            {
                _placementCost = cost;
                InvalidateWidget(WIDX_PRICE);
            }

            TrackDesignPreviewDrawOutlines(tds, *_trackDesign, RideGetTemporaryForPreview(), trackLoc);
        }

        void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            ClearProvisional();
            MapInvalidateMapSelectionTiles();
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
            gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

            // Take shift modifier into account
            ScreenCoordsXY targetScreenCoords = screenCoords;
            if (_trackPlaceShiftState)
                targetScreenCoords = _trackPlaceShiftStart;

            // Get the tool map position
            CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(targetScreenCoords);
            if (mapCoords.IsNull())
            {
                ClearProvisional();
                return;
            }

            // NB: always use the actual screenCoords here, not the shifted ones
            auto maybeMapZ = GetBaseZ(mapCoords, screenCoords);
            if (!maybeMapZ.has_value())
            {
                ClearProvisional();
                return;
            }

            // Try increasing Z until a feasible placement is found
            CoordsXYZ trackLoc = { mapCoords, maybeMapZ.value() };
            auto res = FindValidTrackDesignPlaceHeight(trackLoc, 0);
            if (res.Error != GameActions::Status::Ok)
            {
                // Unable to build track
                Audio::Play3D(Audio::SoundId::Error, trackLoc);

                auto windowManager = GetWindowManager();
                windowManager->ShowError(res.GetErrorTitle(), res.GetErrorMessage());
                return;
            }

            auto tdAction = TrackDesignAction({ trackLoc, _currentTrackPieceDirection }, *_trackDesign);
            tdAction.SetCallback([&](const GameAction*, const GameActions::Result* result) {
                if (result->Error != GameActions::Status::Ok)
                {
                    Audio::Play3D(Audio::SoundId::Error, result->Position);
                    return;
                }

                rideId = result->GetData<RideId>();
                auto getRide = GetRide(rideId);
                if (getRide != nullptr)
                {
                    auto* windowMgr = Ui::GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::Error);

                    Audio::Play3D(Audio::SoundId::PlaceItem, trackLoc);
                    _currentRideIndex = rideId;

                    if (TrackDesignAreEntranceAndExitPlaced())
                    {
                        auto intent = Intent(WindowClass::Ride);
                        intent.PutExtra(INTENT_EXTRA_RIDE_ID, rideId.ToUnderlying());
                        ContextOpenIntent(&intent);
                        auto* wnd = windowMgr->FindByClass(WindowClass::TrackDesignPlace);
                        windowMgr->Close(*wnd);
                    }
                    else
                    {
                        RideInitialiseConstructionWindow(*getRide);
                        auto* wnd = windowMgr->FindByClass(WindowClass::RideConstruction);
                        wnd->OnMouseUp(WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
                    }
                }
            });
            GameActions::Execute(&tdAction);
        }

        void OnToolAbort(WidgetIndex widgetIndex) override
        {
            ClearProvisional();
        }

        void OnViewportRotate() override
        {
            DrawMiniPreview(*_trackDesign);
        }

        void OnPrepareDraw() override
        {
            DrawMiniPreview(*_trackDesign);
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            auto ft = Formatter::Common();
            ft.Add<char*>(_trackDesign->gameStateData.name.c_str());
            WindowDrawWidgets(*this, dpi);

            // Draw mini tile preview
            DrawPixelInfo clippedDpi;
            if (ClipDrawPixelInfo(clippedDpi, dpi, this->windowPos + ScreenCoordsXY{ 4, 18 }, 168, 78))
            {
                G1Element g1temp = {};
                g1temp.offset = _miniPreview.data();
                g1temp.width = TRACK_MINI_PREVIEW_WIDTH;
                g1temp.height = TRACK_MINI_PREVIEW_HEIGHT;
                GfxSetG1Element(SPR_TEMP, &g1temp);
                DrawingEngineInvalidateImage(SPR_TEMP);
                GfxDrawSprite(clippedDpi, ImageId(SPR_TEMP, this->colours[0].colour), { 0, 0 });
            }

            // Price
            if (_placementCost != kMoney64Undefined && !(GetGameState().Park.Flags & PARK_FLAGS_NO_MONEY))
            {
                ft = Formatter();
                ft.Add<money64>(_placementCost);
                DrawTextBasic(dpi, this->windowPos + ScreenCoordsXY{ 88, 94 }, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
            }
        }

        void OnResize() override
        {
            ResizeFrame();
        }

        void ClearProvisionalTemporarily()
        {
            if (_hasPlacementGhost)
            {
                auto provRide = GetRide(_placementGhostRideId);
                if (provRide != nullptr)
                {
                    TrackDesignPreviewRemoveGhosts(*_trackDesign, *provRide, _placementGhostLoc);
                }
            }
        }

        void RestoreProvisional()
        {
            if (_hasPlacementGhost)
            {
                auto tdAction = TrackDesignAction({ _placementGhostLoc }, *_trackDesign);
                tdAction.SetFlags(GAME_COMMAND_FLAG_NO_SPEND | GAME_COMMAND_FLAG_GHOST);
                auto res = GameActions::Execute(&tdAction);
                if (res.Error != GameActions::Status::Ok)
                {
                    _hasPlacementGhost = false;
                }
            }
        }

        void Init(std::unique_ptr<TrackDesign>&& trackDesign)
        {
            _trackDesign = std::move(trackDesign);
        }

        void DrawMiniPreview(const TrackDesign& td)
        {
            ClearMiniPreview();

            // First pass is used to determine the width and height of the image so it can centre it
            CoordsXY min = { 0, 0 };
            CoordsXY max = { 0, 0 };
            for (int32_t pass = 0; pass < 2; pass++)
            {
                CoordsXY origin = { 0, 0 };
                if (pass == 1)
                {
                    origin.x -= ((max.x + min.x) >> 6) * kCoordsXYStep;
                    origin.y -= ((max.y + min.y) >> 6) * kCoordsXYStep;
                }

                const auto& rtd = GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex);
                if (rtd.specialType == RtdSpecialType::maze)
                {
                    DrawMiniPreviewMaze(td, pass, origin, min, max);
                }
                else
                {
                    DrawMiniPreviewTrack(td, pass, origin, min, max);
                }
            }
        }

        void ClearMiniPreview()
        {
            // Fill with transparent colour.
            std::fill(_miniPreview.begin(), _miniPreview.end(), PALETTE_INDEX_0);
        }

    private:
        void ClearProvisional()
        {
            if (_hasPlacementGhost)
            {
                auto newRide = GetRide(_placementGhostRideId);
                if (newRide != nullptr)
                {
                    TrackDesignPreviewRemoveGhosts(*_trackDesign, *newRide, _placementGhostLoc);
                    _hasPlacementGhost = false;
                }

                VirtualFloorSetHeight(0);
            }
        }

        std::optional<int32_t> GetBaseZ([[maybe_unused]] const CoordsXY& loc, const ScreenCoordsXY& screenCoords)
        {
            CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
            auto surfaceElement = MapGetSurfaceElementAt(mapCoords);
            if (surfaceElement == nullptr)
                return std::nullopt;

            auto& im = GetInputManager();

            if (!_trackPlaceCtrlState && im.IsModifierKeyPressed(ModifierKey::ctrl))
            {
                constexpr auto interactionFlags = EnumsToFlags(
                    ViewportInteractionItem::Terrain, ViewportInteractionItem::Ride, ViewportInteractionItem::Scenery,
                    ViewportInteractionItem::Footpath, ViewportInteractionItem::Wall, ViewportInteractionItem::LargeScenery);

                auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
                if (info.interactionType == ViewportInteractionItem::Terrain)
                {
                    _trackPlaceCtrlZ = floor2(surfaceElement->GetBaseZ(), kCoordsZStep);

                    // Increase Z above water
                    if (surfaceElement->GetWaterHeight() > 0)
                        _trackPlaceCtrlZ = std::max(_trackPlaceCtrlZ, surfaceElement->GetWaterHeight());
                }
                else
                {
                    _trackPlaceCtrlZ = floor2(info.Element->GetBaseZ(), kCoordsZStep);
                }

                _trackPlaceCtrlState = true;
            }
            else if (!im.IsModifierKeyPressed(ModifierKey::ctrl))
            {
                _trackPlaceCtrlState = false;
                _trackPlaceCtrlZ = 0;
            }

            if (!_trackPlaceShiftState && im.IsModifierKeyPressed(ModifierKey::shift))
            {
                _trackPlaceShiftState = true;
                _trackPlaceShiftStart = screenCoords;
                _trackPlaceShiftZ = 0;
            }
            else if (im.IsModifierKeyPressed(ModifierKey::shift))
            {
                uint16_t maxHeight = ZoomLevel::max().ApplyTo(
                    std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

                _trackPlaceShiftZ = _trackPlaceShiftStart.y - screenCoords.y + 4;

                // Scale delta by zoom to match mouse position.
                auto* mainWnd = WindowGetMain();
                if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                    _trackPlaceShiftZ = mainWnd->viewport->zoom.ApplyTo(_trackPlaceShiftZ);

                // Floor to closest kCoordsZStep
                _trackPlaceShiftZ = floor2(_trackPlaceShiftZ, kCoordsZStep);

                // Clamp to maximum possible value of BaseHeight can offer.
                _trackPlaceShiftZ = std::min<int16_t>(_trackPlaceShiftZ, maxHeight);
            }
            else if (_trackPlaceShiftState)
            {
                _trackPlaceShiftState = false;
                _trackPlaceShiftZ = 0;
            }

            if (!_trackPlaceCtrlState)
            {
                _trackPlaceZ = floor2(surfaceElement->GetBaseZ(), kCoordsZStep);

                // Increase Z above water
                if (surfaceElement->GetWaterHeight() > 0)
                    _trackPlaceZ = std::max(_trackPlaceZ, surfaceElement->GetWaterHeight());

                if (_trackPlaceShiftState)
                {
                    _trackPlaceZ += _trackPlaceShiftZ;
                    _trackPlaceZ = std::max<int16_t>(16, _trackPlaceZ);
                }
            }
            else
            {
                _trackPlaceZ = _trackPlaceCtrlZ;
                if (_trackPlaceShiftState)
                    _trackPlaceZ += _trackPlaceShiftZ;

                _trackPlaceZ = std::max<int32_t>(16, _trackPlaceZ);
            }

            if (mapCoords.x == kLocationNull)
                return std::nullopt;

            return _trackPlaceZ
                + TrackDesignGetZPlacement(
                       *_trackDesign, RideGetTemporaryForPreview(), { mapCoords, _trackPlaceZ, _currentTrackPieceDirection });
        }

        void DrawMiniPreviewEntrances(
            const TrackDesign& td, int32_t pass, const CoordsXY& origin, CoordsXY& min, CoordsXY& max, Direction rotation)
        {
            for (const auto& entrance : td.entranceElements)
            {
                auto rotatedAndOffsetEntrance = origin + entrance.location.ToCoordsXY().Rotate(rotation);

                if (pass == 0)
                {
                    min.x = std::min(min.x, rotatedAndOffsetEntrance.x);
                    max.x = std::max(max.x, rotatedAndOffsetEntrance.x);
                    min.y = std::min(min.y, rotatedAndOffsetEntrance.y);
                    max.y = std::max(max.y, rotatedAndOffsetEntrance.y);
                }
                else
                {
                    auto pixelPosition = DrawMiniPreviewGetPixelPosition(rotatedAndOffsetEntrance);
                    if (DrawMiniPreviewIsPixelInBounds(pixelPosition))
                    {
                        uint8_t* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);
                        uint8_t colour = entrance.isExit ? kPaletteIndexColourExit : kPaletteIndexColourEntrance;
                        for (int32_t i = 0; i < 4; i++)
                        {
                            pixel[338 + i] = colour; // x + 2, y + 2
                            pixel[168 + i] = colour; //        y + 1
                            pixel[2 + i] = colour;   // x + 2
                            pixel[172 + i] = colour; // x + 4, y + 1
                        }
                    }
                }
            }
        }

        void DrawMiniPreviewTrack(const TrackDesign& td, int32_t pass, const CoordsXY& origin, CoordsXY& min, CoordsXY& max)
        {
            const uint8_t rotation = (_currentTrackPieceDirection + GetCurrentRotation()) & 3;

            CoordsXY curTrackStart = origin;
            uint8_t curTrackRotation = rotation;
            for (const auto& trackElement : td.trackElements)
            {
                // Follow a single track piece shape
                const auto& ted = GetTrackElementDescriptor(trackElement.type);
                for (size_t sequenceIndex = 0; sequenceIndex < ted.numSequences; sequenceIndex++)
                {
                    const auto& trackBlock = ted.sequences[sequenceIndex].clearance;
                    auto rotatedAndOffsetTrackBlock = curTrackStart
                        + CoordsXY{ trackBlock.x, trackBlock.y }.Rotate(curTrackRotation);

                    if (pass == 0)
                    {
                        min.x = std::min(min.x, rotatedAndOffsetTrackBlock.x);
                        max.x = std::max(max.x, rotatedAndOffsetTrackBlock.x);
                        min.y = std::min(min.y, rotatedAndOffsetTrackBlock.y);
                        max.y = std::max(max.y, rotatedAndOffsetTrackBlock.y);
                    }
                    else
                    {
                        auto pixelPosition = DrawMiniPreviewGetPixelPosition(rotatedAndOffsetTrackBlock);
                        if (DrawMiniPreviewIsPixelInBounds(pixelPosition))
                        {
                            uint8_t* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);

                            auto bits = trackBlock.quarterTile.Rotate(curTrackRotation & 3).GetBaseQuarterOccupied();

                            // Station track is a lighter colour
                            uint8_t colour = (ted.sequences[0].flags & TRACK_SEQUENCE_FLAG_ORIGIN) ? kPaletteIndexColourStation
                                                                                                   : kPaletteIndexColourTrack;

                            for (int32_t i = 0; i < 4; i++)
                            {
                                if (bits & 1)
                                    pixel[338 + i] = colour; // x + 2, y + 2
                                if (bits & 2)
                                    pixel[168 + i] = colour; //        y + 1
                                if (bits & 4)
                                    pixel[2 + i] = colour; // x + 2
                                if (bits & 8)
                                    pixel[172 + i] = colour; // x + 4, y + 1
                            }
                        }
                    }
                }

                // Change rotation and next position based on track curvature
                curTrackRotation &= 3;

                const TrackCoordinates* track_coordinate = &ted.coordinates;

                curTrackStart += CoordsXY{ track_coordinate->x, track_coordinate->y }.Rotate(curTrackRotation);
                curTrackRotation += track_coordinate->rotationEnd - track_coordinate->rotationBegin;
                curTrackRotation &= 3;
                if (track_coordinate->rotationEnd & 4)
                {
                    curTrackRotation |= 4;
                }
                if (!(curTrackRotation & 4))
                {
                    curTrackStart += CoordsDirectionDelta[curTrackRotation];
                }
            }

            DrawMiniPreviewEntrances(td, pass, origin, min, max, rotation);
        }

        void DrawMiniPreviewMaze(const TrackDesign& td, int32_t pass, const CoordsXY& origin, CoordsXY& min, CoordsXY& max)
        {
            uint8_t rotation = (_currentTrackPieceDirection + GetCurrentRotation()) & 3;
            for (const auto& mazeElement : td.mazeElements)
            {
                auto rotatedMazeCoords = origin + mazeElement.location.ToCoordsXY().Rotate(rotation);

                if (pass == 0)
                {
                    min.x = std::min(min.x, rotatedMazeCoords.x);
                    max.x = std::max(max.x, rotatedMazeCoords.x);
                    min.y = std::min(min.y, rotatedMazeCoords.y);
                    max.y = std::max(max.y, rotatedMazeCoords.y);
                }
                else
                {
                    auto pixelPosition = DrawMiniPreviewGetPixelPosition(rotatedMazeCoords);
                    if (DrawMiniPreviewIsPixelInBounds(pixelPosition))
                    {
                        uint8_t* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);

                        uint8_t colour = kPaletteIndexColourTrack;
                        for (int32_t i = 0; i < 4; i++)
                        {
                            pixel[338 + i] = colour; // x + 2, y + 2
                            pixel[168 + i] = colour; //        y + 1
                            pixel[2 + i] = colour;   // x + 2
                            pixel[172 + i] = colour; // x + 4, y + 1
                        }
                    }
                }
            }

            DrawMiniPreviewEntrances(td, pass, origin, min, max, rotation);
        }

        ScreenCoordsXY DrawMiniPreviewGetPixelPosition(const CoordsXY& location)
        {
            auto tilePos = TileCoordsXY(location);
            return { (80 + (tilePos.y - tilePos.x) * 4), (38 + (tilePos.y + tilePos.x) * 2) };
        }

        bool DrawMiniPreviewIsPixelInBounds(const ScreenCoordsXY& pixel)
        {
            return pixel.x >= 0 && pixel.y >= 0 && pixel.x <= 160 && pixel.y <= 75;
        }

        uint8_t* DrawMiniPreviewGetPixelPtr(const ScreenCoordsXY& pixel)
        {
            return &_miniPreview[pixel.y * TRACK_MINI_PREVIEW_WIDTH + pixel.x];
        }

        GameActions::Result FindValidTrackDesignPlaceHeight(CoordsXYZ& loc, uint32_t newFlags)
        {
            GameActions::Result res;
            for (int32_t i = 0; i < 7; i++, loc.z += kCoordsZStep)
            {
                auto tdAction = TrackDesignAction(
                    CoordsXYZD{ loc.x, loc.y, loc.z, _currentTrackPieceDirection }, *_trackDesign);
                tdAction.SetFlags(newFlags);
                res = GameActions::Query(&tdAction);

                // If successful don't keep trying.
                // If failure due to no money then increasing height only makes problem worse
                if (res.Error == GameActions::Status::Ok || res.Error == GameActions::Status::InsufficientFunds)
                {
                    return res;
                }
            }
            return res;
        }
    };

    WindowBase* TrackPlaceOpen(const TrackDesignFileRef* tdFileRef)
    {
        std::unique_ptr<TrackDesign> openTrackDesign = TrackDesignImport(tdFileRef->path.c_str());

        if (openTrackDesign == nullptr)
        {
            return nullptr;
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseConstructionWindows();

        auto* window = windowMgr->FocusOrCreate<TrackDesignPlaceWindow>(WindowClass::TrackDesignPlace, WW, WH, 0);
        if (window != nullptr)
        {
            window->Init(std::move(openTrackDesign));
        }
        return window;
    }

    void TrackPlaceClearProvisionalTemporarily()
    {
        auto* windowMgr = GetWindowManager();
        auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(windowMgr->FindByClass(WindowClass::TrackDesignPlace));
        if (trackPlaceWnd != nullptr)
        {
            trackPlaceWnd->ClearProvisionalTemporarily();
        }
    }

    void TrackPlaceRestoreProvisional()
    {
        auto* windowMgr = GetWindowManager();
        auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(windowMgr->FindByClass(WindowClass::TrackDesignPlace));
        if (trackPlaceWnd != nullptr)
        {
            trackPlaceWnd->RestoreProvisional();
        }
    }
} // namespace OpenRCT2::Ui::Windows
