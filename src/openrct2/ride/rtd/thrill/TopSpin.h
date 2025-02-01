/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
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
constexpr RideTypeDescriptor TopSpinRTD =
{
    .Category = RIDE_CATEGORY_THRILL,
    .StartTrackPiece = OpenRCT2::TrackElemType::FlatTrack3x3,
    .TrackPaintFunctions = TrackDrawerDescriptor({
        .trackStyle = TrackStyle::topSpin,
        .enabledTrackGroups = {},
        .extraTrackGroups = {},
    }),
    .InvertedTrackPaintFunctions = {},
    .Flags = kRtdFlagsHasThreeColours | EnumsToFlags(RtdFlag::hasSinglePieceStation,
                     RtdFlag::cannotHaveGaps, RtdFlag::hasLoadOptions, RtdFlag::vehicleIsIntegral,
                     RtdFlag::noWallsAroundTrack, RtdFlag::isFlatRide, RtdFlag::hasVehicleColours,
                     RtdFlag::allowMusic, RtdFlag::hasEntranceAndExit, RtdFlag::singleSession,
                     RtdFlag::interestingToLookAt, RtdFlag::listVehiclesSeparately),
    .RideModes = EnumsToFlags(RideMode::Beginners, RideMode::Intense, RideMode::Berserk),
    .DefaultMode = RideMode::Beginners,
    .Naming = { STR_RIDE_NAME_TOP_SPIN, STR_RIDE_DESCRIPTION_TOP_SPIN },
    .NameConvention = { RideComponentType::Car, RideComponentType::Structure, RideComponentType::Station },
    .AvailableBreakdowns = (1 << BREAKDOWN_SAFETY_CUT_OUT),
    .Heights = { 16, 112, 3, 2, },
    .MaxMass = 255,
    .LiftData = { OpenRCT2::Audio::SoundId::Null, 5, 5 },
    .RatingsMultipliers = { 24, 20, 10 },
    .UpkeepCosts = { 50, 1, 0, 0, 0, 0 },
    .BuildCosts = { 72.50_GBP, 1.00_GBP, 1, },
    .DefaultPrices = { 20, 0 },
    .DefaultMusic = kMusicObjectTechno,
    .PhotoItem = ShopItem::Photo,
    .BonusValue = 55,
    .ColourPresets = TRACK_COLOUR_PRESETS(
        { COLOUR_BRIGHT_RED, COLOUR_YELLOW, COLOUR_YELLOW },
        { COLOUR_TEAL, COLOUR_WHITE, COLOUR_WHITE },
        { COLOUR_DARK_GREEN, COLOUR_BRIGHT_RED, COLOUR_BRIGHT_RED },
    ),
    .ColourPreview = { SPR_RIDE_DESIGN_PREVIEW_TOP_SPIN_TRACK, 0 },
    .ColourKey = RideColourKey::Ride,
    .Name = "top_spin",
    .RatingsData = 
    {
        RatingsCalculationType::FlatRide,
        { 1, 1, 1 },
        19,
        0,
        false,
        {
            { RatingsModifierType::BonusTopSpinMode, 0, 0, 0, 0 },
            { RatingsModifierType::BonusScenery,     0, 11155, 0, 0 },
        },
    },
};
// clang-format on
