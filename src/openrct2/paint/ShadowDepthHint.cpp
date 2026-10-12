/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShadowDepthHint.h"

#include "../drawing/Drawing.Sprite.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityBase.h"
#include "../interface/Viewport.h"
#include "../object/SmallSceneryEntry.h"
#include "../world/tile_element/BannerElement.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/TileElement.h"
#include "Paint.h"
#include "ShadowRoofFit.h"
#include "ShadowSpriteRoot.h"
#include "tile_element/Paint.TileElement.h"

#include <algorithm>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

namespace
{
    SpriteWorldDepthMode DepthModeForInteraction(ViewportInteractionItem item)
    {
        switch (item)
        {
            case ViewportInteractionItem::terrain:
            case ViewportInteractionItem::water:
                // The heightfield itself: solved and shaded as a plane, but it never casts - it
                // cannot rise above its own light ray. See SpriteWorldDepthMode::terrain.
                return SpriteWorldDepthMode::terrain;
            case ViewportInteractionItem::footpath:
                // Solved like the terrain, but not part of the heightfield: a raised deck stands
                // clear of the ground and casts onto it.
                return SpriteWorldDepthMode::flat;
            case ViewportInteractionItem::wall:
                // A thin upright box along one tile edge. As a card all 32 units of it collapse
                // onto one world XY and cast a point-sized shadow.
                return SpriteWorldDepthMode::wall;
            case ViewportInteractionItem::scenery:
            case ViewportInteractionItem::largeScenery:
            case ViewportInteractionItem::entity:
            case ViewportInteractionItem::ride:
            case ViewportInteractionItem::pathAddition:
            case ViewportInteractionItem::parkEntrance:
            case ViewportInteractionItem::banner:
                // Standing geometry, refined below.
                return SpriteWorldDepthMode::billboard;
            default:
                // Labels and unknown paint: no cast, no receive - but still part of the world, so
                // they must preserve whatever the ground wrote underneath them.
                return SpriteWorldDepthMode::worldNone;
        }
    }

    /** ps.Entity is only meaningful on an entity's own paint: the session's current entity is not
        cleared before tile elements paint, so terrain and scenery carry a stale one. And of
        entities, only peeps: their bound is an offset for sorting, while a ride vehicle's bound
        describes its shape - a swinging ship's, a Top Spin's. */
    const EntityBase* PeepOf(const PaintStruct& ps)
    {
        if (ps.InteractionItem != ViewportInteractionItem::entity || ps.Entity == nullptr)
            return nullptr;
        return ps.Entity->type == EntityType::guest || ps.Entity->type == EntityType::staff ? ps.Entity : nullptr;
    }

    /** Ducks, released balloons and train smoke use point-sized paint bounds but should still cast;
        every other point entity - litter, effects - keeps its depthless behaviour. */
    bool IsPointEntityShadowCaster(const PaintStruct& ps)
    {
        if (ps.Entity == nullptr || ps.InteractionItem != ViewportInteractionItem::entity)
            return false;
        switch (ps.Entity->type)
        {
            case EntityType::steamParticle:
            case EntityType::balloon:
            case EntityType::duck:
                return true;
            default:
                return false;
        }
    }

    bool IsFullTileScenery(const PaintStruct& ps)
    {
        if (ps.InteractionItem != ViewportInteractionItem::scenery || ps.Element == nullptr
            || ps.Element->getType() != TileElementType::smallScenery)
            return false;
        const auto* entry = ps.Element->asSmallScenery()->getEntry();
        return entry != nullptr && entry->flags.has(SmallSceneryFlag::occupiesFullTile);
    }

    /** Props that stand in their bound box well enough to be solved against it. */
    bool IsBulky(const PaintStruct& ps)
    {
        return ps.InteractionItem == ViewportInteractionItem::ride
            || ps.InteractionItem == ViewportInteractionItem::largeScenery
            || ps.InteractionItem == ViewportInteractionItem::parkEntrance || IsFullTileScenery(ps);
    }

