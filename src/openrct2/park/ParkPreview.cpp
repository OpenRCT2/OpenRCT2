/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkPreview.h"

#include "../Context.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../core/Numerics.hpp"
#include "../drawing/Drawing.h"
#include "../drawing/X8DrawingEngine.h"
#include "../interface/Viewport.h"
#include "../interface/Window.h"
#include "../ride/RideManager.hpp"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"

#include <optional>

namespace OpenRCT2
{
    static std::optional<PreviewImage> generatePreviewMap();
    static std::optional<PreviewImage> generatePreviewScreenshot();

    ParkPreview generatePreviewFromGameState(const GameState_t& gameState)
    {
        ParkPreview preview{
            .parkName = gameState.Park.Name,
            .parkRating = gameState.Park.Rating,
            .year = gameState.Date.GetYear(),
            .month = gameState.Date.GetMonth(),
            .day = gameState.Date.GetDay(),
            .parkUsesMoney = !(gameState.Park.Flags & PARK_FLAGS_NO_MONEY),
            .cash = gameState.Cash,
            .numRides = static_cast<uint16_t>(RideManager().size()),
            .numGuests = static_cast<uint16_t>(gameState.NumGuestsInPark),
        };

        if (auto image = generatePreviewMap(); image != std::nullopt)
            preview.images.push_back(*image);

        if (auto image = generatePreviewScreenshot(); image != std::nullopt)
            preview.images.push_back(*image);

        return preview;
    }

    static PaletteIndex getPreviewColourByTilePos(const TileCoordsXY& pos)
    {
        PaletteIndex colour = PALETTE_INDEX_0;

        auto tileElement = MapGetFirstElementAt(pos);
        if (tileElement == nullptr)
            return colour;

        do
        {
            switch (tileElement->GetType())
            {
                case TileElementType::Surface:
                {
                    auto* surfaceElement = tileElement->AsSurface();
                    if (surfaceElement == nullptr)
                    {
                        colour = PALETTE_INDEX_0;
                        break;
                    }

                    if (surfaceElement->GetWaterHeight() > 0)
                    {
                        colour = PALETTE_INDEX_195;
                        break;
                    }

                    const auto* surfaceObject = surfaceElement->GetSurfaceObject();
                    if (surfaceObject != nullptr)
                    {
                        colour = surfaceObject->MapColours[1];
                    }
                    break;
                }

                case TileElementType::Path:
                    colour = PALETTE_INDEX_17;
                    break;

                case TileElementType::Track:
                    colour = PALETTE_INDEX_183;
                    break;

                case TileElementType::SmallScenery:
                case TileElementType::LargeScenery:
                    colour = PALETTE_INDEX_99; // 64
                    break;

                case TileElementType::Entrance:
                    colour = PALETTE_INDEX_186;
                    break;

                default:
                    break;
            }
        } while (!(tileElement++)->IsLastForTile());

        return colour;
    }

    // 0x0046DB4C
    static std::optional<PreviewImage> generatePreviewMap()
    {
        const auto& gameState = GetGameState();
        const auto previewSize = 128;
        const auto longEdgeSize = std::max(gameState.MapSize.x, gameState.MapSize.y);
        const auto nearestPower = Numerics::ceil2(longEdgeSize, previewSize);
        const auto mapSkipFactor = nearestPower / previewSize;
        const auto offset = mapSkipFactor > 0 ? (nearestPower - longEdgeSize) / mapSkipFactor : 1;

        PreviewImage image{
            .type = PreviewImageType::miniMap,
            .width = previewSize,
            .height = previewSize,
        };

        for (auto y = 0u; y < image.height; y++)
        {
            for (auto x = 0u; x < image.width; x++)
            {
                auto pos = TileCoordsXY(gameState.MapSize.x - (x + 1) * mapSkipFactor + 1, y * mapSkipFactor + 1);

                image.pixels[(y + offset) * previewSize + (x + offset)] = getPreviewColourByTilePos(pos);
            }
        }

        return image;
    }

    static std::optional<PreviewImage> generatePreviewScreenshot()
    {
        if (gOpenRCT2NoGraphics)
            return std::nullopt;

        const auto& gameState = GetGameState();
        const auto mainWindow = WindowGetMain();
        const auto mainViewport = WindowGetViewport(mainWindow);

        CoordsXYZ mapPosXYZ{};
        if (mainViewport != nullptr)
        {
            const auto centre = mainViewport->viewPos
                + ScreenCoordsXY{ mainViewport->ViewWidth() / 2, mainViewport->ViewHeight() / 2 };
            const auto mapPos = ViewportPosToMapPos(centre, 24, mainViewport->rotation);
            mapPosXYZ = CoordsXYZ(mapPos.x, mapPos.y, int32_t{ TileElementHeight(mapPos) });
        }
        else if (!gameState.Park.Entrances.empty())
        {
            const auto& entrance = gameState.Park.Entrances[0];
            mapPosXYZ = CoordsXYZ{ entrance.x + 16, entrance.y + 16, entrance.z + 32 };
        }
        else
            return std::nullopt;

        PreviewImage image{
            .type = PreviewImageType::screenshot,
            .width = kMaxPreviewImageSize,
            .height = kMaxPreviewImageSize / 5 * 4, // 5:4 ratio
        };

        Viewport saveVp{
            .width = image.width,
            .height = image.height,
            .flags = 0,
            .zoom = ZoomLevel{ 1 },
            .rotation = mainViewport->rotation,
        };

        auto viewPos = centre_2d_coordinates(mapPosXYZ, &saveVp);
        if (viewPos == std::nullopt)
            return std::nullopt;

        saveVp.viewPos = *viewPos;

        auto drawingEngine = std::make_unique<Drawing::X8DrawingEngine>(GetContext()->GetUiContext());
        if (!drawingEngine)
            return std::nullopt;

        DrawPixelInfo dpi{
            .bits = static_cast<uint8_t*>(image.pixels),
            .x = 0,
            .y = 0,
            .width = image.width,
            .height = image.height,
            .pitch = 0,
            .zoom_level = saveVp.zoom,
            .DrawingEngine = drawingEngine.get(),
        };

        ViewportRender(dpi, &saveVp);

        return image;
    }

    void drawPreviewImage(const PreviewImage& image, DrawPixelInfo& dpi, ScreenCoordsXY screenPos)
    {
        auto* drawingEngine = GetContext()->GetDrawingEngine();
        if (drawingEngine == nullptr)
            return;

        const auto imageId = ImageId(0);
        auto* g1 = const_cast<G1Element*>(GfxGetG1Element(imageId));
        if (g1 != nullptr)
        {
            // Temporarily substitute a G1 image with the data in the preview image
            const auto backupG1 = *g1;
            *g1 = {};
            g1->offset = const_cast<uint8_t*>(image.pixels);
            g1->width = image.width;
            g1->height = image.height;
            drawingEngine->InvalidateImage(imageId.GetIndex());

            // Draw preview image and restore original G1 image
            GfxDrawSprite(dpi, imageId, screenPos);
            *g1 = backupG1;
            drawingEngine->InvalidateImage(imageId.GetIndex());
        }
    }

} // namespace OpenRCT2
