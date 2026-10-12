/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShadowSpriteRoot.h"

#include "../drawing/Drawing.Sprite.h"
#include "../drawing/Drawing.h"
#include "../entity/EntityBase.h"
#include "../interface/Viewport.h"
#include "../object/SmallSceneryEntry.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TileElement.h"
#include "Paint.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <limits>
#include <unordered_map>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

namespace
{
    /** Keyed by an object's address and a view rotation. An object freed and replaced at the same
        address reads a stale entry until it is painted again - one frame of offset shadow. */
    struct RootKey
    {
        const void* object;
        uint8_t rotation;
        bool operator==(const RootKey&) const = default;
    };
    struct RootKeyHash
    {
        size_t operator()(const RootKey& key) const noexcept
        {
            return std::hash<const void*>{}(key.object) ^ (static_cast<size_t>(key.rotation) << 1);
        }
    };
    constexpr size_t kMaxCachedRoots = 1 << 18;

    /** Each prop's root as the camera last saw it. Written by camera paints, read by the sun pass. */
    std::unordered_map<RootKey, CoordsXY, RootKeyHash> _cameraRoots;
    /** An animated prop's root, from the first frame painted and kept: the figure moves through
        its animation, the place it stands does not. */
    std::unordered_map<RootKey, ScreenCoordsXY, RootKeyHash> _animatedRoots;

    template<typename TMap>
    void CacheInsert(TMap& map, const RootKey& key, const typename TMap::mapped_type& value)
    {
        if (map.size() >= kMaxCachedRoots)
            map.clear();
        map[key] = value;
    }

    /** The sprite offset that moves `have` onto `want`, both at height z, in this view. */
    ScreenCoordsXY ShiftBetween(uint8_t rotation, CoordsXY have, CoordsXY want, int32_t z)
    {
        return Translate3DTo2DWithZ(rotation, CoordsXYZ{ want, z }) - Translate3DTo2DWithZ(rotation, CoordsXYZ{ have, z });
    }
} // namespace

/** Leftmost and rightmost opaque texel of one sprite row, or nothing if the row is empty. */
static std::optional<std::pair<int32_t, int32_t>> SpriteRowSpan(const G1Element& g1, int32_t y)
{
    int32_t minX = std::numeric_limits<int32_t>::max();
    int32_t maxX = -1;
    if (g1.flags.has(G1Flag::hasRLECompression))
    {
        uint16_t lineOffset;
        std::memcpy(&lineOffset, g1.offset + y * sizeof(uint16_t), sizeof(uint16_t));
        const uint8_t* data = g1.offset + lineOffset;
        for (bool last = false; !last;)
        {
            uint8_t count = *data++;
            const uint8_t start = *data++;
            last = (count & 0x80) != 0;
            count &= 0x7F;
            if (count > 0)
            {
                minX = std::min<int32_t>(minX, start);
                maxX = std::max<int32_t>(maxX, start + count - 1);
            }
            data += count;
        }
    }
    else
    {
        const uint8_t* row = g1.offset + static_cast<size_t>(y) * static_cast<size_t>(g1.width);
        for (int32_t x = 0; x < g1.width; x++)
        {
            if (!g1.flags.has(G1Flag::hasTransparency) || row[x] != 0)
            {
                minX = std::min(minX, x);
                maxX = std::max(maxX, x);
            }
        }
    }
    if (maxX < 0)
        return std::nullopt;
    return std::pair{ minX, maxX };
}

/**
 * Offset from a paint's anchor to its root: the centre of the footprint its artwork stands on.
 * That is where the object meets the ground in this view - a trunk's middle, a column's - which
 * the anchor is not: tree art is rendered per direction with the trunk wherever the model puts it.
 *
 * The lowest opaque pixel is not it. That is the front of the footprint, the point nearest the
 * viewer, and the camera and the sun look from different sides, so "front" is a different world
 * point in each: rooting both on it put every thick trunk's shadow a radius off. A round footprint
 * draws as an ellipse half as tall as it is wide, so its centre sits a quarter of its width above
 * its lowest pixel. Scan up from the bottom until that height, taking the widest span seen.
 *
 * Every image of the paint counts - the parent and anything attached to it. An animated prop draws
 * its figure as an attached frame over a base that can be tiny or empty.
 */
