/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT12.h"

#include "../core/String.hpp"
#include "../localisation/Formatting.h"
#include "../localisation/Localisation.h"
#include "../object/ObjectList.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../world/Banner.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "../world/Wall.h"

using namespace OpenRCT2;

uint8_t RCT12TileElementBase::GetType() const
{
    return this->type & TILE_ELEMENT_TYPE_MASK;
}

uint8_t RCT12TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t RCT12TileElementBase::GetOccupiedQuadrants() const
{
    return flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
}

void RCT12TileElementBase::SetOccupiedQuadrants(uint8_t quadrants)
{
    flags &= ~TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
    flags |= (quadrants & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK);
}

bool RCT12TileElementBase::IsLastForTile() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

bool RCT12TileElementBase::IsGhost() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_GHOST) != 0;
}

void RCT12TileElementBase::SetLastForTile(bool on)
{
    if (on)
        flags |= RCT12_TILE_ELEMENT_FLAG_LAST_TILE;
    else
        flags &= ~RCT12_TILE_ELEMENT_FLAG_LAST_TILE;
}

void RCT12TileElementBase::SetGhost(bool isGhost)
{
    if (isGhost)
    {
        this->flags |= RCT12_TILE_ELEMENT_FLAG_GHOST;
    }
    else
    {
        this->flags &= ~RCT12_TILE_ELEMENT_FLAG_GHOST;
    }
}