    /** Puts a wall on one boundary of a tile: 0 = +X, 1 = +Y, 2 = -X, 3 = -Y. The shader walks the
        32-unit edge from its low corner, and needs the tile that owns it - an edge on the far side
        of a tile reads, by its own coordinates, as the next tile along. */
    void PlaceOnTileEdge(SpriteWorldDepthHint& hint, CoordsXY tile, int32_t edge)
    {
        hint.centreX = static_cast<float>(tile.x + (edge == 0 ? kCoordsXYStep : 0));
        hint.centreY = static_cast<float>(tile.y + (edge == 1 ? kCoordsXYStep : 0));
        hint.wallAxis = (edge & 1) != 0 ? 0 : 1;
        hint.wallTileShift = static_cast<uint8_t>(edge == 0 ? 1 : (edge == 1 ? 2 : 0));
    }

    // Each builder finishes a hint whose rotation, zMin and zMax are already set.

    void FinishWall(SpriteWorldDepthHint& hint, const PaintStruct& ps, uint8_t rotation)
    {
        hint.mode = SpriteWorldDepthMode::wall;
        hint.wallIsTerrainSide = ps.IsVerticalTileSide && ps.InteractionItem == ViewportInteractionItem::terrain;
        if (ps.IsVerticalTileSide)
        {
            // A terrain or water side: surface paint names the view-relative face it drew, which
            // gives the world tile edge exactly.
            int32_t relativeDirection = 0;
            switch (ps.VerticalTileSideEdge)
            {
                case EDGE_BOTTOMLEFT:
                    relativeDirection = 0;
                    break; // +X at rotation 0
                case EDGE_BOTTOMRIGHT:
                    relativeDirection = 1;
                    break; // +Y
                case EDGE_TOPRIGHT:
                    relativeDirection = 2;
                    break; // -X
                case EDGE_TOPLEFT:
                    relativeDirection = 3;
                    break; // -Y
            }
            PlaceOnTileEdge(hint, ps.MapPos, (relativeDirection + rotation) & 3);
            // With the sun 90 degrees clockwise of the camera the two right-hand faces point at
            // it. Keeping the face, not just its axis, keeps the far parallel face unshadowed.
            hint.wallFacesSun = ps.VerticalTileSideEdge == EDGE_BOTTOMRIGHT || ps.VerticalTileSideEdge == EDGE_TOPRIGHT;
            return;
        }

        // A placed wall: its bound box is thin on one axis and a tile long on the other, which
        // is the edge it runs along. Store the near corner and the offset from the owning tile.
        const auto& bounds = ps.Bounds;
        hint.wallAxis = std::abs(bounds.x_end - bounds.x) >= std::abs(bounds.y_end - bounds.y) ? 0 : 1;
        hint.centreX = static_cast<float>(std::min(bounds.x, bounds.x_end));
        hint.centreY = static_cast<float>(std::min(bounds.y, bounds.y_end));
        hint.wallTileShift = static_cast<uint8_t>(
            ((bounds.x - ps.MapPos.x >= kCoordsXYStep) ? 1 : 0) | ((bounds.y - ps.MapPos.y >= kCoordsXYStep) ? 2 : 0));
        hint.wallFacesSun = hint.wallAxis == (rotation & 1);
    }

    /** A banner is a board strung between two poles along one tile edge. Paint gives each pole a
        1x1 bound and draws the board with the front pole, so as a card the board stood at that
        pole and its shadow lost the far end. It is a wall in all but name. */
    void FinishBanner(SpriteWorldDepthHint& hint, const PaintStruct& ps, uint8_t rotation)
    {
        hint.mode = SpriteWorldDepthMode::wall;
        hint.zMin = static_cast<float>(std::min(ps.Bounds.z, ps.Bounds.z_end) - 2);
        hint.zMax = hint.zMin + 64.0f;
        // The element's position is a world edge, 0 = -X, 1 = +Y, 2 = +X, 3 = -Y.
        PlaceOnTileEdge(hint, ps.MapPos, (2 - ps.Element->asBanner()->getPosition()) & 3);
        hint.wallFacesSun = hint.wallAxis == (rotation & 1);
    }

    /** A plane the shader solves per pixel with TileElementHeight: terrain, water, paths, decals. */
    void FinishSurface(SpriteWorldDepthHint& hint, const PaintStruct& ps)
    {
        hint.centreX = static_cast<float>(ps.MapPos.x);
        hint.centreY = static_cast<float>(ps.MapPos.y);
        hint.baseZ = hint.zMax;
        hint.slope = 0.0f;
        // Water paints with the surface tile as its element, but lies flat at the water height its
        // bound carries. Given the bed's height and slope, opaque water stood above its own record
        // and shaded it wherever the lake was deeper than the terrain's casting margin.
        if (ps.InteractionItem != ViewportInteractionItem::water && ps.Element != nullptr
            && ps.Element->getType() == TileElementType::surface)
        {
            const auto* surface = ps.Element->asSurface();
            hint.baseZ = static_cast<float>(surface->getBaseZ());
            hint.slope = static_cast<float>(surface->getSlope());
        }
        hint.zMin = hint.baseZ;
        hint.zMax = hint.baseZ;
    }

