/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "ShadowRoofFit.h"

#include "../drawing/Drawing.Sprite.h"
#include "../drawing/Drawing.h"
#include "../object/Object.h"
#include "../object/SmallSceneryEntry.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/TileElement.h"
#include "Paint.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <unordered_map>
#include <vector>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;

namespace
{
    /** The fitted shape code of each roof object in each placement, or -1 for one that fits no
        candidate. Keyed by entry and its image base, so a reloaded object is fitted afresh. */
    struct RoofFitKey
    {
        const void* entry;
        uint32_t image;
        uint8_t direction;
        bool operator==(const RoofFitKey&) const = default;
    };
    struct RoofFitKeyHash
    {
        size_t operator()(const RoofFitKey& key) const noexcept
        {
            return std::hash<const void*>{}(key.entry) ^ (static_cast<size_t>(key.image) << 2) ^ key.direction;
        }
    };
    std::unordered_map<RoofFitKey, int32_t, RoofFitKeyHash> _roofFits;
    constexpr size_t kMaxRoofFits = 4096;
} // namespace

// Roof shapes, matching roofHeight() in drawrect.frag. A shape code packs shape | rise direction
// << 4 | height << 8; direction 0 rises toward +X, 1 toward -Y, 2 toward -X, 3 toward +Y.
enum RoofShape : int32_t
{
    kRoofFlat = 0,
    kRoofWedge = 1,
    kRoofRidge = 2,
    kRoofBarrel = 3,
    kRoofHip = 4,
    /** A wedge's sloped top alone, kRoofPlateThickness thick: a loose plank panel. */
    kRoofPlate = 5,
};
constexpr float kRoofPlateThickness = 2.0f;

static float RoofProfile(float lx, float ly, int32_t shape, int32_t direction)
{
    const float t = direction == 0 ? lx : direction == 1 ? 1.0f - ly : direction == 2 ? 1.0f - lx : ly;
    switch (shape)
    {
        case kRoofWedge:
        case kRoofPlate:
            return t;
        case kRoofRidge:
            return 1.0f - std::abs(2.0f * t - 1.0f);
        case kRoofBarrel:
            return std::sqrt(std::max(0.0f, 1.0f - (2.0f * t - 1.0f) * (2.0f * t - 1.0f)));
        case kRoofHip:
            return std::clamp(4.0f * std::min(std::min(lx, 1.0f - lx), std::min(ly, 1.0f - ly)), 0.0f, 1.0f);
        default:
            return 1.0f;
    }
}

/** Opaque texels of a sprite as a width * height grid. */
static std::vector<uint8_t> DecodeSpriteMask(const G1Element& g1)
{
    std::vector<uint8_t> mask(static_cast<size_t>(g1.width) * static_cast<size_t>(g1.height), 0);
    for (int32_t y = 0; y < g1.height; y++)
    {
        uint8_t* out = mask.data() + static_cast<size_t>(y) * g1.width;
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
                for (int32_t i = 0; i < count && start + i < g1.width; i++)
                    out[start + i] = 1;
                data += count;
            }
        }
        else
        {
            const uint8_t* row = g1.offset + static_cast<size_t>(y) * static_cast<size_t>(g1.width);
            for (int32_t x = 0; x < g1.width; x++)
                out[x] = (!g1.flags.has(G1Flag::hasTransparency) || row[x] != 0) ? 1 : 0;
        }
    }
    return mask;
}

/**
 * Which roof solid a full-tile prop's sprite shows, as a shape code, or -1 if none fits well.
 *
 * Every candidate - flat, a wedge rising toward each of the four sides, a ridge or a barrel along
 * either axis, a hipped cap, each at a range of heights - is projected into this view standing on
 * the element's tile and compared with the sprite's opaque texels. A solid standing on a flat base
 * covers one unbroken run of each screen column, so the comparison is per column. The best overlap
 * (intersection over union) wins, if it is good enough; the art has eaves and trim no candidate
 * reproduces, so a perfect score is not expected.
 */
