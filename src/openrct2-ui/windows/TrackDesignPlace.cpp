/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <algorithm>
#include <openrct2-ui/interface/Viewport.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Window.h>
#include <openrct2/Cheats.h>
#include <openrct2/Context.h>
#include <openrct2/Game.h>
#include <openrct2/Input.h>
#include <openrct2/actions/TrackDesignAction.h>
#include <openrct2/audio/audio.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Localisation.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/Track.h>
#include <openrct2/ride/TrackData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/UiContext.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/windows/Intent.h>
#include <openrct2/world/Park.h>
#include <openrct2/world/Surface.h>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::TrackMetaData;

static constexpr const StringId WINDOW_TITLE = STR_STRING;
static constexpr const int32_t WH = 124;
static constexpr const int32_t WW = 200;
constexpr int16_t TRACK_MINI_PREVIEW_WIDTH = 168;
constexpr int16_t TRACK_MINI_PREVIEW_HEIGHT = 78;
constexpr uint16_t TRACK_MINI_PREVIEW_SIZE = TRACK_MINI_PREVIEW_WIDTH * TRACK_MINI_PREVIEW_HEIGHT;

static constexpr uint8_t _PaletteIndexColourEntrance = PALETTE_INDEX_20; // White
static constexpr uint8_t _PaletteIndexColourExit = PALETTE_INDEX_10;     // Black
static constexpr uint8_t _PaletteIndexColourTrack = PALETTE_INDEX_248;   // Grey (dark)
static constexpr uint8_t _PaletteIndexColourStation = PALETTE_INDEX_252; // Grey (light)

// clang-format off
enum {
    WIDX_BACKGROUND,
    WIDX_TITLE,
    WIDX_CLOSE,
    WIDX_ROTATE,
    WIDX_MIRROR,
    WIDX_SELECT_DIFFERENT_DESIGN,
    WIDX_PRICE
};

validate_global_widx(WC_TRACK_DESIGN_PLACE, WIDX_ROTATE);

static Widget window_track_place_widgets[] = {
    WINDOW_SHIM(WINDOW_TITLE, WW, WH),
    MakeWidget({173,  83}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_ROTATE_ARROW),              STR_ROTATE_90_TIP                         ),
    MakeWidget({173,  59}, { 24, 24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_MIRROR_ARROW),              STR_MIRROR_IMAGE_TIP                      ),
    MakeWidget({  4, 109}, {192, 12}, WindowWidgetType::Button,  WindowColour::Primary, STR_SELECT_A_DIFFERENT_DESIGN, STR_GO_BACK_TO_DESIGN_SELECTION_WINDOW_TIP),
    MakeWidget({  0,   0}, {  1,  1}, WindowWidgetType::Empty,   WindowColour::Primary),
    WIDGETS_END,
};

// clang-format on

class TrackDesignPlaceWindow final : public Window
{
public:
    void OnOpen() override
    {
        widgets = window_track_place_widgets;
        WindowInitScrollWidgets(*this);
        ToolSet(*this, WIDX_PRICE, Tool::Crosshair);
        input_set_flag(INPUT_FLAG_6, true);
        WindowPushOthersRight(*this);
        ShowGridlines();
        _miniPreview.resize(TRACK_MINI_PREVIEW_SIZE);
        _placementCost = MONEY32_UNDEFINED;
        _placementLoc.SetNull();
        _currentTrackPieceDirection = (2 - GetCurrentRotation()) & 3;
    }

