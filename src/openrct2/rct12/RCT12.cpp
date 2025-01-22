/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "RCT12.h"

#include "../core/CodepointView.hpp"
#include "../core/String.hpp"
#include "../localisation/Formatting.h"
#include "../object/ObjectList.h"
#include "../rct1/Tables.h"
#include "../rct12/CSStringConverter.h"
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../ride/TrackDesign.h"
#include "../scenario/Scenario.h"
#include "../world/Banner.h"
#include "../world/Footpath.h"
#include "../world/Wall.h"
#include "../world/tile_element/PathElement.h"
#include "../world/tile_element/Slope.h"
#include "../world/tile_element/SmallSceneryElement.h"
#include "../world/tile_element/SurfaceElement.h"
#include "../world/tile_element/WallElement.h"
#include "EntryList.h"

using namespace OpenRCT2;

RCT12TileElementType RCT12TileElementBase::GetType() const
{
    auto elem_type = static_cast<RCT12TileElementType>((this->Type & kTileElementTypeMask) >> 2);
    switch (elem_type)
    {
        case RCT12TileElementType::Surface:
        case RCT12TileElementType::Path:
        case RCT12TileElementType::Track:
        case RCT12TileElementType::SmallScenery:
        case RCT12TileElementType::Entrance:
        case RCT12TileElementType::Wall:
        case RCT12TileElementType::LargeScenery:
        case RCT12TileElementType::Banner:
        case RCT12TileElementType::Corrupt:
        case RCT12TileElementType::EightCarsCorrupt14:
        case RCT12TileElementType::EightCarsCorrupt15:
            return elem_type;
        default:
            // Most corrupt elements were set to 0x255, but not all. Fallback to corrupt element if encountered unknown type.
            return RCT12TileElementType::EightCarsCorrupt15;
    }
}

uint8_t RCT12TileElementBase::GetDirection() const
{
    return this->Type & kTileElementDirectionMask;
}

uint8_t RCT12TileElementBase::GetOccupiedQuadrants() const
{
    return Flags & kTileElementOccupiedQuadrantsMask;
}

