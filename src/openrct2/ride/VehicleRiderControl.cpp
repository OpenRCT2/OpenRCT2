/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../entity/EntityRegistry.h"
#include "Vehicle.h"

#define MPH(x) (x * 29127)

// Acceleration to apply when rider is braking
constexpr int32_t minBrake = (1 << 16);

// Acceleration to apply when rider is braking hard (used when going much faster than the rider's preferred speed or very
// close to the rider in front)
constexpr int32_t maxBrake = (12 << 16);

// Velocity above which riders will attempt to maintain separation from the vehicle in front
constexpr int32_t minFollowVelocity = MPH(4);

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
    { MPH(22), MPH(11), MPH(10), 10 }, { MPH(23), MPH(10), MPH(5), 9 },   { MPH(23), MPH(8), MPH(7), 11 },
    { MPH(27), MPH(17), MPH(12), 6 },  { MPH(25), MPH(18), MPH(15), 6 },  { MPH(23), MPH(14), MPH(12), 9 },
    { MPH(20), MPH(12), MPH(12), 5 },  { MPH(23), MPH(10), MPH(5), 11 },  { MPH(26), MPH(9), MPH(4), 11 },
    { MPH(16), MPH(11), MPH(9), 7 },   { MPH(28), MPH(10), MPH(9), 10 },  { MPH(24), MPH(18), MPH(8), 4 },
    { MPH(23), MPH(15), MPH(13), 5 },  { MPH(18), MPH(10), MPH(10), 10 }, { MPH(25), MPH(15), MPH(8), 9 },
    { MPH(18), MPH(12), MPH(12), 7 },  { MPH(24), MPH(14), MPH(7), 7 },   { MPH(27), MPH(19), MPH(13), 7 },
    { MPH(26), MPH(19), MPH(9), 5 },   { MPH(22), MPH(10), MPH(6), 9 },   { MPH(20), MPH(12), MPH(11), 10 },
    { MPH(27), MPH(16), MPH(12), 7 },  { MPH(22), MPH(13), MPH(10), 7 },  { MPH(23), MPH(11), MPH(10), 11 },
    { MPH(17), MPH(10), MPH(9), 8 },   { MPH(19), MPH(13), MPH(10), 5 },  { MPH(14), MPH(8), MPH(11), 10 },
    { MPH(22), MPH(15), MPH(12), 8 },  { MPH(15), MPH(7), MPH(8), 8 },    { MPH(23), MPH(17), MPH(14), 4 },
    { MPH(25), MPH(14), MPH(13), 7 },  { MPH(22), MPH(8), MPH(6), 13 },   { MPH(27), MPH(11), MPH(6), 11 },
    { MPH(19), MPH(10), MPH(9), 11 },  { MPH(24), MPH(13), MPH(11), 7 },  { MPH(12), MPH(7), MPH(8), 7 },
    { MPH(27), MPH(11), MPH(5), 9 },   { MPH(15), MPH(9), MPH(7), 7 },    { MPH(16), MPH(10), MPH(9), 8 },
    { MPH(11), MPH(7), MPH(7), 9 },    { MPH(25), MPH(17), MPH(9), 6 },   { MPH(22), MPH(10), MPH(4), 13 },
    { MPH(25), MPH(13), MPH(7), 10 },  { MPH(28), MPH(18), MPH(10), 5 },  { MPH(21), MPH(11), MPH(10), 10 },
    { MPH(23), MPH(11), MPH(11), 7 },  { MPH(21), MPH(15), MPH(8), 5 },   { MPH(27), MPH(14), MPH(6), 9 },
    { MPH(26), MPH(17), MPH(12), 6 },  { MPH(17), MPH(7), MPH(6), 12 },   { MPH(17), MPH(9), MPH(8), 9 },
    { MPH(17), MPH(10), MPH(12), 6 },  { MPH(20), MPH(10), MPH(9), 9 },   { MPH(26), MPH(14), MPH(10), 7 },
    { MPH(28), MPH(14), MPH(7), 12 },  { MPH(24), MPH(11), MPH(7), 9 },   { MPH(26), MPH(12), MPH(6), 8 },
    { MPH(24), MPH(12), MPH(11), 9 },  { MPH(24), MPH(11), MPH(5), 7 },   { MPH(29), MPH(21), MPH(16), 5 },
    { MPH(22), MPH(13), MPH(13), 5 },  { MPH(27), MPH(20), MPH(12), 6 },  { MPH(21), MPH(14), MPH(7), 9 },
    { MPH(27), MPH(18), MPH(7), 6 },   { MPH(19), MPH(12), MPH(12), 6 },  { MPH(25), MPH(18), MPH(9), 7 },
    { MPH(20), MPH(10), MPH(10), 10 }, { MPH(19), MPH(9), MPH(7), 9 },    { MPH(24), MPH(14), MPH(7), 7 },
    { MPH(25), MPH(12), MPH(10), 10 }, { MPH(24), MPH(15), MPH(8), 7 },   { MPH(23), MPH(11), MPH(6), 8 },
    { MPH(18), MPH(11), MPH(7), 9 },   { MPH(23), MPH(8), MPH(5), 11 },   { MPH(12), MPH(8), MPH(12), 8 },
    { MPH(22), MPH(14), MPH(12), 9 },  { MPH(25), MPH(13), MPH(14), 8 },  { MPH(26), MPH(12), MPH(5), 12 },
    { MPH(29), MPH(15), MPH(6), 10 },  { MPH(20), MPH(12), MPH(8), 9 },   { MPH(25), MPH(13), MPH(10), 8 },
    { MPH(23), MPH(12), MPH(8), 9 },   { MPH(20), MPH(11), MPH(5), 11 },  { MPH(22), MPH(11), MPH(5), 11 },
    { MPH(21), MPH(9), MPH(6), 9 },    { MPH(28), MPH(13), MPH(8), 10 },  { MPH(27), MPH(14), MPH(10), 11 },
    { MPH(22), MPH(16), MPH(10), 5 },  { MPH(25), MPH(14), MPH(9), 6 },   { MPH(20), MPH(10), MPH(7), 10 },
    { MPH(24), MPH(15), MPH(13), 7 },  { MPH(22), MPH(11), MPH(4), 9 },   { MPH(19), MPH(9), MPH(4), 12 },
    { MPH(19), MPH(11), MPH(8), 7 },   { MPH(24), MPH(16), MPH(15), 5 },  { MPH(15), MPH(11), MPH(9), 7 },
    { MPH(25), MPH(10), MPH(7), 10 },  { MPH(23), MPH(11), MPH(5), 9 },   { MPH(24), MPH(13), MPH(11), 8 },
    { MPH(26), MPH(16), MPH(9), 8 },   { MPH(25), MPH(16), MPH(10), 6 },  { MPH(26), MPH(16), MPH(8), 6 },
    { MPH(26), MPH(16), MPH(12), 7 },  { MPH(25), MPH(11), MPH(10), 11 }, { MPH(21), MPH(13), MPH(13), 6 },
    { MPH(9), MPH(6), MPH(4), 11 },    { MPH(26), MPH(11), MPH(5), 10 },  { MPH(25), MPH(18), MPH(14), 5 },
    { MPH(26), MPH(16), MPH(9), 4 },   { MPH(24), MPH(11), MPH(11), 9 },  { MPH(26), MPH(16), MPH(11), 8 },
    { MPH(29), MPH(12), MPH(5), 10 },  { MPH(26), MPH(11), MPH(4), 12 },  { MPH(19), MPH(9), MPH(12), 8 },
    { MPH(18), MPH(8), MPH(5), 12 },   { MPH(28), MPH(11), MPH(4), 10 },  { MPH(25), MPH(16), MPH(10), 8 },
    { MPH(29), MPH(17), MPH(12), 6 },  { MPH(18), MPH(11), MPH(10), 8 },  { MPH(25), MPH(10), MPH(9), 10 },
    { MPH(25), MPH(12), MPH(6), 12 },  { MPH(23), MPH(12), MPH(8), 8 },   { MPH(27), MPH(9), MPH(9), 11 },
    { MPH(27), MPH(19), MPH(10), 3 },  { MPH(28), MPH(12), MPH(10), 8 },  { MPH(25), MPH(14), MPH(10), 7 },
    { MPH(28), MPH(17), MPH(8), 7 },   { MPH(24), MPH(13), MPH(4), 9 },   { MPH(25), MPH(12), MPH(8), 9 },
    { MPH(18), MPH(9), MPH(6), 13 },   { MPH(19), MPH(8), MPH(7), 11 },   { MPH(28), MPH(12), MPH(7), 10 },
    { MPH(21), MPH(10), MPH(6), 9 },   { MPH(26), MPH(13), MPH(8), 9 },   { MPH(21), MPH(12), MPH(12), 6 },
    { MPH(25), MPH(15), MPH(8), 5 },   { MPH(17), MPH(12), MPH(13), 7 },  { MPH(20), MPH(9), MPH(6), 12 },
    { MPH(27), MPH(18), MPH(13), 5 },  { MPH(22), MPH(14), MPH(14), 5 },  { MPH(19), MPH(12), MPH(15), 5 },
    { MPH(26), MPH(13), MPH(10), 10 }, { MPH(23), MPH(11), MPH(8), 10 },  { MPH(28), MPH(17), MPH(11), 8 },
    { MPH(23), MPH(9), MPH(5), 10 },   { MPH(20), MPH(11), MPH(10), 7 },  { MPH(24), MPH(9), MPH(7), 12 },
    { MPH(25), MPH(13), MPH(8), 8 },   { MPH(25), MPH(18), MPH(14), 6 },  { MPH(22), MPH(16), MPH(12), 7 },
    { MPH(21), MPH(12), MPH(11), 8 },  { MPH(22), MPH(16), MPH(10), 6 },  { MPH(21), MPH(15), MPH(11), 6 },
    { MPH(24), MPH(16), MPH(12), 7 },  { MPH(28), MPH(21), MPH(9), 4 },   { MPH(26), MPH(14), MPH(12), 8 },
    { MPH(23), MPH(13), MPH(8), 7 },   { MPH(26), MPH(16), MPH(14), 7 },  { MPH(23), MPH(14), MPH(8), 7 },
    { MPH(21), MPH(13), MPH(8), 7 },   { MPH(26), MPH(17), MPH(7), 6 },   { MPH(28), MPH(10), MPH(9), 10 },
    { MPH(28), MPH(11), MPH(6), 12 },  { MPH(21), MPH(9), MPH(9), 10 },   { MPH(27), MPH(12), MPH(10), 9 },
    { MPH(20), MPH(10), MPH(11), 8 },  { MPH(26), MPH(17), MPH(15), 6 },  { MPH(23), MPH(9), MPH(6), 11 },
    { MPH(26), MPH(17), MPH(10), 8 },  { MPH(22), MPH(13), MPH(12), 9 },  { MPH(24), MPH(12), MPH(11), 8 },
    { MPH(27), MPH(17), MPH(9), 7 },   { MPH(27), MPH(14), MPH(7), 10 },  { MPH(26), MPH(9), MPH(6), 9 },
    { MPH(23), MPH(10), MPH(11), 9 },  { MPH(30), MPH(23), MPH(10), 3 },  { MPH(14), MPH(7), MPH(7), 10 },
    { MPH(25), MPH(12), MPH(9), 10 },  { MPH(20), MPH(12), MPH(8), 4 },   { MPH(24), MPH(16), MPH(12), 7 },
    { MPH(24), MPH(13), MPH(6), 10 },  { MPH(24), MPH(14), MPH(9), 5 },   { MPH(27), MPH(14), MPH(7), 8 },
    { MPH(26), MPH(12), MPH(9), 9 },   { MPH(23), MPH(14), MPH(8), 8 },   { MPH(25), MPH(16), MPH(11), 6 },
    { MPH(29), MPH(17), MPH(7), 9 },   { MPH(25), MPH(17), MPH(15), 5 },  { MPH(27), MPH(16), MPH(11), 4 },
    { MPH(13), MPH(8), MPH(10), 10 },  { MPH(27), MPH(17), MPH(8), 7 },   { MPH(22), MPH(13), MPH(6), 7 },
    { MPH(25), MPH(18), MPH(16), 4 },  { MPH(23), MPH(12), MPH(6), 8 },   { MPH(22), MPH(16), MPH(12), 5 },
    { MPH(25), MPH(14), MPH(10), 6 },  { MPH(19), MPH(11), MPH(10), 8 },  { MPH(24), MPH(10), MPH(10), 8 },
    { MPH(27), MPH(17), MPH(12), 7 },  { MPH(27), MPH(16), MPH(12), 5 },  { MPH(24), MPH(16), MPH(8), 7 },
    { MPH(20), MPH(9), MPH(8), 9 },    { MPH(27), MPH(19), MPH(15), 4 },  { MPH(21), MPH(10), MPH(11), 8 },
    { MPH(17), MPH(8), MPH(5), 11 },   { MPH(16), MPH(10), MPH(10), 8 },  { MPH(29), MPH(18), MPH(10), 7 },
    { MPH(23), MPH(16), MPH(14), 5 },  { MPH(16), MPH(8), MPH(6), 10 },   { MPH(24), MPH(10), MPH(8), 11 },
    { MPH(21), MPH(11), MPH(7), 11 },  { MPH(26), MPH(13), MPH(8), 7 },   { MPH(22), MPH(10), MPH(8), 11 },
    { MPH(25), MPH(13), MPH(11), 9 },  { MPH(24), MPH(15), MPH(8), 5 },   { MPH(26), MPH(12), MPH(6), 8 },
    { MPH(21), MPH(15), MPH(15), 7 },  { MPH(28), MPH(19), MPH(14), 6 },  { MPH(23), MPH(14), MPH(9), 8 },
    { MPH(25), MPH(13), MPH(6), 9 },   { MPH(18), MPH(11), MPH(12), 7 },  { MPH(22), MPH(13), MPH(7), 6 },
    { MPH(23), MPH(13), MPH(7), 8 },   { MPH(27), MPH(18), MPH(9), 5 },   { MPH(20), MPH(9), MPH(6), 10 },
    { MPH(29), MPH(9), MPH(9), 12 },   { MPH(26), MPH(12), MPH(9), 11 },  { MPH(27), MPH(10), MPH(5), 11 },
    { MPH(26), MPH(20), MPH(9), 3 },   { MPH(18), MPH(10), MPH(11), 8 },  { MPH(28), MPH(16), MPH(12), 8 },
    { MPH(13), MPH(9), MPH(7), 9 },    { MPH(24), MPH(15), MPH(9), 6 },   { MPH(20), MPH(11), MPH(9), 8 },
    { MPH(24), MPH(15), MPH(12), 4 },  { MPH(24), MPH(14), MPH(9), 5 },   { MPH(22), MPH(11), MPH(8), 10 },
    { MPH(24), MPH(11), MPH(10), 10 }, { MPH(24), MPH(17), MPH(10), 7 },  { MPH(28), MPH(18), MPH(13), 7 },
    { MPH(23), MPH(11), MPH(8), 12 },  { MPH(25), MPH(12), MPH(11), 8 },  { MPH(21), MPH(10), MPH(11), 8 },
    { MPH(15), MPH(8), MPH(7), 10 },   { MPH(26), MPH(16), MPH(9), 8 },   { MPH(21), MPH(10), MPH(9), 10 },
    { MPH(27), MPH(17), MPH(16), 6 },  { MPH(22), MPH(12), MPH(6), 9 },   { MPH(25), MPH(9), MPH(4), 11 },
    { MPH(26), MPH(16), MPH(13), 9 },  { MPH(26), MPH(19), MPH(11), 6 },  { MPH(24), MPH(15), MPH(13), 7 },
    { MPH(16), MPH(9), MPH(10), 8 },   { MPH(21), MPH(11), MPH(7), 6 },   { MPH(28), MPH(20), MPH(15), 6 },
    { MPH(25), MPH(15), MPH(9), 6 }
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

        if (distance < followDistance && z_diff < 16 && relativeVelocity > -MPH(2))
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
        case TrackElemType::LeftQuarterTurn3Tiles:
        case TrackElemType::RightQuarterTurn3Tiles:
            targetSpeed = riderSettings.minSpeed;
            brakeThreshold = riderSettings.brakeThreshold / 2;
            break;
        case TrackElemType::LeftQuarterTurn3TilesUp25:
        case TrackElemType::RightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25:
        case TrackElemType::RightQuarterTurn3TilesDown25:
        case TrackElemType::LeftBankToLeftQuarterTurn3TilesUp25:
        case TrackElemType::RightBankToRightQuarterTurn3TilesUp25:
        case TrackElemType::LeftQuarterTurn3TilesDown25ToLeftBank:
        case TrackElemType::RightQuarterTurn3TilesDown25ToRightBank:
            targetSpeed = (riderSettings.maxSpeed + 3 * riderSettings.minSpeed) / 4;
            brakeThreshold = 3 * riderSettings.brakeThreshold / 4;
            break;
        case TrackElemType::LeftHalfBankedHelixUpSmall:
        case TrackElemType::RightHalfBankedHelixUpSmall:
        case TrackElemType::LeftHalfBankedHelixDownSmall:
        case TrackElemType::RightHalfBankedHelixDownSmall:
        case TrackElemType::LeftBankedQuarterTurn3TileUp25:
        case TrackElemType::RightBankedQuarterTurn3TileUp25:
        case TrackElemType::LeftBankedQuarterTurn3TileDown25:
        case TrackElemType::RightBankedQuarterTurn3TileDown25:
        case TrackElemType::LeftBankedQuarterTurn3Tiles:
        case TrackElemType::RightBankedQuarterTurn3Tiles:
        case TrackElemType::LeftQuarterTurn5Tiles:
        case TrackElemType::RightQuarterTurn5Tiles:
            targetSpeed = (riderSettings.maxSpeed + riderSettings.minSpeed) / 2;
            break;
        case TrackElemType::LeftHalfBankedHelixUpLarge:
        case TrackElemType::RightHalfBankedHelixUpLarge:
        case TrackElemType::LeftHalfBankedHelixDownLarge:
        case TrackElemType::RightHalfBankedHelixDownLarge:
        case TrackElemType::LeftBankedQuarterTurn5TileUp25:
        case TrackElemType::RightBankedQuarterTurn5TileUp25:
        case TrackElemType::LeftBankedQuarterTurn5TileDown25:
        case TrackElemType::RightBankedQuarterTurn5TileDown25:
        case TrackElemType::BankedLeftQuarterTurn5Tiles:
        case TrackElemType::BankedRightQuarterTurn5Tiles:
        case TrackElemType::LeftQuarterTurn5TilesUp25:
        case TrackElemType::RightQuarterTurn5TilesUp25:
        case TrackElemType::LeftQuarterTurn5TilesDown25:
        case TrackElemType::RightQuarterTurn5TilesDown25:
        case TrackElemType::LeftEighthToDiag:
        case TrackElemType::RightEighthToDiag:
        case TrackElemType::LeftEighthToOrthogonal:
        case TrackElemType::RightEighthToOrthogonal:
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