uint8_t RCT12SurfaceElement::GetSlope() const
{
    return (slope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

uint32_t RCT12SurfaceElement::GetSurfaceStyle() const
{
    uint32_t retVal = (terrain >> 5) & 7;
    retVal |= (type & RCT12_SURFACE_ELEMENT_TYPE_SURFACE_MASK) << 3;
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
    return (terrain & RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK) * 16;
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
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 4;
}

uint8_t RCT12PathElement::GetQueueBannerDirection() const
{
    return ((type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

bool RCT12PathElement::IsSloped() const
{
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) != 0;
}

uint8_t RCT12PathElement::GetSlopeDirection() const
{
    return entryIndex & RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

uint8_t RCT12PathElement::GetRideIndex() const
{
    return rideIndex;
}

uint8_t RCT12PathElement::GetStationIndex() const
{
    return (additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK) >> 4;
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
    return (entryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER) != 0;
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
    return additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
}

bool RCT12PathElement::AdditionIsGhost() const
{
    return (additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST) != 0;
}

uint8_t RCT12PathElement::GetAdditionStatus() const
{
    return additionStatus;
}

uint8_t RCT12PathElement::GetRCT1PathType() const
{
    uint8_t pathColour = type & 3;
    uint8_t pathType2 = (entryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 2;

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
    return sequence & RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
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
    if (track_type_is_station(trackType) || trackType == TrackElemType::TowerBase)
    {
        return (sequence & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
    }
    return 0;
}

bool RCT12TrackElement::HasChain() const
{
    return type & RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

bool RCT12TrackElement::HasCableLift() const
{
    return colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool RCT12TrackElement::IsInverted() const
{
    return colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

uint8_t RCT12TrackElement::GetBrakeBoosterSpeed() const
{
    if (TrackTypeHasSpeedSetting(GetTrackType()))
    {
        return (sequence >> 4) << 1;
    }
    return 0;
}

bool RCT12TrackElement::HasGreenLight() const
{
    if (track_type_is_station(trackType))
    {
        return (sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
    }
    return false;
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
    if (GetTrackType() == TrackElemType::OnRidePhoto)
    {
        return sequence >> 4;
    }
    return 0;
}

uint8_t RCT12TrackElement::GetDoorAState() const
{
    return (colour & RCT12_TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t RCT12TrackElement::GetDoorBState() const
{
    return (colour & RCT12_TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

void RCT12TrackElement::SetDoorAState(uint8_t newState)
{
    colour &= ~RCT12_TRACK_ELEMENT_DOOR_B_MASK;
    colour |= ((newState << 2) & RCT12_TRACK_ELEMENT_DOOR_B_MASK);
}

void RCT12TrackElement::SetDoorBState(uint8_t newState)
{
    colour &= ~RCT12_TRACK_ELEMENT_DOOR_B_MASK;
    colour |= ((newState << 5) & RCT12_TRACK_ELEMENT_DOOR_B_MASK);
}

bool RCT12TrackElement::IsIndestructible() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) != 0;
}

void RCT12TrackElement::SetIsIndestructible(bool isIndestructible)
{
    if (isIndestructible)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE;
    }
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
    return colour_1 & MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

uint32_t RCT12LargeSceneryElement::GetEntryIndex() const
{
    return entryIndex & RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
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

uint8_t RCT12LargeSceneryElement::GetBannerIndex() const
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

uint8_t RCT12WallElement::GetBannerIndex() const
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

    return -1;
}

colour_t RCT12WallElement::GetRCT1WallColour() const
{
    return ((type & 0xC0) >> 3) | ((entryIndex & 0xE0) >> 5);
}

uint8_t RCT12WallElement::GetRCT1Slope() const
{
    return entryIndex & 0b00011111;
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
    return (index & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

uint8_t RCT12EntranceElement::GetSequenceIndex() const
{
    return index & 0xF;
}

uint8_t RCT12EntranceElement::GetPathType() const
{
    return pathType;
}

uint8_t RCT12BannerElement::GetIndex() const
{
    return index;
}

uint8_t RCT12BannerElement::GetPosition() const
{
    return position;
}

uint8_t RCT12BannerElement::GetAllowedEdges() const
{
    return AllowedEdges & 0b00001111;
}

bool is_user_string_id(rct_string_id stringId)
{
    return stringId >= 0x8000 && stringId < 0x9000;
}

uint8_t RCT12TileElement::GetBannerIndex()
{
    switch (GetType())
    {
        case TILE_ELEMENT_TYPE_LARGE_SCENERY:
        {
            auto* sceneryEntry = get_large_scenery_entry(AsLargeScenery()->GetEntryIndex());
            if (sceneryEntry->scrolling_mode == SCROLLING_MODE_NONE)
                return RCT12_BANNER_INDEX_NULL;

            return AsLargeScenery()->GetBannerIndex();
        }
        case TILE_ELEMENT_TYPE_WALL:
        {
            auto* wallEntry = get_wall_entry(AsWall()->GetEntryIndex());
            if (wallEntry == nullptr || wallEntry->scrolling_mode == SCROLLING_MODE_NONE)
                return RCT12_BANNER_INDEX_NULL;

            return AsWall()->GetBannerIndex();
        }
        case TILE_ELEMENT_TYPE_BANNER:
            return AsBanner()->GetIndex();
        default:
            return RCT12_BANNER_INDEX_NULL;
    }
}

void RCT12TileElementBase::SetDirection(uint8_t direction)
{
    this->type &= ~TILE_ELEMENT_DIRECTION_MASK;
    this->type |= (direction & TILE_ELEMENT_DIRECTION_MASK);
}

void RCT12TileElement::ClearAs(uint8_t newType)
{
    type = newType;
    flags = 0;
    base_height = MINIMUM_LAND_HEIGHT;
    clearance_height = MINIMUM_LAND_HEIGHT;
    std::fill_n(pad_04, sizeof(pad_04), 0x00);
}

void RCT12LargeSceneryElement::SetEntryIndex(uint32_t newIndex)
{
    entryIndex &= ~RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (newIndex & RCT12_TILE_ELEMENT_LARGE_TYPE_MASK);
}

void RCT12LargeSceneryElement::SetSequenceIndex(uint16_t sequence)
{
    entryIndex &= RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
    entryIndex |= (sequence << 10);
}

void RCT12LargeSceneryElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[0] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[0] |= newColour;
}

void RCT12LargeSceneryElement::SetSecondaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour[1] &= ~TILE_ELEMENT_COLOUR_MASK;
    colour[1] |= newColour;
}

void RCT12LargeSceneryElement::SetBannerIndex(uint8_t newIndex)
{
    type |= newIndex & 0xC0;
    colour[0] |= (newIndex & 0x38) << 2;
    colour[1] |= (newIndex & 7) << 5;
}

void RCT12SurfaceElement::SetSurfaceStyle(uint32_t newStyle)
{
    // Bits 3, 4 for terrain are stored in element.type bit 0, 1
    type &= ~RCT12_SURFACE_ELEMENT_TYPE_SURFACE_MASK;
    type |= (newStyle >> 3) & RCT12_SURFACE_ELEMENT_TYPE_SURFACE_MASK;

    // Bits 0, 1, 2 for terrain are stored in element.terrain bit 5, 6, 7
    terrain &= ~0xE0;
    terrain |= (newStyle & 7) << 5;
}

void RCT12SurfaceElement::SetEdgeStyle(uint32_t newStyle)
{
    // Bit 3 for terrain is stored in element.type bit 7
    if (newStyle & 8)
        type |= 128;
    else
        type &= ~128;

    // Bits 0, 1, 2 for terrain are stored in element.slope bit 5, 6, 7
    slope &= ~RCT12_TILE_ELEMENT_SURFACE_EDGE_STYLE_MASK;
    slope |= (newStyle & 7) << 5;
}

void RCT12SurfaceElement::SetWaterHeight(uint32_t newWaterHeight)
{
    newWaterHeight >>= 4;
    newWaterHeight &= 0x1F;
    terrain &= ~RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK;
    terrain |= newWaterHeight;
}

void RCT12SurfaceElement::SetGrassLength(uint8_t newLength)
{
    grass_length = newLength;
}

void RCT12SurfaceElement::SetOwnership(uint8_t newOwnership)
{
    ownership &= ~TILE_ELEMENT_SURFACE_OWNERSHIP_MASK;
    ownership |= (newOwnership & TILE_ELEMENT_SURFACE_OWNERSHIP_MASK);
}

void RCT12SurfaceElement::SetParkFences(uint8_t newParkFences)
{
    ownership &= ~TILE_ELEMENT_SURFACE_PARK_FENCE_MASK;
    ownership |= (newParkFences & TILE_ELEMENT_SURFACE_PARK_FENCE_MASK);
}

void RCT12SurfaceElement::SetSlope(uint8_t newSlope)
{
    slope &= ~TILE_ELEMENT_SURFACE_SLOPE_MASK;
    slope |= (newSlope & TILE_ELEMENT_SURFACE_SLOPE_MASK);
}

void RCT12SurfaceElement::SetHasTrackThatNeedsWater(bool on)
{
    type &= ~SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
    if (on)
        type |= SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER;
}

void RCT12PathElement::SetPathEntryIndex(uint8_t newEntryIndex)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_TYPE_MASK;
    entryIndex |= (newEntryIndex << 4);
}

void RCT12PathElement::SetQueueBannerDirection(uint8_t direction)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK;
    type |= (direction << 6);
}

void RCT12PathElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12PathElement::SetAdditionStatus(uint8_t newStatus)
{
    additionStatus = newStatus;
}

void RCT12PathElement::SetEdges(uint8_t newEdges)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
    edges |= (newEdges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK);
}

void RCT12PathElement::SetCorners(uint8_t newCorners)
{
    edges &= ~FOOTPATH_PROPERTIES_EDGES_CORNERS_MASK;
    edges |= (newCorners << 4);
}

void RCT12PathElement::SetSloped(bool isSloped)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
    if (isSloped)
        entryIndex |= RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED;
}

void RCT12PathElement::SetSlopeDirection(uint8_t newSlope)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
    entryIndex |= newSlope & RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

void RCT12PathElement::SetStationIndex(uint8_t newStationIndex)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK;
    additions |= ((newStationIndex << 4) & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK);
}

void RCT12PathElement::SetWide(bool isWide)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
    if (isWide)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE;
}

void RCT12PathElement::SetIsQueue(bool isQueue)
{
    type &= ~FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
    if (isQueue)
        type |= FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE;
}

void RCT12PathElement::SetHasQueueBanner(bool hasQueueBanner)
{
    entryIndex &= ~RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
    if (hasQueueBanner)
        entryIndex |= RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER;
}

void RCT12PathElement::SetAddition(uint8_t newAddition)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
    additions |= newAddition;
}

void RCT12PathElement::SetAdditionIsGhost(bool isGhost)
{
    additions &= ~RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST;
    if (isGhost)
        additions |= RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST;
}

bool RCT12PathElement::IsBroken() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BROKEN) != 0;
}

void RCT12PathElement::SetIsBroken(bool isBroken)
{
    if (isBroken)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BROKEN;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BROKEN;
    }
}

bool RCT12PathElement::IsBlockedByVehicle() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE) != 0;
}

void RCT12PathElement::SetIsBlockedByVehicle(bool isBlocked)
{
    if (isBlocked)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE;
    }
}

