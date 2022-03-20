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
#include "../world/Location.hpp"
#include "../world/ScenerySelection.h"
#include "TileElement.h"

#include <limits>
#include <string_view>

#define SCENERY_WITHER_AGE_THRESHOLD_1 0x28
#define SCENERY_WITHER_AGE_THRESHOLD_2 0x37

struct LargeSceneryText;

#pragma pack(push, 1)

struct SceneryEntryBase
{
    rct_string_id name;
    uint32_t image;
};

struct rct_large_scenery_tile
{
    int16_t x_offset;
    int16_t y_offset;
    int16_t z_offset;
    uint8_t z_clearance;
    // CCCC WWWW 0SS0 0000
    uint16_t flags;
};
assert_struct_size(rct_large_scenery_tile, 9);

enum
{
    LARGE_SCENERY_TILE_FLAG_NO_SUPPORTS = 0x20,
    LARGE_SCENERY_TILE_FLAG_ALLOW_SUPPORTS_ABOVE = 0x40,
};

struct rct_large_scenery_text_glyph
{
    uint8_t image_offset;
    uint8_t width;
    uint8_t height;
    uint8_t pad_3;
};
assert_struct_size(rct_large_scenery_text_glyph, 4);

struct rct_large_scenery_text
{
    struct
    {
        int16_t x, y;
    } offset[2];                              // 0x0
    uint16_t max_width;                       // 0x8
    uint16_t pad_A;                           // 0xA
    uint8_t flags;                            // 0xC
    uint8_t num_images;                       // 0xD
    rct_large_scenery_text_glyph glyphs[256]; // 0xE
};
assert_struct_size(rct_large_scenery_text, 14 + 4 * 256);

enum LARGE_SCENERY_TEXT_FLAGS
{
    LARGE_SCENERY_TEXT_FLAG_VERTICAL = (1 << 0), // 0x1
    LARGE_SCENERY_TEXT_FLAG_TWO_LINE = (1 << 1), // 0x2
};

struct LargeSceneryEntry : SceneryEntryBase
{
    CursorID tool_id;
    uint8_t flags;
    int16_t price;
    int16_t removal_price;
    rct_large_scenery_tile* tiles;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
    LargeSceneryText* text;
    uint32_t text_image;
};

enum LARGE_SCENERY_FLAGS
{
    LARGE_SCENERY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    LARGE_SCENERY_FLAG_HAS_SECONDARY_COLOUR = (1 << 1), // 0x2
    LARGE_SCENERY_FLAG_3D_TEXT = (1 << 2),              // 0x4
    LARGE_SCENERY_FLAG_ANIMATED = (1 << 3),             // 0x8
    LARGE_SCENERY_FLAG_PHOTOGENIC = (1 << 4),           // 0x10
    LARGE_SCENERY_FLAG_IS_TREE = (1 << 5),              // 0x20
    LARGE_SCENERY_FLAG_HAS_TERTIARY_COLOUR = (1 << 6),  // 0x40
};

enum WALL_SCENERY_FLAGS
{
    WALL_SCENERY_HAS_PRIMARY_COLOUR = (1 << 0),   // 0x1
    WALL_SCENERY_HAS_GLASS = (1 << 1),            // 0x2
    WALL_SCENERY_CANT_BUILD_ON_SLOPE = (1 << 2),  // 0x4
    WALL_SCENERY_IS_DOUBLE_SIDED = (1 << 3),      // 0x8
    WALL_SCENERY_IS_DOOR = (1 << 4),              // 0x10
    WALL_SCENERY_LONG_DOOR_ANIMATION = (1 << 5),  // 0x20
    WALL_SCENERY_HAS_SECONDARY_COLOUR = (1 << 6), // 0x40
    WALL_SCENERY_HAS_TERTIARY_COLOUR = (1 << 7),  // 0x80
};

enum WALL_SCENERY_2_FLAGS
{
    WALL_SCENERY_2_NO_SELECT_PRIMARY_COLOUR = (1 << 0), // 0x1
    WALL_SCENERY_2_DOOR_SOUND_MASK = 0x6,
    WALL_SCENERY_2_DOOR_SOUND_SHIFT = 1,
    WALL_SCENERY_2_IS_OPAQUE = (1 << 3), // 0x8
    WALL_SCENERY_2_ANIMATED = (1 << 4),  // 0x10
};

struct SmallSceneryEntry : SceneryEntryBase
{
    uint32_t flags;
    uint8_t height;
    CursorID tool_id;
    int16_t price;
    int16_t removal_price;
    uint8_t* frame_offsets;
    uint16_t animation_delay;
    uint16_t animation_mask;
    uint16_t num_frames;
    ObjectEntryIndex scenery_tab_id;

    constexpr bool HasFlag(const uint32_t _flags) const
    {
        return (flags & _flags) != 0;
    }
};

struct WallSceneryEntry : SceneryEntryBase
{
    CursorID tool_id;
    uint8_t flags;
    uint8_t height;
    uint8_t flags2;
    int16_t price;
    ObjectEntryIndex scenery_tab_id;
    uint8_t scrolling_mode;
};
enum class PathBitDrawType : uint8_t;

