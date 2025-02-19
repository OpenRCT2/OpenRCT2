/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ParkPreview.h"

#include "../GameState.h"
#include "../core/Numerics.hpp"
#include "../drawing/Drawing.h"
#include "../ride/RideManager.hpp"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"

namespace OpenRCT2
{
    void ParkPreview::clear()
    {
        info.clear();
        images.clear();
    }

    static PreviewImage generatePreviewMap();

    ParkPreview generatePreviewFromGameState(const GameState_t& gameState)
    {
        ParkPreview preview{};

        // TODO: extend
        preview.info.push_back({ PreviewInfoKind::numGuests, gameState.NumGuestsInPark });
        preview.info.push_back({ PreviewInfoKind::numRides, RideManager().size() });

        // TODO: extend
        preview.images.push_back(generatePreviewMap());

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
    static PreviewImage generatePreviewMap()
    {
        const auto& gameState = GetGameState();
        const auto kPreviewSize = 128; // sizeof(entry.preview[0]);
        const auto kMapSkipFactor = Numerics::ceil2(gameState.MapSize.x, kPreviewSize) / kPreviewSize;

        PreviewImage image{
            .type = PreviewImageType::miniMap,
            .width = kPreviewSize,
            .height = kPreviewSize,
        };

        for (auto y = 0u; y < image.height; y++)
        {
            for (auto x = 0u; x < image.width; x++)
            {
                auto pos = TileCoordsXY(gameState.MapSize.x - (x + 1) * kMapSkipFactor + 1, y * kMapSkipFactor + 1);

                // TODO: 2D array
                image.pixels[y * kPreviewSize + x] = getPreviewColourByTilePos(pos);
            }
        }

        return image;
    }

    void drawPreviewImage(const PreviewImage& image, DrawPixelInfo& dpi, ScreenCoordsXY screenPos)
    {
        const auto imageId = ImageId(0);
        auto* g1 = const_cast<G1Element*>(GfxGetG1Element(imageId));
        if (g1 != nullptr)
        {
            // Temporarily substitute a G1 image with the data in the scenario index
            const auto backupG1 = *g1;
            *g1 = {};
            g1->offset = const_cast<uint8_t*>(image.pixels);
            g1->width = image.width;
            g1->height = image.height;

            // Draw preview image and restore original G1 image.
            GfxDrawSprite(dpi, imageId, screenPos);
            *g1 = backupG1;

            // Draw compass
            // GfxDrawSprite(dpi, ImageIds::height_map_compass, screenPos);
        }
    }

} // namespace OpenRCT2