void RCT12TrackElement::SetTrackType(uint8_t newType)
{
    trackType = newType;
}

void RCT12TrackElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    sequence &= ~RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
    sequence |= (newSequenceIndex & RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK);
}

void RCT12TrackElement::SetStationIndex(uint8_t newStationIndex)
{
    if (track_type_is_station(trackType) || trackType == TrackElemType::TowerBase)
    {
        sequence &= ~RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK;
        sequence |= (newStationIndex << 4);
    }
}

void RCT12TrackElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12TrackElement::SetColourScheme(uint8_t newColourScheme)
{
    colour &= ~0x3;
    colour |= (newColourScheme & 0x3);
}

void RCT12TrackElement::SetHasCableLift(bool on)
{
    colour &= ~RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
    if (on)
        colour |= RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

void RCT12TrackElement::SetInverted(bool inverted)
{
    if (inverted)
    {
        colour |= RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
    else
    {
        colour &= ~RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
    }
}

void RCT12TrackElement::SetBrakeBoosterSpeed(uint8_t speed)
{
    if (TrackTypeHasSpeedSetting(GetTrackType()))
    {
        sequence &= ~0b11110000;
        sequence |= ((speed >> 1) << 4);
    }
}

bool RCT12TrackElement::BlockBrakeClosed() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) != 0;
}

