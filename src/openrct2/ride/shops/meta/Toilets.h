/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../ShopItem.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor ToiletsRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_SHOP),
    SET_FIELD(EnabledTrackPieces, {}),
    SET_FIELD(ExtraTrackPieces, {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::FlatTrack1x1A),
    SET_FIELD(TrackPaintFunction, get_track_paint_function_facility),
    SET_FIELD(Flags, RIDE_TYPE_FLAG_HAS_SINGLE_PIECE_STATION | RIDE_TYPE_FLAG_CANNOT_HAVE_GAPS | RIDE_TYPE_FLAG_NO_TEST_MODE | RIDE_TYPE_FLAG_NO_VEHICLES |
                     RIDE_TYPE_FLAG_HAS_NO_TRACK | RIDE_TYPE_FLAG_IS_SHOP_OR_FACILITY | RIDE_TYPE_FLAG_TRACK_NO_WALLS | RIDE_TYPE_FLAG_FLAT_RIDE |
                     RIDE_TYPE_FLAG_PEEP_SHOULD_GO_INSIDE_FACILITY | RIDE_TYPE_FLAG_IN_RIDE | RIDE_TYPE_FLAG_IS_TOILET | RIDE_TYPE_FLAG_LIST_VEHICLES_SEPARATELY),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ShopStall)),
    SET_FIELD(DefaultMode, RideMode::ShopStall),
    SET_FIELD(OperatingSettings, { 4, 4, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_TOILETS, STR_RIDE_DESCRIPTION_TOILETS }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Building, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_TOILETS)),
    SET_FIELD(AvailableBreakdowns, 0),
    SET_FIELD(Heights, { 12, 32, 0, 0, }),
    SET_FIELD(MaxMass, 255),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 5, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_toilets),
    SET_FIELD(RatingsMultipliers, { 0, 0, 0 }),
    SET_FIELD(UpkeepCosts, { 50, 1, 0, 0, 0, 0 }),
    SET_FIELD(BuildCosts, { 225.00_GBP, 0.00_GBP, 1, }),
    SET_FIELD(DefaultPrices, { 0, 0 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_GENTLE),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 5),
    SET_FIELD(ColourPresets, DEFAULT_FLAT_RIDE_COLOUR_PRESET),
    SET_FIELD(ColourPreview, { 0, 0 }),
    SET_FIELD(ColourKey, RideColourKey::Toilets),
    SET_FIELD(Name, "toilets"),
    SET_FIELD(UpdateRotating, UpdateRotatingDefault),
    SET_FIELD(LightFXAddLightsMagicVehicle, nullptr),
    SET_FIELD(StartRideMusic, OpenRCT2::RideAudio::DefaultStartRideMusicChannel),
    SET_FIELD(DesignCreateMode, TrackDesignCreateMode::Default),
    SET_FIELD(MusicUpdateFunction, DefaultMusicUpdate),
    SET_FIELD(Classification, RideClassification::KioskOrFacility),
};
// clang-format on
