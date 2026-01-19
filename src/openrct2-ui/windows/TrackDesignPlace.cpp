/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
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
#include <openrct2/SpriteIds.h>
#include <openrct2/actions/TrackDesignAction.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/paint/VirtualFloor.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Map.h>
#include <openrct2/world/MapSelection.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/tile_element/Slope.h>
#include <openrct2/world/tile_element/SurfaceElement.h>
#include <vector>

using namespace OpenRCT2::Numerics;
using namespace OpenRCT2::TrackMetaData;
using OpenRCT2::Drawing::PaletteIndex;
using OpenRCT2::GameActions::CommandFlag;
using OpenRCT2::GameActions::CommandFlags;

namespace OpenRCT2::Ui::Windows
{
    static constexpr StringId kWindowTitle = STR_STRING;
    static constexpr ScreenSize kWindowSize = { 200, 124 };
    static constexpr ScreenSize kTrackMiniPreviewSize = { 168, 78 };

    static constexpr auto kPaletteIndexColourEntrance = PaletteIndex::pi20;         // White
    static constexpr auto kPaletteIndexColourExit = PaletteIndex::pi10;             // Black
    static constexpr auto kPaletteIndexColourTrack = PaletteIndex::primaryRemap5;   // Grey (dark)
    static constexpr auto kPaletteIndexColourStation = PaletteIndex::primaryRemap9; // Grey (light)

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_ROTATE,
        WIDX_MIRROR,
        WIDX_SELECT_DIFFERENT_DESIGN,
        WIDX_PRICE,
        WIDX_PREVIEW,
    };

    VALIDATE_GLOBAL_WIDX(WC_TRACK_DESIGN_PLACE, WIDX_ROTATE);

    // clang-format off
    static constexpr auto _trackPlaceWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({173,  83}, { 24, 24},             WidgetType::flatBtn, WindowColour::primary, ImageId(SPR_ROTATE_ARROW),     STR_ROTATE_90_TIP                         ),
        makeWidget({173,  59}, { 24, 24},             WidgetType::flatBtn, WindowColour::primary, ImageId(SPR_MIRROR_ARROW),     STR_MIRROR_IMAGE_TIP                      ),
        makeWidget({  4, 109}, {192, 12},             WidgetType::button,  WindowColour::primary, STR_SELECT_A_DIFFERENT_DESIGN, STR_GO_BACK_TO_DESIGN_SELECTION_WINDOW_TIP),
        makeWidget({ 88,  93}, {  1,  1},             WidgetType::empty,   WindowColour::primary),
        makeWidget({  4,  17}, kTrackMiniPreviewSize, WidgetType::empty,   WindowColour::primary)
    );
    // clang-format on

    static bool _placingTrackDesign = false;

    class TrackDesignPlaceWindow final : public Window
    {
    private:
        std::unique_ptr<TrackDesign> _trackDesign;

        CoordsXYZD _placementLoc;
        RideId _placementGhostRideId;
        bool _hasPlacementGhost;
        money64 _placementCost;
        CoordsXYZD _placementGhostLoc;

        std::vector<PaletteIndex> _miniPreview;

        bool _trackPlaceCtrlState = false;
        int32_t _trackPlaceCtrlZ;

        bool _trackPlaceShiftState = false;
        ScreenCoordsXY _trackPlaceShiftStart;
        int32_t _trackPlaceShiftZ;

        int32_t _trackPlaceZ;
        bool _triggeredUndergroundView = false;

    public:
        void onOpen() override
        {
            setWidgets(_trackPlaceWidgets);
            WindowInitScrollWidgets(*this);
            ToolSet(*this, WIDX_PRICE, Tool::crosshair);
            gInputFlags.set(InputFlag::allowRightMouseRemoval);
            WindowPushOthersRight(*this);
            ShowGridlines();
            _miniPreview.resize(kTrackMiniPreviewSize.width * kTrackMiniPreviewSize.height);
            _placementCost = kMoney64Undefined;
            _placementLoc.SetNull();
            _currentTrackPieceDirection = (2 - GetCurrentRotation()) & 3;
        }

        void onClose() override
        {
            ClearProvisional();
            ViewportSetVisibility(ViewportVisibility::standard);
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);
            HideGridlines();
            _miniPreview.clear();
            _miniPreview.shrink_to_fit();
            _trackDesign = nullptr;
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_ROTATE:
                    ClearProvisional();
                    _currentTrackPieceDirection = (_currentTrackPieceDirection + 1) & 3;
                    invalidate();
                    _placementLoc.SetNull();
                    DrawMiniPreview(*_trackDesign);
                    break;
                case WIDX_MIRROR:
                    TrackDesignMirror(*_trackDesign);
                    _currentTrackPieceDirection = (0 - _currentTrackPieceDirection) & 3;
                    invalidate();
                    _placementLoc.SetNull();
                    DrawMiniPreview(*_trackDesign);
                    break;
                case WIDX_SELECT_DIFFERENT_DESIGN:
                    close();

                    auto intent = Intent(WindowClass::trackDesignList);
                    intent.PutExtra(INTENT_EXTRA_RIDE_TYPE, _window_track_list_item.Type);
                    intent.PutExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, _window_track_list_item.EntryIndex);
                    ContextOpenIntent(&intent);
                    break;
            }
        }

        void onUpdate() override
        {
            if (!isToolActive(WindowClass::trackDesignPlace))
                close();
        }

        void onToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            TrackDesignState tds{};

            gMapSelectFlags.unset(MapSelectFlag::enable);
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

            if (_placingTrackDesign)
            {
                return;
            }

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
                    tds, *_trackDesign, RideGetTemporaryForPreview(), { mapCoords, 0, _currentTrackPieceDirection },
                    !gTrackDesignSceneryToggle);
                return;
            }

            money64 cost = kMoney64Undefined;
            if (GameIsNotPaused() || getGameState().cheats.buildInPauseMode)
            {
                ClearProvisional();
                CoordsXYZD ghostTrackLoc = trackLoc;
                auto res = FindValidTrackDesignPlaceHeight(ghostTrackLoc, { CommandFlag::noSpend, CommandFlag::ghost });

                if (res.error == GameActions::Status::ok)
                {
                    // Valid location found. Place the ghost at the location.
                    auto tdAction = GameActions::TrackDesignAction(
                        ghostTrackLoc, *_trackDesign, !gTrackDesignSceneryToggle,
                        Config::Get().general.defaultInspectionInterval);
                    tdAction.SetFlags({ CommandFlag::noSpend, CommandFlag::ghost });
                    tdAction.SetCallback([&](const GameActions::GameAction*, const GameActions::Result* result) {
                        if (result->error == GameActions::Status::ok)
                        {
                            _placementGhostRideId = result->getData<RideId>();
                            _placementGhostLoc = ghostTrackLoc;
                            _hasPlacementGhost = true;
                        }
                    });
                    res = GameActions::Execute(&tdAction, getGameState());
                    cost = res.error == GameActions::Status::ok ? res.cost : kMoney64Undefined;

                    VirtualFloorSetHeight(ghostTrackLoc.z);
                }
            }

            _placementLoc = trackLoc;
            if (cost != _placementCost)
            {
                _placementCost = cost;
                invalidateWidget(WIDX_PRICE);
            }

            TrackDesignPreviewDrawOutlines(
                tds, *_trackDesign, RideGetTemporaryForPreview(), trackLoc, !gTrackDesignSceneryToggle);
        }

        void onToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
        {
            ClearProvisional();
            gMapSelectFlags.unset(MapSelectFlag::enable);
            gMapSelectFlags.unset(MapSelectFlag::enableConstruct);
            gMapSelectFlags.unset(MapSelectFlag::enableArrow);

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
            auto res = FindValidTrackDesignPlaceHeight(trackLoc, {});
            if (res.error != GameActions::Status::ok)
            {
                // Unable to build track
                Audio::Play3D(Audio::SoundId::error, trackLoc);

                auto windowManager = GetWindowManager();
                windowManager->ShowError(res.getErrorTitle(), res.getErrorMessage());
                return;
            }

            _placingTrackDesign = true;

            auto tdAction = GameActions::TrackDesignAction(
                { trackLoc, _currentTrackPieceDirection }, *_trackDesign, !gTrackDesignSceneryToggle,
                Config::Get().general.defaultInspectionInterval);
            tdAction.SetCallback([&, trackLoc](const GameActions::GameAction*, const GameActions::Result* result) {
                if (result->error != GameActions::Status::ok)
                {
                    Audio::Play3D(Audio::SoundId::error, result->position);
                    _placingTrackDesign = false;
                    return;
                }

                rideId = result->getData<RideId>();
                auto getRide = GetRide(rideId);
                if (getRide != nullptr)
                {
                    auto* windowMgr = GetWindowManager();
                    windowMgr->CloseByClass(WindowClass::error);

                    Audio::Play3D(Audio::SoundId::placeItem, trackLoc);
                    _currentRideIndex = rideId;

                    if (TrackDesignAreEntranceAndExitPlaced())
                    {
                        auto intent = Intent(WindowClass::ride);
                        intent.PutExtra(INTENT_EXTRA_RIDE_ID, rideId.ToUnderlying());
                        ContextOpenIntent(&intent);
                        auto* wnd = windowMgr->FindByClass(WindowClass::trackDesignPlace);
                        windowMgr->Close(*wnd);
                    }
                    else
                    {
                        RideInitialiseConstructionWindow(*getRide);
                        auto* wnd = windowMgr->FindByClass(WindowClass::rideConstruction);
                        wnd->onMouseUp(WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
                    }
                }
                _placingTrackDesign = false;
            });
            GameActions::Execute(&tdAction, getGameState());
        }

        void onToolAbort(WidgetIndex widgetIndex) override
        {
            ClearProvisional();
        }

        void onViewportRotate() override
        {
            DrawMiniPreview(*_trackDesign);
        }

        void onPrepareDraw() override
        {
            DrawMiniPreview(*_trackDesign);
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            auto ft = Formatter::Common();
            ft.Add<char*>(_trackDesign->gameStateData.name.c_str());
            WindowDrawWidgets(*this, rt);

            // Draw mini tile preview
            Drawing::RenderTarget clippedRT;
            const auto& previewWidget = widgets[WIDX_PREVIEW];
            const auto previewCoords = windowPos + ScreenCoordsXY{ previewWidget.left, previewWidget.top };
            if (ClipRenderTarget(clippedRT, rt, previewCoords, previewWidget.width(), previewWidget.height()))
            {
                G1Element g1temp = {};
                g1temp.offset = reinterpret_cast<uint8_t*>(_miniPreview.data());
                g1temp.width = kTrackMiniPreviewSize.width;
                g1temp.height = kTrackMiniPreviewSize.height;
                GfxSetG1Element(SPR_TEMP, &g1temp);
                DrawingEngineInvalidateImage(SPR_TEMP);
                GfxDrawSprite(clippedRT, ImageId(SPR_TEMP, this->colours[0].colour), { 0, 0 });
            }

            // Price
            if (_placementCost != kMoney64Undefined && !(getGameState().park.flags & PARK_FLAGS_NO_MONEY))
            {
                ft = Formatter();
                ft.Add<money64>(_placementCost);
                const auto& priceWidget = widgets[WIDX_PRICE];
                const auto priceCoords = windowPos + ScreenCoordsXY{ priceWidget.left, priceWidget.top };
                DrawTextBasic(rt, priceCoords, STR_COST_LABEL, ft, { TextAlignment::centre });
            }
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
                auto tdAction = GameActions::TrackDesignAction(
                    { _placementGhostLoc }, *_trackDesign, !gTrackDesignSceneryToggle,
                    Config::Get().general.defaultInspectionInterval);
                tdAction.SetFlags({ CommandFlag::noSpend, CommandFlag::ghost });
                auto res = GameActions::Execute(&tdAction, getGameState());
                if (res.error != GameActions::Status::ok)
                {
                    _hasPlacementGhost = false;
                }
            }
        }

        void init(std::unique_ptr<TrackDesign>&& trackDesign)
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
            std::fill(_miniPreview.begin(), _miniPreview.end(), PaletteIndex::transparent);
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

            if (!_trackPlaceCtrlState && im.isModifierKeyPressed(ModifierKey::ctrl))
            {
                constexpr auto interactionFlags = EnumsToFlags(
                    ViewportInteractionItem::terrain, ViewportInteractionItem::ride, ViewportInteractionItem::scenery,
                    ViewportInteractionItem::footpath, ViewportInteractionItem::wall, ViewportInteractionItem::largeScenery);

                auto info = GetMapCoordinatesFromPos(screenCoords, interactionFlags);
                if (info.interactionType == ViewportInteractionItem::terrain)
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
            else if (!im.isModifierKeyPressed(ModifierKey::ctrl))
            {
                _trackPlaceCtrlState = false;
                _trackPlaceCtrlZ = 0;
            }

            if (!_trackPlaceShiftState && im.isModifierKeyPressed(ModifierKey::shift))
            {
                _trackPlaceShiftState = true;
                _trackPlaceShiftStart = screenCoords;
                _trackPlaceShiftZ = 0;
            }
            else if (im.isModifierKeyPressed(ModifierKey::shift))
            {
                uint16_t newMaxHeight = ZoomLevel::max().ApplyTo(
                    std::numeric_limits<decltype(TileElement::BaseHeight)>::max() - 32);

                _trackPlaceShiftZ = _trackPlaceShiftStart.y - screenCoords.y + 4;

                // Scale delta by zoom to match mouse position.
                auto* mainWnd = WindowGetMain();
                if (mainWnd != nullptr && mainWnd->viewport != nullptr)
                    _trackPlaceShiftZ = mainWnd->viewport->zoom.ApplyTo(_trackPlaceShiftZ);

                // Floor to closest kCoordsZStep
                _trackPlaceShiftZ = floor2(_trackPlaceShiftZ, kCoordsZStep);

                // Clamp to maximum possible value of BaseHeight can offer.
                _trackPlaceShiftZ = std::min<int16_t>(_trackPlaceShiftZ, newMaxHeight);
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

            // Trigger underground view?
            auto* mainWnd = WindowGetMain();
            if (mainWnd != nullptr && mainWnd->viewport != nullptr)
            {
                if (_trackPlaceZ < surfaceElement->GetBaseZ() && !_triggeredUndergroundView)
                {
                    mainWnd->viewport->flags |= VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                    _triggeredUndergroundView = true;
                }
                else if (_trackPlaceZ >= surfaceElement->GetBaseZ() && _triggeredUndergroundView)
                {
                    mainWnd->viewport->flags &= ~VIEWPORT_FLAG_UNDERGROUND_INSIDE;
                    _triggeredUndergroundView = false;
                }
            }

            // Force placement at the designated position if modifiers are used
            if (_trackPlaceShiftState || _trackPlaceCtrlState)
                return _trackPlaceZ;

            // Figure out a good position to place the design, taking other elements and surface height into account
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
                        PaletteIndex* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);
                        auto colour = entrance.isExit ? kPaletteIndexColourExit : kPaletteIndexColourEntrance;
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
                            PaletteIndex* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);

                            auto bits = trackBlock.quarterTile.Rotate(curTrackRotation & 3).GetBaseQuarterOccupied();

                            // Station track is a lighter colour
                            auto colour = ted.sequences[0].flags.has(SequenceFlag::trackOrigin) ? kPaletteIndexColourStation
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
                        auto* pixel = DrawMiniPreviewGetPixelPtr(pixelPosition);

                        auto colour = kPaletteIndexColourTrack;
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

        PaletteIndex* DrawMiniPreviewGetPixelPtr(const ScreenCoordsXY& pixel)
        {
            return &_miniPreview[pixel.y * kTrackMiniPreviewSize.width + pixel.x];
        }

        GameActions::Result FindValidTrackDesignPlaceHeight(CoordsXYZ& loc, CommandFlags newFlags)
        {
            GameActions::Result res;
            for (int32_t i = 0; i < 7; i++, loc.z += kCoordsZStep)
            {
                auto tdAction = GameActions::TrackDesignAction(
                    CoordsXYZD{ loc.x, loc.y, loc.z, _currentTrackPieceDirection }, *_trackDesign, !gTrackDesignSceneryToggle,
                    Config::Get().general.defaultInspectionInterval);
                tdAction.SetFlags(newFlags);
                res = GameActions::Query(&tdAction, getGameState());

                // If successful don't keep trying.
                // If failure due to no money then increasing height only makes problem worse
                if (res.error == GameActions::Status::ok || res.error == GameActions::Status::insufficientFunds)
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

        auto* windowMgr = GetWindowManager();
        windowMgr->CloseConstructionWindows();

        auto* window = windowMgr->FocusOrCreate<TrackDesignPlaceWindow>(WindowClass::trackDesignPlace, kWindowSize, {});
        if (window != nullptr)
        {
            window->init(std::move(openTrackDesign));
        }
        return window;
    }

    void TrackPlaceClearProvisionalTemporarily()
    {
        auto* windowMgr = GetWindowManager();
        auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(windowMgr->FindByClass(WindowClass::trackDesignPlace));
        if (trackPlaceWnd != nullptr)
        {
            trackPlaceWnd->ClearProvisionalTemporarily();
        }
    }

    void TrackPlaceRestoreProvisional()
    {
        auto* windowMgr = GetWindowManager();
        auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(windowMgr->FindByClass(WindowClass::trackDesignPlace));
        if (trackPlaceWnd != nullptr)
        {
            trackPlaceWnd->RestoreProvisional();
        }
    }
} // namespace OpenRCT2::Ui::Windows