void RCT12TrackElement::SetBlockBrakeClosed(bool isClosed)
{
    if (isClosed)
    {
        flags |= RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
    }
    else
    {
        flags &= ~RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED;
    }
}

void RCT12TrackElement::SetHasGreenLight(uint8_t greenLight)
{
    if (track_type_is_station(trackType))
    {
        sequence &= ~MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
        if (greenLight)
        {
            sequence |= MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT;
        }
    }
}

void RCT12TrackElement::SetHasChain(bool on)
{
    if (on)
    {
        type |= RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
    else
    {
        type &= ~RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
    }
}

void RCT12TrackElement::SetSeatRotation(uint8_t newSeatRotation)
{
    colour &= 0x0F;
    colour |= (newSeatRotation << 4);
}

void RCT12TrackElement::SetMazeEntry(uint16_t newMazeEntry)
{
    mazeEntry = newMazeEntry;
}

void RCT12TrackElement::SetPhotoTimeout(uint8_t value)
{
    if (GetTrackType() == TrackElemType::OnRidePhoto)
    {
        sequence &= RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
        sequence |= (value << 4);
    }
}

void RCT12SmallSceneryElement::SetEntryIndex(uint8_t newIndex)
{
    this->entryIndex = newIndex;
}

void RCT12SmallSceneryElement::SetAge(uint8_t newAge)
{
    this->age = newAge;
}

void RCT12SmallSceneryElement::SetPrimaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= colour;
}

void RCT12SmallSceneryElement::SetSecondaryColour(colour_t colour)
{
    assert(colour <= 31);
    colour_2 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_2 |= colour;
}

void RCT12SmallSceneryElement::SetSceneryQuadrant(uint8_t newQuadrant)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newQuadrant << 6);
}

void RCT12SmallSceneryElement::SetNeedsSupports()
{
    colour_1 |= MAP_ELEM_SMALL_SCENERY_COLOUR_FLAG_NEEDS_SUPPORTS;
}

void RCT12WallElement::SetEntryIndex(uint8_t newIndex)
{
    entryIndex = newIndex;
}

void RCT12WallElement::SetBannerIndex(uint8_t newIndex)
{
    banner_index = newIndex;
}

void RCT12WallElement::SetAcrossTrack(bool acrossTrack)
{
    animation &= ~WALL_ANIMATION_FLAG_ACROSS_TRACK;
    if (acrossTrack)
        animation |= WALL_ANIMATION_FLAG_ACROSS_TRACK;
}

void RCT12WallElement::SetAnimationIsBackwards(bool isBackwards)
{
    animation &= ~WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
    if (isBackwards)
        animation |= WALL_ANIMATION_FLAG_DIRECTION_BACKWARD;
}
void RCT12WallElement::SetSlope(uint8_t newSlope)
{
    type &= ~TILE_ELEMENT_QUADRANT_MASK;
    type |= (newSlope << 6);
}

void RCT12WallElement::SetPrimaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_1 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= newColour;
}

void RCT12WallElement::SetSecondaryColour(colour_t newColour)
{
    colour_1 &= TILE_ELEMENT_COLOUR_MASK;
    colour_1 |= (newColour & 0x7) << 5;
    flags &= ~0x60;
    flags |= (newColour & 0x18) << 2;
}

void RCT12WallElement::SetTertiaryColour(colour_t newColour)
{
    assert(newColour <= 31);
    colour_3 &= ~TILE_ELEMENT_COLOUR_MASK;
    colour_3 |= newColour;
}

void RCT12WallElement::SetAnimationFrame(uint8_t frameNum)
{
    animation &= WALL_ANIMATION_FLAG_ALL_FLAGS;
    animation |= (frameNum & 0xF) << 3;
}

void RCT12EntranceElement::SetEntranceType(uint8_t newType)
{
    entranceType = newType;
}

void RCT12EntranceElement::SetRideIndex(uint8_t newRideIndex)
{
    rideIndex = newRideIndex;
}

void RCT12EntranceElement::SetPathType(uint8_t newPathType)
{
    pathType = newPathType;
}

void RCT12EntranceElement::SetSequenceIndex(uint8_t newSequenceIndex)
{
    index &= ~0xF;
    index |= (newSequenceIndex & 0xF);
}

void RCT12EntranceElement::SetStationIndex(uint8_t stationIndex)
{
    index &= ~RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK;
    index |= (stationIndex << 4);
}

void RCT12BannerElement::SetIndex(uint8_t newIndex)
{
    index = newIndex;
}

void RCT12BannerElement::SetPosition(uint8_t newPosition)
{
    position = newPosition;
}

void RCT12BannerElement::SetAllowedEdges(uint8_t newEdges)
{
    AllowedEdges &= ~0b00001111;
    AllowedEdges |= (newEdges & 0b00001111);
}

