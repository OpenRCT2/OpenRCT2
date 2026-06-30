/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/GameState.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/entity/Guest.h>
#include <openrct2/ride/Ride.h>
#include <openrct2/ride/RideRatings.h>

using namespace OpenRCT2;

namespace
{
    Guest MakeGuest(uint8_t energy, uint8_t hunger, uint8_t nausea, uint8_t happiness)
    {
        Guest guest{};
        guest.Energy = energy;
        guest.hunger = hunger;
        guest.nausea = nausea;
        guest.happiness = happiness;
        // Satisfy non-tolerance gates so only routed checks matter when asserted.
        guest.thirst = 255;
        guest.toilet = 0;
        return guest;
    }

    Ride MakeRatedRide(RideRating_t intensity)
    {
        Ride ride{};
        ride.type = RIDE_TYPE_LOG_FLUME;
        ride.ratings.excitement = RideRating::make(5, 00);
        ride.ratings.intensity = intensity;
        ride.ratings.nausea = RideRating::make(3, 00);
        return ride;
    }
} // namespace

// Hand-computed: tolerance = (Energy * hunger * 255) / (kPeepMaxEnergy * kPeepMaxHunger)
// with kPeepMaxEnergy = 128, kPeepMaxHunger = 255  =>  (Energy * hunger) / 128
//
// | Energy | hunger | tolerance |
// |     128 |    255 |       255 |  identity (healthy path)
// |     128 |    254 |       254 |
// |     128 |    128 |       128 |
// |     128 |      0 |         0 |
// |      64 |    255 |       127 |
// |      64 |    128 |        64 |
// |      32 |    255 |        63 |
// |     100 |    200 |       156 |
// |     127 |    255 |       253 |
// |       1 |    255 |         1 |
// |     128 |      1 |         1 |

TEST(GuestNeedsToleranceTests, ToleranceTable)
{
    static constexpr struct
    {
        uint8_t energy;
        uint8_t hunger;
        uint8_t expectedTolerance;
    } kTable[] = {
        { 128, 255, 255 },
        { 128, 254, 254 },
        { 128, 128, 128 },
        { 128, 0, 0 },
        { 64, 255, 127 },
        { 64, 128, 64 },
        { 32, 255, 63 },
        { 100, 200, 156 },
        { 127, 255, 253 },
        { 1, 255, 1 },
        { 128, 1, 1 },
    };

    for (const auto& row : kTable)
    {
        auto guest = MakeGuest(row.energy, row.hunger, 0, 255);
        EXPECT_EQ(GuestNeedsTolerance(guest), row.expectedTolerance)
            << "Energy=" << int(row.energy) << " hunger=" << int(row.hunger);
    }
}

// GuestReallyLikedRide: happiness must be >= 215. Nausea gate uses
//   nauseaThr = (120 * tolerance) / 255
// Intensity gate uses
//   intensityThr = (1000 * tolerance) / 255   // RideRating::make(10, 00)
// Hand-computed gate rows (no ScenarioRand on this path):
//
// | Energy | hunger | T | nausea | nauseaThr | intensity | intThr | happiness | liked? |
// |    128 |    255 | 255 |    120 |       120 |       1000 |  1000 |       255 | true   |  equal not >
// |    128 |    255 | 255 |    121 |       120 |          0 |  1000 |       255 | false  | nausea
// |    128 |    255 | 255 |      0 |       120 |       1001 |  1000 |       255 | false  | intensity
// |    128 |    255 | 255 |      0 |       120 |          0 |  1000 |       214 | false  | happiness
// |     64 |    128 |  64 |     30 |        30 |          0 |   250 |       255 | true   |
// |     64 |    128 |  64 |     31 |        30 |          0 |   250 |       255 | false  | nausea
// |     64 |    128 |  64 |      0 |        30 |        251 |   250 |       255 | false  | intensity
// |     64 |    128 |  64 |      0 |        30 |        250 |   250 |       255 | true   | equal not >
// |    128 |    128 | 128 |     60 |        60 |        501 |   501 |       255 | true   |
// |    128 |    128 | 128 |     61 |        60 |          0 |   501 |       255 | false  |
// |     32 |    255 |  63 |     29 |        29 |        247 |   247 |       255 | true   |
// |     32 |    255 |  63 |     30 |        29 |          0 |   247 |       255 | false  |
// |      1 |    255 |   1 |      0 |         0 |          3 |     3 |       255 | true   | nauseaThr 0, nausea not >
// |      1 |    255 |   1 |      1 |         0 |          0 |     3 |       255 | false  |
// |      1 |    255 |   1 |      0 |         0 |          4 |     3 |       255 | false  |

