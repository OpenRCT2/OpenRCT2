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
#include "../interface/ViewportFlags.h"
#include "../localisation/StringIdType.h"
#include "../world/Location.hpp"
#include "../world/MapLimits.h"
#include "Boundbox.h"
#include "tile_element/Paint.Tunnel.h"

#include <optional>
#include <sfl/segmented_vector.hpp>
#include <sfl/static_vector.hpp>

enum class ViewportInteractionItem : uint8_t;

namespace OpenRCT2
{
    struct EntityBase;

    struct TileElement;
    struct SurfaceElement;

} // namespace OpenRCT2

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
    OpenRCT2::TileElement* Element;
    OpenRCT2::EntityBase* Entity;
    ImageId image_id;
    ScreenCoordsXY ScreenPos;
    CoordsXY MapPos;
    uint16_t QuadrantIndex;
    uint8_t SortFlags;
    ViewportInteractionItem InteractionItem;
    /** One of the vertical faces of a terrain or water tile rather than the flat top. The
        shadow G-buffer solves these as a segment running along a tile edge, which only works
        because the segment really is 32 units of tile boundary - a raised path deck has the
        same bound-box shape and must not be solved that way. Only the paint code knows the
        difference, so it records it here instead of the renderer guessing. */
    bool IsVerticalTileSide;
    /** View-relative edge_t value for a terrain/water side. This preserves which of the two
        parallel faces was painted; an axis alone cannot tell whether the face points at or
        away from the sun. */
    uint8_t VerticalTileSideEdge;
    bool IsTerrainCap;
    /** Support paint is upright geometry even when it is emitted while painting a footpath.
        Keep this separate from InteractionItem so shadow depth does not change clicking or
        other long-established viewport behaviour. */
    bool IsVerticalSupport;
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
    const OpenRCT2::SurfaceElement* Surface;
    OpenRCT2::EntityBase* CurrentlyDrawnEntity;
    OpenRCT2::TileElement* CurrentlyDrawnTileElement;
    const OpenRCT2::TileElement* PathElementOnSameHeight;
    const OpenRCT2::TileElement* TrackElementOnSameHeight;
    const OpenRCT2::TileElement* SelectedElement;
    PaintStruct* WoodenSupportsPrependTo;
    CoordsXY SpritePosition;
    CoordsXY MapPosition;
    OpenRCT2::ViewportFlags ViewFlags;
    uint32_t QuadrantBackIndex;
    uint32_t QuadrantFrontIndex;
    ImageId TrackColours;
    ImageId SupportColours;
    SupportHeight SupportSegments[9];
    SupportHeight Support;
    uint16_t WaterHeight;
    sfl::static_vector<TunnelEntry, kTunnelMaxCount> LeftTunnels;
    sfl::static_vector<TunnelEntry, kTunnelMaxCount> RightTunnels;
    uint8_t VerticalTunnelHeight;
    uint8_t CurrentRotation;
    uint8_t Flags;
    uint8_t VerticalTileSideEdge;
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
    OpenRCT2::Drawing::RenderTarget rt;
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

/** Extra view-space distance PaintSessionGenerate walks "south" past the region it is filling,
 *  to catch tall objects whose sprites reach up into that region from further away - roughly
 *  the tallest world Z that can exist. Measured from the region, not from any buffer edge, so
 *  it cannot be narrowed for a partial repaint without slicing tall objects out of it. */
constexpr int32_t kPaintVerticalTileMargin = 2128;

/** Set while a drop-shadow sun pass paints, to the rotation of the camera it is shadowing; -1
 *  otherwise. Each view draws an object's image for its own direction, and those images do not
 *  agree to the pixel on where the object meets the ground, so the sun pass shifts each prop to
 *  put its root where the camera drew it. */
extern int32_t gPaintShadowCameraRotation;

PaintStruct* PaintAddImageAsParent(
    PaintSession& session, ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
PaintStruct* PaintAddImageAsSupport(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
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
    PaintSession& session, ImageId image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParent(session, image_id, offset, { offset, boundBoxSize });
}

inline PaintStruct* PaintAddImageAsSupport(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsSupport(session, imageId, offset, { offset, boundBoxSize });
}

[[nodiscard]] PaintStruct* PaintAddImageAsOrphan(
    PaintSession& session, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);
PaintStruct* PaintAddImageAsChild(
    PaintSession& session, ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

PaintStruct* PaintAddImageAsChildRotated(
    PaintSession& session, uint8_t direction, ImageId image_id, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

PaintStruct* PaintAddImageAsParentRotated(
    PaintSession& session, uint8_t direction, ImageId imageId, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

inline PaintStruct* PaintAddImageAsParentRotated(
    PaintSession& session, const uint8_t direction, const ImageId imageId, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize)
{
    return PaintAddImageAsParentRotated(session, direction, imageId, offset, { offset, boundBoxSize });
}

PaintStruct* PaintAddImageAsParentHeight(
    PaintSession& session, ImageId imageId, int32_t height, const CoordsXYZ& offset, const BoundBoxXYZ& boundBox);

bool PaintAttachToPreviousAttach(PaintSession& session, ImageId imageId, int32_t x, int32_t y);
bool PaintAttachToPreviousPS(PaintSession& session, ImageId image_id, int32_t x, int32_t y);
void PaintFloatingMoneyEffect(
    PaintSession& session, money64 amount, StringId string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
    uint32_t rotation);

PaintSession* PaintSessionAlloc(OpenRCT2::Drawing::RenderTarget& rt, OpenRCT2::ViewportFlags viewFlags, uint8_t rotation);
void PaintSessionFree(PaintSession* session);
void PaintSessionGenerate(PaintSession& session);
void PaintSessionArrange(PaintSessionCore& session);
void PaintDrawStructs(PaintSession& session);
void PaintDrawMoneyStructs(OpenRCT2::Drawing::RenderTarget& rt, PaintStringStruct* ps);
