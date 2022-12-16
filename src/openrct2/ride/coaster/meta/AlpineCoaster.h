/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../../RideData.h"
#include "../../Track.h"

// clang-format off
constexpr const RideTypeDescriptor AlpineCoasterRTD =
{
    SET_FIELD(AlternateType, RIDE_TYPE_NULL),
    SET_FIELD(Category, RIDE_CATEGORY_ROLLERCOASTER),
    SET_FIELD(EnabledTrackPieces, { TRACK_FLAT, TRACK_STRAIGHT, TRACK_STATION_END, TRACK_LIFT_HILL, TRACK_FLAT_ROLL_BANKING, TRACK_SLOPE, TRACK_SLOPE_LONG, TRACK_SLOPE_CURVE, TRACK_S_BEND, TRACK_CURVE_SMALL, TRACK_CURVE, TRACK_HELIX_SMALL}),
    SET_FIELD(ExtraTrackPieces,  {}),
    SET_FIELD(CoveredTrackPieces, {}),
    SET_FIELD(StartTrackPiece, TrackElemType::EndStation),
    SET_FIELD(TrackPaintFunction, AlpineRC::GetTrackPaintFunction),
    SET_FIELD(Flags, {
                RideTypeFlags::HasTrackColourMain,
                RideTypeFlags::HasTrackColourSupports,
                RideTypeFlags::HasDataLogging,
                RideTypeFlags::HasDrops,
                RideTypeFlags::HasLoadOptions,
                RideTypeFlags::PeepWillRideAgain,
                RideTypeFlags::HasVehicleColours,
                RideTypeFlags::CheckForStalling,
                RideTypeFlags::HasTrack,
                RideTypeFlags::AllowMusic,
                RideTypeFlags::InterestingToLookAt,
                RideTypeFlags::CanSynchroniseAdjacentStations,
                RideTypeFlags::HasEntranceExit,
                RideTypeFlags::HasLargeCurves,
                RideTypeFlags::NoTestMode,
                RideTypeFlags::AllowMoreVehiclesThanStationFits,
                RideTypeFlags::UpInclineRequiresLift } ),
    SET_FIELD(RideModes, EnumsToFlags(RideMode::ContinuousCircuit)),
    SET_FIELD(DefaultMode, RideMode::ContinuousCircuit),
    SET_FIELD(OperatingSettings, { 0, 0, 0, 0, 0, 0 }),
    SET_FIELD(Naming, { STR_RIDE_NAME_ALPINE_COASTER, STR_RIDE_DESCRIPTION_ALPINE_COASTER }),
    SET_FIELD(NameConvention, { RideComponentType::Car, RideComponentType::Track, RideComponentType::Station }),
    SET_FIELD(EnumName, nameof(RIDE_TYPE_ALPINE_ROLLER_COASTER)),
    SET_FIELD(AvailableBreakdowns, (1 << BREAKDOWN_SAFETY_CUT_OUT) | (1 << BREAKDOWN_VEHICLE_MALFUNCTION)),
    SET_FIELD(Heights, {18, 24, 3, 7}),
    SET_FIELD(MaxMass, 4),
    SET_FIELD(LiftData, { OpenRCT2::Audio::SoundId::Null, 4, 5 }),
    SET_FIELD(RatingsCalculationFunction, ride_ratings_calculate_alpine_coaster),
    SET_FIELD(RatingsMultipliers, { 50, 30, 10 }),
    SET_FIELD(UpkeepCosts, { 35, 20, 80, 8, 3, 10 }),
    SET_FIELD(BuildCosts, { 22.50_GBP, 2.50_GBP, 50}),
    SET_FIELD(DefaultPrices, { 20, 20 }),
    SET_FIELD(DefaultMusic, MUSIC_OBJECT_GENTLE),
    SET_FIELD(PhotoItem, ShopItem::Photo),
    SET_FIELD(BonusValue, 65),
    SET_FIELD(ColourPresets, TRACK_COLOUR_PRESETS(
        { COLOUR_GREY, COLOUR_GREY, COLOUR_BLACK },
    )),
    SET_FIELD(ColourPreview, { SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_TRACK, SPR_RIDE_DESIGN_PREVIEW_ALPINE_COASTER_SUPPORTS }),
    SET_FIELD(ColourKey, RideColourKey::Ride),
    SET_FIELD(Name, "alpine_rc"),
};
// clang-format on
