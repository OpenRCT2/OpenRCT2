/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../core/Money.hpp"
#include "../drawing/Drawing.h"
#include "../interface/Colour.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "Boundbox.h"
#include "tile_element/Paint.Tunnel.h"

#include <mutex>
#include <sfl/segmented_vector.hpp>
#include <sfl/static_vector.hpp>
#include <thread>

struct EntityBase;
struct TileElement;
struct SurfaceElement;
enum class RailingEntrySupportType : uint8_t;
enum class ViewportInteractionItem : uint8_t;

struct AttachedPaintStruct
{
    AttachedPaintStruct* NextEntry;
    ImageId image_id;
    ImageId ColourImageId;
    // This is relative to the parent where we are attached to.
    ScreenCoordsXY RelativePos;
    bool IsMasked;
};

struct PaintStructBoundBox
{
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t x_end;
    int32_t y_end;
    int32_t z_end;
};

struct PaintStruct
{
    PaintStructBoundBox Bounds;
    AttachedPaintStruct* Attached;
    PaintStruct* Children;
    PaintStruct* NextQuadrantEntry;
    TileElement* Element;
    EntityBase* Entity;
    ImageId image_id;
    ScreenCoordsXY ScreenPos;
    CoordsXY MapPos;
    uint16_t QuadrantIndex;
    uint8_t SortFlags;
    ViewportInteractionItem InteractionItem;
};

struct PaintStringStruct
{
    StringId string_id;
    PaintStringStruct* NextEntry;
    ScreenCoordsXY ScreenPos;
    uint32_t args[4];
    uint8_t* y_offsets;
};

struct PaintEntry
{
private:
    // Avoid including expensive <algorithm> for std::max. Manually ensure we use the largest type.
    static_assert(sizeof(PaintStruct) >= sizeof(AttachedPaintStruct));
    static_assert(sizeof(PaintStruct) >= sizeof(PaintStringStruct));
    std::array<uint8_t, sizeof(PaintStruct)> data;

public:
    PaintStruct* AsBasic()
    {
        auto* res = reinterpret_cast<PaintStruct*>(data.data());
        ::new (res) PaintStruct();
        return res;
    }
    AttachedPaintStruct* AsAttached()
    {
        auto* res = reinterpret_cast<AttachedPaintStruct*>(data.data());
        ::new (res) AttachedPaintStruct();
        return res;
    }
    PaintStringStruct* AsString()
    {
        auto* res = reinterpret_cast<PaintStringStruct*>(data.data());
        ::new (res) PaintStringStruct();
        return res;
    }
};
static_assert(sizeof(PaintEntry) >= sizeof(PaintStruct));
static_assert(sizeof(PaintEntry) >= sizeof(AttachedPaintStruct));
static_assert(sizeof(PaintEntry) >= sizeof(PaintStringStruct));

struct SpriteBb
{
    uint32_t sprite_id;
    CoordsXYZ offset;
    CoordsXYZ bb_offset;
    CoordsXYZ bb_size;
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
    PaintStruct* PaintHead;
    PaintStruct* Quadrants[MaxPaintQuadrants];
    PaintStruct* LastPS;
    PaintStringStruct* PSStringHead;
    PaintStringStruct* LastPSString;
    AttachedPaintStruct* LastAttachedPS;
    const SurfaceElement* Surface;
    EntityBase* CurrentlyDrawnEntity;
    TileElement* CurrentlyDrawnTileElement;
    const TileElement* PathElementOnSameHeight;
    const TileElement* TrackElementOnSameHeight;
    const TileElement* SelectedElement;
    PaintStruct* WoodenSupportsPrependTo;
    CoordsXY SpritePosition;
    CoordsXY MapPosition;
    uint32_t ViewFlags;
    uint32_t QuadrantBackIndex;
    uint32_t QuadrantFrontIndex;
    ImageId TrackColours;
    ImageId SupportColours;
    SupportHeight SupportSegments[9];
    SupportHeight Support;
    uint16_t WaterHeight;
    TunnelEntry LeftTunnels[kTunnelMaxCount];
    TunnelEntry RightTunnels[kTunnelMaxCount];
    uint8_t LeftTunnelCount;
    uint8_t RightTunnelCount;
    uint8_t VerticalTunnelHeight;
    uint8_t CurrentRotation;
    uint8_t Flags;
    ViewportInteractionItem InteractionType;
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
    DrawPixelInfo DPI;
    PaintNodeStorage paintEntries;