    /** A roof's top is a surface like the ground's and receives like it, solved per pixel against
        the fitted solid. Its gable ends stay casters. */
    void FinishRoof(SpriteWorldDepthHint& hint, const PaintStruct& ps, int32_t shapeCode)
    {
        hint.mode = SpriteWorldDepthMode::roof;
        hint.centreX = static_cast<float>(ps.MapPos.x);
        hint.centreY = static_cast<float>(ps.MapPos.y);
        hint.baseZ = static_cast<float>(ps.Element->getBaseZ());
        hint.slope = static_cast<float>(shapeCode);
        hint.zMin = hint.baseZ;
        hint.zMax = hint.baseZ + static_cast<float>(shapeCode >> 8);
    }

    /** Solved against the bound box: each pixel takes the point where its view ray enters it. */
    void FinishBox(SpriteWorldDepthHint& hint, const PaintStruct& ps)
    {
        const auto& bounds = ps.Bounds;
        int32_t x0 = std::min(bounds.x, bounds.x_end);
        int32_t y0 = std::min(bounds.y, bounds.y_end);
        int32_t sizeX = std::clamp(std::abs(bounds.x_end - bounds.x), 1, 63);
        int32_t sizeY = std::clamp(std::abs(bounds.y_end - bounds.y), 1, 63);
        if (ps.Element != nullptr && ps.Element->getType() == TileElementType::entrance)
        {
            // An entrance's bounds are sorting devices: a thin slab for the back wall, and a box
            // for the front that starts 30 units up so it sorts over the queue. The building is
            // the tile, inset as the paint insets it, from its base to the top of its art.
            x0 = ps.MapPos.x + 2;
            y0 = ps.MapPos.y + 2;
            sizeX = 28;
            sizeY = 28;
            hint.zMin = static_cast<float>(ps.Element->getBaseZ());
            hint.zMax = std::max(hint.zMax, hint.zMin + 30.0f);
        }
        const int32_t sizeZ = std::clamp(static_cast<int32_t>(hint.zMax - hint.zMin), 1, 4095);
        hint.mode = SpriteWorldDepthMode::box;
        hint.centreX = static_cast<float>(x0);
        hint.centreY = static_cast<float>(y0);
        hint.baseZ = hint.zMin;
        hint.slope = static_cast<float>(sizeX | (sizeY << 6) | (sizeZ << 12));
    }

    /** An upright card: screen row maps to world Z, standing at the object's root. */
    void FinishCard(
        SpriteWorldDepthHint& hint, const PaintStruct& ps, const G1Element* g1, const EntityBase* peep, uint8_t rotation)
    {
        CoordsXY centre{ (ps.Bounds.x + ps.Bounds.x_end) / 2, (ps.Bounds.y + ps.Bounds.y_end) / 2 };
        std::optional<ShadowRoot> root;
        if (g1 != nullptr && peep != nullptr)
        {
            // A peep's paint bound starts 5 units above its feet so it sorts over railways, and is
            // 11 tall against a 30px sprite. Stand the card on the peep and reach the sprite's top.
            hint.zMin = static_cast<float>(peep->z);
            hint.zMax = std::max(hint.zMax, hint.zMin - static_cast<float>(g1->yOffset));
            root = PeepShadowRoot(*peep, rotation);
        }
        else if (g1 != nullptr && ps.InteractionItem == ViewportInteractionItem::scenery)
        {
            // Rides keep the bound centre: their lowest pixel is a front corner, not where they stand.
            root = PropShadowRoot(ps, *g1, static_cast<int32_t>(hint.zMin), rotation);
        }
        if (root.has_value())
        {
            centre = root->world;
            hint.drawOffsetX = static_cast<int16_t>(root->drawOffset.x);
            hint.drawOffsetY = static_cast<int16_t>(root->drawOffset.y);
        }
        hint.centreX = static_cast<float>(centre.x);
        hint.centreY = static_cast<float>(centre.y);
        hint.screenTop = static_cast<float>(
            Translate3DTo2DWithZ(rotation, CoordsXYZ{ centre, static_cast<int32_t>(hint.zMax) }).y);
        hint.screenBottom = static_cast<float>(
            Translate3DTo2DWithZ(rotation, CoordsXYZ{ centre, static_cast<int32_t>(hint.zMin) }).y);
        hint.valid = hint.screenBottom > hint.screenTop || hint.zMax > hint.zMin;
    }

