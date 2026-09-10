/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Palette.h"

#include "../Game.h"
#include "../GameState.h"
#include "../OpenRCT2.h"
#include "../SpriteIds.h"
#include "../config/Config.h"
#include "../object/WaterEntry.h"
#include "../util/Util.h"
#include "../world/Weather.h"
#include "Drawing.Screen.h"
#include "Drawing.Sprite.h"
#include "FilterPaletteIds.h"
#include "LightFX.h"
#include "NewDrawing.h"

namespace OpenRCT2::Drawing
{
    static constexpr auto kPaletteOffsetDynamic = PaletteIndex::pi10;
    static constexpr uint8_t kPaletteLengthDynamic = 236;

    static constexpr uint8_t kPaletteLengthWaterWaves = 5;
    static constexpr uint8_t kPaletteLengthWaterSparkles = 5;

    static constexpr auto kPaletteOffsetAnimated = PaletteIndex::waterWaves0;
    static constexpr uint8_t kPaletteLengthAnimated = 16;

    GamePalette gPalette;
    GamePalette gGamePalette;
    uint32_t gPaletteEffectFrame;

    void UpdatePalette(std::span<const BGRAColour> palette, PaletteIndex startIndex, int32_t numColours)
    {
        for (int32_t i = EnumValue(startIndex); i < numColours + EnumValue(startIndex); i++)
        {
            const auto& colour = palette[i];
            uint8_t b = colour.blue;
            uint8_t g = colour.green;
            uint8_t r = colour.red;

            if (LightFx::IsAvailable())
            {
                LightFx::ApplyPaletteFilter(i, &r, &g, &b);
            }
            else
            {
                float night = gDayNightCycle;
                if (night >= 0 && Weather::gLightningFlash == 0)
                {
                    r = Lerp(r, SoftLight(r, 8), night);
                    g = Lerp(g, SoftLight(g, 8), night);
                    b = Lerp(b, SoftLight(b, 128), night);
                }
            }

            gPalette[i].blue = b;
            gPalette[i].green = g;
            gPalette[i].red = r;
            gPalette[i].alpha = 0;
        }

        // Fix #1749 and #6535: rainbow path, donut shop and pause button contain black spots that should be white.
        gPalette[255].blue = 255;
        gPalette[255].green = 255;
        gPalette[255].red = 255;
        gPalette[255].alpha = 0;

        if (!gOpenRCT2Headless)
        {
            DrawingEngineSetPalette(gPalette);
        }
    }