    PaintStruct* AllocateNormalPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();
        LastPS = entry->AsBasic();
        return LastPS;
    }

    AttachedPaintStruct* AllocateAttachedPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();
        LastAttachedPS = entry->AsAttached();
        return LastAttachedPS;
    }

    PaintStringStruct* AllocateStringPaintEntry() noexcept
    {
        auto* entry = paintEntries.allocate();

        auto* string = entry->AsString();
        if (LastPSString == nullptr)
        {
            PSStringHead = string;
        }
        else
        {
            LastPSString->NextEntry = string;
        }

        LastPSString = string;
        return LastPSString;
    }
};

struct FootpathPaintInfo
{
    uint32_t SurfaceImageId{};
    uint32_t BridgeImageId{};
    uint32_t RailingsImageId{};
    uint32_t SurfaceFlags{};
    uint32_t RailingFlags{};
    uint8_t ScrollingMode{};
    RailingEntrySupportType SupportType{};
    colour_t SupportColour = 255;
};

extern PaintSession gPaintSession;

// Globals for paint clipping
extern uint8_t gClipHeight;
extern CoordsXY gClipSelectionA;
extern CoordsXY gClipSelectionB;

/** rct2: 0x00993CC4. The white ghost that indicates not-yet-built elements. */
constexpr ImageId ConstructionMarker = ImageId(0).WithRemap(FilterPaletteID::PaletteGhost);
constexpr ImageId HighlightMarker = ImageId(0).WithRemap(FilterPaletteID::PaletteGhost);
constexpr ImageId TrackStationColour = ImageId(0, COLOUR_BLACK);
constexpr ImageId ShopSupportColour = ImageId(0, COLOUR_DARK_BROWN);

extern bool gShowDirtyVisuals;
extern bool gPaintBoundingBoxes;
extern bool gPaintBlockedTiles;
extern bool gPaintWidePathsAsGhost;
extern bool gPaintStableSort;

PaintStruct* PaintAddImageAsParent(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
/**
 *  rct2: 0x006861AC, 0x00686337, 0x006864D0, 0x0068666B, 0x0098196C
 *
 * @param image_id (ebx)
 * @param x_offset (al)
 * @param y_offset (cl)
 * @param bound_box_length_x (di)
 * @param bound_box_length_y (si)
 * @param bound_box_length_z (ah)
 * @param z_offset (dx)
 * @return (ebp) PaintStruct on success (CF == 0), nullptr on failure (CF == 1)
 */
inline PaintStruct* PaintAddImageAsParent(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParent(session, image_id, offset, { offset, boundBoxSize });
}

[[nodiscard]] PaintStruct* PaintAddImageAsOrphan(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
PaintStruct* PaintAddImageAsChild(
    PaintSession& session, const ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

PaintStruct* PaintAddImageAsChildRotated(
    PaintSession& session, const uint8_t direction, const ImageId image_id, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox);

PaintStruct* PaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const BoundBoxXYZ& boundBox);

inline PaintStruct* PaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParentRotated(session, direction, imageId, offset, { offset, boundBoxSize });
}

bool PaintAttachToPreviousAttach(PaintSession& session, const ImageId imageId, int32_t x, int32_t y);
bool PaintAttachToPreviousPS(PaintSession& session, const ImageId image_id, int32_t x, int32_t y);
void PaintFloatingMoneyEffect(
    PaintSession& session, money64 amount, StringId string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
    uint32_t rotation);

PaintSession* PaintSessionAlloc(DrawPixelInfo& dpi, uint32_t viewFlags, uint8_t rotation);
void PaintSessionFree(PaintSession* session);
void PaintSessionGenerate(PaintSession& session);
void PaintSessionArrange(PaintSessionCore& session);
void PaintDrawStructs(PaintSession& session);
void PaintDrawMoneyStructs(DrawPixelInfo& dpi, PaintStringStruct* ps);
