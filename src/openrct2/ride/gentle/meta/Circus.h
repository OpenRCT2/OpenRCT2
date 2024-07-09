/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../../sprites.h"
#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr RideTypeDescriptor CircusRTD =
{
    .Category = RIDE_CATEGORY_GENTLE,
    .StartTrackPiece = TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .Drawer = GetTrackPaintFunctionCircus,
        .EnabledTrackPieces = {},
        .ExtraTrackPieces = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS |
                     RIDE_TYPE_FLAG_HAS_LOAD_OPTIONS | RIDE_TYPE_FLAG_VEHICLE_IS_INTEGRAL | RIDE_TYPE_FLAG_TRACK_NO_WALLS |
                     RIDE_TYPE_FLAG_FLAT_RIDE | RIDE_TYPE_FLAG_IN_RIDE | RIDE_TYPE_FLAG_HAS_VEHICLE_COLOURS |
                     RIDE_TYPE_FLAG_MUSIC_ON_DEFAULT | RIDE_TYPE_FLAG_HAS_ENTRANCE_EXIT | RIDE_TYPE_FLAG_SINGLE_SESSION |
                     RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY,
    .RideModes = EnumsToFlags(RideMode::Circus),
    .DefaultMode = RideMode::Circus,
    .Naming = { STR_RIDE_NAME_CIRCUS, STR_RIDE_DESCRIPTION_CIRCUS },
    .NameConvention = { RideComponentType::Building, RideComponentType::Structure, RideComponentType::Station },
    .EnumName = "RIDE_TYPE_CIRCUS",
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 12, 128, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 20, 10, 0 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 62.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 12, 0 },
    .DefaultMusic = {},
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 39,
    .ColourPresets = DEFAULT_FLAT_RIDE_COLOUR_PRESET,
    .ColourPreview = { 0, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "circus",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { RIDE_RATING(2, 10), RIDE_RATING(0, 30), RIDE_RATING(0, 0) },
        9,
        7,
        false,
        { { RatingsModifierType::NoModifier, 0, 0, 0, 0 } }
    },
    .UpdateRotating = UpdateRotatingDefault,
    .LightFXAddLightsMagicVehicle = nullptr,
    .StartRideMusic = OpenRCT2::RideAudio::CircusStartRideMusicChannel,
    .DesignCreateMode = TrackDesignCreateMode::Default,
    .MusicUpdateFunction = CircusMusicUpdate,
    .Classification = RideClassification::Ride,
    .UpdateLeaveEntrance = PeepUpdateRideLeaveEntranceDefault,
    .SpecialElementRatingAdjustment = SpecialTrackElementRatingsAjustment_Default,
    .GetGuestWaypointLocation = GetGuestWaypointLocationDefault,
    .ConstructionWindowContext = RideConstructionWindowContext::Default,
    .RideUpdate = nullptr,
    .UpdateMeasurementsSpecialElements = RideUpdateMeasurementsSpecialElements_Default,
    .MusicTrackOffsetLength = OpenRCT2::RideAudio::RideMusicGetTrackOffsetLength_Circus,
};
// clang-format on
