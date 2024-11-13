/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../../../ride/Track.h"
#include "../../../ride/TrackPaint.h"
#include "../../../sprites.h"
#include "../../tile_element/Paint.Tunnel.h"
#include "../../tile_element/Segment.h"
#include "WoodenRollerCoaster.hpp"

using namespace OpenRCT2;

// static constexpr TunnelGroup kTunnelGroup = TunnelGroup::Square;

enum
{
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_0 = SPR_CSG_BEGIN + 65447,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_1 = SPR_CSG_BEGIN + 65448,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_2 = SPR_CSG_BEGIN + 65449,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_3 = SPR_CSG_BEGIN + 65450,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_0 = SPR_CSG_BEGIN + 65451,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_1 = SPR_CSG_BEGIN + 65452,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_2 = SPR_CSG_BEGIN + 65453,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_3 = SPR_CSG_BEGIN + 65454,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_0 = SPR_CSG_BEGIN + 65455,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_1 = SPR_CSG_BEGIN + 65456,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_2 = SPR_CSG_BEGIN + 65457,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_3 = SPR_CSG_BEGIN + 65458,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_0 = SPR_CSG_BEGIN + 65459,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_1 = SPR_CSG_BEGIN + 65460,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_2 = SPR_CSG_BEGIN + 65461,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_3 = SPR_CSG_BEGIN + 65462,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_0 = SPR_CSG_BEGIN + 65463,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_1 = SPR_CSG_BEGIN + 65464,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_2 = SPR_CSG_BEGIN + 65465,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_3 = SPR_CSG_BEGIN + 65466,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_0 = SPR_CSG_BEGIN + 65467,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_1 = SPR_CSG_BEGIN + 65468,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_2 = SPR_CSG_BEGIN + 65469,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_3 = SPR_CSG_BEGIN + 65470,

    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_0 = SPR_CSG_BEGIN + 65471,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_1 = SPR_CSG_BEGIN + 65472,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_2 = SPR_CSG_BEGIN + 65473,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_3 = SPR_CSG_BEGIN + 65474,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_1 = SPR_CSG_BEGIN + 65475,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_3 = SPR_CSG_BEGIN + 65476,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_2 = SPR_CSG_BEGIN + 65477,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_FRONT_1 = SPR_CSG_BEGIN + 65478,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_1 = SPR_CSG_BEGIN + 65482,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_2 = SPR_CSG_BEGIN + 65483,

    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_FRONT_1 = SPR_CSG_BEGIN + 65484,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_FRONT_2 = SPR_CSG_BEGIN + 65485,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_0 = SPR_CSG_BEGIN + 65486,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_0 = SPR_CSG_BEGIN + 66203,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_1 = SPR_CSG_BEGIN + 66204,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_2 = SPR_CSG_BEGIN + 66205,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_3 = SPR_CSG_BEGIN + 66206,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_0 = SPR_CSG_BEGIN + 66207,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_1 = SPR_CSG_BEGIN + 66208,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_2 = SPR_CSG_BEGIN + 66209,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_3 = SPR_CSG_BEGIN + 66210,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_0 = SPR_CSG_BEGIN + 66211,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_1 = SPR_CSG_BEGIN + 66212,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_2 = SPR_CSG_BEGIN + 66213,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_3 = SPR_CSG_BEGIN + 66214,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_0 = SPR_CSG_BEGIN + 66215,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_1 = SPR_CSG_BEGIN + 66216,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_2 = SPR_CSG_BEGIN + 66217,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_3 = SPR_CSG_BEGIN + 66218,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_0 = SPR_CSG_BEGIN + 66219,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_1 = SPR_CSG_BEGIN + 66220,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_2 = SPR_CSG_BEGIN + 66221,
    SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_3 = SPR_CSG_BEGIN + 66222,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_0 = SPR_CSG_BEGIN + 66223,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_1 = SPR_CSG_BEGIN + 66224,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_2 = SPR_CSG_BEGIN + 66225,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_3 = SPR_CSG_BEGIN + 66226,

    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_0 = SPR_CSG_BEGIN + 66227,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_1 = SPR_CSG_BEGIN + 66228,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_2 = SPR_CSG_BEGIN + 66229,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_3 = SPR_CSG_BEGIN + 66230,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_HANDRAIL_1 = SPR_CSG_BEGIN + 66231,
    SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_HANDRAIL_3 = SPR_CSG_BEGIN + 66232,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_HANDRAIL_2 = SPR_CSG_BEGIN + 66233,

    SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_FRONT_HANDRAIL_1 = SPR_CSG_BEGIN + 66234,

    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_HANDRAIL_1 = SPR_CSG_BEGIN + 66238,
    SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_HANDRAIL_2 = SPR_CSG_BEGIN + 66239,

    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_FRONT_HANDRAIL_1 = SPR_CSG_BEGIN + 66240,
    SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_FRONT_HANDRAIL_2 = SPR_CSG_BEGIN + 66241,

    SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_HANDRAIL_0 = SPR_CSG_BEGIN + 66242,
};

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kFlatToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_0,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_1,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_1,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_1,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_2,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_3,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_HANDRAIL_3,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_3,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_LEFT_BANK_FRONT_HANDRAIL_3,
    },
} };

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kFlatToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_0,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_0,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_0,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_1,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_2,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_2,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_2,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_FRONT_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_3,
        SPR_CLASSIC_WOODEN_RC_FLAT_TO_RIGHT_BANK_HANDRAIL_3,
    },
} };

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kLeftBankImages = { {
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_0, SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_0 },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_1, SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_1 },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_2, SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_2 },
    { SPR_CLASSIC_WOODEN_RC_LEFT_BANK_3, SPR_CLASSIC_WOODEN_RC_LEFT_BANK_HANDRAIL_3 },
} };

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kUp25ToLeftBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_0,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_1,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_1,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_FRONT_1,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_FRONT_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_2,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_3,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_LEFT_BANK_HANDRAIL_3,
    },
} };

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kUp25ToRightBankImages = { {
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_0,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_1,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_2,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_3,
        SPR_CLASSIC_WOODEN_RC_UP25_TO_RIGHT_BANK_HANDRAIL_3,
    },
} };

static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kLeftBankToUp25Images = { {
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_0,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_1,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_1,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_1,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_2,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_2,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_2,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_FRONT_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_3,
        SPR_CLASSIC_WOODEN_RC_LEFT_BANK_TO_UP25_HANDRAIL_3,
    },
} };
static constexpr std::array<StraightWoodenTrack, kNumOrthogonalDirections> kRightBankToUp25Images = { {
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_0,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_0,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_1,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_1,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_2,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_2,
    },
    {
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_3,
        SPR_CLASSIC_WOODEN_RC_RIGHT_BANK_TO_UP25_HANDRAIL_3,
    },
} };

static void ClassicWoodenTwisterRCTrackLeftBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<false, kFlatToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrackRightBankToFlat(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<false, kFlatToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrackRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackFlatToBank<false, kLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrackLeftBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<false, kUp25ToRightBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrackRightBankTo25DegDown(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrack25DegUpToBank<false, kUp25ToLeftBankImages>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrack25DegDownToLeftBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<false, kRightBankToUp25Images>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

static void ClassicWoodenTwisterRCTrack25DegDownToRightBank(
    PaintSession& session, const Ride& ride, uint8_t trackSequence, uint8_t direction, int32_t height,
    const TrackElement& trackElement, SupportType supportType)
{
    WoodenRCTrackBankTo25DegUp<false, kLeftBankToUp25Images>(
        session, ride, trackSequence, DirectionReverse(direction), height, trackElement, supportType);
}

// Stylistically, this coaster is _very_ similar to the regular Wooden Roller Coaster.
// The only difference is the degree of the banking.
// As such, all non-banked pieces are simply drawn as regular wooden roller coaster pieces.
TRACK_PAINT_FUNCTION GetTrackPaintFunctionClassicWoodenTwisterRC(OpenRCT2::TrackElemType trackType)
{
    if (!IsCsgLoaded())
    {
        return GetTrackPaintFunctionWoodenRC(trackType);
    }

    switch (trackType)
    {
        case TrackElemType::FlatToLeftBank:
            return WoodenRCTrackFlatToBank<false, kFlatToLeftBankImages>;
        case TrackElemType::FlatToRightBank:
            return WoodenRCTrackFlatToBank<false, kFlatToRightBankImages>;
        case TrackElemType::LeftBankToFlat:
            return ClassicWoodenTwisterRCTrackLeftBankToFlat;
        case TrackElemType::RightBankToFlat:
            return ClassicWoodenTwisterRCTrackRightBankToFlat;
        case TrackElemType::LeftBank:
            return WoodenRCTrackFlatToBank<false, kLeftBankImages>;
        case TrackElemType::RightBank:
            return ClassicWoodenTwisterRCTrackRightBank;
        case TrackElemType::Up25ToLeftBank:
            return WoodenRCTrack25DegUpToBank<false, kUp25ToLeftBankImages>;
        case TrackElemType::Up25ToRightBank:
            return WoodenRCTrack25DegUpToBank<false, kUp25ToRightBankImages>;
        case TrackElemType::LeftBankToDown25:
            return ClassicWoodenTwisterRCTrackLeftBankTo25DegDown;
        case TrackElemType::RightBankToDown25:
            return ClassicWoodenTwisterRCTrackRightBankTo25DegDown;

        case TrackElemType::LeftBankToUp25:
            return WoodenRCTrackBankTo25DegUp<false, kLeftBankToUp25Images>;
        case TrackElemType::RightBankToUp25:
            return WoodenRCTrackBankTo25DegUp<false, kRightBankToUp25Images>;
        case TrackElemType::Down25ToLeftBank:
            return ClassicWoodenTwisterRCTrack25DegDownToLeftBank;
        case TrackElemType::Down25ToRightBank:
            return ClassicWoodenTwisterRCTrack25DegDownToRightBank;

        default:
            return GetTrackPaintFunctionWoodenRC(trackType);
    }
}
