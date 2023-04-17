/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../common.h"
#include "../core/FixedVector.h"
#include "../drawing/Drawing.h"
#include "../interface/Colour.h"
#include "../world/Location.hpp"
#include "../world/Map.h"
#include "Boundbox.h"

#include <mutex>
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
    std::array<uint8_t, std::max({ sizeof(PaintStruct), sizeof(AttachedPaintStruct), sizeof(PaintStringStruct) })> data;

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

struct TunnelEntry
{
    uint8_t height;
    uint8_t type;
};

// The maximum size must be MAXIMUM_MAP_SIZE_TECHNICAL multiplied by 2 because
// the quadrant index is based on the x and y components combined.
static constexpr int32_t MaxPaintQuadrants = MAXIMUM_MAP_SIZE_TECHNICAL * 2;

#define TUNNEL_MAX_COUNT 65

/**
 * A pool of PaintEntry instances that can be rented out.
 * The internal implementation uses an unrolled linked list so that each
 * paint session can quickly allocate a new paint entry until it requires
 * another node / block of paint entries. Only the node allocation needs to
 * be thread safe.
 */
class PaintEntryPool
{
    static constexpr size_t NodeSize = 512;

public:
    struct Node
    {
        Node* Next{};
        size_t Count{};
        PaintEntry PaintStructs[NodeSize]{};
    };

    struct Chain
    {
        PaintEntryPool* Pool{};
        Node* Head{};
        Node* Current{};

        Chain() = default;
        Chain(PaintEntryPool* pool);
        Chain(Chain&& chain);
        ~Chain();

        Chain& operator=(Chain&& chain) noexcept;

        PaintEntry* Allocate();
        void Clear();
        size_t GetCount() const;
    };

private:
    std::vector<Node*> _available;
    std::mutex _mutex;

    Node* AllocateNode();

public:
    ~PaintEntryPool();

    Chain Create();
    void FreeNodes(Node* head);
};

struct PaintSessionCore
{
    PaintStruct PaintHead;
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
    PaintStruct* WoodenSupportsPrependTo;
    CoordsXY SpritePosition;
    CoordsXY MapPosition;
    uint32_t ViewFlags;
    uint32_t QuadrantBackIndex;
    uint32_t QuadrantFrontIndex;
    ImageId TrackColours[4];
    SupportHeight SupportSegments[9];
    SupportHeight Support;
    uint16_t WaterHeight;
    TunnelEntry LeftTunnels[TUNNEL_MAX_COUNT];
    TunnelEntry RightTunnels[TUNNEL_MAX_COUNT];
    uint8_t LeftTunnelCount;
    uint8_t RightTunnelCount;
    uint8_t VerticalTunnelHeight;
    uint8_t CurrentRotation;
    uint8_t Flags;
    ViewportInteractionItem InteractionType;
};

struct PaintSession : public PaintSessionCore
{
    DrawPixelInfo DPI;
    PaintEntryPool::Chain PaintEntryChain;

    PaintStruct* AllocateNormalPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
            LastPS = entry->AsBasic();
            return LastPS;
        }
        return nullptr;
    }

    AttachedPaintStruct* AllocateAttachedPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
            LastAttachedPS = entry->AsAttached();
            return LastAttachedPS;
        }
        return nullptr;
    }

    PaintStringStruct* AllocateStringPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
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
        return nullptr;
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

struct RecordedPaintSession
{
    PaintSessionCore Session;
    std::vector<PaintEntry> Entries;
};

extern PaintSession gPaintSession;

// Globals for paint clipping
extern uint8_t gClipHeight;
extern CoordsXY gClipSelectionA;
extern CoordsXY gClipSelectionB;

/** rct2: 0x00993CC4. The white ghost that indicates not-yet-built elements. */
constexpr const ImageId ConstructionMarker = ImageId(0).WithRemap(FilterPaletteID::PaletteGhost);
constexpr const ImageId HighlightMarker = ImageId(0).WithRemap(FilterPaletteID::PaletteGhost);
constexpr const ImageId TrackGhost = ImageId(0, FilterPaletteID::PaletteNull);

extern bool gShowDirtyVisuals;
extern bool gPaintBoundingBoxes;
extern bool gPaintBlockedTiles;
extern bool gPaintWidePathsAsGhost;

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

void PaintUtilPushTunnelRotated(PaintSession& session, uint8_t direction, uint16_t height, uint8_t type);

bool PaintAttachToPreviousAttach(PaintSession& session, const ImageId imageId, int32_t x, int32_t y);
bool PaintAttachToPreviousPS(PaintSession& session, const ImageId image_id, int32_t x, int32_t y);
void PaintFloatingMoneyEffect(
    PaintSession& session, money64 amount, StringId string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
    uint32_t rotation);

PaintSession* PaintSessionAlloc(DrawPixelInfo& dpi, uint32_t viewFlags);
void PaintSessionFree(PaintSession* session);
void PaintSessionGenerate(PaintSession& session);
void PaintSessionArrange(PaintSessionCore& session);
void PaintDrawStructs(PaintSession& session);
void PaintDrawMoneyStructs(DrawPixelInfo& dpi, PaintStringStruct* ps);
