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
#include "../rct2/RCT2.h"
#include "../ride/Ride.h"
#include "../ride/Track.h"
#include "../scenario/Scenario.h"
#include "../world/Banner.h"
#include "../world/Footpath.h"
#include "../world/LargeScenery.h"
#include "../world/SmallScenery.h"
#include "../world/Surface.h"
#include "../world/TileElement.h"
#include "../world/Wall.h"
#include "EntryList.h"

using namespace OpenRCT2;

RCT12TileElementType RCT12TileElementBase::GetType() const
{
    return static_cast<RCT12TileElementType>((this->type & TILE_ELEMENT_TYPE_MASK) >> 2);
}

uint8_t RCT12TileElementBase::GetDirection() const
{
    return this->type & TILE_ELEMENT_DIRECTION_MASK;
}

uint8_t RCT12TileElementBase::GetOccupiedQuadrants() const
{
    return flags & TILE_ELEMENT_OCCUPIED_QUADRANTS_MASK;
}

bool RCT12TileElementBase::IsLastForTile() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_LAST_TILE) != 0;
}

bool RCT12TileElementBase::IsGhost() const
{
    return (this->flags & RCT12_TILE_ELEMENT_FLAG_GHOST) != 0;
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
        uint8_t speed = (sequence >> 4) << 1;
        if (trackType == TrackElemType::BlockBrakes && speed == 0)
        {
            return 1 << 1;
        }
        return speed;
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

bool RCT12TrackElement::IsIndestructible() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_INDESTRUCTIBLE_TRACK_PIECE) != 0;
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

bool RCT12PathElement::IsBroken() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BROKEN) != 0;
}

bool RCT12PathElement::IsBlockedByVehicle() const
{
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCKED_BY_VEHICLE) != 0;
}

bool RCT12TrackElement::GetBrakeClosed() const
{
    if (trackType == TrackElemType::Brakes)
    {
        return true;
    }
    return (flags & RCT12_TILE_ELEMENT_FLAG_BLOCK_BRAKE_CLOSED) != 0;
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

static void AppendRequiredObjects(ObjectList& objectList, ObjectType objectType, const std::vector<std::string>& objectNames)
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
    if (origValue == RCT12_COMPANY_VALUE_ON_FAILED_OBJECTIVE)
        return COMPANY_VALUE_ON_FAILED_OBJECTIVE;

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
        newResearchItem.type = Research::EntryType{ type };
        newResearchItem.entryIndex = RCTEntryIndexToOpenRCT2EntryIndex(entryIndex);
        newResearchItem.flags = flags;
        newResearchItem.category = static_cast<ResearchCategory>(category);
        if (newResearchItem.type == Research::EntryType::Ride)
        {
            auto* rideEntry = get_ride_entry(newResearchItem.entryIndex);
            newResearchItem.baseRideType = rideEntry != nullptr ? RCT2::RCT2RideTypeToOpenRCT2RideType(baseRideType, rideEntry)
                                                                : baseRideType;
        }
        else
        {
            newResearchItem.baseRideType = 0;
        }
    }

    return newResearchItem;
}
