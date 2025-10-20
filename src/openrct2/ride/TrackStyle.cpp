/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "TrackStyle.h"

#include "../core/EnumUtils.hpp"
#include "../ride/TrackData.h"
#include "Track.h"
#include "TrackPaint.h"

using TrackPaintFunctionGetter = TrackPaintFunction (*)(OpenRCT2::TrackElemType trackType);
static TrackPaintFunction DummyGetter(OpenRCT2::TrackElemType trackType)
{
    return TrackPaintFunctionDummy;
}

static constexpr TrackPaintFunctionGetter kPaintFunctionMap[] = {
    GetTrackPaintFunction3dCinema,                 // _3DCinema
    GetTrackPaintFunctionAirPoweredVerticalRC,     // airPoweredVerticalCoaster
    OpenRCT2::AlpineRC::GetTrackPaintFunction,     // alpineCoaster
    GetTrackPaintFunctionBoatHire,                 // boatHire
    GetTrackPaintFunctionBobsleighRC,              // bobsleighCoaster
    GetTrackPaintFunctionCarRide,                  // carRide
    GetTrackPaintFunctionChairlift,                // chairlift
    GetTrackPaintFunctionCircus,                   // circus
    GetTrackPaintFunctionClassicStandUpRC,         // classicStandUpRollerCoaster
    GetTrackPaintFunctionClassicWoodenRC,          // classicWoodenRollerCoaster
    GetTrackPaintFunctionClassicWoodenTwisterRC,   // classicWoodenTwisterRollerCoaster
    GetTrackPaintFunctionCompactInvertedRC,        // compactInvertedCoaster
    GetTrackPaintFunctionCorkscrewRC,              // corkscrewRollerCoaster
    GetTrackPaintFunctionCrookedHouse,             // crookedHouse
    GetTrackPaintFunctionDinghySlide,              // dinghySlide
    GetTrackPaintFunctionDinghySlideCovered,       // dinghySlideCovered
    GetTrackPaintFunctionDodgems,                  // dodgems
    GetTrackPaintFunctionEnterprise,               // enterprise
    GetTrackPaintFunctionFacility,                 // facility
    GetTrackPaintFunctionFerrisWheel,              // ferrisWheel
    GetTrackPaintFunctionFlyingRC,                 // flyingRollerCoaster
    GetTrackPaintFunctionFlyingRCInverted,         // flyingRollerCoasterInverted
    GetTrackPaintFunctionFlyingSaucers,            // flyingSaucers
    GetTrackPaintFunctionGhostTrain,               // ghostTrain
    GetTrackPaintFunctionGoKarts,                  // goKarts
    GetTrackPaintFunctionHauntedHouse,             // hauntedHouse
    GetTrackPaintFunctionHeartlineTwisterRC,       // heartlineTwisterCoaster
    OpenRCT2::HybridRC::GetTrackPaintFunction,     // hybridCoaster
    GetTrackPaintFunctionInvertedHairpinRC,        // invertedHairpinCoaster
    GetTrackPaintFunctionInvertedImpulseRC,        // invertedImpulseCoaster
    GetTrackPaintFunctionInvertedRC,               // invertedRollerCoaster
    GetTrackPaintFunctionJuniorRC,                 // juniorRollerCoaster
    GetTrackPaintFunctionLatticeTriangleTrack,     // latticeTriangle
    GetTrackPaintFunctionLatticeTriangleTrackAlt,  // latticeTriangleAlt
    GetTrackPaintFunctionLaunchedFreefall,         // launchedFreefall
    GetTrackPaintFunctionLayDownRCInverted,        // layDownRollerCoasterInverted
    GetTrackPaintFunctionLift,                     // lift
    GetTrackPaintFunctionLimLaunchedRC,            // limLaunchedRollerCoaster
    GetTrackPaintFunctionLogFlume,                 // logFlume
    GetTrackPaintFunctionLoopingRC,                // loopingRollerCoaster
    GetTrackPaintFunctionMagicCarpet,              // magicCarpet
    GetTrackPaintFunctionMaze,                     // maze
    GetTrackPaintFunctionMerryGoRound,             // merryGoRound
    GetTrackPaintFunctionMineRide,                 // mineRide
    GetTrackPaintFunctionMineTrainRC,              // mineTrainCoaster
    GetTrackPaintFunctionMiniGolf,                 // miniGolf
    GetTrackPaintFunctionMiniHelicopters,          // miniHelicopters
    GetTrackPaintFunctionMiniRC,                   // miniRollerCoaster
    GetTrackPaintFunctionMiniSuspendedRC,          // miniSuspendedCoaster
    GetTrackPaintFunctionMiniatureRailway,         // miniatureRailway
    GetTrackPaintFunctionMonorail,                 // monorail
    GetTrackPaintFunctionMonorailCycles,           // monorailCycles
    GetTrackPaintFunctionMotionsimulator,          // motionSimulator
    GetTrackPaintFunctionMultiDimensionRC,         // multiDimensionRollerCoaster
    GetTrackPaintFunctionMultiDimensionRC,         // multiDimensionRollerCoasterInverted
    GetTrackPaintFunctionObservationTower,         // observationTower
    GetTrackPaintFunctionReverseFreefallRC,        // reverseFreefallCoaster
    GetTrackPaintFunctionReverserRC,               // reverserRollerCoaster
    GetTrackPaintFunctionRiverRapids,              // riverRapids
    GetTrackPaintFunctionRotoDrop,                 // rotoDrop
    GetTrackPaintFunctionShop,                     // shop
    GetTrackPaintFunctionSideFrictionRC,           // sideFrictionRollerCoaster
    OpenRCT2::SingleRailRC::GetTrackPaintFunction, // singleRailRollerCoaster
    GetTrackPaintFunctionSpaceRings,               // spaceRings
    GetTrackPaintFunctionSpiralSlide,              // spiralSlide
    GetTrackPaintFunctionSplashBoats,              // splashBoats
    GetTrackPaintFunctionStandUpRC,                // standUpRollerCoaster
    GetTrackPaintFunctionWildMouse,                // steelWildMouse
    GetTrackPaintFunctionSteeplechase,             // steeplechase
    GetTrackPaintFunctionSubmarineRide,            // submarineRide
    GetTrackPaintFunctionSuspendedMonorail,        // suspendedMonorail
    GetTrackPaintFunctionSuspendedSwingingRC,      // suspendedSwingingCoaster
    GetTrackPaintFunctionSwingingInverterShip,     // swingingInverterShip
    GetTrackPaintFunctionSwingingShip,             // swingingShip
    GetTrackPaintFunctionTopspin,                  // topSpin
    GetTrackPaintFunctionTwist,                    // twist
    GetTrackPaintFunctionTwisterRC,                // twisterRollerCoaster
    GetTrackPaintFunctionVirginiaReel,             // virginiaReel
    GetTrackPaintFunctionWaterRC,                  // waterCoaster
    GetTrackPaintFunctionWoodenRC,                 // woodenRollerCoaster
    GetTrackPaintFunctionWoodenWildMouse,          // woodenWildMouse
    DummyGetter,                                   // null
};
static_assert(std::size(kPaintFunctionMap) == kTrackStyleCount);