bool RCT12ResearchItem::IsInventedEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_SEPARATOR;
}

bool RCT12ResearchItem::IsUninventedEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_END;
}

bool RCT12ResearchItem::IsRandomEndMarker() const
{
    return rawValue == RCT12_RESEARCHED_ITEMS_END_2;
}

ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(const RCT12ObjectEntryIndex index)
{
    if (index == RCT12_OBJECT_ENTRY_INDEX_NULL)
        return OBJECT_ENTRY_INDEX_NULL;

    return index;
}

RCT12ObjectEntryIndex OpenRCT2EntryIndexToRCTEntryIndex(const ObjectEntryIndex index)
{
    if (index == OBJECT_ENTRY_INDEX_NULL)
        return RCT12_OBJECT_ENTRY_INDEX_NULL;

    return index;
}

ride_id_t RCT12RideIdToOpenRCT2RideId(const RCT12RideId rideId)
{
    if (rideId == RCT12_RIDE_ID_NULL)
        return RIDE_ID_NULL;

    return static_cast<ride_id_t>(rideId);
}

RCT12RideId OpenRCT2RideIdToRCT12RideId(const ride_id_t rideId)
{
    if (rideId == RIDE_ID_NULL)
        return RCT12_RIDE_ID_NULL;

    return static_cast<RCT12RideId>(rideId);
}

static bool RCT12IsFormatChar(codepoint_t c)
{
    if (c >= RCT2_STRING_FORMAT_ARG_START && c <= RCT2_STRING_FORMAT_ARG_END)
    {
        return true;
    }
    if (c >= RCT2_STRING_FORMAT_COLOUR_START && c <= RCT2_STRING_FORMAT_COLOUR_END)
    {
        return true;
    }
    return false;
}

static bool RCT12IsFormatChar(char c)
{
    return RCT12IsFormatChar(static_cast<codepoint_t>(c));
}

bool IsLikelyUTF8(std::string_view s)
{
    // RCT2 uses CP-1252 so some characters may be >= 128. However we don't expect any
    // characters that are reserved for formatting strings, so if those are found, assume
    // that the string is UTF-8.
    for (auto c : s)
    {
        if (RCT12IsFormatChar(c))
        {
            return true;
        }
    }
    return false;
}

std::string RCT12RemoveFormattingUTF8(std::string_view s)
{
    std::string result;
    result.reserve(s.size() * 2);

    CodepointView codepoints(s);
    for (auto codepoint : codepoints)
    {
        if (!RCT12IsFormatChar(codepoint))
        {
            String::AppendCodepoint(result, codepoint);
        }
    }

    result.shrink_to_fit();
    return result;
}

namespace RCT12FormatCode
{
    constexpr codepoint_t Newline = 5;
    constexpr codepoint_t NewlineSmall = 6;
    constexpr codepoint_t ColourBlack = 142;
    constexpr codepoint_t ColourGrey = 143;
    constexpr codepoint_t ColourWhite = 144;
    constexpr codepoint_t ColourRed = 145;
    constexpr codepoint_t ColourGreen = 146;
    constexpr codepoint_t ColourYellow = 147;
    constexpr codepoint_t ColourTopaz = 148;
    constexpr codepoint_t ColourCeladon = 149;
    constexpr codepoint_t ColourBabyBlue = 150;
    constexpr codepoint_t ColourPaleLavender = 151;
    constexpr codepoint_t ColourPaleGold = 152;
    constexpr codepoint_t ColourLightPink = 153;
    constexpr codepoint_t ColourPearlAqua = 154;
    constexpr codepoint_t ColourPaleSilver = 155;
} // namespace RCT12FormatCode

static FormatToken GetFormatTokenFromRCT12Code(codepoint_t codepoint)
{
    switch (codepoint)
    {
        case RCT12FormatCode::Newline:
            return FormatToken::Newline;
        case RCT12FormatCode::NewlineSmall:
            return FormatToken::NewlineSmall;
        case RCT12FormatCode::ColourBlack:
            return FormatToken::ColourBlack;
        case RCT12FormatCode::ColourGrey:
            return FormatToken::ColourGrey;
        case RCT12FormatCode::ColourWhite:
            return FormatToken::ColourWhite;
        case RCT12FormatCode::ColourRed:
            return FormatToken::ColourRed;
        case RCT12FormatCode::ColourGreen:
            return FormatToken::ColourGreen;
        case RCT12FormatCode::ColourYellow:
            return FormatToken::ColourYellow;
        case RCT12FormatCode::ColourTopaz:
            return FormatToken::ColourTopaz;
        case RCT12FormatCode::ColourCeladon:
            return FormatToken::ColourCeladon;
        case RCT12FormatCode::ColourBabyBlue:
            return FormatToken::ColourBabyBlue;
        case RCT12FormatCode::ColourPaleLavender:
            return FormatToken::ColourPaleLavender;
        case RCT12FormatCode::ColourPaleGold:
            return FormatToken::ColourPaleGold;
        case RCT12FormatCode::ColourLightPink:
            return FormatToken::ColourLightPink;
        case RCT12FormatCode::ColourPearlAqua:
            return FormatToken::ColourPearlAqua;
        case RCT12FormatCode::ColourPaleSilver:
            return FormatToken::ColourPaleSilver;
        default:
            return FormatToken::Unknown;
    }
}

