/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../entity/EntityRegistry.h"
#include "Vehicle.h"

constexpr int operator"" _MPH(unsigned long long x) noexcept
{
    return x * 29127;
}

// Acceleration to apply when rider is braking
constexpr int32_t minBrake = (1 << 16);

// Acceleration to apply when rider is braking hard (used when going much faster than the rider's preferred speed or very
// close to the rider in front)
constexpr int32_t maxBrake = (12 << 16);

// Velocity above which riders will attempt to maintain separation from the vehicle in front
constexpr int32_t minFollowVelocity = 4_MPH;

// Minimum separation distance that riders will allow (regardless of followDistance)
constexpr int32_t minFollowDistance = 32;

// These parameters determine when and how the rider will apply the brakes
struct RiderControlSettings
{
    int32_t maxSpeed;       // Preferred speed on straight track
    int32_t minSpeed;       // Preferred speed in the tightest turn
    int32_t brakeThreshold; // Tolerance for exceeding their preferred speed
    int32_t followDistance; // Preferred spacing between this rider and the vehicle in front of them
};

static RiderControlSettings riderTable[256] = {
    { 22_MPH, 11_MPH, 10_MPH, 10 }, { 23_MPH, 10_MPH, 5_MPH, 9 },   { 23_MPH, 8_MPH, 7_MPH, 11 },
    { 27_MPH, 17_MPH, 12_MPH, 6 },  { 25_MPH, 18_MPH, 15_MPH, 6 },  { 23_MPH, 14_MPH, 12_MPH, 9 },
    { 20_MPH, 12_MPH, 12_MPH, 5 },  { 23_MPH, 10_MPH, 5_MPH, 11 },  { 26_MPH, 9_MPH, 4_MPH, 11 },
    { 16_MPH, 11_MPH, 9_MPH, 7 },   { 28_MPH, 10_MPH, 9_MPH, 10 },  { 24_MPH, 18_MPH, 8_MPH, 4 },
    { 23_MPH, 15_MPH, 13_MPH, 5 },  { 18_MPH, 10_MPH, 10_MPH, 10 }, { 25_MPH, 15_MPH, 8_MPH, 9 },
    { 18_MPH, 12_MPH, 12_MPH, 7 },  { 24_MPH, 14_MPH, 7_MPH, 7 },   { 27_MPH, 19_MPH, 13_MPH, 7 },
    { 26_MPH, 19_MPH, 9_MPH, 5 },   { 22_MPH, 10_MPH, 6_MPH, 9 },   { 20_MPH, 12_MPH, 11_MPH, 10 },
    { 27_MPH, 16_MPH, 12_MPH, 7 },  { 22_MPH, 13_MPH, 10_MPH, 7 },  { 23_MPH, 11_MPH, 10_MPH, 11 },
    { 17_MPH, 10_MPH, 9_MPH, 8 },   { 19_MPH, 13_MPH, 10_MPH, 5 },  { 14_MPH, 8_MPH, 11_MPH, 10 },
    { 22_MPH, 15_MPH, 12_MPH, 8 },  { 15_MPH, 7_MPH, 8_MPH, 8 },    { 23_MPH, 17_MPH, 14_MPH, 4 },
    { 25_MPH, 14_MPH, 13_MPH, 7 },  { 22_MPH, 8_MPH, 6_MPH, 13 },   { 27_MPH, 11_MPH, 6_MPH, 11 },
    { 19_MPH, 10_MPH, 9_MPH, 11 },  { 24_MPH, 13_MPH, 11_MPH, 7 },  { 12_MPH, 7_MPH, 8_MPH, 7 },
    { 27_MPH, 11_MPH, 5_MPH, 9 },   { 15_MPH, 9_MPH, 7_MPH, 7 },    { 16_MPH, 10_MPH, 9_MPH, 8 },
    { 11_MPH, 7_MPH, 7_MPH, 9 },    { 25_MPH, 17_MPH, 9_MPH, 6 },   { 22_MPH, 10_MPH, 4_MPH, 13 },
    { 25_MPH, 13_MPH, 7_MPH, 10 },  { 28_MPH, 18_MPH, 10_MPH, 5 },  { 21_MPH, 11_MPH, 10_MPH, 10 },
    { 23_MPH, 11_MPH, 11_MPH, 7 },  { 21_MPH, 15_MPH, 8_MPH, 5 },   { 27_MPH, 14_MPH, 6_MPH, 9 },
    { 26_MPH, 17_MPH, 12_MPH, 6 },  { 17_MPH, 7_MPH, 6_MPH, 12 },   { 17_MPH, 9_MPH, 8_MPH, 9 },
    { 17_MPH, 10_MPH, 12_MPH, 6 },  { 20_MPH, 10_MPH, 9_MPH, 9 },   { 26_MPH, 14_MPH, 10_MPH, 7 },
    { 28_MPH, 14_MPH, 7_MPH, 12 },  { 24_MPH, 11_MPH, 7_MPH, 9 },   { 26_MPH, 12_MPH, 6_MPH, 8 },
    { 24_MPH, 12_MPH, 11_MPH, 9 },  { 24_MPH, 11_MPH, 5_MPH, 7 },   { 29_MPH, 21_MPH, 16_MPH, 5 },
    { 22_MPH, 13_MPH, 13_MPH, 5 },  { 27_MPH, 20_MPH, 12_MPH, 6 },  { 21_MPH, 14_MPH, 7_MPH, 9 },
    { 27_MPH, 18_MPH, 7_MPH, 6 },   { 19_MPH, 12_MPH, 12_MPH, 6 },  { 25_MPH, 18_MPH, 9_MPH, 7 },
    { 20_MPH, 10_MPH, 10_MPH, 10 }, { 19_MPH, 9_MPH, 7_MPH, 9 },    { 24_MPH, 14_MPH, 7_MPH, 7 },
    { 25_MPH, 12_MPH, 10_MPH, 10 }, { 24_MPH, 15_MPH, 8_MPH, 7 },   { 23_MPH, 11_MPH, 6_MPH, 8 },
    { 18_MPH, 11_MPH, 7_MPH, 9 },   { 23_MPH, 8_MPH, 5_MPH, 11 },   { 12_MPH, 8_MPH, 12_MPH, 8 },
    { 22_MPH, 14_MPH, 12_MPH, 9 },  { 25_MPH, 13_MPH, 14_MPH, 8 },  { 26_MPH, 12_MPH, 5_MPH, 12 },
    { 29_MPH, 15_MPH, 6_MPH, 10 },  { 20_MPH, 12_MPH, 8_MPH, 9 },   { 25_MPH, 13_MPH, 10_MPH, 8 },
    { 23_MPH, 12_MPH, 8_MPH, 9 },   { 20_MPH, 11_MPH, 5_MPH, 11 },  { 22_MPH, 11_MPH, 5_MPH, 11 },
    { 21_MPH, 9_MPH, 6_MPH, 9 },    { 28_MPH, 13_MPH, 8_MPH, 10 },  { 27_MPH, 14_MPH, 10_MPH, 11 },
    { 22_MPH, 16_MPH, 10_MPH, 5 },  { 25_MPH, 14_MPH, 9_MPH, 6 },   { 20_MPH, 10_MPH, 7_MPH, 10 },
    { 24_MPH, 15_MPH, 13_MPH, 7 },  { 22_MPH, 11_MPH, 4_MPH, 9 },   { 19_MPH, 9_MPH, 4_MPH, 12 },
    { 19_MPH, 11_MPH, 8_MPH, 7 },   { 24_MPH, 16_MPH, 15_MPH, 5 },  { 15_MPH, 11_MPH, 9_MPH, 7 },
    { 25_MPH, 10_MPH, 7_MPH, 10 },  { 23_MPH, 11_MPH, 5_MPH, 9 },   { 24_MPH, 13_MPH, 11_MPH, 8 },
    { 26_MPH, 16_MPH, 9_MPH, 8 },   { 25_MPH, 16_MPH, 10_MPH, 6 },  { 26_MPH, 16_MPH, 8_MPH, 6 },
    { 26_MPH, 16_MPH, 12_MPH, 7 },  { 25_MPH, 11_MPH, 10_MPH, 11 }, { 21_MPH, 13_MPH, 13_MPH, 6 },
    { 9_MPH, 6_MPH, 4_MPH, 11 },    { 26_MPH, 11_MPH, 5_MPH, 10 },  { 25_MPH, 18_MPH, 14_MPH, 5 },
    { 26_MPH, 16_MPH, 9_MPH, 4 },   { 24_MPH, 11_MPH, 11_MPH, 9 },  { 26_MPH, 16_MPH, 11_MPH, 8 },
    { 29_MPH, 12_MPH, 5_MPH, 10 },  { 26_MPH, 11_MPH, 4_MPH, 12 },  { 19_MPH, 9_MPH, 12_MPH, 8 },
    { 18_MPH, 8_MPH, 5_MPH, 12 },   { 28_MPH, 11_MPH, 4_MPH, 10 },  { 25_MPH, 16_MPH, 10_MPH, 8 },
    { 29_MPH, 17_MPH, 12_MPH, 6 },  { 18_MPH, 11_MPH, 10_MPH, 8 },  { 25_MPH, 10_MPH, 9_MPH, 10 },
    { 25_MPH, 12_MPH, 6_MPH, 12 },  { 23_MPH, 12_MPH, 8_MPH, 8 },   { 27_MPH, 9_MPH, 9_MPH, 11 },
    { 27_MPH, 19_MPH, 10_MPH, 3 },  { 28_MPH, 12_MPH, 10_MPH, 8 },  { 25_MPH, 14_MPH, 10_MPH, 7 },
    { 28_MPH, 17_MPH, 8_MPH, 7 },   { 24_MPH, 13_MPH, 4_MPH, 9 },   { 25_MPH, 12_MPH, 8_MPH, 9 },
    { 18_MPH, 9_MPH, 6_MPH, 13 },   { 19_MPH, 8_MPH, 7_MPH, 11 },   { 28_MPH, 12_MPH, 7_MPH, 10 },
    { 21_MPH, 10_MPH, 6_MPH, 9 },   { 26_MPH, 13_MPH, 8_MPH, 9 },   { 21_MPH, 12_MPH, 12_MPH, 6 },
    { 25_MPH, 15_MPH, 8_MPH, 5 },   { 17_MPH, 12_MPH, 13_MPH, 7 },  { 20_MPH, 9_MPH, 6_MPH, 12 },
    { 27_MPH, 18_MPH, 13_MPH, 5 },  { 22_MPH, 14_MPH, 14_MPH, 5 },  { 19_MPH, 12_MPH, 15_MPH, 5 },
    { 26_MPH, 13_MPH, 10_MPH, 10 }, { 23_MPH, 11_MPH, 8_MPH, 10 },  { 28_MPH, 17_MPH, 11_MPH, 8 },
    { 23_MPH, 9_MPH, 5_MPH, 10 },   { 20_MPH, 11_MPH, 10_MPH, 7 },  { 24_MPH, 9_MPH, 7_MPH, 12 },
    { 25_MPH, 13_MPH, 8_MPH, 8 },   { 25_MPH, 18_MPH, 14_MPH, 6 },  { 22_MPH, 16_MPH, 12_MPH, 7 },
    { 21_MPH, 12_MPH, 11_MPH, 8 },  { 22_MPH, 16_MPH, 10_MPH, 6 },  { 21_MPH, 15_MPH, 11_MPH, 6 },
    { 24_MPH, 16_MPH, 12_MPH, 7 },  { 28_MPH, 21_MPH, 9_MPH, 4 },   { 26_MPH, 14_MPH, 12_MPH, 8 },
    { 23_MPH, 13_MPH, 8_MPH, 7 },   { 26_MPH, 16_MPH, 14_MPH, 7 },  { 23_MPH, 14_MPH, 8_MPH, 7 },
    { 21_MPH, 13_MPH, 8_MPH, 7 },   { 26_MPH, 17_MPH, 7_MPH, 6 },   { 28_MPH, 10_MPH, 9_MPH, 10 },
    { 28_MPH, 11_MPH, 6_MPH, 12 },  { 21_MPH, 9_MPH, 9_MPH, 10 },   { 27_MPH, 12_MPH, 10_MPH, 9 },
    { 20_MPH, 10_MPH, 11_MPH, 8 },  { 26_MPH, 17_MPH, 15_MPH, 6 },  { 23_MPH, 9_MPH, 6_MPH, 11 },
    { 26_MPH, 17_MPH, 10_MPH, 8 },  { 22_MPH, 13_MPH, 12_MPH, 9 },  { 24_MPH, 12_MPH, 11_MPH, 8 },
    { 27_MPH, 17_MPH, 9_MPH, 7 },   { 27_MPH, 14_MPH, 7_MPH, 10 },  { 26_MPH, 9_MPH, 6_MPH, 9 },
    { 23_MPH, 10_MPH, 11_MPH, 9 },  { 30_MPH, 23_MPH, 10_MPH, 3 },  { 14_MPH, 7_MPH, 7_MPH, 10 },
    { 25_MPH, 12_MPH, 9_MPH, 10 },  { 20_MPH, 12_MPH, 8_MPH, 4 },   { 24_MPH, 16_MPH, 12_MPH, 7 },
    { 24_MPH, 13_MPH, 6_MPH, 10 },  { 24_MPH, 14_MPH, 9_MPH, 5 },   { 27_MPH, 14_MPH, 7_MPH, 8 },
    { 26_MPH, 12_MPH, 9_MPH, 9 },   { 23_MPH, 14_MPH, 8_MPH, 8 },   { 25_MPH, 16_MPH, 11_MPH, 6 },
    { 29_MPH, 17_MPH, 7_MPH, 9 },   { 25_MPH, 17_MPH, 15_MPH, 5 },  { 27_MPH, 16_MPH, 11_MPH, 4 },
    { 13_MPH, 8_MPH, 10_MPH, 10 },  { 27_MPH, 17_MPH, 8_MPH, 7 },   { 22_MPH, 13_MPH, 6_MPH, 7 },
    { 25_MPH, 18_MPH, 16_MPH, 4 },  { 23_MPH, 12_MPH, 6_MPH, 8 },   { 22_MPH, 16_MPH, 12_MPH, 5 },
    { 25_MPH, 14_MPH, 10_MPH, 6 },  { 19_MPH, 11_MPH, 10_MPH, 8 },  { 24_MPH, 10_MPH, 10_MPH, 8 },
    { 27_MPH, 17_MPH, 12_MPH, 7 },  { 27_MPH, 16_MPH, 12_MPH, 5 },  { 24_MPH, 16_MPH, 8_MPH, 7 },
    { 20_MPH, 9_MPH, 8_MPH, 9 },    { 27_MPH, 19_MPH, 15_MPH, 4 },  { 21_MPH, 10_MPH, 11_MPH, 8 },
    { 17_MPH, 8_MPH, 5_MPH, 11 },   { 16_MPH, 10_MPH, 10_MPH, 8 },  { 29_MPH, 18_MPH, 10_MPH, 7 },
    { 23_MPH, 16_MPH, 14_MPH, 5 },  { 16_MPH, 8_MPH, 6_MPH, 10 },   { 24_MPH, 10_MPH, 8_MPH, 11 },
    { 21_MPH, 11_MPH, 7_MPH, 11 },  { 26_MPH, 13_MPH, 8_MPH, 7 },   { 22_MPH, 10_MPH, 8_MPH, 11 },
    { 25_MPH, 13_MPH, 11_MPH, 9 },  { 24_MPH, 15_MPH, 8_MPH, 5 },   { 26_MPH, 12_MPH, 6_MPH, 8 },
    { 21_MPH, 15_MPH, 15_MPH, 7 },  { 28_MPH, 19_MPH, 14_MPH, 6 },  { 23_MPH, 14_MPH, 9_MPH, 8 },
    { 25_MPH, 13_MPH, 6_MPH, 9 },   { 18_MPH, 11_MPH, 12_MPH, 7 },  { 22_MPH, 13_MPH, 7_MPH, 6 },
    { 23_MPH, 13_MPH, 7_MPH, 8 },   { 27_MPH, 18_MPH, 9_MPH, 5 },   { 20_MPH, 9_MPH, 6_MPH, 10 },
    { 29_MPH, 9_MPH, 9_MPH, 12 },   { 26_MPH, 12_MPH, 9_MPH, 11 },  { 27_MPH, 10_MPH, 5_MPH, 11 },
    { 26_MPH, 20_MPH, 9_MPH, 3 },   { 18_MPH, 10_MPH, 11_MPH, 8 },  { 28_MPH, 16_MPH, 12_MPH, 8 },
    { 13_MPH, 9_MPH, 7_MPH, 9 },    { 24_MPH, 15_MPH, 9_MPH, 6 },   { 20_MPH, 11_MPH, 9_MPH, 8 },
    { 24_MPH, 15_MPH, 12_MPH, 4 },  { 24_MPH, 14_MPH, 9_MPH, 5 },   { 22_MPH, 11_MPH, 8_MPH, 10 },
    { 24_MPH, 11_MPH, 10_MPH, 10 }, { 24_MPH, 17_MPH, 10_MPH, 7 },  { 28_MPH, 18_MPH, 13_MPH, 7 },
    { 23_MPH, 11_MPH, 8_MPH, 12 },  { 25_MPH, 12_MPH, 11_MPH, 8 },  { 21_MPH, 10_MPH, 11_MPH, 8 },
    { 15_MPH, 8_MPH, 7_MPH, 10 },   { 26_MPH, 16_MPH, 9_MPH, 8 },   { 21_MPH, 10_MPH, 9_MPH, 10 },
    { 27_MPH, 17_MPH, 16_MPH, 6 },  { 22_MPH, 12_MPH, 6_MPH, 9 },   { 25_MPH, 9_MPH, 4_MPH, 11 },
    { 26_MPH, 16_MPH, 13_MPH, 9 },  { 26_MPH, 19_MPH, 11_MPH, 6 },  { 24_MPH, 15_MPH, 13_MPH, 7 },
    { 16_MPH, 9_MPH, 10_MPH, 8 },   { 21_MPH, 11_MPH, 7_MPH, 6 },   { 28_MPH, 20_MPH, 15_MPH, 6 },
    { 25_MPH, 15_MPH, 9_MPH, 6 }
};