    void OnClose() override
    {
        ClearProvisional();
        ViewportSetVisibility(0);
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
                DrawMiniPreview(_trackDesign.get());
                break;
            case WIDX_MIRROR:
                TrackDesignMirror(_trackDesign.get());
                _currentTrackPieceDirection = (0 - _currentTrackPieceDirection) & 3;
                Invalidate();
                _placementLoc.SetNull();
                DrawMiniPreview(_trackDesign.get());
                break;
            case WIDX_SELECT_DIFFERENT_DESIGN:
                Close();

                auto intent = Intent(WindowClass::TrackDesignList);
                intent.putExtra(INTENT_EXTRA_RIDE_TYPE, _window_track_list_item.Type);
                intent.putExtra(INTENT_EXTRA_RIDE_ENTRY_INDEX, _window_track_list_item.EntryIndex);
                ContextOpenIntent(&intent);
                break;
        }
    }

    void OnUpdate() override
    {
        if (!(input_test_flag(INPUT_FLAG_TOOL_ACTIVE)))
            if (gCurrentToolWidget.window_classification != WindowClass::TrackDesignPlace)
                Close();
    }

    void OnToolUpdate(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        TrackDesignState tds{};
        int16_t mapZ;

        MapInvalidateMapSelectionTiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        // Get the tool map position
        CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
        if (mapCoords.IsNull())
        {
            ClearProvisional();
            return;
        }

        // Check if tool map position has changed since last update
        if (mapCoords == _placementLoc)
        {
            TrackDesignPreviewDrawOutlines(
                tds, _trackDesign.get(), *GetOrAllocateRide(PreviewRideId), { mapCoords, 0, _currentTrackPieceDirection });
            return;
        }

        money32 cost = MONEY32_UNDEFINED;

        // Get base Z position
        mapZ = GetBaseZ(mapCoords);
        CoordsXYZD trackLoc = { mapCoords, mapZ, _currentTrackPieceDirection };

        if (game_is_not_paused() || gCheatsBuildInPauseMode)
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
                cost = res.Error == GameActions::Status::Ok ? res.Cost : MONEY32_UNDEFINED;
            }
        }

        _placementLoc = trackLoc;
        if (cost != _placementCost)
        {
            _placementCost = cost;
            WidgetInvalidate(*this, WIDX_PRICE);
        }

        TrackDesignPreviewDrawOutlines(tds, _trackDesign.get(), *GetOrAllocateRide(PreviewRideId), trackLoc);
    }

    void OnToolDown(WidgetIndex widgetIndex, const ScreenCoordsXY& screenCoords) override
    {
        ClearProvisional();
        MapInvalidateMapSelectionTiles();
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_CONSTRUCT;
        gMapSelectFlags &= ~MAP_SELECT_FLAG_ENABLE_ARROW;

        const CoordsXY mapCoords = ViewportInteractionGetTileStartAtCursor(screenCoords);
        if (mapCoords.IsNull())
            return;

        // Try increasing Z until a feasible placement is found
        int16_t mapZ = GetBaseZ(mapCoords);
        CoordsXYZ trackLoc = { mapCoords, mapZ };

        auto res = FindValidTrackDesignPlaceHeight(trackLoc, 0);
        if (res.Error == GameActions::Status::Ok)
        {
            auto tdAction = TrackDesignAction({ trackLoc, _currentTrackPieceDirection }, *_trackDesign);
            tdAction.SetCallback([&](const GameAction*, const GameActions::Result* result) {
                if (result->Error == GameActions::Status::Ok)
                {
                    rideId = result->GetData<RideId>();
                    auto getRide = get_ride(rideId);
                    if (getRide != nullptr)
                    {
                        WindowCloseByClass(WindowClass::Error);
                        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::PlaceItem, trackLoc);

                        _currentRideIndex = rideId;
                        if (track_design_are_entrance_and_exit_placed())
                        {
                            auto intent = Intent(WindowClass::Ride);
                            intent.putExtra(INTENT_EXTRA_RIDE_ID, rideId.ToUnderlying());
                            ContextOpenIntent(&intent);
                            auto wnd = WindowFindByClass(WindowClass::TrackDesignPlace);
                            WindowClose(*wnd);
                        }
                        else
                        {
                            ride_initialise_construction_window(*getRide);
                            auto wnd = WindowFindByClass(WindowClass::RideConstruction);
                            WindowEventMouseUpCall(wnd, WC_RIDE_CONSTRUCTION__WIDX_ENTRANCE);
                        }
                    }
                }
                else
                {
                    OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Error, result->Position);
                }
            });
            GameActions::Execute(&tdAction);
            return;
        }

        // Unable to build track
        OpenRCT2::Audio::Play3D(OpenRCT2::Audio::SoundId::Error, trackLoc);

        auto windowManager = GetContext()->GetUiContext()->GetWindowManager();
        windowManager->ShowError(res.GetErrorTitle(), res.GetErrorMessage());
    }

    void OnToolAbort(WidgetIndex widgetIndex) override
    {
        ClearProvisional();
    }

    void OnViewportRotate() override
    {
        DrawMiniPreview(_trackDesign.get());
    }

    void OnPrepareDraw() override
    {
        DrawMiniPreview(_trackDesign.get());
    }

    void OnDraw(rct_drawpixelinfo& dpi) override
    {
        auto ft = Formatter::Common();
        ft.Add<char*>(_trackDesign->name.c_str());
        WindowDrawWidgets(*this, &dpi);

        // Draw mini tile preview
        rct_drawpixelinfo clippedDpi;
        if (ClipDrawPixelInfo(&clippedDpi, &dpi, this->windowPos + ScreenCoordsXY{ 4, 18 }, 168, 78))
        {
            rct_g1_element g1temp = {};
            g1temp.offset = _miniPreview.data();
            g1temp.width = TRACK_MINI_PREVIEW_WIDTH;
            g1temp.height = TRACK_MINI_PREVIEW_HEIGHT;
            GfxSetG1Element(SPR_TEMP, &g1temp);
            DrawingEngineInvalidateImage(SPR_TEMP);
            GfxDrawSprite(&clippedDpi, ImageId(SPR_TEMP, NOT_TRANSLUCENT(this->colours[0])), { 0, 0 });
        }

        // Price
        if (_placementCost != MONEY32_UNDEFINED && !(gParkFlags & PARK_FLAGS_NO_MONEY))
        {
            ft = Formatter();
            ft.Add<money64>(_placementCost);
            DrawTextBasic(&dpi, this->windowPos + ScreenCoordsXY{ 88, 94 }, STR_COST_LABEL, ft, { TextAlignment::CENTRE });
        }
    }

    void ClearProvisionalTemporarily()
    {
        if (_hasPlacementGhost)
        {
            auto provRide = get_ride(_placementGhostRideId);
            if (provRide != nullptr)
            {
                TrackDesignPreviewRemoveGhosts(_trackDesign.get(), *provRide, _placementGhostLoc);
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

    void DrawMiniPreview(TrackDesign* td6)
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
                origin.x -= ((max.x + min.x) >> 6) * COORDS_XY_STEP;
                origin.y -= ((max.y + min.y) >> 6) * COORDS_XY_STEP;
            }

            const auto& rtd = GetRideTypeDescriptor(td6->type);
            if (rtd.HasFlag(RIDE_TYPE_FLAG_IS_MAZE))
            {
                DrawMiniPreviewMaze(td6, pass, origin, min, max);
            }
            else
            {
                DrawMiniPreviewTrack(td6, pass, origin, min, max);
            }
        }
    }

    void ClearMiniPreview()
    {
        // Fill with transparent colour.
        std::fill(_miniPreview.begin(), _miniPreview.end(), PALETTE_INDEX_0);
    }