static codepoint_t GetRCT12CodeFromFormatToken(FormatToken token)
{
    switch (token)
    {
        case FormatToken::Newline:
            return RCT12FormatCode::Newline;
        case FormatToken::NewlineSmall:
            return RCT12FormatCode::NewlineSmall;
        case FormatToken::ColourBlack:
            return RCT12FormatCode::ColourBlack;
        case FormatToken::ColourGrey:
            return RCT12FormatCode::ColourGrey;
        case FormatToken::ColourWhite:
            return RCT12FormatCode::ColourWhite;
        case FormatToken::ColourRed:
            return RCT12FormatCode::ColourRed;
        case FormatToken::ColourGreen:
            return RCT12FormatCode::ColourGreen;
        case FormatToken::ColourYellow:
            return RCT12FormatCode::ColourYellow;
        case FormatToken::ColourTopaz:
            return RCT12FormatCode::ColourTopaz;
        case FormatToken::ColourCeladon:
            return RCT12FormatCode::ColourCeladon;
        case FormatToken::ColourBabyBlue:
            return RCT12FormatCode::ColourBabyBlue;
        case FormatToken::ColourPaleLavender:
            return RCT12FormatCode::ColourPaleLavender;
        case FormatToken::ColourPaleGold:
            return RCT12FormatCode::ColourPaleGold;
        case FormatToken::ColourLightPink:
            return RCT12FormatCode::ColourLightPink;
        case FormatToken::ColourPearlAqua:
            return RCT12FormatCode::ColourPearlAqua;
        case FormatToken::ColourPaleSilver:
            return RCT12FormatCode::ColourPaleSilver;
        default:
            return 0;
    }
}

std::string ConvertFormattedStringToOpenRCT2(std::string_view buffer)
{
    auto nullTerminator = buffer.find('\0');
    if (nullTerminator != std::string::npos)
    {
        buffer = buffer.substr(0, nullTerminator);
    }
    auto asUtf8 = rct2_to_utf8(buffer, RCT2LanguageId::EnglishUK);

    std::string result;
    CodepointView codepoints(asUtf8);
    for (auto codepoint : codepoints)
    {
        auto token = GetFormatTokenFromRCT12Code(codepoint);
        if (token != FormatToken::Unknown)
        {
            result += GetFormatTokenStringWithBraces(token);
        }
        else
        {
            String::AppendCodepoint(result, codepoint);
        }
    }
    return result;
}

std::string ConvertFormattedStringToRCT2(std::string_view buffer, size_t maxLength)
{
    std::string result;
    FmtString fmt(buffer);
    for (const auto& token : fmt)
    {
        if (token.IsLiteral())
        {
            result += token.text;
        }
        else
        {
            auto codepoint = GetRCT12CodeFromFormatToken(token.kind);
            if (codepoint == 0)
            {
                result += token.text;
            }
            else
            {
                String::AppendCodepoint(result, codepoint);
            }
        }
    }
    return GetTruncatedRCT2String(result, maxLength);
}

std::string GetTruncatedRCT2String(std::string_view src, size_t maxLength)
{
    auto rct2encoded = utf8_to_rct2(src);
    if (rct2encoded.size() > maxLength - 1)
    {
        log_warning(
            "The user string '%s' is too long for the S6 file format and has therefore been truncated.",
            std::string(src).c_str());

        rct2encoded.resize(maxLength - 1);
        for (size_t i = 0; i < rct2encoded.size(); i++)
        {
            if (rct2encoded[i] == static_cast<char>(static_cast<uint8_t>(0xFF)))
            {
                if (i > maxLength - 4)
                {
                    // This codepoint was truncated, remove codepoint altogether
                    rct2encoded.resize(i);
                    break;
                }

                // Skip the next two bytes which represent the unicode character
                i += 2;
            }
        }
    }
    return rct2encoded;
}