int32_t Vehicle::CalculateRiderBraking() const
{
    if (num_peeps == 0)
        return 0;

    RiderControlSettings riderSettings = riderTable[peep[0].ToUnderlying() & 0xFF];

    // Brake if close to the vehicle in front
    Vehicle* prevVehicle = GetEntity<Vehicle>(prev_vehicle_on_ride);
    if (prevVehicle != nullptr && this != prevVehicle && _vehicleVelocityF64E08 > minFollowVelocity)
    {
        int32_t followDistance = std::max(minFollowDistance, (riderSettings.followDistance * _vehicleVelocityF64E08) >> 15);
        int32_t distance = std::max(abs(x - prevVehicle->x), abs(y - prevVehicle->y));
        int32_t relativeVelocity = velocity - prevVehicle->velocity;
        int32_t z_diff = abs(z - prevVehicle->z);

        if (distance < followDistance && z_diff < 16 && relativeVelocity > -2_MPH)
        {
            if (distance < followDistance / 2 || relativeVelocity > riderSettings.brakeThreshold)
            {
                return -maxBrake;
            }
            else
            {
                return -minBrake;
            }
        }
    }

    // Brake more on turns
    int32_t targetSpeed = riderSettings.maxSpeed;
    int32_t brakeThreshold = riderSettings.brakeThreshold;
    auto trackType = GetTrackType();
    switch (trackType)
    {
        case TrackElemType::kLeftQuarterTurn3Tiles:
        case TrackElemType::kRightQuarterTurn3Tiles:
            targetSpeed = riderSettings.minSpeed;
            brakeThreshold = riderSettings.brakeThreshold / 2;
            break;
        case TrackElemType::kLeftQuarterTurn3TilesUp25:
        case TrackElemType::kRightQuarterTurn3TilesUp25:
        case TrackElemType::kLeftQuarterTurn3TilesDown25:
        case TrackElemType::kRightQuarterTurn3TilesDown25:
        case TrackElemType::kLeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::kRightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::kLeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::kRightQuarterTurn3TilesDown25ToRightBank:
            targetSpeed = (riderSettings.maxSpeed + 3 * riderSettings.minSpeed) / 4;
            brakeThreshold = 3 * riderSettings.brakeThreshold / 4;
            break;
        case TrackElemType::kLeftHalfBankedHelixUpSmall:
        case TrackElemType::kRightHalfBankedHelixUpSmall:
        case TrackElemType::kLeftHalfBankedHelixDownSmall:
        case TrackElemType::kRightHalfBankedHelixDownSmall:
        case TrackElemType::kLeftBankedQuarterTurn3TileUp25:
        case TrackElemType::kRightBankedQuarterTurn3TileUp25:
        case TrackElemType::kLeftBankedQuarterTurn3TileDown25:
        case TrackElemType::kRightBankedQuarterTurn3TileDown25:
        case TrackElemType::kLeftBankedQuarterTurn3Tiles:
        case TrackElemType::kRightBankedQuarterTurn3Tiles:
        case TrackElemType::kLeftQuarterTurn5Tiles:
        case TrackElemType::kRightQuarterTurn5Tiles:
            targetSpeed = (riderSettings.maxSpeed + riderSettings.minSpeed) / 2;
            break;
        case TrackElemType::kLeftHalfBankedHelixUpLarge:
        case TrackElemType::kRightHalfBankedHelixUpLarge:
        case TrackElemType::kLeftHalfBankedHelixDownLarge:
        case TrackElemType::kRightHalfBankedHelixDownLarge:
        case TrackElemType::kLeftBankedQuarterTurn5TileUp25:
        case TrackElemType::kRightBankedQuarterTurn5TileUp25:
        case TrackElemType::kLeftBankedQuarterTurn5TileDown25:
        case TrackElemType::kRightBankedQuarterTurn5TileDown25:
        case TrackElemType::kBankedLeftQuarterTurn5Tiles:
        case TrackElemType::kBankedRightQuarterTurn5Tiles:
        case TrackElemType::kLeftQuarterTurn5TilesUp25:
        case TrackElemType::kRightQuarterTurn5TilesUp25:
        case TrackElemType::kLeftQuarterTurn5TilesDown25:
        case TrackElemType::kRightQuarterTurn5TilesDown25:
        case TrackElemType::kLeftEighthToDiag:
        case TrackElemType::kRightEighthToDiag:
        case TrackElemType::kLeftEighthToOrthogonal:
        case TrackElemType::kRightEighthToOrthogonal:
            targetSpeed = (3 * riderSettings.maxSpeed + riderSettings.minSpeed) / 4;
            break;
        default:
            break;
    }

    // Brake if car exceeds rider's preferred max speed
    if (_vehicleVelocityF64E08 > targetSpeed + brakeThreshold)
    {
        return -maxBrake;
    }
    else if (_vehicleVelocityF64E08 > targetSpeed)
    {
        return -minBrake;
    }

    return 0;
}