private:
    std::unique_ptr<TrackDesign> _trackDesign;

    CoordsXY _placementLoc;
    RideId _placementGhostRideId;
    bool _hasPlacementGhost;
    money32 _placementCost;
    CoordsXYZD _placementGhostLoc;

    std::vector<uint8_t> _miniPreview;

    void ClearProvisional()
    {
        if (_hasPlacementGhost)
        {
            auto newRide = get_ride(_placementGhostRideId);
            if (newRide != nullptr)
            {
                TrackDesignPreviewRemoveGhosts(_trackDesign.get(), *newRide, _placementGhostLoc);
                _hasPlacementGhost = false;
            }
        }
    }

    int32_t GetBaseZ(const CoordsXY& loc)
    {
        auto surfaceElement = MapGetSurfaceElementAt(loc);
        if (surfaceElement == nullptr)
            return 0;

        auto z = surfaceElement->GetBaseZ();

        // Increase Z above slope
        if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_ALL_CORNERS_UP)
        {
            z += 16;

            // Increase Z above double slope
            if (surfaceElement->GetSlope() & TILE_ELEMENT_SLOPE_DOUBLE_HEIGHT)
                z += 16;
        }

        // Increase Z above water
        if (surfaceElement->GetWaterHeight() > 0)
            z = std::max(z, surfaceElement->GetWaterHeight());

        return z
            + TrackDesignGetZPlacement(
                   _trackDesign.get(), *GetOrAllocateRide(PreviewRideId), { loc, z, _currentTrackPieceDirection });
    }

    void DrawMiniPreviewTrack(TrackDesign* td6, int32_t pass, const CoordsXY& origin, CoordsXY min, CoordsXY max)
    {
        const uint8_t rotation = (_currentTrackPieceDirection + GetCurrentRotation()) & 3;

        CoordsXY curTrackStart = origin;
        uint8_t curTrackRotation = rotation;
        for (const auto& trackElement : td6->track_elements)
        {
            int32_t trackType = trackElement.type;
            if (trackType == TrackElemType::InvertedUp90ToFlatQuarterLoopAlias)
            {
                trackType = TrackElemType::MultiDimInvertedUp90ToFlatQuarterLoop;
            }

            // Follow a single track piece shape
            const auto& ted = GetTrackElementDescriptor(trackType);
            const rct_preview_track* trackBlock = ted.Block;
            while (trackBlock->index != 255)
            {
                auto rotatedAndOffsetTrackBlock = curTrackStart
                    + CoordsXY{ trackBlock->x, trackBlock->y }.Rotate(curTrackRotation);

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

                        auto bits = trackBlock->var_08.Rotate(curTrackRotation & 3).GetBaseQuarterOccupied();

                        // Station track is a lighter colour
                        uint8_t colour = (ted.SequenceProperties[0] & TRACK_SEQUENCE_FLAG_ORIGIN) ? _PaletteIndexColourStation
                                                                                                  : _PaletteIndexColourTrack;

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
                trackBlock++;
            }

            // Change rotation and next position based on track curvature
            curTrackRotation &= 3;

            const rct_track_coordinates* track_coordinate = &ted.Coordinates;

            curTrackStart += CoordsXY{ track_coordinate->x, track_coordinate->y }.Rotate(curTrackRotation);
            curTrackRotation += track_coordinate->rotation_end - track_coordinate->rotation_begin;
            curTrackRotation &= 3;
            if (track_coordinate->rotation_end & 4)
            {
                curTrackRotation |= 4;
            }
            if (!(curTrackRotation & 4))
            {
                curTrackStart += CoordsDirectionDelta[curTrackRotation];
            }
        }

        // Draw entrance and exit preview.
        for (const auto& entrance : td6->entrance_elements)
        {
            auto rotatedAndOffsetEntrance = origin + CoordsXY{ entrance.x, entrance.y }.Rotate(rotation);

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
                    uint8_t colour = entrance.isExit ? _PaletteIndexColourExit : _PaletteIndexColourEntrance;
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

    void DrawMiniPreviewMaze(TrackDesign* td6, int32_t pass, const CoordsXY& origin, CoordsXY min, CoordsXY max)
    {
        uint8_t rotation = (_currentTrackPieceDirection + GetCurrentRotation()) & 3;
        for (const auto& mazeElement : td6->maze_elements)
        {
            auto rotatedMazeCoords = origin + TileCoordsXY{ mazeElement.x, mazeElement.y }.ToCoordsXY().Rotate(rotation);

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

                    uint8_t colour = _PaletteIndexColourTrack;

                    // Draw entrance and exit with different colours.
                    if (mazeElement.type == MAZE_ELEMENT_TYPE_ENTRANCE)
                        colour = _PaletteIndexColourEntrance;
                    else if (mazeElement.type == MAZE_ELEMENT_TYPE_EXIT)
                        colour = _PaletteIndexColourExit;

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
        for (int32_t i = 0; i < 7; i++, loc.z += 8)
        {
            auto tdAction = TrackDesignAction(CoordsXYZD{ loc.x, loc.y, loc.z, _currentTrackPieceDirection }, *_trackDesign);
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

rct_window* WindowTrackPlaceOpen(const TrackDesignFileRef* tdFileRef)
{
    std::unique_ptr<TrackDesign> openTrackDesign = TrackDesignImport(tdFileRef->path.c_str());

    if (openTrackDesign == nullptr)
    {
        return nullptr;
    }

    WindowCloseConstructionWindows();

    auto* window = WindowFocusOrCreate<TrackDesignPlaceWindow>(WindowClass::TrackDesignPlace, WW, WH, 0);
    if (window != nullptr)
    {
        window->Init(std::move(openTrackDesign));
    }
    return window;
}

void TrackPlaceClearProvisionalTemporarily()
{
    auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(WindowFindByClass(WindowClass::TrackDesignPlace));
    if (trackPlaceWnd != nullptr)
    {
        trackPlaceWnd->ClearProvisionalTemporarily();
    }
}

void TrackPlaceRestoreProvisional()
{
    auto* trackPlaceWnd = static_cast<TrackDesignPlaceWindow*>(WindowFindByClass(WindowClass::TrackDesignPlace));
    if (trackPlaceWnd != nullptr)
    {
        trackPlaceWnd->RestoreProvisional();
    }
}
