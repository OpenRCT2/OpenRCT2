/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
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

#include <mutex>
#include <thread>

struct TileElement;
enum class ViewportInteractionItem : uint8_t;

struct attached_paint_struct
{
    attached_paint_struct* next;
    uint32_t image_id;
    union
    {
        uint32_t tertiary_colour;
        // If masked image_id is masked_id
        uint32_t colour_image_id;
    };
    int32_t x;
    int32_t y;
    uint8_t flags;
};

struct paint_struct_bound_box
{
    int32_t x;
    int32_t y;
    int32_t z;
    int32_t x_end;
    int32_t y_end;
    int32_t z_end;
};

struct paint_struct
{
    paint_struct_bound_box bounds;
    attached_paint_struct* attached_ps;
    paint_struct* children;
    paint_struct* next_quadrant_ps;
    TileElement* tileElement;
    uint32_t image_id;
    union
    {
        uint32_t tertiary_colour;
        // If masked image_id is masked_id
        uint32_t colour_image_id;
    };
    int32_t x;
    int32_t y;
    int32_t map_x;
    int32_t map_y;
    uint16_t quadrant_index;
    uint8_t flags;
    uint8_t SortFlags;
    ViewportInteractionItem sprite_type;
};

struct paint_string_struct
{
    rct_string_id string_id;
    paint_string_struct* next;
    int32_t x;
    int32_t y;
    uint32_t args[4];
    uint8_t* y_offsets;
};

union paint_entry
{
    paint_struct basic;
    attached_paint_struct attached;
    paint_string_struct string;
};

struct sprite_bb
{
    uint32_t sprite_id;
    CoordsXYZ offset;
    CoordsXYZ bb_offset;
    CoordsXYZ bb_size;
};

enum PAINT_STRUCT_FLAGS
{
    PAINT_STRUCT_FLAG_IS_MASKED = (1 << 0)
};

struct support_height
{
    uint16_t height;
    uint8_t slope;
    uint8_t pad;
};

struct tunnel_entry
{
    uint8_t height;
    uint8_t type;
};

#define MAX_PAINT_QUADRANTS 512
#define TUNNEL_MAX_COUNT 65

/**
 * A pool of paint_entry instances that can be rented out.
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
        paint_entry PaintStructs[NodeSize]{};
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

        paint_entry* Allocate();
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
    paint_struct* Quadrants[MAX_PAINT_QUADRANTS];
    paint_struct* LastPS;
    paint_string_struct* PSStringHead;
    paint_string_struct* LastPSString;
    attached_paint_struct* LastAttachedPS;
    const TileElement* SurfaceElement;
    const void* CurrentlyDrawnItem;
    const TileElement* PathElementOnSameHeight;
    const TileElement* TrackElementOnSameHeight;
    paint_struct PaintHead;
    uint32_t ViewFlags;
    uint32_t QuadrantBackIndex;
    uint32_t QuadrantFrontIndex;
    CoordsXY SpritePosition;
    ViewportInteractionItem InteractionType;
    uint8_t CurrentRotation;
    support_height SupportSegments[9];
    support_height Support;
    paint_struct* WoodenSupportsPrependTo;
    CoordsXY MapPosition;
    tunnel_entry LeftTunnels[TUNNEL_MAX_COUNT];
    uint8_t LeftTunnelCount;
    tunnel_entry RightTunnels[TUNNEL_MAX_COUNT];
    uint8_t RightTunnelCount;
    uint8_t VerticalTunnelHeight;
    bool DidPassSurface;
    bool BlockSupports;
    uint8_t Unk141E9DB;
    uint16_t WaterHeight;
    uint32_t TrackColours[4];
};

struct paint_session : public PaintSessionCore
{
    rct_drawpixelinfo DPI;
    PaintEntryPool::Chain PaintEntryChain;

    paint_struct* AllocateNormalPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
            LastPS = &entry->basic;
            return LastPS;
        }
        return nullptr;
    }

    attached_paint_struct* AllocateAttachedPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
            LastAttachedPS = &entry->attached;
            return LastAttachedPS;
        }
        return nullptr;
    }

    paint_string_struct* AllocateStringPaintEntry() noexcept
    {
        auto* entry = PaintEntryChain.Allocate();
        if (entry != nullptr)
        {
            auto* string = &entry->string;
            if (LastPSString == nullptr)
            {
                PSStringHead = string;
            }
            else
            {
                LastPSString->next = string;
            }
            LastPSString = string;
            return LastPSString;
        }
        return nullptr;
    }
};

struct RecordedPaintSession
{
    PaintSessionCore Session;
    std::vector<paint_entry> Entries;
};

extern paint_session gPaintSession;

// Globals for paint clipping
extern uint8_t gClipHeight;
extern CoordsXY gClipSelectionA;
extern CoordsXY gClipSelectionB;

/** rct2: 0x00993CC4. The white ghost that indicates not-yet-built elements. */
#define CONSTRUCTION_MARKER (COLOUR_DARK_GREEN << 19 | COLOUR_GREY << 24 | IMAGE_TYPE_REMAP)
extern bool gShowDirtyVisuals;
extern bool gPaintBoundingBoxes;
extern bool gPaintBlockedTiles;
extern bool gPaintWidePathsAsGhost;

paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize);
paint_struct* PaintAddImageAsParent(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxSize,
    const CoordsXYZ& boundBoxOffset);
[[nodiscard]] paint_struct* PaintAddImageAsOrphan(
    paint_session* session, uint32_t image_id, int32_t x_offset, int32_t y_offset, int32_t bound_box_length_x,
    int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset, int32_t bound_box_offset_x,
    int32_t bound_box_offset_y, int32_t bound_box_offset_z);
paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, int32_t x_offset, int32_t y_offset, int32_t bound_box_length_x,
    int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset, int32_t bound_box_offset_x,
    int32_t bound_box_offset_y, int32_t bound_box_offset_z);
paint_struct* PaintAddImageAsChild(
    paint_session* session, uint32_t image_id, const CoordsXYZ& offset, const CoordsXYZ& boundBoxLength,
    const CoordsXYZ& boundBoxOffset);

paint_struct* PaintAddImageAsParentRotated(
    paint_session* session, uint8_t direction, uint32_t image_id, int32_t x_offset, int32_t y_offset,
    int32_t bound_box_length_x, int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset);
paint_struct* PaintAddImageAsParentRotated(
    paint_session* session, uint8_t direction, uint32_t image_id, int32_t x_offset, int32_t y_offset,
    int32_t bound_box_length_x, int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset,
    int32_t bound_box_offset_x, int32_t bound_box_offset_y, int32_t bound_box_offset_z);
paint_struct* PaintAddImageAsChildRotated(
    paint_session* session, uint8_t direction, uint32_t image_id, int32_t x_offset, int32_t y_offset,
    int32_t bound_box_length_x, int32_t bound_box_length_y, int32_t bound_box_length_z, int32_t z_offset,
    int32_t bound_box_offset_x, int32_t bound_box_offset_y, int32_t bound_box_offset_z);
paint_struct* PaintAddImageAsChildRotated(
    paint_session* session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset);
paint_struct* PaintAddImageAsParentRotated(
    paint_session* session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize);
paint_struct* PaintAddImageAsParentRotated(
    paint_session* session, const uint8_t direction, const uint32_t image_id, const CoordsXYZ& offset,
    const CoordsXYZ& boundBoxSize, const CoordsXYZ& boundBoxOffset);

void paint_util_push_tunnel_rotated(paint_session* session, uint8_t direction, uint16_t height, uint8_t type);

bool PaintAttachToPreviousAttach(paint_session* session, uint32_t image_id, int32_t x, int32_t y);
bool PaintAttachToPreviousPS(paint_session* session, uint32_t image_id, int32_t x, int32_t y);
void PaintFloatingMoneyEffect(
    paint_session* session, money64 amount, rct_string_id string_id, int32_t y, int32_t z, int8_t y_offsets[], int32_t offset_x,
    uint32_t rotation);

paint_session* PaintSessionAlloc(rct_drawpixelinfo* dpi, uint32_t viewFlags);
void PaintSessionFree(paint_session* session);
void PaintSessionGenerate(paint_session* session);
void PaintSessionArrange(PaintSessionCore* session);
void PaintDrawStructs(paint_session* session);
void PaintDrawMoneyStructs(rct_drawpixelinfo* dpi, paint_string_struct* ps);

// TESTING
#ifdef __TESTPAINT__
void testpaint_clear_ignore();
void testpaint_ignore(uint8_t direction, uint8_t trackSequence);
void testpaint_ignore_all();
bool testpaint_is_ignored(uint8_t direction, uint8_t trackSequence);

#    define TESTPAINT_IGNORE(direction, trackSequence) testpaint_ignore(direction, trackSequence)
#    define TESTPAINT_IGNORE_ALL() testpaint_ignore_all()
#else
#    define TESTPAINT_IGNORE(direction, trackSequence)
#    define TESTPAINT_IGNORE_ALL()
#endif
