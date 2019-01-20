/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT12.h"

#include "../ride/Track.h"
#include "../world/Footpath.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "../world/Wall.h"

uint8_t RCT12TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

uint8_t RCT12TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

bool RCT12TileElementBase::IsLastForTile() const
{
    return (this->flags & TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

uint8_t RCT12SurfaceElement::GetSlope() const
{
    return (slope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

uint32_t RCT12SurfaceElement::GetSurfaceStyle() const
{
    uint32_t retVal = (terrain >> 5) & 7;
    if (type & 1)
        retVal |= (1 << 3);
    return retVal;
}

uint32_t RCT12SurfaceElement::GetEdgeStyle() const
{
    uint32_t terrain_edge = (slope >> 5) & 7;
    if (type & 128)
        terrain_edge |= (1 << 3);
    return terrain_edge;
}

uint8_t RCT12SurfaceElement::GetGrassLength() const
{
    return grass_length;
}

uint8_t RCT12SurfaceElement::GetOwnership() const
{
    return (ownership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

uint32_t RCT12SurfaceElement::GetWaterHeight() const
{
    return terrain & TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
}

uint8_t RCT12SurfaceElement::GetParkFences() const
{
    return (ownership & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

bool RCT12SurfaceElement::HasTrackThatNeedsWater() const
{
    return (type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
}

uint8_t RCT12PathElement::GetEntryIndex() const
{
    return (entryIndex & FOOTPATH_PROPERTIES_TYPE_MASK) >> 4;
}

uint8_t RCT12PathElement::GetQueueBannerDirection() const
{
    return ((type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

bool RCT12PathElement::IsSloped() const
{
    return (entryIndex & FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) != 0;
}

uint8_t RCT12PathElement::GetSlopeDirection() const
{
    return entryIndex & FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

uint8_t RCT12PathElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12PathElement::GetStationIndex() const
{
    return (additions & FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK) >> 4;
}

bool RCT12PathElement::IsWide() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
}

bool RCT12PathElement::IsQueue() const
{
    return (type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
}

bool RCT12PathElement::HasQueueBanner() const
{
    return (entryIndex & FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER) != 0;
}
uint8_t RCT12PathElement::GetEdges() const
{
    return edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
}

uint8_t RCT12PathElement::GetCorners() const
{
    return edges >> 4;
}

uint8_t RCT12PathElement::GetAddition() const
{
    return additions & FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
}

bool RCT12PathElement::AdditionIsGhost() const
{
    return (additions & FOOTPATH_ADDITION_FLAG_IS_GHOST) != 0;
}

uint8_t RCT12PathElement::GetAdditionStatus() const
{
    return additionStatus;
}

uint8_t RCT12PathElement::GetRCT1PathType() const
{
    uint8_t pathColour = type & 3;
    uint8_t pathType2 = (entryIndex & FOOTPATH_PROPERTIES_TYPE_MASK) >> 2;

    pathType2 = pathType2 | pathColour;
    return pathType2;
}

uint8_t RCT12PathElement::GetRCT1SupportType() const
{
    return (flags & 0b01100000) >> 5;
}

uint8_t RCT12TrackElement::GetTrackType() const
{
    return trackType;
}

uint8_t RCT12TrackElement::GetSequenceIndex() const
{
    return sequence & MAP_ELEM_TRACK_SEQUENCE_SEQUENCE_MASK;
}

uint8_t RCT12TrackElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12TrackElement::GetColourScheme() const
{
    return colour & 0x3;
}

uint8_t RCT12TrackElement::GetStationIndex() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

bool RCT12TrackElement::HasChain() const
{
    return type & TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

bool RCT12TrackElement::HasCableLift() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool RCT12TrackElement::IsInverted() const
{
    return colour & TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

uint8_t RCT12TrackElement::GetBrakeBoosterSpeed() const
{
    return (sequence >> 4) << 1;
}

bool RCT12TrackElement::HasGreenLight() const
{
    return (sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
}

uint8_t RCT12TrackElement::GetSeatRotation() const
{
    return colour >> 4;
}

uint16_t RCT12TrackElement::GetMazeEntry() const
{
    return mazeEntry;
}

uint8_t RCT12TrackElement::GetPhotoTimeout() const
{
    return sequence >> 4;
}

uint8_t RCT12TrackElement::GetDoorAState() const
{
    return (colour & TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t RCT12TrackElement::GetDoorBState() const
{
    return (colour & TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

uint8_t RCT12SmallSceneryElement::GetEntryIndex() const
{
    return this->entryIndex;
}

uint8_t RCT12SmallSceneryElement::GetAge() const
{
    return this->age;
}

uint8_t RCT12SmallSceneryElement::GetSceneryQuadrant() const
{
    return (this->type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

colour_t RCT12SmallSceneryElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12SmallSceneryElement::GetSecondaryColour() const
{
    return colour_2 & TILE_ELEMENT_COLOUR_MASK;
}

bool RCT12SmallSceneryElement::NeedsSupports() const
{
    return (bool)(colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS);
}

uint32_t RCT12LargeSceneryElement::GetEntryIndex() const
{
    return entryIndex & TILE_ELEMENT_LARGE_TYPE_MASK;
}

uint16_t RCT12LargeSceneryElement::GetSequenceIndex() const
{
    return (entryIndex >> 10);
}
colour_t RCT12LargeSceneryElement::GetPrimaryColour() const
{
    return colour[0] & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12LargeSceneryElement::GetSecondaryColour() const
{
    return colour[1] & TILE_ELEMENT_COLOUR_MASK;
}

BannerIndex RCT12LargeSceneryElement::GetBannerIndex() const
{
    return (type & 0xC0) | (((colour[0]) & ~TILE_ELEMENT_COLOUR_MASK) >> 2) | (((colour[1]) & ~TILE_ELEMENT_COLOUR_MASK) >> 5);
}

uint8_t RCT12WallElement::GetEntryIndex() const
{
    return entryIndex;
}

uint8_t RCT12WallElement::GetSlope() const
{
    return (type & TILE_ELEMENT_QUADRANT_MASK) >> 6;
}

colour_t RCT12WallElement::GetPrimaryColour() const
{
    return colour_1 & TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12WallElement::GetSecondaryColour() const
{
    uint8_t secondaryColour = (colour_1 & ~TILE_ELEMENT_COLOUR_MASK) >> 5;
    secondaryColour |= (flags & 0x60) >> 2;
    return secondaryColour;
}

colour_t RCT12WallElement::GetTertiaryColour() const
{
    return colour_3 & TILE_ELEMENT_COLOUR_MASK;
}

uint8_t RCT12WallElement::GetAnimationFrame() const
{
    return (animation >> 3) & 0xF;
}

BannerIndex RCT12WallElement::GetBannerIndex() const
{
    return banner_index;
}

bool RCT12WallElement::IsAcrossTrack() const
{
    return (animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
}

bool RCT12WallElement::AnimationIsBackwards() const
{
    return (animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
}

uint32_t RCT12WallElement::GetRawRCT1WallTypeData() const
{
    return colour_3 | (colour_1 << 8) | (animation << 16);
}

int32_t RCT12WallElement::GetRCT1WallType(int32_t edge) const
{
    uint8_t var_05 = colour_3;
    uint16_t var_06 = colour_1 | (animation << 8);

    int32_t typeA = (var_05 >> (edge * 2)) & 3;
    int32_t typeB = (var_06 >> (edge * 4)) & 0x0F;

    if (typeB != 0x0F)
    {
        return typeA | (typeB << 2);
    }
    else
    {
        return -1;
    }
}

colour_t RCT12WallElement::GetRCT1WallColour() const
{
    return ((type & 0xC0) >> 3) | ((entryIndex & 0xE0) >> 5);
}

uint8_t RCT12EntranceElement::GetEntranceType() const
{
    return entranceType;
}

uint8_t RCT12EntranceElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12EntranceElement::GetStationIndex() const
{
    return (index & MAP_ELEM_TRACK_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

uint8_t RCT12EntranceElement::GetSequenceIndex() const
{
    return index & 0xF;
}

uint8_t RCT12EntranceElement::GetPathType() const
{
    return pathType;
}

BannerIndex RCT12BannerElement::GetIndex() const
{
    return index;
}

uint8_t RCT12BannerElement::GetPosition() const
{
    return position;
}

uint8_t RCT12BannerElement::GetAllowedEdges() const
{
    return flags & 0b00001111;
}
