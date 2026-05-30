/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../drawing/FilterPaletteIds.h"
#include "../drawing/ImageId.hpp"
#include "../drawing/RenderTarget.h"
#include "../localisation/StringIdType.h"
#include "../world/Location.hpp"
#include "../world/MapLimits.h"
#include "Boundbox.h"
#include "tile_element/Paint.Tunnel.h"

#include <mutex>
#include <optional>
#include <sfl/segmented_vector.hpp>
#include <sfl/static_vector.hpp>
#include <thread>

enum class ViewportInteractionItem : uint8_t;

namespace OpenRCT2
{
    struct EntityBase;

    struct TileElement;
    struct SurfaceElement;

    enum class RailingEntrySupportType : uint8_t;
} // namespace OpenRCT2

struct AttachedPaintStruct
{
    AttachedPaintStruct* nextEntry;
    ImageId imageId;
    ImageId colourImageId;
    // This is relative to the parent where we are attached to.
    ScreenCoordsXY relativePos;
    bool isMasked;
};

struct PaintStructBoundBox
{
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t xEnd;
    int32_t yEnd;
    int32_t zEnd;
};

struct PaintStruct
{
    PaintStructBoundBox bounds;
    AttachedPaintStruct* attached;
    PaintStruct* children;
    PaintStruct* nextQuadrantEntry;
    OpenRCT2::TileElement* element;
    OpenRCT2::EntityBase* entity;
    ImageId imageId;
    ScreenCoordsXY screenPos;
    CoordsXY mapPos;
    uint16_t quadrantIndex;
    uint8_t sortFlags;
    ViewportInteractionItem interactionItem;
};

struct PaintStringStruct
{
    StringId stringId;
    PaintStringStruct* nextEntry;
    ScreenCoordsXY screenPos;
    uint32_t args[4];
    uint8_t* yOffsets;
};

struct PaintEntry
{
private:
    // Avoid including expensive <algorithm> for std::max. Manually ensure we use the largest type.
    static_assert(sizeof(PaintStruct) >= sizeof(AttachedPaintStruct));
    static_assert(sizeof(PaintStruct) >= sizeof(PaintStringStruct));
    std::array<uint8_t, sizeof(PaintStruct)> _data;

public:
    PaintStruct* asBasic()
    {
        auto* res = reinterpret_cast<PaintStruct*>(_data.data());
        ::new (res) PaintStruct();
        return res;
    }
    AttachedPaintStruct* asAttached()
    {
        auto* res = reinterpret_cast<AttachedPaintStruct*>(_data.data());
        ::new (res) AttachedPaintStruct();
        return res;
    }
    PaintStringStruct* asString()
    {
        auto* res = reinterpret_cast<PaintStringStruct*>(_data.data());
        ::new (res) PaintStringStruct();
        return res;
    }
};
static_assert(sizeof(PaintEntry) >= sizeof(PaintStruct));
static_assert(sizeof(PaintEntry) >= sizeof(AttachedPaintStruct));
static_assert(sizeof(PaintEntry) >= sizeof(PaintStringStruct));

struct SpriteBb
{
    uint32_t spriteId;
    CoordsXYZ offset;
    CoordsXYZ bbOffset;
    CoordsXYZ bbSize;
};

struct SupportHeight
{
    uint16_t height;
    uint8_t slope;
    uint8_t pad;
};

// The maximum size must be kMaximumMapSizeTechnical multiplied by 2 because
// the quadrant index is based on the x and y components combined.
static constexpr int32_t MaxPaintQuadrants = kMaximumMapSizeTechnical * 2;

struct PaintSessionCore
{
    PaintStruct* paintHead;
    PaintStruct* quadrants[MaxPaintQuadrants];
    PaintStruct* lastPS;
    PaintStringStruct* psStringHead;
    PaintStringStruct* lastPSString;
    AttachedPaintStruct* lastAttachedPS;
    const OpenRCT2::SurfaceElement* surface;
    OpenRCT2::EntityBase* currentlyDrawnEntity;
    OpenRCT2::TileElement* currentlyDrawnTileElement;
    const OpenRCT2::TileElement* pathElementOnSameHeight;
    const OpenRCT2::TileElement* trackElementOnSameHeight;
    const OpenRCT2::TileElement* selectedElement;
    PaintStruct* woodenSupportsPrependTo;
    CoordsXY spritePosition;
    CoordsXY mapPosition;
    uint32_t viewFlags;
    uint32_t quadrantBackIndex;
    uint32_t quadrantFrontIndex;
    ImageId trackColours;
    ImageId supportColours;
    SupportHeight supportSegments[9];
    SupportHeight support;
    uint16_t waterHeight;
    sfl::static_vector<TunnelEntry, kTunnelMaxCount> leftTunnels;
    sfl::static_vector<TunnelEntry, kTunnelMaxCount> rightTunnels;
    uint8_t verticalTunnelHeight;
    uint8_t currentRotation;
    uint8_t flags;
    ViewportInteractionItem interactionType;
};