static int32_t FitRoofShape(const G1Element& g1, ScreenCoordsXY anchor, CoordsXY tile, int32_t baseZ, uint8_t rotation)
{
    if (g1.offset == nullptr || g1.width <= 0 || g1.height <= 0)
        return -1;

    const auto mask = DecodeSpriteMask(g1);
    // Columns cover the sprite plus a margin, so a candidate spilling past the art is penalised.
    constexpr int32_t kMargin = 16;
    const int32_t left = anchor.x + g1.xOffset - kMargin;
    const int32_t top = anchor.y + g1.yOffset;
    const int32_t columns = g1.width + 2 * kMargin;
    // prefix[c * (h + 1) + r]: covered texels in column c above sprite row r. A column counts as
    // covered from its first opaque texel to its last, the way the candidates are: a glass roof's
    // sprite is only its frame, the panes being drawn separately.
    const int32_t h = g1.height;
    std::vector<int32_t> prefix(static_cast<size_t>(columns) * (h + 1), 0);
    int32_t spriteCount = 0;
    for (int32_t c = 0; c < columns; c++)
    {
        const int32_t x = c - kMargin;
        int32_t first = h;
        int32_t last = -1;
        for (int32_t r = 0; x >= 0 && x < g1.width && r < h; r++)
        {
            if (mask[static_cast<size_t>(r) * g1.width + x] != 0)
            {
                first = std::min(first, r);
                last = r;
            }
        }
        int32_t* column = prefix.data() + static_cast<size_t>(c) * (h + 1);
        for (int32_t r = 0; r < h; r++)
            column[r + 1] = column[r] + ((r >= first && r <= last) ? 1 : 0);
        spriteCount += column[h];
    }
    if (spriteCount == 0)
        return -1;

    static constexpr int32_t kHeights[] = { 4, 8, 12, 16, 20, 24, 28, 32, 40, 48 };
    // shape, direction: flat, four wedges, two ridges, two barrels, a hip, four plates.
    static constexpr std::array<std::array<int32_t, 2>, 14> kShapes = { {
        { kRoofFlat, 0 },
        { kRoofWedge, 0 },
        { kRoofWedge, 1 },
        { kRoofWedge, 2 },
        { kRoofWedge, 3 },
        { kRoofRidge, 0 },
        { kRoofRidge, 1 },
        { kRoofBarrel, 0 },
        { kRoofBarrel, 1 },
        { kRoofHip, 0 },
        { kRoofPlate, 0 },
        { kRoofPlate, 1 },
        { kRoofPlate, 2 },
        { kRoofPlate, 3 },
    } };

    constexpr int32_t kSamples = 64;
    std::vector<int32_t> spanTop(columns);
    std::vector<int32_t> spanBottom(columns);
    float bestScore = 0.0f;
    int32_t bestCode = -1;
    for (const auto& [shape, direction] : kShapes)
    {
        for (const int32_t height : kHeights)
        {
            std::fill(spanTop.begin(), spanTop.end(), std::numeric_limits<int32_t>::max());
            std::fill(spanBottom.begin(), spanBottom.end(), std::numeric_limits<int32_t>::min());
            int32_t spill = 0;
            for (int32_t i = 0; i < kSamples; i++)
            {
                for (int32_t j = 0; j < kSamples; j++)
                {
                    const float lx = (i + 0.5f) / kSamples;
                    const float ly = (j + 0.5f) / kSamples;
                    const float wx = tile.x + lx * kCoordsXYStep;
                    const float wy = tile.y + ly * kCoordsXYStep;
                    float rx = wx, ry = wy;
                    switch (rotation & 3)
                    {
                        case 1:
                            rx = wy;
                            ry = -wx;
                            break;
                        case 2:
                            rx = -wx;
                            ry = -wy;
                            break;
                        case 3:
                            rx = -wy;
                            ry = wx;
                            break;
                        default:
                            break;
                    }
                    const int32_t c = static_cast<int32_t>(std::floor(ry - rx)) - left;
                    if (c < 0 || c >= columns)
                    {
                        spill++;
                        continue;
                    }
                    const float ground = (rx + ry) * 0.5f;
                    const float roofZ = baseZ + height * RoofProfile(lx, ly, shape, direction);
                    spanTop[c] = std::min(spanTop[c], static_cast<int32_t>(std::floor(ground - roofZ)) - top);
                    const float floorZ = shape == kRoofPlate ? roofZ - kRoofPlateThickness : static_cast<float>(baseZ);
                    spanBottom[c] = std::max(spanBottom[c], static_cast<int32_t>(std::floor(ground - floorZ)) - top);
                }
            }
            int32_t candidateCount = 0;
            int32_t overlap = 0;
            for (int32_t c = 0; c < columns; c++)
            {
                if (spanTop[c] > spanBottom[c])
                    continue;
                candidateCount += spanBottom[c] - spanTop[c] + 1;
                const int32_t r0 = std::clamp(spanTop[c], 0, h);
                const int32_t r1 = std::clamp(spanBottom[c] + 1, 0, h);
                const int32_t* column = prefix.data() + static_cast<size_t>(c) * (h + 1);
                overlap += column[r1] - column[r0];
            }
            const float score = static_cast<float>(overlap)
                / static_cast<float>(spriteCount + candidateCount - overlap + spill);
            // Ties go to the earlier, simpler candidate.
            if (score > bestScore + 0.002f)
            {
                bestScore = score;
                bestCode = shape | (direction << 4) | (height << 8);
            }
        }
    }
    constexpr float kMinimumOverlap = 0.7f;
    return bestScore >= kMinimumOverlap ? bestCode : -1;
}

std::optional<int32_t> RoofShapeFor(const PaintStruct& ps, const G1Element& g1, uint8_t rotation)
{
    if (ps.Element == nullptr || ps.Element->getType() != TileElementType::smallScenery)
        return std::nullopt;
    const auto* scenery = ps.Element->asSmallScenery();
    const auto* entry = scenery->getEntry();
    if (entry == nullptr || !entry->flags.has(SmallSceneryFlag::occupiesFullTile))
        return std::nullopt;

    const RoofFitKey key{ entry, entry->image, scenery->getDirection() };
    auto it = _roofFits.find(key);
    if (it == _roofFits.end())
    {
        // Only objects that call themselves roofs are tried: a solid block would fit "flat" just
        // as well, and is better left a card than have its sides treated as a roof's.
        const auto* object = ObjectEntryGetObject(ObjectType::smallScenery, scenery->getEntryIndex());
        std::string identifier(object != nullptr ? object->GetIdentifier() : std::string_view{});
        std::transform(identifier.begin(), identifier.end(), identifier.begin(), [](unsigned char ch) {
            return static_cast<char>(std::tolower(ch));
        });
        const int32_t code = identifier.find("roof") != std::string::npos
            ? FitRoofShape(g1, ps.ScreenPos, ps.MapPos, scenery->getBaseZ(), rotation)
            : -1;
        if (_roofFits.size() >= kMaxRoofFits)
            _roofFits.clear();
        it = _roofFits.emplace(key, code).first;
    }
    return it->second >= 0 ? std::optional<int32_t>(it->second) : std::nullopt;
}