static std::optional<ScreenCoordsXY> FindSpriteRoot(const PaintStruct& ps, const G1Element& parent)
{
    struct Part
    {
        const G1Element* g1;
        ScreenCoordsXY offset; // of the sprite's top-left texel from the anchor
    };
    std::array<Part, 8> parts{};
    size_t partCount = 0;
    const auto addPart = [&](const G1Element* g1, ScreenCoordsXY relative) {
        if (g1 != nullptr && g1->offset != nullptr && g1->width > 0 && g1->height > 0 && partCount < parts.size())
            parts[partCount++] = { g1, relative + ScreenCoordsXY{ g1->xOffset, g1->yOffset } };
    };
    addPart(&parent, {});
    for (const auto* attached = ps.Attached; attached != nullptr; attached = attached->NextEntry)
        addPart(GfxGetG1Element(attached->image_id), attached->RelativePos);
    if (partCount == 0)
        return std::nullopt;

    int32_t top = std::numeric_limits<int32_t>::max();
    int32_t bottom = std::numeric_limits<int32_t>::min();
    for (size_t i = 0; i < partCount; i++)
    {
        top = std::min(top, parts[i].offset.y);
        bottom = std::max(bottom, parts[i].offset.y + parts[i].g1->height - 1);
    }
    const auto spanAt = [&](int32_t y) -> std::optional<std::pair<int32_t, int32_t>> {
        std::optional<std::pair<int32_t, int32_t>> merged;
        for (size_t i = 0; i < partCount; i++)
        {
            const int32_t row = y - parts[i].offset.y;
            if (row < 0 || row >= parts[i].g1->height)
                continue;
            if (const auto span = SpriteRowSpan(*parts[i].g1, row); span.has_value())
            {
                const int32_t lo = span->first + parts[i].offset.x;
                const int32_t hi = span->second + parts[i].offset.x;
                merged = merged.has_value() ? std::pair{ std::min(merged->first, lo), std::max(merged->second, hi) }
                                            : std::pair{ lo, hi };
            }
        }
        return merged;
    };

    int32_t lowest = bottom;
    while (lowest >= top && !spanAt(lowest).has_value())
        lowest--;
    if (lowest < top)
        return std::nullopt;

    // A footprint wider than this is not a trunk standing on one point, and searching higher
    // would reach into a canopy.
    constexpr int32_t kMaxRowsUp = 24;
    int32_t widest = 0;
    int32_t middle = 0;
    for (int32_t y = lowest; y >= top && lowest - y <= kMaxRowsUp; y--)
    {
        if (const auto span = spanAt(y); span.has_value())
        {
            const int32_t width = span->second - span->first + 1;
            if (width > widest)
            {
                widest = width;
                middle = (span->first + span->second) / 2;
            }
        }
        if ((lowest - y) * 4 >= widest)
            break;
    }
    return ScreenCoordsXY{ middle, lowest - widest / 4 };
}

ShadowRoot PeepShadowRoot(const EntityBase& entity, uint8_t rotation)
{
    // Their feet sit kPeepFootDrop px below the anchor (measured: 2-3 px across walk frames),
    // centred. The sun's own "3px below the anchor" is a different world point, so in the shadow
    // pass the image is shifted to put its foot on the camera's.
    constexpr int32_t kPeepFootDrop = 3;
    const CoordsXYZ position{ entity.x, entity.y, entity.z };
    const auto footFor = [&](uint8_t viewRotation) {
        return ViewportPosToMapPos(
            Translate3DTo2DWithZ(viewRotation, position) + ScreenCoordsXY{ 0, kPeepFootDrop }, position.z, viewRotation);
    };
    ShadowRoot root{ footFor(rotation), {} };
    if (gPaintShadowCameraRotation >= 0)
    {
        const CoordsXY cameraFoot = footFor(static_cast<uint8_t>(gPaintShadowCameraRotation));
        root.drawOffset = ShiftBetween(rotation, root.world, cameraFoot, position.z);
        root.world = cameraFoot;
    }
    return root;
}

std::optional<ShadowRoot> PropShadowRoot(const PaintStruct& ps, const G1Element& g1, int32_t baseZ, uint8_t rotation)
{
    // An animated prop's frames each have their own lowest row; rooting on every frame made the
    // shadow dance with the figure.
    const auto* scenery = ps.Element != nullptr && ps.Element->getType() == TileElementType::smallScenery
        ? ps.Element->asSmallScenery()
        : nullptr;
    const bool animated = scenery != nullptr && scenery->getEntry() != nullptr
        && scenery->getEntry()->flags.has(SmallSceneryFlag::isAnimated);

    std::optional<ScreenCoordsXY> screenRoot;
    if (animated)
    {
        const RootKey key{ ps.Element, rotation };
        if (const auto it = _animatedRoots.find(key); it != _animatedRoots.end())
            screenRoot = it->second;
        else if (screenRoot = FindSpriteRoot(ps, g1); screenRoot.has_value())
            CacheInsert(_animatedRoots, key, *screenRoot);
    }
    else
    {
        screenRoot = FindSpriteRoot(ps, g1);
    }
    if (!screenRoot.has_value())
        return std::nullopt;

    ShadowRoot root{ ViewportPosToMapPos(ps.ScreenPos + *screenRoot, baseZ, rotation), {} };
    if (gPaintShadowCameraRotation < 0)
    {
        CacheInsert(_cameraRoots, { ps.Element, rotation }, root.world);
    }
    else if (const auto it = _cameraRoots.find({ ps.Element, static_cast<uint8_t>(gPaintShadowCameraRotation) });
             it != _cameraRoots.end())
    {
        root.drawOffset = ShiftBetween(rotation, root.world, it->second, baseZ);
        root.world = it->second;
    }
    return root;
}