struct PaintNodeStorage
{
    // 1024 is typically enough to cover the column, after its full it will use dynamicPaintEntries.
    sfl::static_vector<PaintEntry, 1024> fixedPaintEntries;

    // This has to be wrapped in optional as it allocates memory before it is used.
    std::optional<sfl::segmented_vector<PaintEntry, 256>> dynamicPaintEntries;

    PaintEntry* allocate()
    {
        if (!fixedPaintEntries.full())
        {
            return &fixedPaintEntries.emplace_back();
        }

        if (!dynamicPaintEntries.has_value())
        {
            dynamicPaintEntries.emplace();
        }

        return &dynamicPaintEntries->emplace_back();
    }

    void clear()
    {
        fixedPaintEntries.clear();
        dynamicPaintEntries.reset();
    }
};

struct PaintSession : public PaintSessionCore
{
    OpenRCT2::Drawing::RenderTarget rt;
    PaintNodeStorage paintEntries;

    PaintStruct* allocateNormalPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();
        lastPS = entry->asBasic();
        return lastPS;
    }

    AttachedPaintStruct* allocateAttachedPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();
        lastAttachedPS = entry->asAttached();
        return lastAttachedPS;
    }

    PaintStringStruct* allocateStringPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();

        auto* string = entry->asString();
        if (lastPSString == nullptr)
        {
            psStringHead = string;
        }
        else
        {
            lastPSString->nextEntry = string;
        }

        lastPSString = string;
        return lastPSString;
    }
};

extern PaintSession gPaintSession;

// Globals for paint clipping
extern uint8_t gClipHeight;
extern CoordsXY gClipSelectionA;
extern CoordsXY gClipSelectionB;

/** rct2: 0x00993CC4. The white ghost that indicates not-yet-built elements. */
constexpr ImageId ConstructionMarker = ImageId(0).WithRemap(OpenRCT2::Drawing::FilterPaletteID::paletteGhost);
constexpr ImageId HighlightMarker = ImageId(0).WithRemap(OpenRCT2::Drawing::FilterPaletteID::paletteGhost);
constexpr ImageId TrackStationColour = ImageId(0, OpenRCT2::Drawing::Colour::black);
constexpr ImageId ShopSupportColour = ImageId(0, OpenRCT2::Drawing::Colour::darkBrown);

extern bool gShowDirtyVisuals;
extern bool gPaintBoundingBoxes;
extern bool gPaintBlockedTiles;
extern bool gPaintWidePathsAsGhost;
extern bool gPaintStableSort;

PaintStruct* paintAddImageAsParent(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
/**
 *  rct2: 0x006861AC, 0x00686337, 0x006864D0, 0x0068666B, 0x0098196C
 *
 * @param imageId (ebx)
 * @param xOffset (al)
 * @param yOffset (cl)
 * @param boundBoxLengthX (di)
 * @param boundBoxLengthY (si)
 * @param boundBoxLengthZ (ah)
 * @param zOffset (dx)
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 */
inline PaintStruct* paintAddImageAsParent(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return paintAddImageAsParent(session, imageId, offset, { offset, boundBoxSize });
}

[[nodiscard]] PaintStruct* paintAddImageAsOrphan(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
PaintStruct* paintAddImageAsChild(PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

PaintStruct* paintAddImageAsChildRotated(
    PaintSession& session, uint8_t direction, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

PaintStruct* paintAddImageAsParentRotated(
    PaintSession& session, uint8_t direction, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

inline PaintStruct* paintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize)
{
    return paintAddImageAsParentRotated(session, direction, imageId, offset, { offset, boundBoxSize });
}

PaintStruct* paintAddImageAsParentHeight(
    PaintSession& session, ImageId imageId, int32_t height, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

bool paintAttachToPreviousAttach(PaintSession& session, ImageId imageId, int32_t x, int32_t y);
bool paintAttachToPreviousPS(PaintSession& session, ImageId imageId, int32_t x, int32_t y);
void paintFloatingMoneyEffect(
    PaintSession& session, money64 amount, StringId stringId, int32_t y, int32_t z, int8_t yOffsets[], int32_t offsetX,
    uint32_t rotation);

PaintSession* paintSessionAlloc(OpenRCT2::Drawing::RenderTarget& rt, uint32_t viewFlags, uint8_t rotation);
void paintSessionFree(PaintSession* session);
void paintSessionGenerate(PaintSession& session);
void paintSessionArrange(PaintSessionCore& session);
void paintDrawStructs(PaintSession& session);
void paintDrawMoneyStructs(OpenRCT2::Drawing::RenderTarget& rt, PaintStringStruct* ps);