TEST(GuestNeedsToleranceTests, ReallyLikedRideGateTable)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = CreateContext();
    ASSERT_TRUE(context->Initialise());
    getGameState().cheats.ignoreRideIntensity = false;

    static constexpr struct
    {
        uint8_t energy;
        uint8_t hunger;
        uint8_t nausea;
        int16_t intensityWhole;
        uint8_t intensityFrac;
        uint8_t happiness;
        bool expectedLiked;
    } kTable[] = {
        { 128, 255, 120, 10, 0, 255, true },
        { 128, 255, 121, 0, 0, 255, false },
        { 128, 255, 0, 10, 1, 255, false },
        { 128, 255, 0, 0, 0, 214, false },
        { 64, 128, 30, 0, 0, 255, true },
        { 64, 128, 31, 0, 0, 255, false },
        { 64, 128, 0, 2, 51, 255, false },
        { 64, 128, 0, 2, 50, 255, true },
        { 128, 128, 60, 5, 1, 255, true },
        { 128, 128, 61, 0, 0, 255, false },
        { 32, 255, 29, 2, 47, 255, true },
        { 32, 255, 30, 0, 0, 255, false },
        { 1, 255, 0, 0, 3, 255, true },
        { 1, 255, 1, 0, 0, 255, false },
        { 1, 255, 0, 0, 4, 255, false },
    };

    for (const auto& row : kTable)
    {
        auto guest = MakeGuest(row.energy, row.hunger, row.nausea, row.happiness);
        auto ride = MakeRatedRide(RideRating::make(row.intensityWhole, row.intensityFrac));
        EXPECT_EQ(GuestReallyLikedRide(guest, ride), row.expectedLiked)
            << "Energy=" << int(row.energy) << " hunger=" << int(row.hunger) << " nausea=" << int(row.nausea)
            << " intensity=" << row.intensityWhole << "." << int(row.intensityFrac)
            << " happiness=" << int(row.happiness);
    }
}

// GuestMeetsGoOnRideAgainConditions (deterministic; no ScenarioRand):
// requires Energy >= 100, happiness >= 180, hunger >= 30, plus nausea/intensity via tolerance.
// nauseaThr = (160 * T) / 255, intensityThr = (1000 * T) / 255
//
// | Energy | hunger | T | nausea | nauseaThr | intensity | intThr | happiness | meets? |
// |    128 |    255 | 255 |    160 |       160 |       1000 |  1000 |       255 | true   |
// |    128 |    255 | 255 |    161 |       160 |          0 |  1000 |       255 | false  |
// |    128 |    255 | 255 |      0 |       160 |       1001 |  1000 |       255 | false  |
// |    128 |    255 | 255 |      0 |       160 |          0 |  1000 |       179 | false  |
// |    100 |    255 | 199 |     99 |       124 |          0 |   780 |       255 | true? wait
// Energy 100 hunger 255: T = (100*255)/128 = 25500/128 = 199
// nauseaThr = 160*199/255 = 31840/255 = 124
// intThr = 1000*199/255 = 199000/255 = 780
// |    100 |    255 | 199 |    124 |       124 |        780 |   780 |       255 | true   |
// |    100 |    255 | 199 |    125 |       124 |          0 |   780 |       255 | false  |
// |     99 |    255 | 197 |      0 |       123 |          0 |   772 |       255 | false  | Energy gate
// |    128 |    128 | 128 |     80 |        80 |        501 |   501 |       255 | true   |
// |    128 |    128 | 128 |     81 |        80 |          0 |   501 |       255 | false  |
// |    128 |     29 |  57 |      0 |        35 |          0 |   223 |       255 | false  | hunger gate (29 < 30)
// |    128 |     30 |  60 |     37 |        37 |        235 |   235 |       255 | true   | T=(128*30)/128=30; wait
// Energy 128 hunger 30: T = (128*30)/128 = 30
// nauseaThr = 160*30/255 = 4800/255 = 18
// intThr = 1000*30/255 = 30000/255 = 117
// Recalculate row for hunger 30:
// |    128 |     30 |  30 |     18 |        18 |        117 |   117 |       255 | true   |
// |    128 |     30 |  30 |     19 |        18 |          0 |   117 |       255 | false  |
// |     64 |    255 | 127 |     79 |        79 |        498 |   498 |       255 | true? Energy 64 < 100 false
// Energy gate fails for 64.

TEST(GuestNeedsToleranceTests, GoOnRideAgainGateTable)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;
    auto context = CreateContext();
    ASSERT_TRUE(context->Initialise());
    getGameState().cheats.ignoreRideIntensity = false;

    static constexpr struct
    {
        uint8_t energy;
        uint8_t hunger;
        uint8_t nausea;
        int16_t intensityWhole;
        uint8_t intensityFrac;
        uint8_t happiness;
        bool expectedMeets;
    } kTable[] = {
        { 128, 255, 160, 10, 0, 255, true },
        { 128, 255, 161, 0, 0, 255, false },
        { 128, 255, 0, 10, 1, 255, false },
        { 128, 255, 0, 0, 0, 179, false },
        { 100, 255, 124, 7, 80, 255, true },
        { 100, 255, 125, 0, 0, 255, false },
        { 99, 255, 0, 0, 0, 255, false },
        { 128, 128, 80, 5, 1, 255, true },
        { 128, 128, 81, 0, 0, 255, false },
        { 128, 29, 0, 0, 0, 255, false },
        { 128, 30, 18, 1, 17, 255, true },
        { 128, 30, 19, 0, 0, 255, false },
        { 64, 255, 0, 0, 0, 255, false },
        // Healthy identity: intensity exactly at legacy 10.00 still allowed
        { 128, 255, 0, 10, 0, 255, true },
        // Degraded: same intensity 10.00 fails once tolerance tightens (intThr=501 for T=128)
        { 128, 128, 0, 10, 0, 255, false },
    };

    for (const auto& row : kTable)
    {
        auto guest = MakeGuest(row.energy, row.hunger, row.nausea, row.happiness);
        auto ride = MakeRatedRide(RideRating::make(row.intensityWhole, row.intensityFrac));
        EXPECT_EQ(GuestMeetsGoOnRideAgainConditions(guest, ride), row.expectedMeets)
            << "Energy=" << int(row.energy) << " hunger=" << int(row.hunger) << " nausea=" << int(row.nausea)
            << " intensity=" << row.intensityWhole << "." << int(row.intensityFrac)
            << " happiness=" << int(row.happiness);
    }
}