track_type_t RCT12FlatTrackTypeToOpenRCT2(RCT12TrackType origTrackType)
{
    switch (origTrackType)
    {
        case TrackElemType::FlatTrack1x4A_Alias:
            return TrackElemType::FlatTrack1x4A;
        case TrackElemType::FlatTrack2x2_Alias:
            return TrackElemType::FlatTrack2x2;
        case TrackElemType::FlatTrack4x4_Alias:
            return TrackElemType::FlatTrack4x4;
        case TrackElemType::FlatTrack2x4_Alias:
            return TrackElemType::FlatTrack2x4;
        case TrackElemType::FlatTrack1x5_Alias:
            return TrackElemType::FlatTrack1x5;
        case TrackElemType::FlatTrack1x1A_Alias:
            return TrackElemType::FlatTrack1x1A;
        case TrackElemType::FlatTrack1x4B_Alias:
            return TrackElemType::FlatTrack1x4B;
        case TrackElemType::FlatTrack1x1B_Alias:
            return TrackElemType::FlatTrack1x1B;
        case TrackElemType::FlatTrack1x4C_Alias:
            return TrackElemType::FlatTrack1x4C;
        case TrackElemType::FlatTrack3x3_Alias:
            return TrackElemType::FlatTrack3x3;
    }

    return origTrackType;
}

RCT12TrackType OpenRCT2FlatTrackTypeToRCT12(track_type_t origTrackType)
{
    switch (origTrackType)
    {
        case TrackElemType::FlatTrack1x4A:
            return TrackElemType::FlatTrack1x4A_Alias;
        case TrackElemType::FlatTrack2x2:
            return TrackElemType::FlatTrack2x2_Alias;
        case TrackElemType::FlatTrack4x4:
            return TrackElemType::FlatTrack4x4_Alias;
        case TrackElemType::FlatTrack2x4:
            return TrackElemType::FlatTrack2x4_Alias;
        case TrackElemType::FlatTrack1x5:
            return TrackElemType::FlatTrack1x5_Alias;
        case TrackElemType::FlatTrack1x1A:
            return TrackElemType::FlatTrack1x1A_Alias;
        case TrackElemType::FlatTrack1x4B:
            return TrackElemType::FlatTrack1x4B_Alias;
        case TrackElemType::FlatTrack1x1B:
            return TrackElemType::FlatTrack1x1B_Alias;
        case TrackElemType::FlatTrack1x4C:
            return TrackElemType::FlatTrack1x4C_Alias;
        case TrackElemType::FlatTrack3x3:
            return TrackElemType::FlatTrack3x3_Alias;
    }

    return origTrackType;
}

static constexpr std::string_view _stationStyles[] = {
    "rct2.station.plain",          "rct2.station.wooden", "rct2.station.canvas_tent", "rct2.station.castle_grey",
    "rct2.station.castle_brown",   "rct2.station.jungle", "rct2.station.log",         "rct2.station.classical",
    "rct2.station.abstract",       "rct2.station.snow",   "rct2.station.pagoda",      "rct2.station.space",
    "openrct2.station.noentrance",
};

static constexpr std::string_view _musicStyles[] = {
    "rct2.music.dodgems",
    "rct2.music.fairground",
    "rct2.music.roman",
    "rct2.music.oriental",
    "rct2.music.martian",
    "rct2.music.jungle",
    "rct2.music.egyptian",
    "rct2.music.toyland",
    "", // CIRCUS
    "rct2.music.space",
    "rct2.music.horror",
    "rct2.music.techno",
    "rct2.music.gentle",
    "rct2.music.summer",
    "rct2.music.water",
    "rct2.music.wildwest",
    "rct2.music.jurassic",
    "rct2.music.rock1",
    "rct2.music.ragtime",
    "rct2.music.fantasy",
    "rct2.music.rock2",
    "rct2.music.ice",
    "rct2.music.snow",
    "rct2.music.custom1",
    "rct2.music.custom2",
    "rct2.music.medieval",
    "rct2.music.urban",
    "rct2.music.organ",
    "rct2.music.mechanical",
    "rct2.music.modern",
    "rct2.music.pirate",
    "rct2.music.rock3",
    "rct2.music.candy",
};

std::string_view GetStationIdentifierFromStyle(uint8_t style)
{
    if (style < std::size(_stationStyles))
    {
        return _stationStyles[style];
    }
    return {};
}

std::optional<uint8_t> GetStyleFromMusicIdentifier(std::string_view identifier)
{
    auto it = std::find(std::begin(_musicStyles), std::end(_musicStyles), identifier);
    if (it != std::end(_musicStyles))
    {
        return std::distance(std::begin(_musicStyles), it);
    }
    return std::nullopt;
}