bool RCT12TileElementBase::IsLastForTile() const
{
    return (this->Flags & RCT12_TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

bool RCT12TileElementBase::IsGhost() const
{
    return (this->Flags & RCT12_TILE_ELEMENT_FLAG_GHOST) != 0;
}

uint8_t RCT12SurfaceElement::GetSlope() const
{
    return (Slope & kTileSlopeMask);
}

uint32_t RCT12SurfaceElement::GetSurfaceStyle() const
{
    uint32_t retVal = (Terrain >> 5) & 7;
    retVal |= (Type & RCT12_SURFACE_ELEMENT_TYPE_SURFACE_MASK) << 3;
    return retVal;
}

uint32_t RCT12SurfaceElement::GetEdgeStyle() const
{
    uint32_t terrainEdge = (Slope >> 5) & 7;
    if (Type & 128)
        terrainEdge |= (1 << 3);
    return terrainEdge;
}

uint8_t RCT12SurfaceElement::GetGrassLength() const
{
    return GrassLength;
}

uint8_t RCT12SurfaceElement::GetOwnership() const
{
    return (Ownership & kTileElementSurfaceOwnershipMask);
}

uint32_t RCT12SurfaceElement::GetWaterHeight() const
{
    return (Terrain & RCT12_TILE_ELEMENT_SURFACE_WATER_HEIGHT_MASK) * kWaterHeightStep;
}

uint8_t RCT12SurfaceElement::GetParkFences() const
{
    return (Ownership & kTileElementSurfaceParkFenceMask);
}

bool RCT12SurfaceElement::HasTrackThatNeedsWater() const
{
    return (Type & SURFACE_ELEMENT_HAS_TRACK_THAT_NEEDS_WATER) != 0;
}

uint8_t RCT12PathElement::GetEntryIndex() const
{
    return (EntryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 4;
}

uint8_t RCT12PathElement::GetQueueBannerDirection() const
{
    return ((Type & FOOTPATH_ELEMENT_TYPE_DIRECTION_MASK) >> 6);
}

bool RCT12PathElement::IsSloped() const
{
    return (EntryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_IS_SLOPED) != 0;
}

uint8_t RCT12PathElement::GetSlopeDirection() const
{
    return EntryIndex & RCT12_FOOTPATH_PROPERTIES_SLOPE_DIRECTION_MASK;
}

uint8_t RCT12PathElement::GetRideIndex() const
{
    return RideIndex;
}

uint8_t RCT12PathElement::GetStationIndex() const
{
    return (Additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_STATION_INDEX_MASK) >> 4;
}

bool RCT12PathElement::IsWide() const
{
    return (Type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_WIDE) != 0;
}

bool RCT12PathElement::IsQueue() const
{
    return (Type & FOOTPATH_ELEMENT_TYPE_FLAG_IS_QUEUE) != 0;
}

bool RCT12PathElement::HasQueueBanner() const
{
    return (EntryIndex & RCT12_FOOTPATH_PROPERTIES_FLAG_HAS_QUEUE_BANNER) != 0;
}
uint8_t RCT12PathElement::GetEdges() const
{
    return Edges & FOOTPATH_PROPERTIES_EDGES_EDGES_MASK;
}

uint8_t RCT12PathElement::GetCorners() const
{
    return Edges >> 4;
}

uint8_t RCT12PathElement::GetAddition() const
{
    return Additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_TYPE_MASK;
}

bool RCT12PathElement::AdditionIsGhost() const
{
    return (Additions & RCT12_FOOTPATH_PROPERTIES_ADDITIONS_FLAG_GHOST) != 0;
}

uint8_t RCT12PathElement::GetAdditionStatus() const
{
    return AdditionStatus;
}

uint8_t RCT12PathElement::GetRCT1PathType() const
{
    uint8_t pathColour = Type & 3;
    uint8_t pathType2 = (EntryIndex & RCT12_FOOTPATH_PROPERTIES_TYPE_MASK) >> 2;

    pathType2 = pathType2 | pathColour;
    return pathType2;
}

uint8_t RCT12PathElement::GetRCT1SupportType() const
{
    return (Flags & 0b01100000) >> 5;
}

OpenRCT2::RCT12::TrackElemType RCT12TrackElement::GetTrackType() const
{
    return TrackType;
}

uint8_t RCT12TrackElement::GetSequenceIndex() const
{
    return Sequence & RCT12_TRACK_ELEMENT_SEQUENCE_SEQUENCE_MASK;
}

uint8_t RCT12TrackElement::GetRideIndex() const
{
    return RideIndex;
}

RideColourScheme RCT12TrackElement::GetColourScheme() const
{
    return static_cast<RideColourScheme>(Colour & 0x3);
}

uint8_t RCT12TrackElement::GetStationIndex() const
{
    if (TrackTypeIsStation(TrackType) || TrackType == OpenRCT2::RCT12::TrackElemType::TowerBase)
    {
        return (Sequence & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
    }
    return 0;
}

bool RCT12TrackElement::HasChain() const
{
    return Type & RCT12_TRACK_ELEMENT_TYPE_FLAG_CHAIN_LIFT;
}

bool RCT12TrackElement::HasCableLift() const
{
    return Colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_CABLE_LIFT;
}

bool RCT12TrackElement::IsInverted() const
{
    return Colour & RCT12_TRACK_ELEMENT_COLOUR_FLAG_INVERTED;
}

uint8_t RCT12TrackElement::GetBrakeBoosterSpeed() const
{
    if (TrackTypeHasSpeedSetting(TrackType))
    {
        return (Sequence >> 4) << 1;
    }
    return 0;
}

bool RCT12TrackElement::HasGreenLight() const
{
    if (TrackTypeIsStation(TrackType))
    {
        return (Sequence & MAP_ELEM_TRACK_SEQUENCE_GREEN_LIGHT) != 0;
    }
    return false;
}

uint8_t RCT12TrackElement::GetSeatRotation() const
{
    return Colour >> 4;
}

uint16_t RCT12TrackElement::GetMazeEntry() const
{
    return MazeEntry;
}

uint8_t RCT12TrackElement::GetPhotoTimeout() const
{
    if (TrackType == OpenRCT2::RCT12::TrackElemType::OnRidePhoto)
    {
        return Sequence >> 4;
    }
    return 0;
}

uint8_t RCT12TrackElement::GetDoorAState() const
{
    return (Colour & RCT12_TRACK_ELEMENT_DOOR_A_MASK) >> 2;
}

uint8_t RCT12TrackElement::GetDoorBState() const
{
    return (Colour & RCT12_TRACK_ELEMENT_DOOR_B_MASK) >> 5;
}

bool RCT12TrackElement::IsIndestructible() const
{
    return (Flags & RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) != 0;
}

uint8_t RCT12SmallSceneryElement::GetEntryIndex() const
{
    return this->EntryIndex;
}

uint8_t RCT12SmallSceneryElement::GetAge() const
{
    return this->Age;
}

uint8_t RCT12SmallSceneryElement::GetSceneryQuadrant() const
{
    return (this->Type & kTileElementQuadrantMask) >> 6;
}

colour_t RCT12SmallSceneryElement::GetPrimaryColour() const
{
    return Colour1 & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12SmallSceneryElement::GetSecondaryColour() const
{
    return Colour2 & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

bool RCT12SmallSceneryElement::NeedsSupports() const
{
    return Colour1 & RCT12_SMALL_SCENERY_ELEMENT_NEEDS_SUPPORTS_FLAG;
}

uint32_t RCT12LargeSceneryElement::GetEntryIndex() const
{
    return EntryIndex & RCT12_TILE_ELEMENT_LARGE_TYPE_MASK;
}

uint16_t RCT12LargeSceneryElement::GetSequenceIndex() const
{
    return (EntryIndex >> 10);
}
colour_t RCT12LargeSceneryElement::GetPrimaryColour() const
{
    return Colour[0] & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12LargeSceneryElement::GetSecondaryColour() const
{
    return Colour[1] & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

uint8_t RCT12LargeSceneryElement::GetBannerIndex() const
{
    return (Type & 0xC0) | (((Colour[0]) & ~RCT12_TILE_ELEMENT_COLOUR_MASK) >> 2)
        | (((Colour[1]) & ~RCT12_TILE_ELEMENT_COLOUR_MASK) >> 5);
}

uint8_t RCT12WallElement::GetEntryIndex() const
{
    return EntryIndex;
}

uint8_t RCT12WallElement::GetSlope() const
{
    return (Type & kTileElementQuadrantMask) >> 6;
}

colour_t RCT12WallElement::GetPrimaryColour() const
{
    return Colour1 & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

colour_t RCT12WallElement::GetSecondaryColour() const
{
    uint8_t secondaryColour = (Colour1 & ~RCT12_TILE_ELEMENT_COLOUR_MASK) >> 5;
    secondaryColour |= (Flags & 0x60) >> 2;
    return secondaryColour;
}

colour_t RCT12WallElement::GetTertiaryColour() const
{
    return Colour3 & RCT12_TILE_ELEMENT_COLOUR_MASK;
}

uint8_t RCT12WallElement::GetAnimationFrame() const
{
    return (Animation >> 3) & 0xF;
}

uint8_t RCT12WallElement::GetBannerIndex() const
{
    return BannerIndex;
}

bool RCT12WallElement::IsAcrossTrack() const
{
    return (Animation & WALL_ANIMATION_FLAG_ACROSS_TRACK) != 0;
}

bool RCT12WallElement::AnimationIsBackwards() const
{
    return (Animation & WALL_ANIMATION_FLAG_DIRECTION_BACKWARD) != 0;
}

int32_t RCT12WallElement::GetRCT1WallType(int32_t edge) const
{
    uint8_t var_05 = Colour3;
    uint16_t var_06 = Colour1 | (Animation << 8);

    int32_t typeA = (var_05 >> (edge * 2)) & 3;
    int32_t typeB = (var_06 >> (edge * 4)) & 0x0F;

    if (typeB != 0x0F)
    {
        int32_t index = typeA | (typeB << 2);

        auto slope = GetRCT1Slope();
        auto edgeSlope = GetWallSlopeFromEdgeSlope(slope, edge & 3);
        if (edgeSlope & (EDGE_SLOPE_UPWARDS | EDGE_SLOPE_DOWNWARDS))
            index = RCT1::MapSlopedWall(index);

        return index;
    }

    return -1;
}

colour_t RCT12WallElement::GetRCT1WallColour() const
{
    return ((Type & 0xC0) >> 3) | ((EntryIndex & 0xE0) >> 5);
}

uint8_t RCT12WallElement::GetRCT1Slope() const
{
    return EntryIndex & 0b00011111;
}

uint8_t RCT12EntranceElement::GetEntranceType() const
{
    return EntranceType;
}

uint8_t RCT12EntranceElement::GetRideIndex() const
{
    return RideIndex;
}

uint8_t RCT12EntranceElement::GetStationIndex() const
{
    return (Index & RCT12_TRACK_ELEMENT_SEQUENCE_STATION_INDEX_MASK) >> 4;
}

uint8_t RCT12EntranceElement::GetSequenceIndex() const
{
    return Index & 0xF;
}

uint8_t RCT12EntranceElement::GetPathType() const
{
    return PathType;
}

uint8_t RCT12BannerElement::GetIndex() const
{
    return Index;
}

uint8_t RCT12BannerElement::GetPosition() const
{
    return Position;
}

uint8_t RCT12BannerElement::GetAllowedEdges() const
{
    return AllowedEdges & 0b00001111;
}

bool IsUserStringID(StringId stringId)
{
    return stringId >= 0x8000 && stringId < 0x9000;
}

bool RCT12PathElement::IsBroken() const
{
    return (Flags & RCT12_TILE_ELEMENT_FLAG_BROKEN) != 0;
}

bool RCT12PathElement::IsBlockedByVehicle() const
{
    return (Flags & RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE) != 0;
}

bool RCT12TrackElement::BlockBrakeClosed() const
{
    return (Flags & RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) != 0;
}

bool RCT12ResearchItem::IsInventedEndMarker() const
{
    return RawValue == RCT12_RESEARCHED_ITEMS_SEPARATOR;
}

bool RCT12ResearchItem::IsUninventedEndMarker() const
{
    return RawValue == RCT12_RESEARCHED_ITEMS_END;
}

bool RCT12ResearchItem::IsRandomEndMarker() const
{
    return RawValue == RCT12_RESEARCHED_ITEMS_END_2;
}

ObjectEntryIndex RCTEntryIndexToOpenRCT2EntryIndex(const RCT12ObjectEntryIndex index)
{
    if (index == kRCT12ObjectEntryIndexNull)
        return kObjectEntryIndexNull;

    return index;
}

RideId RCT12RideIdToOpenRCT2RideId(const RCT12RideId rideId)
{
    if (rideId == RCT12_RIDE_ID_NULL)
        return RideId::GetNull();

    return RideId::FromUnderlying(rideId);
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
            String::appendCodepoint(result, codepoint);
        }
    }

    result.shrink_to_fit();
    return result;
}

namespace OpenRCT2::RCT12FormatCode
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
} // namespace OpenRCT2::RCT12FormatCode

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

std::string ConvertFormattedStringToOpenRCT2(std::string_view buffer)
{
    auto nullTerminator = buffer.find('\0');
    if (nullTerminator != std::string::npos)
    {
        buffer = buffer.substr(0, nullTerminator);
    }
    auto asUtf8 = RCT2StringToUTF8(buffer, RCT2LanguageId::EnglishUK);

    std::string result;
    CodepointView codepoints(asUtf8);
    for (auto codepoint : codepoints)
    {
        auto token = GetFormatTokenFromRCT12Code(codepoint);
        if (token != FormatToken::Unknown)
        {
            result += FormatTokenToStringWithBraces(token);
        }
        else
        {
            String::appendCodepoint(result, codepoint);
        }
    }
    return result;
}

OpenRCT2::TrackElemType RCT12FlatTrackTypeToOpenRCT2(OpenRCT2::RCT12::TrackElemType origTrackType)
{
    switch (origTrackType)
    {
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x4A_Alias:
            return TrackElemType::FlatTrack1x4A;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack2x2_Alias:
            return TrackElemType::FlatTrack2x2;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack4x4_Alias:
            return TrackElemType::FlatTrack4x4;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack2x4_Alias:
            return TrackElemType::FlatTrack2x4;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x5_Alias:
            return TrackElemType::FlatTrack1x5;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x1A_Alias:
            return TrackElemType::FlatTrack1x1A;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x4B_Alias:
            return TrackElemType::FlatTrack1x4B;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x1B_Alias:
            return TrackElemType::FlatTrack1x1B;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack1x4C_Alias:
            return TrackElemType::FlatTrack1x4C;
        case OpenRCT2::RCT12::TrackElemType::FlatTrack3x3_Alias:
            return TrackElemType::FlatTrack3x3;
        default:
            return static_cast<OpenRCT2::TrackElemType>(origTrackType);
    }
}

OpenRCT2::RCT12::TrackElemType OpenRCT2FlatTrackTypeToRCT12(OpenRCT2::TrackElemType origTrackType)
{
    switch (origTrackType)
    {
        case TrackElemType::FlatTrack1x4A:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x4A_Alias;
        case TrackElemType::FlatTrack2x2:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack2x2_Alias;
        case TrackElemType::FlatTrack4x4:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack4x4_Alias;
        case TrackElemType::FlatTrack2x4:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack2x4_Alias;
        case TrackElemType::FlatTrack1x5:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x5_Alias;
        case TrackElemType::FlatTrack1x1A:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x1A_Alias;
        case TrackElemType::FlatTrack1x4B:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x4B_Alias;
        case TrackElemType::FlatTrack1x1B:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x1B_Alias;
        case TrackElemType::FlatTrack1x4C:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack1x4C_Alias;
        case TrackElemType::FlatTrack3x3:
            return OpenRCT2::RCT12::TrackElemType::FlatTrack3x3_Alias;
        default:
            return static_cast<OpenRCT2::RCT12::TrackElemType>(origTrackType);
    }
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
    return _stationStyles[RCT12_STATION_STYLE_INVISIBLE];
}

uint8_t GetStationStyleFromIdentifier(u8string_view identifier)
{
    // Not supported in TD6, closest match.
    if (identifier == "openrct2.station.noplatformnoentrance")
        return RCT12_STATION_STYLE_INVISIBLE;

    for (uint8_t i = RCT12_STATION_STYLE_PLAIN; i < std::size(_stationStyles); i++)
    {
        if (_stationStyles[i] == identifier)
        {
            return i;
        }
    }

    return RCT12_STATION_STYLE_PLAIN;
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

void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const std::vector<std::string_view>& objectNames)
{
    for (const auto& objectName : objectNames)
    {
        auto descriptor = ObjectEntryDescriptor(objectName);
        descriptor.Type = objectType;
        objectList.Add(descriptor);
    }
}

void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const RCT12::EntryList& entryList)
{
    AppendRequiredObjects(objectList, objectType, entryList.GetEntries());
}

money64 RCT12CompletedCompanyValueToOpenRCT2(money32 origValue)
{
    if (origValue == kRCT12CompanyValueOnFailedObjective)
        return kCompanyValueOnFailedObjective;

    return ToMoney64(origValue);
}

ResearchItem RCT12ResearchItem::ToResearchItem() const
{
    auto newResearchItem = ResearchItem();
    if (IsInventedEndMarker() || IsUninventedEndMarker() || IsRandomEndMarker())
    {
        newResearchItem.rawValue = 0;
        newResearchItem.flags = 0;
        newResearchItem.category = ResearchCategory::Transport;
        newResearchItem.SetNull();
    }
    else
    {
        newResearchItem.type = Research::EntryType{ Type };
        newResearchItem.entryIndex = RCTEntryIndexToOpenRCT2EntryIndex(EntryIndex);
        newResearchItem.flags = Flags;
        newResearchItem.category = static_cast<ResearchCategory>(Category);
        if (newResearchItem.type == Research::EntryType::Ride)
        {
            auto* rideEntry = GetRideEntryByIndex(newResearchItem.entryIndex);
            newResearchItem.baseRideType = rideEntry != nullptr ? RCT2::RCT2RideTypeToOpenRCT2RideType(BaseRideType, *rideEntry)
                                                                : BaseRideType;
        }
        else
        {
            newResearchItem.baseRideType = 0;
        }
    }

    return newResearchItem;
}

void ConvertFromTD46Flags(TrackDesignTrackElement& target, uint8_t flags)
{
    target.brakeBoosterSpeed = kRCT2DefaultBlockBrakeSpeed;
    if (TrackTypeIsStation(target.type))
    {
        auto stationIndex = flags & EnumValue(TD46Flags::StationId);
        target.stationIndex = StationIndex::FromUnderlying(stationIndex);
    }
    else
    {
        auto speedOrSeatRotation = flags & EnumValue(TD46Flags::SpeedOrSeatRotation);
        if (TrackTypeHasSpeedSetting(target.type) && target.type != TrackElemType::BlockBrakes)
        {
            target.brakeBoosterSpeed = speedOrSeatRotation << 1;
        }
        else
        {
            target.seatRotation = speedOrSeatRotation;
        }
    }

    target.colourScheme = (flags & EnumValue(TD46Flags::ColourScheme)) >> 4;
    if (flags & EnumValue(TD46Flags::IsInverted))
        target.SetFlag(TrackDesignTrackElementFlag::isInverted);
    if (flags & EnumValue(TD46Flags::HasChain))
        target.SetFlag(TrackDesignTrackElementFlag::hasChain);
}

uint8_t ConvertToTD46Flags(const TrackDesignTrackElement& source)
{
    uint8_t trackFlags = 0;
    if (TrackTypeIsStation(source.type))
    {
        trackFlags = (source.stationIndex.ToUnderlying() & EnumValue(TD46Flags::StationId));
    }
    else if (TrackTypeHasSpeedSetting(source.type) && source.type != TrackElemType::BlockBrakes)
    {
        trackFlags = (source.brakeBoosterSpeed >> 1);
    }
    else
    {
        trackFlags = source.seatRotation;
    }

    trackFlags |= source.colourScheme << 4;

    if (source.HasFlag(TrackDesignTrackElementFlag::hasChain))
        trackFlags |= EnumValue(TD46Flags::HasChain);
    if (source.HasFlag(TrackDesignTrackElementFlag::isInverted))
        trackFlags |= EnumValue(TD46Flags::IsInverted);

    return trackFlags;
}

void ImportMazeElement(TrackDesign& td, const TD46MazeElement& td46MazeElement)
{
    if (td46MazeElement.IsEntrance() || td46MazeElement.IsExit())
    {
        TrackDesignEntranceElement element{};
        element.location = TileCoordsXYZD(td46MazeElement.x, td46MazeElement.y, 0, td46MazeElement.Direction);
        element.isExit = td46MazeElement.IsExit();
        td.entranceElements.push_back(element);
    }
    else
    {
        TrackDesignMazeElement mazeElement{};
        mazeElement.location.x = td46MazeElement.x;
        mazeElement.location.y = td46MazeElement.y;
        mazeElement.mazeEntry = td46MazeElement.MazeEntry;
        td.mazeElements.push_back(mazeElement);
    }
}

namespace OpenRCT2::RCT12
{
    size_t GetRCTStringBufferLen(const char* buffer, size_t maxBufferLen)
    {
        constexpr char MULTIBYTE = static_cast<char>(255);
        size_t len = 0;
        for (size_t i = 0; i < maxBufferLen; i++)
        {
            auto ch = buffer[i];
            if (ch == MULTIBYTE)
            {
                i += 2;

                // Check if reading two more bytes exceeds max buffer len
                if (i < maxBufferLen)
                {
                    len += 3;
                }
            }
            else if (ch == '\0')
            {
                break;
            }
            else
            {
                len++;
            }
        }
        return len;
    }

    bool TrackTypeHasSpeedSetting(TrackElemType trackType)
    {
        return trackType == TrackElemType::Booster || trackType == TrackElemType::Brakes;
    }

    bool TrackTypeIsStation(TrackElemType trackType)
    {
        switch (trackType)
        {
            case TrackElemType::EndStation:
            case TrackElemType::BeginStation:
            case TrackElemType::MiddleStation:
                return true;
            default:
                return false;
        }
    }
} // namespace OpenRCT2::RCT12