static TrackPaintFunction GetTrackPaintFunction(TrackStyle trackStyle, OpenRCT2::TrackElemType trackType)
{
    return kPaintFunctionMap[static_cast<uint8_t>(trackStyle)](trackType);
}

static std::array<TrackStylePaintInfo, kTrackStyleCount> kTrackStylePaintInfos{};
static bool kTrackStylePaintInfosCreated = false;

static constexpr auto kTrackStyleClearances = std::to_array<int32_t>({
    0,  // _3DCinema
    32, // airPoweredVerticalCoaster
    24, // alpineCoaster
    16, // boatHire
    24, // bobsleighCoaster
    24, // carRide
    32, // chairlift
    0,  // circus
    24, // classicStandUpRollerCoaster
    24, // classicWoodenRollerCoaster
    24, // classicWoodenTwisterRollerCoaster
    40, // compactInvertedCoaster
    24, // corkscrewRollerCoaster
    0,  // crookedHouse
    24, // dinghySlide
    24, // dinghySlideCovered
    0,  // dodgems
    0,  // enterprise
    0,  // facility
    0,  // ferrisWheel
    24, // flyingRollerCoaster
    24, // flyingRollerCoasterInverted
    0,  // flyingSaucers
    24, // ghostTrain
    24, // goKarts
    0,  // hauntedHouse
    24, // heartlineTwisterCoaster
    24, // hybridCoaster
    24, // invertedHairpinCoaster
    40, // invertedImpulseCoaster
    40, // invertedRollerCoaster
    24, // juniorRollerCoaster
    24, // latticeTriangle
    24, // latticeTriangleAlt
    0,  // launchedFreefall
    24, // layDownRollerCoasterInverted
    0,  // lift
    24, // limLaunchedRollerCoaster
    24, // logFlume
    24, // loopingRollerCoaster
    0,  // magicCarpet
    24, // maze
    0,  // merryGoRound
    24, // mineRide
    24, // mineTrainCoaster
    32, // miniGolf
    24, // miniHelicopters
    24, // miniRollerCoaster
    24, // miniSuspendedCoaster
    32, // miniatureRailway
    32, // monorail
    24, // monorailCycles
    0,  // motionSimulator
    24, // multiDimensionRollerCoaster
    24, // multiDimensionRollerCoasterInverted
    0,  // observationTower
    32, // reverseFreefallCoaster
    24, // reverserRollerCoaster
    32, // riverRapids
    0,  // rotoDrop
    0,  // shop
    24, // sideFrictionRollerCoaster
    24, // singleRailRollerCoaster
    0,  // spaceRings
    0,  // spiralSlide
    24, // splashBoats
    24, // standUpRollerCoaster
    24, // steelWildMouse
    24, // steeplechase
    16, // submarineRide
    40, // suspendedMonorail
    40, // suspendedSwingingCoaster
    0,  // swingingInverterShip
    0,  // swingingShip
    0,  // topSpin
    0,  // twist
    24, // twisterRollerCoaster
    24, // virginiaReel
    24, // waterCoaster
    24, // woodenRollerCoaster
    24, // woodenWildMouse
    0,  // null
});
static_assert(std::size(kTrackStyleClearances) == kTrackStyleCount);