void SetDefaultRCT2TerrainObjects(ObjectList& objectList)
{
    // Surfaces
    objectList.SetObject(ObjectType::TerrainSurface, 0, "rct2.terrain_surface.grass");
    objectList.SetObject(ObjectType::TerrainSurface, 1, "rct2.terrain_surface.sand");
    objectList.SetObject(ObjectType::TerrainSurface, 2, "rct2.terrain_surface.dirt");
    objectList.SetObject(ObjectType::TerrainSurface, 3, "rct2.terrain_surface.rock");
    objectList.SetObject(ObjectType::TerrainSurface, 4, "rct2.terrain_surface.martian");
    objectList.SetObject(ObjectType::TerrainSurface, 5, "rct2.terrain_surface.chequerboard");
    objectList.SetObject(ObjectType::TerrainSurface, 6, "rct2.terrain_surface.grass_clumps");
    objectList.SetObject(ObjectType::TerrainSurface, 7, "rct2.terrain_surface.ice");
    objectList.SetObject(ObjectType::TerrainSurface, 8, "rct2.terrain_surface.grid_red");
    objectList.SetObject(ObjectType::TerrainSurface, 9, "rct2.terrain_surface.grid_yellow");
    objectList.SetObject(ObjectType::TerrainSurface, 10, "rct2.terrain_surface.grid_purple");
    objectList.SetObject(ObjectType::TerrainSurface, 11, "rct2.terrain_surface.grid_green");
    objectList.SetObject(ObjectType::TerrainSurface, 12, "rct2.terrain_surface.sand_red");
    objectList.SetObject(ObjectType::TerrainSurface, 13, "rct2.terrain_surface.sand_brown");
    objectList.SetObject(ObjectType::TerrainSurface, 14, "rct1aa.terrain_surface.roof_red");
    objectList.SetObject(ObjectType::TerrainSurface, 15, "rct1ll.terrain_surface.roof_grey");
    objectList.SetObject(ObjectType::TerrainSurface, 16, "rct1ll.terrain_surface.rust");
    objectList.SetObject(ObjectType::TerrainSurface, 17, "rct1ll.terrain_surface.wood");

    // Edges
    objectList.SetObject(ObjectType::TerrainEdge, 0, "rct2.terrain_edge.rock");
    objectList.SetObject(ObjectType::TerrainEdge, 1, "rct2.terrain_edge.wood_red");
    objectList.SetObject(ObjectType::TerrainEdge, 2, "rct2.terrain_edge.wood_black");
    objectList.SetObject(ObjectType::TerrainEdge, 3, "rct2.terrain_edge.ice");
    objectList.SetObject(ObjectType::TerrainEdge, 4, "rct1.terrain_edge.brick");
    objectList.SetObject(ObjectType::TerrainEdge, 5, "rct1.terrain_edge.iron");
    objectList.SetObject(ObjectType::TerrainEdge, 6, "rct1aa.terrain_edge.grey");
    objectList.SetObject(ObjectType::TerrainEdge, 7, "rct1aa.terrain_edge.yellow");
    objectList.SetObject(ObjectType::TerrainEdge, 8, "rct1aa.terrain_edge.red");
    objectList.SetObject(ObjectType::TerrainEdge, 9, "rct1ll.terrain_edge.purple");
    objectList.SetObject(ObjectType::TerrainEdge, 10, "rct1ll.terrain_edge.green");
    objectList.SetObject(ObjectType::TerrainEdge, 11, "rct1ll.terrain_edge.stone_brown");
    objectList.SetObject(ObjectType::TerrainEdge, 12, "rct1ll.terrain_edge.stone_grey");
    objectList.SetObject(ObjectType::TerrainEdge, 13, "rct1ll.terrain_edge.skyscraper_a");
    objectList.SetObject(ObjectType::TerrainEdge, 14, "rct1ll.terrain_edge.skyscraper_b");
}

void RCT12AddDefaultObjects(ObjectList& objectList)
{
    // Stations
    for (size_t i = 0; i < std::size(_stationStyles); i++)
    {
        objectList.SetObject(ObjectType::Station, static_cast<ObjectEntryIndex>(i), _stationStyles[i]);
    }

    // Music
    for (size_t i = 0; i < std::size(_musicStyles); i++)
    {
        if (!_musicStyles[i].empty())
        {
            objectList.SetObject(ObjectType::Music, static_cast<ObjectEntryIndex>(i), _musicStyles[i]);
        }
    }
}

money64 RCT12CompletedCompanyValueToOpenRCT2(money32 origValue)
{
    if (origValue == RCT12_COMPANY_VALUE_ON_FAILED_OBJECTIVE)
        return COMPANY_VALUE_ON_FAILED_OBJECTIVE;

    return ToMoney64(origValue);
}

money32 OpenRCT2CompletedCompanyValueToRCT12(money64 origValue)
{
    if (origValue == COMPANY_VALUE_ON_FAILED_OBJECTIVE)
        return RCT12_COMPANY_VALUE_ON_FAILED_OBJECTIVE;

    return ToMoney32(origValue);
}