struct PathBitEntry : SceneryEntryBase
{
    uint16_t flags;
    PathBitDrawType draw_type;
    CursorID tool_id;
    int16_t price;
    ObjectEntryIndex scenery_tab_id;
};

struct BannerSceneryEntry : SceneryEntryBase
{
    uint8_t scrolling_mode;
    uint8_t flags;
    int16_t price;
    ObjectEntryIndex scenery_tab_id;
};

#pragma pack(pop)

struct LargeSceneryText
{
    CoordsXY offset[2];
    uint16_t max_width;
    uint8_t flags;
    uint16_t num_images;
    rct_large_scenery_text_glyph glyphs[256];

    LargeSceneryText() = default;
    explicit LargeSceneryText(const rct_large_scenery_text& original);
    const rct_large_scenery_text_glyph* GetGlyph(char32_t codepoint) const;
    const rct_large_scenery_text_glyph& GetGlyph(char32_t codepoint, char32_t defaultCodepoint) const;
    int32_t MeasureWidth(std::string_view text) const;
    int32_t MeasureHeight(std::string_view text) const;
};

struct rct_scenery_group_entry
{
    rct_string_id name;
    uint32_t image;
    ScenerySelection scenery_entries[0x80];
    uint8_t entry_count;
    uint8_t priority;
    uint32_t entertainer_costumes;
};

enum
{
    PATH_BIT_FLAG_IS_BIN = 1 << 0,
    PATH_BIT_FLAG_IS_BENCH = 1 << 1,
    PATH_BIT_FLAG_BREAKABLE = 1 << 2,
    PATH_BIT_FLAG_LAMP = 1 << 3,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_WATER = 1 << 4,
    PATH_BIT_FLAG_JUMPING_FOUNTAIN_SNOW = 1 << 5,
    PATH_BIT_FLAG_DONT_ALLOW_ON_QUEUE = 1 << 6,
    PATH_BIT_FLAG_DONT_ALLOW_ON_SLOPE = 1 << 7,
    PATH_BIT_FLAG_IS_QUEUE_SCREEN = 1 << 8
};

enum class PathBitDrawType : uint8_t
{
    Light,
    Bin,
    Bench,
    JumpingFountain,
};

enum
{
    SCENERY_TYPE_SMALL,
    SCENERY_TYPE_PATH_ITEM,
    SCENERY_TYPE_WALL,
    SCENERY_TYPE_LARGE,
    SCENERY_TYPE_BANNER,

    SCENERY_TYPE_COUNT,
};

enum
{
    SCENERY_GHOST_FLAG_0 = (1 << SCENERY_TYPE_SMALL),
    SCENERY_GHOST_FLAG_1 = (1 << SCENERY_TYPE_PATH_ITEM),
    SCENERY_GHOST_FLAG_2 = (1 << SCENERY_TYPE_WALL),
    SCENERY_GHOST_FLAG_3 = (1 << SCENERY_TYPE_LARGE),
    SCENERY_GHOST_FLAG_4 = (1 << SCENERY_TYPE_BANNER)
};

enum
{
    BANNER_ENTRY_FLAG_HAS_PRIMARY_COLOUR = (1 << 0),
};

enum class ScatterToolDensity : uint8_t
{
    LowDensity,
    MediumDensity,
    HighDensity
};

extern uint8_t gSceneryQuadrant;

extern money32 gSceneryPlaceCost;
extern ScenerySelection gSceneryPlaceObject;
extern uint16_t gSceneryPlaceObjectEntryIndex;
extern int16_t gSceneryPlaceZ;
extern uint8_t gSceneryPlaceRotation;

extern uint8_t gSceneryGhostType;
extern CoordsXYZ gSceneryGhostPosition;
extern uint8_t gSceneryGhostWallRotation;

extern int16_t gSceneryShiftPressed;
extern int16_t gSceneryShiftPressX;
extern int16_t gSceneryShiftPressY;
extern int16_t gSceneryShiftPressZOffset;

extern int16_t gSceneryCtrlPressed;
extern int16_t gSceneryCtrlPressZ;

extern const CoordsXY SceneryQuadrantOffsets[];

extern money64 gClearSceneryCost;

void init_scenery();
void scenery_update_tile(const CoordsXY& sceneryPos);
void scenery_set_default_placement_configuration();
void scenery_remove_ghost_tool_placement();

WallSceneryEntry* get_wall_entry(ObjectEntryIndex entryIndex);
BannerSceneryEntry* get_banner_entry(ObjectEntryIndex entryIndex);
PathBitEntry* get_footpath_item_entry(ObjectEntryIndex entryIndex);
rct_scenery_group_entry* get_scenery_group_entry(ObjectEntryIndex entryIndex);

int32_t wall_entry_get_door_sound(const WallSceneryEntry* wallEntry);

bool IsSceneryAvailableToBuild(const ScenerySelection& item);

bool IsSceneryItemRestricted(const ScenerySelection& item);
void ClearRestrictedScenery();
void RestrictAllMiscScenery();
void MarkAllUnrestrictedSceneryAsInvented();
std::vector<ScenerySelection>& GetRestrictedScenery();

ObjectType GetObjectTypeFromSceneryType(uint8_t type);
uint8_t GetSceneryTypeFromObjectType(ObjectType type);
