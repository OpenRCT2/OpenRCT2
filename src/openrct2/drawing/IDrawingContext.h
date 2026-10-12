/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "FilterPaletteIds.h"
#include "ImageId.hpp"
#include "PaletteIndex.h"

struct ScreenLine;
struct TTFSurface;

namespace OpenRCT2::Drawing
{
    struct IDrawingEngine;
    struct PaletteMap;
    struct RenderTarget;
    struct TextDrawInfo;

    /**
     * How a sprite's pixels map to world positions in the world-depth G-buffer, which the drop
     * shadow resolve reads. The buffer layout is documented in data/shaders/worlddepth.glsl,
     * and these values must match the DEPTH_MODE_* constants there.
     */
    enum class SpriteWorldDepthMode : uint8_t
    {
        none = 0,
        flat = 1,
        billboard = 2,
        /** Thin upright box along one tile edge (walls/fences). Unlike a billboard this has
            a real 32-unit footprint, so its XY must be solved per pixel or its shadow is
            cast from a single point. */
        wall = 3,
        /** Painted as part of the world but with no world position of its own: labels, price
            text, placement ghosts. Must leave the ground's depth record alone rather than
            zeroing it, or it punches an unshaded hole through the shadow it sits on. Distinct
            from a UI draw, which has no hint at all and *should* clear the record. */
        worldNone = 4,
        /** The heightfield itself - terrain tops and water. Solved exactly like `flat`, and
            receives exactly like it; the distinction is that it can never *cast*. A sun ray
            climbs one unit of z per two of (u+v), and a one-step RCT slope climbs at that
            same rate, so the heightfield can never rise above its own light. Its shadows come
            from its vertical faces, which are walls. A deck or a path stays `flat`, because a
            surface floating clear of the ground occludes by the ray crossing it and must keep
            casting. */
        terrain = 5,
        /** A terrain tile whose top caps an actual height discontinuity. It may be flat or
            sloped; continuous terrain stays in `terrain`. */
        terrainCap = 6,
        /** A roof prop matched to a simple solid (wedge, ridge, barrel, hip or flat), carried
            in `slope` as a shape code. Its top is solved per pixel against that solid and
            receives like a floor; its gable ends are written as billboard. */
        roof = 7,
        /** A bulky prop solved against its paint bound box: building-like rides, stalls,
            entrances, large scenery tiles, full-tile props. Each pixel takes the point where its
            view ray enters the box, so the faces carry their true heights - as a card through
            the centre, the near bottom corner sat below the card and cast nothing. Carries the
            box as centre = min corner, baseZ = bottom, slope = sizes (x | y << 6 | z << 12).
            Written to the G-buffer as billboard. */
        box = 8,
    };

    struct SpriteWorldDepthHint
    {
        float zMin = 0.0f;
        float zMax = 0.0f;
        /** Billboard: bound centre. Flat: tile origin (MapPos). */
        float centreX = 0.0f;
        float centreY = 0.0f;
        /** Flat terrain: base Z (world) and raw slope byte for TileElementHeight. */
        float baseZ = 0.0f;
        float slope = 0.0f;
        float screenTop = 0.0f;
        float screenBottom = 0.0f;
        SpriteWorldDepthMode mode = SpriteWorldDepthMode::none;
        /** Viewport rotation this hint was built for; travels per-command so viewports
            with different rotations can share one G-buffer. */
        uint8_t rotation = 0;
        /** Wall: 0 = the edge runs along world X, 1 = along world Y. */
        uint8_t wallAxis = 0;
        /** Wall: bit 0 / bit 1 set when the edge sits a full tile along X / Y from the tile
            that owns it. Without this the shader cannot tell which tile a boundary wall
            belongs to, and a pillar's own side gets credited to the tile it shadows. */
        uint8_t wallTileShift = 0;
        /** Whether the visible wall face points toward the sun. Terrain paint supplies the
            actual edge; ordinary wall sprites fall back to their oriented bound box. */
        bool wallFacesSun = false;
        /** The wall was emitted by a terrain tile side rather than a placed wall object.
            This lets the resolve clip the last rasterised wall texels against an adjoining
            terrain edge whose height already covers them. */
        bool wallIsTerrainSide = false;
        /** Billboard, shadow pass only: view-unit offset to draw the sprite at, so its root
            lands where the camera's image of the same object has its root. */
        int16_t drawOffsetX = 0;
        int16_t drawOffsetY = 0;
        bool valid = false;
    };

    struct IDrawingContext
    {
        virtual ~IDrawingContext() = default;

        virtual void SetSpriteWorldDepthHint(const SpriteWorldDepthHint& hint)
        {
            (void)hint;
        }

        virtual void Clear(RenderTarget& rt, PaletteIndex paletteIndex) = 0;
        virtual void FillRect(
            RenderTarget& rt, PaletteIndex paletteIndex, int32_t left, int32_t top, int32_t right, int32_t bottom,
            bool crossHatch = false)
            = 0;
        virtual void FilterRect(
            RenderTarget& rt, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom)
            = 0;
        virtual void DrawLine(RenderTarget& rt, PaletteIndex colour, const ScreenLine& line) = 0;
        virtual void DrawSprite(RenderTarget& rt, ImageId image, int32_t x, int32_t y) = 0;
        virtual void DrawSpriteRawMasked(RenderTarget& rt, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage) = 0;
        virtual void DrawSpriteSolid(RenderTarget& rt, ImageId image, int32_t x, int32_t y, PaletteIndex colour) = 0;
        virtual void DrawGlyph(RenderTarget& rt, ImageId image, int32_t x, int32_t y, const PaletteMap& palette) = 0;
        virtual void DrawTTFBitmap(
            RenderTarget& rt, const TextDrawInfo& info, TTFSurface* surface, int32_t x, int32_t y, uint8_t hintingThreshold)
            = 0;
    };

} // namespace OpenRCT2::Drawing