    /**
     *
     *  rct2: 0x006838BD
     */
    void UpdatePaletteEffects()
    {
        const auto& waterEntry = getActiveWaterEntry();

        if (Weather::gLightningFlash == 1)
        {
            // Change palette to lighter colour during lightning
            auto palette = waterEntry.mainPalette;
            const auto* g1 = GfxGetG1Palette(palette);
            if (g1 != nullptr)
            {
                auto startIndex = g1->startIndex;

                for (int32_t i = 0; i < g1->numColours; i++)
                {
                    auto& paletteOffset = gGamePalette[startIndex + i];
                    const auto& g1PaletteEntry = g1->palette[i];
                    paletteOffset.blue = -((0xFF - g1PaletteEntry.blue) / 2) - 1;
                    paletteOffset.green = -((0xFF - g1PaletteEntry.green) / 2) - 1;
                    paletteOffset.red = -((0xFF - g1PaletteEntry.red) / 2) - 1;
                }

                UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
            }
            Weather::gLightningFlash++;
        }
        else
        {
            if (Weather::gLightningFlash == 2)
            {
                // Change palette back to normal after lightning
                auto palette = waterEntry.mainPalette;
                const auto* g1 = GfxGetG1Palette(palette);
                if (g1 != nullptr)
                {
                    auto startIndex = g1->startIndex;

                    for (int32_t i = 0; i < g1->numColours; i++)
                    {
                        auto& paletteOffset = gGamePalette[startIndex + i];
                        const auto& g1PaletteEntry = g1->palette[i];
                        paletteOffset.blue = g1PaletteEntry.blue;
                        paletteOffset.green = g1PaletteEntry.green;
                        paletteOffset.red = g1PaletteEntry.red;
                    }
                }
            }

            // Animate the water/lava/chain movement palette
            uint32_t shade = 0;
            if (Config::Get().general.renderWeatherGloom)
            {
                auto paletteId = Weather::getWeatherGloomPaletteId(getGameState().weatherCurrent);
                if (paletteId != FilterPaletteID::paletteNull)
                {
                    shade = 1;
                    if (paletteId != FilterPaletteID::paletteDarken1)
                    {
                        shade = 2;
                    }
                }
            }
            uint32_t j = gPaletteEffectFrame;
            j = ((static_cast<uint16_t>((~j / 2) * 128) * 15) >> 16);

            auto waterId = waterEntry.waterWavesPalette;
            const auto* g1 = GfxGetG1Palette(shade + waterId);
            if (g1 != nullptr)
            {
                const auto* g1PaletteEntry = &g1->palette[j];
                int32_t n = kPaletteLengthWaterWaves;
                for (int32_t i = 0; i < n; i++)
                {
                    auto& vd = gGamePalette[EnumValue(PaletteIndex::waterWaves0) + i];
                    vd.blue = g1PaletteEntry->blue;
                    vd.green = g1PaletteEntry->green;
                    vd.red = g1PaletteEntry->red;
                    g1PaletteEntry += 3;
                    if (g1PaletteEntry >= &g1->palette[3 * n])
                    {
                        g1PaletteEntry -= 3 * n;
                    }
                }
            }

            waterId = waterEntry.waterSparklesPalette;
            g1 = GfxGetG1Palette(shade + waterId);
            if (g1 != nullptr)
            {
                auto* src = &g1->palette[j];
                int32_t n = kPaletteLengthWaterSparkles;
                for (int32_t i = 0; i < n; i++)
                {
                    auto& vd = gGamePalette[EnumValue(PaletteIndex::waterSparkles0) + i];
                    vd.blue = src->blue;
                    vd.green = src->green;
                    vd.red = src->red;
                    src += 3;
                    if (src >= &g1->palette[3 * n])
                    {
                        src -= 3 * n;
                    }
                }
            }

            j = (static_cast<uint16_t>(gPaletteEffectFrame * -960) * 3) >> 16;
            waterId = SPR_GAME_PALETTE_4;
            g1 = GfxGetG1Palette(shade + waterId);
            if (g1 != nullptr)
            {
                auto* src = &g1->palette[j];
                const int32_t n = 3;
                for (int32_t i = 0; i < n; i++)
                {
                    auto& vd = gGamePalette[EnumValue(PaletteIndex::primaryRemap0) + i];
                    vd.blue = src->blue;
                    vd.green = src->green;
                    vd.red = src->red;
                    src++;
                    if (src >= &g1->palette[3])
                    {
                        src -= n;
                    }
                }
            }

            UpdatePalette(gGamePalette, kPaletteOffsetAnimated, kPaletteLengthAnimated);
            if (Weather::gLightningFlash == 2)
            {
                UpdatePalette(gGamePalette, kPaletteOffsetDynamic, kPaletteLengthDynamic);
                Weather::gLightningFlash = 0;
            }
        }
    }

    /**
     *
     *  rct2: 0x00683854
     * a1 (ebx)
     * product (cl)
     */
    void GfxTransposePalette(ImageIndex pal, uint8_t product)
    {
        const auto* g1 = GfxGetG1Palette(pal);
        if (g1 == nullptr)
            return;

        auto index = g1->startIndex;
        auto* src = g1->palette;

        for (auto numColours = g1->numColours; numColours > 0; numColours--)
        {
            auto& dst = gGamePalette[index];
            // Make sure the image never gets darker than the void colour (not-quite-black), to avoid the background colour
            // jumping between void and 100% black.
            dst.blue = std::max<uint8_t>(35, ((src->blue * product) >> 8));
            dst.green = std::max<uint8_t>(35, ((src->green * product) >> 8));
            dst.red = std::max<uint8_t>(23, ((src->red * product) >> 8));
            src++;

            index++;
        }
        UpdatePalette(gGamePalette, PaletteIndex::pi10, 236);
    }

    /**
     *
     *  rct2: 0x006837E3
     */
    void LoadPalette()
    {
        if (gOpenRCT2NoGraphics)
        {
            return;
        }

        const auto& waterEntry = getActiveWaterEntry();
        Guard::Assert(waterEntry.mainPalette != kImageIndexUndefined, "Failed to load water palette");
        auto palette = waterEntry.mainPalette;

        const auto* g1 = GfxGetG1Palette(palette);
        if (g1 != nullptr)
        {
            auto index = g1->startIndex;
            auto* src = g1->palette;
            for (auto numColours = g1->numColours; numColours > 0; numColours--)
            {
                auto& dst = gGamePalette[index];
                dst.blue = src->blue;
                dst.green = src->green;
                dst.red = src->red;
                src++;
                index++;
            }
        }
        UpdatePalette(gGamePalette, PaletteIndex::pi10, 236);
        GfxInvalidateScreen();
    }
} // namespace OpenRCT2::Drawing