    /** Everything that stands up: in order of precedence, banners, roofs, decals lying on the
        ground, bulky props, and the card that covers the rest. */
    void FinishUpright(SpriteWorldDepthHint& hint, const PaintStruct& ps, uint8_t rotation)
    {
        if (ps.InteractionItem == ViewportInteractionItem::banner && ps.Element != nullptr
            && ps.Element->getType() == TileElementType::banner)
        {
            FinishBanner(hint, ps, rotation);
            return;
        }

        // Station platforms, some inversion pieces and point entities are painted with flat bounds.
        // A one-unit span makes them valid casters without touching their sorting; where a shadow
        // lands comes from the sun's silhouette, so the card's height does not matter.
        if (hint.zMin == hint.zMax && (ps.InteractionItem != ViewportInteractionItem::entity || IsPointEntityShadowCaster(ps)))
            hint.zMax += 1.0f;

        const auto* g1 = GfxGetG1Element(ps.image_id);
        const EntityBase* peep = PeepOf(ps);
        if (g1 != nullptr && ps.InteractionItem == ViewportInteractionItem::scenery)
        {
            if (const auto shapeCode = RoofShapeFor(ps, *g1, rotation); shapeCode.has_value())
            {
                FinishRoof(hint, ps, *shapeCode);
                return;
            }
        }

        const bool isScenery = ps.InteractionItem == ViewportInteractionItem::scenery
            || ps.InteractionItem == ViewportInteractionItem::largeScenery;
        if (peep == nullptr && !ps.IsVerticalSupport && isScenery && hint.zMax - hint.zMin <= 4.0f)
        {
            // Under half a height step tall: a decal lying on the ground - paving, markings, a
            // flower bed. As a card it would stand a sprite tall and shade the ground it lies on.
            hint.mode = SpriteWorldDepthMode::flat;
            FinishSurface(hint, ps);
            return;
        }

        if (peep == nullptr && !ps.IsVerticalSupport && IsBulky(ps))
        {
            FinishBox(hint, ps);
            return;
        }

        FinishCard(hint, ps, g1, peep, rotation);
    }
} // namespace

SpriteWorldDepthHint MakeSpriteWorldDepthHint(const PaintStruct& ps, uint8_t rotation)
{
    SpriteWorldDepthHint hint{};
    hint.rotation = rotation;
    hint.mode = ps.IsVerticalSupport ? SpriteWorldDepthMode::billboard : DepthModeForInteraction(ps.InteractionItem);
    if (hint.mode == SpriteWorldDepthMode::none)
        return hint;
    hint.valid = true;
    if (hint.mode == SpriteWorldDepthMode::worldNone)
        return hint;

    // Terrain tops use length.z = -1, so z_end < z; the plane's height is the larger value.
    hint.zMin = static_cast<float>(std::min(ps.Bounds.z, ps.Bounds.z_end));
    hint.zMax = static_cast<float>(std::max(ps.Bounds.z, ps.Bounds.z_end));

    switch (hint.mode)
    {
        case SpriteWorldDepthMode::wall:
            FinishWall(hint, ps, rotation);
            break;
        case SpriteWorldDepthMode::flat:
        case SpriteWorldDepthMode::terrain:
            if (ps.IsVerticalTileSide)
            {
                // A terrain or water side is a wall on its tile edge; as a plane it collapsed to
                // a line, and the bound box cannot tell it from a raised path deck.
                FinishWall(hint, ps, rotation);
                break;
            }
            if (hint.mode == SpriteWorldDepthMode::terrain && ps.IsTerrainCap)
                hint.mode = SpriteWorldDepthMode::terrainCap;
            FinishSurface(hint, ps);
            break;
        default:
            FinishUpright(hint, ps, rotation);
            break;
    }
    return hint;
}