static bool GetShouldNotSetSupportHeight(
    const TrackStyle trackStyle, const OpenRCT2::TrackElemType trackElemType, const uint8_t trackSequence)
{
    using enum OpenRCT2::TrackElemType;

    // to prevent missing supports with the miniature railways special supports
    if (trackStyle == TrackStyle::miniatureRailway
        && (trackElemType == DiagFlat || trackElemType == DiagUp25 || trackElemType == DiagFlatToUp25
            || trackElemType == DiagUp25ToFlat || trackElemType == DiagDown25 || trackElemType == DiagFlatToDown25
            || trackElemType == DiagDown25ToFlat || trackElemType == LeftEighthToOrthogonal
            || trackElemType == RightEighthToOrthogonal)
        && trackSequence == 0)
    {
        return true;
    }

    return false;
}

static int32_t GetTrackStyleSupportHeightOffset(
    const TrackStyle trackStyle, const OpenRCT2::TrackElemType trackElemType, const uint8_t trackSequence)
{
    using enum OpenRCT2::TrackElemType;

    if (trackStyle == TrackStyle::compactInvertedCoaster)
    {
        // loops and half loops have some inconsistent tiles
        if (trackElemType == LeftVerticalLoop && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == LeftVerticalLoop && trackSequence == 7)
            return -kCoordsZPerTinyZ;
        if (trackElemType == RightVerticalLoop && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == RightVerticalLoop && trackSequence == 7)
            return -kCoordsZPerTinyZ;
        if (trackElemType == HalfLoopUp && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == HalfLoopDown && trackSequence == 1)
            return -kCoordsZPerTinyZ;

        // not raised for ride clearance
        if (trackElemType == LeftQuarterTurn1TileUp60 || trackElemType == RightQuarterTurn1TileUp60
            || trackElemType == LeftQuarterTurn1TileDown60 || trackElemType == RightQuarterTurn1TileDown60
            || trackElemType == Up90 || trackElemType == Down90)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::flyingRollerCoasterInverted)
    {
        // all inverted flying roller coaster sloped banked pieces and photo section are too high
        switch (trackElemType)
        {
            case LeftQuarterBankedHelixLargeUp:
            case RightQuarterBankedHelixLargeUp:
            case LeftQuarterBankedHelixLargeDown:
            case RightQuarterBankedHelixLargeDown:
            case Up25LeftBanked:
            case Up25RightBanked:
            case OnRidePhoto:
            case Down25LeftBanked:
            case Down25RightBanked:
            case LeftBankedQuarterTurn3TileUp25:
            case RightBankedQuarterTurn3TileUp25:
            case LeftBankedQuarterTurn3TileDown25:
            case RightBankedQuarterTurn3TileDown25:
            case LeftBankedQuarterTurn5TileUp25:
            case RightBankedQuarterTurn5TileUp25:
            case LeftBankedQuarterTurn5TileDown25:
            case RightBankedQuarterTurn5TileDown25:
            case Up25ToLeftBankedUp25:
            case Up25ToRightBankedUp25:
            case LeftBankedUp25ToUp25:
            case RightBankedUp25ToUp25:
            case Down25ToLeftBankedDown25:
            case Down25ToRightBankedDown25:
            case LeftBankedDown25ToDown25:
            case RightBankedDown25ToDown25:
            case LeftBankedFlatToLeftBankedUp25:
            case RightBankedFlatToRightBankedUp25:
            case LeftBankedUp25ToLeftBankedFlat:
            case RightBankedUp25ToRightBankedFlat:
            case LeftBankedFlatToLeftBankedDown25:
            case RightBankedFlatToRightBankedDown25:
            case LeftBankedDown25ToLeftBankedFlat:
            case RightBankedDown25ToRightBankedFlat:
            case FlatToLeftBankedUp25:
            case FlatToRightBankedUp25:
            case LeftBankedUp25ToFlat:
            case RightBankedUp25ToFlat:
            case FlatToLeftBankedDown25:
            case FlatToRightBankedDown25:
            case LeftBankedDown25ToFlat:
            case RightBankedDown25ToFlat:
                return kCoordsZPerTinyZ;
            default:
                return 0;
        }
    }

    if (trackStyle == TrackStyle::invertedHairpinCoaster)
    {
        // these are correct where all other tracks are not
        if (trackElemType == Up60ToFlat || trackElemType == FlatToDown60)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::invertedImpulseCoaster)
    {
        // these clip the track completely override or no override
        if (trackElemType == Up90 || trackElemType == Down90 || trackElemType == LeftQuarterTurn1TileUp90
            || trackElemType == RightQuarterTurn1TileUp90 || trackElemType == LeftQuarterTurn1TileDown90
            || trackElemType == RightQuarterTurn1TileDown90)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::invertedRollerCoaster)
    {
        // loops and half loops have some inconsistent tiles
        if (trackElemType == LeftVerticalLoop && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == LeftVerticalLoop && trackSequence == 7)
            return -kCoordsZPerTinyZ;
        if (trackElemType == RightVerticalLoop && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == RightVerticalLoop && trackSequence == 7)
            return -kCoordsZPerTinyZ;
        if (trackElemType == HalfLoopUp && trackSequence == 2)
            return -kCoordsZPerTinyZ;
        if (trackElemType == HalfLoopDown && trackSequence == 1)
            return -kCoordsZPerTinyZ;

        // not raised for ride clearance
        if (trackElemType == LeftQuarterTurn1TileUp60 || trackElemType == RightQuarterTurn1TileUp60
            || trackElemType == LeftQuarterTurn1TileDown60 || trackElemType == RightQuarterTurn1TileDown60)
        {
            return -kCoordsZPerTinyZ;
        }

        // large half loops copied from the uninverted coaster, not raised for ride clearance
        if (trackElemType == LeftLargeHalfLoopUp || trackElemType == RightLargeHalfLoopUp
            || trackElemType == LeftLargeHalfLoopDown || trackElemType == RightLargeHalfLoopDown)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::layDownRollerCoasterInverted)
    {
        // inverted lay down roller coaster helix and photo section are too high
        switch (trackElemType)
        {
            case LeftQuarterBankedHelixLargeUp:
            case RightQuarterBankedHelixLargeUp:
            case LeftQuarterBankedHelixLargeDown:
            case RightQuarterBankedHelixLargeDown:
            case OnRidePhoto:
                return kCoordsZPerTinyZ;
            default:
                return 0;
        }
    }

    if (trackStyle == TrackStyle::limLaunchedRollerCoaster || trackStyle == TrackStyle::loopingRollerCoaster)
    {
        // lim twists have fixed consistent heights because they were so completely broken previously
        if ((trackElemType == LeftTwistDownToUp || trackElemType == RightTwistDownToUp) && trackSequence == 2)
        {
            return -kCoordsZPerTinyZ;
        }
        if ((trackElemType == LeftTwistUpToDown || trackElemType == RightTwistUpToDown) && trackSequence == 0)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::miniSuspendedCoaster)
    {
        // mini suspended is all higher than it should be except station and small turns
        if (!(trackElemType == EndStation || trackElemType == BeginStation || trackElemType == MiddleStation
              || trackElemType == LeftQuarterTurn3Tiles || trackElemType == RightQuarterTurn3Tiles))
        {
            return kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::multiDimensionRollerCoaster)
    {
        // these are invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == MultiDimInvertedFlatToDown90QuarterLoop || trackElemType == MultiDimInvertedUp90ToFlatQuarterLoop)
        {
            return kCoordsZPerTinyZ;
        }
    }

    if (trackStyle == TrackStyle::multiDimensionRollerCoasterInverted)
    {
        // too high
        if (trackElemType == OnRidePhoto)
        {
            return kCoordsZPerTinyZ;
        }

        // these clip the track completely override or no override
        if (trackElemType == Up60ToUp90 || trackElemType == Down90ToDown60 || trackElemType == Up90ToUp60
            || trackElemType == Down60ToDown90)
        {
            return kCoordsZPerTinyZ;
        }

        // these are invalid inverted pieces but kept for backwards compatibility
        if (trackElemType == MultiDimUp90ToInvertedFlatQuarterLoop || trackElemType == MultiDimFlatToDown90QuarterLoop)
        {
            return -kCoordsZPerTinyZ;
        }
    }

    return 0;
}

static std::optional<int32_t> CalculateGeneralSupportHeight(
    const OpenRCT2::TrackMetaData::TrackElementDescriptor& trackElementDescriptor, const TrackStyle trackStyle,
    const OpenRCT2::TrackElemType trackElemType, const uint8_t trackSequence)
{
    const auto& sequenceDescriptor = trackElementDescriptor.sequences[trackSequence];

    if ((sequenceDescriptor.flags & TRACK_SEQUENCE_FLAG_DO_NOT_SET_GENERAL_SUPPORT_HEIGHT)
        || GetShouldNotSetSupportHeight(trackStyle, trackElemType, trackSequence))
    {
        return std::nullopt;
    }

    const int32_t trackStyleClearance = Numerics::ceil2(kTrackStyleClearances[EnumValue(trackStyle)], kCoordsZStep);
    const int32_t trackClearance = Numerics::ceil2(sequenceDescriptor.clearance.clearanceZ, kCoordsZStep);
    const int32_t supportClearance = Numerics::ceil2(sequenceDescriptor.generalSupportHeight.clearance, kCoordsZStep);

    const int32_t trackStyleOffset = GetTrackStyleSupportHeightOffset(trackStyle, trackElemType, trackSequence);
    const int32_t clearance = trackStyleClearance + trackClearance + supportClearance + trackStyleOffset;

    const bool startsAtHalfHeight = (trackElementDescriptor.flags & TRACK_ELEM_FLAG_STARTS_AT_HALF_HEIGHT) != 0;
    const int32_t heightOffset = startsAtHalfHeight * kCoordsZStep;

    const int32_t trackElementOffset = (1024 + sequenceDescriptor.clearance.z + clearance + heightOffset) % kCoordsZPerTinyZ;

    const int32_t minimumClearance = sequenceDescriptor.generalSupportHeight.minimumClearance;

    return std::optional(std::max(clearance + trackElementOffset, minimumClearance));
}

void CreateTrackStylePaintInfos()
{
    if (kTrackStylePaintInfosCreated == true)
    {
        return;
    }

    for (size_t trackStyleIndex = 0; trackStyleIndex < kTrackStyleCount; trackStyleIndex++)
    {
        auto& trackStylePaintInfo = kTrackStylePaintInfos[trackStyleIndex];
        const TrackStyle trackStyle = static_cast<TrackStyle>(trackStyleIndex);

        for (size_t trackElemTypeIndex = 0; trackElemTypeIndex < EnumValue(OpenRCT2::TrackElemType::Count);
             trackElemTypeIndex++)
        {
            const auto trackElemType = static_cast<OpenRCT2::TrackElemType>(trackElemTypeIndex);
            TrackPaintFunction paintFunction = GetTrackPaintFunction(trackStyle, trackElemType);

            if (paintFunction == TrackPaintFunctionDummy)
            {
                trackStylePaintInfo.trackElemTypePaintInfos.emplace_back(paintFunction);
                continue;
            }

            std::array<TrackSequencePaintInfo, OpenRCT2::TrackMetaData::kMaxSequencesPerPiece> sequenceInfo{};

            const auto& ted = OpenRCT2::TrackMetaData::GetTrackElementDescriptor(trackElemType);
            for (uint8_t trackSequenceIndex = 0; trackSequenceIndex < ted.numSequences; trackSequenceIndex++)
            {
                const auto generalSupportHeight = CalculateGeneralSupportHeight(
                    ted, trackStyle, trackElemType, trackSequenceIndex);

                sequenceInfo[trackSequenceIndex] = TrackSequencePaintInfo{ generalSupportHeight };
            }

            trackStylePaintInfo.trackElemTypePaintInfos.emplace_back(paintFunction, sequenceInfo);
        }
    }

    kTrackStylePaintInfosCreated = true;
}

const TrackStylePaintInfo& GetTrackStylePaintInfo(const TrackStyle trackStyle)
{
    return kTrackStylePaintInfos[EnumValue(trackStyle)];
}
